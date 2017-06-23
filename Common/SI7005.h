typedef enum
{
	MeasHum  = 0x01,
	MeasTemp = 0x11
}SI7005_MeasTypeDef;

typedef struct
{
	/* Data */
	char Humidity;
	float Temperature;
	
	/* Functions */
	char (*WriteReg)(char I2C_Adrs, char Reg, char Value);
	char (*ReadReg) (char I2C_Adrs, char Res, char * buf, char size);	
	
	/* Settings */
	char I2C_Adrs;
	char UseRelativeMeas;
	SI7005_MeasTypeDef MeasType;
}SI7005_StructTypeDef;

char SI7005_CheckID (SI7005_StructTypeDef * SI7005_Struct);
char SI7005_StartConversion (SI7005_StructTypeDef * SI7005_Struct);
char SI7005_GetResult (SI7005_StructTypeDef * SI7005_Struct);
char SI7005_DataReady (SI7005_StructTypeDef * SI7005_Struct);
