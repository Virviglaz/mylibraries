#include "DataTypes.h"

/* Init structure */
typedef struct
{
	/* Functions */
	void (*WriteData)(u8 reg, u8 * buf, u16 size);
	void (*ReadData) (u8 reg, u8 * buf, u16 size);

	
}Si7021_InitStructTypeDef;
