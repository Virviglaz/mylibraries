void SPI_Init_ClockLOW (void);
void SPI_Init_ClockHigh (void);
u8 SPI_ReadByte (u8 Data);
void SPI_SendByte (u8 Data);
void SPI_Select (GPIO_TypeDef * GPIOx,u8 PINx);
void SPI_Deselect (GPIO_TypeDef * GPIOx,u8 PINx);
u8 SPI_WriteReg (GPIO_TypeDef * GPIOx, u8 PINx, u8 reg, u8 * buf, u16 size);
u8 SPI_ReadReg (GPIO_TypeDef * GPIOx, u8 PINx, u8 reg, u8 * buf, u16 size);
