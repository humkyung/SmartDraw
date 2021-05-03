#include "StdAfx.h"
#include <assert.h>
#include <tchar.h>
#include <IsTools.h>
#include <IsString.h>
#include "aDraw_View.h"
#include "aDraw_Structure.h"

#include <fstream>
using namespace std;

ADRAW_IMPLEMENT_FUNC(CaDraw_Structure , CaDraw_Entity , _T("Structure"))

CaDraw_Structure::CaDraw_Structure(void)
{
	m_pName = new STRING_T[2];
}

CaDraw_Structure::~CaDraw_Structure(void)
{
	try
	{
		if(m_pName)
		{
			delete []m_pName;
			m_pName = NULL;
		}
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
void CaDraw_Structure::Copy(CaDraw_Structure* unnamed)
{
	assert(unnamed && "unnamed is NULL");
	CaDraw_Entity::Copy(unnamed);

	if(unnamed)
	{
		m_ptOrigin = unnamed->m_ptOrigin;
		*(m_pName) = *(unnamed->m_pName);
	}
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.03
*/
const TCHAR* CaDraw_Structure::name2() const
{
	return m_pName[1].c_str();
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.03
*/
const TCHAR* CaDraw_Structure::name1() const
{
	return m_pName->c_str();
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.03
*/
int CaDraw_Structure::SetName1(const STRING_T& rName)
{
	(m_pName[0]) = rName;
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.03
*/
int CaDraw_Structure::SetName2(const STRING_T& rName)
{
	(m_pName[1]) = rName;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_Structure
    @function   operator=
    @return     bool
    @param      const   CaDraw_Structure&
    @param      stru
    @brief
******************************************************************************/
bool CaDraw_Structure::operator==(CaDraw_Structure* rhs) const
{
	assert(rhs && "rhs is NULL");
	bool res = false;

	if(rhs)
	{
		res = this->Equal(rhs);
	}

	return res;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_Structure
    @function   Equal
    @return     bool
    @param      CaDraw_Entity*  pEnt
    @brief
******************************************************************************/
bool CaDraw_Structure::Equal(const CaDraw_Entity* pEnt) const
{
	assert(pEnt && "pEnt is NULL");
	bool res = false;
	if(pEnt && pEnt->IsKindOf(CaDraw_Structure::TypeString()))
	{
		CaDraw_Structure* pStru = (CaDraw_Structure*)(pEnt);
		res = 
			((m_ptOrigin == pStru->m_ptOrigin) && 
			(m_pName[0]  == pStru->m_pName[0]) && 
			(m_pName[1]  == pStru->m_pName[1]));
	}

	return res;
}

/******************************************************************************
    @brief
	@author     humkyung
	@date       2012-02-15
    @return     int
******************************************************************************/
int CaDraw_Structure::Report(soci::session& oSession,const STRING_T& sViewGuid) const
{
	OSTRINGSTREAM_T oss;
	oss << _T("insert into ") << CaDraw_Structure::TypeString() << _T(" values('") << sViewGuid << _T("','") << this->name2() << _T("','") << this->name1() << _T("',");
	const CIsPoint3d ptOrigin = m_pView->ModelCoord2ViewCoord(m_ptOrigin);
	oss << IsString::TrimedTrailingZero(ptOrigin.x()) << _T(",") << IsString::TrimedTrailingZero(ptOrigin.y()) << _T(",") << IsString::TrimedTrailingZero(ptOrigin.z()) << _T(")");
	oSession << oss.str();

	return ERROR_SUCCESS;
}

/**
	@brief	determine this is enable by checking filters
	@author	humkyung
	@date	2016.03.19
*/
bool CaDraw_Structure::IsEnable(AnnoFilters* pAnnoFilters) const
{
	assert((NULL != pAnnoFilters) && "pAnnoFilters is null");

	if(NULL != pAnnoFilters)
	{
		for(AnnoFilters::const_iterator itr = pAnnoFilters->begin();itr != pAnnoFilters->end();++itr)
		{
			const STRING_T sProp = (*itr)->GetProperty();
			if(_T("Name1") == sProp)
			{
				const STRING_T sPropValue = this->name1();
				if(false == (*itr)->Evaluate(sPropValue)) return false;
			}
			else if(_T("Name2") == sProp)
			{
				const STRING_T sPropValue = this->name2();
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