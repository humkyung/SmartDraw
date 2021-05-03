// SmartExchanger.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <DGNLib.h>

#include "SmartExchanger.h"
#include "verinfo.h"
#include "VersionNo.h"

#include <list>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

HANDLE hInst=NULL;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	hInst = hModule;
	return TRUE;
}

double g_nUOR=1.;


#ifndef SMARTEXCHANGER_EXPORTS
	#define DLL_EXPORT __declspec(dllexport)
#endif
#ifndef SMARTEXCHANGER_EXPORTS
	#define DLL_IMPORT __declspec(dllimport)
#endif

string GetFileSize(LPCTSTR lpFilePath)
{
	string res;

	FILE* fp = fopen(lpFilePath , "rb");
	if(NULL != fp)
	{
		fseek(fp , 0 , SEEK_END);
		const long lSize = ftell(fp);
		fclose(fp);

		ostringstream oss;
		oss << lSize;
		res = oss.str();
	}
	//// CreateFile에 GENERIC_READ 옵션을 사용하여 ReadMe.txt파일을 연다.
	//HANDLE h_file = CreateFile(lpFilePath , GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//if(h_file != INVALID_HANDLE_VALUE)
	//{
	//	FILETIME create_time, access_time, write_time;

	//	// 지정된 파일에서 파일의 생성, 최근 사용 그리고 최근 갱신된 시간을 얻는다.
	//	GetFileTime(h_file, &create_time, &access_time, &write_time);

	//	SYSTEMTIME write_system_time, write_local_time;
	//	// FILETIME 구조체정보를 SYSTEMTIME 구조체 정보로 변환한다. FILETIME 구조체에 들어있는 정보는
	//	// 우리가 직접적으로 이용하려면 계산이 복잡해지기 때문에 사용하기 편리한 SYSTEMTIME 구조체
	//	// 형식으로 변환해서 사용한다.
	//	FileTimeToSystemTime(&write_time, &write_system_time);

	//	// FILETIME 구조체에서 SYSTEMTIME 구조체로 변환되면 시간정보가 UTC(Universal Time Coordinated) 형식의 
	//	// 시간이기 때문에 탐색기나 기타 프로그램에서 보는 시간과 다르다. 따라서 정확한 시간정보를 얻기 위해서
	//	// UTC 형식의 시간을 지역시간으로 변환해야 한다. 아래의 함수는 해당 작업을 하는 함수이다.
	//	SystemTimeToTzSpecificLocalTime(NULL, &write_system_time, &write_local_time);

	//	// write_local_time 을 사용하면 된다..
	//	ostringstream oss;
	//	oss << write_local_time.wYear << "-" << write_local_time.wMonth << "-" << write_local_time.wDay << " ";
	//	oss << write_local_time.wHour << ":" << write_local_time.wMinute;
	//	res = oss.str();

	//	CloseHandle(h_file);
	//}

	return res;
}

/**	
	@brief	convert .dgn to .nsq

	@author	humkyung

	@param	pExportFilePath	a parameter of type const char*
	@param	pImportFilePath	a parameter of type const char*
	@param	pIniFilePath 환경 설정 파일 경로

	@return	void	
*/
extern "C" DLL_EXPORT
void __stdcall Dgn2Sxf(const TCHAR* pExportFilePath , const int& iModelType , const TCHAR* pImportFilePath , const TCHAR* pIniFilePath , const double& dOffsetX , const double& dOffsetY , const double& dOffsetZ )
{
	assert(pExportFilePath && "pExportFilePath is NULL");
	assert(pImportFilePath && "pImportFilePath is NULL");
	assert(pIniFilePath && "pIniFilePath is NULL");
	static const string rApp("HLR_OPTIONS");

	if(pExportFilePath && pImportFilePath && pIniFilePath)
	{
		int ver[4]={0,};
		sscanf(STRFILEVER , _T("%d,%d,%d,%d") , &ver[0] , &ver[1] , &ver[2], &ver[3]);
		ostringstream oss;
		oss << ver[0] << "," << ver[1] << "," << ver[2];
		string sVer = oss.str();

		string sLastWriteTime = GetFileSize(pImportFilePath);

		/// check version and date
		ifstream ifile(pExportFilePath);
		if(ifile.is_open())
		{
			string sLastVer , sLastDate;
			getline(ifile , sLastVer);
			getline(ifile , sLastDate);
			ifile.close();

			if((string("#" + sVer) == sLastVer) && (string("#" + sLastWriteTime) == sLastDate)) return;
		}
		/// up to here

		CDGNLib* pDGNLib=CDGNLib::GetInstance();
		if(pDGNLib && pDGNLib->Load(pImportFilePath))
		{
			CDGNDoc* pDoc=pDGNLib->GetDoc();
			if(pDoc)
			{
				PSQPRIMITIVE pHead=pDoc->GetHead();

				TCHAR szBuf[MAX_PATH + 1] = {'\0',};
				int nSkipLevel = 0;
				if(GetPrivateProfileString(rApp.c_str() , _T("InsulationLevel") , _T("") , szBuf , MAX_PATH , pIniFilePath))
				{
					nSkipLevel = atoi(szBuf);
				}
				if(GetPrivateProfileString(rApp.c_str() , _T("InsulationDisplay") , _T("") , szBuf , MAX_PATH , pIniFilePath))
				{
					if(string("Remove") != szBuf) nSkipLevel = 0;
				}

				SxfExportFile(pExportFilePath , iModelType , pHead , nSkipLevel , sVer , sLastWriteTime , dOffsetX , dOffsetY , dOffsetZ);
			}
		}
	}
}