// RevisionChart.cpp: implementation of the CRevisionChart class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include <windows.h>
#include <assert.h>
#include <Tokenizer.h>
#include "RevisionChart.h"

#include <fstream>
using namespace std;

/**
	@brief	return revision column index corresponding to given column name
	@author	humkyung
	@date	2014.11.17
	@return	int
*/
int REVISION_CHART_T::col(const STRING_T& sColumnName)
{
	for(int i = 0;i < nColumns;++i)
	{
		if(sColumnName == CRevisionChart::COLUMNS[i]) return i;
	}

	return -1;
}

REVISION_CHART_T CRevisionChart::m_varRevision;
TCHAR CRevisionChart::m_szLocation[MAX_PATH];
STRING_T CRevisionChart::COLUMNS[8] = 
{
	_T("REV_NO") , _T("DATE") , _T("DESC") , _T("DRWN_BY") , _T("CHKD_BY") , _T("REVD_BY") , _T("APPD_BY") , _T("STAT")
};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRevisionChart::CRevisionChart()
{
	memset(&(CRevisionChart::m_varRevision),'\0',sizeof(REVISION_CHART_T));
	memset(CRevisionChart::m_szLocation , '\0' , sizeof(char)*MAX_PATH);

	CRevisionChart::m_varRevision.nX = CRevisionChart::m_varRevision.nY = 0;
	CRevisionChart::m_varRevision.nWidth = 0;
	CRevisionChart::m_varRevision.nRows  = 0;
}

CRevisionChart::~CRevisionChart()
{
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-28
    @function   Load
    @return     bool
    @param      LPCTSTR pIniFilePath
    @param      LPCTSTR pSdrFileName
    @brief
******************************************************************************/
bool CRevisionChart::Load(LPCTSTR pIniFilePath , LPCTSTR pSdrFileName)
{
	assert(pIniFilePath && "pIniFilePath is NULL");
	static const STRING_T sApp(_T("Revision Data"));

	if(pIniFilePath)
	{
		TCHAR szTmp[MAX_PATH + 1]={'\0',} , szBuf[MAX_PATH + 1]={'\0',};

		STRING_T sBorderCfgFilePath(pIniFilePath) , sRevisionDataFilePath(pIniFilePath);
		if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , _T("") , szBuf , MAX_PATH , pIniFilePath))
		{
			STRING_T sBorderFileName(szBuf);

			int at = sBorderCfgFilePath.rfind(_T("\\"));
			sBorderCfgFilePath = sBorderCfgFilePath.substr(0 , at);
			at = sBorderCfgFilePath.rfind(_T("\\"));
			sBorderCfgFilePath = sBorderCfgFilePath.substr(0 , at);
			sRevisionDataFilePath = sBorderCfgFilePath;
			sBorderCfgFilePath += _T("\\Border\\") + sBorderFileName + _T(".cfg");
			sRevisionDataFilePath += _T("\\Dra\\") + STRING_T(pSdrFileName) + _T(".ini");
		}

		this->LoadSetting(pIniFilePath , sBorderCfgFilePath);

		vector<STRING_T> oResult;
		const UINT uiCount = GetPrivateProfileInt(sApp.c_str() , _T("Count") , 0 , sRevisionDataFilePath.c_str());
		for(int i = 0;i < int(uiCount);++i)
		{
			SPRINTF_T(szTmp , _T("Data%d") , i);
			if(GetPrivateProfileString(sApp.c_str() , szTmp , _T("") , szBuf , MAX_PATH , sRevisionDataFilePath.c_str()))
			{
				CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
				for(int j = 0;j < int(oResult.size());++j)
				{
					STRNCPY_T(CRevisionChart::m_varRevision.varData[i].szText[j] , oResult[j].c_str() , REV_DATA_BUF_SIZ);
					CRevisionChart::m_varRevision.varData[i].szText[j][REV_DATA_BUF_SIZ - 1] = '\0';
				}
			}
		}

		CRevisionChart::m_varRevision.nRows = (uiCount < CRevisionChart::m_varRevision.nMinCount) ? CRevisionChart::m_varRevision.nMinCount : uiCount;
		if(0 == STRICMP_T(m_szLocation , _T("UpperRight")))
		{
			CRevisionChart::m_varRevision.nX -= CRevisionChart::m_varRevision.nWidth;
		}
		else if(0 == STRICMP_T(m_szLocation ,_T("LowerLeft")))
		{
			CRevisionChart::m_varRevision.nY += (CRevisionChart::m_varRevision.nRows + 1)*CRevisionChart::m_varRevision.nRowHeight;
		}
		else if(0 == STRICMP_T(m_szLocation , _T("LowerRight")))
		{
			CRevisionChart::m_varRevision.nX -= CRevisionChart::m_varRevision.nWidth;
			CRevisionChart::m_varRevision.nY += (CRevisionChart::m_varRevision.nRows + 1)*CRevisionChart::m_varRevision.nRowHeight;
		}
	}

	return true;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-28
    @function   Write
    @return     bool
    @param      OFSTREAM_T& file
    @brief
******************************************************************************/
bool CRevisionChart::Write(OFSTREAM_T& file,const double& dDrawingScale)
{
	bool bRet=false;

	if(file.is_open())
	{
		double dWidth = 0.f;
		double dHeight= 0.f;
		for(int i= CRevisionChart::m_varRevision.nRows-1;i >= 0;--i)
		{
			dWidth = 0.f;
			for(int j=0;j < CRevisionChart::m_varRevision.nColumns;j++)
			{
				file << _T("<TEXT>|");
				file << (CRevisionChart::m_varRevision.nX*dDrawingScale + dWidth + CRevisionChart::m_varRevision.nColWidth[j]*0.5*dDrawingScale) << _T(",");
				file << (CRevisionChart::m_varRevision.nY*dDrawingScale - dHeight- CRevisionChart::m_varRevision.nRowHeight*0.5*dDrawingScale) << _T(",0,");
				file << m_oTextStyle.height()*dDrawingScale << _T(",") << m_oTextStyle.width()*dDrawingScale << _T(",0,");
				const int col = CRevisionChart::m_varRevision.col(CRevisionChart::m_varRevision.szColumn[j]);
				file << CRevisionChart::m_varRevision.varData[i].szText[col] << _T(",Center Center,0|");
				file << m_oLabelProp.level << _T(",") << m_oLabelProp.color << _T(",");
				file << m_oTextStyle << std::endl;
				
				dWidth += CRevisionChart::m_varRevision.nColWidth[j]*dDrawingScale;
				
			}
			dHeight += CRevisionChart::m_varRevision.nRowHeight*dDrawingScale;
		}
		if(3 == CRevisionChart::m_varRevision.uiDisplayType)
		{
			for(int i= CRevisionChart::m_varRevision.nRows;i >= 0;--i)
			{
				file << _T("<LINE>|");
				file << CRevisionChart::m_varRevision.nX*dDrawingScale << _T(",");
				file << (CRevisionChart::m_varRevision.nY*dDrawingScale - CRevisionChart::m_varRevision.nRowHeight*i*dDrawingScale) << _T(",0,");
				file << (CRevisionChart::m_varRevision.nX*dDrawingScale + dWidth) << _T(",");
				file << (CRevisionChart::m_varRevision.nY*dDrawingScale - CRevisionChart::m_varRevision.nRowHeight*i*dDrawingScale) << _T(",0|");
				file << m_oLeaderProp << std::endl;
			}
		}
		
		if(1 != CRevisionChart::m_varRevision.uiDisplayType)
		{
			dWidth = 0.f;
			for(int j=0;j < CRevisionChart::m_varRevision.nColumns;++j)
			{
				file << _T("<TEXT>|");
				file << (CRevisionChart::m_varRevision.nX*dDrawingScale + dWidth + CRevisionChart::m_varRevision.nColWidth[j]*0.5*dDrawingScale) << _T(",");
				file << (CRevisionChart::m_varRevision.nY*dDrawingScale - dHeight- CRevisionChart::m_varRevision.nRowHeight*0.5*dDrawingScale) << _T(",0,");
				file << m_oTextStyle.height()*dDrawingScale << _T(",") << m_oTextStyle.width()*dDrawingScale << _T(",0,");
				file << CRevisionChart::m_varRevision.szLabel[j] << _T(",Center Center,0|");
				file << m_oLabelProp.level << _T(",") << m_oLabelProp.color << _T(",");
				file << m_oTextStyle << std::endl;				
				
				dWidth += CRevisionChart::m_varRevision.nColWidth[j]*dDrawingScale;
			}
			dHeight += CRevisionChart::m_varRevision.nRowHeight*dDrawingScale;
			
			if(3 == CRevisionChart::m_varRevision.uiDisplayType)
			{
				file << _T("<LINE>|");
				file << CRevisionChart::m_varRevision.nX*dDrawingScale << _T(",");
				file << (CRevisionChart::m_varRevision.nY*dDrawingScale - CRevisionChart::m_varRevision.nRowHeight*(CRevisionChart::m_varRevision.nRows + 1)*dDrawingScale) << _T(",0,");
				file << (CRevisionChart::m_varRevision.nX*dDrawingScale + dWidth) << _T(",");
				file << (CRevisionChart::m_varRevision.nY*dDrawingScale - CRevisionChart::m_varRevision.nRowHeight*(CRevisionChart::m_varRevision.nRows + 1)*dDrawingScale) << _T(",0|");
				file << m_oLeaderProp << std::endl;
			}
		}
		
		if(3 == CRevisionChart::m_varRevision.uiDisplayType)
		{
			dWidth = 0.f;
			file << _T("<LINE>|");
			file << (CRevisionChart::m_varRevision.nX*dDrawingScale + dWidth) << _T(",");
			file << (CRevisionChart::m_varRevision.nY*dDrawingScale) << _T(",0,");
			file << (CRevisionChart::m_varRevision.nX*dDrawingScale + dWidth) << _T(",");
			file << (CRevisionChart::m_varRevision.nY*dDrawingScale - dHeight)<< _T(",0|");
			file << m_oLeaderProp << std::endl;
			
			for(int j=0;j < CRevisionChart::m_varRevision.nColumns;j++)
			{
				dWidth += CRevisionChart::m_varRevision.nColWidth[j]*dDrawingScale;
				file << _T("<LINE>|");
				file << (CRevisionChart::m_varRevision.nX*dDrawingScale + dWidth) << _T(",");
				file << (CRevisionChart::m_varRevision.nY*dDrawingScale) << _T(",0,");
				file << (CRevisionChart::m_varRevision.nX*dDrawingScale + dWidth) << _T(",");
				file << (CRevisionChart::m_varRevision.nY*dDrawingScale - dHeight)<< _T(",0|");
				file << m_oLeaderProp << std::endl;
			}
		}
	}

	return bRet;
}

/**	
	@brief	The CRevisionChart::LoadIni function
	@author	humkyung
	@param	sCfgFilePath
	@return	bool	
*/
bool CRevisionChart::LoadSetting(const STRING_T& sIniFilePath , const STRING_T& sCfgFilePath)
{
	static const STRING_T sApp(_T("Revision Chart"));
	{
		TCHAR szBuf[MAX_PATH + 1]={'\0',};
		
		vector<STRING_T> oResult;
		if(GetPrivateProfileString(sApp.c_str() , _T("Width") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());

			for(int i = 0;i < int(oResult.size());++i)
			{
				CRevisionChart::m_varRevision.nColWidth[i] = ATOF_T(oResult[i].c_str());
				CRevisionChart::m_varRevision.nWidth      += CRevisionChart::m_varRevision.nColWidth[i];
			}
		}
		
		CRevisionChart::m_varRevision.nMinCount = GetPrivateProfileInt(sApp.c_str() , _T("RowCount") , 0 , sCfgFilePath.c_str());
		CRevisionChart::m_varRevision.nRowHeight = 0.0;
		if(GetPrivateProfileString(sApp.c_str() , _T("RowHeight") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			CRevisionChart::m_varRevision.nRowHeight = ATOF_T(szBuf);
		}

		if(GetPrivateProfileString(sApp.c_str() , _T("TextStyle") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			m_oTextStyle.Parse(szBuf , sIniFilePath);
		}
		
		if(GetPrivateProfileString(sApp.c_str() , _T("Label_Level") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			m_oLabelProp.level = szBuf;
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Label_Color") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			m_oLabelProp.color = szBuf;
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Label_Style") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			m_oLabelProp.style = szBuf;
		}
	
		if(GetPrivateProfileString(sApp.c_str() , _T("Origin") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			
			if(3 == oResult.size())
			{
				STRNCPY_T(CRevisionChart::m_szLocation , oResult[0].c_str() , MAX_PATH);
				CRevisionChart::m_varRevision.nX = ATOF_T(oResult[1].c_str());
				CRevisionChart::m_varRevision.nY = ATOF_T(oResult[2].c_str());				
			}
		}
		
		if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Level") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			m_oLeaderProp.level = szBuf;
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Color") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			m_oLeaderProp.color = szBuf;
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Style") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			m_oLeaderProp.style = szBuf;
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Weight") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			m_oLeaderProp.weight = szBuf;
		}
		
		if(GetPrivateProfileString(sApp.c_str() , _T("DisplayStyle") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			CRevisionChart::m_varRevision.uiDisplayType = (0 == STRICMP_T(szBuf , _T("Only Data"))) ? 1 : ((0 == STRICMP_T(szBuf , _T("Data+Label"))) ? 2 : 3);
		}
		
		if(GetPrivateProfileString(sApp.c_str() , _T("Label") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			for(int i = 0;i < int(oResult.size());++i)
			{
				STRCPY_T(CRevisionChart::m_varRevision.szLabel[i] , oResult[i].c_str());
			}
			CRevisionChart::m_varRevision.nColumns = oResult.size();
		}

		/// read column names from setting file - 2014.11.17 added by humkyung
		if(GetPrivateProfileString(sApp.c_str() , _T("Column") , _T("") , szBuf , MAX_PATH , sCfgFilePath.c_str()))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			for(int i = 0;i < int(oResult.size());++i)
			{
				STRCPY_T(CRevisionChart::m_varRevision.szColumn[i] , oResult[i].c_str());
			}
		}
		/// up to here
	}

	return true;
}
