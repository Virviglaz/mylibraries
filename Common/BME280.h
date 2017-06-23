typedef enum
{
	BME280_OV_Skipped = 0,
	BME280_OversamplingX1,
	BME280_OversamplingX2,
	BME280_OversamplingX4,
	BME280_OversamplingX8,
	BME280_OversamplingX16,	
}BME280_OversamplingEnumTypeDef;

typedef struct
{
	/* Data */
	float Humidity;
	long Pressure;
	float Temperature;
	
	/* Functions */
	char (*WriteReg)(char I2C_Adrs, char Reg, char Value);
	char (*ReadReg) (char I2C_Adrs, char Reg, char * buf, char size);
	
	/* Settings */
	BME280_OversamplingEnumTypeDef HumidityOversampling;
	BME280_OversamplingEnumTypeDef PressureOversampling;
	BME280_OversamplingEnumTypeDef TemperatureOversampling;
	char I2C_Adrs;				//I2c address. Default value 0xEC
	
	/* Internal data */
	unsigned short 	dig_T1;
	short 					dig_T2;
	short						dig_T3;
	unsigned short 	dig_P1;
	short 					dig_P2;
	short 					dig_P3;
	short 					dig_P4;
	short 					dig_P5;
	short 					dig_P6;
	short 					dig_P7;
	short 					dig_P8;
	short 					dig_P9;
	char						dig_H1;
	short						dig_H2;
	char						dig_H3;
	short						dig_H4;
	short						dig_H5;
	signed char			dig_H6;
}BME280_StructTypeDef;

char BME280_Init (BME280_StructTypeDef * BME280_Struct);
char BME280_Get_Result (BME280_StructTypeDef * BME280_Struct);
char BME280_Busy (BME280_StructTypeDef * BME280_Struct);
char BME280_Check_ID (BME280_StructTypeDef * BME280_Struct);
