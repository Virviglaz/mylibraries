/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "portmacro.h"

/* FreeRTOS+FAT includes. */
#include "ff_sddisk.h"
#include "ff_sys.h"

/* STM32F10x Includes */
#include "stm32f10x_sdio.h"
#include "stm32f10x_rcc.h"

/* Misc definitions. */
#define sdSIGNATURE 			0x41404342UL
#define sdHUNDRED_64_BIT		( 100ull )
#define sdBYTES_PER_MB			( 1024ull * 1024ull )
#define sdSECTORS_PER_MB		( sdBYTES_PER_MB / 512ull )
#define sdIOMAN_MEM_SIZE		4096

/* Define a time-out for all DMA transactions in msec. */
#ifndef sdMAX_TIME_TICKS
	#define sdMAX_TIME_TICKS	pdMS_TO_TICKS( 2000UL )
#endif

#ifndef sdCARD_DETECT_DEBOUNCE_TIME_MS
	/* Debouncing time is applied only after card gets inserted. */
	#define sdCARD_DETECT_DEBOUNCE_TIME_MS	( 5000 )
#endif

#ifndef sdARRAY_SIZE
	#define	sdARRAY_SIZE( x )	( int )( sizeof( x ) / sizeof( x )[ 0 ] )
#endif

/*-----------------------------------------------------------*/

/*
 * Return pdFALSE if the SD card is not inserted.  This function just reads the
 * value of the GPIO C/D pin.
 */
static BaseType_t prvSDDetect( void );

/*
 * Translate a numeric code like 'SD_TX_UNDERRUN' to a printable string.
 */
static const char *prvSDCodePrintable( uint32_t ulCode );

/*
 * Hardware initialisation.
 */
static void prvSDIO_SD_Init( void );
static void vGPIO_SD_Init( SD_HandleTypeDef* xSDHandle );

/*
 * Check if the card is present, and if so, print out some info on the card.
 */
static BaseType_t prvSDMMCInit( BaseType_t xDriveNumber );

typedef struct
{
	/* Only after a card has been inserted, debouncing is necessary. */
	TickType_t xRemainingTime;
	TimeOut_t xTimeOut;
	UBaseType_t
		bLastPresent : 1,
		bStableSignal : 1;
} CardDetect_t;

/* Used to handle timeouts. */
static TickType_t xDMARemainingTime;
static TimeOut_t xDMATimeOut;

/* Used to unblock the task that calls prvEventWaitFunction() after an event has
occurred. */
static SemaphoreHandle_t xSDCardSemaphore = NULL;

/* Handle of the SD card being used. */
static SD_HandleTypeDef xSDHandle;

/* Holds parameters for the detected SD card. */
static HAL_SD_CardInfoTypedef xSDCardInfo;

/* Mutex for partition. */
static SemaphoreHandle_t xPlusFATMutex = NULL;

/* Remembers if the card is currently considered to be present. */
static BaseType_t xSDCardStatus = pdFALSE;

/* Maintains state for card detection. */
static CardDetect_t xCardDetect;

/*-----------------------------------------------------------*/

static int32_t prvFFRead( uint8_t *pucBuffer, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk )
{
int32_t lReturnCode = FF_ERR_IOMAN_OUT_OF_BOUNDS_READ | FF_ERRFLAG;

	if( ( pxDisk != NULL ) &&
		( xSDCardStatus == pdPASS ) &&
		( pxDisk->ulSignature == sdSIGNATURE ) &&
		( pxDisk->xStatus.bIsInitialised != pdFALSE ) &&
		( ulSectorNumber < pxDisk->ulNumberOfSectors ) &&
		( ( pxDisk->ulNumberOfSectors - ulSectorNumber ) >= ulSectorCount ) )
	{
	uint64_t ullReadAddr;
	HAL_SD_ErrorTypedef sd_result;

		ullReadAddr = 512ull * ( uint64_t ) ulSectorNumber;
		
		if( ( ( ( size_t )pucBuffer ) & ( sizeof( size_t ) - 1 ) ) == 0 )
		{
			/* The buffer is word-aligned, call DMA read directly. */
			sd_result = HAL_SD_ReadBlocks_DMA( &xSDHandle, (uint32_t *) pucBuffer, ullReadAddr, 512ul, ulSectorCount);
			if( sd_result == SD_OK )
			{
				sd_result = HAL_SD_CheckReadOperation( &xSDHandle, sdMAX_TIME_TICKS );
			}
		}
		else
		{
		uint32_t ulSector;
		uint8_t *pucDMABuffer = ffconfigMALLOC( 512ul );

			/* The buffer is NOT word-aligned, copy first to an aligned buffer. */
			if( pucDMABuffer != NULL )
			{
				sd_result = SD_OK;
				for( ulSector = 0; ulSector < ulSectorCount; ulSector++ )
				{
					ullReadAddr = 512ull * ( ( uint64_t ) ulSectorNumber + ( uint64_t ) ulSector );
					sd_result = HAL_SD_ReadBlocks_DMA( &xSDHandle, ( uint32_t * )pucDMABuffer, ullReadAddr, 512ul, 1 );

					if( sd_result == SD_OK )
					{
						sd_result = HAL_SD_CheckReadOperation( &xSDHandle, sdMAX_TIME_TICKS );
						if( sd_result != SD_OK )
						{
							break;
						}
						memcpy( pucBuffer + 512ul * ulSector, pucDMABuffer, 512ul );
					}
				}
				ffconfigFREE( pucDMABuffer );
			}
			else
			{
				sd_result = SD_INVALID_PARAMETER;
			}
		}
		
		if( sd_result == SD_OK )
		{
			lReturnCode = 0L;
		}
		else
		{
			/* Some error occurred. */
			FF_PRINTF( "prvFFRead: %lu: %lu (%s)\n", ulSectorNumber, sd_result, prvSDCodePrintable( sd_result ) );
		}
	}
	else
	{
		/* Make sure no random data is in the returned buffer. */
		memset( ( void * ) pucBuffer, '\0', ulSectorCount * 512UL );

		if( pxDisk->xStatus.bIsInitialised != pdFALSE )
		{
			FF_PRINTF( "prvFFRead: warning: %lu + %lu > %lu\n", ulSectorNumber, ulSectorCount, pxDisk->ulNumberOfSectors );
		}
	}

	return lReturnCode;
}
/*-----------------------------------------------------------*/

static int32_t prvFFWrite( uint8_t *pucBuffer, uint32_t ulSectorNumber, uint32_t ulSectorCount, FF_Disk_t *pxDisk )
{
int32_t lReturnCode = FF_ERR_IOMAN_OUT_OF_BOUNDS_READ | FF_ERRFLAG;

	if( ( pxDisk != NULL ) &&
		( xSDCardStatus == pdPASS ) &&
		( pxDisk->ulSignature == sdSIGNATURE ) &&
		( pxDisk->xStatus.bIsInitialised != pdFALSE ) &&
		( ulSectorNumber < pxDisk->ulNumberOfSectors ) &&
		( ( pxDisk->ulNumberOfSectors - ulSectorNumber ) >= ulSectorCount ) )
	{
		HAL_SD_ErrorTypedef sd_result;
		uint64_t ullWriteAddr;
		ullWriteAddr = 512ull * ulSectorNumber;

		if( ( ( ( size_t )pucBuffer ) & ( sizeof( size_t ) - 1 ) ) == 0 )
		{
			/* The buffer is word-aligned, call DMA reawrite directly. */
			sd_result = HAL_SD_WriteBlocks_DMA( &xSDHandle, ( uint32_t * )pucBuffer, ullWriteAddr, 512ul, ulSectorCount );
			if( sd_result == SD_OK )
			{
				sd_result = HAL_SD_CheckWriteOperation( &xSDHandle, sdMAX_TIME_TICKS );
			}
		}
		else
		{
		uint32_t ulSector;
		uint8_t *pucDMABuffer = ffconfigMALLOC( 512ul );

			/* The buffer is NOT word-aligned, read to an aligned buffer and then
			copy the data to the user provided buffer. */
			if( pucDMABuffer != NULL )
			{
				sd_result = SD_OK;
				for( ulSector = 0; ulSector < ulSectorCount; ulSector++ )
				{
					memcpy( pucDMABuffer, pucBuffer + 512ul * ulSector, 512ul );
					ullWriteAddr = 512ull * ( ulSectorNumber + ulSector );
					sd_result = HAL_SD_WriteBlocks_DMA( &xSDHandle, ( uint32_t * )pucDMABuffer, ullWriteAddr, 512ul, 1 );
					if( sd_result == SD_OK )
					{
						sd_result = HAL_SD_CheckWriteOperation( &xSDHandle, sdMAX_TIME_TICKS );
						if( sd_result != SD_OK )
						{
							break;
						}
					}
				}
				ffconfigFREE( pucDMABuffer );
			}
			else
			{
				sd_result = SD_INVALID_PARAMETER;
			}
		}

		if( sd_result == SD_OK )
		{
			/* No errors. */
			lReturnCode = 0L;
		}
		else
		{
			FF_PRINTF( "prvFFWrite: %lu: %lu (%s)\n", ulSectorNumber, sd_result, prvSDCodePrintable( sd_result ) );
		}
	}
	else
	{
		if( pxDisk->xStatus.bIsInitialised != pdFALSE )
		{
			FF_PRINTF( "prvFFWrite: warning: %lu + %lu > %lu\n", ulSectorNumber, ulSectorCount, pxDisk->ulNumberOfSectors );
		}
	}

	return lReturnCode;
}
/*-----------------------------------------------------------*/

void FF_SDDiskFlush( FF_Disk_t *pxDisk )
{
	if( ( pxDisk != NULL ) &&
		( pxDisk->xStatus.bIsInitialised != pdFALSE ) &&
		( pxDisk->pxIOManager != NULL ) )
	{
		FF_FlushCache( pxDisk->pxIOManager );
	}
}
/*-----------------------------------------------------------*/

static void vGPIO_SD_Init(SD_HandleTypeDef* xSDHandle)
{
  /*!< GPIOC and GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | SD_DETECT_GPIO_CLK, ENABLE);
	
  /*!< Configure PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK pin */
	GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
  /*!< Configure PD.02 CMD line */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /*!< Configure SD_CD pin: SD Card detect pin */
  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);
  
  /*!< Enable the SDIO AHB Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO, ENABLE);
	
  /*!< Configure the SDIO peripheral */
  /*!< SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_TRANSFER_CLK_DIV) */
	SDIO_InitTypeDef SDIO_InitStructure;
	SDIO_InitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV; 
	SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
	SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
	SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
	SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_4b;
	SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
	SDIO_Init(&SDIO_InitStructure);
}
/*-----------------------------------------------------------*/

FF_Disk_t *FF_SDDiskInit( const char *pcName )
{
FF_Error_t xFFError;
BaseType_t xPartitionNumber = 0;
FF_CreationParameters_t xParameters;
FF_Disk_t *pxDisk;

	xSDCardStatus = prvSDMMCInit( 0 );

	if( xSDCardStatus != pdPASS )
	{
		FF_PRINTF( "FF_SDDiskInit: prvSDMMCInit failed\n" );
		pxDisk = NULL;
	}
	else
	{
		pxDisk = (FF_Disk_t *)ffconfigMALLOC( sizeof( *pxDisk ) );
		if( pxDisk == NULL )
		{
			FF_PRINTF( "FF_SDDiskInit: Malloc failed\n" );
		}
		else
		{
			/* Initialise the created disk structure. */
			memset( pxDisk, '\0', sizeof( *pxDisk ) );

			pxDisk->ulNumberOfSectors = xSDCardInfo.CardCapacity / 512;

			if( xPlusFATMutex == NULL )
			{
				xPlusFATMutex = xSemaphoreCreateRecursiveMutex();
			}
			pxDisk->ulSignature = sdSIGNATURE;

			if( xPlusFATMutex != NULL)
			{
				memset( &xParameters, '\0', sizeof( xParameters ) );
				xParameters.ulMemorySize = sdIOMAN_MEM_SIZE;
				xParameters.ulSectorSize = 512;
				xParameters.fnWriteBlocks = prvFFWrite;
				xParameters.fnReadBlocks = prvFFRead;
				xParameters.pxDisk = pxDisk;

				/* prvFFRead()/prvFFWrite() are not re-entrant and must be
				protected with the use of a semaphore. */
				xParameters.xBlockDeviceIsReentrant = pdFALSE;

				/* The semaphore will be used to protect critical sections in
				the +FAT driver, and also to avoid concurrent calls to
				prvFFRead()/prvFFWrite() from different tasks. */
				xParameters.pvSemaphore = ( void * ) xPlusFATMutex;

				pxDisk->pxIOManager = FF_CreateIOManger( &xParameters, &xFFError );

				if( pxDisk->pxIOManager == NULL )
				{
					FF_PRINTF( "FF_SDDiskInit: FF_CreateIOManger: %s\n", (const char*)FF_GetErrMessage( xFFError ) );
					FF_SDDiskDelete( pxDisk );
					pxDisk = NULL;
				}
				else
				{
					pxDisk->xStatus.bIsInitialised = pdTRUE;
					pxDisk->xStatus.bPartitionNumber = xPartitionNumber;
					if( FF_SDDiskMount( pxDisk ) == 0 )
					{
						FF_SDDiskDelete( pxDisk );
						pxDisk = NULL;
					}
					else
					{
						if( pcName == NULL )
						{
							pcName = "/";
						}
						FF_FS_Add( pcName, pxDisk );
						FF_PRINTF( "FF_SDDiskInit: Mounted SD-card as root \"%s\"\n", pcName );
						FF_SDDiskShowPartition( pxDisk );
					}
				}	/* if( pxDisk->pxIOManager != NULL ) */
			}	/* if( xPlusFATMutex != NULL) */
		}	/* if( pxDisk != NULL ) */
	}	/* if( xSDCardStatus == pdPASS ) */

	return pxDisk;
}
/*-----------------------------------------------------------*/

BaseType_t FF_SDDiskFormat( FF_Disk_t *pxDisk, BaseType_t xPartitionNumber )
{
FF_Error_t xError;
BaseType_t xReturn = pdFAIL;

	xError = FF_Unmount( pxDisk );

	if( FF_isERR( xError ) != pdFALSE )
	{
		FF_PRINTF( "FF_SDDiskFormat: unmount fails: %08x\n", ( unsigned ) xError );
	}
	else
	{
		/* Format the drive - try FAT32 with large clusters. */
		xError = FF_Format( pxDisk, xPartitionNumber, pdFALSE, pdFALSE);

		if( FF_isERR( xError ) )
		{
			FF_PRINTF( "FF_SDDiskFormat: %s\n", (const char*)FF_GetErrMessage( xError ) );
		}
		else
		{
			FF_PRINTF( "FF_SDDiskFormat: OK, now remounting\n" );
			pxDisk->xStatus.bPartitionNumber = xPartitionNumber;
			xError = FF_SDDiskMount( pxDisk );
			FF_PRINTF( "FF_SDDiskFormat: rc %08x\n", ( unsigned )xError );
			if( FF_isERR( xError ) == pdFALSE )
			{
				xReturn = pdPASS;
				FF_SDDiskShowPartition( pxDisk );
			}
		}
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t FF_SDDiskUnmount( FF_Disk_t *pxDisk )
{
FF_Error_t xFFError;
BaseType_t xReturn = pdPASS;

	if( ( pxDisk != NULL ) && ( pxDisk->xStatus.bIsMounted != pdFALSE ) )
	{
		pxDisk->xStatus.bIsMounted = pdFALSE;
		xFFError = FF_Unmount( pxDisk );

		if( FF_isERR( xFFError ) )
		{
			FF_PRINTF( "FF_SDDiskUnmount: rc %08x\n", ( unsigned )xFFError );
			xReturn = pdFAIL;
		}
		else
		{
			FF_PRINTF( "Drive unmounted\n" );
		}
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t FF_SDDiskReinit( FF_Disk_t *pxDisk )
{
BaseType_t xStatus = prvSDMMCInit( 0 ); /* Hard coded index. */

	/*_RB_ parameter not used. */
	( void ) pxDisk;

	FF_PRINTF( "FF_SDDiskReinit: rc %08x\n", ( unsigned ) xStatus );
	return xStatus;
}
/*-----------------------------------------------------------*/

BaseType_t FF_SDDiskMount( FF_Disk_t *pxDisk )
{
FF_Error_t xFFError;
BaseType_t xReturn;

	/* Mount the partition */
	xFFError = FF_Mount( pxDisk, pxDisk->xStatus.bPartitionNumber );

	if( FF_isERR( xFFError ) )
	{
		FF_PRINTF( "FF_SDDiskMount: %08lX\n", xFFError );
		xReturn = pdFAIL;
	}
	else
	{
		pxDisk->xStatus.bIsMounted = pdTRUE;
		FF_PRINTF( "****** FreeRTOS+FAT initialized %lu sectors\n", pxDisk->pxIOManager->xPartition.ulTotalSectors );
		xReturn = pdPASS;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

FF_IOManager_t *sddisk_ioman( FF_Disk_t *pxDisk )
{
FF_IOManager_t *pxReturn;

	if( ( pxDisk != NULL ) && ( pxDisk->xStatus.bIsInitialised != pdFALSE ) )
	{
		pxReturn = pxDisk->pxIOManager;
	}
	else
	{
		pxReturn = NULL;
	}
	return pxReturn;
}
/*-----------------------------------------------------------*/

/* Release all resources */
BaseType_t FF_SDDiskDelete( FF_Disk_t *pxDisk )
{
	if( pxDisk != NULL )
	{
		pxDisk->ulSignature = 0;
		pxDisk->xStatus.bIsInitialised = 0;
		if( pxDisk->pxIOManager != NULL )
		{
			if( FF_Mounted( pxDisk->pxIOManager ) != pdFALSE )
			{
				FF_Unmount( pxDisk );
			}
			FF_DeleteIOManager( pxDisk->pxIOManager );
		}

		vPortFree( pxDisk );
	}
	return 1;
}
/*-----------------------------------------------------------*/

BaseType_t FF_SDDiskShowPartition( FF_Disk_t *pxDisk )
{
FF_Error_t xError;
uint64_t ullFreeSectors;
uint32_t ulTotalSizeMB, ulFreeSizeMB;
int iPercentageFree;
FF_IOManager_t *pxIOManager;
const char *pcTypeName = "unknown type";
BaseType_t xReturn = pdPASS;

	if( pxDisk == NULL )
	{
		xReturn = pdFAIL;
	}
	else
	{
		pxIOManager = pxDisk->pxIOManager;

		FF_PRINTF( "Reading FAT and calculating Free Space\n" );

		switch( pxIOManager->xPartition.ucType )
		{
			case FF_T_FAT12:
				pcTypeName = "FAT12";
				break;

			case FF_T_FAT16:
				pcTypeName = "FAT16";
				break;

			case FF_T_FAT32:
				pcTypeName = "FAT32";
				break;

			default:
				pcTypeName = "UNKOWN";
				break;
		}

		FF_GetFreeSize( pxIOManager, &xError );

		ullFreeSectors = pxIOManager->xPartition.ulFreeClusterCount * pxIOManager->xPartition.ulSectorsPerCluster;
		iPercentageFree = ( int ) ( ( sdHUNDRED_64_BIT * ullFreeSectors + pxIOManager->xPartition.ulDataSectors / 2 ) /
			( ( uint64_t )pxIOManager->xPartition.ulDataSectors ) );

		ulTotalSizeMB = pxIOManager->xPartition.ulDataSectors / sdSECTORS_PER_MB;
		ulFreeSizeMB = ( uint32_t ) ( ullFreeSectors / sdSECTORS_PER_MB );

		/* It is better not to use the 64-bit format such as %Lu because it
		might not be implemented. */
		FF_PRINTF( "Partition Nr   %8u\n", pxDisk->xStatus.bPartitionNumber );
		FF_PRINTF( "Type           %8u (%s)\n", pxIOManager->xPartition.ucType, pcTypeName );
		FF_PRINTF( "VolLabel       '%8s' \n", pxIOManager->xPartition.pcVolumeLabel );
		FF_PRINTF( "TotalSectors   %8lu\n", pxIOManager->xPartition.ulTotalSectors );
		FF_PRINTF( "SecsPerCluster %8lu\n", pxIOManager->xPartition.ulSectorsPerCluster );
		FF_PRINTF( "Size           %8lu MB\n", ulTotalSizeMB );
		FF_PRINTF( "FreeSize       %8lu MB ( %d perc free )\n", ulFreeSizeMB, iPercentageFree );
	}

	return xReturn;
}
/*-----------------------------------------------------------*/


/* This routine returns true if the SD-card is inserted.  After insertion, it
will wait for sdCARD_DETECT_DEBOUNCE_TIME_MS before returning pdTRUE. */
BaseType_t FF_SDDiskDetect( FF_Disk_t *pxDisk )
{
int xReturn;

	xReturn = prvSDDetect();

	if( xReturn != pdFALSE )
	{
		if( xCardDetect.bStableSignal == pdFALSE )
		{
			/* The card seems to be present. */
			if( xCardDetect.bLastPresent == pdFALSE )
			{
				xCardDetect.bLastPresent = pdTRUE;
				xCardDetect.xRemainingTime = pdMS_TO_TICKS( ( TickType_t ) sdCARD_DETECT_DEBOUNCE_TIME_MS );
				/* Fetch the current time. */
				vTaskSetTimeOutState( &xCardDetect.xTimeOut );
			}
			/* Has the timeout been reached? */
			if( xTaskCheckForTimeOut( &xCardDetect.xTimeOut, &xCardDetect.xRemainingTime ) != pdFALSE )
			{
				xCardDetect.bStableSignal = pdTRUE;
			}
			else
			{
				/* keep returning false until de time-out is reached. */
				xReturn = pdFALSE;
			}
		}
	}
	else
	{
		xCardDetect.bLastPresent = pdFALSE;
		xCardDetect.bStableSignal = pdFALSE;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

/* Raw SD-card detection, just return the GPIO status. */
static BaseType_t prvSDDetect( void )
{
int iReturn;

	/*!< Check GPIO to detect SD */
	if( HAL_GPIO_ReadPin( configSD_DETECT_GPIO_PORT, configSD_DETECT_PIN ) != 0 )
	{
		/* The internal pull-up makes the signal high. */
		iReturn = pdFALSE;
	}
	else
	{
		/* The card will pull the GPIO signal down. */
		iReturn = pdTRUE;
	}

	return iReturn;
}
/*-----------------------------------------------------------*/

static BaseType_t prvSDMMCInit( BaseType_t xDriveNumber )
{
	/* 'xDriveNumber' not yet in use. */
	( void )xDriveNumber;

	if( xSDCardSemaphore == NULL )
	{
		xSDCardSemaphore = xSemaphoreCreateBinary();
	}
	prvSDIO_SD_Init();

	vGPIO_SD_Init( &xSDHandle );

	int SD_state = SD_OK;
	/* Check if the SD card is plugged in the slot */
	if( prvSDDetect() == pdFALSE )
	{
		FF_PRINTF( "No SD card detected\n" );
		return 0;
	}
	/* When starting up, skip debouncing of the Card Detect signal. */
	xCardDetect.bLastPresent = pdTRUE;
	xCardDetect.bStableSignal = pdTRUE;
	/* Initialise the SDIO device and read the card parameters. */
	SD_state = HAL_SD_Init( &xSDHandle, &xSDCardInfo );
	#if( BUS_4BITS != 0 )
    {
		if( SD_state == SD_OK )
		{
			HAL_SD_ErrorTypedef rc;

			xSDHandle.Init.BusWide = SDIO_BUS_WIDE_4B;
			rc = HAL_SD_WideBusOperation_Config(&xSDHandle, SDIO_BUS_WIDE_4B);
			if( rc != SD_OK )
			{
				FF_PRINTF( "HAL_SD_WideBus: %d: %s\n", rc, prvSDCodePrintable( ( uint32_t )rc ) );
			}
		}
    }
	#endif
	FF_PRINTF( "HAL_SD_Init: %d: %s type: %s Capacity: %lu MB\n",
		SD_state, prvSDCodePrintable( ( uint32_t )SD_state ),
		xSDHandle.CardType == HIGH_CAPACITY_SD_CARD ? "SDHC" : "SD",
		xSDCardInfo.CardCapacity / ( 1024 * 1024 ) );

	return SD_state == SD_OK ? 1 : 0;
}
/*-----------------------------------------------------------*/

struct xCODE_NAME
{
	uint32_t ulValue;
	const char *pcName;
};

const struct xCODE_NAME xSD_CODES[] =
{
	{ SD_CMD_CRC_FAIL,          "CMD_CRC_FAIL: Command response received (but CRC check failed)" },
	{ SD_DATA_CRC_FAIL,         "DATA_CRC_FAIL: Data block sent/received (CRC check failed)" },
	{ SD_CMD_RSP_TIMEOUT,       "CMD_RSP_TIMEOUT: Command response timeout" },
	{ SD_DATA_TIMEOUT,          "DATA_TIMEOUT: Data timeout" },
	{ SD_TX_UNDERRUN,           "TX_UNDERRUN: Transmit FIFO underrun" },
	{ SD_RX_OVERRUN,            "RX_OVERRUN: Receive FIFO overrun" },
	{ SD_START_BIT_ERR,         "START_BIT_ERR: Start bit not detected on all data signals in wide bus mode" },
	{ SD_CMD_OUT_OF_RANGE,      "CMD_OUT_OF_RANGE: Command's argument was out of range" },
	{ SD_ADDR_MISALIGNED,       "ADDR_MISALIGNED: Misaligned address" },
	{ SD_BLOCK_LEN_ERR,         "BLOCK_LEN_ERR: Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length" },
	{ SD_ERASE_SEQ_ERR,         "ERASE_SEQ_ERR: An error in the sequence of erase command occurs." },
	{ SD_BAD_ERASE_PARAM,       "BAD_ERASE_PARAM: An invalid selection for erase groups" },
	{ SD_WRITE_PROT_VIOLATION,  "WRITE_PROT_VIOLATION: Attempt to program a write protect block" },
	{ SD_LOCK_UNLOCK_FAILED,    "LOCK_UNLOCK_FAILED: Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card" },
	{ SD_COM_CRC_FAILED,        "COM_CRC_FAILED: CRC check of the previous command failed" },
	{ SD_ILLEGAL_CMD,           "ILLEGAL_CMD: Command is not legal for the card state" },
	{ SD_CARD_ECC_FAILED,       "CARD_ECC_FAILED: Card internal ECC was applied but failed to correct the data" },
	{ SD_CC_ERROR,              "CC_ERROR: Internal card controller error" },
	{ SD_GENERAL_UNKNOWN_ERROR, "GENERAL_UNKNOWN_ERROR: General or unknown error" },
	{ SD_STREAM_READ_UNDERRUN,  "STREAM_READ_UNDERRUN: The card could not sustain data transfer in stream read operation" },
	{ SD_STREAM_WRITE_OVERRUN,  "STREAM_WRITE_OVERRUN: The card could not sustain data programming in stream mode" },
	{ SD_CID_CSD_OVERWRITE,     "CID_CSD_OVERWRITE: CID/CSD overwrite error" },
	{ SD_WP_ERASE_SKIP,         "WP_ERASE_SKIP: Only partial address space was erased" },
	{ SD_CARD_ECC_DISABLED,     "CARD_ECC_DISABLED: Command has been executed without using internal ECC" },
	{ SD_ERASE_RESET,           "ERASE_RESET: Erase sequence was cleared before executing because an out of erase sequence command was received" },
	{ SD_AKE_SEQ_ERROR,         "AKE_SEQ_ERROR: Error in sequence of authentication" },
	{ SD_INVALID_VOLTRANGE,     "INVALID_VOLTRANGE" },
	{ SD_ADDR_OUT_OF_RANGE,     "ADDR_OUT_OF_RANGE" },
	{ SD_SWITCH_ERROR,          "SWITCH_ERROR" },
	{ SD_SDIO_DISABLED,         "SDIO_DISABLED" },
	{ SD_SDIO_FUNCTION_BUSY,    "SDIO_FUNCTION_BUSY" },
	{ SD_SDIO_FUNCTION_FAILED,  "SDIO_FUNCTION_FAILED" },
	{ SD_SDIO_UNKNOWN_FUNCTION, "SDIO_UNKNOWN_FUNCTION" },

	/**
	* @brief  Standard error defines
	*/
	{ SD_INTERNAL_ERROR,        "INTERNAL_ERROR" },
	{ SD_NOT_CONFIGURED,        "NOT_CONFIGURED" },
	{ SD_REQUEST_PENDING,       "REQUEST_PENDING" },
	{ SD_REQUEST_NOT_APPLICABLE,"REQUEST_NOT_APPLICABLE" },
	{ SD_INVALID_PARAMETER,     "INVALID_PARAMETER" },
	{ SD_UNSUPPORTED_FEATURE,   "UNSUPPORTED_FEATURE" },
	{ SD_UNSUPPORTED_HW,        "UNSUPPORTED_HW" },
	{ SD_ERROR,                 "ERROR" },
	{ SD_OK,                    "OK" },
};
/*-----------------------------------------------------------*/

static const char *prvSDCodePrintable( uint32_t ulCode )
{
static char retString[32];
const struct xCODE_NAME *pxCode;

	for( pxCode = xSD_CODES; pxCode <= xSD_CODES + sdARRAY_SIZE( xSD_CODES ) - 1; pxCode++ )
	{
		if( pxCode->ulValue == ulCode )
		{
			return pxCode->pcName;
		}
	}
	snprintf( retString, sizeof( retString ), "SD code %lu\n", ulCode );
	return retString;
}
