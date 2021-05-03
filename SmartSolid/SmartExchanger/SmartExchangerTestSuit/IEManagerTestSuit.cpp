// DGNLibTestSuit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include "..\SmartExchanger.h"

extern "C" __declspec(dllimport)
void __stdcall Dgn2Sxf(const TCHAR* pExportFilePath , const int& iModelType , const TCHAR* pImportFilePath , const TCHAR* pIniFilePath , const double&,const double&,const double&);

int main(int argc, char* argv[])
{
	double x=0,y=0,z=0;
	Dgn2Sxf(argv[1] , 3 , argv[2] , argv[3],x,y,z);
	return 0;
}
