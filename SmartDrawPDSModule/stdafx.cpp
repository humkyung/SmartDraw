// stdafx.cpp : source file that includes just the standard includes
// SmartDrawPDSModule.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


/**
	@brief  Get Module Path   
	@author	HumKyung.BAEK
	@date	2010.05.07
*/
CString GetExecPath(void)
{
	TCHAR szFileName[MAX_PATH + 1] = {'\0', };
	GetModuleFileName(NULL , szFileName , MAX_PATH);
	CString rFilePath = szFileName;
	int at = rFilePath.ReverseFind('\\');
	if(-1 != at) rFilePath.Left(at);
	CString rModulePath = rFilePath.Left(at);

	return rModulePath;
}
