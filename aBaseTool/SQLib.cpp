#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "SQLib.h"

#define _9SQDEBUG

const char* format(const char* fmt,...){
#ifndef MAX_PATH
	#define MAX_PATH 256
#endif
	static char buffer[MAX_PATH];
	va_list arg;
	
	va_start(arg,fmt);
	vsprintf(buffer,fmt,arg);
	va_end(arg);
	
	return buffer;
}

