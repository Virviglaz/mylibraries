#include "console.h"
#include <stdio.h>
#include <stdlib.h>

char GetParams (char * Source, char ** Dist, char BreakChar, 
                void * memalloc (char len), void * memrelease (char * p))
{
    char i, j, k = 0;
    char * ParamsSizes;

    //get number of params in string separeted by BreakChar
    char n = GetParamsNum(Source, BreakChar);
    
    //reserve memory for pointers
    ParamsSizes = memalloc (n);
    GetParamsSize(Source, ParamsSizes, BreakChar);

    * Dist = memalloc (n);

    for (i = 0; i < n; i ++)
    {
        Dist[i] = memalloc (ParamsSizes[i] + 1);
        
        for (j = 0; j < ParamsSizes[i]; j++)
            Dist[i][j] = Source[k++];
        k++;
        
        Dist[i][j + 1] = 0;
    }
    
    memrelease(ParamsSizes);
    
    return n;
}

char GetParamsNum (char * Source, char BreakChar)
{
    char n = 1;
    while (* Source)
        if (* Source++ == BreakChar)  n++;  
    return n;
}

char GetParamsSize (char * Source, char * Dist, char BreakChar)
{
    char i, n = 0;
    while (* Source)
    {
        i = 0;
        while (* Source++ != BreakChar && * Source) i++;
        if (* Source) 
            Dist[n] = i;
        else 
            Dist[n] = i + 1;
        n++;
    }
    return n;
}

void ReleaseMemory (char ParamNum, char ** Dist, void * memrelease (char * p))
{
  while(ParamNum)
    memrelease (Dist[ParamNum--]);

  memrelease (* Dist);
}

