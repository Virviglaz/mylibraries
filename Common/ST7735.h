#ifndef ST7735_H
#define ST7735_H

#include <stdint.h>

typedef enum
{
	eCMD = 0,
	eDAT
}ST7735_DataTypeEnum;

typedef struct
{
	uint8_t cmd;
	uint8_t dummy;
	uint8_t module_manufacture_id;
	uint8_t driver_version_id;
	uint8_t driver_id;
}ST7735_ID_t;

typedef struct
{
	void (* wr) (uint8_t * data, uint16_t len, ST7735_DataTypeEnum eData);
	void (* rd) (uint8_t * data, uint16_t len, ST7735_DataTypeEnum eData);
}ST7735_t;

ST7735_t * ST7735_Init (ST7735_t * driver);
void ST7735S_DefineScrollArea (uint8_t top, uint8_t bottom);
ST7735_ID_t * ST7735S_GetId (void);

#endif
