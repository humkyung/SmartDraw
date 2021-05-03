// UnitTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../GenerateDraFromDrawing.h"
#include "../SmartDrawPDSModuleImpl.h"

int _tmain(int argc, _TCHAR* argv[])
{
	/*CGenerateDraFromDrawing generator;
	generator.Generate(
		_T("e:\\DATA\\PDS\\yer4\\Cable_Route\\VAD\\sec_112c.dra") , 
		_T("e:\\DATA\\PDS\\yer4\\Cable_Route\\Hidden\\sec_112c.dgn") , 
		_T("e:\\DATA\\PDS\\yer4\\Cable_Route\\DBtext\\PDS_Data_Dump.mdb"));*/

	if(4 == argc)
	{
		CSmartDrawPDSModuleImpl oImpl;
		oImpl.WriteModelToXml(argv[1] , argv[2] , ATOI_T(argv[3]));
	}
	else if(3 == argc)
	{
		CSmartDrawPDSModuleImpl oImpl;
		oImpl.WriteModelToDump(argv[1] , argv[2] , 1);
	}

	return 0;
}

