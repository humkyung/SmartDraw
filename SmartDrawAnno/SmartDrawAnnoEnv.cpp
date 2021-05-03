#include "StdAfx.h"
#include <tchar.h>
#include <Tokenizer.h>
#include <aDraw_DraFile.h>
#include "SmartDrawAnnoPipe.h"
#include "SmartDrawAnnoEqui.h"
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoPos.h"

int CaDraw_TagEnv::OUTPUT_UNIT;
int CaDraw_TagEnv::INPUT_UNIT;
double CaDraw_TagEnv::TOLERANCE = 0.1;

DIM_STYLE_ENV_T::DIM_STYLE_ENV_T() : m_bViewLimitOn(true)
{
}

DIM_STYLE_ENV_T::~DIM_STYLE_ENV_T()
{
	try
	{
	}
	catch(...)
	{
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CaDraw_TagEnv::CaDraw_TagEnv(void)
{
	m_nBaseLineOffset = 0.f;
}

CaDraw_TagEnv::CaDraw_TagEnv(const CaDraw_TagEnv& rhs)
{
	m_nBaseLineOffset = 0.f;
}

CaDraw_TagEnv::~CaDraw_TagEnv(void)
{
}

CaDraw_TagEnv& CaDraw_TagEnv::GetInstance(void)
{
	static CaDraw_TagEnv __ENV__;

	return __ENV__;
}

/**	
	@brief	read settings
	@author	HumKyung.BAEK
	@date	2010.05.27
	@param	
	@return
*/
int CaDraw_TagEnv::Parse(const STRING_T& rIniFilePath,const STRING_T& sRefFilePath)
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	
	m_sIniFilePath = rIniFilePath;

	m_nWidthFactor = 1.f;
	//////////////////////////////////////////////////////////////////////////
	/// Output unit
	if(GetPrivateProfileString(_T("UNIT") , _T("Output") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		if(0 == STRICMP_T(szBuf , _T("Inch Feet")))
			CaDraw_TagEnv::OUTPUT_UNIT = CaDraw_TagEnv::INCH_FEET;
		else	CaDraw_TagEnv::OUTPUT_UNIT = CaDraw_TagEnv::METRIC;
	}
	//////////////////////////////////////////////////////////////////////////
	/// Input unit
	if(GetPrivateProfileString( _T("UNIT") , _T("Input") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		if(0 == STRCMP_T(szBuf , _T("Inch Feet") ))
			CaDraw_TagEnv::INPUT_UNIT = CaDraw_TagEnv::INCH_FEET;
		else	CaDraw_TagEnv::INPUT_UNIT = CaDraw_TagEnv::METRIC;
	}

	//////////////////////////////////////////////////////////////////////////
	/// unit conversion factor (input --> output)
	m_nUnitConvFactor = 1.f;
	if(GetPrivateProfileString(_T("UNIT") , _T("Conv Factor") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_nUnitConvFactor = ATOF_T(szBuf);
	}

	m_nGap = 0.5f;
	if(GetPrivateProfileString(_T("ANNOTATION_OPTIONS") , _T("LineNo_ExtGapA") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_nGap = ATOF_T(szBuf);
	}

	m_extDimStyle.Parse(_T("Ext Dim Style") , rIniFilePath);
	m_intDimStyle.Parse(_T("Int Dim Style") , rIniFilePath);

	m_anno.Parse(rIniFilePath.c_str());
	m_nozzle.Parse(rIniFilePath);
	m_reducer.Parse(rIniFilePath);
	m_inst.Parse(rIniFilePath);
	m_support.Parse(rIniFilePath);

	m_pipe.Parse(rIniFilePath,sRefFilePath);
	m_equi.Parse(_T("Outside Equipment") , rIniFilePath);
	m_InsideEqp.Parse(_T("Inside Equipment") , rIniFilePath);
	m_stru.Parse(rIniFilePath);
	m_leader.Parse(rIniFilePath);

	m_area.Parse(rIniFilePath);
	m_outside.Parse(rIniFilePath);
	m_nDistText = m_outside.m_nDistA + m_outside.m_nDistB + m_outside.m_nDistC + m_anno.m_nLineTextHeight;

	m_elev.Parse(rIniFilePath);
	m_match.Parse(rIniFilePath);
	m_coord.Parse(rIniFilePath);

	m_generation.Parse(rIniFilePath,sRefFilePath);

	/// read tolerance value from ini file.
	CaDraw_TagEnv::TOLERANCE = 0.1;
	if(GetPrivateProfileString(_T("ANNOTATION_OPTIONS"),_T("Ext_Tolerance"),_T(""),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		CaDraw_TagEnv::TOLERANCE = ATOF_T(szBuf);
	}

	/// read cable tray options
	m_cableT.Parse(rIniFilePath);

	/// read platform options - 2012.02.26 added by humkyung
	m_platform.Parse(rIniFilePath);
	/// read vertical pipe section mark - 2014.04.13 added by humkyung
	CaDraw_TagPipe::HATCHING_ENV::GetInstance().Parse(rIniFilePath);
	CaDraw_TagEqui::HATCHING_ENV::GetInstance().Parse(rIniFilePath);

	/// read valve options - 2012.03.24 added by humkyung
	m_valve.Parse(rIniFilePath);

	/// read specialty options - 2015.09.01 added by humkyung
	m_specialty.Parse(rIniFilePath);

	return ERROR_SUCCESS;
}

EQP_ENV_T::EQP_ENV_T()
{
	m_enmTextAlign[0] = m_enmTextAlign[1] = m_enmTextAlign[2] = m_enmTextAlign[3] = OUTSIDE_ALIGN;
}

/******************************************************************************
    @brief		return alignment style for top side
	@author     humkyung
    @date       2014-11-11
    @class      EQP_ENV_T
    @function   GetTopSideAlignment
    @return     TEXT_ALIGN_E
******************************************************************************/
TEXT_ALIGN_E EQP_ENV_T::GetTopSideAlignment() const
{
	return m_enmTextAlign[0];
}

/******************************************************************************
    @brief		return alignment style for bottom side
	@author     humkyung
    @date       2014-11-11
    @class      EQP_ENV_T
    @function   GetBottomSideAlignment
    @return     TEXT_ALIGN_E
******************************************************************************/
TEXT_ALIGN_E EQP_ENV_T::GetBottomSideAlignment() const
{
	return m_enmTextAlign[1];
}

/******************************************************************************
    @brief		return alignment style for left side
	@author     humkyung
    @date       2014-11-11
    @class      EQP_ENV_T
    @function   GetLeftSideAlignment
    @return     TEXT_ALIGN_E
******************************************************************************/
TEXT_ALIGN_E EQP_ENV_T::GetLeftSideAlignment() const
{
	return m_enmTextAlign[2];
}

/******************************************************************************
    @brief		return alignment style for right side
	@author     humkyung
    @date       2014-11-11
    @class      EQP_ENV_T
    @function   GetRightSideAlignment
    @return     TEXT_ALIGN_E
******************************************************************************/
TEXT_ALIGN_E EQP_ENV_T::GetRightSideAlignment() const
{
	return m_enmTextAlign[3];
}

/**	
	@brief	read equipment settings
	@author	humkyung
	@date	2010.05.27
	@param	ini file pathi
	@return
*/
int EQP_ENV_T::Parse(const STRING_T& sAppName , const STRING_T& rIniFilePath)
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	/// Outside EQP
	m_eTagStyle =  EQP_MIXED;	/// MIXED is default value
	if(GetPrivateProfileString(_T("Inside Equipment") , _T("TagStyle") , _T("Outside View") , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		if(0 == STRICMP_T(szBuf , _T("Depend on area")))
				m_eTagStyle = EQP_MIXED;
		else if(0 == STRICMP_T(szBuf , _T("Inside View")))
				m_eTagStyle = EQP_INTERNAL;
		else	m_eTagStyle = EQP_EXTERNAL;
	}

	m_bWriteCoordinate = true;
	m_bWriteDesc = false;
	if(GetPrivateProfileString(sAppName.c_str() , _T("DisplayStyle") , _T("") , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		m_bWriteCoordinate = (0 == STRCMP_T(szBuf, _T("Name/Coord")));
		m_bWriteDesc = (0 == STRCMP_T(szBuf, _T("Name/Desc")));
	}
	
	if(GetPrivateProfileString(sAppName.c_str(), _T("Name_TextStyle") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oNameTextStyle.Parse(szBuf , rIniFilePath);
	}

	if(GetPrivateProfileString(sAppName.c_str(), _T("Name_Level") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oNameProp.level = szBuf;
	}
	if(GetPrivateProfileString(sAppName.c_str(), _T("Name_Color") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oNameProp.color = szBuf;
	}
	if(GetPrivateProfileString(sAppName.c_str(), _T("Name_Style") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oNameProp.style = szBuf;
	}

	/// get coordinate display option - 2014.09.12 added by humkyung
	if(GetPrivateProfileString(sAppName.c_str(), _T("Coord_Display") , _T("OFF") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_bWriteCoordinate = (0 == STRCMP_T(szBuf , _T("ON")));
	}
	/// up to here

	if(GetPrivateProfileString(sAppName.c_str(), _T("Coord_TextStyle") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oCoordTextStyle.Parse(szBuf , rIniFilePath);
	}

	if(GetPrivateProfileString(sAppName.c_str(), _T("Coord_Level") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oCoordProp.level = szBuf;
	}
	if(GetPrivateProfileString(sAppName.c_str(), _T("Coord_Color") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oCoordProp.color = szBuf;
	}
	if(GetPrivateProfileString(sAppName.c_str(), _T("Coord_Style") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oCoordProp.style = szBuf;
	}

	if(GetPrivateProfileString(sAppName.c_str(), _T("Leader_Level") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oLeaderProp.level = szBuf;
	}
	if(GetPrivateProfileString(sAppName.c_str(), _T("Leader_Color") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oLeaderProp.color = szBuf;
	}
	if(GetPrivateProfileString(sAppName.c_str(), _T("Leader_Style") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oLeaderProp.style = szBuf;
	}
	if(GetPrivateProfileString(sAppName.c_str(), _T("Leader_Weight") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oLeaderProp.weight = szBuf;
	}

	/// get equipment center line properties
	m_bCenterLine = true;
	if(GetPrivateProfileString(_T("Inside Equipment") , _T("CenterLine") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_bCenterLine = (0 == STRCMP_T(szBuf , _T("ON")));
	}

	if(GetPrivateProfileString(_T("Inside Equipment"), _T("Centerline_Level") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oCenterlineProp.level = szBuf;
	}
	if(GetPrivateProfileString(_T("Inside Equipment"), _T("Centerline_Color") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oCenterlineProp.color = szBuf;
	}
	if(GetPrivateProfileString(_T("Inside Equipment"), _T("Centerline_Style") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oCenterlineProp.style = szBuf;
	}
	if(GetPrivateProfileString(_T("Inside Equipment"), _T("Centerline_Weight") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oCenterlineProp.weight = szBuf;
	}
	/// up to here

	/// read equipment tag on/off - 2012.02.20 added by humkyung
	bTag = true;
	if(GetPrivateProfileString(sAppName.c_str() , _T("EQP_Tag") , _T("") , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		bTag = (0 == STRICMP_T(szBuf , _T("ON"))) ? true : false;
	}
	/// up to here

	/// get text alignment style - 2014.07.02 added by humkyung
	m_enmTextAlign[0] = m_enmTextAlign[1] = m_enmTextAlign[2] = m_enmTextAlign[3] = OUTSIDE_ALIGN;
	if(GetPrivateProfileString(sAppName.c_str(),_T("Alignment_Style"),_T("Outside"),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_enmTextAlign[0] = (0 == STRCMP_T(szBuf,_T("Outside"))) ? OUTSIDE_ALIGN : INSIDE_ALIGN;
		m_enmTextAlign[1] = m_enmTextAlign[2] = m_enmTextAlign[3] = m_enmTextAlign[0];
	}

	if(GetPrivateProfileString(sAppName.c_str(),_T("Top Alignment_Style"),_T("Outside"),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_enmTextAlign[0] = (0 == STRCMP_T(szBuf,_T("Outside"))) ? OUTSIDE_ALIGN : INSIDE_ALIGN;
	}
	if(GetPrivateProfileString(sAppName.c_str(),_T("Bottom Alignment_Style"),_T("Outside"),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_enmTextAlign[1] = (0 == STRCMP_T(szBuf,_T("Outside"))) ? OUTSIDE_ALIGN : INSIDE_ALIGN;
	}
	if(GetPrivateProfileString(sAppName.c_str(),_T("Left Alignment_Style"),_T("Outside"),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_enmTextAlign[2] = (0 == STRCMP_T(szBuf,_T("Outside"))) ? OUTSIDE_ALIGN : INSIDE_ALIGN;
	}
	if(GetPrivateProfileString(sAppName.c_str(),_T("Right Alignment_Style"),_T("Outside"),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_enmTextAlign[3] = (0 == STRCMP_T(szBuf,_T("Outside"))) ? OUTSIDE_ALIGN : INSIDE_ALIGN;
	}
	/// up to here

	/// get label offset - 2014.09.12 added by humkyung
	m_dOffset = 1.0;
	if(GetPrivateProfileString(sAppName.c_str(),_T("Label_Offset"),_T("1.0"),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_dOffset = ATOF_T(szBuf);
	}
	/// up to here

	return ERROR_SUCCESS;
}

/**	
	@brief	read structure settings
	@author	HumKyung.BAEK
	@date	2010.05.27
	@param	
	@return
*/
int STR_ENV_T::Parse(const STRING_T& rIniFilePath)
{
	static const STRING_T rApp(_T("Structure"));

	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	//////////////////////////////////////////////////////////////////////////

	nTagDir = 0;
	
	nDist = 10.f;
	if(GetPrivateProfileString(_T("OUTSIDE_OPTIONS") , _T("DIST_STR"),_T(""),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
			nDist = ATOF_T(szBuf);
	}

	bTaggingOverlap = false;
	if(GetPrivateProfileString(rApp.c_str(),_T("STR_TaggingOverlap"),_T(""),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		bTaggingOverlap = (0 == STRCMP_T(szBuf,_T("ON")));
	}

	if(GetPrivateProfileString(rApp.c_str() , _T("UpDown") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		if(0 == STRICMP_T(szBuf , _T("UP")))
			nTagDir |= STR_UP;
		else if(0 == STRICMP_T(szBuf , _T("DOWN")))
			nTagDir |= STR_DOWN;
		else if(0 == STRICMP_T(szBuf , _T("BOTH")))
			nTagDir |= (STR_UP | STR_DOWN);
	}
	else
	{
		/// default value
		nTagDir |= STR_UP;
	}

	if(GetPrivateProfileString(rApp.c_str() , _T("LeftRight") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		if(0 == STRICMP_T(szBuf , _T("LEFT")))
			nTagDir |= STR_LEFT;
		else if(0 == STRICMP_T(szBuf , _T("RIGHT")))
			nTagDir |= STR_RIGHT;
		else if(0 == STRICMP_T(szBuf , _T("BOTH")))
			nTagDir |= (STR_LEFT | STR_RIGHT);
	}
	else
	{
		/// default value
		nTagDir |= STR_LEFT;
	}

	if(GetPrivateProfileString(rApp.c_str() , _T("TextStyle") , _T("") , szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oTextStyle.Parse( szBuf , rIniFilePath );
	}	

	if(GetPrivateProfileString(rApp.c_str() , _T("Label_Level") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oLabelProp.level = szBuf;
	}
	if(GetPrivateProfileString(rApp.c_str() , _T("Label_Color") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oLabelProp.color = szBuf;
	}
	if(GetPrivateProfileString(rApp.c_str() , _T("Label_Style") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oLabelProp.style = szBuf;
	}

	if(GetPrivateProfileString(rApp.c_str() , _T("Leader_Level") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oLeaderProp.level = szBuf;
	}
	if(GetPrivateProfileString(rApp.c_str() , _T("Leader_Color") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oLeaderProp.color = szBuf;
	}
	if(GetPrivateProfileString(rApp.c_str() , _T("Leader_Style") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oLeaderProp.style = szBuf;
	}
	if(GetPrivateProfileString(rApp.c_str() , _T("Leader_Weight") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oLeaderProp.style = szBuf;
	}

	/*if(GetPrivateProfileString(rApp.c_str(),"STR_TextHeight","",szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		nTextHeight[0] = ATOF_T(szBuf)*0.5;
		nTextHeight[1] = nTextHeight[0];
	}
	else
	{
		nTextHeight[0] = 1.5;
		nTextHeight[1] = 1.5;
	}
	if(GetPrivateProfileString(rApp.c_str(),"STR_2LineTextHeight","",szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		nTextHeight[1] = ATOF_T(szBuf)*0.5;
	}else	nTextHeight[1] = 1.5;*/

	/// default value is ' '
	szLine2Delimiter[0] = ' ';
	szLine2Delimiter[1] = '\0';
	/// up to here
	if(GetPrivateProfileString(rApp.c_str() , _T("STR_Line2Delimiter") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		szLine2Delimiter[0] = szBuf[0];
		szLine2Delimiter[1] = '\0';
	}

	m_dRadius = 3.f;
	if(GetPrivateProfileString(rApp.c_str() , _T("BubbleSize") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_dRadius = ATOF_T(szBuf)*0.5;
	}

	if(GetPrivateProfileString(rApp.c_str() , _T("Delimiter") , _T("-") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		szDelimiter[0] = szBuf[0];
	}

	if(GetPrivateProfileString(rApp.c_str() , _T("Column Name Direction") , _T("East West-North South") , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		m_sColumnNameDir.assign(szBuf);
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	return true if name direction is north south-east west
	@author	humkyung
	@date	2014.11.17
	@return	bool
*/
bool STR_ENV_T::IsNorthSouth_EastWestNameDir()
{
	return (m_sColumnNameDir == _T("North South-East West"));
}

/**	
	@brief	return true if name direction is east west-north south
	@author	humkyung
	@date	2014.11.17
	@return	bool
*/
bool STR_ENV_T::IsEastWest_NorthSouthNameDir()
{
	return (m_sColumnNameDir == _T("East West-North South"));
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-18
    @function   Write
    @return     int
    @param      OFSTREAM&   ofile
    @brief
******************************************************************************/
int DIM_STYLE_ENV_T::Write(OFSTREAM_T& ofile , double dDrawingScale)
{
	if(ofile.is_open())
	{
		ofile << _T("<DIMSTYLE>|");
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		ofile << m_sDimStyle << _T(",") << ((MSTN == env.m_generation.m_eOutputFormat) ? m_sFontName : rTextStyle) << _T(",") << (nTextHeight*dDrawingScale) << _T(",") << nTextHeight*m_dWidthFactor*dDrawingScale << _T(",");
		ofile << m_iTextWeight << _T(",") << rLevel << _T(",") << rTextColor << _T(",") << ATOF_T(rTextOffset.c_str())*dDrawingScale << _T(",") << m_iTextPrecision << _T(",");
		ofile << rDimLineColor << _T(",") << ATOF_T(rDimLineDistance.c_str())*dDrawingScale << _T(",");
		ofile << rExtLineColor << _T(",") << nExtLineOffset*dDrawingScale << _T(",") << nLineExt*dDrawingScale << _T(",");
		ofile << rArrowType << _T(",") << ATOF_T(rArrowSize.c_str())*dDrawingScale << _T(",") << ATOF_T(rArrowSize.c_str())*dDrawingScale << _T(",");
		ofile << ((true == m_bAutoTextSize) ? _T("ON") : _T("OFF")) << std::endl;

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**	
	@brief	read dimension settings
	@author	HumKyung.BAEK
	@date	2010.05.27
	@param	
	@return
*/
int DIM_STYLE_ENV_T::Parse(const STRING_T& sDimStyle , const STRING_T& rIniFilePath)
{
	static const STRING_T sApp(_T("DIMENSION_STYLE"));
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	m_sDimStyle = sDimStyle;	/// set dimension style

	rTextStyle = _T("3");
	nTextHeight = 1.5f;
	rLevel  = _T("1");
	rTextColor = _T("0");
	rTextOffset = _T("3.0");
	rDimLineColor = _T("0");
	rDimLineDistance  = _T("0");
	rExtLineColor  = _T("0");
	nExtLineOffset = 0;
	nLineExt = 0;
	rArrowType = _T("CLOSE");
	rArrowSize  = _T("2.5");
	m_bAutoTextSize = true;	/// 2014.11.16 added by humkyung

	vector<STRING_T> oResult;
	if(GetPrivateProfileString(sApp.c_str(), sDimStyle.c_str() , _T("") , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		if((13 == oResult.size()) || (14 == oResult.size()))
		{
			CSmartDrawAnnoTextStyle oTextStyle;
			if(ERROR_SUCCESS == oTextStyle.Parse(oResult[0] , rIniFilePath))
			{
				rTextStyle = oResult[0];
				m_sFontName= oTextStyle.GetFontName();
				nTextHeight = oTextStyle.height();
				m_dWidthFactor = oTextStyle.GetWidthFactor();
				m_iTextWeight = oTextStyle.weight();
				rLevel  =  oResult[1];
				rTextColor = oResult[2];
				rTextOffset= oResult[3];
				m_iTextPrecision = ATOI_T(oResult[4].c_str());	/// text precision
				rDimLineColor = oResult[5];
				rDimLineDistance = oResult[6];
				rExtLineColor = oResult[7];
				nExtLineOffset = ATOF_T(oResult[8].c_str());
				nLineExt = ATOF_T(oResult[9].c_str());
				rArrowType = oResult[10];
				rArrowSize  =  oResult[11];
				oResult[12];	/// height of arrow size
				if(14 == oResult.size()) m_bAutoTextSize = _T("ON" == oResult[13]);	/// 2014.11.16 added by humkyung
			}
		}
	}

	/// 2012.03.13 added by humkyung
	m_bViewLimitOn = true;
	if(GetPrivateProfileString( sApp.c_str() , _T("ViewLimit_On") , NULL , szBuf , MAX_PATH , rIniFilePath.c_str() ))
	{
		m_bViewLimitOn = (0 == STRICMP_T(szBuf , _T("ON")));
	}
	/// up to here

	return ERROR_SUCCESS;
}

/**	
	@brief	read area settings

	@author	HumKyung.BAEK

	@date	2010.05.27

	@param	

	@return
*/
int AREA_T::Parse(const STRING_T& rIniFilePath)
{
	const static STRING_T rApp(_T("AREA_OPTIONS"));

	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	//////////////////////////////////////////////////////////////////////////
	//"[AREA_OPTIONS]"
	Range[TOP_SIDE] = Range[BOTTOM_SIDE] = Range[LEFT_SIDE] = Range[RIGHT_SIDE] = 0.1;
	if(GetPrivateProfileString(rApp.c_str(), _T("Outside_Top"),_T(""),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		Range[TOP_SIDE] = ATOF_T(szBuf)*0.01;
	}
	if(GetPrivateProfileString(rApp.c_str(),_T("Outside_Bottom"),_T(""),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		Range[BOTTOM_SIDE] = ATOF_T(szBuf)*0.01;
	}
	if(GetPrivateProfileString(rApp.c_str(),_T("Outside_Left"),_T(""),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		Range[LEFT_SIDE] = ATOF_T(szBuf)*0.01;
	}
	if(GetPrivateProfileString(rApp.c_str(),_T("Outside_Right"),_T(""),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		Range[RIGHT_SIDE] = ATOF_T(szBuf)*0.01;
	}

	/// read line no range - 2014.09.11 added by humkyung
	LineNoRange[TOP_SIDE] = GetPrivateProfileInt(rApp.c_str(), _T("Line_No_Outside_Top"), 15 ,rIniFilePath.c_str())*0.01;
	LineNoRange[BOTTOM_SIDE] = GetPrivateProfileInt(rApp.c_str(),_T("Line_No_Outside_Bottom"),15,rIniFilePath.c_str())*0.01;
	LineNoRange[LEFT_SIDE] = GetPrivateProfileInt(rApp.c_str(),_T("Line_No_Outside_Left"),15,rIniFilePath.c_str())*0.01;
	LineNoRange[RIGHT_SIDE] = GetPrivateProfileInt(rApp.c_str(),_T("Line_No_Outside_Right"),15,rIniFilePath.c_str())*0.01;
	/// up to here

	/// set Ext_Tag_Side option - 2012.03.18 added by humkyung
	this->m_iExtTagSide = 0;
	vector<STRING_T> oResult;
	if(GetPrivateProfileString(rApp.c_str() , _T("Ext_Tag_Side") , _T("") , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		for(vector<STRING_T>::iterator itr = oResult.begin();itr != oResult.end();++itr)
		{
			if(_T("TOP") == (*itr))
			{
				this->m_iExtTagSide |= CaDraw_DraFile::TOP_EXT_SIDE;
			}
			else if(_T("BOTTOM") == (*itr))
			{
				this->m_iExtTagSide |= CaDraw_DraFile::BOTTOM_EXT_SIDE;
			}
			else if(_T("LEFT") == (*itr))
			{
				this->m_iExtTagSide |= CaDraw_DraFile::LEFT_EXT_SIDE;
			}
			else if(_T("RIGHT") == (*itr))
			{
				this->m_iExtTagSide |= CaDraw_DraFile::RIGHT_EXT_SIDE;
			}
		}
	}
	else
	{
		this->m_iExtTagSide = (CaDraw_DraFile::TOP_EXT_SIDE | CaDraw_DraFile::BOTTOM_EXT_SIDE | CaDraw_DraFile::LEFT_EXT_SIDE | CaDraw_DraFile::RIGHT_EXT_SIDE);
	}
	/// up to here

	return ERROR_SUCCESS;
}

/**	
	@brief	read coord settings
	@author	HumKyung.BAEK
	@date	2010.05.27
	@param	
	@return
*/
int COORD_ENV_T::Parse(const STRING_T& sIniFilePath)
{
	const static STRING_T sApp( _T("View Annotation") ) ;

	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	//////////////////////////////////////////////////////////////////////////

	///? COORD
	m_bCoordBoxOut = true;
	if(GetPrivateProfileString(sApp.c_str(), _T("View_Coord_Display_Box") , _T("ON") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_bCoordBoxOut = (STRING_T(_T("ON")) == szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("View_Coord_TextStyle") , _T("Standard") , szBuf , MAX_PATH,sIniFilePath.c_str()))
	{
		m_oTextStyle.Parse(szBuf , sIniFilePath);
	}	

	m_dLabelOffset = 0.5;
	/*if(GetPrivateProfileString(sApp.c_str() , _T("Label_Offset") , _T("") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_dLabelOffset = ATOF_T(szBuf);
	}*/

	if(GetPrivateProfileString(sApp.c_str() , _T("View_Coord_Level") , _T("1") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_oLabelProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Coord_Color") , _T("1") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_oLabelProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Coord_Style") , _T("1") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_oLabelProp.style = szBuf;
	}

	m_eOption = CaDraw_View::COORD_TOPLEFT_BOTTOMRIGHT;
	if(GetPrivateProfileString(sApp.c_str(), _T("View_Coord_Display_Style") , _T("All") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		if(0 == STRICMP_T(szBuf, _T("ALL")))
			m_eOption = CaDraw_View::COORD_ALL;
		else if(0 == STRCMP_T(szBuf, _T("TopLeft/BottomRight")))
			m_eOption = CaDraw_View::COORD_TOPLEFT_BOTTOMRIGHT;
		else if(0 == STRCMP_T(szBuf , _T("BottomLeft/TopRight")))
			m_eOption = CaDraw_View::COORD_BOTTOMLEFT_TOPRIGHT;
	}
	///? COORD

	return ERROR_SUCCESS;
}

/**	
	@brief	read true if box out is true
	@author	humkyung
	@date	2014.11.14
	@param	
	@return
*/
bool COORD_ENV_T::IsBoxOut() const
{
	return m_bCoordBoxOut;
}

/**	
	@brief	read match line data settings
	@author	HumKyung.BAEK
	@date	2010.05.27
	@param	
	@return
*/
int MATCH_ENV_T::Parse(const STRING_T& rIniFilePath)
{
	const static STRING_T rApp(_T("View Annotation"));

	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	//////////////////////////////////////////////////////////////////////////

	///? MATCH
	//if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("DrawViewData_5") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	//{
	//	bWriteMatchLine = (1 == ATOI_T(szBuf)) ? true : false; // match line
	//}

	if(GetPrivateProfileString(rApp.c_str(),_T("MatchLine_Data_TextStyle"),_T("Standard"),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oTextStyle.Parse( szBuf , rIniFilePath );
	}
	if(GetPrivateProfileString(rApp.c_str() , _T("MatchLine_Data_Level") , _T("1") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		m_oLabelProp.level = szBuf;
	}
	if(GetPrivateProfileString(rApp.c_str(),_T("MatchLine_Data_Color"),_T("1"),szBuf,MAX_PATH,rIniFilePath.c_str()))
		m_oLabelProp.color = szBuf;
	if(GetPrivateProfileString(rApp.c_str(),_T("MatchLine_Data_Style"),_T("1"),szBuf,MAX_PATH,rIniFilePath.c_str()))
		m_oLabelProp.style = szBuf;
	if(GetPrivateProfileString(rApp.c_str(),_T("MatchLine_Data_Dist"),_T("0"),szBuf,MAX_PATH,rIniFilePath.c_str()))
		nMatchDistA = ATOF_T(szBuf);

	if(GetPrivateProfileString(rApp.c_str(),_T("MatchLine_Data_Alignment"),_T("Center"),szBuf,MAX_PATH,rIniFilePath.c_str()))
		STRCPY_T(m_szJustify,szBuf);
	///? MATCH

	return ERROR_SUCCESS;
}

/**	
	@brief	read view information settings
	@author	HumKyung.BAEK
	@date	2010.05.27
	@param	
	@return
*/
int ELEV_ENV_T::Parse(const STRING_T& sIniFilePath)
{
	const static STRING_T sApp(_T("View Annotation"));

	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	//////////////////////////////////////////////////////////////////////////

	if(GetPrivateProfileString(sApp.c_str(), _T("View_Info_TextStyle") , _T("Standard") , szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_oTextStyle.Parse( szBuf , sIniFilePath );
	}	

	if(GetPrivateProfileString(sApp.c_str() , _T("View_Info_Level") , _T("1") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_oLabelProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Info_Color") , _T("1") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_oLabelProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Info_Style") , _T("1") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_oLabelProp.style = szBuf;
	}
	
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Info_Dist") , _T("0") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_dDistA = ATOF_T(szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("View_Info_Alignment") , _T("Center") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		STRCPY_T(m_szJustify , szBuf);
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	The CDraftLeader::ReadEnv function
	@author	BHK
	@date	?
	@param	rIniFilePath.c_str()	a parameter of type const char*
	@return	void	
*/
int LEADER_ENV_T::Parse(const STRING_T& rIniFilePath)
{
	const static STRING_T rApp(_T("ANNOTATION_OPTIONS"));

	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	//////////////////////////////////////////////////////////////////////////

	if(GetPrivateProfileString(rApp.c_str(), _T("LineNo_IntLeaderColor") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		STRNCPY_T(szLineColor,szBuf,16);
	}else	STRCPY_T(szLineColor, _T("white"));

	STRCPY_T(szLineStyle , _T("0"));
	STRCPY_T(szLineWeight, _T("0"));

	return ERROR_SUCCESS;
}