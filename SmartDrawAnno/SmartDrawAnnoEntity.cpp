#include "StdAfx.h"
#include <assert.h>
#include "SmartDrawAnnoEntity.h"
#include "SmartDrawAnnoItem.h"
#include "SmartDrawAnnoView.h"

///using namespace tag_entity;

CaDraw_TagEntity::CaDraw_TagEntity(CaDraw_Entity* unnamed) : m_pDrawEntity(unnamed)
{
	m_bTagged = false;
	m_pTypeString = new STRING_T;
}

CaDraw_TagEntity::~CaDraw_TagEntity(void)
{
	try
	{
		if(m_pTypeString) delete m_pTypeString;
		m_pTypeString = NULL;
	}
	catch(...)
	{
	}
}

bool CaDraw_TagEntity::IsKindOf(const TCHAR* typeString)
{
	if(0 == STRCMP_T(CaDraw_TagEntity::TypeString() , typeString)) return true;

	return false;
}

CaDraw_TagEntity* CaDraw_TagEntity::Clone()
{
	CaDraw_TagEntity* p = new CaDraw_TagEntity(NULL);
	p->Copy(this);
	return p;
}

const TCHAR* CaDraw_TagEntity::TypeString()
{
	static const TCHAR* typeString = _T("CaDraw_TagEntity");
	return typeString;
}

STRING_T CaDraw_TagEntity::typeString() const
{
	return CaDraw_TagEntity::TypeString();
}

CaDraw_TagEntity* CaDraw_TagEntity::CreateInstance(CaDraw_Entity* unnamed)
{
	return new CaDraw_TagEntity(unnamed);
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.19

	@param	

	@return
*/
void CaDraw_TagEntity::Copy(CaDraw_TagEntity* unnamed)
{
	assert(unnamed && "unnamed is NULL");

	if(unnamed)
	{
		m_pDrawEntity = unnamed->m_pDrawEntity;
	}
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.19

	@param	

	@return
*/
CIsPoint3d CaDraw_TagEntity::ModelCoord2ViewCoord(const CIsPoint3d& pt)
{
	return m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pt);
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_Entity
    @function   GetView
    @return     CaDraw_View*
    @brief
******************************************************************************/
CaDraw_View* CaDraw_TagEntity::GetView() const
{
	if(NULL != m_pDrawEntity)
	{
		return m_pDrawEntity->GetView();
	}

	return NULL;
}

/**	
	@brief	create tag item of tag entity.

	@author	HumKyung.BAEK

	@date	2010.06.03

	@param	

	@return	
*/
CaDraw_TagItem* CaDraw_TagEntity::CreateTagObjects(list<CaDraw_TagItem*>& TagItemList , CSmartDrawAnnoView *pAnnoView)
{
	TagItemList.clear();
	return NULL;
}

/**	
	@brief	create tag item of tag entity.

	@author	HumKyung.BAEK

	@date	2010.05.28

	@param	

	@return	CaDraw_TagItem*
*/
CaDraw_TagItem* CaDraw_TagEntity::OnCreateTagObject(CSmartDrawAnnoView *pAnnoView
					 , const CIsPoint3d& ptOrigin,const CIsVect3d& vec,const DITEM_E& enmType)
{
	CaDraw_TagItem* pTagItem=NULL;
	if(pTagItem = new CaDraw_TagItem)
	{
		const double nWidth = pAnnoView->drawView()->GetViewWidth();
		const double nHeight= pAnnoView->drawView()->GetViewHeight();

		pTagItem->m_ptOrigin= ptOrigin;
		pTagItem->vecDir    = vec;
		pTagItem->enmType   = enmType;
		pTagItem->m_pEnt    = this;

		if(pTagItem->m_ptOrigin.x() > ptOrigin.x() + nWidth*0.5)
			pTagItem->m_ptOrigin.x() = (ptOrigin.x() + nWidth*0.5);
		if(pTagItem->m_ptOrigin.x() < ptOrigin.x() - nWidth*0.5)
			pTagItem->m_ptOrigin.x() = (ptOrigin.x() - nWidth*0.5);
		if(pTagItem->m_ptOrigin.y() > ptOrigin.y() + nHeight*0.5)
			pTagItem->m_ptOrigin.y() = (ptOrigin.y() + nHeight*0.5);
		if(pTagItem->m_ptOrigin.y() < ptOrigin.y() - nHeight*0.5)
			pTagItem->m_ptOrigin.y() = ( ptOrigin.y() - nHeight*0.5);
	}

	return pTagItem;
}