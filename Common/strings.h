#ifndef STRINGS_H
#define STRINGS_H

#include <stdint.h>

void uLongToStr (unsigned long Value, char * String);
void uCharToStr (unsigned char Value, char * String);
void sLongToStr (signed long Value, char * String);
void sCharToStr (signed char Value, char * String);
void FillString (char * Data, char * String);
void FillStringByLen (char * Data, char * String, unsigned char Len);
void ClearString (char * String, char Len, char Data);
void StringMoveLeft (char * String, char Len);
void StringMoveRight (char * String, char Len);
unsigned char StringLen (char * String);
void ValueToStringHEX_Byte (unsigned char Value, char * String);
void ValueToStringHEX_Word (unsigned int Value, char * String);
void ValueToStringHEX_Long (unsigned long Value, char * String);
void FloatToString (float Value, char * String, char Accuracy);
void StringReplaceChar (char * String, char Char1, char Char2);
unsigned long MOD (signed long Value);
unsigned char StringsCompareByLen (char * String1, char * String2, unsigned int Len);
unsigned char DecToBCD (unsigned char Value);
unsigned char BcdToDec (unsigned char Value);
unsigned char SearchForString (char * StringToSearch, 
                               unsigned char StringToSearchLen, 
                               char * TextString,
                               unsigned char TextStringLen);
void AddString (char * Source, char * Target);
char ByteParse (char * buf, char pos);
unsigned int uIntParse (char * buf, char pos);
void GetFirstParamNullT (char * Source, char * OutParam, char Separator);
char FindFirstParamNullT (char * Source, char Separator);
char SearchForStringNullT (char * Str, const char * Source);
void StringSmallLetters (char * Str, char size);
uint8_t chartohex (char ch);
uint32_t atoh (char * str);
uint8_t getparams (char * source, char delim, char ** buffer, const uint8_t size);

#endif
