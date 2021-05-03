#include "StdAfx.h"
#include "aDraw_Specialty.h"

ADRAW_IMPLEMENT_FUNC(CaDraw_Specialty , CaDraw_Entity , _T("Specialty"))

CaDraw_Specialty::CaDraw_Specialty(void)
{
}

CaDraw_Specialty::~CaDraw_Specialty(void)
{
}

/******************************************************************************
    @brief		report specialty data
	@author     humkyung
    @date       2016-02-25
    @return     int
******************************************************************************/
int CaDraw_Specialty::Report(soci::session& oSession,const STRING_T& sViewGuid) const
{
	OSTRINGSTREAM_T oss;
	oss << _T("insert into ") << CaDraw_Specialty::TypeString() << _T(" values('") << sViewGuid << _T("','");
	oss << (*m_pName) << _T("')");
	oSession << oss.str();

	return ERROR_SUCCESS;
}

/**
	@brief	determine this is enable by checking filters
	@author	humkyung
	@date	2016.03.19
*/
bool CaDraw_Specialty::IsEnable(AnnoFilters* pAnnoFilters) const
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
			else if(_T("None") == sProp)
			{
				if(false == (*itr)->Evaluate(STRING_T())) return false;
			}
		}
	}

	return true;
}