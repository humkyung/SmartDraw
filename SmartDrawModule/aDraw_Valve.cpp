#include "StdAfx.h"
#include "aDraw_Valve.h"

ADRAW_IMPLEMENT_FUNC(CaDraw_Valve , CaDraw_Entity , _T("Valve"))

CaDraw_Valve::CaDraw_Valve(void)
{
	m_pRemarks = new STRING_T;
}

CaDraw_Valve::~CaDraw_Valve(void)
{
	try
	{
		if(m_pRemarks) SAFE_DELETE(m_pRemarks);
	}
	catch(...)
	{
	}
}

/**	
	@brief	
	@author	humkyung
	@date	2015.09.01
	@param	
	@return
*/
void CaDraw_Valve::Copy(CaDraw_Valve* unnamed)
{
	assert(unnamed && "unnamed is NULL");
	CaDraw_Entity::Copy(unnamed);

	if(unnamed)
	{
		*m_pRemarks = *(unnamed->m_pRemarks);
	}
}

/**	
	@brief	return remarks
	@author	humkyung
	@date	2015.09.01
	@param	
	@return
*/
LPCTSTR CaDraw_Valve::remarks() const
{
	return m_pRemarks->c_str();
}

/******************************************************************************
    @brief		report valve data
	@author     humkyung
    @date       2016-02-24
    @return     int
******************************************************************************/
int CaDraw_Valve::Report(soci::session& oSession,const STRING_T& sViewGuid) const
{
	OSTRINGSTREAM_T oss;
	oss << _T("insert into ") << CaDraw_Valve::TypeString() << _T(" values('") << sViewGuid << _T("','");
	oss << (*m_pName) << _T("','") << (*m_pRemarks) << _T("')");
	oSession << oss.str();

	return ERROR_SUCCESS;
}

/**
	@brief	determine this is enable by checking filters
	@author	humkyung
	@date	2016.03.19
*/
bool CaDraw_Valve::IsEnable(AnnoFilters* pAnnoFilters) const
{
	assert((NULL != pAnnoFilters) && "pAnnoFilters is null");

	if(NULL != pAnnoFilters)
	{
		for(AnnoFilters::const_iterator itr = pAnnoFilters->begin();itr != pAnnoFilters->end();++itr)
		{
			const STRING_T sProp = (*itr)->GetProperty();
			if(_T("Name") == sProp)
			{
				const STRING_T sPropValue = this->name();
				if(false == (*itr)->Evaluate(sPropValue)) return false;
			}
		}
	}

	return true;
}