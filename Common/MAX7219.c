#include "MAX7219.h"

#define CHIP_ENABLE  0
#define CHIP_DISABLE 1

/* Private functions  declaration */
uint8_t GetDeviceN (MAX7219_InitStructTypeDef * MAX7219_InitStruct);
void WriteReg (MAX7219_InitStructTypeDef * MAX7219_InitStruct, uint8_t Reg, uint8_t Value);
uint8_t reverse(uint8_t b);

/* Public functions */
uint8_t MAX7219_Init (MAX7219_InitStructTypeDef * MAX7219_InitStruct)
{        
  if (MAX7219_InitStruct->DeviceN == 0) //autodetect amount of devices on bus if not defined
    MAX7219_InitStruct->DeviceN = GetDeviceN(MAX7219_InitStruct);
	
  WriteReg(MAX7219_InitStruct, MAX7219_REG_DecodeMode, (uint8_t)MAX7219_InitStruct->DecodeMode);
  WriteReg(MAX7219_InitStruct, MAX7219_REG_Intensity, (uint8_t)MAX7219_InitStruct->Intensity);
  WriteReg(MAX7219_InitStruct, MAX7219_REG_ScanLim, (uint8_t)MAX7219_InitStruct->ScanLimit);
  WriteReg(MAX7219_InitStruct, MAX7219_REG_TestMode, (uint8_t)MAX7219_InitStruct->TestMode);
  WriteReg(MAX7219_InitStruct, MAX7219_REG_ShutDown, (uint8_t)MAX7219_InitStruct->Power);
  
   return 0;
}

void MAX7219_Print (MAX7219_InitStructTypeDef * MAX7219_InitStruct, uint8_t * buf, uint8_t * font)
{
  uint8_t digcnt = 0;
  while (* buf)
    MAX7219_UpdateDigit(MAX7219_InitStruct, digcnt++, font + MAX7219_InitStruct->DigitsPerChip * (*buf++));
}

void MAX7219_UpdateDigit (MAX7219_InitStructTypeDef * MAX7219_InitStruct, uint8_t DigNum, uint8_t * buf)
{
  uint8_t devcnt, digcnt;

  for (digcnt = 0; digcnt != MAX7219_InitStruct->DigitsPerChip; digcnt++)
  {
    MAX7219_InitStruct->ChipEnable(CHIP_ENABLE);
    for (devcnt = 0; devcnt != MAX7219_InitStruct->DeviceN; devcnt++)
    {
      if (devcnt == DigNum)
      {
        MAX7219_InitStruct->SendReceive(digcnt + 1);
        MAX7219_InitStruct->xMirror ? MAX7219_InitStruct->SendReceive(reverse(*buf++)) : MAX7219_InitStruct->SendReceive(*buf++);
          
      }
      else
      {
        MAX7219_InitStruct->SendReceive(0); //NOP
        MAX7219_InitStruct->SendReceive(0);
      }
    }
    MAX7219_InitStruct->ChipEnable(CHIP_DISABLE);
  }  
}

void MAX7219_DrawWindow (MAX7219_InitStructTypeDef * MAX7219_InitStruct, uint8_t * buf, uint16_t size)
{
  uint8_t devcnt, cnt, byte, row = 1;
  while (size)
  {
    MAX7219_InitStruct->ChipEnable(CHIP_ENABLE);
    for (devcnt = 0; devcnt != MAX7219_InitStruct->DeviceN; devcnt++)
    {
      byte = 0;
      for (cnt = 0; cnt != 8; cnt++)
      {
        if (*buf++) byte |= (1 << cnt);
        size--;
      }
      MAX7219_InitStruct->SendReceive(row);
      MAX7219_InitStruct->SendReceive(byte);
    }
    MAX7219_InitStruct->ChipEnable(CHIP_DISABLE);
    row++;
  }
}

void MAX7219_StructInitForDecode (MAX7219_InitStructTypeDef * MAX7219_InitStruct)
{
  MAX7219_InitStruct->DecodeMode = 0xFF;
  MAX7219_InitStruct->Intensity = 0x0F;
  MAX7219_InitStruct->ScanLimit = 7;
  MAX7219_InitStruct->TestMode = MAX7219_Test_NormalOperation;
  MAX7219_InitStruct->Power = MAX7219_Power_NormalOperation; 
}

void MAX7219_StructInitNoDecode (MAX7219_InitStructTypeDef * MAX7219_InitStruct)
{
  MAX7219_InitStruct->DecodeMode = 0x00;
  MAX7219_InitStruct->Intensity = 0x0F;
  MAX7219_InitStruct->ScanLimit = 7;
  MAX7219_InitStruct->TestMode = MAX7219_Test_NormalOperation;
  MAX7219_InitStruct->Power = MAX7219_Power_NormalOperation; 
}
  
void WriteReg (MAX7219_InitStructTypeDef * MAX7219_InitStruct, uint8_t Reg, uint8_t Value)
{
  uint8_t cnt;
  
  MAX7219_InitStruct->ChipEnable(CHIP_ENABLE);
  for (cnt = 0; cnt != MAX7219_InitStruct->DeviceN; cnt++)
  {
    MAX7219_InitStruct->SendReceive(Reg);
    MAX7219_InitStruct->SendReceive(Value);
  }
  MAX7219_InitStruct->ChipEnable(CHIP_DISABLE);
}

uint8_t GetDeviceN (MAX7219_InitStructTypeDef * MAX7219_InitStruct)
{
  uint8_t cnt = 0, rtn;
  if (MAX7219_InitStruct->SendReceive == 0) return 0;
  
  do
  {
    cnt++;
    rtn = MAX7219_InitStruct->SendReceive(0xFF);
  }while (cnt < 100 && rtn != 0xFF);
          
  cnt = 0;
  
  do
  {
    cnt++;
    rtn = MAX7219_InitStruct->SendReceive(0x00);
  }while (cnt < 100 && rtn != 0x00);
          
  return cnt;
}

uint8_t reverse(uint8_t b) 
{
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

