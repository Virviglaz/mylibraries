#include "SW_I2C.h"

#define ACK     0
#define NACK    1

I2C_Result I2C__Start (I2C_SW_InitStructTypeDef * I2C_Struct)
{
	//Release SDA
	PIN_ON(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);
  
	//Release SCL
	PIN_ON(I2C_Struct->SCL_GPIO, I2C_Struct->SCL_PIN);
	
	//Some delay before check the BUS
	I2C_Struct->delay_func(I2C_Struct->DelayValue);
  
	//Check, if any BUS signal is low that meas that BUS is busy by other device or H/W issue
	if (!(PIN_SYG(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN) | 
			PIN_SYG(I2C_Struct->SCL_GPIO, I2C_Struct->SCL_PIN))) 
		return I2C_BUS_BUSY;
  
	//Force down SDA
	PIN_OFF(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);
  
	//Some delay - start condition started
	I2C_Struct->delay_func(I2C_Struct->DelayValue);
  
	//Force down SCL - start condition finished
	PIN_OFF(I2C_Struct->SCL_GPIO, I2C_Struct->SCL_PIN);
	
	//Some delay after start
	I2C_Struct->delay_func(I2C_Struct->DelayValue);
  return I2C_SUCCESS;
}

void I2C__ReStart (I2C_SW_InitStructTypeDef * I2C_Struct)
{
  //Release SDA
  PIN_ON(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);
  
  //Release SCL - do the same as START, but no BUS checking
  PIN_ON(I2C_Struct->SCL_GPIO, I2C_Struct->SCL_PIN);
  
  //Some delay
  I2C_Struct->delay_func(I2C_Struct->DelayValue);
  
  //Force down SDA
  PIN_OFF(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);
  
  //Some delay
  I2C_Struct->delay_func(I2C_Struct->DelayValue);
  
  //Force down SCL
  PIN_OFF(I2C_Struct->SCL_GPIO, I2C_Struct->SCL_PIN);
}

void I2C__Stop (I2C_SW_InitStructTypeDef * I2C_Struct)
{
  I2C_Struct->delay_func(I2C_Struct->DelayValue);
  PIN_OFF(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);
  PIN_ON(I2C_Struct->SCL_GPIO, I2C_Struct->SCL_PIN);
  
  I2C_Struct->delay_func(I2C_Struct->DelayValue);
  PIN_ON(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);  
  I2C_Struct->delay_func(I2C_Struct->DelayValue);
}

uint16_t I2C__Clock(I2C_SW_InitStructTypeDef * I2C_Struct)
{
  uint16_t Result;
  PIN_ON(I2C_Struct->SCL_GPIO, I2C_Struct->SCL_PIN);
  I2C_Struct->delay_func(I2C_Struct->DelayValue);
  Result = PIN_SYG(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);
  PIN_OFF(I2C_Struct->SCL_GPIO, I2C_Struct->SCL_PIN);
  I2C_Struct->delay_func(I2C_Struct->DelayValue);
  return Result;  
}

uint16_t I2C__Write(I2C_SW_InitStructTypeDef * I2C_Struct, unsigned char data)
{
  uint8_t mask = 0x80;
  while (mask)
  {
    if(data & mask)
			PIN_ON(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);
    else
			PIN_OFF(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN); 
    I2C__Clock(I2C_Struct);  
    mask >>= 1;
  }
  PIN_ON(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);
  return I2C__Clock(I2C_Struct);
}

uint16_t I2C__Read (I2C_SW_InitStructTypeDef * I2C_Struct, unsigned char Acknowledgment)
{
  uint8_t data = 0, mask = 0x80;        
  while(mask)
  {
    if (I2C__Clock(I2C_Struct))
      data |= mask;
    mask >>= 1;
  }
  if (Acknowledgment)
  {
		PIN_OFF(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);	
    I2C__Clock(I2C_Struct);
		PIN_ON(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);
  }
  else
  {
		PIN_ON(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);
    I2C__Clock(I2C_Struct);
  }
  return data;
}

I2C_Result SW_I2C_Read_Reg      (I2C_SW_InitStructTypeDef * I2C_Struct)
{
  I2C_Result Result = I2C__Start(I2C_Struct);
  if (Result) return Result;
  Result = I2C_ADD_NOT_EXIST;
  if (I2C__Write(I2C_Struct, I2C_Struct->I2C_Address) == ACK)
    if (I2C__Write(I2C_Struct, I2C_Struct->Reg_AddressOrLen) == ACK)
    {
      I2C__ReStart(I2C_Struct);
      if (I2C__Write(I2C_Struct, I2C_Struct->I2C_Address | 1) == ACK)
      {
        while (I2C_Struct->pBufferSize--)
          * I2C_Struct->pBuffer++ = I2C__Read(I2C_Struct, I2C_Struct->pBufferSize ? 1 : 0);
        Result = I2C_SUCCESS;
      }
    }
  I2C__Stop(I2C_Struct);
  return Result;
}

I2C_Result SW_I2C_Read_Page (I2C_SW_InitStructTypeDef * I2C_Struct)
{
  I2C_Result Result = I2C__Start(I2C_Struct);
  if (Result) return Result;
  Result = I2C_ADD_NOT_EXIST;
  if (I2C__Write(I2C_Struct, I2C_Struct->I2C_Address & 0xFE) == ACK)
  {
    Result = (I2C_Result)ACK;
    while(I2C_Struct->Reg_AddressOrLen-- && Result == ACK)
      Result = (I2C_Result)I2C__Write(I2C_Struct, * I2C_Struct->Reg_Address++);
      I2C__ReStart(I2C_Struct);
      if (I2C__Write(I2C_Struct, I2C_Struct->I2C_Address | 0x01) == ACK)
      {
        while (I2C_Struct->pBufferSize--)
          * I2C_Struct->pBuffer++ = I2C__Read(I2C_Struct, I2C_Struct->pBufferSize ? 1 : 0);
        Result = I2C_SUCCESS;
      }   
  }
  I2C__Stop(I2C_Struct);
  return Result;
}

I2C_Result SW_I2C_Write_Reg      (I2C_SW_InitStructTypeDef * I2C_Struct)
{
  I2C_Result Result = I2C_ADD_NOT_EXIST;
  if (I2C__Start(I2C_Struct) != I2C_SUCCESS) return I2C_BUS_BUSY;
  if (I2C__Write(I2C_Struct, I2C_Struct->I2C_Address & 0xFE) == ACK)
    if (I2C__Write(I2C_Struct, I2C_Struct->Reg_AddressOrLen) == ACK)
    {
      while (I2C_Struct->pBufferSize--) 
        I2C__Write(I2C_Struct, * I2C_Struct->pBuffer++);
      Result = I2C_SUCCESS;
    }  
  I2C__Stop(I2C_Struct); 
  return Result;   
}

I2C_Result SW_I2C_Write_Page  (I2C_SW_InitStructTypeDef * I2C_Struct)
{
  I2C_Result Result = I2C_ADD_NOT_EXIST; 
  if (I2C__Start(I2C_Struct) != I2C_SUCCESS) return I2C_BUS_BUSY;
  if (I2C__Write(I2C_Struct, I2C_Struct->I2C_Address & 0xFE) == ACK)
  {
		Result = (I2C_Result)ACK;
		while(I2C_Struct->Reg_AddressOrLen-- && Result == ACK)
			Result = (I2C_Result)I2C__Write(I2C_Struct, * I2C_Struct->Reg_Address++);
		while(I2C_Struct->pBufferSize-- && Result == ACK)
			Result = (I2C_Result)I2C__Write(I2C_Struct, * I2C_Struct->pBuffer++);
  }
  I2C__Stop(I2C_Struct);  
  return Result;
}

I2C_Result SW_I2C_Write_Byte (I2C_SW_InitStructTypeDef * I2C_Struct, unsigned char Reg, unsigned char Value)
{
	unsigned char pValue[1];
	I2C_Struct->Reg_AddressOrLen = Reg;
	pValue[0] = Value;
	I2C_Struct->pBuffer = pValue;
	return SW_I2C_Write_Reg(I2C_Struct);
}

I2C_Result SW_I2C_Check_Bus (I2C_SW_InitStructTypeDef * I2C_Struct)
{
	char i = 100;
	if (!PIN_SYG(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN) || !PIN_SYG(I2C_Struct->SCL_GPIO, I2C_Struct->SCL_PIN))
	{
		/* BUS on HOLD. PERFORM RESET! */
		PIN_OFF(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);
		PIN_OFF(I2C_Struct->SCL_GPIO, I2C_Struct->SCL_PIN);
		while (i--)
			I2C_Struct->delay_func(I2C_Struct->DelayValue);
		
		/* Release BUS */
		PIN_ON(I2C_Struct->SDA_GPIO, I2C_Struct->SDA_PIN);
		PIN_ON(I2C_Struct->SCL_GPIO, I2C_Struct->SCL_PIN);
		return I2C_BUS_BUSY;
	}
	return I2C_SUCCESS;
}
