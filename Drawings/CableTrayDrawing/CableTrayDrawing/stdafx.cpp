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
	string x = (LPSTR)(pVoid); 
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

/******************************************************************************
    @author     humkyung
    @date       2012-03-15
    @class
    @function   GetInputPointOnMSTNJ
    @return     int
    @param      double&             x
    @param      double&             y
    @param      const               STRING_T&
    @param      sCommandFilePath
    @brief
******************************************************************************/
//int GetInputPointOnMSTNJ(double& x , double& y , const STRING_T& sCommandFilePath)
//{
//	OFSTREAM_T ofile(sCommandFilePath.c_str());
//	if(ofile.is_open())
//	{
//		ofile << _T("MDL LOAD aPickTool \"C:\\TEMP\\aPickTool.dat\" ") << std::endl;
//		ofile.close();
//	}else	return ERROR_BAD_ENVIRONMENT;
//
//	STRING_T sDataFile(_T("C:\\TEMP\\aPickTool.dat"));
//	::DeleteFile(sDataFile.c_str());
//
//	CoInitialize(NULL);
//	_CCallMSTNJPtr pDa;
//	pDa.CreateInstance(__uuidof(CCallMSTNJ));
//	if(NULL != pDa)
//	{
//		pDa->SendCommand(_bstr_t(sCommandFilePath.c_str()));
//		pDa.Detach();
//		CoUninitialize();
//
//		DWORD dwThreadParam = 1 , dwThreadId;
//		HANDLE hThread = CreateThread( NULL , 0 , MonitorFileCreates , (LPVOID)(sDataFile.c_str()) , 0 , &dwThreadId );
//		if( hThread )
//		{
//			WaitForSingleObject( hThread , INFINITE); 
//		}
//
//		IFSTREAM_T ifile(_T("C:\\TEMP\\aPickTool.dat"));
//		if(ifile.is_open())
//		{
//			STRING_T aLine;
//			getline(ifile , aLine);
//
//			vector<STRING_T> oResult;
//			CTokenizer<CIsComma>::Tokenize(oResult , aLine , CIsComma());
//			if(3 == oResult.size())
//			{
//				x = atof(oResult[0].c_str());
//				y = atof(oResult[1].c_str());
//			}
//		}
//
//		return ERROR_SUCCESS;
//	}
//
//	return ERROR_BAD_ENVIRONMENT;
//}