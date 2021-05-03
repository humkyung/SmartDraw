#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include "aDraw_LogicView.h"

#define LOGIC_VIEW_TOLER 0.00000001
#define LOGIC_VIEW_ISEQUAL(a,b) ((fabs((double)(a)-(b)) >= (double) LOGIC_VIEW_TOLER) ? 0 : 1)

ADRAW_IMPLEMENT_FUNC(CaDraw_LogicView , CaDraw_Entity , _T("CaDraw_LogicView"))

CaDraw_LogicView::CaDraw_LogicView(void)
{
	m_pViewList = new list<CaDraw_View*>;
	m_pViewBoundarySegment = NULL;
}

CaDraw_LogicView::~CaDraw_LogicView(void)
{
	try
	{
		if(m_pViewList) SAFE_DELETE(m_pViewList);
		
		CViewBoundaySegment* ptrNext=NULL;
		for(CViewBoundaySegment* ptr=m_pViewBoundarySegment;ptr;ptr = ptrNext)
		{
			ptrNext = ptr->next;

			delete (ptr);
			ptr = NULL;
		}
	}
	catch(...)
	{
	}
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.25

	@param	

	@return
*/
bool CaDraw_LogicView::IsAdjacentView(CaDraw_View* pView , const double& nTol)
{
	assert(pView && "pView is NULL");
	if(pView)
	{
		for(list<CaDraw_View*>::iterator itr = m_pViewList->begin();itr != m_pViewList->end();++itr)
		{
			if((*itr)->IsSameShapeView(pView) || (!(*itr)->IsInsideViewOf(pView) && !pView->IsInsideViewOf(*itr)))
			{
				CIsPoint3d ptOrigin[2];
				ptOrigin[0] = (*itr)->center();
				ptOrigin[1] = pView->center();

				const double dx = fabs(ptOrigin[0].x() - ptOrigin[1].x());
				const double dy = fabs(ptOrigin[0].y() - ptOrigin[1].y());
				double nWidth =((*itr)->GetViewWidth()*0.5)  + (pView->GetViewWidth()*0.5)  + nTol;
				double nHeight=((*itr)->GetViewHeight()*0.5) + (pView->GetViewHeight()*0.5) + nTol;
				if((dx < nWidth) && (dy < nHeight)) return true;
			}
		}
	}

	return false;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.25

	@param	

	@return
*/
int CaDraw_LogicView::AddView(CaDraw_View* pView)
{
	assert(pView && "pView is NULL");

	if(pView)
	{
		m_pViewList->push_back(pView);
		m_volume += pView->GetVolume();
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.25

	@param	

	@return
*/
int CaDraw_LogicView::GetViewIncludeEntity(list<CaDraw_View*>& ViewList , CaDraw_Entity* pEnt)
{
	assert(pEnt && "pEnt is NULL");
	if(pEnt)
	{
		for(list<CaDraw_View*>::iterator itr = m_pViewList->begin();itr != m_pViewList->end();++itr)
		{
			if((*itr)->IsEntityIn(pEnt))
			{
				ViewList.push_back(*itr);
			}
		}
	}

	return ERROR_SUCCESS;
}

/*
//	parameter	:
//	description	: unique each of contents
//	remarks		: end of pSrc must be NULL.
//	returns		:
template<class T>
T* ListPush(T* pDest,T* pSrc)
{
	assert(pSrc && "pSrc is NULL");

	if(pSrc)
	{
		if(pDest)
		{
			T* ptr = pDest;
			for(ptr = pDest;ptr && ptr->next;ptr = ptr->next)
			{
				assert(ptr != pSrc);
			}
			ptr->next = pSrc;
			//pSrc->prev= ptr;
			//pSrc->next= NULL;

			return pDest;
		}
		else
		{
			pDest = pSrc;
			//pDest->prev = pDest->next = NULL;

			return pSrc;
		}
	}

	return pDest;
}
*/

//	parameter	:
//	description	: unique each of contents
//	remarks		: end of pSrc must be NULL.
//	returns		:
template<class T>
T* ListPush(T* pDest,T* pSrc)
{
	assert(pSrc && "pSrc is NULL");

	if(pSrc)
	{
		if(pDest)
		{
			T* ptr = pDest;
			for(ptr = pDest;ptr && ptr->next;ptr = ptr->next)
			{
				assert(ptr != pSrc);
			}
			ptr->next = pSrc;
			//pSrc->prev= ptr;
			//pSrc->next= NULL;

			return pDest;
		}
		else
		{
			pDest = pSrc;
			//pDest->prev = pDest->next = NULL;

			return pSrc;
		}
	}

	return pDest;
}

/**	
	@brief	create view boundary of logic view.
	@author	HumKyung.BAEK
	@date	2010.05.25
	@param	
	@return
*/
void CaDraw_LogicView::CreateViewBoundary(const double& nToler , const int& iExtTagSide , const CaDraw_View::COORD_OPTION_E& coords)
{
	/// create view boundary from view.
	list<CViewBoundary*> lstViewBoundary;
	for(list<CaDraw_View*>::iterator itr = m_pViewList->begin();itr != m_pViewList->end();++itr)
	{
		CViewBoundary* pViewBoundary = (*itr)->CreateViewBoundary(iExtTagSide , coords);
		if(pViewBoundary) lstViewBoundary.push_back(pViewBoundary);
	}
	
	//! 뷰에서 겹쳐지는 Boundary 부분을 제거하는 부분(Rectangle Hidden 로직)
	for(list<CViewBoundary*>::iterator itr=lstViewBoundary.begin();itr != lstViewBoundary.end();++itr)
	{
		for(list<CViewBoundary*>::iterator itrNext=lstViewBoundary.begin();itrNext != lstViewBoundary.end();++itrNext)
		{
			if(itr != itrNext)
			{
				(*itr)->Split(*itrNext);
				CViewBoundaySegment* ptr = NULL;
				for(ptr = (*itrNext)->m_pVBS;ptr && ptr->next;ptr = ptr->next)
				{
					if((*itr)->IsHiddenLine(ptr,ptr->next , nToler))
						ptr->render = HIDE;
				}
				for(ptr = (*itr)->m_pVBS;ptr && ptr->next;ptr = ptr->next)
				{
					if((*itrNext)->IsHiddenLine(ptr,ptr->next , nToler))
						ptr->render = HIDE;
				}
			}
		}
	}

	CViewBoundaySegment aryViewBoundary[128];
	int    nViewBoundaryEdge=0;
	for(list<CViewBoundary*>::iterator itr=lstViewBoundary.begin();itr != lstViewBoundary.end();++itr)
	{
		for(CViewBoundaySegment* ptr = (*itr)->m_pVBS;ptr && ptr->next;ptr = ptr->next)
		{
			if((SHOW == ptr->render) && (nViewBoundaryEdge < 128))
			{
				memcpy(&(aryViewBoundary[nViewBoundaryEdge]),ptr,sizeof(CViewBoundaySegment));
				aryViewBoundary[nViewBoundaryEdge].edge.Set(ptr->pt , ptr->next->pt);
				nViewBoundaryEdge++;
			}
		}
	}
	//<-- delete CViewBoundarys <--
	for(list<CViewBoundary*>::iterator itrViewBoundary=lstViewBoundary.begin();itrViewBoundary != lstViewBoundary.end();)
	{
		delete (*itrViewBoundary);
		itrViewBoundary = lstViewBoundary.erase(itrViewBoundary);
	}
	//-->

	RemoveRedundantViewBoundary(aryViewBoundary,nViewBoundaryEdge , nToler);
	JoinViewLine(aryViewBoundary,nViewBoundaryEdge , nToler);
	SetViewBoundaryAttr(aryViewBoundary,nViewBoundaryEdge);

	m_pViewBoundarySegment = NULL;
	for(int i = 0;i < nViewBoundaryEdge;++i)
	{
		CViewBoundaySegment *p = new CViewBoundaySegment;
		memcpy(p , &(aryViewBoundary[i]) , sizeof(CViewBoundaySegment));
		p->next = NULL;

		m_pViewBoundarySegment = ListPush(m_pViewBoundarySegment , p);
	}
}

/**	
	@brief	두 점이 임계값 안에 들어가 있으면 같은 점으로 간주한다.

	@author	HumKyung.BAEK

	@param	pt1	a parameter of type const POINT_T&
	@param	pt2	a parameter of type const POINT_T&

	@return	bool	
*/
bool CaDraw_LogicView::IsSamePoint(const CIsPoint3d& pt1,const CIsPoint3d& pt2 , const double& nToler)
{
	const double dx = pt2.x() - pt1.x();
	const double dy = pt2.y() - pt1.y();
	const double dz = pt2.z() - pt1.z();
	if((fabs(dx) < nToler) && (fabs(dy) < nToler) && (fabs(dz) < nToler))
	{
		return true;
	}

	return false;
}

/**	
	@brief	두 개의 VIEW SEGMENT가 동일한지 검사한다.

	@author	HumKyung.BAEK

	\param	line1	a parameter of type const LINE_T&
	\param	line2	a parameter of type const LINE_T&

	\return	bool	
*/
bool CaDraw_LogicView::IsSameViewLine(const CIsLine3d& line1,const CIsLine3d& line2 , const double& nToler)
{
	double dx[2]={0.,},dy[2]={0.,};

	dx[0] = line2.start().x() - line1.start().x();
	dy[0] = line2.start().y() - line1.start().y();
	dx[1] = line2.end().x() - line1.end().x();
	dy[1] = line2.end().y() - line1.end().y();

	if(((fabs(dx[0]) < nToler) && (fabs(dx[1]) < nToler)) && ((fabs(dy[0]) < nToler) && (fabs(dy[1]) < nToler)))
		return true;

	dx[0] = line2.end().x() - line1.start().x();
	dy[0] = line2.end().y() - line1.start().y();
	dx[1] = line2.start().x() - line1.end().x();
	dy[1] = line2.start().y() - line1.end().y();

	if(((fabs(dx[0]) < nToler) && (fabs(dx[1]) < nToler)) && ((fabs(dy[0]) < nToler) && (fabs(dy[1]) < nToler)))
		return true;

	return false;

}

/**	
	@brief	remove redundant and very shortest view boundary

	@author	BHK

	@date	?

	@param	aryViewBoundary[]	a parameter of type VBS
	@param	nViewBoundaryEdge	a parameter of type int&

	@return	void	
*/
void CaDraw_LogicView::RemoveRedundantViewBoundary(CViewBoundaySegment aryViewBoundary[],int& nViewBoundaryEdge , const double& nToler)
{
	int i = 0;
	int j = 0;
	for(i=0;i < nViewBoundaryEdge;++i)
	{
		double dx=0.,dy=0.,dz=0.;
		for(j=i+1;j < nViewBoundaryEdge;++j)
		{
			dx = aryViewBoundary[j].vecTagDir.dx() - aryViewBoundary[i].vecTagDir.dx();
			dy = aryViewBoundary[j].vecTagDir.dy() - aryViewBoundary[i].vecTagDir.dy();
			dz = aryViewBoundary[j].vecTagDir.dz() - aryViewBoundary[i].vecTagDir.dz();
			if((fabs(dx) < LOGIC_VIEW_TOLER) && (fabs(dy) < LOGIC_VIEW_TOLER) && (fabs(dz) < LOGIC_VIEW_TOLER))
			{ // same direction
				if(IsSameViewLine(aryViewBoundary[i].edge,aryViewBoundary[j].edge , nToler))
				{
					memcpy(&(aryViewBoundary[j]),&(aryViewBoundary[j + 1]),sizeof(CViewBoundaySegment)*(nViewBoundaryEdge - j - 1));
					nViewBoundaryEdge--;
				}
			}
		}
	}

	/// remove small boundaries less than critical size
	for(i=0;i < nViewBoundaryEdge;++i)
	{
		double dx=aryViewBoundary[i].edge.end().x() - aryViewBoundary[i].edge.start().x();
		double dy=aryViewBoundary[i].edge.end().y() - aryViewBoundary[i].edge.start().y();
		double dz=aryViewBoundary[i].edge.end().z() - aryViewBoundary[i].edge.start().z();

		if((dx*dx + dy*dy + dz*dz) < nToler*nToler)
		{
			for(j=0;j < nViewBoundaryEdge;j++)
			{
				if((i != j) && IsSamePoint(aryViewBoundary[i].edge.start(),aryViewBoundary[j].edge.end() , nToler))
				{
					double nLen=sqrt(dx*dx + dy*dy + dz*dz);
					CIsVect3d vec(dx,dy,0.);
					vec.Normalize();
					aryViewBoundary[j].edge.Set(
						aryViewBoundary[j].edge.start() + CIsPoint3d(vec.dx()*nLen , vec.dy()*nLen , 0.f),
						aryViewBoundary[j].edge.end() + CIsPoint3d(vec.dx()*nLen , vec.dy()*nLen , 0.f));
				}
				if((i != j) && IsSamePoint(aryViewBoundary[i].edge.end(),aryViewBoundary[j].edge.start() , nToler))
				{
					double nLen=sqrt(dx*dx + dy*dy + dz*dz);
					CIsVect3d vec(dx,dy,0.);
					vec.Normalize();
					aryViewBoundary[j].edge.Set(
						aryViewBoundary[j].edge.start() + CIsPoint3d(vec.dx()*nLen , vec.dy()*nLen , 0.f),
						aryViewBoundary[j].edge.end() + CIsPoint3d(vec.dx()*nLen , vec.dy()*nLen , 0.f));
				}
			}
			memcpy(&(aryViewBoundary[i]),&(aryViewBoundary[i + 1]),sizeof(CViewBoundaySegment)*(nViewBoundaryEdge - i - 1));
			nViewBoundaryEdge--;
			continue;
		}
	}
}

/**	
	@brief	join the view line

	@author	HumKyung.BAEK

	@param	aryViewBoundary[]	a parameter of type VBS
	@param	nViewBoundaryEdge	a parameter of type int&

	@return	void	
*/
void CaDraw_LogicView::JoinViewLine(CViewBoundaySegment aryViewBoundary[],int& nViewBoundaryEdge , const double& nToler)
{
	CIsVect3d vec[2];
	double nLen[2]={0.,};
	for(int i=0;i < nViewBoundaryEdge;++i)
	{
		vec[0].Set(
			aryViewBoundary[i].edge.end().x() - aryViewBoundary[i].edge.start().x() ,
			aryViewBoundary[i].edge.end().y() - aryViewBoundary[i].edge.start().y() ,
			0.f);
		nLen[0] = vec[0].dx()*vec[0].dx() + vec[0].dy()*vec[0].dy();
		vec[0].Normalize();
		for(int j=i+1;j < nViewBoundaryEdge;++j)
		{
			double dx=0.,dy=0.,dz=0.;

			dx = aryViewBoundary[j].vecTagDir.dx() - aryViewBoundary[i].vecTagDir.dx();
			dy = aryViewBoundary[j].vecTagDir.dy() - aryViewBoundary[i].vecTagDir.dy();
			dz = 0.;
			if((fabs(dx) < LOGIC_VIEW_TOLER) && (fabs(dy) < LOGIC_VIEW_TOLER) && (fabs(dz) < LOGIC_VIEW_TOLER))
			{
				vec[1].Set(
					aryViewBoundary[j].edge.end().x() - aryViewBoundary[j].edge.start().x() , 
					aryViewBoundary[j].edge.end().y() - aryViewBoundary[j].edge.start().y() , 
					0.f);
				nLen[0] = vec[1].dx()*vec[1].dx() + vec[1].dy()*vec[1].dy();
				vec[1].Normalize();
				
				if((fabs(vec[1].dx() - vec[0].dx()) < LOGIC_VIEW_TOLER) && (fabs(vec[1].dy() - vec[0].dy()) < LOGIC_VIEW_TOLER))
				{ // same dir
					if(IsSamePoint(aryViewBoundary[i].edge.start(),aryViewBoundary[j].edge.start() , nToler))
					{
						aryViewBoundary[i].edge.Set(
							aryViewBoundary[j].edge.end(), aryViewBoundary[i].edge.end());
						STRCPY_T(aryViewBoundary[i].szDim[0],aryViewBoundary[j].szDim[1]);
						memcpy(&(aryViewBoundary[j]),&(aryViewBoundary[j + 1]),sizeof(CViewBoundaySegment)*(nViewBoundaryEdge - j - 1));
						nViewBoundaryEdge--;

						if(aryViewBoundary[i].vecTagDir.dx() < 0.)
						{
							if(aryViewBoundary[i].edge.start().x() > aryViewBoundary[i].edge.end().x())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else	
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f));
							}
						}
						else if(aryViewBoundary[i].vecTagDir.dx() > 0.)
						{
							if(aryViewBoundary[i].edge.start().x() < aryViewBoundary[i].edge.end().x())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f));
							}
						}
						else if(aryViewBoundary[i].vecTagDir.dy() < 0.)
						{
							if(aryViewBoundary[i].edge.start().y() > aryViewBoundary[i].edge.end().y())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f));
							}
						}
						else if(aryViewBoundary[i].vecTagDir.dy() > 0.)
						{
							if(aryViewBoundary[i].edge.start().y() < aryViewBoundary[i].edge.end().y())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f));
							}
						}
					}
					else if(IsSamePoint(aryViewBoundary[i].edge.start(),aryViewBoundary[j].edge.end() , nToler))
					{
						aryViewBoundary[i].edge.Set(
							aryViewBoundary[j].edge.start(), aryViewBoundary[i].edge.end());
						STRCPY_T(aryViewBoundary[i].szDim[0],aryViewBoundary[j].szDim[0]);
						memcpy(&(aryViewBoundary[j]),&(aryViewBoundary[j + 1]),sizeof(CViewBoundaySegment)*(nViewBoundaryEdge - j - 1));
						nViewBoundaryEdge--;

						if(aryViewBoundary[i].vecTagDir.dx() < 0.)
						{
							if(aryViewBoundary[i].edge.start().x() > aryViewBoundary[i].edge.end().x())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f));
							}
						}
						else if(aryViewBoundary[i].vecTagDir.dx() > 0.)
						{
							if(aryViewBoundary[i].edge.start().x() < aryViewBoundary[i].edge.end().x())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f));
							}
						}
						else if(aryViewBoundary[i].vecTagDir.dy() < 0.)
						{
							if(aryViewBoundary[i].edge.start().y() > aryViewBoundary[i].edge.end().y())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f));
							}
						}
						else if(aryViewBoundary[i].vecTagDir.dy() > 0.)
						{
							if(aryViewBoundary[i].edge.start().y() < aryViewBoundary[i].edge.end().y())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f));
							}
						}
					}
					else if(IsSamePoint(aryViewBoundary[i].edge.end(),aryViewBoundary[j].edge.start() , nToler))
					{
						aryViewBoundary[i].edge.Set(
							aryViewBoundary[i].edge.start(), aryViewBoundary[j].edge.end());
						STRCPY_T(aryViewBoundary[i].szDim[1],aryViewBoundary[j].szDim[1]);
						memcpy(&(aryViewBoundary[j]),&(aryViewBoundary[j + 1]),sizeof(CViewBoundaySegment)*(nViewBoundaryEdge - j - 1));
						nViewBoundaryEdge--;

						if(aryViewBoundary[i].vecTagDir.dx() < 0.)
						{
							if(aryViewBoundary[i].edge.start().x() > aryViewBoundary[i].edge.end().x())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f));
							}
						}
						else if(aryViewBoundary[i].vecTagDir.dx() > 0.)
						{
							if(aryViewBoundary[i].edge.start().x() < aryViewBoundary[i].edge.end().x())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f));
							}
						}
						else if(aryViewBoundary[i].vecTagDir.dy() < 0.)
						{
							if(aryViewBoundary[i].edge.start().y() > aryViewBoundary[i].edge.end().y())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f));
							}
						}
						else if(aryViewBoundary[i].vecTagDir.dy() > 0.)
						{
							if(aryViewBoundary[i].edge.start().y() < aryViewBoundary[i].edge.end().y())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f));
							}
						}
					}
					else if(IsSamePoint(aryViewBoundary[i].edge.end(),aryViewBoundary[j].edge.end() , nToler))
					{
						aryViewBoundary[i].edge.Set(
							aryViewBoundary[i].edge.start(), aryViewBoundary[j].edge.start());
						STRCPY_T(aryViewBoundary[i].szDim[1],aryViewBoundary[j].szDim[0]);
						memcpy(&(aryViewBoundary[j]),&(aryViewBoundary[j + 1]),sizeof(CViewBoundaySegment)*(nViewBoundaryEdge - j - 1));
						nViewBoundaryEdge--;

						if(aryViewBoundary[i].vecTagDir.dx() < 0.)
						{
							if(aryViewBoundary[i].edge.start().x() > aryViewBoundary[i].edge.end().x())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f));
							}
						}
						else if(aryViewBoundary[i].vecTagDir.dx() > 0.)
						{
							if(aryViewBoundary[i].edge.start().x() < aryViewBoundary[i].edge.end().x())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f));
							}
						}
						else if(aryViewBoundary[i].vecTagDir.dy() < 0.)
						{
							if(aryViewBoundary[i].edge.start().y() > aryViewBoundary[i].edge.end().y())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f));
							}
						}
						else if(aryViewBoundary[i].vecTagDir.dy() > 0.)
						{
							if(aryViewBoundary[i].edge.start().y() < aryViewBoundary[i].edge.end().y())
							{
								aryViewBoundary[i].edge.Set(
									CIsPoint3d(aryViewBoundary[i].edge.start().x() , aryViewBoundary[i].edge.end().y() , 0.f),
									aryViewBoundary[i].edge.end());
							}
							else
							{
								aryViewBoundary[i].edge.Set(
									aryViewBoundary[i].edge.start() , 
									CIsPoint3d(aryViewBoundary[i].edge.end().x() , aryViewBoundary[i].edge.start().y() , 0.f));
							}
						}
					}
				}
			}
		}
	}
}

/**	@brief	set view boundary attribute

	\param	aryViewBoundary[]	a parameter of type VBS
	\param	nViewBoundaryEdge	a parameter of type const int

	\return	void	
*/
void CaDraw_LogicView::SetViewBoundaryAttr(CViewBoundaySegment aryViewBoundary[],const int nViewBoundaryEdge)
{
	CIsPoint3d ptStart,ptEnd;
	CIsVect3d vec[2];
	for(int i=0;i < nViewBoundaryEdge;++i)
	{
		ptEnd = aryViewBoundary[i].edge.end();
		for(int j=0;j < nViewBoundaryEdge;++j)
		{
			ptStart = aryViewBoundary[j].edge.start();
			if(LOGIC_VIEW_ISEQUAL(ptEnd.x(),ptStart.x()) && LOGIC_VIEW_ISEQUAL(ptEnd.y(),ptStart.y()))
			{
				vec[0].Set(
					aryViewBoundary[i].edge.end().x() - aryViewBoundary[i].edge.start().x() , 
					aryViewBoundary[i].edge.end().y() - aryViewBoundary[i].edge.start().y() , 0.f);
				vec[1].Set(
					aryViewBoundary[j].edge.end().x() - aryViewBoundary[j].edge.start().x() , 
					aryViewBoundary[j].edge.end().y() - aryViewBoundary[j].edge.start().y() , 0.f);
				
				double nCross=vec[0].dy()*vec[1].dx() - vec[0].dx()*vec[1].dy();
				if(nCross < 0.)
				{
					aryViewBoundary[i].bOpen[1] = true;
					aryViewBoundary[j].bOpen[0] = true;
				}
				else
				{
					aryViewBoundary[i].bOpen[1] = false;
					aryViewBoundary[j].bOpen[0] = false;
				}
			}
		}
	}
}

/**	
	@brief	

	@author	BHK

	@date	2010.05.25

	@return	int
*/
int CaDraw_LogicView::GetViewList(list<CaDraw_View*>* pViewList) const
{
	assert(pViewList && "pViewList is NULL");

	if(pViewList)
	{
		pViewList->clear();
		pViewList->insert(pViewList->begin() , m_pViewList->begin() , m_pViewList->end());

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**	
	@brief	

	@author	BHK

	@date	2010.05.27

	@return	int
*/
const CIsVolume CaDraw_LogicView::volume() const
{
	return m_volume;
}

/**
	@brief	determine this is enable by checking filters
	@author	humkyung
	@date	2016.03.19
*/
bool CaDraw_LogicView::IsEnable(AnnoFilters* pAnnoFilters) const
{
	return true;
}