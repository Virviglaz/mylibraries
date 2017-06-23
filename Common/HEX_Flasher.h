#ifndef HEX_Flasher_H
#define HEX_Flasher_H

#include <stdint.h>

typedef enum
{
	DataToWriteCMD = 0,
	LastStringCMD  = 1,
	DefineStartCMD = 2,
	DefineInSegCMD = 4
}HEXcmdTypeDef;

typedef enum
{
	HEX_OK = 0,
	HEX_WrongFormat,
	HEX_WrongCRC,
	HEX_OutOfLimits,
	HEX_Finished_OK,
}HEX_ErrorTypeDef;

HEX_ErrorTypeDef FlashHexString (uint8_t * buf, uint32_t * curr_segm,
														void (* FlashHandler)(uint32_t Address, uint32_t Data), 
															uint32_t MinAdrs, uint32_t MaxAdrs);

#endif
