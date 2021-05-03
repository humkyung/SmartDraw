// stdafx.cpp : source file that includes just the standard includes
// aDraw_Tag.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "SmartDrawAnnoModule.h"
// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

/**
	@brief  Get Module Path
	@author	HumKyung.BAEK
	@date	2010.05.29
*/
STRING_T GetExecPath(void)
{
	TCHAR szFileName[MAX_PATH + 1] = {'\0', };
	::GetModuleFileName(NULL , szFileName , MAX_PATH);
	STRING_T rFilePath(szFileName);
	STRING_T::size_type at = rFilePath.find_last_of(_T("\\"));
	if(string::npos != at) rFilePath = rFilePath.substr(0 , at);

	return rFilePath;
}

bool fn_Len_Sort(const CCellMap::SearchRes& lhs, const CCellMap::SearchRes& rhs)
{
	return (lhs.distance < rhs.distance);
}
bool fn_Cnt_Sort(const CCellMap::SearchRes& lhs, const CCellMap::SearchRes& rhs)
{
	return (lhs.occupied < rhs.occupied);
}