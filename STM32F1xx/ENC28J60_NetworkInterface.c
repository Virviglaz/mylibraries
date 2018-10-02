/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"

#include "ENC28J60.h"
#include <stdbool.h>

#define MUTEX_TAKE while ( xSemaphoreTake( enc28j60_mutex, portMAX_DELAY ) == pdFALSE )
#define MUTEX_GIVE xSemaphoreGive( enc28j60_mutex )

/* Local driver instance to ENC28J60 chip */
extern enc28j60_t ENC28J60;
SemaphoreHandle_t enc28j60_mutex;

/* Local functions */
static void nicRxTask( void *pvParameters );
static void locked_rxProcess (void);
static void processPacket(xNetworkBufferDescriptor_t *pxBufferDescriptor);

BaseType_t xNetworkInterfaceInitialise(void) 
{
	enc28j60_mutex = xSemaphoreCreateMutex();
	ENC28J60.mac_address = (uint8_t*)FreeRTOS_GetMACAddress();
	
	if (enc28j60_Init(&ENC28J60))
		return xTaskCreate(nicRxTask, "EthRxTask", configMINIMAL_STACK_SIZE, NULL, ipconfigIP_TASK_PRIORITY, NULL);
	
	return pdFAIL;
}

static void nicRxTask( void *pvParameters )
{
	while(1)
	{
		uint8_t rx_pkt_num = enc28j60_numPacketsRx();
		while(rx_pkt_num--)
			locked_rxProcess();
	#ifndef ETH_READ_INT
		#define ETH_READ_INT 10
	#endif
		vTaskDelay(ETH_READ_INT);
	}
}

static void locked_rxProcess (void)
{
	MUTEX_TAKE;
	size_t xBytesReceived = enc28j60_packetReceive(NULL, 0);
	MUTEX_GIVE;

	if (xBytesReceived == 0 || xBytesReceived > ipconfigNETWORK_MTU)
		return;
	
	xNetworkBufferDescriptor_t *pxBufferDescriptor;
	uint8_t atmp_cnt = 100;
	
	do
	{
		pxBufferDescriptor = pxGetNetworkBufferWithDescriptor(xBytesReceived, 0);
		taskYIELD();
	}while(pxBufferDescriptor == NULL && --atmp_cnt);
	
	if (atmp_cnt == 0) 
	{
		iptraceETHERNET_RX_EVENT_LOST();
		return;
	}
	
	pxBufferDescriptor->xDataLength = xBytesReceived;
	MUTEX_TAKE;
	xBytesReceived = enc28j60_packetReceive(pxBufferDescriptor->pucEthernetBuffer, pxBufferDescriptor->xDataLength);
	MUTEX_GIVE;
	processPacket(pxBufferDescriptor);
}


static void processPacket(xNetworkBufferDescriptor_t *pxBufferDescriptor)
{
	if (eConsiderFrameForProcessing(pxBufferDescriptor->pucEthernetBuffer) == eProcessBuffer)
	{
		xIPStackEvent_t xRxEvent = { eNetworkRxEvent, pxBufferDescriptor };
		
		if (xSendEventStructToIPTask((void*)&xRxEvent, 0) == pdFAIL)
		{
			/* The buffer could not be sent to the IP task so the buffer
			must be released. */
			vReleaseNetworkBufferAndDescriptor(pxBufferDescriptor);

			/* Make a call to the standard trace macro to log the
			occurrence. */
			iptraceETHERNET_RX_EVENT_LOST();
		}
		else
		{
			/* The message was successfully sent to the TCP/IP stack.
			Call the standard trace macro to log the occurrence. */
			iptraceNETWORK_INTERFACE_RECEIVE();
		}
		return;		
	}
}

BaseType_t xNetworkInterfaceOutput(xNetworkBufferDescriptor_t * const pxDescriptor, BaseType_t xReleaseAfterSend)
{
	MUTEX_TAKE;
	enc28j60_packetSend(pxDescriptor->pucEthernetBuffer, pxDescriptor->xDataLength);
	MUTEX_GIVE;
	
	if (xReleaseAfterSend)
		vReleaseNetworkBufferAndDescriptor(pxDescriptor);	
	
	/* Call the standard trace macro to log the send event. */
	iptraceNETWORK_INTERFACE_TRANSMIT();

	return pdTRUE;
}

