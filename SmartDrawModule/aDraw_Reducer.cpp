#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include "aDraw_Reducer.h"

ADRAW_IMPLEMENT_FUNC(CaDraw_Reducer , CaDraw_Entity , _T("Reducer"))

CaDraw_Reducer::CaDraw_Reducer(void) : m_eType(CaDraw_Reducer::CON)
{
	m_pName = new STRING_T;
	m_dSizeInMetric[0] = m_dSizeInMetric[1] = 0.0;
}

CaDraw_Reducer::~CaDraw_Reducer(void)
{
	try
	{
		if(m_pName) SAFE_DELETE(m_pName);
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
void CaDraw_Reducer::Copy(CaDraw_Reducer* unnamed)
{
	assert(unnamed && "unnamed is NULL");
	CaDraw_Entity::Copy(unnamed);

	if(unnamed)
	{
		m_eType = unnamed->m_eType;
		m_dSize[0] = unnamed->m_dSize[0];
		m_dSize[1] = unnamed->m_dSize[1];
		m_dSizeInMetric[0] = unnamed->m_dSizeInMetric[0];
		m_dSizeInMetric[1] = unnamed->m_dSizeInMetric[1];
	}
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.05
*/
CaDraw_Reducer::ReducerType CaDraw_Reducer::type() const
{
	return m_eType;
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.05
*/
double CaDraw_Reducer::size1() const
{
	return m_dSize[0];
}

/**
	@brief	
	@author	humkyung
	@date	2016.02.12
*/
double CaDraw_Reducer::GetSize1InMetric() const
{
	return m_dSizeInMetric[0];
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.05
*/
double CaDraw_Reducer::size2() const
{
	return m_dSize[1];
}

/**
	@brief	
	@author	humkyung
	@date	2016.02.12
*/
double CaDraw_Reducer::GetSize2InMetric() const
{
	return m_dSizeInMetric[1];
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.05
*/
int CaDraw_Reducer::SetReducerType(const ReducerType& type)
{
	m_eType = type;
	return ERROR_SUCCESS;
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.05
*/
int CaDraw_Reducer::SetSize1(const double& size)
{
	m_dSize[0] = size;
	return ERROR_SUCCESS;
}

/**
	@brief	
	@author	humkyung
	@date	2016.02.12
*/
int CaDraw_Reducer::SetSize1InMetric(const double& dSize)
{
	m_dSizeInMetric[0] = dSize;
	return ERROR_SUCCESS;
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.05
*/
int CaDraw_Reducer::SetSize2(const double& size)
{
	m_dSize[1] = size;
	return ERROR_SUCCESS;
}

/**
	@brief	
	@author	humkyung
	@date	2016.02.12
*/
int CaDraw_Reducer::SetSize2InMetric(const double& dSize)
{
	m_dSizeInMetric[1] = dSize;
	return ERROR_SUCCESS;
}

/**
	@brief	determine this is enable by checking filters
	@author	humkyung
	@date	2016.03.19
*/
bool CaDraw_Reducer::IsEnable(AnnoFilters* pAnnoFilters) const
{
	assert((NULL != pAnnoFilters) && "pAnnoFilters is null");

	if(NULL != pAnnoFilters)
	{
		for(AnnoFilters::const_iterator itr = pAnnoFilters->begin();itr != pAnnoFilters->end();++itr)
		{
			const STRING_T sProp = (*itr)->GetProperty();
			if(_T("None") == sProp)
			{
				if(false == (*itr)->Evaluate(STRING_T())) return false;
			}
		}
	}

	return true;
}