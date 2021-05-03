#include "StdAfx.h"
#include "KeyPlanHatching.h"
#include <Tokenizer.h>
#include <IsString.h>

CKeyPlanHatching::CKeyPlanHatching(void) : m_dXScale(1.0),m_dYScale(1.0),m_iAngle(0)
{
}

CKeyPlanHatching::~CKeyPlanHatching(void)
{
}

/**
	@brief	read keyplan hatching setting
	@author	humkyung
	@date	2014.06.07
*/
int CKeyPlanHatching::LoadIni(LPCTSTR pszIniFilePath)
{
	assert(pszIniFilePath && "pszIniFilePath is NULL");

	if(pszIniFilePath)
	{
		TCHAR szTmp[MAX_PATH + 1]={'\0',} , szBuf[MAX_PATH + 1]={'\0',};

		STRING_T sBorderCfgFilePath(pszIniFilePath);
		if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , _T("") , szBuf , MAX_PATH , pszIniFilePath))
		{
			STRING_T sBorderFileName(szBuf);

			int at = sBorderCfgFilePath.rfind(_T("\\"));
			sBorderCfgFilePath = sBorderCfgFilePath.substr(0 , at);
			at = sBorderCfgFilePath.rfind(_T("\\"));
			sBorderCfgFilePath = sBorderCfgFilePath.substr(0 , at);
			sBorderCfgFilePath += _T("\\Border\\") + sBorderFileName + _T(".cfg");
		}

		vector<STRING_T> oResult;
		if(GetPrivateProfileString(_T("Key Plan") , _T("Pos") , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(2 == oResult.size())
			{
				m_ptKeyPlanPosition.Set(ATOF_T(oResult[0].c_str()) , ATOF_T(oResult[1].c_str()));
			}
		}
		
		const STRING_T sApp(_T("KeyPlan Hatching"));
		
		if(GetPrivateProfileString(sApp.c_str() , _T("Model Position") , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(2 == oResult.size())
			{
				m_ptModelPosition.Set(ATOF_T(oResult[0].c_str()) , ATOF_T(oResult[1].c_str()));
			}
		}

		if(GetPrivateProfileString(sApp.c_str() , _T("X Scale") , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_dXScale = ATOF_T(szBuf);
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Y Scale") , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_dYScale = ATOF_T(szBuf);
		}
		m_iAngle = GetPrivateProfileInt(sApp.c_str() , _T("Angle") , 0 , sBorderCfgFilePath.c_str());
		
		if(GetPrivateProfileString(sApp.c_str() , _T("This Drawing Text") , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_sThisDrawingText = szBuf;
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Text Position") , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(2 == oResult.size())
			{
				m_ptTextPosition.Set(ATOF_T(oResult[0].c_str()) , ATOF_T(oResult[1].c_str()));
			}
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Text Style") , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_oTextStyle.Parse(szBuf , pszIniFilePath);
		}

		if(GetPrivateProfileString(sApp.c_str() , _T("Text Level") , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_oTextProp.level.assign(szBuf);
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Text Color") , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_oTextProp.color.assign(szBuf);
		}

		m_oHatchingPntList.clear();

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	write keyplan hatching
	@author	humkyung
	@date	2014.06.07
*/
int CKeyPlanHatching::WriteHatching(OFSTREAM_T& ofile,CSmartDrawAnnoView* pAnnoView,const double& dDrawingScale)
{
	assert(pAnnoView && "pAnnoView is NULL");

	if(pAnnoView)
	{
		CIsPoint3d ptCorners[8];
		CIsVolume volume = pAnnoView->drawView()->GetVolume();
		ptCorners[0].Set(volume.minx(),volume.miny(),volume.minz());
		ptCorners[1].Set(volume.maxx(),volume.miny(),volume.minz());
		ptCorners[2].Set(volume.maxx(),volume.maxy(),volume.minz());
		ptCorners[3].Set(volume.minx(),volume.maxy(),volume.minz());
		ptCorners[4].Set(volume.minx(),volume.miny(),volume.maxz());
		ptCorners[5].Set(volume.maxx(),volume.miny(),volume.maxz());
		ptCorners[6].Set(volume.maxx(),volume.maxy(),volume.maxz());
		ptCorners[7].Set(volume.minx(),volume.maxy(),volume.maxz());
		CIsMatrix3d rmat = pAnnoView->drawView()->GetRMatrix();
		for(int i = 0;i < SIZE_OF_ARRAY(ptCorners);++i)
		{
			ptCorners[0] = rmat*ptCorners[0];
		}

		CIsPoint2d ptMin(DBL_MAX,DBL_MAX),ptMax(-DBL_MAX,-DBL_MAX);
		for(int i = 0;i < SIZE_OF_ARRAY(ptCorners);++i)
		{
			ptMin.x() = min(ptMin.x() , ptCorners[i].x());
			ptMin.y() = min(ptMin.y() , ptCorners[i].y());
			ptMax.x() = max(ptMax.x() , ptCorners[i].x());
			ptMax.y() = max(ptMax.y() , ptCorners[i].y());
		}

		CaDraw_View::ROTATE_T rotate = pAnnoView->drawView()->GetViewAngle();
		if((CaDraw_View::R90_T == rotate) || (CaDraw_View::R270_T == rotate))
		{
			CIsPoint2d ptMid((ptMin+ptMax)*0.5);
			const double dWidth = ptMax.x() - ptMin.x();
			const double dHeight= ptMax.y() - ptMin.y();
			ptMin.Set(ptMid.x() - dHeight*0.5 , ptMin.y() - dWidth*0.5);
			ptMax.Set(ptMid.x() + dHeight*0.5 , ptMin.y() + dWidth*0.5);
		}

		ptMin -= m_ptModelPosition;
		ptMax -= m_ptModelPosition;
		ptMin.x() /= m_dXScale;
		ptMin.y() /= m_dYScale;
		ptMax.x() /= m_dXScale;
		ptMax.y() /= m_dYScale;
		ptMin += m_ptKeyPlanPosition;
		ptMax += m_ptKeyPlanPosition;

		ofile << _T("<HATCHPLINE>|");
		ofile << ptMin.x()*dDrawingScale << _T(",") << ptMin.y()*dDrawingScale << _T(",");
		ofile << ptMax.x()*dDrawingScale << _T(",") << ptMin.y()*dDrawingScale << _T(",");
		ofile << ptMax.x()*dDrawingScale << _T(",") << ptMax.y()*dDrawingScale << _T(",");
		ofile << ptMin.x()*dDrawingScale << _T(",") << ptMax.y()*dDrawingScale << _T(",");
		ofile << ptMin.x()*dDrawingScale << _T(",") << ptMin.y()*dDrawingScale << _T("|");
		ofile << CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oHatchingProp.level << _T(",") << CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oHatchingProp.color;
		ofile << _T(",NULL,NULL,45,") << dDrawingScale << _T(",No") << std::endl;

		m_oHatchingPntList.push_back(ptMin);

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	write "this drawing"
	@author	humkyung
	@date	2014.06.10
*/
int CKeyPlanHatching::WriteThisDrawing(OFSTREAM_T& ofile,const double& dDrawingScale) const
{
	ofile << _T("<TEXT>|");
	ofile << IsString::TrimedTrailingZero(m_ptTextPosition.x()*dDrawingScale) << _T(",");
	ofile << IsString::TrimedTrailingZero(m_ptTextPosition.y()*dDrawingScale) << _T(",0,");
	ofile << IsString::TrimedTrailingZero(m_oTextStyle.height()*0.5*dDrawingScale) << _T(",");
	ofile << IsString::TrimedTrailingZero(m_oTextStyle.height()*0.5*dDrawingScale) << _T(",0,");
	ofile << m_sThisDrawingText << _T(",Center Center,0|");
	ofile << m_oTextProp.level << _T(",") << m_oTextProp.color << _T(",");
	ofile << m_oTextStyle << std::endl;

	for(vector<CIsPoint2d>::const_iterator itr = m_oHatchingPntList.begin();itr != m_oHatchingPntList.end();++itr)
	{
		ofile << _T("<LINE>|");
		ofile << IsString::TrimedTrailingZero(m_ptTextPosition.x()*dDrawingScale) << _T(",") << IsString::TrimedTrailingZero(m_ptTextPosition.y()*dDrawingScale) << _T(",0,");
		ofile << itr->x()*dDrawingScale << _T(",") << itr->y()*dDrawingScale << _T(",0|");
		ofile << _T("1,0,");
		ofile << _T("0,0,0") << std::endl;
	}

	return ERROR_SUCCESS;
}