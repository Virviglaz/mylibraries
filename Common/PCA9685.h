#ifndef PCA9685_H
#define PCA9685_H

#include <stdint.h>

typedef enum
{
  PCA9685_EXT_CLK_DISABLED = 0,
  PCA9685_EXT_CLK_ENABLED = 0x40
}PCA9685_EXTCLK_TypeDef;

typedef struct
{
  PCA9685_EXTCLK_TypeDef PCA9685_EXTCLK;
  uint8_t Prescaler; //121 for 50Hz
  
  /* Interface function */
  uint8_t (* WriteReg) (uint8_t reg, uint8_t value);
}PCA9685_InitStructTypeDef;


uint8_t PCA9685_Init (PCA9685_InitStructTypeDef * PCA9685_Init);
uint8_t PCA9685_SetPWM (uint8_t Channel, uint16_t Value);

#endif
