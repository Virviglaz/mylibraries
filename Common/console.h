#ifndef CONSOLE_H
#define CONSOLE_H

char GetParams (char * Source, char ** Dist, char BreakChar, 
                void * memalloc (char len), void * memrelease (char * p));
char GetParamsNum (char * Source, char BreakChar);
char GetParamsSize (char * Source, char * Dist, char BreakChar);
void ReleaseMemory (char ParamNum, char ** Dist, void * memrelease (char * p));

#endif
