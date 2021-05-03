#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include "aDraw_Elbow.h"

ADRAW_IMPLEMENT_FUNC(CaDraw_Elbow , CaDraw_Entity , _T("CaDraw_Elbow"))

CaDraw_Elbow::CaDraw_Elbow(void)
{
	m_pConnName[0] = new STRING_T;
	m_pConnName[1] = new STRING_T;
}

CaDraw_Elbow::~CaDraw_Elbow(void)
{
	try
	{
		SAFE_DELETE(m_pConnName[0]);
		SAFE_DELETE(m_pConnName[1]);
	}
	catch(...)
	{
	}
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05

	@param	

	@return
*/
void CaDraw_Elbow::Copy(CaDraw_Elbow* unnamed)
{
	assert(unnamed && "unnamed is NULL");
	CaDraw_Entity::Copy(unnamed);

	if(unnamed)
	{
		*(m_pConnName[0]) = *(unnamed->m_pConnName[0]);
		*(m_pConnName[1]) = *(unnamed->m_pConnName[1]);
	}
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
const TCHAR* CaDraw_Elbow::conn_name1() const
{
	return m_pConnName[0]->c_str();
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
const TCHAR* CaDraw_Elbow::conn_name2() const
{
	return m_pConnName[1]->c_str();
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
int CaDraw_Elbow::SetConnName1(const STRING_T& name)
{
	*(m_pConnName[0]) = name;
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
int CaDraw_Elbow::SetConnName2(const STRING_T& name)
{
	*(m_pConnName[1]) = name;
	return ERROR_SUCCESS;
}

/**
	@brief	determine this is enable by checking filters
	@author	humkyung
	@date	2016.03.19
*/
bool CaDraw_Elbow::IsEnable(AnnoFilters* pAnnoFilters) const
{
	return true;
}