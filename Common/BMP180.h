typedef enum
{
	BMP180_OV_Single = 0,
	BMP180_OversamplingX2,
	BMP180_OversamplingX4,
	BMP180_OversamplingX8,	
}BMP180_OversamplingEnumTypeDef;

typedef struct
{
	/* Data */
	float Temperature;
	long Pressure;
	
	/* Functions */
	char (*WriteReg)(char I2C_Adrs, char Reg, char Value);
	char (*ReadReg) (char I2C_Adrs, char Reg, char * buf, char size);
	void (*delay_func)(unsigned int ms);
	
	/* Settings */
	char I2C_Adrs;				//I2c address. Default value 0xEE
	BMP180_OversamplingEnumTypeDef P_Oversampling;
	
	/* Internal data */
	short AC1;
	short AC2;
	short AC3;
	unsigned short AC4;
	unsigned short AC5;
	unsigned short AC6;
	short B1;
	short B2;
	short MB;
	short MC;
	short MD;
	long UT;
	long UP;
}BMP180_StructTypeDef;

char BMP180_Init (BMP180_StructTypeDef * BMP180_Struct);
void BMP180_Get_Result (BMP180_StructTypeDef * BMP180_Struct);
float Altitude (long Pressure);
unsigned short Pa_To_Hg (long Pressure_In_Pascals);
char BMP180_Check_ID (BMP180_StructTypeDef * BMP180_Struct);
