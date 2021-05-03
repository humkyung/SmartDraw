#include "StdAfx.h"
#include <tchar.h>
#include <assert.h>
#include <Tokenizer.h>

#include "SmartDrawAnnoModule.h"
#include "TitleBlockData.h"

#include <fstream>
using namespace std;

CTitleBlockData::CTitleBlockData(void)
{
	m_pDataValueMap = new map<STRING_T , TitleBlockData>;
}

CTitleBlockData::~CTitleBlockData(void)
{
	try
	{
		SAFE_DELETE( m_pDataValueMap );
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-19
    @class      CTitleBlockData
    @function   Load
    @return     bool
    @param      LPCTSTR pIniFilePath
    @param      LPCTSTR pSdrFileName
    @brief
******************************************************************************/
bool CTitleBlockData::Load(LPCTSTR pIniFilePath , LPCTSTR pSdrFileName)
{
	assert(pIniFilePath && pSdrFileName && "pIniFilePath or pSdrFileName is NULL");
	static const STRING_T sApp(_T("Title Block Data"));

	if(pIniFilePath && pSdrFileName)
	{
		TCHAR szTmp[MAX_PATH + 1]={'\0',} , szBuf[MAX_PATH + 1]={'\0',};

		STRING_T sBorderCfgFilePath(pIniFilePath) , sTitleBlockDataFilePath(pIniFilePath);
		if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , _T("") , szBuf , MAX_PATH , pIniFilePath))
		{
			STRING_T sBorderFileName(szBuf);

			int at = sBorderCfgFilePath.rfind(_T("\\"));
			sBorderCfgFilePath = sBorderCfgFilePath.substr(0 , at);
			at = sBorderCfgFilePath.rfind(_T("\\"));
			sBorderCfgFilePath = sBorderCfgFilePath.substr(0 , at);
			sTitleBlockDataFilePath = sBorderCfgFilePath;
			sBorderCfgFilePath += _T("\\Border\\") + sBorderFileName + _T(".cfg");
			sTitleBlockDataFilePath += _T("\\Dra\\") + STRING_T(pSdrFileName) + _T(".ini");
		}

		this->LoadSetting(pIniFilePath , sBorderCfgFilePath);

		for(map<STRING_T , TitleBlockData>::iterator itr = m_pDataValueMap->begin();itr != m_pDataValueMap->end();++itr)
		{
			if(GetPrivateProfileString(sApp.c_str() , itr->first.c_str() , _T("") , szBuf , MAX_PATH , sTitleBlockDataFilePath.c_str()))
			{
				itr->second.sValue.assign( szBuf );
			}
		}

		return true;
	}
	
	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-19
    @class      CTitleBlockData
    @function   Write
    @return     bool
    @param      OFSTREAM_T& ofile
    @brief
******************************************************************************/
bool CTitleBlockData::Write(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	for(map<STRING_T , TitleBlockData>::iterator itr = m_pDataValueMap->begin();itr != m_pDataValueMap->end();++itr)
	{
		if(itr->second.sValue.empty()) continue;

		ofile << _T("<TEXT>|");
		ofile << (itr->second.ptLoc.x()*dDrawingScale) << _T(",");
		ofile << (itr->second.ptLoc.y()*dDrawingScale) << _T(",0,");
		ofile << itr->second.oTextStyle.height()*dDrawingScale << _T(",") << itr->second.oTextStyle.width()*dDrawingScale << _T(",") << itr->second.dRotation << _T(",");
		ofile << CSmartDrawAnnoModule::CSVString(itr->second.sValue) << _T(",") << itr->second.sJustify << _T(",0|");
		ofile << itr->second.oProp.level << _T(",") << itr->second.oProp.color << _T(",") << itr->second.oTextStyle << std::endl;
	}

	return true;
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-19
    @class      CTitleBlockData
    @function   LoadSetting
    @return     bool
    @param      const           STRING_T&
    @param      sIniFilePath    const
    @param      STRING_T&       sCfgFilePath
    @brief
******************************************************************************/
bool CTitleBlockData::LoadSetting(const STRING_T& sIniFilePath , const STRING_T& sBorderCfgFilePath)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	double dBorderScale = 1.0;
	if(GetPrivateProfileString(_T("GENERAL") , _T("Border_Scale") , _T("1") , szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
	{
		dBorderScale = 1.0/ATOF_T(szBuf);
	}
	
	static const STRING_T sApp(_T("Title Block"));
	{
		vector<STRING_T> oResult;
		const UINT uiCount = GetPrivateProfileInt(sApp.c_str() , _T("Count") , 0 , sBorderCfgFilePath.c_str());
		for(int i = 0;i < int(uiCount);++i)
		{
			OSTRINGSTREAM_T oss;
			oss << _T("Data") << i;

			vector<STRING_T> oResult;
			if(GetPrivateProfileString(sApp.c_str() , oss.str().c_str() , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
			{
				CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
				if(8 == oResult.size())
				{
					TitleBlockData oTitleBlockData;
					oTitleBlockData.ptLoc.x() = ATOF_T(oResult[1].c_str())*dBorderScale;
					oTitleBlockData.ptLoc.y() = ATOF_T(oResult[2].c_str())*dBorderScale;
					
					oTitleBlockData.oTextStyle.Parse(oResult[3] , sIniFilePath);

					oTitleBlockData.sJustify = oResult[4];
					oTitleBlockData.dRotation = ATOF_T(oResult[5].c_str());
					oTitleBlockData.oProp.level = oResult[6];
					oTitleBlockData.oProp.color = oResult[7];

					m_pDataValueMap->insert(make_pair(oResult[0] , oTitleBlockData));
				}
			}
		}
	}

	return true;
}