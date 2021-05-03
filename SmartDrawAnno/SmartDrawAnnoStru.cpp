#include "StdAfx.h"
#include <assert.h>
#include <Tokenizer.h>
#include <aDraw_Structure.h>
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoView.h"
#include "SmartDrawAnnoStru.h"
#include "SmartDrawAnnoItem.h"

///using namespace tag_entity;

SMARTDRAW_TAG_IMPLEMENT_FUNC(CaDraw_TagStru , CaDraw_TagEntity , _T("Structure"))

CaDraw_TagStru::CaDraw_TagStru(CaDraw_Entity* unnamed) : CaDraw_TagEntity(unnamed)
{
}

CaDraw_TagStru::~CaDraw_TagStru(void)
{
}

/**	
	@brief	create tag item of tag entity.
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return	
*/
CaDraw_TagItem* CaDraw_TagStru::CreateTagObjects(list<CaDraw_TagItem*>& TagItemList , CSmartDrawAnnoView *pAnnoView)
{
	assert(pAnnoView && "pAnnoView is NULL");
	if(NULL == pAnnoView) return NULL;
	TagItemList.clear();

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	CaDraw_Structure *pDrawStru = static_cast<CaDraw_Structure*>(m_pDrawEntity);
	CIsPoint3d origin = pDrawStru->origin();
	origin = pAnnoView->drawView()->ModelCoord2ViewCoord(origin);

	STRING_T name[2];
	name[0].assign(pDrawStru->name1());
	name[1].assign(pDrawStru->name2());
	
	const CaDraw_View::DIRECTIONT_T direction = pAnnoView->drawView()->GetViewDir();
	if((CaDraw_View::TOP_VIEW_T == direction) || (CaDraw_View::BOTTOM_VIEW_T == direction))
	{
		CIsVect3d vecDir;
		const CaDraw_View::ROTATE_T rotate = pAnnoView->drawView()->GetViewAngle();
		const double c=cos(DEG2RAD(rotate)),s=sin(DEG2RAD(rotate));
		if('\0' != name[0][0])
		{
			vecDir = GetTagDir(direction , 0);
			const double x = vecDir.dx() , y = vecDir.dy();
			vecDir.dx() = x*c - y*s; vecDir.dy() = x*s + y*c;

			CaDraw_TagItem* pTagItem = OnCreateTagObject(pAnnoView , origin , vecDir , DRAFT_STR);
			if(pTagItem)
			{
				vector<STRING_T> oResult;
				CTokenizer<CIsFromString>::Tokenize(oResult , name[0] , CIsFromString(env.m_stru.szLine2Delimiter));
				if(2 == oResult.size())
				{
					pTagItem->name[0]->assign(oResult[0].c_str());
					pTagItem->name[1]->assign(oResult[1].c_str());
				}
				else if(1 == oResult.size())
				{
					pTagItem->name[0]->assign(oResult[0].c_str());
				}

				pTagItem->ptModelOrigin = pDrawStru->origin();
				TagItemList.push_back(pTagItem);
			}
		}

		if('\0' != name[1][0])
		{
			vecDir = GetTagDir(direction , 1);
			const double x = vecDir.dx(), y = vecDir.dy();
			vecDir.dx() = x*c - y*s; vecDir.dy() = x*s + y*c;

			CaDraw_TagItem* pTagItem = OnCreateTagObject(pAnnoView , origin , vecDir , DRAFT_STR);
			if(pTagItem)
			{
				vector<STRING_T> oResult;
				CTokenizer<CIsFromString>::Tokenize(oResult , name[1] , CIsFromString(env.m_stru.szLine2Delimiter));
				if(2 == oResult.size())
				{
					pTagItem->name[0]->assign(oResult[0].c_str());
					pTagItem->name[1]->assign(oResult[1].c_str());
				}
				else if(1 == oResult.size())
				{
					pTagItem->name[0]->assign(oResult[0].c_str());
				}

				pTagItem->ptModelOrigin = pDrawStru->origin();
				TagItemList.push_back(pTagItem);
			}
		}
	}
	else
	{	/// section view
		CIsVect3d vecDir;

		const CaDraw_View::ROTATE_T rotate = pAnnoView->drawView()->GetViewAngle();
		const double c=cos(DEG2RAD(rotate)),s=sin(DEG2RAD(rotate));
		if('\0' != name[0][0])
		{
			vecDir = GetTagDir(direction , 0);
			if(0. == vecDir.dx())
			{ /// NORTH / SOUTH
				double x=vecDir.dx(), y=vecDir.dy();
				vecDir.dx() = x*c - y*s; vecDir.dy() = x*s + y*c;

				CaDraw_TagItem* pTagItem = OnCreateTagObject(pAnnoView , origin , vecDir , DRAFT_STR);
				if(pTagItem)
				{
					vector<STRING_T> oResult;
					CTokenizer<CIsFromString>::Tokenize(oResult , name[0] , CIsFromString(env.m_stru.szLine2Delimiter));
					if(2 == oResult.size())
					{
						pTagItem->name[0]->assign(oResult[0].c_str());
						pTagItem->name[1]->assign(oResult[1].c_str());
					}
					else if(1 == oResult.size())
					{
						pTagItem->name[0]->assign(oResult[0].c_str());
					}
					
					pTagItem->ptModelOrigin = pDrawStru->origin();
					TagItemList.push_back(pTagItem);
				}
			}
		}

		if('\0' != name[1][0])
		{
			vecDir = GetTagDir(direction , 1);
			if(0. == vecDir.dx())
			{
				double x=vecDir.dx() , y=vecDir.dy();
				vecDir.dx() = x*c - y*s; vecDir.dy() = x*s + y*c;

				CaDraw_TagItem* pTagItem = OnCreateTagObject(pAnnoView , origin , vecDir , DRAFT_STR);
				if(pTagItem)
				{
					vector<STRING_T> oResult;
					CTokenizer<CIsFromString>::Tokenize(oResult , name[1] , CIsFromString(env.m_stru.szLine2Delimiter));
					if(2 == oResult.size())
					{
						pTagItem->name[0]->assign(oResult[0].c_str());
						pTagItem->name[1]->assign(oResult[1].c_str());
					}
					else if(1 == oResult.size())
					{
						pTagItem->name[0]->assign(oResult[0].c_str());
					}

					pTagItem->ptModelOrigin = pDrawStru->origin();
					TagItemList.push_back(pTagItem);
				}
			}
		}
	}

	return NULL;
}

/**	
	@brief	
	@author	BHK
	@date	2010.06.03
	@param	
	@return	CaDraw_TagItem*	
*/
CaDraw_TagItem* CaDraw_TagStru::OnCreateTagObject(CSmartDrawAnnoView *pAnnoView, const CIsPoint3d& ptOrigin, const CIsVect3d& vec, const DITEM_E& enmType)
{
	assert(pAnnoView && "pAnnoView is NULL");
	
	CaDraw_StruTagItem* pTagItem=NULL;
	if(pAnnoView)
	{
		if(pTagItem = new CaDraw_StruTagItem)
		{
			CIsPoint3d ptViewOrigin = pAnnoView->drawView()->center();
			double nViewWidth       = pAnnoView->drawView()->GetViewWidth();
			double nViewHeight      = pAnnoView->drawView()->GetViewHeight();

			pTagItem->m_ptOrigin= ptOrigin;
			pTagItem->vecDir  = vec;
			pTagItem->enmType = enmType;
			pTagItem->m_pEnt  = this;
			pTagItem->nLevel  = DIM_LEVEL_1;
			
			if(pTagItem->m_ptOrigin.x() > ptViewOrigin.x() + nViewWidth*0.5)
				pTagItem->m_ptOrigin.x() = ptViewOrigin.x() + nViewWidth*0.5;
			if(pTagItem->m_ptOrigin.x() < ptViewOrigin.x() - nViewWidth*0.5)
				pTagItem->m_ptOrigin.x() = ptViewOrigin.x() - nViewWidth*0.5;
			if(pTagItem->m_ptOrigin.y() > ptViewOrigin.y() + nViewHeight*0.5)
				pTagItem->m_ptOrigin.y() = ptViewOrigin.y() + nViewHeight*0.5;
			if(pTagItem->m_ptOrigin.y() < ptViewOrigin.y() - nViewHeight*0.5)
				pTagItem->m_ptOrigin.y() = ptViewOrigin.y() - nViewHeight*0.5;
		}
	}
	
	return pTagItem;
}

/**	
	@brief	return tag direction corresponding to given name index
	@author	humkyung
	@date	?

	@param	
	@param	

	@remarks\n
	;2004.12.06 - change tag dir when domain is AutoPLANT.
	@return	CIsVect3d	
*/
CIsVect3d CaDraw_TagStru::GetTagDir(CaDraw_View::DIRECTIONT_T direction , const int& nIndex)
{
	CIsVect3d vec;

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	if(env.m_stru.IsEastWest_NorthSouthNameDir())
	{
		(0 == nIndex) ? (vec.dx() = -1,vec.dy() = 0,vec.dz() = 0) : (vec.dx() = 0,vec.dy() = 1,vec.dz() = 0);
	}
	else
	{
		(0 == nIndex) ? (vec.dx() = 0,vec.dy() = 1,vec.dz() = 0) : (vec.dx() = -1,vec.dy() = 0,vec.dz() = 0);
	}

	return vec;
}