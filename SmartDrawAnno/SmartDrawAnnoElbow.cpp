#include "StdAfx.h"
#include "SmartDrawAnnoElbow.h"

///using namespace tag_entity;

SMARTDRAW_TAG_IMPLEMENT_FUNC(CaDraw_TagElbow , CaDraw_TagEntity , _T("CaDraw_TagElbow"))

CaDraw_TagElbow::CaDraw_TagElbow(CaDraw_Entity* unnamed) : CaDraw_TagEntity(unnamed)
{
}

CaDraw_TagElbow::~CaDraw_TagElbow(void)
{
}

/**	
	@brief	create tag item of tag entity.

	@author	HumKyung.BAEK

	@date	2010.06.08

	@param	

	@return	
*/
CaDraw_TagItem* CaDraw_TagElbow::CreateTagObjects(list<CaDraw_TagItem*>& TagItemList , CSmartDrawAnnoView *pAnnoView)
{
	TagItemList.clear();
	return NULL;
}

/**	
	@brief	

	@author	BHK

	@date	2010.06.08

	@param	

	@return	CaDraw_TagItem*	
*/
CaDraw_TagItem* CaDraw_TagElbow::OnCreateTagObject(CSmartDrawAnnoView *pAnnoView , const CIsPoint3d& ptOrigin,const CIsVect3d& vec,const DITEM_E& enmType)
{
	CaDraw_TagItem* pTagItem=NULL;
	
	return pTagItem;
}