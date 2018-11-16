#include "ILI9341.h"

#define X_SIZE 240
#define Y_SIZE 320

#define MADCTL_MY  0x80  ///< Bottom to top
#define MADCTL_MX  0x40  ///< Right to left
#define MADCTL_MV  0x20  ///< Reverse Mode
#define MADCTL_ML  0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00  ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08  ///< Blue-Green-Red pixel order
#define MADCTL_MH  0x04  ///< LCD refresh right to left

ILI9341_t * ILI9341 = 0;
uint32_t byte_cnt = 0;

ILI9341_t * ILI9341_Init (ILI9341_t * driver)
{
	if (driver)
		ILI9341 = driver;

	const ILI9341_init_t initSeq[] = {
		{ 0, ILI9341_SWRESET, 0 },
		{ 0, ILI9341_DISPOFF, 0 },
		{ 3, 0xCF, "\x00\x83\x30" },
		{ 4, 0xED, "\x64\x03\x12\x81" },
		{ 3, 0xE8, "\x85\x01\x79" },
		{ 5, 0xCB, "\x39\x2C\x00\x34\x02" },
		{ 1, 0xF7, "\x20" },
		{ 2, 0xEA, "\x00\x00" },
		{ 1, ILI9341_PWCTR1, "\x26" },             	// Power control VRH[5:0]
		{ 1, ILI9341_PWCTR2, "\x11" },             	// Power control SAP[2:0];BT[3:0]
		{ 2, ILI9341_VMCTR1, "\x35\x3E" },       		// VCM control
		{ 1, ILI9341_VMCTR2, "\xBE" },             	// VCM control2
		{ 1, ILI9341_MADCTL, "\x48" },             	// Memory Access Control
		{ 1, ILI9341_PIXFMT, "\x55" },
		{ 2, ILI9341_FRMCTR1, "\x00\x1B" },
		{ 1, 0xF2, "\x08" },
		{ 1, ILI9341_GAMMASET, "\x01" },
		{ 15, ILI9341_GMCTRP1, "\x0F\x31\x2B\x0C\x0E\x08\x4E\xF1\x37\x07\x10\x03\x0E\x09\x00" }, // Set Gamma
		{ 15, ILI9341_GMCTRN1, "\x00\x0E\x14\x03\x11\x07\x31\xC1\x48\x08\x0F\x0C\x31\x36\x0F" }, // Set Gamma
		{ 1, 0xB7, "\x07" },
		{ 4, ILI9341_DFUNCTR, "\x0A\x82\x27\x00" },
		{ 0, 0x11, 0 },
		{ 0, 0x29, 0 },
	};
	
	for (uint8_t i = 0; i != sizeof(initSeq) / sizeof(ILI9341_init_t); i++)	
		ILI9341->wr_cmd(initSeq[i].cmd, (void*)&initSeq[i].args[0], initSeq[i].argc);

	return ILI9341;
}

void ILI9341S_Clear (uint16_t color)
{
	ILI9341S_SetWindow(0, 0, X_SIZE, Y_SIZE);
	for (uint32_t i = 0; i != X_SIZE * Y_SIZE; i++)
		ILI9341S_WriteData(color);
}

void ILI9341S_SetWindow (uint16_t x, uint16_t y, uint16_t w, uint16_t h) 
{
	byte_cnt = w * h;
	ILI9341->cs(false);
	
	uint8_t buf[4];
	buf[0] = x >> 8;
  buf[1] = x;
  buf[2] = (x + w - 1) >> 8;
  buf[3] = x + w - 1;
  ILI9341->wr_cmd(ILI9341_CASET, buf, sizeof(buf)); // Column addr set
	
	buf[0] = y >> 8;
  buf[1] = y;
  buf[2] = (y + h - 1) >> 8;
  buf[3] = y + h - 1;	
  ILI9341->wr_cmd(ILI9341_PASET, buf, sizeof(buf)); // Row addr set
  
	ILI9341->wr_cmd(ILI9341_RAMWR, 0, 0); // write to RAM

	ILI9341->cs(true);
}

void ILI9341S_WriteData (uint16_t data)
{
	ILI9341->wr_dat(data);
	byte_cnt--;
	if (byte_cnt == 0) ILI9341->cs(false);
}

ILI9341_ID_t * ILI9341S_GetId (void)
{
	static ILI9341_ID_t id;
	
	if (!ILI9341) return 0;
	
	ILI9341->rd_id(0x0A, &id);
	
	return &id;
}

void ILI9341S_PrintChar (uint16_t StartX, uint16_t StartY, char Ch)
{
	ILI9341S_SetWindow(StartX, StartY, ILI9341->FontStruct->FontXsize, ILI9341->FontStruct->FontYsize);
	for (uint8_t x = 0; x != ILI9341->FontStruct->FontXsize; x++)
		for (uint8_t y = 0; y != ILI9341->FontStruct->FontYsize; y++)
			ILI9341S_WriteData ((ILI9341->FontStruct->Font[Ch * ILI9341->FontStruct->FontXsize + x] & (1 << y)) ? ILI9341->Color : ILI9341->BackColor);
}

uint16_t ILI9341S_PrintText (uint16_t StartX, uint16_t StartY, const char * Text)
{
	uint16_t X = StartX, Row = 0;
	while(* Text)
	{
		if (* Text == '\n')
		{	
			StartX = X;
			StartY += ILI9341->FontStruct->FontYsize;
			Row += 1;
		}
		else
		{
			ILI9341S_PrintChar(StartX, StartY, * Text);
			StartX += ILI9341->FontStruct->FontXsize;			
		}
		Text++;
	}
	return Row;
}

void ILI9341S_Pixel (uint16_t x, uint16_t y, uint16_t color)
{
	ILI9341S_SetWindow(x, y, 1, 1);
	ILI9341S_WriteData(color);
}

void ILI9341S_vLine (uint16_t x0, uint16_t y0, uint16_t y1, uint16_t color)
{
	int16_t dy = y1 > y0 ? y1 - y0 : y0 - y1;
	ILI9341S_SetWindow(x0, y0, 1, dy);
	for (int32_t i = 0; i < dy; i++)
		ILI9341S_WriteData(color);
}

void ILI9341S_Circle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t x = -r, y = 0, err = 2 - 2 * r, e2;
    do {
        ILI9341S_Pixel(x0 - x, y0 + y, color);
        ILI9341S_Pixel(x0 + x, y0 + y, color);
        ILI9341S_Pixel(x0 + x, y0 - y, color);
        ILI9341S_Pixel(x0 - x, y0 - y, color);
        e2 = err;
        if (e2 <= y) {
            err += ++y * 2 + 1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x * 2 + 1;
    } while (x <= 0);
}

void ILI9341S_FillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
    int16_t x = -r, y = 0, err = 2 - 2 * r, e2;
    do {
        ILI9341S_vLine(x0 - x, y0 - y, y0 + y, color);
        ILI9341S_vLine(x0 + x, y0 - y, y0 + y, color);
        e2 = err;
        if (e2 <= y) {
            err += ++y * 2 + 1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x * 2 + 1;
    } while (x <= 0);
}