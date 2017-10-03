#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char ** ConsoleGetPars (char * src, char * delim);
size_t ConsoleGetParsLen (char ** params);
size_t ConsoleGetParsNum (char * src, char * delim);

static const struct
{
	char ** (* GetPars) (char * src, char * delim);
	size_t (* GetParsLen) (char ** params);
	size_t (* GetParsNum) (char * src, char * delim);
}Console = { ConsoleGetPars, ConsoleGetParsLen, ConsoleGetParsNum };

#endif
