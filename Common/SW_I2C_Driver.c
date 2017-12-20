#include "SW_I2C_Driver.h"

#define ACK     0
#define NACK    1
#define HIGH		1
#define LOW			0

/* Local driver pointer */
SW_I2C_DriverStructTypeDef * SW_I2C_Driver;

/* Local static functions */
static I2C_Result SW_I2C_Start (void);
static void SW_I2C_ReStart (void);
static void SW_I2C_Stop (void);
static uint16_t SW_I2C_Clock(void);
static uint16_t SW_I2C_Write(uint8_t data);
static uint16_t SW_I2C_Read (uint8_t ack);

I2C_Result SW_I2C_ASSIGN (SW_I2C_DriverStructTypeDef * SW_I2C_DriverToAssign)
{
	SW_I2C_Driver = SW_I2C_DriverToAssign;
	
	return SW_I2C_Driver ? I2C_SUCCESS : I2C_INTERFACE_ERROR;
}

SW_I2C_DriverStructTypeDef * SW_I2C_GetDriver (void)
{
	return SW_I2C_Driver;
}

void SW_I2C_RESET_BUS (void)
{
	SW_I2C_Driver->IO_SDA_Write(LOW);
	SW_I2C_Driver->IO_SCL_Write(LOW);
	
	//Some delay if needed
	if (SW_I2C_Driver->Delay_func)
		SW_I2C_Driver->Delay_func(SW_I2C_Driver->DelayValue);	

	SW_I2C_Driver->IO_SDA_Write(HIGH);
	SW_I2C_Driver->IO_SCL_Write(HIGH);

	//Some delay if needed
	if (SW_I2C_Driver->Delay_func)
		SW_I2C_Driver->Delay_func(SW_I2C_Driver->DelayValue);	
}

I2C_Result SW_I2C_WR (uint8_t address, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size)
{
  I2C_Result Result = I2C_ADD_NOT_EXIST;
	
	/* Check interface exist */
	if (SW_I2C_Driver == 0) return I2C_INTERFACE_ERROR;
	
	/* Check BUS */
  if (SW_I2C_Start() != I2C_SUCCESS) return I2C_BUS_BUSY;
	
	/* Send device address for write */
  if (SW_I2C_Write(address & 0xFE) == ACK) //if device respond, procceed
  {
		Result = I2C_SUCCESS;
		
		while (reglen--) //send reg value
			Result = (I2C_Result)SW_I2C_Write (* reg++);
		
		while(size-- && Result == I2C_SUCCESS) //send buffer
			Result = (I2C_Result)SW_I2C_Write(* buf++);
  }
	
  SW_I2C_Stop();  
  return Result;
}

I2C_Result SW_I2C_RD (uint8_t address, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size)
{
	I2C_Result Result = I2C_ADD_NOT_EXIST;

	/* Check interface exist */
	if (SW_I2C_Driver == 0) return I2C_INTERFACE_ERROR;
	
	/* Check BUS */
	if (SW_I2C_Start() != I2C_SUCCESS) return I2C_BUS_BUSY;

	/* Send device address for write */
  if (SW_I2C_Write(address & 0xFE) == ACK) //if device respond, procceed
  {
    Result = (I2C_Result)ACK;
		
    while(reglen--)
      Result = (I2C_Result)SW_I2C_Write (* reg++);
		
      SW_I2C_ReStart();

      if (SW_I2C_Write(address | 0x01) == I2C_SUCCESS)
      {
        while (size--)
          * buf++ = SW_I2C_Read(size ? 1 : 0);  //read to buffer
        Result = I2C_SUCCESS;
      }   
  }
  SW_I2C_Stop();
  return Result;
}

I2C_Result SW_I2C_FAST_RD (uint8_t address, uint8_t * buf, uint16_t size)
{
	I2C_Result Result = I2C_ADD_NOT_EXIST;

	/* Check interface exist */
	if (SW_I2C_Driver == 0) return I2C_INTERFACE_ERROR;
	
	/* Check BUS */
	if (SW_I2C_Start() != I2C_SUCCESS) return I2C_BUS_BUSY;

	if (SW_I2C_Write(address | 0x01) == ACK)
	{
		while (size--)
			* buf++ = SW_I2C_Read(size ? 1 : 0);  //read to buffer
		Result = I2C_SUCCESS;
	}   
  
  SW_I2C_Stop();
  return Result;
}

I2C_Result SW_I2C_RD_POOLING_ACK (uint8_t address, uint8_t reg, uint8_t * buf, uint16_t size, uint8_t attempts)
{
	I2C_Result Result = I2C_ADD_NOT_EXIST;

	/* Check interface exist */
	if (SW_I2C_Driver == 0) return I2C_INTERFACE_ERROR;
	
	/* Check BUS */
	if (SW_I2C_Start() != I2C_SUCCESS) return I2C_BUS_BUSY;

	/* Send device address for write */
  if (SW_I2C_Write(address & 0xFE) == ACK) //if device respond, procceed
  {
		SW_I2C_Write(reg);
		do
		{
			SW_I2C_ReStart();
			Result = (I2C_Result)SW_I2C_Write(address | 0x01);
		}while (Result && --attempts);
		
		if (attempts) 
		{
			while (size--)
				* buf++ = SW_I2C_Read(size ? 1 : 0);  //read to buffer
		}
		else
			Result = I2C_TIMEOUT;
	}

  SW_I2C_Stop();
  return Result;
}

uint8_t SW_I2C_WriteWithFlagPooling (uint8_t address, uint8_t reg, uint8_t * value, uint8_t attempts, uint8_t flagToPooling)
{
	uint8_t res = I2C_SUCCESS, data;

	/* Check interface exist */
	if (SW_I2C_Driver == 0) return I2C_INTERFACE_ERROR;
	
	/* Check BUS */
	if (SW_I2C_Start() != I2C_SUCCESS) return I2C_BUS_BUSY;
	
	/* Send device address for write */
  if (SW_I2C_Write(address & 0xFE) == I2C_SUCCESS) //if device respond, procceed
	{
		/* Choose register if needed */
		if (reg) SW_I2C_Write(reg);
		
		if (value) //write value if needed
			SW_I2C_Write(* value);	
			
		SW_I2C_ReStart();
		SW_I2C_Write(address | 0x01);
		
		do //flag pooling
		{
			data = SW_I2C_Read(1);
		}while(attempts-- && (data & flagToPooling));
		
		SW_I2C_Read(0);
	}
	
	SW_I2C_Stop();
	
	if (attempts == 0) return I2C_TIMEOUT;

	return res;
}

/* INTERNAL STATIC FUNCTIONS */

static I2C_Result SW_I2C_Start (void)
{
	//Release SDA
	SW_I2C_Driver->IO_SDA_Write(HIGH);
  
	//Release SCL
	SW_I2C_Driver->IO_SCL_Write(HIGH);
	
	//Some delay before check the BUS if needed
	if (SW_I2C_Driver->Delay_func)
		SW_I2C_Driver->Delay_func(SW_I2C_Driver->DelayValue);
  
	//Check, if any BUS signal is low that meas that BUS is busy by other device or H/W issue
	if (SW_I2C_Driver->IO_SDA_Read() == 0)	return I2C_BUS_BUSY;
  
	//Force down SDA
	SW_I2C_Driver->IO_SDA_Write(LOW);
  
	//Some delay if needed
	if (SW_I2C_Driver->Delay_func)
		SW_I2C_Driver->Delay_func(SW_I2C_Driver->DelayValue);
  
	//Force down SCL - start condition finished
	SW_I2C_Driver->IO_SCL_Write(LOW);
	
	//Some delay after start
	if (SW_I2C_Driver->Delay_func)
		SW_I2C_Driver->Delay_func(SW_I2C_Driver->DelayValue);
	
  return I2C_SUCCESS;
}

static void SW_I2C_ReStart (void)
{
  //Release SDA
  SW_I2C_Driver->IO_SDA_Write(HIGH);
  
  //Release SCL - do the same as START, but no BUS checking
  SW_I2C_Driver->IO_SCL_Write(HIGH);
  
	//Some delay if needed
	if (SW_I2C_Driver->Delay_func)
		SW_I2C_Driver->Delay_func(SW_I2C_Driver->DelayValue);
  
  //Force down SDA
  SW_I2C_Driver->IO_SDA_Write(LOW);
  
	//Some delay if needed
	if (SW_I2C_Driver->Delay_func)
		SW_I2C_Driver->Delay_func(SW_I2C_Driver->DelayValue);
  
  //Force down SCL
  SW_I2C_Driver->IO_SCL_Write(LOW);
}

static void SW_I2C_Stop (void)
{
	//Some delay if needed
	if (SW_I2C_Driver->Delay_func)
		SW_I2C_Driver->Delay_func(SW_I2C_Driver->DelayValue);
	
  //Force down SDA
  SW_I2C_Driver->IO_SDA_Write(LOW);
	
	//Release SCL
  SW_I2C_Driver->IO_SCL_Write(HIGH);
  
	//Some delay if needed
	if (SW_I2C_Driver->Delay_func)
		SW_I2C_Driver->Delay_func(SW_I2C_Driver->DelayValue);
	
	//Release SDA
  SW_I2C_Driver->IO_SDA_Write(HIGH);  

	//Some delay if needed
	if (SW_I2C_Driver->Delay_func)
		SW_I2C_Driver->Delay_func(SW_I2C_Driver->DelayValue);
}

static uint16_t SW_I2C_Clock(void)
{
  uint16_t Result;
	
	//Release SCL
  SW_I2C_Driver->IO_SCL_Write(HIGH);
	
	//Some delay if needed
	if (SW_I2C_Driver->Delay_func)
		SW_I2C_Driver->Delay_func(SW_I2C_Driver->DelayValue);
	
  Result = SW_I2C_Driver->IO_SDA_Read();
	
  //Force down SCL
  SW_I2C_Driver->IO_SCL_Write(LOW);
	
	//Some delay if needed
	if (SW_I2C_Driver->Delay_func)
		SW_I2C_Driver->Delay_func(SW_I2C_Driver->DelayValue);
	
  return Result;  
}

static uint16_t SW_I2C_Write(uint8_t data)
{
  uint8_t mask = 0x80;
	
  while (mask)
  {
		(data & mask) ? SW_I2C_Driver->IO_SDA_Write(HIGH) : SW_I2C_Driver->IO_SDA_Write(LOW);
		
    SW_I2C_Clock();  
    mask >>= 1;
  }
	
  SW_I2C_Driver->IO_SDA_Write(HIGH);
  return SW_I2C_Clock();
}

static uint16_t SW_I2C_Read (uint8_t ack)
{
  uint8_t data = 0, mask = 0x80;
	
  while(mask)
  {
    if (SW_I2C_Clock()) data |= mask;
    mask >>= 1;
  }
	
  if (ack)
  {
		SW_I2C_Driver->IO_SDA_Write(LOW);	
    SW_I2C_Clock();
		SW_I2C_Driver->IO_SDA_Write(HIGH);
  }
  else
  {
		SW_I2C_Driver->IO_SDA_Write(HIGH);
    SW_I2C_Clock();
  }
	
  return data;
}
