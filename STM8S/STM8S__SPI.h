void SPI_Init_ClockLOW (SPI_BaudRatePrescaler_TypeDef SPI_BaudRatePrescaler);
void SPI_Init_ClockHigh (SPI_BaudRatePrescaler_TypeDef SPI_BaudRatePrescaler);
u8 SPI_ReadByte   (u8 Data);
void SPI_Select   (GPIO_TypeDef * GPIOx, u8 PINx);
void SPI_Deselect (GPIO_TypeDef * GPIOx, u8 PINx);
u8 SPI_WriteReg (GPIO_TypeDef * GPIOx, u8 PINx, u8 reg, u8 * buf, u16 size);
u8 SPI_ReadReg  (GPIO_TypeDef * GPIOx, u8 PINx, u8 reg, u8 * buf, u16 size);
u8 SPI_RW_Reg (GPIO_TypeDef * GPIOx, u8 PINx, u8 reg, u8 * buf, u16 size);
u8 SPI_ReadRegInc (GPIO_TypeDef * GPIOx, u8 PINx, u8 reg, u8 * buf, u16 size, s8 inc);
