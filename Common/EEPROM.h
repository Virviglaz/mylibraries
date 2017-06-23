typedef struct
{
	/* Data */
	int Mem_adrs;
	char * buf;
	int size;
	
	/* Functions */
	char (*WritePage)(char I2C_Adrs, char * MemPos, char MemPosSize, char * buf, char size);
	char (*ReadPage) (char I2C_Adrs, char * MemPos, char MemPosSize, char * buf, char size);	
	void (*delay_func)(unsigned int ms);
	
	/* Settings */
	char I2C_Adrs;
	char PageSize;
	char PageWriteTime;
	char isWriting;
}EEPROM_StructTypeDef;

char EEPROM_RW (EEPROM_StructTypeDef * EEPROM);
