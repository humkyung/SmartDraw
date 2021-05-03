// hiddenDllTestSuit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include "../EnumString.h"

#ifdef	UNICODE
	#ifdef	_DEBUG
		#pragma comment(lib, "SmartHSR_vc90_unicode_debug")
	#else
		#pragma comment(lib, "SmartHSR_vc90_unicode")
	#endif
#else
	#ifdef	_DEBUG
		#pragma comment(lib, "SmartHSR_vc90_debug")
	#else
		#pragma comment(lib, "SmartHSR_vc90")
	#endif
#endif
extern "C" __declspec(dllimport)
void __stdcall SmartHSR(const TCHAR* pHsrFilePath , const TCHAR* pDraFilePath , const TCHAR* pIniFilePath , const TCHAR* pSxfFolderPath);

enum WeekEnd
{
	Sunday = 1,
    Saturday = 7
};

// String support for WeekEnd
Begin_Enum_String( WeekEnd )
{
	Enum_String( Sunday );
	Enum_String( Saturday );
}
End_Enum_String;

int main(int argc, char* argv[])
{
	// Convert from WeekEnd to string
    const std::string &str = EnumString<WeekEnd>::From( Saturday );
	// str should now be "Saturday"

	/*SmartHSR(
		"e:\\PROJECT\\SolutionWare\\Projects\\2012.01.SmartDraw\\SmartDraw_PDS\\Data\\bpa4 Project Opening\\Working\\100S-S03.hsr" , 
		"e:\\PROJECT\\SolutionWare\\Projects\\2012.01.SmartDraw\\SmartDraw_PDS\\Data\\bpa4 Project Opening\\Dra\\100S-S03.dra" ,
		"e:\\PROJECT\\SolutionWare\\Projects\\2012.01.SmartDraw\\SmartDraw_PDS\\Data\\bpa4 Project Opening\\Setting\\Default.ini" , 
		"e:\\PROJECT\\SolutionWare\\Projects\\2012.01.SmartDraw\\SmartDraw_PDS\\Data\\bpa4 Project Opening\\Model");*/

	/*SmartHSR(
		_T("d:\\Projects\\SmartDraw\\SmartDraw_PDS\\Data\\JPP\\Working\\p57-a05-p2.hsr") , 
		_T("d:\\Projects\\SmartDraw\\SmartDraw_PDS\\Data\\JPP\\Dra\\p57-a05-p2.dra") ,
		_T("d:\\Projects\\SmartDraw\\SmartDraw_PDS\\Data\\JPP\\Setting\\Default.ini") , 
		_T("d:\\Projects\\SmartDraw\\SmartDraw_PDS\\Data\\JPP\\Model"));*/

	return 0;
}
