#include "PCA9685.h"

PCA9685_InitStructTypeDef * PCA9685;

uint8_t PCA9685_Init (PCA9685_InitStructTypeDef * PCA9685_Init)
{
  if (PCA9685_Init == 0) return 0xFF;
  
  PCA9685 = PCA9685_Init;
  
  PCA9685->WriteReg(0xFE, PCA9685->Prescaler);
  return PCA9685->WriteReg(0x00, (uint8_t)PCA9685->PCA9685_EXTCLK | 0x20);
}

uint8_t PCA9685_SetPWM (uint8_t Channel, uint16_t Value)
{
 uint8_t RegN = 0x06 + Channel * 4;
 
 //PCA9685->WriteReg(RegN++, 0); //LSB
 //PCA9685->WriteReg(RegN++, 0); //MSB
 
 RegN += 2;
 
 PCA9685->WriteReg(RegN++, (uint8_t)Value); //LSB
 PCA9685->WriteReg(RegN, 0x0F & (Value >> 8)); //MSB
 
 return 0; 
}
