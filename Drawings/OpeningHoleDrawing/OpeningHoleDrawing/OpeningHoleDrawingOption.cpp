#include "StdAfx.h"
#include <Tokenizer.h>
#include "OpeningHoleDrawingOption.h"

COpeningHoleDrawingOption::COpeningHoleDrawingOption(void)
{
	m_dChartRowHeight = 0.0;
	m_oChartLineProp.level = _T("0");
	m_oChartLineProp.weight = _T("0");
	m_oChartLabelProp.level = _T("0");
	m_oChartLabelProp.weight = _T("0");
	m_iChartTextColor = 0;
}

COpeningHoleDrawingOption::~COpeningHoleDrawingOption(void)
{
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   GetInstance
    @return     COpeningHoleDrawingOption&
    @param      void
    @brief
******************************************************************************/
COpeningHoleDrawingOption& COpeningHoleDrawingOption::GetInstance(void)
{
	static COpeningHoleDrawingOption __instance__;

	return __instance__;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   GetClearanceOf
    @return     double
    @param      const double& dPipeSize
    @brief
******************************************************************************/
double COpeningHoleDrawingOption::GetClearanceOf(const double& dPipeSize) const
{
	return (dPipeSize < 10) ? 300 : 500;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   GetGroupType
    @return     STRING_T
    @brief
******************************************************************************/
STRING_T COpeningHoleDrawingOption::GetGroupType() const
{
	static const STRING_T sHoleType(_T("Box"));

	return sHoleType;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   GetChartLocation
    @return     CLocation
    @brief
******************************************************************************/
CLocation COpeningHoleDrawingOption::GetChartLocation()
{
	return m_oChartLocation;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   GetLabelWidthContainer
    @return     CLabelWidthContainer
    @param      void
    @brief
******************************************************************************/
CLabelWidthContainer COpeningHoleDrawingOption::GetLabelWidthContainer(void) const
{
	return m_oLabelWidthContainer;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   GetChartRowHeight
    @return     double
    @brief
******************************************************************************/
double COpeningHoleDrawingOption::GetChartRowHeight() const
{
	return m_dChartRowHeight;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   GetChartLineColor
    @return     int
    @param      void
    @brief
******************************************************************************/
CompProp COpeningHoleDrawingOption::GetChartLineProp() const
{
	return m_oChartLineProp;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   GetChartProp
    @return     CompProp
    @brief
******************************************************************************/
CompProp COpeningHoleDrawingOption::GetChartLabelProp() const
{
	return m_oChartLabelProp;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   GetCharTextColor
    @return     int
    @param      void
    @brief
******************************************************************************/
int COpeningHoleDrawingOption::GetChartTextColor(void) const
{
	return m_iChartTextColor;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   GetLabelEntry
    @return     int
    @param      vector<STRING_T>&   oLabelEntry
    @brief
******************************************************************************/
int COpeningHoleDrawingOption::GetLabelEntry(vector<STRING_T>& oLabelEntry)
{
	oLabelEntry.clear();
	oLabelEntry.insert(oLabelEntry.begin() , m_oLabelEntry.begin() , m_oLabelEntry.end());

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   Parse
    @return     int
    @param      const STRING_T& sIniFilePath
    @brief
******************************************************************************/
int COpeningHoleDrawingOption::Parse(const STRING_T& sIniFilePath)
{
	static const STRING_T sApp(_T("OpeningHole"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	if(::GetPrivateProfileString(sApp.c_str() , _T("TextStyle") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oTextStyle.m_rName = szBuf;
		m_oTextStyle.Parse(sIniFilePath);
	}

	vector<STRING_T> oResult;
	if(GetPrivateProfileStringA(sApp.c_str() , _T("Chart_Location") , NULL , (szBuf) , MAX_PATH , (sIniFilePath.c_str())))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		if(3 == oResult.size())
		{
			m_oChartLocation.Set( oResult[0] , atof(oResult[1].c_str()) , atof(oResult[2].c_str()) );
		}
	}

	if(GetPrivateProfileStringA(sApp.c_str() , _T("Chart_RowHeight") , NULL , (szBuf) , MAX_PATH , (sIniFilePath.c_str())))
	{
		m_dChartRowHeight = atof(szBuf);
	}

	if(GetPrivateProfileStringA(sApp.c_str() , _T("Chart_Level") , NULL , (szBuf) , MAX_PATH , (sIniFilePath.c_str())))
	{
		m_oChartLineProp.level = szBuf;
	}

	if(GetPrivateProfileStringA(sApp.c_str() , _T("Chart_Color") , NULL , (szBuf) , MAX_PATH , (sIniFilePath.c_str())))
	{
		m_oChartLineProp.color = szBuf;
	}

	if(GetPrivateProfileStringA(sApp.c_str() , _T("Chart_Style") , NULL , (szBuf) , MAX_PATH , (sIniFilePath.c_str())))
	{
		m_oChartLineProp.style = szBuf;
	}

	if(GetPrivateProfileStringA(sApp.c_str() , _T("Chart_Weight") , NULL , (szBuf) , MAX_PATH , (sIniFilePath.c_str())))
	{
		m_oChartLineProp.weight = szBuf;
	}

	if(GetPrivateProfileStringA(sApp.c_str() , _T("Chart_Label_Level") , NULL , (szBuf) , MAX_PATH , (sIniFilePath.c_str())))
	{
		m_oChartLabelProp.level = szBuf;
	}

	if(GetPrivateProfileStringA(sApp.c_str() , _T("Chart_Label_Color") , NULL , (szBuf) , MAX_PATH , (sIniFilePath.c_str())))
	{
		m_oChartLabelProp.color = szBuf;
	}

	if(GetPrivateProfileStringA(sApp.c_str() , _T("Chart_Label_Style") , NULL , (szBuf) , MAX_PATH , (sIniFilePath.c_str())))
	{
		m_oChartLabelProp.style = szBuf;
	}

	if(GetPrivateProfileStringA(sApp.c_str() , _T("Chart_Label_Weight") , NULL , (szBuf) , MAX_PATH , (sIniFilePath.c_str())))
	{
		m_oChartLabelProp.weight = szBuf;
	}

	m_oLabelEntry.clear();
	if(GetPrivateProfileStringA(sApp.c_str() , _T("Chart_Label") , NULL , (szBuf) , MAX_PATH , (sIniFilePath.c_str())))
	{
		CTokenizer<CIsComma>::Tokenize( m_oLabelEntry , szBuf , CIsComma() );
	}

	if(GetPrivateProfileStringA(sApp.c_str() , _T("Chart_Width") , NULL , (szBuf) , MAX_PATH , (sIniFilePath.c_str())))
	{
		CTokenizer<CIsComma>::Tokenize( oResult , szBuf , CIsComma() );
		m_oLabelWidthContainer.Parse( oResult );
	}

	return ERROR_SUCCESS;
}