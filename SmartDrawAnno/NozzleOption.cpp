#include "StdAfx.h"
#include <Tokenizer.h>
#include "NozzleOption.h"

using namespace tag_option;

CNozzleOption::CNozzleOption(void)
{
}

CNozzleOption::~CNozzleOption(void)
{
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.07

	@return
*/
int CNozzleOption::Parse(const STRING_T& sIniFilePath)
{
	static const STRING_T sApp(_T("Nozzle"));

	TCHAR szBuf[MAX_PATH] = {'\0',};

	/////////////////////////////////////////////////
	// NOZZLE Options                              //
	///////////////////////////////////////////////// 
	if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("InsideLabel_3") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		Tag = ATOI_T(szBuf);
	}else	Tag = 1;

	if (Tag)  
	{		
		if(GetPrivateProfileString(_T("ANNOTATION_OPTIONS"),_T("NOZ_GapA") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
		{
			Gap = ATOF_T(szBuf);
		}else	Gap = 1.0;

		if(GetPrivateProfileString(_T("ANNOTATION_OPTIONS"),_T("NOZ_Leader") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
		{
			LeaderOnOff = (STRING_T(szBuf) == _T("ON")) ? true : false;
		}else	LeaderOnOff = false;

		///TODO: if (T_opt.Type == 1)
		{
			if(GetPrivateProfileString(_T("ANNOTATION_OPTIONS"),_T("NOZ_PrefixUse") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
			{
				if (STRING_T(szBuf) == _T("Yes")) 
				{
					if(GetPrivateProfileString(_T("ANNOTATION_OPTIONS"),_T("NOZ_LabelPrefix") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
						Txt_Header.assign(szBuf);
					else	Txt_Header = _T("");
				}else	Txt_Header = _T("0");
			}else	Txt_Header = _T("0");
		}///else 	Txt_Header = "0";

		if(GetPrivateProfileString(_T("ANNOTATION_OPTIONS"),_T("NOZ_Display") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
		{
			if (STRING_T(szBuf) == _T("Delimiter_Character"))
			{
				if(GetPrivateProfileString(_T("ANNOTATION_OPTIONS"),_T("NOZ_Delimiter") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
					NozDelimiter.assign(szBuf);
				else	NozDelimiter = _T("0");
			}else	NozDelimiter = _T("0");
		}else	NozDelimiter = _T("0");
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("TextStyle") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oTextStyle.Parse(szBuf , sIniFilePath);
	}
	
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Offset") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_dLabelOffset = ATOF_T(szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLabelProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLabelProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLabelProp.style = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLabelProp.weight = szBuf;
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Type") , _T("CLOSE") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sArrowType = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Size") , _T("1.5") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_dArrowSize = ATOF_T(szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.style = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.weight = szBuf;
	}

	return ERROR_SUCCESS;
}