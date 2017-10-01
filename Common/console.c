#include "console.h"
#include <stdio.h>
#include <stdlib.h>

char ** GetParamsFromString (char * src, char delim)
{
	char ** result; //pointer to pointer value that will be returned as the result
	char * token;
	char delimstring[] = {delim, 0};
	uint8_t i = 0;

	/* Calculate amount of memory need to store params pointers */
	token = src; //store pointer position
	while(*src)
		if (*src++ == delim)
			i++;
	src = token; //restore pointer position (reusing variable)
		
	/* Get memory for array of pointers */
	result = malloc(i + 2);
		
	result[i + 1] = NULL; //null terminate last pointer
	i = 0;	//reusing counter variable
		
	/* Find first occurance */	
	token = strtok(src, delimstring);
		
	/* While possible, assing the pointer values */	
	while (token)
	{
		result[i++] = token; //store pointer position
		token = strtok(NULL, delimstring); //get new occurance if possible
	}
	
	return result;
}

size_t GetParamsFromStringLen (char ** params)
{
	size_t i = 0;
	while (params[i++]);
	return i - 1;
}

