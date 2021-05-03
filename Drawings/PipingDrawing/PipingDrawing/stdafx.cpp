// stdafx.cpp : source file that includes just the standard includes
// OpeningHoleDrawing.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <windows.h>

#include <Tokenizer.h>

#include <fstream>
#include <vector>
#include <string>
using namespace std;

#define	KEY_DOWN(vk_code) ((::GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

static DWORD WINAPI MonitorFileCreates(LPVOID pVoid)
{
	STRING_T x = (LPTSTR)(pVoid); 
	SHFILEINFO sfi;
	do
	{
		if(0 != SHGetFileInfo(x.c_str() , FILE_ATTRIBUTE_NORMAL , &sfi , sizeof(sfi) , SHGFI_ATTRIBUTES) )
		{
			break;
		}
		::Sleep( 100 );
		if(KEY_DOWN(VK_ESCAPE)) break;
	}while(1);
	return 0;
}