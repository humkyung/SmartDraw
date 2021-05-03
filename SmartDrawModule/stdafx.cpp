// stdafx.cpp : source file that includes just the standard includes
// aDraw_Module.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <comutil.h>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

STRING_T NewGuid()
{
	GUID guid;
	CoCreateGuid(&guid); 
	OLECHAR wszBuff[39];
	StringFromGUID2(guid, wszBuff, 39);
	char out[39]; 
	wcstombs(out, wszBuff, 39);
	return STRING_T(out).substr(1,36);
}