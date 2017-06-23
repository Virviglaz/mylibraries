#include "HEX_Flasher.h"

//HEX FORMAT
// : NN AAAA CC DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD SS
// NN - amount of bytes in string
// AAAA - address offset
// CC - command (HEXcmdTypeDef)
// D...D - data
// SS - CRC (should be equal to 0)

//20 bytes short format compatible with BLE
HEX_ErrorTypeDef FlashHexString (uint8_t * buf, uint32_t * curr_segm,
														void (* FlashHandler)(uint32_t Address, uint32_t Data), uint32_t MinAdrs, uint32_t MaxAdrs)
{
	HEXcmdTypeDef CMD;
	HEX_ErrorTypeDef Result = HEX_OK;	
	union {uint32_t V32; uint8_t V8[4];} MemOffset, DataToWrite;
	uint8_t BytesToWrite = buf[0];
	uint8_t cnt;
	
	if (BytesToWrite > 16) return HEX_WrongFormat;
	
	CMD = (HEXcmdTypeDef) buf[3];
	if (CMD > 4) return HEX_WrongFormat;

	/* Get latest used segment */
	MemOffset.V32 = * curr_segm;
	
	switch (CMD)
	{
		case DataToWriteCMD: MemOffset.V8[1] = buf[1]; MemOffset.V8[0] = buf[2]; break;
		case LastStringCMD : Result = HEX_Finished_OK; break;
		case DefineInSegCMD: MemOffset.V8[3] = buf[4]; MemOffset.V8[2] = buf[5]; break;
		default: Result = HEX_WrongFormat; break;
	}
	
	* curr_segm = MemOffset.V32;
	
	if (CMD != DataToWriteCMD) return HEX_OK;
	
	for (cnt = 0; cnt < BytesToWrite; cnt += 4)
	{
		DataToWrite.V8[3] = cnt + 4;
		DataToWrite.V8[2] = cnt + 5;
		DataToWrite.V8[1] = cnt + 6;
		DataToWrite.V8[0] = cnt + 7;
		if (MemOffset.V32 < MinAdrs || MemOffset.V32 > MaxAdrs) return HEX_OutOfLimits;
		FlashHandler (MemOffset.V32, DataToWrite.V32);
	}
	
	return Result;
}
