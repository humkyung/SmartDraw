#include "StdAfx.h"
#include "SupportOption.h"

using namespace tag_option;

CSupportOption::CSupportOption(void)
{
	m_dLabelOffset = 0.0;
}

CSupportOption::~CSupportOption(void)
{
}

/**	
	@brief	read support settings
	@author	HumKyung.BAEK
	@date	2010.05.27
	@param	
	@return
*/
int CSupportOption::Parse(const STRING_T& sIniFilePath)
{
	static const STRING_T sApp(_T("Support"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	/// 2014.12.31 added by humkyung
	GetPrivateProfileString(sApp.c_str() , _T("DisplayName") ,_T("pipe_support_no") ,szBuf,MAX_PATH , sIniFilePath.c_str());
	m_sDisplayName.assign(szBuf);
	/// up to here

	m_sDisplayType = _T("Name+Elevation");
	if(GetPrivateProfileString(sApp.c_str() , _T("DisplayType") ,NULL,szBuf,MAX_PATH , sIniFilePath.c_str()))
	{
		m_sDisplayType = szBuf;
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("EnclosureType") ,_T("Rectangle"),szBuf,MAX_PATH , sIniFilePath.c_str()))
	{
		if(0 == STRICMP_T(szBuf , _T("None")))
		{
			m_iEnclosureType = CaDraw_EntTagShape::NONE;
		}
		else if(0 == STRICMP_T(szBuf , _T("Rectangle")))
		{ 
			m_iEnclosureType = CaDraw_EntTagShape::RECTANGLE;
		}
	}else	m_iEnclosureType = CaDraw_EntTagShape::RECTANGLE;

	
	if(GetPrivateProfileString(sApp.c_str() , _T("TextStyle") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oTextStyle.Parse(szBuf , sIniFilePath);
	}	

	m_dLabelOffset = 0.5;
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Offset") ,NULL,szBuf,MAX_PATH,sIniFilePath.c_str()))
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

	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Type") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sArrowType = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Size") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
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

/**	
	@brief	return display name for support
	@author	humkyung
	@date	2012.12.31
	@param	
	@return
*/
CSupportOption::eDisplayName CSupportOption::GetDisplayName() const
{
	if(_T("details_for_shop") == m_sDisplayName) return CSupportOption::SHOP_DETAIL;
	if(_T("details_for_field") == m_sDisplayName) return CSupportOption::FIELD_DETAIL;
	if(_T("commodity_code") == m_sDisplayName) return CSupportOption::COMMODITY_CODE;
	return CSupportOption::SUPPORT_NO;
}