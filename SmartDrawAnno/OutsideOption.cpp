// OutsideOption.cpp: implementation of the COutsideOption class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include "OutsideOption.h"
#include "SmartDrawAnnoEnv.h"

#include <string>
using namespace std;

using namespace tag_option;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * COutsideOption:
 *
 * @return  
 */
COutsideOption::COutsideOption() : m_dPipeEqpDist(100)
{
	m_nDistA = 0.f;
	m_nDistB = 0.f;
	m_nDistC = 0.f;
	m_nDistD = 0.f;
	m_nDistE = 0.f;
	m_nOverlineDist = 0.f;
}

COutsideOption::~COutsideOption()
{

}

/**
	@Parse:
	@author	humkyung
	@param rIniFilePath.c_str() 
	@return void 
 */
void COutsideOption::Parse(const STRING_T& rIniFilePath)
{
	const static STRING_T rApp(_T("OUTSIDE_OPTIONS"));
	const double nUOR=1.;
	
	if(rIniFilePath.c_str())
	{
		TCHAR szBuf[MAX_PATH + 1]={0,};

		//////////////////////////////////////////////////////////////////////////
		//"[OTUSIDE_OPTIONS]"
		if(GetPrivateProfileString(rApp.c_str() , _T("DIST_PIPE/EQP"),_T(""),szBuf,MAX_PATH,rIniFilePath.c_str()))
		{
			m_dPipeEqpDist = ATOF_T(szBuf)*nUOR;
		}

		if(GetPrivateProfileString(rApp.c_str() , _T("INSIDE DIST_PIPE/EQP"),_T("0"),szBuf,MAX_PATH,rIniFilePath.c_str()))
		{
			m_dInsidePipeEqpDist = ATOF_T(szBuf)*nUOR;
		}

		m_nDistA = 10.f;
		if(GetPrivateProfileString(rApp.c_str() ,_T("DIST_A"),_T(""),szBuf,MAX_PATH,rIniFilePath.c_str()))
			m_nDistA = ATOF_T(szBuf)*nUOR;
		
		m_nDistB = 7.f;
		if(GetPrivateProfileString(rApp.c_str() ,_T("DIST_B"),_T(""),szBuf,MAX_PATH,rIniFilePath.c_str()))
			m_nDistB = ATOF_T(szBuf)*nUOR;
		
		m_nDistC = 7.f;
		if(GetPrivateProfileString(rApp.c_str() ,_T("DIST_C"),_T(""),szBuf,MAX_PATH,rIniFilePath.c_str()))
			m_nDistC = ATOF_T(szBuf)*nUOR;

		if(GetPrivateProfileString(rApp.c_str() ,_T("DIST_D"),_T("3"),szBuf,MAX_PATH,rIniFilePath.c_str()))
			m_nDistD = ATOF_T(szBuf);
		
		if(GetPrivateProfileString(rApp.c_str() ,_T("DIST_E"),_T("3"),szBuf,MAX_PATH,rIniFilePath.c_str()))
			m_nDistE = ATOF_T(szBuf);

		if(GetPrivateProfileString(rApp.c_str() ,_T("OVERLINE_DIST"),_T(""),szBuf,MAX_PATH,rIniFilePath.c_str()))
			m_nOverlineDist = ATOF_T(szBuf);
		
		if(GetPrivateProfileString(_T("OUTSIDE_OPTIONS"),_T("DIST_E1"),_T(""),szBuf,256,rIniFilePath.c_str()))
			m_nElevOffset = ATOF_T(szBuf)*nUOR;
		if(GetPrivateProfileString(_T("OUTSIDE_OPTIONS"),_T("DIST_E2"),_T(""),szBuf,256,rIniFilePath.c_str()))
			m_nElevStartDist = ATOF_T(szBuf)*nUOR;
		
		if(GetPrivateProfileString(_T("OUTSIDE_OPTIONS"),_T("DIST_ELEV"),_T(""),szBuf,256,rIniFilePath.c_str()))
		{
			m_dElevEndDist = ATOF_T(szBuf)*nUOR;
		}
	}
}

/**
 * DistBIsZero:
 *
 * @return bool 
 */
bool COutsideOption::DistBIsZero()
{
	return (0 == m_nDistB);
}

/**
 * DistCIsZero:
 *
 * @return bool 
 */
bool COutsideOption::DistCIsZero()
{
	return (0 == m_nDistC);
}

/**
	@brief	
	@author	BHK
	@date	2009.11.05
	@param
	@return	
*/
double COutsideOption::GetOverlineDist()
{
	if(0.f != m_nOverlineDist) return m_nOverlineDist;

	/// for old version.
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	return (env.m_anno.m_nLineTextHeight);
}

/**
	@brief	return distance from base line to given dim. line
	@author	humkyung
	@date	2014.06.14
	@param
	@return	double
*/
double COutsideOption::GetDistToDimLine(const int& eDimLevel) const
{
	double res = 0.0;
	switch(eDimLevel)
	{
		case DIM_LEVEL_0:
			res = m_nDistA;
			break;
		case DIM_LEVEL_1:
			res = (m_nDistA + m_nDistB);
			break;
		case DIM_LEVEL_2:
			res = (m_nDistA + m_nDistB + m_nDistC);
			break;
	}

	return res;
}

/**
	@brief	return distance from base line to pipe/equipment
	@author	humkyung
	@date	2014.11.10
	@param
	@return	double
*/
const double COutsideOption::PipeEqpDist() const
{
	return m_dPipeEqpDist;
}

/**
	@brief	return distance from base line to inside pipe/equipment
	@author	humkyung
	@date	2014.11.10
	@param
	@return	double
*/
const double COutsideOption::InsidePipeEqpDist() const
{
	return m_dInsidePipeEqpDist;
}

/**
	@brief	return distance from base line to elevation tag
	@author	humkyung
	@date	2014.11.10
	@param
	@return	double
*/
const double COutsideOption::ElevDist() const
{
	return m_dElevEndDist;
}