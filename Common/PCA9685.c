#include "PCA9685.h"

PCA9685_InitStructTypeDef * PCA9685_Driver;

uint8_t PCA9685_Init (PCA9685_InitStructTypeDef * PCA9685_Init)
{
  uint8_t CLK = (uint8_t)PCA9685_Driver->PCA9685_EXTCLK | 0x20 | 0x80;
  if (PCA9685_Init == 0) return 0xFF;
  
  PCA9685_Driver = PCA9685_Init;
  
  PCA9685_Driver->WriteReg(0xFE, &PCA9685_Driver->Prescaler, 1);
  return PCA9685_Driver->WriteReg(0x00, &CLK, 1);
}

uint8_t PCA9685_SetPWM (uint8_t Channel, uint16_t Value)
{
  uint8_t buf[4] = { 0, 0, Value, (Value >> 8) & 0x0F };
  return PCA9685_Driver->WriteReg(6 + Channel * 4, buf, sizeof(buf));
}

uint8_t PCA9685_SetAll (uint16_t * Values)
{
  uint8_t cnt;
  uint8_t * buf = PCA9685_Driver->PwmBuf;
  
  if (buf) //if buffer used, update all at once, takes more memory
  {
    for (cnt = 0; cnt < 16; cnt++)
    {
      * buf++ = 0;
      * buf++ = 0;
      * buf++ = (uint8_t)* Values;
      * buf++ = (* Values++ >> 8) & 0x0F;
    }
    return PCA9685_Driver->WriteReg (6, PCA9685_Driver->PwmBuf, 64); //fixed size of buffer
  }
  /* buffer not used, update one by one, takes more time */
  for (cnt = 0; cnt < 15; cnt++)
    PCA9685_SetPWM(cnt, Values[cnt]);

  return PCA9685_SetPWM(15, Values[15]);
}