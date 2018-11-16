#include "ST7735.h"

#define ST7735S_TFTWIDTH  		128
#define ST7735S_TFTHEIGHT 		160
#define ST7735S_NOP     			0x00
#define ST7735S_SWRESET 			0x01
#define ST7735S_RDDID   			0x04
#define ST7735S_RDDST   			0x09
#define ST7735S_SLPIN   			0x10
#define ST7735S_SLPOUT  			0x11
#define ST7735S_PTLON   			0x12
#define ST7735S_NORON   			0x13
#define ST7735S_RDMODE  			0x0A
#define ST7735S_RDMADCTL  		0x0B
#define ST7735S_RDPIXFMT  		0x0C
#define ST7735S_RDIMGFMT  		0x0A
#define ST7735S_RDSELFDIAG  	0x0F
#define ST7735S_INVOFF  			0x20
#define ST7735S_INVON   			0x21
#define ST7735S_GAMMASET 			0x26
#define ST7735S_DISPOFF 			0x28
#define ST7735S_DISPON  			0x29
#define ST7735S_CASET   			0x2A
#define ST7735S_PASET   			0x2B
#define ST7735S_RAMWR   			0x2C
#define ST7735S_RAMRD   			0x2E
#define ST7735S_PTLAR   			0x30
#define ST7735S_SCRLAR				0x33
#define ST7735S_MADCTL  			0x36
#define ST7732S_VSCSAD				0x37
#define ST7735S_MADCTL_MY  		0x80
#define ST7735S_MADCTL_MX  		0x40
#define ST7735S_MADCTL_MV  		0x20
#define ST7735S_MADCTL_ML  		0x10
#define ST7735S_MADCTL_RGB 		0x00
#define ST7735S_MADCTL_BGR 		0x08
#define ST7735S_MADCTL_MH  		0x04
#define ST7735S_PIXFMT  			0x3A
#define ST7735S_FRMCTR1 			0xB1
#define ST7735S_FRMCTR2 			0xB2
#define ST7735S_FRMCTR3 			0xB3
#define ST7735S_INVCTR  			0xB4
#define ST7735S_DFUNCTR 			0xB6
#define ST7735S_PWCTR1  			0xC0
#define ST7735S_PWCTR2  			0xC1
#define ST7735S_PWCTR3  			0xC2
#define ST7735S_PWCTR4  			0xC3
#define ST7735S_PWCTR5  			0xC4
#define ST7735S_VMCTR1  			0xC5
#define ST7735S_VMCTR2  			0xC7
#define ST7735S_RDID1   			0xDA
#define ST7735S_RDID2   			0xDB
#define ST7735S_RDID3   			0xDC
#define ST7735S_RDID4   			0xDD
#define ST7735S_GMCTRP1 			0xE0
#define ST7735S_GMCTRN1 			0xE1

ST7735_t * ST7735 = 0;

static void st7735_writeC (uint8_t cmd);
static void st7735_writeD (uint8_t dat);

ST7735_t * ST7735_Init (ST7735_t * driver)
{
	if (driver)
		ST7735 = driver;
	
	const uint8_t LCD_Init_Seq1[] = { 0x39, 0x2C, 0x00, 0x34, 0x02 };
	const uint8_t LCD_Init_GMCTRP1[] = { 0x0F,0x31,0x2B,0x0C,0x0E,0x08,0x4E,0xF1,0x37,0x07,0x10,0x03,0x0E,0x09,0x00 };
	const uint8_t LCD_Init_GMCTRN1[] = { 0x00,0x0E,0x14,0x03,0x11,0x07,0x31,0xC1,0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F };

	st7735_writeC(0xCB);
	ST7735->wr((void*)LCD_Init_Seq1, sizeof(LCD_Init_Seq1), eDAT);
	
  st7735_writeC(0xF7);  
  st7735_writeD(0x20); 

  st7735_writeC(0xEA);  
  st7735_writeD(0x00); 
  st7735_writeD(0x00); 
	
	st7735_writeC(ST7735S_PWCTR1);    //Power control 
  st7735_writeD(0x23);   //VRH[5:0] 
 
  st7735_writeC(ST7735S_PWCTR2);    //Power control 
  st7735_writeD(0x10);   //SAP[2:0];BT[3:0] 
 
  st7735_writeC(ST7735S_VMCTR1);    //VCM control 
  st7735_writeD(0x3e); 
  st7735_writeD(0x28); 
  
  st7735_writeC(ST7735S_VMCTR2);    //VCM control2 
  st7735_writeD(0x86);  
 
  st7735_writeC(ST7735S_MADCTL);    // Memory Access Control 
  st7735_writeD(ST7735S_MADCTL_MX | ST7735S_MADCTL_MY | ST7735S_MADCTL_BGR);

  st7735_writeC(ST7735S_PIXFMT);    
  st7735_writeD(0x55); 
  
  st7735_writeC(ST7735S_FRMCTR1);    
  st7735_writeD(0x00);  
  st7735_writeD(0x18); 
 
  st7735_writeC(ST7735S_DFUNCTR);    // Display Function Control 
  st7735_writeD(0x08); 
  st7735_writeD(0x82);
  st7735_writeD(0x27);
	
	ST7735S_DefineScrollArea(16, 0);
	
	st7735_writeC(0xF2);    // 3Gamma Function Disable 
  st7735_writeD(0x00); 
 
  st7735_writeC(ST7735S_GAMMASET);    //Gamma curve selected 
  st7735_writeD(0x01); 
	
	st7735_writeC(ST7735S_GMCTRP1);    //Set Gamma 
	ST7735->wr((void*)LCD_Init_GMCTRP1, sizeof(LCD_Init_GMCTRP1), eDAT);
  
  st7735_writeC(ST7735S_GMCTRN1);    //Set Gamma
	ST7735->wr((void*)LCD_Init_GMCTRN1, sizeof(LCD_Init_GMCTRN1), eDAT);	
	
	st7735_writeC(ST7735S_SLPOUT);    //Exit Sleep 
	
  st7735_writeC(ST7735S_DISPON);    //Display on 

	return ST7735;
}

void ST7735S_DefineScrollArea (uint8_t top, uint8_t bottom)
{
	st7735_writeC(ST7735S_SCRLAR); 	//Vertical scroll
	st7735_writeD(0);	//BOTTOM area size
	st7735_writeD(bottom);
	st7735_writeD(0);	//Scroll area size
	st7735_writeD(ST7735S_TFTHEIGHT - top - bottom);
	st7735_writeD(0);	//TOP area size
	st7735_writeD(top);	
}

ST7735_ID_t * ST7735S_GetId (void)
{
	static ST7735_ID_t id;
	
	if (!ST7735) return 0;
	
	id.cmd = 0xD3;
	
	ST7735->rd((void*)&id, sizeof(id), eCMD);
	
	return &id;
}

static void st7735_writeC (uint8_t cmd)
{
	ST7735->wr(&cmd, sizeof(cmd), eCMD);
}

static void st7735_writeD (uint8_t dat)
{
	ST7735->wr(&dat, sizeof(dat), eDAT);
}
