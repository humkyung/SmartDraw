#include "StdAfx.h"
#include <IsTools.h>
#include <Tokenizer.h>
#include "ReducerOption.h"

#include <vector>
using namespace tag_option;

CReducerOption::CReducerOption(void)
{
}

CReducerOption::~CReducerOption(void)
{
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.07

	@return
*/
int CReducerOption::Parse(const string& rIniFilePath)
{
	TCHAR szBuf[MAX_PATH] = {'\0',};
	
	/////////////////////////////////////////////////
	// REDUCER Options                             //
	/////////////////////////////////////////////////
	if(GetPrivateProfileString(("GENERATION_OPTIONS") , ("InsideLabel_4") , NULL , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		Tag = atoi(szBuf);
	}else	Tag = 1;

	if(GetPrivateProfileString("ANNOTATION_OPTIONS" , "RED_GapA" , NULL , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		Gap = atof(szBuf);
	}else	Gap = 1.0;

	if(GetPrivateProfileString("ANNOTATION_OPTIONS" , "RED_LeaderColor" , NULL , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		Leader_Color.assign(szBuf);
	}else	Leader_Color = ("0");

	if(GetPrivateProfileString("ANNOTATION_OPTIONS" , "RED_TextStyle" , NULL , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		m_TextStyle.m_rName.assign(szBuf);
	}else	m_TextStyle.m_rName.assign("Standard");
	m_TextStyle.Parse(rIniFilePath);

	if(GetPrivateProfileString("ANNOTATION_OPTIONS" , "RED_TextColor" , NULL , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		Txt_Color.assign(szBuf);
	}else	Txt_Color = "0";
	
	if(GetPrivateProfileString("ANNOTATION_OPTIONS" , "RED_Arrow" , NULL , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		Arr_Type = atoi(szBuf) + 1;
	}else	Arr_Type = 1;

	return ERROR_SUCCESS;
}