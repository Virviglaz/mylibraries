#include "1-Wire.h"
#include "stm32_GPIO.h"

/**
  * @brief  Perform RESET sequence
  * @param  One_Wire_Bus: Pin type structure to specify corresponding I/O pin
  * @retval One_Wire_Success or One_Wire_Bus_Low_Error
  */
One_Wire_ErrorTypeDef One_Wire_Reset(One_Wire_StructTypeDef * One_Wire_Bus)								
{
	/* Declare result variable */
	One_Wire_ErrorTypeDef Result = One_Wire_Success;

	/* Wait some time before send RESET pulse */
	One_Wire_Bus->Delay_Func(Time_Before_Reset);

	/* Check that bus is in high state before RESET */
	#ifdef DoubleWire
		if (PIN_SYG(One_Wire_Bus->GPIO_Pin_Rx, One_Wire_Bus->PIN_Rx)) //read pin state (inverted logic)
	#else
		PIN_OUT_OD(One_Wire_Bus->GPIO_Pin, One_Wire_Bus->PIN);		//switch to output open drain
		PIN_ON(One_Wire_Bus->GPIO_Pin, One_Wire_Bus->PIN);				//release bus
		if (!PIN_SYG(One_Wire_Bus->GPIO_Pin, One_Wire_Bus->PIN))	//read pin state
	#endif
			return One_Wire_Bus_Low_Error;
	
	/* Force TX pin low - begin of RESET sequencs */
	#ifdef DoubleWire
		PIN_OFF(One_Wire_Bus->GPIO_Pin_Tx, One_Wire_Bus->PIN_Tx);	// force bus low
	#else
		PIN_OFF(One_Wire_Bus->GPIO_Pin, One_Wire_Bus->PIN);				//force bus low
	#endif
		
	/* Keep TX pin low for ~500us (typical) */
	One_Wire_Bus->Delay_Func(Time_Reset_Low);

	/* Release TX pin */
	#ifdef DoubleWire
		PIN_ON(One_Wire_Bus->GPIO_Pin_Tx, One_Wire_Bus->PIN_Tx);
	#else
		PIN_ON(One_Wire_Bus->GPIO_Pin, One_Wire_Bus->PIN);
	#endif
		
	/* Wait some time before read RESPONCE status */
	One_Wire_Bus->Delay_Func(Time_Pulse_Delay_High);
	
	/* If bus is high that means that no RESPONCE obtained */
	#ifdef DoubleWire
		if (!PIN_SYG(One_Wire_Bus->GPIO_Pin_Rx, One_Wire_Bus->PIN_Rx)) 	//read pin state (inverted logic)
	#else
		if (PIN_SYG(One_Wire_Bus->GPIO_Pin, One_Wire_Bus->PIN))					//read pin state
	#endif
			Result = One_Wire_Error_No_Echo;
	
	/* Wait some time before transmisson started */
	One_Wire_Bus->Delay_Func(Time_After_Reset);

	return Result;
}

/**
  * @brief  Perform write one bit to 1-wire device
  * @param  One_Wire_Bus: Pin type structure to specify corresponding I/O pin
  * @param  Bit: data to send
  * @retval none
  */
void One_Wire_Write_Bit (One_Wire_StructTypeDef * One_Wire_Bus, unsigned char Bit)
{
	/* Force TX pin to LOW state and initialise transmission */
	#ifdef DoubleWire
		PIN_OFF(One_Wire_Bus->GPIO_Pin_Tx, One_Wire_Bus->PIN_Tx);
	#else
		PIN_OFF(One_Wire_Bus->GPIO_Pin, One_Wire_Bus->PIN);
	#endif
	
	if (Bit)
	{
		/* Write 1 to 1-wire device */
		One_Wire_Bus->Delay_Func(Time_Pulse_Delay_Low);
		#ifdef DoubleWire
			PIN_ON(One_Wire_Bus->GPIO_Pin_Tx, One_Wire_Bus->PIN_Tx);
		#else
			PIN_ON(One_Wire_Bus->GPIO_Pin, One_Wire_Bus->PIN);
		#endif
		One_Wire_Bus->Delay_Func(Time_Pulse_Delay_High);
	}
	else
	{
		/* Write 0 to 1-wire device */
		One_Wire_Bus->Delay_Func(Time_Pulse_Delay_High);
		#ifdef DoubleWire
			PIN_ON(One_Wire_Bus->GPIO_Pin_Tx, One_Wire_Bus->PIN_Tx);
		#else
			PIN_ON(One_Wire_Bus->GPIO_Pin, One_Wire_Bus->PIN);
		#endif
		One_Wire_Bus->Delay_Func(Time_Pulse_Delay_Low);
	}
} 

/**
  * @brief  Perform write one byte to 1-wire device
  * @param  One_Wire_Bus: Pin type structure to specify corresponding I/O pin
  * @param  Byte: data to send
  * @retval none
  */
void One_Wire_Write_Byte(One_Wire_StructTypeDef * One_Wire_Bus, unsigned char Byte)
{
	unsigned char cnt;
	for (cnt=0; cnt!=8; cnt++) 
		One_Wire_Write_Bit(One_Wire_Bus, Byte & (1 << cnt));
}

/**
  * @brief  Perform read one bit from 1-wire device
  * @param  One_Wire_Bus: Pin type structure to specify corresponding I/O pin
  * @retval Data readed from 1-wire device
  */
unsigned char One_Wire_Read_Bit (One_Wire_StructTypeDef * One_Wire_Bus)
{
		unsigned char tmp;
	
		#ifdef DoubleWire
			PIN_OFF(One_Wire_Bus->GPIO_Pin_Tx, One_Wire_Bus->PIN_Tx);
		#else
			PIN_OFF(One_Wire_Bus->GPIO_Pin, One_Wire_Bus->PIN);
		#endif
		One_Wire_Bus->Delay_Func(Time_Hold_Down);
	
		#ifdef DoubleWire
			PIN_ON(One_Wire_Bus->GPIO_Pin_Tx, One_Wire_Bus->PIN_Tx);
		#else
			PIN_ON(One_Wire_Bus->GPIO_Pin, One_Wire_Bus->PIN);
		#endif
	
		One_Wire_Bus->Delay_Func(Time_Pulse_Delay_Low);
	
		#ifdef DoubleWire
			if (PIN_SYG(One_Wire_Bus->GPIO_Pin_Rx, One_Wire_Bus->PIN_Rx))
		#else				
			if (!PIN_SYG(One_Wire_Bus->GPIO_Pin, One_Wire_Bus->PIN))
		#endif				
					tmp = 0;
				else 
					tmp = 1;
			
		One_Wire_Bus->Delay_Func(Time_Pulse_Delay_High);
		return tmp;
}

/**
  * @brief  Perform read one byte from 1-wire device
  * @param  One_Wire_Bus: Pin type structure to specify corresponding I/O pin
  * @retval Data readed from 1-wire device
  */
unsigned char One_Wire_Read_Byte(One_Wire_StructTypeDef * One_Wire_Bus)
{
	unsigned char tmp = 0;
	unsigned char cnt;
	for (cnt = 0; cnt != 8; cnt++)
		if (One_Wire_Read_Bit(One_Wire_Bus))	tmp |= (1 << cnt);
	One_Wire_Bus->Delay_Func(Time_Pulse_Delay_High);
	return tmp;
}


