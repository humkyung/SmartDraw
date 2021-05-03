#include "StdAfx.h"
#include <assert.h>
#include <tchar.h>
#include <IsTools.h>
#include <IsString.h>
#include "aDraw_View.h"
#include "aDraw_Equipment.h"

#include <fstream>
using namespace std;

ADRAW_IMPLEMENT_FUNC(CaDraw_Equipment , CaDraw_Entity , _T("Equipment"))

CaDraw_Equipment::CaDraw_Equipment(void)
{
	m_pName2 = new STRING_T;
	m_pDesc = new STRING_T;

	m_pShapeList = new list<CylinderShape>();	/// 2014.04.17 added by humkyung
	m_pNozzleList = new list<CaDraw_Nozzle*>();
}

CaDraw_Equipment::~CaDraw_Equipment(void)
{
	try
	{
		if(m_pName) SAFE_DELETE(m_pName);
		if(m_pDesc) SAFE_DELETE(m_pDesc);
		if(m_pShapeList) SAFE_DELETE(m_pShapeList);	/// 2014.04.17 added by humkyung
		if(m_pNozzleList) SAFE_DELETE(m_pNozzleList);
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
void CaDraw_Equipment::Copy(CaDraw_Equipment* unnamed)
{
	assert(unnamed && "unnamed is NULL");
	CaDraw_Entity::Copy(unnamed);

	if(unnamed)
	{
		m_ptOrigin = unnamed->m_ptOrigin;
		(*m_pName2) = *(unnamed->m_pName2);
		(*m_pDesc) = *(unnamed->m_pDesc);
	}
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.01
*/
const TCHAR* CaDraw_Equipment::name2() const
{
	return m_pName2->c_str();
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.01
*/
int CaDraw_Equipment::SetName2(const STRING_T& rName)
{
	*(m_pName2) = rName;
	return ERROR_SUCCESS;
}

/**
	@brief	SetDesc

	@author	HumKyung.BAEK

	@date	2010.06.02
*/
int CaDraw_Equipment::SetDesc(const STRING_T& rDesc)
{
	*(m_pDesc) = rDesc;
	return ERROR_SUCCESS;
}

/**
	@brief	SetDesc

	@author	HumKyung.BAEK

	@date	2010.06.02
*/
const TCHAR* CaDraw_Equipment::desc() const
{
	return m_pDesc->c_str();
}

/**
	@brief	add cylinder shape to shape list
	@author	humkyung
	@date	2014.04.17
*/
int CaDraw_Equipment::AppendShape(const CylinderShape& shape)
{
	m_pShapeList->push_back(shape);
	return ERROR_SUCCESS;
}

/**
	@brief	return iterator of shape list
	@author	humkyung
	@date	2014.04.17
*/
list<CaDraw_Equipment::CylinderShape>* CaDraw_Equipment::GetShapeList()
{
	return (m_pShapeList);
}

/******************************************************************************
    @brief
	@author     humkyung
    @date       2012-02-15
    @return     int
******************************************************************************/
int CaDraw_Equipment::Report(soci::session& oSession,const STRING_T& sViewGuid) const
{
	OSTRINGSTREAM_T oss;
	oss << _T("insert into ") << CaDraw_Equipment::TypeString() << _T(" values('") << sViewGuid << _T("','");
	oss << (m_pName[0]) << _T("','") << (*m_pName2) << _T("','") << (*m_pDesc) << _T("',");
	oss << IsString::TrimedTrailingZero(m_ptOrigin.x()) << _T(",") << IsString::TrimedTrailingZero(m_ptOrigin.y()) << _T(",") << IsString::TrimedTrailingZero(m_ptOrigin.z()) << _T(")");
	oSession << oss.str();

	return ERROR_SUCCESS;
}

/**
	@brief	clear nozzle list
	@author	humkyung
	@date	2016.03.25
*/
int CaDraw_Equipment::ClearNozzleList()
{
	if(NULL != m_pNozzleList) m_pNozzleList->clear();
	return ERROR_SUCCESS;
}

/**
	@brief	add given nozzle to nozzle list
	@author	humkyung
	@date	2016.03.21
*/
int CaDraw_Equipment::AddNozzle(CaDraw_Nozzle* pNozzle)
{
	assert(pNozzle && "pNozzle is null");
	if(NULL != pNozzle)
	{
		m_pNozzleList->push_back(pNozzle);
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	determine this is enable by checking filters
	@author	humkyung
	@date	2016.03.19
*/
bool CaDraw_Equipment::IsEnable(AnnoFilters* pAnnoFilters) const
{
	assert((NULL != pAnnoFilters) && "pAnnoFilters is null");

	if(NULL != pAnnoFilters)
	{
		for(AnnoFilters::const_iterator itr = pAnnoFilters->begin();itr != pAnnoFilters->end();++itr)
		{
			const STRING_T sProp = (*itr)->GetProperty();
			if(_T("Name") == sProp)
			{
				if(false == (*itr)->Evaluate(STRING_T(name()))) return false;
			}
			else if(_T("HasNozzle") == sProp)
			{
				if(false == (*itr)->Evaluate(!m_pNozzleList->empty())) return false;
			}
			else if(_T("None") == sProp)
			{
				if(false == (*itr)->Evaluate(STRING_T())) return false;
			}
		}
	}

	return true;
}