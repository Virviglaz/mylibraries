#include "strings.h"

void uLongToStr (unsigned long Value, char * String)
{
  char zero = 0;
  char tmp = 0;
  unsigned long cnt = 1000000000;
  while (cnt != 1)
  {
    while (Value >= cnt)
    {
      Value -= cnt;
      tmp++;
    }
    if (tmp) zero = 1;
    if (zero) * String++ = tmp + 48;
    tmp = 0;
    cnt /= 10;
  }
  * String++ = Value + 48;
  * String = 0;
}

void uCharToStr (unsigned char Value, char * String)
{
  char zero = 0;
  char tmp = 0;
  unsigned long cnt = 100;
  while (cnt != 1)
  {
    while (Value >= cnt)
    {
      Value -= cnt;
      tmp++;
    }
    if (tmp) zero = 1;
    if (zero) * String++ = tmp + 48;
    tmp = 0;
    cnt /= 10;
  }
  * String++ = Value + 48;
  * String = 0;
}

void sLongToStr (signed long Value, char * String)
{
	unsigned long tmp;
	if (Value < 0) 
	{
		Value = -Value;		
		*String++ = '-';
	}
        tmp = Value;
        uLongToStr(tmp, String);
}

void sCharToStr (signed char Value, char * String)
{
	unsigned char tmp;
	if (Value < 0) 
	{
		Value = -Value;
		*String++ = '-';
        }
        tmp = Value;
        uCharToStr(tmp, String);
}

void FillString (char * Data, char * String)
{
  while (* Data) * String++=* Data++;
}

void FillStringByLen (char * Data, char * String, unsigned char Len)
{
  while (Len--)	* String++=* Data++;
}

void ClearString (char * String, char Len, char Data)
{
  while (Len--) * String++=Data;
}

void StringMoveLeft (char * String, char Len)
{
        char tmp;
        char dat = String[0];
        char cnt;
        if (Len) 
          tmp = Len - 1;
        else 
          tmp = StringLen(String) - 1;
        
        for (cnt = 0; cnt != tmp; cnt++) 
          String[cnt] = String[cnt + 1];
        
        String[tmp] = dat;
}

void StringMoveRight (char * String, char Len)
{
        char tmp;
        char dat;
        char cnt;
        if (Len) 
          tmp = Len - 1;
        else 
          tmp = StringLen(String) - 1;
        
        dat = String[Len - 1];        
        for (cnt = tmp; cnt != 0; cnt--) 
          String[cnt] = String[cnt - 1];
        String[0] = dat;
}

unsigned char StringLen (char * String)
{
        char cnt = 0;
        while (* String++) cnt++;
        return cnt;
}

void ValueToStringHEX_Byte (unsigned char Value, char * String)
{
   const unsigned char HEX_Var[17]={"0123456789ABCDEF"};
   * String++ =   ((Value & 0xF0) >> 4)[HEX_Var];
   * String   =    (Value & 0x0F)[HEX_Var];
}

void ValueToStringHEX_Word (unsigned int Value, char * String)
{
   const unsigned char HEX_Var[17]={"0123456789ABCDEF"};
   * String++ =   ((Value & 0xF000)>>12)[HEX_Var];
   * String++ =   ((Value & 0x0F00)>>8)[HEX_Var];
   * String++ =   ((Value & 0x00F0)>>4)[HEX_Var];
   * String   =    (Value & 0x000F)[HEX_Var];
}

void ValueToStringHEX_Long (unsigned long Value, char * String)
{
   const unsigned char HEX_Var[17]={"0123456789ABCDEF"};
   * String++ =   ((Value & 0xF0000000)>>28)[HEX_Var];
   * String++ =   ((Value & 0x0F000000)>>24)[HEX_Var];
   * String++ =   ((Value & 0x00F00000)>>20)[HEX_Var];
   * String++ =   ((Value & 0x000F0000)>>16)[HEX_Var];
   * String++ =   ((Value & 0x0000F000)>>12)[HEX_Var];
   * String++ =   ((Value & 0x00000F00)>>8)[HEX_Var];
   * String++ =   ((Value & 0x000000F0)>>4)[HEX_Var];
   * String   =    (Value & 0x0000000F)[HEX_Var];
}

void FloatToString (float Value, char * String, char Accuracy)
{
	signed long k = 1;
	unsigned long tmp; 
	unsigned char Len;
	while (Accuracy--) k *= 10;
        tmp = MOD ((signed long)((Value*(float)k)) - ((signed long)Value)*k);
        if (Value < 0) 
        {
          * String++ = '-';
          Value = -Value;
        }
	uLongToStr((unsigned long)Value, String);
	Len = StringLen(String);
	String[Len++]='.';
	uLongToStr(tmp, String + Len);
}

void StringReplaceChar (char * String, char Char1, char Char2)
{
	char cnt = StringLen(String);
	while (cnt--)
		if (String[cnt] == Char1) String[cnt] = Char2;
}

unsigned long MOD (signed long Value)
{
	if (Value > 0) 
          return Value;
	else 
          return -Value;
}

unsigned char StringsCompareByLen (char * String1, char * String2, unsigned int Len)
{
	unsigned int Diff = 0;
	while(Len--)
		if (*String1++ != *String2++) Diff++;
	return Diff;
}

unsigned char DecToBCD (unsigned char Value)
{
  return ((Value / 10) << 4) | (Value % 10);
}

unsigned char BcdToDec (unsigned char Value)
{
  return ((Value >> 4) * 10) + (Value & 0x0F);
}

unsigned char SearchForString (char * StringToSearch, 
                               unsigned char StringToSearchLen, 
                               char * TextString,
                               unsigned char TextStringLen)
{
  unsigned char Res;
  for (Res = 0; Res != (TextStringLen - StringToSearchLen); Res++)
  {
    if (StringsCompareByLen(StringToSearch, TextString + Res, StringToSearchLen) == 0)
      return Res;
  }
  return 0xFF;
}

void AddString (char * Source, char * Target)
{
	while(*Source)	Source++;
	while(*Target)	*Source++ = *Target++;
}

char ByteParse (char * buf, char pos)
{
	const char ASCII_Table[] = {0,1,2,3,4,5,6,7,8,9,':',';','<','=','>','?','@',10,11,12,13,14,15};
	return (ASCII_Table[(buf[pos] - '0')] << 4) | ASCII_Table[buf[pos + 1] - '0'];
}

unsigned int uIntParse (char * buf, char pos)
{
	return (ByteParse(buf, pos) << 8) | ByteParse(buf, pos + 2);
}

void GetFirstParamNullT (char * Source, char * OutParam, char Separator)
{
	char cnt = 0;
	while (* Source != Separator && * Source)
	{
		cnt++;
		Source++;
	}
	if (*Source == 0) return; //no params found
	while (*Source++ == Separator); //skip spaces
	while (*Source) //copy
		*OutParam = *Source++;
}

char FindFirstParamNullT (char * Source, char Separator)
{
	char cnt = 1;
	while (* Source != Separator && * Source && * Source != 0x0A)
	{
		cnt++;
		Source++;
	}
	if (*Source == 0) return 0; //no params found
	while (*Source++ == Separator); //skip spaces
	return cnt;
}

char SearchForStringNullT (char * Str, const char * Source)
{
	while (*Source == *Str && *Str && *Source)
	{
		Source++;
		Str++;
	}
	if (*Source) return 0;
	return 1;
}

void StringSmallLetters (char * Str, char size)
{
	char cnt;
	for (cnt = 0; cnt < size; cnt++)
		if (Str[cnt] >= 'A' && Str[cnt] <= 'Z')
			Str[cnt] += ('a' - 'A');
}
