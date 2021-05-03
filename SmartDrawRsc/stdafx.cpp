// stdafx.cpp : source file that includes just the standard includes
// SmartDrawRsc.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


/******************************************************************************
    @author     humkyung
    @date       2012-03-12
    @class
    @function   GetExecPath
    @return     CString
    @param      void
    @brief
******************************************************************************/
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