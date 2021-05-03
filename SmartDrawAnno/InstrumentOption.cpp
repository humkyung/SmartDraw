#include "StdAfx.h"
#include "InstrumentOption.h"

using namespace tag_option;

CInstrumentOption::CInstrumentOption(void)
{
	m_dEnclosureSizeA = m_dEnclosureSizeB = 0.0;
}

CInstrumentOption::~CInstrumentOption(void)
{
}

/**
	@brief	parse instrument option

	@author	HumKyung.BAEK

	@date	2010.06.12

	@return
*/
int CInstrumentOption::Parse(const STRING_T& sIniFilePath)
{
	static const STRING_T sApp(_T("Instrument"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	if(GetPrivateProfileString(sApp.c_str() , _T("EnclosureType") ,NULL,szBuf,MAX_PATH , sIniFilePath.c_str()))
	{
		if(0 == STRICMP_T(szBuf , _T("Circle")))
		{
			m_iEnclosureType = CaDraw_EntTagShape::CIRCLE;
		}
		else if (0 == STRICMP_T(szBuf , _T("Diamond")))
		{ 
			m_iEnclosureType = CaDraw_EntTagShape::DIAMOND;
		}
		else if(0 == STRICMP_T(szBuf , _T("Rectangle")))
		{ 
			m_iEnclosureType = CaDraw_EntTagShape::RECTANGLE;
		}
		else if (0 == STRICMP_T(szBuf , _T("Ellipse")))
		{ 
			m_iEnclosureType = CaDraw_EntTagShape::ELLIPSE;
		}
		else if (0 == STRICMP_T(szBuf , _T("Hexagon")))
		{ 
			m_iEnclosureType = CaDraw_EntTagShape::HEXAGON;
		}
		else if (0 == STRICMP_T(szBuf , _T("Octagon")))
		{ 
			m_iEnclosureType = CaDraw_EntTagShape::OCTAGON;
		}
		else
		{
			m_iEnclosureType = CaDraw_EntTagShape::CIRCLE;
		}
	}else	m_iEnclosureType = CaDraw_EntTagShape::CIRCLE;

	if(GetPrivateProfileString(sApp.c_str() , _T("Enclosure_SizeA") , NULL , szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_dEnclosureSizeA = ATOF_T(szBuf);
	}
		
	if ((2 == m_iEnclosureType) || (4 == m_iEnclosureType) || (5 == m_iEnclosureType)) 
	{
		if(GetPrivateProfileString(sApp.c_str() , _T("Enclosure_SizeB") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
		{
			m_dEnclosureSizeB = ATOF_T(szBuf);
		}
	}else	m_dEnclosureSizeB = m_dEnclosureSizeA;

	if(GetPrivateProfileString(sApp.c_str() , _T("Enclosure_CenterLine") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_bEnclosureCenterLine = (0 == STRICMP_T(szBuf , _T("ON")));
	}else	m_bEnclosureCenterLine = false;

	m_sDelimiter = _T("-");
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Delimiter") ,NULL,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_sDelimiter.assign(szBuf);
	}

	STRNCPY_T(szBuf , _T("Standard") , 8);
	GetPrivateProfileString(sApp.c_str() , _T("TextStyle") ,NULL,szBuf,MAX_PATH,sIniFilePath.c_str());
	m_oTextStyle.Parse(szBuf , sIniFilePath);

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