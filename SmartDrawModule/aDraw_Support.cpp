#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include "aDraw_Support.h"

ADRAW_IMPLEMENT_FUNC(CaDraw_Support , CaDraw_Entity , _T("Support"))

CaDraw_Support::CaDraw_Support(void)
{
	m_pLineNo = new STRING_T;
	m_pShopName = new STRING_T;
	m_pFieldName= new STRING_T;
	m_pCommodityCode = new STRING_T;
}

CaDraw_Support::~CaDraw_Support(void)
{
	try
	{
		if(m_pLineNo)  SAFE_DELETE(m_pLineNo);
		if(m_pShopName)  SAFE_DELETE(m_pShopName);
		if(m_pFieldName) SAFE_DELETE(m_pFieldName);
		if(m_pCommodityCode) SAFE_DELETE(m_pCommodityCode);
	}
	catch(...)
	{
	}
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return
*/
void CaDraw_Support::Copy(CaDraw_Support* unnamed)
{
	assert(unnamed && "unnamed is NULL");
	CaDraw_Entity::Copy(unnamed);

	if(unnamed)
	{
		*m_pLineNo = *(unnamed->m_pLineNo);
		*m_pShopName = *(unnamed->m_pShopName);
		*m_pFieldName= *(unnamed->m_pFieldName);
		*m_pCommodityCode= *(unnamed->m_pCommodityCode);
	}
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return
*/
const TCHAR* CaDraw_Support::line_no() const
{
	return m_pLineNo->c_str();
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return
*/
const TCHAR* CaDraw_Support::shop_name() const
{
	return m_pShopName->c_str();
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return
*/
const TCHAR* CaDraw_Support::field_name() const
{
	return m_pFieldName->c_str();
}

/**	
	@brief	return commodity code
	@author	humkyung
	@date	2014.12.31
	@param	
	@return	const TCHAR*
*/
const TCHAR* CaDraw_Support::commodity_code() const
{
	return m_pCommodityCode->c_str();
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.03

	@param	

	@return
*/
int CaDraw_Support::SetLineNo(const STRING_T& name)
{
	*m_pLineNo = name;
	return ERROR_SUCCESS;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.03

	@param	

	@return
*/
int CaDraw_Support::SetShopName(const STRING_T& name)
{
	*m_pShopName = name;
	return ERROR_SUCCESS;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return
*/
int CaDraw_Support::SetFieldName(const STRING_T& name)
{
	*m_pFieldName = name;
	return ERROR_SUCCESS;
}

/**	
	@brief	set commodity code
	@author	humkyung
	@date	2014.12.31
	@param	
	@return
*/
int CaDraw_Support::SetCommodityCode(const STRING_T& code)
{
	*m_pCommodityCode = code;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		report support data
	@author     humkyung
    @date       2016-02-25
    @return     int
******************************************************************************/
int CaDraw_Support::Report(soci::session& oSession,const STRING_T& sViewGuid) const
{
	OSTRINGSTREAM_T oss;
	oss << _T("insert into ") << CaDraw_Support::TypeString() << _T(" values('") << sViewGuid << _T("','");
	oss << (*m_pName) << _T("','") << (*m_pLineNo) << _T("','") << (*m_pShopName) << _T("','") << (*m_pFieldName) << _T("','");
	oss << (*m_pCommodityCode) << _T("')");
	oSession << oss.str();

	return ERROR_SUCCESS;
}

/**
	@brief	determine this is enable by checking filters
	@author	humkyung
	@date	2016.03.19
*/
bool CaDraw_Support::IsEnable(AnnoFilters* pAnnoFilters) const
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