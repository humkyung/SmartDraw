#include "StdAfx.h"
#include <Tokenizer.h>
#include "PipeOption.h"

#include "soci.h"
#include "soci-sqlite3.h"

using namespace tag_option;

CPipeOption::CPipeOption(void)
{
	m_pElevationTypeBySpec = new map<STRING_T,STRING_T>;
}

CPipeOption::~CPipeOption(void)
{
	try
	{
		SAFE_DELETE(m_pElevationTypeBySpec);
	}
	catch(...)
	{}
}

/**	
	@brief	read pipe settings
	@author	humkyung
	@date	2010.05.27
	@param	
	@return
*/
int CPipeOption::Parse(const STRING_T& sIniFilePath,const STRING_T& sRefFilePath)
{
	///get elevation type by spec:#18(http://atools.co.kr:9002/redmine/issues/18) - 2016.03.12 added by humkyung
	try
	{
		soci::session oSession(*soci::factory_sqlite3() , sRefFilePath.c_str());
		{
			soci::rowset<soci::row> rs(oSession.prepare << _T("select Spec,ElevationType from ElevationTypeBySpec"));
			for(soci::rowset<soci::row>::const_iterator itr = rs.begin();itr != rs.end();++itr)
			{
				m_pElevationTypeBySpec->insert(make_pair(itr->get<STRING_T>(0),itr->get<STRING_T>(1)));
			}
		}
	}
	catch(...){}
	///up to here

	///get trim end dash option - 2016.04.01 added by humkyung
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	if(GetPrivateProfileString(_T("Line Number") , _T("Trim End -") , _T("False") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_bTrimEndDash = (_T("True") == STRING_T(szBuf));
	}
	///up to here

	m_ext.Parse(sIniFilePath);
	m_int.Parse(sIniFilePath);

	return ERROR_SUCCESS;
}

/**	
	@brief	return elevation type of given spec
	@author	humkyung
	@date	2016.03.12
	@param	spec
	@return	PIPE_ELEV_TYPE
*/
PIPE_ELEV_T CPipeOption::GetElevationTypeBySpec(const STRING_T& sSpec)
{
	if(NULL != m_pElevationTypeBySpec)
	{
		map<STRING_T,STRING_T>::const_iterator where = m_pElevationTypeBySpec->find(sSpec);
		if(m_pElevationTypeBySpec->end() != where)
		{
			if(_T("BOP") == where->second) return ELEV_BOP;
			if(_T("CENTER") == where->second) return ELEV_CL;
			if(_T("TOP") == where->second) return ELEV_TOP;
			if(_T("Mixed(CL + BOP)") == where->second) return ELEV_MIXIN;
		}
	}
	
	return ELEV_BOP;
}

/**	
	@brief	return trim end dash option
	@author	humkyung
	@date	2016.04.01
	@return	bool
*/
bool CPipeOption::GetTrimEndDash() const
{
	return m_bTrimEndDash;
}

CPipeOption::EXT_T::EXT_T()
{
	m_enmTextAlign[0] = m_enmTextAlign[1] = m_enmTextAlign[2] = m_enmTextAlign[3] = OUTSIDE_ALIGN;
}

/******************************************************************************
    @brief		parse pipe option for external
	@author     humkyung
    @date       2014-11-11
    @class      CPipeOption
    @function   EXT_T::Parse
    @return     int
    @param      const STRING_T& sIniFilePath
******************************************************************************/
int CPipeOption::EXT_T::Parse(const STRING_T& sIniFilePath)
{
	const static STRING_T sApp( _T("Outside Pipe") );
	
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	
	/// get tag display size - 2013.06.16 added by humkyung
	sTagDisplaySize = _T("All");
	if(GetPrivateProfileString(_T("Line Number") , _T("DisplayStyle") , _T("") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		if(0 == STRICMP_T(szBuf , _T("Over the size")))
		{
			if(GetPrivateProfileString(_T("Line Number") , _T("CriticalSize") , _T("") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
			{
				sTagDisplaySize = szBuf;
			}
		}
	}
	/// up to here

	/// get dim. display size - 2014.06.14 added by humkyung
	sDimDisplaySize = _T("All");
	if(GetPrivateProfileString(_T("Line Number") , _T("Display Dim. Style") , _T("All") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		sDimDisplaySize = szBuf;
	}
	/// up to here

	eElev = ELEV_MIXIN;
	if(GetPrivateProfileString(sApp.c_str(),_T("LineNo_ExtGrpArrow"),_T(""),szBuf,MAX_PATH,sIniFilePath.c_str()))
		nGroupArrow = ATOI_T(szBuf);

	/// PIPE
	eTag = DOUBLELINE;
	if(GetPrivateProfileString(sApp.c_str(), _T("DisplayStyle") , _T("") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		if(0 == STRICMP_T(szBuf , _T("LineNo")))
		{
			eTag = SINGLE_LINENO;
		}
		else if(0 == STRICMP_T(szBuf , _T("LineNo+Elev")))
		{
			eTag = SINGLE_LINENO_ELEV;
		}
		else if(0 == STRICMP_T(szBuf, _T("LineNo/Elev")))
		{
			eTag = DOUBLELINE;
		}
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("LineNo_TextStyle") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLineNoTextStyle.Parse(szBuf , sIniFilePath);
	}	
	
	if(GetPrivateProfileString(sApp.c_str() , _T("LineNo_Level") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLineNoProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("LineNo_Color") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLineNoProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("LineNo_Style") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLineNoProp.style = szBuf;
	}
	
	if(GetPrivateProfileString(sApp.c_str() , _T("Elev_TextStyle") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oElevTextStyle.Parse(szBuf , sIniFilePath);
	}	
	
	if(GetPrivateProfileString(sApp.c_str() , _T("Elev_Level") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oElevProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Elev_Color") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oElevProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Elev_Style") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oElevProp.style = szBuf;
	}

	/// 2013.06.16 added by humkyung
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_bLeaderLine = (_T("ON") == STRING_T(szBuf));
	}
	else	m_bLeaderLine = FALSE;
	/// up to here

	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Level") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Color") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Style") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.style = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Weight") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.weight = szBuf;
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Offset") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_dLabelOffset = ATOF_T(szBuf);
	}
	
	if(GetPrivateProfileString(sApp.c_str(),_T("LineNo_ExtGrpLevel"),_T("LEVEL_0"),szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		if(0 == STRCMP_T(szBuf,_T("LEVEL_0")))		nGroupLevel = 0;
		else if(0 == STRCMP_T(szBuf,_T("LEVEL_1")))	nGroupLevel = 1;
		else if(0 == STRCMP_T(szBuf,_T("LEVEL_2"))) nGroupLevel = 2;
		else if(0 == STRCMP_T(szBuf,_T("LEVEL_3"))) nGroupLevel = 3;
	}

	memset(szPrefix,'\0',sizeof(TCHAR)*3*16);
	if(GetPrivateProfileString(_T("Line Number") , _T("CL_Prefix") , _T("") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		STRCPY_T(szPrefix[0],szBuf);
	}else	szPrefix[0][0]='\0';
	if(GetPrivateProfileString(_T("Line Number") , _T("BOP_Prefix") , _T("") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		STRCPY_T(szPrefix[1],szBuf);
	}else	szPrefix[1][0]='\0';
	if(GetPrivateProfileString(_T("Line Number") , _T("TOP_Prefix") , _T("") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		STRCPY_T(szPrefix[2],szBuf);
	}else	szPrefix[2][0]='\0';

	if(GetPrivateProfileString(sApp.c_str(), _T("Elev_Type") , _T("BOP") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		if(0 == STRCMP_T(szBuf, _T("Mixed(CL + BOP)")))
		{
			eElev = ELEV_MIXIN;
		}
		else if(0 == STRCMP_T(szBuf, _T("CENTER") ))
		{
			eElev = ELEV_CL;
		}
		else if(0 == STRCMP_T(szBuf, _T("BOP")))
		{
			eElev = ELEV_BOP;
		}
		else if(0 == STRCMP_T(szBuf, _T("TOP")))
		{
			eElev = ELEV_TOP;
		}
		else if(0 == STRICMP_T(szBuf, _T("BySpec")))
		{
			eElev = ELEV_BYSPEC;
		}
	}
	else
	{
		/// default value
		eElev = ELEV_MIXIN;
		STRCPY_T(szPrefix[0] , _T("CL."));
		STRCPY_T(szPrefix[1] , _T("BOP."));
		STRCPY_T(szPrefix[2] , _T("TOP."));
	}
	
	if(GetPrivateProfileString(_T("Line Number") , _T("WP_Prefix") , _T("") ,szBuf,MAX_PATH,sIniFilePath.c_str()))
		STRNCPY_T(szWP , szBuf , 15);
	else
	{
		memset(szWP , '\0' , PREFIX_BUF_SIZ); 
	}
	szWP[15] = '\0';

	/// 2007.11.9에 수정한 것을 PDS에 적용 - 2008.12.23
	if(GetPrivateProfileString(sApp.c_str(),_T("LineNo_ExtElvCoord"),_T(""),szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		if( STRING_T(_T("Elev")) == szBuf )
		{
			eSecondTagType = ONLY_ELEV;
		}
		else
		{
			eSecondTagType = ELEV_COORD;
		}
	}else	eSecondTagType = ONLY_ELEV;
	///? PIPE

	/// get text alignment style - 2014.07.02 added by humkyung
	m_enmTextAlign[0] = m_enmTextAlign[1] = m_enmTextAlign[2] = m_enmTextAlign[3] = OUTSIDE_ALIGN;
	if(GetPrivateProfileString(sApp.c_str(),_T("Alignment_Style"),_T("Outside"),szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_enmTextAlign[0] = (0 == STRCMP_T(szBuf,_T("Outside"))) ? OUTSIDE_ALIGN : INSIDE_ALIGN;
		m_enmTextAlign[1] = m_enmTextAlign[0];
		m_enmTextAlign[2] = m_enmTextAlign[0];
		m_enmTextAlign[3] = m_enmTextAlign[0];
	}

	if(GetPrivateProfileString(sApp.c_str(),_T("Top Alignment_Style"),_T("Outside"),szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_enmTextAlign[0] = (0 == STRCMP_T(szBuf,_T("Outside"))) ? OUTSIDE_ALIGN : INSIDE_ALIGN;
	}if(GetPrivateProfileString(sApp.c_str(),_T("Bottom Alignment_Style"),_T("Outside"),szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_enmTextAlign[1] = (0 == STRCMP_T(szBuf,_T("Outside"))) ? OUTSIDE_ALIGN : INSIDE_ALIGN;
	}if(GetPrivateProfileString(sApp.c_str(),_T("Left Alignment_Style"),_T("Outside"),szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_enmTextAlign[2] = (0 == STRCMP_T(szBuf,_T("Outside"))) ? OUTSIDE_ALIGN : INSIDE_ALIGN;
	}if(GetPrivateProfileString(sApp.c_str(),_T("Right Alignment_Style"),_T("Outside"),szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_enmTextAlign[3] = (0 == STRCMP_T(szBuf,_T("Outside"))) ? OUTSIDE_ALIGN : INSIDE_ALIGN;
	}
	/// up to here

	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Type") , _T("OPEN30") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sArrowType = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Size") , _T("1.5") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_dArrowSize = ATOF_T(szBuf);
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		return alignment style for top side
	@author     humkyung
    @date       2014-11-11
    @class      CPipeOption::EXT_T
    @function   GetTopSideAlignment
    @return     double
******************************************************************************/
TEXT_ALIGN_E CPipeOption::EXT_T::GetTopSideAlignment() const
{
	return m_enmTextAlign[0];
}

/******************************************************************************
    @brief		return alignment style for bottom side
	@author     humkyung
    @date       2014-11-11
    @class      CPipeOption::EXT_T
    @function   GetTopSideAlignment
    @return     double
******************************************************************************/
TEXT_ALIGN_E CPipeOption::EXT_T::GetBottomSideAlignment() const
{
	return m_enmTextAlign[1];
}

/******************************************************************************
    @brief		return alignment style for left side
	@author     humkyung
    @date       2014-11-11
    @class      CPipeOption::EXT_T
    @function   GetTopSideAlignment
    @return     double
******************************************************************************/
TEXT_ALIGN_E CPipeOption::EXT_T::GetLeftSideAlignment() const
{
	return m_enmTextAlign[2];
}

/******************************************************************************
    @brief		return alignment style for right side
	@author     humkyung
    @date       2014-11-11
    @class      CPipeOption::EXT_T
    @function   GetTopSideAlignment
    @return     double
******************************************************************************/
TEXT_ALIGN_E CPipeOption::EXT_T::GetRightSideAlignment() const
{
	return m_enmTextAlign[3];
}

/******************************************************************************
	@brief		return tag display size in mm
    @author     humkyung
    @date       2014-06-14
    @class      CPipeOption::EXT_T
    @function   GetTagDisplaySize
    @return     double
******************************************************************************/
double CPipeOption::EXT_T::GetTagDisplaySize() const
{
	double dCriticalSize = 0.0;
	if(0 != STRICMP_T(_T("All") , sTagDisplaySize.c_str()))
	{
		vector<STRING_T> oResult;
		CTokenizer<CIsFromString>::Tokenize(oResult , sTagDisplaySize , CIsFromString(_T("-")));
		if(oResult.size() >= 2)
		{
			dCriticalSize = ATOF_T(oResult[oResult.size() - 1].c_str());
		}
	}

	return dCriticalSize;
}

/******************************************************************************
	@brief		return dimension display size
    @author     humkyung
    @date       2014-06-14
    @return     double
******************************************************************************/
double CPipeOption::EXT_T::GetDimDisplaySize() const
{
	double dCriticalSize = 0.0;
	if(0 != STRICMP_T(_T("All") , sDimDisplaySize.c_str()))
	{
		vector<STRING_T> oResult;
		CTokenizer<CIsFromString>::Tokenize(oResult , sDimDisplaySize , CIsFromString(_T("-")));
		if(oResult.size() >= 2)
		{
			dCriticalSize = ATOF_T(oResult[oResult.size() - 1].c_str());
		}
	}

	return dCriticalSize;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-19
    @class      CPipeOption
    @function   INT_T::Parse
    @return     int
    @param      const           STRING_T&
    @param      sIniFilePath
    @brief
******************************************************************************/
int CPipeOption::INT_T::Parse(const STRING_T& sIniFilePath)
{
	static const STRING_T sApp(_T("Inside Pipe"));
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	m_sDisplayStyle = _T("Only LineNo");
	if(GetPrivateProfileString(sApp.c_str() , _T("DisplayStyle") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sDisplayStyle.assign(szBuf);
	}
	
	m_sCoordDisplayStyle = _T("Elev");
	if(GetPrivateProfileString(sApp.c_str() , _T("Elev_DisplayStyle") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sCoordDisplayStyle.assign(szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("LineNo_TextStyle") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLineNoTextStyle.Parse(szBuf , sIniFilePath);
	}	

	if(GetPrivateProfileString(sApp.c_str() , _T("Elev_TextStyle") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oElevTextStyle.Parse(szBuf , sIniFilePath);
	}	

	m_dOffset = 0.5;
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Offset") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_dOffset = ATOF_T(szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Level") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Color") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Style") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.style = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Weight") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.weight = szBuf;
	}

	m_sArrowType = _T("OPEN30");
	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Type") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sArrowType = szBuf;
	}
	m_dArrowSize = 1.5;
	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Size") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_dArrowSize = ATOF_T(szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("LineNo_Level") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLineNoProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("LineNo_Color") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLineNoProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("LineNo_Style") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLineNoProp.style = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("LineNo_Weight") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLineNoProp.weight = szBuf;
	}
	
	if(GetPrivateProfileString(sApp.c_str() , _T("Elev_Level") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oElevProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Elev_Color") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oElevProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Elev_Style") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oElevProp.style = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Elev_Weight") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oElevProp.weight = szBuf;
	}

	return ERROR_SUCCESS;
}