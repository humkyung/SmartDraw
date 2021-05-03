// AnnoOption.cpp: implementation of the CAnnoOption class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <math.h>
#include "SmartDrawAnnoEnv.h"
#include "AnnoOption.h"

using namespace tag_option;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnnoOption::CAnnoOption() : m_nTagSize(-1)
{
	m_nLineTextHeight = 1.f;
	m_nElevTextHeight = 1.f;
}

CAnnoOption::~CAnnoOption()
{

}

/**
 * Read4PDS3xxx:
 *
 * @param pszFilePath 
 * @return void 
 */
void CAnnoOption::Parse(const TCHAR *pszFilePath)
{
	assert(pszFilePath && "pszFilePath is NULL");
	const static STRING_T rApp(_T("ANNOTATION_OPTIONS"));

	if(pszFilePath)
	{
		TCHAR szBuf[MAX_PATH + 1]={0,};

		m_nTagSize = -1;
		if(GetPrivateProfileString(rApp.c_str(), _T("LineNo_OverSize") , _T("") ,szBuf,MAX_PATH,pszFilePath))
		{
			TCHAR* ptr=NULL;
			if(ptr = STRSTR_T(szBuf, _T("-") ))
			{
				if(CaDraw_TagEnv::INCH_FEET == CaDraw_TagEnv::INPUT_UNIT)
				{
					m_nTagSize = (0.5 * ATOF_T(ptr + 1)) / 25.4;
				}
				else
				{
					m_nTagSize = (0.5 * ATOF_T(ptr + 1));
				}
			}else	m_nTagSize = -1;
		}

		if(GetPrivateProfileString(rApp.c_str(), _T("LineNo_TagSize") , _T("") ,szBuf,MAX_PATH,pszFilePath))
		{
			if(0 == STRCMP_T(szBuf, _T("ALL")))
			{
				m_nTagSize = -1;
			}
		}

		/*if(GetPrivateProfileString(rApp.c_str(), _T("LineNo_ExtTextStyle1") , _T("") ,szBuf,MAX_PATH,pszFilePath))
		{
			m_oLineNoTextStyle.Parse(szBuf , pszFilePath);
		}
		else
		{
			m_oLineNoTextStyle.SetName(_T("3"));
		}*/
		if(GetPrivateProfileString(rApp.c_str(), _T("LineNo_ExtTextHeight1") , _T("") ,szBuf,MAX_PATH,pszFilePath))
			m_nLineTextHeight = ATOF_T(szBuf);
		
		if(GetPrivateProfileString(rApp.c_str(), _T("LineNo_ExtTextStyle2") , _T("") ,szBuf,MAX_PATH,pszFilePath))
		{
			m_oElevTextStyle.Parse(szBuf , pszFilePath);
		}
		else
		{
			m_oElevTextStyle.SetName(_T("3"));
		}

		if(GetPrivateProfileString(rApp.c_str() , _T("LineNo_ExtTextHeight2") , _T("") ,szBuf,MAX_PATH,pszFilePath))
		{
			m_nElevTextHeight = ATOF_T(szBuf);
		}
	}
}

/**	
	@brief	The CAnnoOption::ReadLineNoOverSize function
	@param	pszFilePath	a parameter of type const char *
	@return	void
*/
void CAnnoOption::ReadLineNoOverSize(const TCHAR *pszFilePath)
{
	assert(pszFilePath && "pszFilePath is NULL");
	const static STRING_T rApp( _T("ANNOTATION_OPTIONS") );
	m_nTagSize = -1;

	if(pszFilePath)
	{
		TCHAR szBuf[MAX_PATH] = {0,};
		if(GetPrivateProfileString(rApp.c_str(), _T("LineNo_OverSize") , _T("") ,szBuf,MAX_PATH,pszFilePath))
		{
			TCHAR* ptr=NULL;
			if(ptr = STRSTR_T(szBuf, _T("-") ))
			{
				if(CaDraw_TagEnv::INCH_FEET == CaDraw_TagEnv::INPUT_UNIT)
				{
					m_nTagSize = ATOF_T(ptr + 1) * 0.5 / 25.4;
				}
				else
				{
					m_nTagSize = ATOF_T(ptr + 1) * 0.5;
				}
				
			}else	m_nTagSize = -1;
		}
		if(GetPrivateProfileString(rApp.c_str(), _T("LineNo_TagSize") , _T("") ,szBuf,MAX_PATH,pszFilePath))
		{
			if(0 == STRCMP_T(szBuf, _T("ALL") ))
			{
				m_nTagSize = -1;
			}
		}
	}
}
