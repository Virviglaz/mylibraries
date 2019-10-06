#include "spi.h"

inline static void spi_select(GPIO_TypeDef *GPIOx, uint16_t PINx)
{	
	GPIOx->BRR = PINx;
}

inline static void spi_release(SPI_TypeDef *SPIx,
	GPIO_TypeDef *GPIOx, uint16_t PINx)
{
	while (SPIx->SR & SPI_SR_BSY);
	GPIOx->BSRR = PINx;	
}

uint8_t spi_read_byte(SPI_TypeDef *SPIx, uint8_t value)
{
	/* Loop while DR register in not empty */
	while (!(SPIx->SR & SPI_SR_TXE));

	/* Send byte through the SPI peripheral */
	*((__IO uint8_t *)&SPIx->DR) = value;

	/* Wait to receive a byte */
	while (!(SPIx->SR & SPI_SR_RXNE));

	return *((__IO uint8_t *)&SPIx->DR);
}

uint8_t spi_write_reg(SPI_TypeDef *SPIx, GPIO_TypeDef *GPIOx, uint16_t PINx,
	uint8_t reg, uint8_t *buf, uint16_t size)
{
	uint8_t ret;

	spi_select(GPIOx, PINx);

	ret = spi_read_byte(SPIx, reg);

	while(size--)
		spi_read_byte(SPIx, *buf++);

	spi_release(SPIx, GPIOx, PINx);

	return ret;
}

uint8_t spi_read_reg(SPI_TypeDef *SPIx, GPIO_TypeDef *GPIOx, uint16_t PINx,
	uint8_t reg, uint8_t *buf, uint16_t size)
{
	uint8_t ret;

	spi_select(GPIOx, PINx);	
	ret = spi_read_byte(SPIx, reg);

	while(size--)
		*buf++ = spi_read_byte(SPIx, 0x00);

	spi_release(SPIx, GPIOx, PINx);

	return ret;
}

void spi_init(SPI_TypeDef *SPIx, enum spi_dir dir,
	enum spi_clockdiv div, enum spi_clockmode clkdiv)
{
	SPIx->CR2 = 0x700 | SPI_CR2_FRXTH;
	SPIx->CR1 = (uint16_t)dir | (uint16_t)div | (uint16_t)clkdiv \
		| SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_MSTR | SPI_CR1_SPE;
}
