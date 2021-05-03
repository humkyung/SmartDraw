// aDraw_AnnotationView.cpp: implementation of the CSmartDrawAnnoView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <math.h>
#include <IsTools.h>
#include <aDraw_Pipe.h>
#include <aDraw_Equipment.h>
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoView.h"
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoNozzle.h"
#include "SmartDrawAnnoModule.h"
#include "ExternalTerritory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CSmartDrawAnnoView::CSmartDrawAnnoView(CaDraw_View* pView) : m_pDrawView(pView)
{
}


/**	
	@brief
*/
CSmartDrawAnnoView::~CSmartDrawAnnoView()
{
	try
	{
		///Clear();
	}
	catch(...)
	{
	}
}

/**	
	@brief	The CSmartDrawAnnoView::Annotate function
	@author	humkyung
	@date	????.??.??
	@remarks\n
	;check view orientation\n
	;2003.02.19 - support vertical pipe
	;2004.12.20 - modify code to register only one tag point when pipe is match line.
	@return	void	
*/
void CSmartDrawAnnoView::Annotate(const RECT_T& ExtentRect , CExternalTerritory* pTerritory)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	m_pExternalTerritory = pTerritory;
	
	Run4Pipe(ExtentRect);
	Run4Equi(ExtentRect);
	Run4Stru(ExtentRect);
	CreateTagItemForCableTray(ExtentRect);
/*
	for(list<CDraftEDIM*>::iterator itrDraftEDIM=m_plstDraftEDIM->begin();itrDraftEDIM != m_plstDraftEDIM->end();++itrDraftEDIM)
	{
		CIsVect3d vec={0,0,0};

		CaDraw_TagItem* pTagItem=CreateDITEM((*itrDraftEDIM)->ptOrigin,vec,DRAFT_DIM);
		if(pTagItem)
		{
			pTagItem->nLevel = (*itrDraftEDIM)->m_nLevel;
			m_pExternalTerritory->AddTagItem(pTagItem);
		}
	}

	for(list<CDraftETIM*>::iterator itrDraftETIM=m_plstDraftETIM->begin();itrDraftETIM != m_plstDraftETIM->end();++itrDraftETIM)
	{
		CIsVect3d vec={0,0,0};

		CaDraw_TagItem* pTagItem=CreateDITEM((*itrDraftETIM)->ptOrigin,vec,DRAFT_ETIM);
		if(pTagItem)
		{
			pTagItem->name[0]->assign((*itrDraftETIM)->m_szName[0]);
			pTagItem->name[1]->assign((*itrDraftETIM)->m_szName[1]);
			m_pExternalTerritory->AddTagItem(pTagItem);
		}
	}

	//!
	for(list<CDraftEntity*>::iterator itr=m_plstDraftEntity->begin();itr != m_plstDraftEntity->end();++itr)
	{

		CDraftEntity* pEnt = (*itr);
		(*itr)->OnCreateTagObject( this , ExtentRect);
	}
	*/
}

/******************************************************************************
	@brief
    @author     humkyung
    @date       2012-02-21
    @class      CSmartDrawAnnoView
    @function   CreateTagEntities
    @return     int
******************************************************************************/
int CSmartDrawAnnoView::CreateTagEntities()
{
	list<CaDraw_Entity*> oDrawEntList;
	m_pDrawView->GetEntList(&oDrawEntList);

	/// 2012.03.26 added by humkyung
	list<CaDraw_Nozzle*> oNozzleList;
	for(list<CaDraw_Entity*>::const_iterator itr = oDrawEntList.begin();itr != oDrawEntList.end();++itr)
	{
		if((*itr)->IsKindOf(CaDraw_Nozzle::TypeString()))
		{
			oNozzleList.push_back(static_cast<CaDraw_Nozzle*>(*itr));
		}
		else if((*itr)->IsKindOf(CaDraw_Equipment::TypeString()))
		{
			CaDraw_Equipment* pEqui = static_cast<CaDraw_Equipment*>(*itr);
			pEqui->ClearNozzleList();
		}
	}
	for(list<CaDraw_Nozzle*>::const_iterator itr = oNozzleList.begin();itr != oNozzleList.end();++itr)
	{
		for(list<CaDraw_Entity*>::const_iterator jtr = oDrawEntList.begin();jtr != oDrawEntList.end();++jtr)
		{
			if((*jtr)->IsKindOf(CaDraw_Equipment::TypeString()))
			{
				CaDraw_Equipment* pEqui = static_cast<CaDraw_Equipment*>(*jtr);
				if(STRING_T((*itr)->equi_name()) == STRING_T(pEqui->name()))
				{
					pEqui->AddNozzle(*itr);
				}
			}
		}
	}

	for(list<CaDraw_Entity*>::const_iterator itr = oDrawEntList.begin();itr != oDrawEntList.end();++itr)
	{
		CaDraw_TagEntity* pTagEnt = CSmartDrawAnnoModule::CreateTagEntityOf(*itr);
		if(pTagEnt) m_oTagEntList.push_back(pTagEnt);
	}

	/*/// 2012.03.26 added by humkyung
	list<CaDraw_TagNozzle*> oNozzleList;
	for(list<CaDraw_TagEntity*>::const_iterator itr = m_oTagEntList.begin();itr != m_oTagEntList.end();++itr)
	{
		if((*itr)->IsKindOf(CaDraw_TagNozzle::TypeString()))
		{
			oNozzleList.push_back( static_cast<CaDraw_TagNozzle*>(*itr) );
		}
	}
	for(list<CaDraw_TagNozzle*>::const_iterator itr = oNozzleList.begin();itr != oNozzleList.end();++itr)
	{
		for(list<CaDraw_TagEntity*>::const_iterator jtr = m_oTagEntList.begin();jtr != m_oTagEntList.end();++jtr)
		{
			if((*jtr)->IsKindOf(CaDraw_TagEqui::TypeString()))
			{
				CaDraw_TagEqui* pTagEqui = static_cast<CaDraw_TagEqui*>(*jtr);
				if(STRING_T((*itr)->nozzle()->equi_name()) == STRING_T(pTagEqui->equi()->name()))
				{
					pTagEqui->AddTagNozzle( *itr );
				}
			}
		}
	}
	/// up to here*/

	/*///#103:노즐이 없는 EQP Annotation ON/OFF 기능 - 2016.02.19 added by humkyung
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	if(CGenerationOption::OFF_IF_HAS_NO_NOZZLE == env.m_generation.GetItemGenerationType(CaDraw_TagEqui::TypeString()))
	{
		for(list<CaDraw_TagEntity*>::iterator itr = m_oTagEntList.begin();itr != m_oTagEntList.end();)
		{
			if((*itr)->IsKindOf(CaDraw_TagEqui::TypeString()))
			{
				list<CaDraw_TagNozzle*> oTagNozzleList;
				CaDraw_TagEqui* pTagEqui = static_cast<CaDraw_TagEqui*>(*itr);
				pTagEqui->GetTagNozzleList(oTagNozzleList);
				if(oTagNozzleList.empty())
				{
					SAFE_DELETE(*itr);
					itr = m_oTagEntList.erase(itr);
					continue;
				}
			}
			++itr;
		}
	}
	///up to here*/

	return ERROR_SUCCESS;
}

/**	
	@brief	get tag entity list

	@author	HumKyung.BAEK

	@date	2010.06.06

	@return	the tag position
*/
int CSmartDrawAnnoView::GetTagEntList(list<CaDraw_TagEntity*>* pTagEntList) const
{
	assert(pTagEntList && "pTagEntList is NULL");

	if(pTagEntList)
	{
		pTagEntList->clear();
		pTagEntList->insert(pTagEntList->begin() , m_oTagEntList.begin() , m_oTagEntList.end());

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}


/**	
	@brief	get tag position

	@author	HumKyung.BAEK

	\param	ptTag	a parameter of type POINT_T
	\param	ptStart	a parameter of type POINT_T
	\param	dir	a parameter of type int

	\return	the tag position
*/
CIsPoint3d CSmartDrawAnnoView::GetTagPoint(const CIsPoint3d& ptTag,const CIsPoint3d& ptStart,const int& dir)
{
	CIsPoint3d ptRet(ptTag);

	CIsPoint3d ptOrigin = m_pDrawView->center();
	const double nWidth = m_pDrawView->GetViewWidth();
	const double nHeight= m_pDrawView->GetViewHeight();
	CIsVect3d vec(ptTag.x() - ptStart.x(),ptTag.y() - ptStart.y(),ptTag.z() - ptStart.z());
	if(X_DIR == dir)
	{
		double nScale=0.;
		if(ptTag.x() >= (ptOrigin.x() + nWidth*0.5))
		{
			nScale = (ptOrigin.x() + nWidth*0.5 - ptTag.x())/(ptTag.x() - ptStart.x());
			ptRet.Set(
				ptTag.x() + vec.dx()*nScale , 
				ptTag.y() + vec.dy()*nScale , 
				ptTag.z() + vec.dz()*nScale);
		}
		else if(ptTag.x() <= (ptOrigin.x() - nWidth*0.5))
		{
			nScale = (ptOrigin.x() - nWidth*0.5 - ptTag.x())/(ptTag.x() - ptStart.x());
			ptRet.Set(
				ptTag.x() + vec.dx()*nScale ,
				ptTag.y() + vec.dy()*nScale ,
				ptTag.z() + vec.dz()*nScale);
		}
	}
	else if(Y_DIR == dir)
	{
		double nScale=0.;
		if(ptTag.y() >= (ptOrigin.y() + nHeight*0.5))
		{
			nScale = (ptOrigin.y() + nHeight*0.5 - ptStart.y())/(ptTag.y() - ptStart.y());
			ptRet.Set(
				ptStart.x() + vec.dx()*nScale ,
				ptStart.y() + vec.dy()*nScale ,
				ptStart.z() + vec.dz()*nScale);
		}
		else if(ptTag.y() <= (ptOrigin.y() - nHeight*0.5))
		{
			nScale = (ptOrigin.y() - nHeight*0.5 - ptStart.y())/(ptTag.y() - ptStart.y());
			ptRet.Set(
				ptStart.x() + vec.dx()*nScale ,
				ptStart.y() + vec.dy()*nScale ,
				ptStart.z() + vec.dz()*nScale);
		}
	}
	else if(XY_DIR == dir)
	{
		double nScale=0.;
		if((ptRet.x() >= (ptOrigin.x() + nWidth*0.5)))
		{
			nScale = (ptOrigin.x() + nWidth*0.5 - ptStart.x())/(ptRet.x() - ptStart.x());
			ptRet.Set(
				ptStart.x() + vec.dx()*nScale ,
				ptStart.y() + vec.dy()*nScale ,
				ptStart.z() + vec.dz()*nScale);
		}
		else if((ptRet.x() <= (ptOrigin.x() - nWidth*0.5)))
		{
			nScale = (ptOrigin.x() - nWidth*0.5 - ptStart.x())/(ptRet.x() - ptStart.x());
			ptRet.Set(
				ptStart.x() + vec.dx()*nScale ,
				ptStart.y() + vec.dy()*nScale ,
				ptStart.z() + vec.dz()*nScale);
		}

		if((ptRet.y() >= (ptOrigin.y() + nHeight*0.5)))
		{
			nScale = (ptOrigin.y() + nHeight*0.5 - ptStart.y())/(ptRet.y() - ptStart.y());
			ptRet.Set(
				ptStart.x() + vec.dx()*nScale ,
				ptStart.y() + vec.dy()*nScale ,
				ptStart.z() + vec.dz()*nScale);
		}
		else if((ptRet.y() <= (ptOrigin.y() - nHeight*0.5)))
		{
			nScale = (ptOrigin.y() - nHeight*0.5 - ptStart.y())/(ptRet.y() - ptStart.y());
			ptRet.Set(
				ptStart.x() + vec.dx()*nScale ,
				ptStart.y() + vec.dy()*nScale ,
				ptStart.z() + vec.dz()*nScale);
		}
	}

	return ptRet;
}

/**
	@brief	Run4Pipe:
	@author	BHK
	@date	?
	@param
	@return void 
 */
void CSmartDrawAnnoView::Run4Pipe(const RECT_T& ExtentRect)
{
	static const double nTol = 0.001;

	int nIndex = 0;
	CIsPoint3d pt[2];
	for(list<CaDraw_TagEntity*>::iterator itr=m_oTagEntList.begin();itr != m_oTagEntList.end();++itr,++nIndex)
	{
		if(!(*itr)->IsKindOf(CaDraw_TagPipe::TypeString())) continue;
		CaDraw_TagPipe* pTagPipe = static_cast<CaDraw_TagPipe*>((*itr));

		const STRING_T name1 = pTagPipe->pipe()->line_label();
		
		/// NOTICE: z value is model value
		pt[0] = m_pDrawView->ModelCoord2ViewCoord(pTagPipe->pipe()->GetStartPoint());
		pt[0].z() = pTagPipe->pipe()->GetStartPoint().z();
		pt[1] = m_pDrawView->ModelCoord2ViewCoord(pTagPipe->pipe()->GetEndPoint());
		pt[1].z() = pTagPipe->pipe()->GetEndPoint().z();
		
		CIsVect3d vecDir(pt[1] - pt[0]);
		if(!vecDir.IsValid()) continue;
		
		if((fabs(vecDir.dx()) < nTol) && (fabs(vecDir.dy()) < nTol) && (fabs(vecDir.dz()) > nTol))
		{	/// VERTICAL
			Run4VerticalPipe(pt , pTagPipe);
		}
		else if((fabs(vecDir.dz()) < nTol) && (fabs(vecDir.dx()) < nTol))
		{	/// HORIZONTAL Y-DIR
			RunYDir4Pipe(pt , pTagPipe);
		}
		else if((fabs(vecDir.dz()) < nTol) && (fabs(vecDir.dy()) < nTol))
		{	/// HORIZONTAL X-DIR
			RunXDir4Pipe(pt , pTagPipe);
		}
		else if((fabs(vecDir.dz()) < nTol) && (fabs(vecDir.dx()) > nTol) && (fabs(vecDir.dy()) > nTol))
		{	/// HORIZONTAL X-Y DIR
			RunXYDir4Pipe(pt , pTagPipe);
		}
		else if(fabs(vecDir.dz()) > nTol)
		{	/// SLOPE
			Run4SlopePipe(pt , pTagPipe);
		}
	}
}

/**	
	@brief	The CSmartDrawAnnoView::Run4VerticalPipe function
	@author	HumKyung.BAEK
	@param	pTagPipe	a parameter of type CDraftPipe*
	@return	void	
*/
void CSmartDrawAnnoView::Run4VerticalPipe(CIsPoint3d pt[2] , CaDraw_TagPipe* pTagPipe)
{
	assert(pTagPipe && "pTagPipe is NULL");

	if(pTagPipe)
	{
		CIsPoint3d ptTag;

		const double zhigh=(pt[1].z() > pt[0].z()) ? pt[1].z() : pt[0].z();
		if(zhigh >= m_pDrawView->GetViewTopElev())	//! when pipe is cutted by view
		{ /// vertical pipe
			CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

			CIsPoint3d ptOrigin = m_pDrawView->center();
			const double nWidth = m_pDrawView->GetViewWidth();
			const double nHeight= m_pDrawView->GetViewHeight();
			if(	(pt[0].x() >= (ptOrigin.x() + nWidth*0.5 - nWidth*env.m_area.LineNoRange[RIGHT_SIDE])) ||
				(pt[0].x() <= (ptOrigin.x() - nWidth*0.5 + nWidth*env.m_area.LineNoRange[LEFT_SIDE])))
			{
				CIsVect3d vec;
				double nLen[2]={0,};
				nLen[0] = fabs(ptOrigin.x() + nWidth*0.5 - pt[0].x());
				nLen[1] = fabs(ptOrigin.x() - nWidth*0.5 - pt[0].x());
				vec.SetDX(((nLen[0] < nLen[1]) ? 1.f : -1.f));
				
				if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
				{	
					CaDraw_TagItem* pTagItem = pTagPipe->OnCreateTagObject(this , pt[0],vec,DRAFT_DIM);
					if(pTagItem)
					{
						pTagItem->m_nDimStyle = DIM_ONE_SIDE;
						pTagItem->nLevel = DIM_LEVEL_0;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
				else
				{
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , pt[0],vec,DRAFT_PIPE);
					if(pTagItem)
					{
						pTagItem->enmSubType = VERTICAL;
						pTagPipe->GetTagNo(pTagItem->name[0],pt[0],m_pDrawView);
						pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
						pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
						pTagItem->m_pEnt = pTagPipe;
						pTagPipe->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
			}
			
			if(	(pt[0].y() >= (ptOrigin.y() + nHeight*0.5 - nHeight*env.m_area.LineNoRange[TOP_SIDE])) ||
				(pt[0].y() <= (ptOrigin.y() - nHeight*0.5 + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE])))
			{
				CIsVect3d vec;
				double nLen[2]={0,};
				nLen[0] = fabs(ptOrigin.y() + nHeight*0.5 - pt[0].y());
				nLen[1] = fabs(ptOrigin.y() - nHeight*0.5 - pt[0].y());
				vec.SetDY(((nLen[0] < nLen[1]) ? 1.f : -1.f));
				
				if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
				{	
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , pt[0],vec,DRAFT_DIM);
					if(pTagItem)
					{
						pTagItem->m_nDimStyle = DIM_ONE_SIDE;
						pTagItem->nLevel = DIM_LEVEL_0;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
				else
				{
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , pt[0],vec,DRAFT_PIPE);
					if(pTagItem)
					{
						pTagItem->enmSubType = VERTICAL;
						pTagPipe->GetTagNo(pTagItem->name[0],pt[0],m_pDrawView);
						pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
						pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
						pTagItem->m_pEnt = pTagPipe;
						pTagPipe->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
			}
		}
	}
}

/**	
	@brief	The CSmartDrawAnnoView::RunYDir4Pipe function
	@param	pTagPipe	a parameter of type CDraftPipe *
	@update\n
	;2005.02.12	select correct tag point when two points are in tag area. 
	@return	void	
*/
void CSmartDrawAnnoView::RunYDir4Pipe(CIsPoint3d pt[2] , CaDraw_TagPipe *pTagPipe)
{
	assert(pTagPipe && "pTagPipe is NULL");

	if(pTagPipe)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		CIsPoint3d ptOrigin = m_pDrawView->center();
		const double nWidth = m_pDrawView->GetViewWidth();
		const double nHeight= m_pDrawView->GetViewHeight();

		CIsPoint3d ptTag;
		const double nTopSideY   =ptOrigin.y() + nHeight*0.5;
		const double nBottomSideY=ptOrigin.y() - nHeight*0.5;
		if((pt[0].y() >= (nTopSideY - nHeight*env.m_area.LineNoRange[TOP_SIDE])) &&
		   (pt[1].y() >= (nTopSideY - nHeight*env.m_area.LineNoRange[TOP_SIDE])))
		{
			CIsVect3d vec(pt[0].x() - pt[1].x(),pt[0].y() - pt[1].y(),pt[0].z() - pt[1].z());
			vec.Normalize();
			
			if(pt[0].y() < pt[1].y()) swap(pt[0],pt[1]);
			ptTag = GetTagPoint(pt[0],pt[1],Y_DIR);
			
			if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
			{	
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
				if(pTagItem)
				{
					pTagItem->m_nDimStyle = DIM_ONE_SIDE;
					pTagItem->nLevel = DIM_LEVEL_0;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
			else
			{
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
				if(pTagItem)
				{
					pTagItem->enmSubType = HORIZONTAL;
					(pTagPipe)->GetTagNo(pTagItem->name[0],ptTag,m_pDrawView);
					
					pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
					pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
					if(pt[0].y() >= (ptOrigin.y() + nHeight*0.5 - nHeight*env.m_area.LineNoRange[TOP_SIDE]))
					{
						pTagItem->nCL =(int)(SAFE_ROUND(pt[0].z()/CSmartDrawAnnoModule::UNIT,1));
						pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
						pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
					}
					else if(pt[0].y() <= (ptOrigin.y() - nHeight*0.5 + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE]))
					{
						pTagItem->nCL =(int)(SAFE_ROUND(pt[0].z()/CSmartDrawAnnoModule::UNIT,1));
						pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
						pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
					}
					pTagItem->m_pEnt = pTagPipe;
					(pTagPipe)->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
		}
		else if((pt[0].y() <= (nBottomSideY + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE])) &&
			(pt[1].y() <= (nBottomSideY + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE])))
		{
			CIsVect3d vec(pt[0].x() - pt[1].x(),pt[0].y() - pt[1].y(),pt[0].z() - pt[1].z());
			vec.Normalize();
			
			if(pt[0].y() > pt[1].y()) swap(pt[0],pt[1]);
			ptTag = GetTagPoint(pt[0],pt[1],Y_DIR);

			if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
			{	
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
				if(pTagItem)
				{
					pTagItem->m_nDimStyle = DIM_ONE_SIDE;
					pTagItem->nLevel = DIM_LEVEL_0;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
			else
			{
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
				if(pTagItem)
				{
					pTagItem->enmSubType = HORIZONTAL;
					(pTagPipe)->GetTagNo(pTagItem->name[0],ptTag,m_pDrawView);
					
					pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
					pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
					if(pt[0].y() >= (ptOrigin.y() + nHeight*0.5 - nHeight*env.m_area.LineNoRange[TOP_SIDE]))
					{
						pTagItem->nCL =(int)(SAFE_ROUND(pt[0].z()/CSmartDrawAnnoModule::UNIT,1));
						pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
						pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
					}
					else if(pt[0].y() <= (ptOrigin.y() - nHeight*0.5 + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE]))
					{
						pTagItem->nCL =(int)(SAFE_ROUND(pt[0].z()/CSmartDrawAnnoModule::UNIT,1));
						pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
						pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
					}
					pTagItem->m_pEnt = pTagPipe;
					(pTagPipe)->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
		}
		else
		{
			if(	(pt[0].y() >= (nTopSideY - nHeight*env.m_area.LineNoRange[TOP_SIDE])) ||
				(pt[0].y() <= (nBottomSideY + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE])))
			{
				CIsVect3d vec(pt[0].x() - pt[1].x(),pt[0].y() - pt[1].y(),pt[0].z() - pt[1].z());
				vec.Normalize();
				
				ptTag = GetTagPoint(pt[0],pt[1],Y_DIR);
				if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
				{	
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
					if(pTagItem)
					{
						pTagItem->m_nDimStyle = DIM_ONE_SIDE;
						pTagItem->nLevel = DIM_LEVEL_0;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
				else
				{
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
					if(pTagItem)
					{
						pTagItem->enmSubType = HORIZONTAL;
						(pTagPipe)->GetTagNo(pTagItem->name[0],ptTag,m_pDrawView);
						
						pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
						pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
						if(pt[0].y() >= (ptOrigin.y() + nHeight*0.5 - nHeight*env.m_area.LineNoRange[TOP_SIDE]))
						{
							pTagItem->nCL =(int)(SAFE_ROUND(pt[0].z()/CSmartDrawAnnoModule::UNIT,1));
							pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
							pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
						}
						else if(pt[0].y() <= (ptOrigin.y() - nHeight*0.5 + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE]))
						{
							pTagItem->nCL =(int)(SAFE_ROUND(pt[0].z()/CSmartDrawAnnoModule::UNIT,1));
							pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
							pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
						}
						pTagItem->m_pEnt = pTagPipe;
						(pTagPipe)->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
			}
			if(	(pt[1].y() >= (nTopSideY - nHeight*env.m_area.LineNoRange[TOP_SIDE])) ||
				(pt[1].y() <= (nBottomSideY + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE])))
			{
				CIsVect3d vec(pt[1].x() - pt[0].x(),pt[1].y() - pt[0].y(),pt[1].z() - pt[0].z());
				vec.Normalize();
				
				ptTag = GetTagPoint(pt[1],pt[0],Y_DIR);
				if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
				{	
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
					if(pTagItem)
					{
						pTagItem->m_nDimStyle = DIM_ONE_SIDE;
						pTagItem->nLevel = DIM_LEVEL_0;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
				else
				{
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
					if(pTagItem)
					{
						pTagItem->enmSubType = HORIZONTAL;
						(pTagPipe)->GetTagNo(pTagItem->name[0],pt[1],m_pDrawView);
						
						pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
						pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
						pTagItem->nCL =(int)(SAFE_ROUND(ptTag.z()/CSmartDrawAnnoModule::UNIT,1));
						pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
						pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
						pTagItem->m_pEnt = pTagPipe;
						(pTagPipe)->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
			}
		}
	}
}

/**	
	@brief	The CSmartDrawAnnoView::RunXDir4Pipe function

	@param	pTagPipe	a parameter of type CDraftPipe *

	@update\n
	;2005.02.12	select correct tag point when two points are in tag area. 
	
	@return	void	
*/
void CSmartDrawAnnoView::RunXDir4Pipe(CIsPoint3d pt[2] , CaDraw_TagPipe *pTagPipe)
{
	assert(pTagPipe && "pTagPipe is NULL");

	if(pTagPipe)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		CIsPoint3d ptTag;
		
		CIsPoint3d ptOrigin = m_pDrawView->center();
		const double nWidth = m_pDrawView->GetViewWidth();
		const double nHeight= m_pDrawView->GetViewHeight();

		const double nLeftSideX =ptOrigin.x() - nWidth*0.5;
		const double nRightSideX=ptOrigin.x() + nWidth*0.5;
		if((pt[0].x() >= (nRightSideX - nWidth*env.m_area.LineNoRange[RIGHT_SIDE])) &&
		   (pt[1].x() >= (nRightSideX - nWidth*env.m_area.LineNoRange[RIGHT_SIDE])))
		{
			CIsVect3d vec(pt[0].x() - pt[1].x(),pt[0].y() - pt[1].y(),pt[0].z() - pt[1].z());
			vec.Normalize();
			
			if(pt[0].x() < pt[1].x()) swap(pt[0],pt[1]);
			ptTag = GetTagPoint(pt[0],pt[1],X_DIR);
			if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
			{	
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
				if(pTagItem)
				{
					pTagItem->m_nDimStyle = DIM_ONE_SIDE;
					pTagItem->nLevel = DIM_LEVEL_0;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
			else
			{
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
				if(pTagItem)
				{
					pTagItem->enmSubType = HORIZONTAL;
					(pTagPipe)->GetTagNo(pTagItem->name[0],ptTag,m_pDrawView);
					
					pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
					pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
					pTagItem->nCL =(int)(SAFE_ROUND(ptTag.z()/CSmartDrawAnnoModule::UNIT,1));
					pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
					pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
					pTagItem->m_pEnt = pTagPipe;
					(pTagPipe)->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
		}
		else if((pt[0].x() <= (nLeftSideX + nWidth*env.m_area.LineNoRange[LEFT_SIDE])) &&
			(pt[1].x() <= (nLeftSideX + nWidth*env.m_area.LineNoRange[LEFT_SIDE])))
		{
			CIsVect3d vec(pt[0].x() - pt[1].x(),pt[0].y() - pt[1].y(),pt[0].z() - pt[1].z());
			vec.Normalize();
			
			if(pt[0].x() > pt[1].x()) swap(pt[0],pt[1]);
			ptTag = GetTagPoint(pt[0],pt[1],X_DIR);
			if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
			{	
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
				if(pTagItem)
				{
					pTagItem->m_nDimStyle = DIM_ONE_SIDE;
					pTagItem->nLevel = DIM_LEVEL_0;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
			else
			{
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
				if(pTagItem)
				{
					pTagItem->enmSubType = HORIZONTAL;
					(pTagPipe)->GetTagNo(pTagItem->name[0],ptTag,m_pDrawView);
					
					pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
					pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
					pTagItem->nCL =(int)(SAFE_ROUND(ptTag.z()/CSmartDrawAnnoModule::UNIT,1));
					pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
					pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
					pTagItem->m_pEnt = pTagPipe;
					(pTagPipe)->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
		}
		else
		{
			
			if(	(pt[0].x() >= (nRightSideX - nWidth*env.m_area.LineNoRange[RIGHT_SIDE])) ||
				(pt[0].x() <= (nLeftSideX + nWidth*env.m_area.LineNoRange[LEFT_SIDE])))
			{
				CIsVect3d vec(pt[0].x() - pt[1].x(),pt[0].y() - pt[1].y(),pt[0].z() - pt[1].z());
				vec.Normalize();
				
				ptTag = GetTagPoint(pt[0],pt[1],X_DIR);
				if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
				{	
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
					if(pTagItem)
					{
						pTagItem->m_nDimStyle = DIM_ONE_SIDE;
						pTagItem->nLevel = DIM_LEVEL_0;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
				else
				{
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
					if(pTagItem)
					{
						pTagItem->enmSubType = HORIZONTAL;
						(pTagPipe)->GetTagNo(pTagItem->name[0],ptTag,m_pDrawView);
						
						pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
						pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
						pTagItem->nCL =(int)(SAFE_ROUND(ptTag.z()/CSmartDrawAnnoModule::UNIT,1));
						pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
						pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
						pTagItem->m_pEnt = pTagPipe;
						(pTagPipe)->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
			}
			if(	(pt[1].x() >= (nRightSideX - nWidth*env.m_area.LineNoRange[RIGHT_SIDE])) ||
				(pt[1].x() <= (nLeftSideX + nWidth*env.m_area.LineNoRange[LEFT_SIDE])))
			{
				CIsVect3d vec(pt[1].x() - pt[0].x(),pt[1].y() - pt[0].y(),pt[1].z() - pt[0].z());
				vec.Normalize();
				
				ptTag = GetTagPoint(pt[1],pt[0],X_DIR);
				if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
				{	
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
					if(pTagItem)
					{
						pTagItem->m_nDimStyle = DIM_ONE_SIDE;
						pTagItem->nLevel = DIM_LEVEL_0;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
				else
				{
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
					if(pTagItem)
					{
						pTagItem->enmSubType = HORIZONTAL;
						(pTagPipe)->GetTagNo(pTagItem->name[0],ptTag,m_pDrawView);
						
						pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
						pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
						pTagItem->nCL =(int)(SAFE_ROUND(ptTag.z()/CSmartDrawAnnoModule::UNIT,1));
						pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
						pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
						pTagItem->m_pEnt = pTagPipe;
						(pTagPipe)->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
			}
		}
	}
}

/**
	@brief	

	@author	BHK

	@date	2009.11.12

	@param

	@return
*/
void CSmartDrawAnnoView::RunXYDir4Pipe(CIsPoint3d pt[2] , CaDraw_TagPipe *pTagPipe)
{
	assert(pTagPipe && "pTagPipe is NULL");
	
	if(pTagPipe)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		CIsPoint3d ptTag;
		CIsPoint3d ptOrigin = m_pDrawView->center();
		const double nWidth = m_pDrawView->GetViewWidth();
		const double nHeight= m_pDrawView->GetViewHeight();

		if((pt[0].x() >= ptOrigin.x() + nWidth*0.5)  || (pt[0].x() <= ptOrigin.x() - nWidth*0.5) ||
		   (pt[0].y() >= ptOrigin.y() + nHeight*0.5) || (pt[0].y() <= ptOrigin.y() - nHeight*0.5))
		{
			CIsVect3d vec(pt[0].x() - pt[1].x(),pt[0].y() - pt[1].y(),pt[0].z() - pt[1].z());
			vec.Normalize();
			
			ptTag = GetTagPoint(pt[0],pt[1],XY_DIR);
			if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
			{	
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
				if(pTagItem)
				{
					pTagItem->m_nDimStyle = DIM_ONE_SIDE;
					pTagItem->nLevel = DIM_LEVEL_0;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
			else
			{
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
				if(pTagItem)
				{
					pTagItem->enmSubType = HORIZONTAL;
					pTagPipe->GetTagNo(pTagItem->name[0],ptTag,m_pDrawView);
					
					pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
					pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
					pTagItem->nCL =(int)(SAFE_ROUND(ptTag.z()/CSmartDrawAnnoModule::UNIT,1));
					pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - pTagPipe->pipe()->GetRadius(),1));
					pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + pTagPipe->pipe()->GetRadius(),1));
					pTagItem->m_pEnt = pTagPipe;
					pTagPipe->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
		}
		else
		{
			if(	(pt[0].x() >= (ptOrigin.x() + nWidth*0.5 - nWidth*env.m_area.LineNoRange[RIGHT_SIDE])) ||
				(pt[0].x() <= (ptOrigin.x() - nWidth*0.5 + nWidth*env.m_area.LineNoRange[LEFT_SIDE])))
			{
				CIsVect3d vec(pt[0].x() - pt[1].x(),pt[0].y() - pt[1].y(),pt[0].z() - pt[1].z());
				vec.Normalize();
				
				ptTag = GetTagPoint(pt[0],pt[1],XY_DIR);
				if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
				{	
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
					if(pTagItem)
					{
						pTagItem->m_nDimStyle = DIM_ONE_SIDE;
						pTagItem->nLevel = DIM_LEVEL_0;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
				else
				{
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
					if(pTagItem)
					{
						pTagItem->enmSubType = HORIZONTAL;
						pTagPipe->GetTagNo(pTagItem->name[0],ptTag,m_pDrawView);
						
						pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
						pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
						pTagItem->nCL =(int)(SAFE_ROUND(ptTag.z()/CSmartDrawAnnoModule::UNIT,1));
						pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - pTagPipe->pipe()->GetRadius(),1));
						pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + pTagPipe->pipe()->GetRadius(),1));
						pTagItem->m_pEnt = pTagPipe;
						pTagPipe->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
			}
			if(	(pt[0].y() >= (ptOrigin.y() + nHeight*0.5 - nHeight*env.m_area.LineNoRange[TOP_SIDE])) ||
				(pt[0].y() <= (ptOrigin.y() - nHeight*0.5 + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE])))
			{
				CIsVect3d vec(pt[0].x() - pt[1].x(),pt[0].y() - pt[1].y(),pt[0].z() - pt[1].z());
				vec.Normalize();
				
				ptTag = GetTagPoint(pt[0],pt[1],XY_DIR);
				if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
				{	
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
					if(pTagItem)
					{
						pTagItem->m_nDimStyle = DIM_ONE_SIDE;
						pTagItem->nLevel = DIM_LEVEL_0;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
				else
				{
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
					if(pTagItem)
					{
						pTagItem->enmSubType = HORIZONTAL;
						pTagPipe->GetTagNo(pTagItem->name[0],ptTag,m_pDrawView);
						
						pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
						pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
						if(pt[0].y() >= (ptOrigin.y() + nHeight*0.5 - nHeight*env.m_area.LineNoRange[TOP_SIDE]))
						{
							pTagItem->nCL =(int)(SAFE_ROUND(pt[0].z()/CSmartDrawAnnoModule::UNIT,1));
							pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - pTagPipe->pipe()->GetRadius(),1));
							pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + pTagPipe->pipe()->GetRadius(),1));
						}
						else if(pt[0].y() <= (ptOrigin.y() - nHeight*0.5 + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE]))
						{
							pTagItem->nCL =(int)(SAFE_ROUND(pt[0].z()/CSmartDrawAnnoModule::UNIT,1));
							pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - pTagPipe->pipe()->GetRadius(),1));
							pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + pTagPipe->pipe()->GetRadius(),1));
						}
						pTagItem->m_pEnt = pTagPipe;
						pTagPipe->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
			}
		}
		
		if((pt[1].x() >= ptOrigin.x() + nWidth*0.5)  || (pt[1].x() <= ptOrigin.x() - nWidth*0.5) ||
		   (pt[1].y() >= ptOrigin.y() + nHeight*0.5) || (pt[1].y() <= ptOrigin.y() - nHeight*0.5))
		{ 
			CIsVect3d vec(pt[1].x() - pt[0].x(),pt[1].y() - pt[0].y(),pt[1].z() - pt[0].z());
			vec.Normalize();
			
			ptTag = GetTagPoint(pt[1],pt[0],XY_DIR);
			if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
			{	
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
				if(pTagItem)
				{
					pTagItem->m_nDimStyle = DIM_ONE_SIDE;
					pTagItem->nLevel = DIM_LEVEL_0;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
			else
			{
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
				if(pTagItem)
				{
					pTagItem->enmSubType = HORIZONTAL;
					pTagPipe->GetTagNo(pTagItem->name[0],ptTag,m_pDrawView);
					
					pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
					pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
					pTagItem->nCL =(int)(SAFE_ROUND(ptTag.z()/CSmartDrawAnnoModule::UNIT,1));
					pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - pTagPipe->pipe()->GetRadius(),1));
					pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + pTagPipe->pipe()->GetRadius(),1));
					pTagItem->m_pEnt = pTagPipe;
					pTagPipe->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
		}
		else
		{
			if(	(pt[1].x() >= (ptOrigin.x() + nWidth*0.5 - nWidth*env.m_area.LineNoRange[RIGHT_SIDE])) ||
				(pt[1].x() <= (ptOrigin.x() - nWidth*0.5 + nWidth*env.m_area.LineNoRange[LEFT_SIDE])))
			{
				CIsVect3d vec(pt[1].x() - pt[0].x(),pt[1].y() - pt[0].y(),pt[1].z() - pt[0].z());
				vec.Normalize();
				
				ptTag = GetTagPoint(pt[1],pt[0],XY_DIR);
				if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
				{	
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
					if(pTagItem)
					{
						pTagItem->m_nDimStyle = DIM_ONE_SIDE;
						pTagItem->nLevel = DIM_LEVEL_0;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
				else
				{
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
					if(pTagItem)
					{
						pTagItem->enmSubType = HORIZONTAL;
						pTagPipe->GetTagNo(pTagItem->name[0],ptTag,m_pDrawView);
						
						pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
						pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
						pTagItem->nCL =(int)(SAFE_ROUND(ptTag.z()/CSmartDrawAnnoModule::UNIT,1));
						pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - pTagPipe->pipe()->GetRadius(),1));
						pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + pTagPipe->pipe()->GetRadius(),1));
						pTagItem->m_pEnt = pTagPipe;
						pTagPipe->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
			}
			
			if(	(pt[1].y() >= (ptOrigin.y() + nHeight*0.5 - nHeight*env.m_area.LineNoRange[TOP_SIDE])) ||
				(pt[1].y() <= (ptOrigin.y() - nHeight*0.5 + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE])))
			{
				CIsVect3d vec(pt[1].x() - pt[0].x(),pt[1].y() - pt[0].y(),pt[1].z() - pt[0].z());
				vec.Normalize();
				
				ptTag = GetTagPoint(pt[1],pt[0],XY_DIR);
				if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
				{	
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_DIM);
					if(pTagItem)
					{
						pTagItem->m_nDimStyle = DIM_ONE_SIDE;
						pTagItem->nLevel = DIM_LEVEL_0;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
				else
				{
					CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag,vec,DRAFT_PIPE);
					if(pTagItem)
					{
						pTagItem->enmSubType = HORIZONTAL;
						pTagPipe->GetTagNo(pTagItem->name[0],pt[1],m_pDrawView);
						
						pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
						pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
						pTagItem->nCL =(int)(SAFE_ROUND(ptTag.z()/CSmartDrawAnnoModule::UNIT,1));
						pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - pTagPipe->pipe()->GetRadius(),1));
						pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + pTagPipe->pipe()->GetRadius(),1));
						pTagItem->m_pEnt = pTagPipe;
						pTagPipe->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
						m_pExternalTerritory->AddTagItem(pTagItem);
					}
				}
			}
		}
	}
}

/**
 * Run4SlopePipe:
 *
 * @param pTagPipe 
 * @return void 
 */
void CSmartDrawAnnoView::Run4SlopePipe(CIsPoint3d pt[2] , CaDraw_TagPipe *pTagPipe)
{
	assert(pTagPipe && "pTagPipe is NULL");

	if(pTagPipe)
	{
		CIsPoint3d ptTag[2];

		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		CIsPoint3d ptOrigin = m_pDrawView->center();
		const double nWidth = m_pDrawView->GetViewWidth();
		const double nHeight= m_pDrawView->GetViewHeight();

		const double nTopSideY    = ptOrigin.y() + nHeight*0.5;
		const double nBottomSideY = ptOrigin.y() - nHeight*0.5;
		const double nLeftSideX   = ptOrigin.x() - nWidth*0.5;
		const double nRightSideX  = ptOrigin.x() + nWidth*0.5;
		
		CIsVect3d vec(pt[0] - pt[1]);
		if(!vec.IsValid()) return;

		ptTag[0] = GetTagPoint(pt[0],pt[1],XY_DIR);
		if((ptTag[0].y() >= (nTopSideY - nHeight*env.m_area.LineNoRange[TOP_SIDE])) ||
		   (ptTag[0].y() <= (nBottomSideY + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE])) ||
		   (ptTag[0].x() >= (nRightSideX - nWidth*env.m_area.LineNoRange[RIGHT_SIDE])) ||
		   (ptTag[0].x() <= (nLeftSideX + nWidth*env.m_area.LineNoRange[LEFT_SIDE])))
		{
			if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
			{	
				CaDraw_TagItem* pTagItem = pTagPipe->OnCreateTagObject(this , ptTag[0] , vec , DRAFT_DIM);
				if(pTagItem)
				{
					pTagItem->m_nDimStyle = DIM_ONE_SIDE;
					pTagItem->nLevel = DIM_LEVEL_0;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
			else
			{
				CaDraw_TagItem* pTagItem = pTagPipe->OnCreateTagObject(this , ptTag[0],vec,DRAFT_PIPE);
				if(pTagItem)
				{
					pTagItem->enmSubType = SLOPE;
					(pTagPipe)->GetTagNo(pTagItem->name[0],ptTag[0],m_pDrawView);
					
					pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
					pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
					if(pt[0].y() >= (ptOrigin.y() + nHeight*0.5 - nHeight*env.m_area.LineNoRange[TOP_SIDE]))
					{
						pTagItem->nCL =(int)(SAFE_ROUND(ptTag[0].z()/CSmartDrawAnnoModule::UNIT,1));
						pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
						pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
					}
					else if(pt[0].y() <= (ptOrigin.y() - nHeight*0.5 + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE]))
					{
						pTagItem->nCL =(int)(SAFE_ROUND(ptTag[0].z()/CSmartDrawAnnoModule::UNIT,1));
						pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
						pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
					}
					pTagItem->m_pEnt = pTagPipe;
					(pTagPipe)->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
		}

		vec = (pt[1] - pt[0]);
		vec.Normalize();

		ptTag[1] = GetTagPoint(pt[1],pt[0],XY_DIR);
		if((ptTag[1].y() >= (nTopSideY - nHeight*env.m_area.LineNoRange[TOP_SIDE])) ||
		   (ptTag[1].y() <= (nBottomSideY + nHeight*env.m_area.LineNoRange[BOTTOM_SIDE])) ||
		   (ptTag[1].x() >= (nRightSideX - nWidth*env.m_area.LineNoRange[RIGHT_SIDE])) ||
		   (ptTag[1].x() <= (nLeftSideX + nWidth*env.m_area.LineNoRange[LEFT_SIDE])))
		{
			if(LINE_NO_INTERNAL == env.m_pipe.m_ext.eTagArea)
			{	
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag[1],vec,DRAFT_DIM);
				if(pTagItem)
				{
					pTagItem->m_nDimStyle = DIM_ONE_SIDE;
					pTagItem->nLevel = DIM_LEVEL_0;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
			else
			{
				CaDraw_TagItem* pTagItem=pTagPipe->OnCreateTagObject(this , ptTag[1],vec,DRAFT_PIPE);
				if(pTagItem)
				{
					pTagItem->enmSubType = SLOPE;
					(pTagPipe)->GetTagNo(pTagItem->name[0],ptTag[1],m_pDrawView);
					
					pTagItem->ptModelOrigin = pTagPipe->m_ptTag;
					pTagItem->name[1]->assign(pTagPipe->pipe()->line_id());
					pTagItem->nCL =(int)(SAFE_ROUND(ptTag[1].z()/CSmartDrawAnnoModule::UNIT,1));
					pTagItem->nBOP=(int)(SAFE_ROUND(pTagItem->nCL - (pTagPipe)->pipe()->GetRadius(),1));
					pTagItem->nTOP=(int)(SAFE_ROUND(pTagItem->nCL + (pTagPipe)->pipe()->GetRadius(),1));
					pTagItem->m_pEnt = pTagPipe;
					(pTagPipe)->pipe()->m_nTagFlag = CaDraw_Pipe::READY_TAGGING;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
			}
		}
	}
}

/**	
	@brief	The CSmartDrawAnnoView::Run4Equi function
	@author	humkyung
	@date	????.??.??
	@param	
	@return	void	
*/
void CSmartDrawAnnoView::Run4Equi(const RECT_T& ExtentRect)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	CIsPoint3d ptViewOrigin = m_pDrawView->center();
	const double nViewWidth = m_pDrawView->GetViewWidth();
	const double nViewHeight= m_pDrawView->GetViewHeight();

	CIsVect3d vec(1,1,0);
	for(list<CaDraw_TagEntity*>::iterator itr=m_oTagEntList.begin();itr != m_oTagEntList.end();++itr)
	{
		if(!(*itr)->IsKindOf(CaDraw_TagEqui::TypeString())) continue;
		CaDraw_TagEqui* pTagEqui = static_cast<CaDraw_TagEqui*>((*itr));
		CIsPoint3d origin = pTagEqui->equi()->origin();
		origin = m_pDrawView->ModelCoord2ViewCoord(origin);

		if(EQP_EXTERNAL == env.m_equi.m_eTagStyle)
		{
			CaDraw_TagItem* pTagItem = pTagEqui->OnCreateTagObject(this , origin , vec , DRAFT_EQP);
			if(pTagItem)
			{
				pTagEqui->GetTagNo(pTagItem->name[0],pTagItem->name[1]);
				pTagItem->nLevel= DIM_LEVEL_1;
								
				CIsPoint3d ptMin(pTagEqui->equi()->GetVolume().minx() , pTagEqui->equi()->GetVolume().miny() , pTagEqui->equi()->GetVolume().minz());
				CIsPoint3d ptMax(pTagEqui->equi()->GetVolume().maxx() , pTagEqui->equi()->GetVolume().maxy() , pTagEqui->equi()->GetVolume().maxz());
				ptMin = m_pDrawView->ModelCoord2ViewCoord(ptMin);
				ptMax = m_pDrawView->ModelCoord2ViewCoord(ptMax);
				pTagItem->ptMin.Set(min(ptMin.x() , ptMax.x()) , min(ptMin.y() , ptMax.y()) , 0.f);
				pTagItem->ptMax.Set(max(ptMin.x() , ptMax.x()) , max(ptMin.y() , ptMax.y()) , 0.f);

				m_pExternalTerritory->AddTagItem(pTagItem);

				(*itr)->m_bTagged = true;	/// set tagged flag - 2012.02.27 added by humkyung
			}
		}
		else if(EQP_INTERNAL == env.m_equi.m_eTagStyle)
		{ /// only dimension
			CaDraw_TagItem* pTagItem = pTagEqui->OnCreateTagObject(this , origin , vec , DRAFT_DIM);
			if(pTagItem)
			{
				pTagItem->nLevel = (DIM_LEVEL_0 | DIM_LEVEL_1);
				m_pExternalTerritory->AddTagItem(pTagItem);
			}
			
			/// register equipment center line to logic view
			/*

			LINE_T line={0,};
			line.ptStart.x = (*itr)->m_ptMin.x;
			line.ptStart.y = (*itr)->ptOriginInView.y;
			line.ptEnd.x = (*itr)->m_ptMax.x;
			line.ptEnd.y = (*itr)->ptOriginInView.y;
			m_pParent->m_lstEqpCenterline.push_back(line);
			line.ptStart.x = (*itr)->ptOriginInView.x;
			line.ptStart.y = (*itr)->m_ptMin.y;
			line.ptEnd.x = (*itr)->ptOriginInView.x;
			line.ptEnd.y = (*itr)->m_ptMax.y;
			m_pParent->m_lstEqpCenterline.push_back(line);
			*/
		}
		else if(EQP_MIXED == env.m_equi.m_eTagStyle)
		{
			if((origin.x() >= (ptViewOrigin.x() + nViewWidth*0.5  - nViewWidth*env.m_area.Range[RIGHT_SIDE])) ||
			   (origin.x() <= (ptViewOrigin.x() - nViewWidth*0.5  + nViewWidth*env.m_area.Range[LEFT_SIDE])) ||
			   (origin.y() >= (ptViewOrigin.y() + nViewHeight*0.5 - nViewHeight*env.m_area.Range[TOP_SIDE]))||
			   (origin.y() <= (ptViewOrigin.y() - nViewHeight*0.5 + nViewHeight*env.m_area.Range[BOTTOM_SIDE])))
			{
				CaDraw_TagItem* pTagItem = pTagEqui->OnCreateTagObject(this , origin , vec , DRAFT_EQP);
				if(pTagItem)
				{
					pTagEqui->GetTagNo(pTagItem->name[0],pTagItem->name[1]);
					pTagItem->nLevel= DIM_LEVEL_1;
					
					CIsPoint3d ptMin(pTagEqui->equi()->GetVolume().minx() , pTagEqui->equi()->GetVolume().miny() , pTagEqui->equi()->GetVolume().minz());
					CIsPoint3d ptMax(pTagEqui->equi()->GetVolume().maxx() , pTagEqui->equi()->GetVolume().maxy() , pTagEqui->equi()->GetVolume().maxz());
					ptMin = m_pDrawView->ModelCoord2ViewCoord(ptMin);
					ptMax = m_pDrawView->ModelCoord2ViewCoord(ptMax);
					pTagItem->ptMin.Set(min(ptMin.x() , ptMax.x()) , min(ptMin.y() , ptMax.y()) , 0.f);
					pTagItem->ptMax.Set(max(ptMin.x() , ptMax.x()) , max(ptMin.y() , ptMax.y()) , 0.f);
					m_pExternalTerritory->AddTagItem(pTagItem);

					(*itr)->m_bTagged = true;	/// set tagged flag - 2012.02.27 added by humkyung
				}
			}
			else
			{ /// internal에서 처리할 것임.
				CIsPoint3d _origin(origin);
				_origin.x() = (_origin.x() < ptViewOrigin.x()) ? ptViewOrigin.x() - nViewWidth*0.5 : ptViewOrigin.x() + nViewWidth*0.5;
				//! vertical line쪽으로 Dimension을 매기도록 한다.
				CaDraw_TagItem* pTagItem = pTagEqui->OnCreateTagObject(this , _origin , vec , DRAFT_DIM);
				if(pTagItem)
				{
					pTagItem->SetTypeString(_T("EQUP"));
					pTagItem->nLevel = DIM_LEVEL_0;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
				
				pTagItem = pTagEqui->OnCreateTagObject(this , _origin , vec , DRAFT_DIM);
				if(pTagItem)
				{
					pTagItem->SetTypeString(_T("EQUP"));
					pTagItem->nLevel = DIM_LEVEL_1;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
				
				//! horizontal line쪽으로 Dimension을 매기도록 한다.
				_origin = origin;
				_origin.y() = (_origin.y() < ptViewOrigin.y()) ? ptViewOrigin.y() - nViewHeight*0.5 : ptViewOrigin.y() + nViewHeight*0.5;
				if(pTagItem = pTagEqui->OnCreateTagObject(this , _origin , vec , DRAFT_DIM))
				{
					pTagItem->SetTypeString(_T("EQUP"));
					pTagItem->nLevel = DIM_LEVEL_0;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
				_origin.y() = (_origin.y() < ptViewOrigin.y()) ? ptViewOrigin.y() - nViewHeight*0.5 : ptViewOrigin.y() + nViewHeight*0.5;
				if(pTagItem = pTagEqui->OnCreateTagObject(this , _origin , vec , DRAFT_DIM))
				{
					pTagItem->SetTypeString(_T("EQUP"));
					pTagItem->nLevel = DIM_LEVEL_1;
					m_pExternalTerritory->AddTagItem(pTagItem);
				}
				
				/// register equipment center line to logic view
				/*
				LINE_T line={0,};
				line.ptStart.x = (*itr)->m_ptMin.x;
				line.ptStart.y = (*itr)->ptOriginInView.y;
				line.ptEnd.x = (*itr)->m_ptMax.x;
				line.ptEnd.y = (*itr)->ptOriginInView.y;
				m_pParent->m_lstEqpCenterline.push_back(line);
				line.ptStart.x = (*itr)->ptOriginInView.x;
				line.ptStart.y = (*itr)->m_ptMin.y;
				line.ptEnd.x = (*itr)->ptOriginInView.x;
				line.ptEnd.y = (*itr)->m_ptMax.y;
				m_pParent->m_lstEqpCenterline.push_back(line);
				*/
			}
		}
	}
}

/**	
	@brief	The CSmartDrawAnnoView::Run4Stru function

	@author	HumKyung.BAEK

	@date	?

	@return	void	
*/
void CSmartDrawAnnoView::Run4Stru(const RECT_T& ExtentRect)
{
	list<CaDraw_TagItem*> TagItemList;
	for(list<CaDraw_TagEntity*>::iterator itr=m_oTagEntList.begin();itr != m_oTagEntList.end();++itr)
	{
		if(!(*itr)->IsKindOf(CaDraw_TagStru::TypeString())) continue;
		CaDraw_TagStru *pTagStru = static_cast<CaDraw_TagStru*>(*itr);
		pTagStru->CreateTagObjects(TagItemList , this);

		for(list<CaDraw_TagItem*>::iterator jtr = TagItemList.begin();jtr != TagItemList.end();++jtr)
		{
			m_pExternalTerritory->AddTagItem(*jtr);
		}
	}
}

/**	
	@brief	Create tag item for cable tray

	@author	humkyung

	@return	void	
*/
void CSmartDrawAnnoView::CreateTagItemForCableTray(const RECT_T& ExtentRect)
{
	list<CaDraw_TagItem*> oTagItemList;
	for(list<CaDraw_TagEntity*>::iterator itr=m_oTagEntList.begin();itr != m_oTagEntList.end();++itr)
	{
		if(!(*itr)->IsKindOf(CaDraw_TagCableTray::TypeString())) continue;
		(*itr)->CreateTagObjects(oTagItemList , this);	

		/// add tag item to external terriroty
		for(list<CaDraw_TagItem*>::iterator jtr = oTagItemList.begin();jtr != oTagItemList.end();++jtr)
		{
			m_pExternalTerritory->AddTagItem(*jtr);
		}
	}
}

/**	
	@brief	write view information and view match

	@author	BHK

	@date	?

	@param	fp	a parameter of type FILE*

	@remarks\n
	view scale을 소수점 8째 자리까지 출력한다.

	@return	void	
*/
/*
void CSmartDrawAnnoView::Write(FILE* fp)
{
	assert(fp && "fp is NULL");
	static TCHAR szBuf[MAX_PATH]={0,};
	CDraft* pDraft=CDraft::GetInstance();

	if(fp && pDraft)
	{
		CDraftEnv* pEnv=pDraft->GetDraftEnv();

		const int nDomain=pDraft->GetDomain();
		if(ENM_PDS == nDomain)
		{
			sprintf(szBuf,"VIEW,%s,"
				"%.3lf,%.3lf,%.3lf,"
				"%.3lf,%.3lf,%.3lf,"
				"%.3lf,%.3lf,%.3lf,"
				"%.3lf,%.3lf,%.3lf,"
				"%.3lf,%.3lf,%.3lf,"
				"%.3lf,%.3lf,%.8lf,"
				"0,%s\r\n",
				m_szViewName,
				m_ptEdge[0].x*g_nUOR,m_ptEdge[0].y*g_nUOR,m_ptEdge[0].z*g_nUOR,
				m_ptEdge[1].x*g_nUOR,m_ptEdge[1].y*g_nUOR,m_ptEdge[1].z*g_nUOR,
				m_ptEdge[2].x*g_nUOR,m_ptEdge[2].y*g_nUOR,m_ptEdge[2].z*g_nUOR,
				m_ptEdge[3].x*g_nUOR,m_ptEdge[3].y*g_nUOR,m_ptEdge[3].z*g_nUOR,
				ptOrigin.x*g_nUOR,ptOrigin.y*g_nUOR,ptOrigin.z*g_nUOR,
				nWidth*g_nUOR,nHeight*g_nUOR,1./m_nScale,
				((TOP_VIEW == m_enmView) || (BOTTOM_VIEW == m_enmView)) ? "PLAN" : "SECTION");
		}
		else if(ENM_PDMS == nDomain)
		{
			const double nDivider=g_nUOR;
			char szViewDir[8]={0,};

			strcpy(szViewDir,"Top"); // defalut variable
			if(TOP_VIEW == m_enmView) strcpy(szViewDir,"Top");
			else if(BOTTOM_VIEW == m_enmView) strcpy(szViewDir,"Bottom");
			else if(FRONT_VIEW == m_enmView) strcpy(szViewDir,"Front");
			else if(BACK_VIEW == m_enmView) strcpy(szViewDir,"Back");
			else if(LEFT_VIEW == m_enmView) strcpy(szViewDir,"Left");
			else if(RIGHT_VIEW == m_enmView) strcpy(szViewDir,"Right");

			char szViewType[16] = {'\0',};
			switch(m_ViewType)
			{
				case CSmartDrawAnnoView::RECTANGLE_VIEW:
					strcpy(szViewType , "RECTANGLE");
					break;
				case CSmartDrawAnnoView::QUADRANT1_VIEW:
					strcpy(szViewType , "Q1");
					break;
				case CSmartDrawAnnoView::QUADRANT2_VIEW:
					strcpy(szViewType , "Q2");
					break;
				case CSmartDrawAnnoView::QUADRANT3_VIEW:
					strcpy(szViewType , "Q3");
					break;
				case CSmartDrawAnnoView::QUADRANT4_VIEW:
					strcpy(szViewType , "Q4");
					break;
			};

			sprintf(szBuf,"VIEW,%s,"
				"%.3lf,%.3lf,"		// origin
				"%.3lf,%.3lf,"		// width,height
				"%.8lf,%d,"		// scale,angle
				"%.3lf,%.3lf,%.3lf,"	// volume min
				"%.3lf,%.3lf,%.3lf,"	// volume max
				"%s,%s\r\n",		// view dir,view type
				m_szViewName,
				ptOrigin.x/nDivider,ptOrigin.y/nDivider,
				nWidth/nDivider,nHeight/nDivider,
				1./m_nScale,m_nAngle,
				m_volModel.x[0]/nDivider,m_volModel.y[0]/nDivider,m_volModel.z[0]/nDivider,
				m_volModel.x[1]/nDivider,m_volModel.y[1]/nDivider,m_volModel.z[1]/nDivider,
				szViewDir , szViewType);
		}
		else if(ENM_AP == nDomain)
		{
			const double nDivider=1.;
			if(0L == CDraftEnv::m_nVer)
			{
				sprintf(szBuf,"VIEW,%s,"
					"%.3lf,%.3lf,"
					"%.3lf,%.3lf,"
					"%.8lf,%d,"	/// scale , angle
					"%s\r\n",
					m_szViewName,
					ptOrigin.x/nDivider,ptOrigin.y/nDivider,
					nWidth/nDivider,nHeight/nDivider,
					1./m_nScale,m_nAngle,
					((TOP_VIEW == m_enmView) || (BOTTOM_VIEW == m_enmView)) ? "Top" : "Section");
			}
			else
			{
				char szViewDir[8]={0,};
				
				strcpy(szViewDir,"Top"); // defalut variable
				if(TOP_VIEW == m_enmView) strcpy(szViewDir,"Top");
				else if(BOTTOM_VIEW == m_enmView) strcpy(szViewDir,"Bottom");
				else if(FRONT_VIEW == m_enmView) strcpy(szViewDir,"Front");
				else if(BACK_VIEW == m_enmView) strcpy(szViewDir,"Back");
				else if(LEFT_VIEW == m_enmView) strcpy(szViewDir,"Left");
				else if(RIGHT_VIEW == m_enmView) strcpy(szViewDir,"Right");

				char szViewType[16] = {'\0',};
				switch(m_ViewType)
				{
					case CSmartDrawAnnoView::RECTANGLE_VIEW:
						strcpy(szViewType , "RECTANGLE");
						break;
					case CSmartDrawAnnoView::QUADRANT1_VIEW:
						strcpy(szViewType , "Q1");
						break;
					case CSmartDrawAnnoView::QUADRANT2_VIEW:
						strcpy(szViewType , "Q2");
						break;
					case CSmartDrawAnnoView::QUADRANT3_VIEW:
						strcpy(szViewType , "Q3");
						break;
					case CSmartDrawAnnoView::QUADRANT4_VIEW:
						strcpy(szViewType , "Q4");
						break;
				};
				
				sprintf(szBuf,"VIEW,%s,"	// view name
					"%.3lf,%.3lf,"		// origin
					"%.3lf,%.3lf,"		// width,height
					"%.8lf,%d,"		// scale,angle
					"%.3lf,%.3lf,%.3lf,"	// volume min
					"%.3lf,%.3lf,%.3lf,"	// volume max
					"%s,%s\r\n",		// view dir,view type
					m_szViewName,
					ptOrigin.x/nDivider,ptOrigin.y/nDivider,
					nWidth/nDivider,nHeight/nDivider,
					1./m_nScale,m_nAngle,
					m_volModel.x[0]/nDivider,m_volModel.y[0]/nDivider,m_volModel.z[0]/nDivider,
					m_volModel.x[1]/nDivider,m_volModel.y[1]/nDivider,m_volModel.z[1]/nDivider,
					szViewDir,szViewType
					);
			}
		}
		else if(ENM_CADWORX == nDomain)
		{
			char szViewType[8]={0,};
			
			strcpy(szViewType,"Top"); // defalut variable
			if(TOP_VIEW == m_enmView) strcpy(szViewType,"Top");
			else if(BOTTOM_VIEW == m_enmView) strcpy(szViewType,"Bottom");
			else if(FRONT_VIEW == m_enmView) strcpy(szViewType,"Front");
			else if(BACK_VIEW == m_enmView) strcpy(szViewType,"Back");
			else if(LEFT_VIEW == m_enmView) strcpy(szViewType,"Left");
			else if(RIGHT_VIEW == m_enmView) strcpy(szViewType,"Right");
			
			sprintf(szBuf,"VIEW,%s,"
				"%.6lf,%.6lf,"		// origin
				"%.6lf,%.6lf,"		// width,height
				"%.8lf,%d,"		// scale,angle
				"%.6lf,%.6lf,%.6lf,"	// volume min
				"%.6lf,%.6lf,%.6lf,"	// volume max
				"%s\r\n",		// view type
				m_szViewName,
				ptOrigin.x,ptOrigin.y,
				nWidth,nHeight,
				1./m_nScale,m_nAngle,
				m_volModel.x[0],m_volModel.y[0],m_volModel.z[0],
				m_volModel.x[1],m_volModel.y[1],m_volModel.z[1],
				szViewType);
		}

		fprintf(fp,szBuf);
	}
}
*/

/**	
	@brief	The CSmartDrawAnnoView::Clear function

	@author	BHK

	@date	?

	@param

	@return	void	
*/
/*
void CSmartDrawAnnoView::Clear()
{
	if(NULL != m_plstDraftPipe) delete m_plstDraftPipe;
	m_plstDraftPipe = NULL;
	
	if(NULL != m_plstDraftEqui) delete m_plstDraftEqui;
	m_plstDraftEqui = NULL;
	
	if(NULL != m_plstDraftStru) delete m_plstDraftStru;
	m_plstDraftStru = NULL;

	if(NULL != m_pExternalTerritory->m_plstTagItem) delete m_pExternalTerritory->m_plstTagItem;
	m_pExternalTerritory->m_plstTagItem = NULL;

	if(NULL != m_plstDraftETIM) delete m_plstDraftETIM;
	m_plstDraftETIM = NULL;

	if(NULL != m_plstDraftEDIM) delete m_plstDraftEDIM;
	m_plstDraftEDIM = NULL;

	if(NULL != m_plstDraftEntity) delete m_plstDraftEntity;
	m_plstDraftEntity = NULL;
}
*/

/**
	@brief	GetCoordValue:

	@author	BHK

	@date	?

	@param	where 

	@return const double 
 */
/*
const double CSmartDrawAnnoView::GetCoordValue(int where)
{
	double nCoord=0.;
	
	VOLUME_T volume=GetModelVolume();
	int nViewType=GetViewDir();
	
	const double nAllCoords[8][6]=
	{
	///       TOP_VIEW  ,BOTTOM_VIEW,FRONT_VIEW , BACK_VIEW , LEFT_VIEW ,RIGHT_VIEW
		{volume.x[0],volume.x[0],volume.x[0],volume.x[1],volume.y[1],volume.y[0]}, /// BOTTOM_LEFT
		{volume.x[1],volume.x[1],volume.x[1],volume.x[0],volume.y[0],volume.y[1]}, /// BOTTOM_RIGHT
		{volume.y[0],volume.y[1],volume.z[0],volume.z[0],volume.z[0],volume.z[0]}, /// RIGHT_DOWN
		{volume.y[1],volume.y[0],volume.z[1],volume.z[1],volume.z[1],volume.z[1]}, /// RIGHT_UP
		{volume.x[1],volume.x[1],volume.x[1],volume.x[0],volume.y[0],volume.y[1]}, /// TOP_RIGHT
		{volume.x[0],volume.x[0],volume.x[0],volume.x[1],volume.y[1],volume.y[0]}, /// TOP_LEFT
		{volume.y[1],volume.y[0],volume.z[1],volume.z[1],volume.z[1],volume.z[1]}, /// LEFT_UP
		{volume.y[0],volume.y[1],volume.z[0],volume.z[0],volume.z[0],volume.z[0]}  /// LEFT_DOWN
	};

	CDraft* pDraft=CDraft::GetInstance();
	CDraftEnv* pEnv = pDraft->GetDraftEnv();
	double nCoords[8]={0.,};
	for(int i=0;i < 8;i++) nCoords[i] = nAllCoords[i][nViewType] * pEnv->m_nUnitConvFactor;
	
	double nRotate=GetViewAngle();
	double nTmp[2]={0,};
	if(fabs(nRotate - 90.) < 1.)
	{
		nTmp[0] = nCoords[6],nTmp[1] = nCoords[7];
		memmove(&(nCoords[2]),nCoords,sizeof(double)*6);
		nCoords[0] = nTmp[0];nCoords[1] = nTmp[1];
	}
	else if(fabs(nRotate - 180.) < 1.)
	{
		nTmp[0] = nCoords[6],nTmp[1] = nCoords[7];
		memmove(&(nCoords[2]),nCoords,sizeof(double)*6);
		nCoords[0] = nTmp[0];nCoords[1] = nTmp[1];
		nTmp[0] = nCoords[6],nTmp[1] = nCoords[7];
		memmove(&(nCoords[2]),nCoords,sizeof(double)*6);
		nCoords[0] = nTmp[0];nCoords[1] = nTmp[1];
	}
	else if(fabs(nRotate - 270.) < 1.)
	{
		nTmp[0] = nCoords[6],nTmp[1] = nCoords[7];
		memmove(&(nCoords[2]),nCoords,sizeof(double)*6);
		nCoords[0] = nTmp[0];nCoords[1] = nTmp[1];
		nTmp[0] = nCoords[6],nTmp[1] = nCoords[7];
		memmove(&(nCoords[2]),nCoords,sizeof(double)*6);
		nCoords[0] = nTmp[0];nCoords[1] = nTmp[1];
		nTmp[0] = nCoords[6],nTmp[1] = nCoords[7];
		memmove(&(nCoords[2]),nCoords,sizeof(double)*6);
		nCoords[0] = nTmp[0];nCoords[1] = nTmp[1];
	}
	
	nCoord = nCoords[where];
	
	return nCoord;
}
*/

/**	\brief	뷰의 좌표를 구한다.

	\param	pCoordText	a parameter of type char *
	\param	nDir	a parameter of type int
	\param	nVal	a parameter of type double

	\return	void	
*/
/*
void CSmartDrawAnnoView::GetCoordText(char *pCoordText, int nDir, double nVal)
{
	assert(pCoordText && "pCoordText is NULL");
	
	CDraft* pDraft=CDraft::GetInstance();
	if(pCoordText && pDraft)
	{
		if(ENM_PDS == pDraft->GetDomain())
		{
			const double uor = 1.f;///pDraft->GetDraftEnv()->GetUOR();
			if(CSmartDrawAnnoView::TOP_VIEW == GetViewDir())
			{
				const int iVal = SAFE_ROUND(nVal / uor , 1);
				if(1 == nDir)
				{		// E or W
					if(nVal > 0.)
						sprintf(pCoordText,"E. %s",CSmartDrawAnnoModule::UNIT2STR(iVal));
					else	sprintf(pCoordText,"W. %s",CSmartDrawAnnoModule::UNIT2STR(abs(iVal)));
				}else if(2 == nDir){	// N or S
					if(nVal > 0.)
						sprintf(pCoordText,"N. %s",CSmartDrawAnnoModule::UNIT2STR(iVal));
					else	sprintf(pCoordText,"S. %s",CSmartDrawAnnoModule::UNIT2STR(abs(iVal)));
				}
			}
			else if(CSmartDrawAnnoView::BOTTOM_VIEW == GetViewDir())
			{
				const int iVal = SAFE_ROUND(nVal / uor , 1);
				if(1 == nDir)
				{		// E or W
					if(nVal >= 0)
						sprintf(pCoordText,"E. %s",CSmartDrawAnnoModule::UNIT2STR(iVal));
					else	sprintf(pCoordText,"W. %s",CSmartDrawAnnoModule::UNIT2STR(abs(iVal)));
				}else if(2 == nDir){	// S or N
					if(nVal >= 0)
						sprintf(pCoordText,"N. %s",CSmartDrawAnnoModule::UNIT2STR(iVal));
					else	sprintf(pCoordText,"S. %s",CSmartDrawAnnoModule::UNIT2STR(abs(iVal)));
				}
			}
			else if(CSmartDrawAnnoView::FRONT_VIEW == GetViewDir())
			{
				const int iVal = SAFE_ROUND(nVal / uor , 1);
				if(1 == nDir)
				{		// East - West
					if(nVal >= 0)
						sprintf(pCoordText,"E. %s",CSmartDrawAnnoModule::UNIT2STR(iVal));
					else	sprintf(pCoordText,"W. %s",CSmartDrawAnnoModule::UNIT2STR(abs(iVal)));
				}else if(2 == nDir) pCoordText[0] = '\0'; // Up - Dn
			}
			else if(CSmartDrawAnnoView::BACK_VIEW == GetViewDir())
			{
				const int iVal = SAFE_ROUND(nVal / uor , 1);
				if(1 == nDir)
				{		// West - East
					if(nVal >= 0)
						sprintf(pCoordText,"E. %s",CSmartDrawAnnoModule::UNIT2STR(iVal));
					else	sprintf(pCoordText,"W. %s",CSmartDrawAnnoModule::UNIT2STR(abs(iVal)));
				}else if(2 == nDir) pCoordText[0] = '\0'; // Up - Dn
			}
			else if(CSmartDrawAnnoView::LEFT_VIEW == GetViewDir())
			{
				const int iVal = SAFE_ROUND(nVal / uor , 1);
				if(1 == nDir)
				{		// South - North
					if(nVal >= 0)
						sprintf(pCoordText,"N. %s",CSmartDrawAnnoModule::UNIT2STR(iVal));
					else	sprintf(pCoordText,"S. %s",CSmartDrawAnnoModule::UNIT2STR(abs(iVal)));
				}else if(2 == nDir) pCoordText[0] = '\0'; // Up - Dn
			}
			else if(CSmartDrawAnnoView::RIGHT_VIEW == GetViewDir())
			{
				const int iVal = SAFE_ROUND(nVal / uor , 1);
				if(1 == nDir)
				{		// North - South
					if(nVal >= 0)
						sprintf(pCoordText,"N. %s",CSmartDrawAnnoModule::UNIT2STR(iVal));
					else	sprintf(pCoordText,"S. %s",CSmartDrawAnnoModule::UNIT2STR(abs(iVal)));
				}else if(2 == nDir) pCoordText[0] = '\0'; // Up - Dn
			}
		}
		else
		{
			if(CSmartDrawAnnoView::TOP_VIEW == GetViewDir())
			{
				double coord = nVal;
				if((CDraftEnv::INCH_FEET != CDraftEnv::INPUT_UNIT) || (CDraftEnv::INCH_FEET != CDraftEnv::OUTPUT_UNIT))
				{
					coord = SAFE_ROUND(nVal/g_nUOR , 1);
				}

				if(1 == nDir)
				{		// E or W
					if(nVal >= 0)
						sprintf(pCoordText,"E. %s",CSmartDrawAnnoModule::UNIT2STR(coord));
					else	sprintf(pCoordText,"W. %s",CSmartDrawAnnoModule::UNIT2STR(fabs(coord)));
				}
				else if(2 == nDir)
				{	// N or S
					if(nVal >= 0)
						sprintf(pCoordText,"N. %s",CSmartDrawAnnoModule::UNIT2STR(coord));
					else	sprintf(pCoordText,"S. %s",CSmartDrawAnnoModule::UNIT2STR(fabs(coord)));
				}
			}
			else if(CSmartDrawAnnoView::BOTTOM_VIEW == GetViewDir())
			{
				double coord = nVal;
				if((CDraftEnv::INCH_FEET != CDraftEnv::INPUT_UNIT) || (CDraftEnv::INCH_FEET != CDraftEnv::OUTPUT_UNIT))
				{
					coord = SAFE_ROUND(nVal/g_nUOR , 1);
				}

				if(1 == nDir)
				{		// E or W
					if(nVal >= 0)
						sprintf(pCoordText,"E. %s",CSmartDrawAnnoModule::UNIT2STR(coord));
					else	sprintf(pCoordText,"W. %s",CSmartDrawAnnoModule::UNIT2STR(fabs(coord)));
				}
				else if(2 == nDir)
				{	// S or N
					if(nVal >= 0)
						sprintf(pCoordText,"N. %s",CSmartDrawAnnoModule::UNIT2STR(coord));
					else	sprintf(pCoordText,"S. %s",CSmartDrawAnnoModule::UNIT2STR(fabs(coord)));
				}
			}
			else if(CSmartDrawAnnoView::FRONT_VIEW == GetViewDir())
			{
				double coord = nVal;
				if((CDraftEnv::INCH_FEET != CDraftEnv::INPUT_UNIT) || (CDraftEnv::INCH_FEET != CDraftEnv::OUTPUT_UNIT))
				{
					coord = SAFE_ROUND(nVal/g_nUOR , 1);
				}

				if(1 == nDir)
				{		// East - West
					if(nVal >= 0)
						sprintf(pCoordText,"E. %s",CSmartDrawAnnoModule::UNIT2STR(coord));
					else	sprintf(pCoordText,"W. %s",CSmartDrawAnnoModule::UNIT2STR(fabs(coord)));
				}else if(2 == nDir) pCoordText[0] = '\0'; // Up - Dn
			}
			else if(CSmartDrawAnnoView::BACK_VIEW == GetViewDir())
			{
				double coord = nVal;
				if((CDraftEnv::INCH_FEET != CDraftEnv::INPUT_UNIT) || (CDraftEnv::INCH_FEET != CDraftEnv::OUTPUT_UNIT))
				{
					coord = SAFE_ROUND(nVal/g_nUOR , 1);
				}

				if(1 == nDir)
				{		// West - East
					if(nVal >= 0)
						sprintf(pCoordText,"E. %s",CSmartDrawAnnoModule::UNIT2STR(coord));
					else	sprintf(pCoordText,"W. %s",CSmartDrawAnnoModule::UNIT2STR(fabs(coord)));
				}else if(2 == nDir) pCoordText[0] = '\0'; // Up - Dn
			}
			else if(CSmartDrawAnnoView::LEFT_VIEW == GetViewDir())
			{
				double coord = nVal;
				if((CDraftEnv::INCH_FEET != CDraftEnv::INPUT_UNIT) || (CDraftEnv::INCH_FEET != CDraftEnv::OUTPUT_UNIT))
				{
					coord = SAFE_ROUND(nVal/g_nUOR , 1);
				}

				if(1 == nDir)
				{		// South - North
					if(nVal >= 0)
						sprintf(pCoordText,"N. %s",CSmartDrawAnnoModule::UNIT2STR(coord));
					else	sprintf(pCoordText,"S. %s",CSmartDrawAnnoModule::UNIT2STR(fabs(coord)));
				}else if(2 == nDir) pCoordText[0] = '\0'; // Up - Dn
			}
			else if(CSmartDrawAnnoView::RIGHT_VIEW == GetViewDir())
			{
				double coord = nVal;
				if((CDraftEnv::INCH_FEET != CDraftEnv::INPUT_UNIT) || (CDraftEnv::INCH_FEET != CDraftEnv::OUTPUT_UNIT))
				{
					coord = SAFE_ROUND(nVal/g_nUOR , 1);
				}

				if(1 == nDir)
				{		// North - South
					if(nVal >= 0)
						sprintf(pCoordText,"N. %s",CSmartDrawAnnoModule::UNIT2STR(coord));
					else	sprintf(pCoordText,"S. %s",CSmartDrawAnnoModule::UNIT2STR(fabs(coord)));
				}else if(2 == nDir) pCoordText[0] = '\0'; // Up - Dn
			}
		}
	}
}
*/

/**	
	@brief	The CSmartDrawAnnoView::RegisterTagObject function

	@author	BHK

	@date	?

	@param	pTagObject	a parameter of type CaDraw_TagItem *

	@return	void	
*/
/*
void CSmartDrawAnnoView::RegisterTagObject(CaDraw_TagItem *pTagObject)
{
	assert(pTagObject && "pTagObject is NULL");

	if(pTagObject) m_pExternalTerritory->AddTagItem(pTagObject);
}

void CSmartDrawAnnoView::SetViewInfo(const char *pViewInfo)
{
	assert(pViewInfo && "pViewInfo is NULL");

	if(pViewInfo) strncpy(m_szViewInfo,pViewInfo,VIEW_INFO_BUF_SIZ);
}

void CSmartDrawAnnoView::GetViewInfo(char *pViewInfo)
{
	assert(pViewInfo && "pViewInfo is NULL");

	if(pViewInfo) strncpy(pViewInfo,m_szViewInfo,VIEW_INFO_BUF_SIZ);
}

AREA_T CSmartDrawAnnoView::GetTagRange()
{
	return env.m_area;
}

inline const double CSmartDrawAnnoView::GetUOR()
{
	return g_nUOR;
}
*/