#include "http.h"
#include <string.h>

static const char std_header_Windows1251[] = "GET/HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=Windows-1251\r\n\r\n";	


void http_add_std_header (char * text, char * buf)
{
	strcpy(buf, std_header_Windows1251);
	strcpy(--buf + sizeof(std_header_Windows1251), text);
}
