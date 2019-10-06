#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include "gpio.h"

enum spi_dir {
	SPIx_Unidirection = 0x00,
	SPIx_Bidirection =  SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE,
};

enum spi_clockdiv {
	SPIx_Div2   = 0x00,
	SPIx_Div4   = 0x08,
	SPIx_Div8   = 0x10,
	SPIx_Div16  = 0x18,
	SPIx_Div32  = 0x20,
	SPIx_Div64  = 0x28,
	SPIx_Div128 = 0x30,
	SPIx_Div256 = 0x38,
};

enum spi_clockmode {
	SPIx_ClockLow = 0,
	SPIx_ClockHigh = SPI_CR1_CPHA | SPI_CR1_CPOL,
};

void spi_init(SPI_TypeDef *SPIx,
	enum spi_dir dir,
	enum spi_clockdiv div,
	enum spi_clockmode clkdiv);

uint8_t spi_read_byte(SPI_TypeDef *SPIx, uint8_t value);

uint8_t spi_write_reg(SPI_TypeDef *SPIx, GPIO_TypeDef *GPIOx, uint16_t PINx,
	uint8_t reg, uint8_t *buf, uint16_t size);

uint8_t spi_read_reg(SPI_TypeDef *SPIx, GPIO_TypeDef *GPIOx, uint16_t PINx,
	uint8_t reg, uint8_t *buf, uint16_t size);

#endif /* SPI_H */
