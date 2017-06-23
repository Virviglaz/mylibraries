#include "STM8S__I2C.h"
#define I2c_time_out  10000

I2C_Result I2C__Start (void);
I2C_Result I2c__WriteAddress (unsigned char data);
unsigned char I2C__Write (unsigned char data);
void I2C__Stop (void);
unsigned char I2C__Read (unsigned char I2C_ACK);

void I2C__Init (unsigned long F_OSC_hz, unsigned long F_i2c_hz)
{
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
  I2C_Init(F_i2c_hz, 0xA0, I2C_DUTYCYCLE_2, I2C_ACK_NONE, I2C_ADDMODE_7BIT, 16);
  I2C_Cmd(ENABLE);  
}

I2C_Result I2C__Start (void)
{
  if (I2C->SR3 & I2C_SR3_BUSY) return I2C_BUS_BUSY;
  I2C->CR2 |= I2C_CR2_START;
  while(!(I2C->SR1 & I2C_CR2_START));
  return I2C_SUCCESS;
}

void I2C__Stop (void)
{
  I2C->CR2 |= I2C_CR2_STOP; 
  while (!(I2C->SR1 & I2C_SR1_STOPF)); 
}

I2C_Result I2c__WriteAddress (unsigned char data)
{
  unsigned int timeout = I2c_time_out;
  I2C->DR = data; 
  while(!(I2C->SR1 & I2C_SR1_ADDR) && timeout) timeout--;
  I2C->SR3;
  if (timeout == 0) return I2C_ADD_NOT_EXIST;
  while(!(I2C->SR1 & I2C_SR1_TXE));
  return I2C_SUCCESS;
}

unsigned char I2C__Write (unsigned char data)
{
  while(!(I2C->SR1 & I2C_SR1_TXE));
  I2C->DR = data;
  //while(!(I2C->SR1 & I2C_SR1_BTF));
  //while(!(I2C->SR1 & I2C_SR1_TXE));
  if (I2C->SR2 & I2C_SR2_AF)
  {
    I2C->SR2 &=~I2C_SR2_AF;
    return NACK;
  }
  return ACK;
}

unsigned char I2C__Read (unsigned char I2C_ACK)
{
  unsigned char i;
  if (ACK) I2C->CR2 |= I2C_CR2_ACK;
  else I2C->CR2 &= ~I2C_CR2_ACK;
  while(!(I2C->SR1 & I2C_SR1_RXNE));
abc:
  if(I2C->SR1 & I2C_SR1_BTF) i = I2C->DR;
  else goto abc;
  if(I2C->SR1 & I2C_SR1_BTF) goto abc;
  return i;
}

I2C_Result I2C__Read_Page (unsigned char I2c_Address, unsigned char Mem_Address, 
                          unsigned char * pBuffer, unsigned char pBufferSize)
{
  I2C_Result Result = I2C__Start();
  if (Result) return Result;
  Result = I2C_ADD_NOT_EXIST;
  if (I2c__WriteAddress(I2c_Address) == I2C_SUCCESS)
    if (I2C__Write(Mem_Address) == ACK)
    {
      I2C__Start();
      if (I2C__Write(I2c_Address | 1) == ACK)
      {
        while (pBufferSize--)
          *pBuffer++ = I2C__Read(pBufferSize ? 1 : 0);
        Result = I2C_SUCCESS;
      }
    }
  I2C__Stop();
  return Result;
}

I2C_Result I2C__Write_Page (unsigned char I2c_Address, unsigned char Mem_Address, 
                          unsigned char * pBuffer, unsigned char pBufferSize)
{
  I2C_Result Result = I2C_ADD_NOT_EXIST;
  if (I2C__Start()) return I2C_BUS_BUSY;
  if (Result) return Result;
  if (I2C__Write(I2c_Address & 0xFE) == ACK)
  {
    while (pBufferSize--) 
      I2C__Write(* pBuffer++);
    Result = I2C_SUCCESS;
  }  
  I2C__Stop(); 
  return Result;   
}

I2C_Result I2C__Write_Register (unsigned char I2c_Address, unsigned char Reg, unsigned char data)
{
  I2C_Result Result = I2C__Start(); 
  if (Result != I2C_SUCCESS) return Result;
  Result = I2c__WriteAddress(I2c_Address & 0xFE);
  if (Result == I2C_SUCCESS)
  {
    I2C__Write(Reg);
    I2C__Write(data);
  }
  I2C__Stop();  
  return Result;
}

