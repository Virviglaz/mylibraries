#include "console.h"

char ** ConsoleGetPars (char * src, char * delim)
{
	char ** result; //pointer to pointer value that will be returned as the result
	char * token;
	uint8_t i = 0;

	/* Calculate amount of memory need to store params pointers */
	token = src; //store pointer position
	while(*src)
		if (strcmp(src++, delim) == 0)
			i++;
	src = token; //restore pointer position (reusing variable)
		
	/* Get memory for array of pointers */
	result = malloc((i + 2) * sizeof(char *));
		
	result[i + 1] = NULL; //null terminate last pointer
	i = 0;	//reusing counter variable
		
	/* Find first occurance */	
	token = strtok(src, delim);
		
	/* While possible, assing the pointer values */	
	while (token)
	{
		result[i++] = token; //store pointer position
		token = strtok(NULL, delim); //get new occurance if possible
	}
	
	return result;
}

size_t ConsoleGetParsLen (char ** params)
{
	size_t i = 0;
	while (params[i++]);
	return i - 1;
}

size_t ConsoleGetParsNum (char * src, char * delim)
{
	size_t i = 0;

	while(*src)
		if (strcmp(src++, delim) == 0)
			i++;

	return i;
}

