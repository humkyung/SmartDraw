// KHiddenCore.cpp: implementation of the KHiddenCore class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <limits>
#include "BSPTree.h"
#include "SmartHSR.h"
#include "HSRCore.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**	
	@brief
*/
CHSRCore::CHSRCore()
{
	m_pRenderList = NULL;
	m_pMergeFace  = NULL;
}

/**	
	@brief
*/
CHSRCore::~CHSRCore()
{
	try
	{
		Clear();
	}
	catch(...)
	{
	}

#ifdef _DEBUG
	OutputDebugString("Destructure of CHSRCore");
#endif
}

/**	@brief		bsp node를 시점에서 바라볼때 일렬로 정렬한다.

	@param		pNode	a parameter of type CBSPNode *

	@remarks	시점을 저 높은 위에서 바라본다.(신의 관점에서...) - z value of view position is max of double
	
	@return	void	
*/
void CHSRCore::CreateRenderList(CBSPNode *pNode)
{
	assert(pNode && "pNode is NULL");
	
	if(pNode && pNode->m_pSameDir)
	{
		//const double hiz = std::numeric_limits<double>::max();//pNode->m_pSameDir->pHead->pt->z;
		//double hiz = pNode->m_pSameDir->pHead->pt->z;
		//for(PVERTEX pVt = pNode->m_pSameDir->pHead;pVt;pVt = pVt->next)
		//	if(hiz < pVt->pt->z) hiz = pVt->pt->z;
		
		POINT_T pt={0,};
		pt.x = pNode->m_pSameDir->pHead->pt->x;
		pt.y = pNode->m_pSameDir->pHead->pt->y;
		pt.z = DBL_MAX;
		
		if(m_BSPTree.IsViewerInPositiveSideOfPlane(&(pNode->m_pSameDir->Plane),&pt))
		{
			CreatePositiveOrderRenderList(pNode);
		}
		else
		{
			CreateNegativeOrderRenderList(pNode);
		}
	}
}

/**	@brief	Positive side의 Render List를 생성한다.
	
	@author	humkyung
	
	@param	pNode	a parameter of type CBSPNode*

	@return	void	
*/
void CHSRCore::CreatePositiveOrderRenderList(CBSPNode* pNode)
{
	assert(pNode && "pNode is NULL");

	if(pNode)
	{
		bool bPositiveSide = false;
		POINT_T pt={0,};
		pt.x = pNode->m_pSameDir->pHead->pt->x;
		pt.y = pNode->m_pSameDir->pHead->pt->y;
		pt.z = DBL_MAX;
		if(m_BSPTree.IsViewerInPositiveSideOfPlane(&(pNode->m_pSameDir->Plane),&pt))
		{
			bPositiveSide = true;
		}

		//! same plane이 positive이면 positive side -> same plane -> oppsite plane -> negative side
		//! same plane이 negative이면 negative side -> same plane -> oppsite plane -> positive side
		if(bPositiveSide)
		{
			if(pNode->m_pPosSide) CreateRenderList(pNode->m_pPosSide);
		}
		else
		{
			if(pNode->m_pNegSide) CreateRenderList(pNode->m_pNegSide);
		}
		
		PRENDERNODE pRenderNode=NULL;
		CHSRFace* pTail=NULL;
		if((pNode->m_pSameDir) || (pNode->m_pOppDir))
		{
			CHSRApp* pHSRApp=CHSRApp::GetInstance();
			RECT_T rectView=pHSRApp->m_clsView.GetViewArea();

			if(pNode->m_pOppDir)
			{
				CHSRFace* ptrNext=NULL;
				for(CHSRFace* ptr=pNode->m_pOppDir;ptr;ptr=ptrNext)
				{
					ptrNext  = ptr->next;
					ptr->prev= NULL;
					ptr->next= NULL;
					
					CalculateRegion(ptr,rectView);
					
					if(NULL == pRenderNode)
					{
						if(pRenderNode = NewRenderNode())
						{
							pRenderNode->pFace = ListPush(pRenderNode->pFace,ptr);
							//m_renderNodePool.push_back(pRenderNode);
						}
					}
					else
					{
						ptr->prev = pTail;
						pTail = ListPush(pTail,ptr);
						//pRenderNode->pFace = ListPush(pRenderNode->pFace,ptr);
					}
					pTail = ptr;
				}
			}
			
			if(pNode->m_pSameDir)
			{
				CHSRFace* ptrNext=NULL;
				for(CHSRFace* ptr=pNode->m_pSameDir;ptr;ptr=ptrNext)
				{
					ptrNext  = ptr->next;
					ptr->prev= NULL;
					ptr->next= NULL;
					
					CalculateRegion(ptr,rectView);
					
					if(NULL == pRenderNode)
					{
						if(pRenderNode = NewRenderNode())
						{
							pRenderNode->pFace = ListPush(pRenderNode->pFace,ptr);
							//m_renderNodePool.push_back(pRenderNode);
						}
					}
					else
					{
						ptr->prev = pTail;
						pTail = ListPush(pTail,ptr);
						//pRenderNode->pFace = ListPush(pRenderNode->pFace,ptr);
					}
					pTail = ptr;
				}
			}
			
			if(pRenderNode)
			{
				pRenderNode->pTail = pTail;
				m_pRenderList = ListPushFront(m_pRenderList,pRenderNode);
			}
		}
		
		//! same plane이 positive이면 negative side -> same plane -> oppsite plane -> positive side
		//! same plane이 negative이면 positive side -> same plane -> oppsite plane -> negative side
		if(bPositiveSide)
		{
			if(pNode->m_pNegSide) CreateRenderList(pNode->m_pNegSide);
		}
		else
		{
			if(pNode->m_pPosSide) CreateRenderList(pNode->m_pPosSide);			
		}
	}
}

/**	@brief	negative side의 Render List를 생성한다.
	
	@author	humkyung

	@param	pNode	a parameter of type CBSPNode*

	@return	void	
*/
void CHSRCore::CreateNegativeOrderRenderList(CBSPNode* pNode)
{
	assert(pNode && "CHSRCore::CreateNegativeSideRenderList - pNode is NULL");

	if(pNode)
	{
		bool bPositiveSide = false;
		POINT_T pt={0,};
		pt.x = pNode->m_pSameDir->pHead->pt->x;
		pt.y = pNode->m_pSameDir->pHead->pt->y;
		pt.z = DBL_MAX;
		if(m_BSPTree.IsViewerInPositiveSideOfPlane(&(pNode->m_pSameDir->Plane),&pt))
		{
			bPositiveSide = true;
		}

		//! same plane이 positive이면 positive side -> same plane -> oppsite plane -> negative side
		//! same plane이 negative이면 negative side -> same plane -> oppsite plane -> positive side
		if(bPositiveSide)
		{
			if(pNode->m_pPosSide) CreateRenderList(pNode->m_pPosSide);
		}
		else
		{
			if(pNode->m_pNegSide) CreateRenderList(pNode->m_pNegSide);
		}
				
		PRENDERNODE pRenderNode=NULL;
		CHSRFace* pTail=NULL;
		if((pNode->m_pOppDir) || (pNode->m_pSameDir))
		{
			CHSRApp* pHSRApp=CHSRApp::GetInstance();
			RECT_T rectView=pHSRApp->m_clsView.GetViewArea();

			
			if(pNode->m_pSameDir)
			{
				CHSRFace* ptrNext=NULL;
				for(CHSRFace* ptr=pNode->m_pSameDir;ptr;ptr=ptrNext)
				{
					ptrNext  = ptr->next;
					ptr->prev= NULL;
					ptr->next= NULL;
								
					CalculateRegion(ptr,rectView);
					if(NULL == pRenderNode)
					{
						if(pRenderNode = NewRenderNode())
						{
							pRenderNode->pFace = ListPush(pRenderNode->pFace,ptr);
							//m_renderNodePool.push_back(pRenderNode);
						}
					}
					else
					{
						ptr->prev = pTail;
						pTail = ListPush(pTail,ptr);
						//pRenderNode->pFace = ListPush(pRenderNode->pFace,ptr);
					}
					pTail = ptr;
				}
			}
			if(pNode->m_pOppDir)
			{
				CHSRFace* ptrNext=NULL;
				for(CHSRFace* ptr=pNode->m_pOppDir;ptr;ptr=ptrNext)
				{
					ptrNext  = ptr->next;
					ptr->prev= NULL;
					ptr->next= NULL;
					
					CalculateRegion(ptr,rectView);
					
					if(NULL == pRenderNode)
					{
						if(pRenderNode = NewRenderNode())
						{
							pRenderNode->pFace = ListPush(pRenderNode->pFace,ptr);
							//m_renderNodePool.push_back(pRenderNode);
						}
					}
					else
					{
						//pRenderNode->pFace = ListPush(pRenderNode->pFace,ptr);
						ptr->prev = pTail;
						pTail = ListPush(pTail,ptr);
					}
					pTail = ptr;
				}
			}

			if(pRenderNode)
			{
				pRenderNode->pTail = pTail;
				m_pRenderList = ListPushFront(m_pRenderList,pRenderNode);
			}
		}
		
		//! same plane이 positive이면 negative side -> same plane -> oppsite plane -> positive side
		//! same plane이 negative이면 positive side -> same plane -> oppsite plane -> negative side
		if(bPositiveSide)
		{
			if(pNode->m_pNegSide) CreateRenderList(pNode->m_pNegSide);
		}
		else
		{
			if(pNode->m_pPosSide) CreateRenderList(pNode->m_pPosSide);
		}
	}
}

/**	
	@brief	calculate face's region.

	@author	humkyung

	@date	????.??.??

	@param	pFace	a parameter of type PFACE
	@param	rectView	a parameter of type const RECT_T&

	@return	void	
*/
void CHSRCore::CalculateRegion(CHSRFace* pFace,const RECT_T& rectView)
{
	assert(pFace && "pFace is NULL");

	if(pFace)
	{
		const int nSquare=4;
		const double nDx = rectView.width / double(nSquare);
		const double nDy = rectView.height / double(nSquare);
		for(int i=0;i < nSquare;++i)
		{
			for(int j=0;j < nSquare;j++)
			{
				if(!((pFace->volume.x[0] > rectView.x + nDx*(i+1)) || (pFace->volume.x[1] < rectView.x + nDx*i) || 
					(pFace->volume.y[0] > rectView.y + nDy*(j+1)) && (pFace->volume.y[1] < rectView.y + nDy*i)))
				{
					pFace->region |= 1 << (j*2 + i);
				}
			}
		}
	}
}

/**	
	@brief	The CHSRCore::ExtractHiddenLines function\n
	
	@author	humkyung

	1. split intersection lines.\n
	2. extract hidden line.\
	3. append faces to m_pFaceList.

	@return	void	
*/
void CHSRCore::ExtractHiddenLines()
{
	assert(m_pRenderList        && "render list is invalid");
	assert(NULL == m_pMergeFace && "mergeface is invalid");

	if(m_pRenderList && (NULL == m_pMergeFace))
	{
		CHSRFace* pFaceNode = NULL;
		char szBuf[MAX_PATH] = {'\0' ,};

		long count=0L;
		/// split intersection lines and remove hidden lines.
		for(PRENDERNODE pRenderNode = m_pRenderList;pRenderNode;pRenderNode = pRenderNode->next)
		{
			for(pFaceNode = pRenderNode->pFace;pFaceNode;pFaceNode = pFaceNode->next)
			{
				if(pRenderNode->next) HideFace(pFaceNode , pRenderNode->next);
				count++;
			}
		}
	}
}

/**	
	@brief	The CHSRCore::RenderList2FaceList function


	@return	void	
*/
void CHSRCore::RenderList2FaceList()
{
	assert((NULL == m_pMergeFace) && "m_pMergeFace is not NULL");
	assert(m_pRenderList && "m_pRenderList is NULL");
	
	if((NULL == m_pMergeFace) && m_pRenderList)
	{
		CHSRFace* pLastFace =NULL;
		// craet face list from render list
		for(PRENDERNODE pRenderNode = m_pRenderList;pRenderNode;pRenderNode = pRenderNode->next)
		{
			if(m_pMergeFace)
			{
				pRenderNode->pFace->prev = pLastFace;
				pLastFace->next = pRenderNode->pFace;
				pLastFace = pRenderNode->pTail;
				/*
				for(CHSRFace* pFaceNode = pLastFace;pFaceNode && pFaceNode->next;pFaceNode = pFaceNode->next);
				pRenderNode->pFace->prev = pFaceNode;
				pFaceNode->next = pRenderNode->pFace;
				
				for(pFaceNode = pRenderNode->pFace;pFaceNode && pFaceNode->next;pFaceNode = pFaceNode->next)
					pFaceNode->next->prev = pFaceNode;
				
				// save last face
				pLastFace = pFaceNode;
				*/
			}
			else
			{
				m_pMergeFace = pLastFace = pRenderNode->pFace;
				pLastFace = pRenderNode->pTail;
				/*
				for(CHSRFace* pFaceNode = pLastFace;pFaceNode && pFaceNode->next;pFaceNode = pFaceNode->next)
					pFaceNode->next->prev = pFaceNode;
				
				/// save last face.
				pLastFace = pFaceNode;
				*/
			}
		}
	}
}

/**	\brief	The CHSRCore::UnionFaces function\n
	union face from face list separated by plane in BSP tree.

	\remarks\n
	;2002-01-25 - not checked first of list is removing.

	\return	void	
*/
void CHSRCore::UnionFaces()
{
	assert(m_pMergeFace && "m_pMergeFace is NULL");

	if(m_pMergeFace)
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		CHSRFace* pFirstOfList=m_pMergeFace;

		for(CHSRFace* pFaceNode = m_pMergeFace;pFaceNode;pFaceNode = pFaceNode->next)
		{
			if(pFaceNode->pSibling)
			{
				for(CHSRFace* ptr = pFaceNode->pSibling;ptr;ptr = ptr->pSibling)
				{
					if(ptr->prev) ptr->prev->next = ptr->next;
					if(ptr->next) ptr->next->prev = ptr->prev;
					if((pFirstOfList == ptr) && ptr->next)
					{
						pFirstOfList = ptr->next;

					}
					ptr->prev = NULL;
					ptr->next = NULL;
				}
			}
		}
		if(pFirstOfList)
		{
			m_pMergeFace = pFirstOfList;
		}
	}
}

/**	
	@brief	hide face through pRenderList
	
	@author	humkyung

	@date	????.??.??

	@param	pFace		a parameter of type PFACE
	@param	pRenderList	a parameter of type PRENDERNODE

	@return	void	
*/
void CHSRCore::HideFace(CHSRFace* pFace, PRENDERNODE pRenderList)
{
	assert(pFace && "face is NULL");
	assert(pRenderList && "render list is NULL");

	if(pFace && pRenderList)
	{
		CHSRApp* pHiddenApp = CHSRApp::GetInstance();

		for(PRENDERNODE pRenderNode = pRenderList;pRenderNode && (false == pFace->hidden);pRenderNode = pRenderNode->next)
		{
			for(CHSRFace* pFaceNode = pRenderNode->pFace;pFaceNode && (false == pFace->hidden);pFaceNode = pFaceNode->next)
			{
				//if(!(pFaceNode->region & pFace->region) || (pFace->id == pFaceNode->id)/* || pHiddenApp->IsOuterVolume(&(pFace->volume),&(pFaceNode->volume))*/) continue;
				if(!pFace->GetRange().CollideWith(pFaceNode->GetRange())) continue;

				//! face with id is -1 can't hide other faces.
				if(-1 != pFaceNode->id)
				{
					try
					{
						pFace->SplitFaceLine(pFaceNode,false);
						pFace->ExtractHiddenLine(pFaceNode);
					}
					catch(...)
					{
						int d = 1;
					}
				}
			}
		}
	}
}

/**	
	@brief	free memories allocated for BSP Tree and rendernode.

	@author	BHK

	@date	?

	@param	

	@return	void	
*/
void CHSRCore::Clear()
{		
	PRENDERNODE pRenderNode;
	for(list<PRENDERNODE>::iterator itr=m_renderNodePool.begin();itr != m_renderNodePool.end();)
	{
		pRenderNode = *itr;
		::free(pRenderNode);
		m_renderNodePool.erase(itr++);
	}
}

/**	\brief	The CHSRCore::NewRenderNode function


	\return	PRENDERNODE	
*/
PRENDERNODE CHSRCore::NewRenderNode()
{
	PRENDERNODE pRenderNode=(PRENDERNODE)calloc(1,sizeof(RENDERNODE));
	if(NULL != pRenderNode)
	{
		m_renderNodePool.push_back(pRenderNode);
		return pRenderNode;
	}

	return NULL;
}
