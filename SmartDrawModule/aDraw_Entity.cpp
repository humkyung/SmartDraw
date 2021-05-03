#include "StdAfx.h"
#include <assert.h>
#include <tchar.h>
#include <IsTools.h>
#include "aDraw_Entity.h"
#include "aDraw_View.h"

CaDraw_Entity::CaDraw_Entity(void) : m_pView(NULL)
{
	m_pName = new STRING_T;
	m_pModelName = new STRING_T;
}

CaDraw_Entity::~CaDraw_Entity(void)
{
	try
	{
		if(m_pName) SAFE_DELETE(m_pName);
		if(m_pModelName) SAFE_DELETE(m_pModelName);
	}
	catch(...)
	{
	}
}

bool CaDraw_Entity::IsKindOf(const TCHAR* typeString) const
{
	if(0 == STRCMP_T(CaDraw_Entity::TypeString() , typeString)) return true;

	return false;
}

CaDraw_Entity* CaDraw_Entity::Clone()
{
	CaDraw_Entity* p = new CaDraw_Entity;
	p->Copy(this);
	return p;
}

const TCHAR* CaDraw_Entity::TypeString()
{
	static const TCHAR* typeString = _T("CaDraw_Entity");
	return typeString;
}

STRING_T CaDraw_Entity::typeString() const
{
	return CaDraw_Entity::TypeString();
}

CaDraw_Entity* CaDraw_Entity::CreateInstance()
{
	return new CaDraw_Entity;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.19
	@param	
	@return
*/
void CaDraw_Entity::Copy(CaDraw_Entity* unnamed)
{
	assert(unnamed && "unnamed is NULL");

	if(unnamed)
	{
		(*m_pName) = *(unnamed->m_pName);
		(*m_pModelName) = *(unnamed->m_pModelName);
		m_volume = unnamed->m_volume;
		m_ptOrigin = unnamed->m_ptOrigin;
		m_eModelType = unnamed->m_eModelType;

		m_pView = unnamed->m_pView;
	}
}

int CaDraw_Entity::SetOrigin(const CIsPoint3d& origin)
{
	m_ptOrigin = origin;
	return ERROR_SUCCESS;
}

int CaDraw_Entity::SetModelName(const STRING_T& rModelName)
{
	(*m_pModelName) = rModelName;
	
	return ERROR_SUCCESS;
}

STRING_T CaDraw_Entity::GetModelName() const
{
	return (*m_pModelName);
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.24

	@param	

	@return
*/
void CaDraw_Entity::SetModelType(const MODEL_TYPE_T& eModelType)
{
	m_eModelType = eModelType;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.24

	@param	

	@return
*/
CaDraw_Entity::MODEL_TYPE_T CaDraw_Entity::GetModelType() const
{
	return m_eModelType;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   GetModelTypeString
    @return     STRING_T
    @param      const       CaDraw_Entity::MODEL_TYPE_T&
    @param      iModelType
    @brief
******************************************************************************/
STRING_T CaDraw_Entity::GetModelTypeString(const CaDraw_Entity::MODEL_TYPE_T& iModelType)
{
	STRING_T sModelType = _T("");
	
	switch(iModelType)
	{
		case PIPE_MODEL_T:
			sModelType = _T("Pipe");
			break;
		case EQUIPMENT_MODEL_T:
			sModelType = _T("Equipment");
			break;
		case STRUCTURE_MODEL_T:
			sModelType = _T("Structure");
			break;
		case CABLE_T_MODEL_T:
			sModelType = _T("CableT");
			break;
		case KEY_PLAN_MODEL_T:
			sModelType = _T("KeyPlan");
			break;
		case BORDER_MODEL_T:
			sModelType = _T("Border");
			break;
		case MISC_MODEL_T:
			sModelType = _T("Misc");
			break;
	}

	return sModelType;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
CaDraw_Entity::MODEL_TYPE_T CaDraw_Entity::GetModelType(const STRING_T& rModelType)
{
	if(0 == STRICMP_T(_T("Pipe") , rModelType.c_str()))
	{
		return PIPE_MODEL_T;
	}
	else if(0 == STRICMP_T(_T("Equipment") , rModelType.c_str()))
	{
		return EQUIPMENT_MODEL_T;
	}
	else if(0 == STRICMP_T(_T("Structure") , rModelType.c_str()))
	{
		return STRUCTURE_MODEL_T;
	}
	else if(0 == STRICMP_T(_T("CableT") , rModelType.c_str()))
	{
		return CABLE_T_MODEL_T;
	}
	else if(0 == STRICMP_T(_T("Misc") , rModelType.c_str()))
	{
		return MISC_MODEL_T;
	}
	/// 2015.06.02 added by humkyung
	else if(0 == STRICMP_T(_T("SITE") , rModelType.c_str()))
	{
		return SITE_MODEL_T;
	}
	else if(0 == STRICMP_T(_T("ZONE") , rModelType.c_str()))
	{
		return ZONE_MODEL_T;
	}
	/// up to here

	throw exception("unexpected model type");
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.24

	@param	

	@return
*/
void CaDraw_Entity::SetVolume(const CIsVolume& volume)
{
	m_volume = volume;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
CIsVolume CaDraw_Entity::GetVolume() const
{
	return m_volume;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
CaDraw_View* CaDraw_Entity::GetView()
{
	return m_pView;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
void CaDraw_Entity::SetView(CaDraw_View* pView)
{
	assert(pView && "pView is NULL");

	if(pView) m_pView = pView;
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.05
*/
const TCHAR* CaDraw_Entity::name() const
{
	return m_pName->c_str();
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.05
*/
int CaDraw_Entity::SetName(const STRING_T& name)
{
	*(m_pName) = name;
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
CIsPoint3d CaDraw_Entity::origin() const
{
	return m_ptOrigin;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_Entity
    @function   Equal
    @return     bool
    @param      CaDraw_Entity*  pEnt
    @brief
******************************************************************************/
bool CaDraw_Entity::Equal(const CaDraw_Entity* pEnt) const
{
	return false;
}

/******************************************************************************
	@brief
    @author     humkyung
    @date       2012-02-15
    @return     int
******************************************************************************/
int CaDraw_Entity::Report(soci::session& oSession,const STRING_T& oViewGuid) const
{
	return ERROR_SUCCESS;
}

bool CaDraw_Entity::IsEnable(AnnoFilters* pAnnoFilters) const
{
	return true;
}