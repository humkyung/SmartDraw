// Leader.cpp: implementation of the CDraftLeader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include "DraftLeader.h"
#include "ExtDivision.h"

#include <fstream>
using namespace std;

CDraftLeader::LEADER_ENV_T CDraftLeader::ENV={0,};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**	\brief
*/
CDraftLeader::CDraftLeader(CaDraw_TagItem* pTITEM) : CaDraw_TagEntity(NULL)
{
	m_pTITEM = pTITEM;
	m_ptOrigin = m_pTITEM->m_ptOrigin;
}

CDraftLeader::~CDraftLeader()
{

}

/**	
	@brief	write leader to file.
	@author	BHK
	@date	?
	@param	fp	a parameter of type tstream&
	@param	pDivision	a parameter of type CExtDivision*
	@return	void	
*/
void CDraftLeader::Write(OFSTREAM_T& file,CExtDivision* pDivision)
{
	assert(pDivision && "pDivision is NULL");

	if(pDivision)
	{
		if(pDivision->IsHorizontal())
		{
			double d=0.f;
			
			file << _T("<LINE>|");
			d = m_ptOrigin.x(); file << d << _T(",");
			d = m_ptOrigin.y(); file << d << _T(",0,");
			d = m_ptOrigin.x(); file << d << _T(",");
			d = pDivision->m_lnBaseLine.start().y(); file << d << _T(",0,|");
			file << _T("0,0,0,0,0") << std::endl;
		}
		else if(pDivision->IsVertical())
		{
			double d=0.f;
			
			file << _T("<LINE>|");
			d = m_ptOrigin.x(); file << d << _T(",");
			d = m_ptOrigin.y(); file << d << _T(",0,");
			d = pDivision->m_lnBaseLine.start().x(); file << d << _T(",");
			d = m_ptOrigin.y(); file << d << _T(",0,|");
			file << _T("0,0,0,0,0") << std::endl;
		}
	}
}

/**	
	@brief	The CDraftLeader::ReadEnv function

	@author	BHK

	@date	?

	@param	pszFilePath	a parameter of type const char*

	@return	void	
*/
void CDraftLeader::ReadEnv(const TCHAR* pszFilePath)
{
	assert(pszFilePath && "pszFilePath is NULL");

	if(pszFilePath)
	{
		TCHAR szBuf[MAX_PATH + 1]={'\0',};
		if(GetPrivateProfileString(_T("ANNOTATION_OPTIONS"),_T("LineNo_IntLeaderColor"),_T(""),szBuf,MAX_PATH,pszFilePath))
		{
			STRNCPY_T(CDraftLeader::ENV.szLineColor,szBuf,16);
		}else	STRCPY_T(CDraftLeader::ENV.szLineColor, _T("white"));

		STRCPY_T(CDraftLeader::ENV.szLineStyle , _T("0"));
		STRCPY_T(CDraftLeader::ENV.szLineWeight, _T("0"));
	}
}
