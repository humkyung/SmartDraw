#include "SmartHSR.h"
#include "HSRView.h"

static CHSRView* theHiddenView = NULL;

/**	\brief	The GetHiddenView function


	\return	CHSRView*	
*/
CHSRView* GetHiddenView()
{
	return theHiddenView;
}

/**	\brief
*/
CHSRView::CHSRView()
{
	theHiddenView = this;
	m_pViewFace   = NULL;

	m_nViewBoundaryEdge = 0;
	m_ViewPoint   = TOP;
}

/**	\brief
*/
CHSRView::CHSRView(POINT_T ptLeftTop,POINT_T ptRightBottom)
{
	m_pViewFace = NULL;

	m_ptLeftTop     = ptLeftTop;
	m_ptRightBottom = ptRightBottom;
}

/**	\brief
*/
CHSRView::~CHSRView()
{
	theHiddenView = NULL;
}

/**	\brief	The CHSRView::SetViewInfo function

	\param	pList	a parameter of type DList*

	\return	bool	
*/
bool CHSRView::SetViewInfo(DList* pList)
{
	assert(pList && "pList is NULL");
	DList* pPointList = pList;

	if(pList)
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		if(NULL != (m_pViewFace= static_cast<CHSRFace*>(pHSRApp->new_hsr_face()))){
			m_pViewFace->id= -1;
			
			if(NULL == (m_pViewFace->pHead = pHSRApp->alloc_vertex())){
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(m_pViewFace->pHead,0,sizeof(HSR_VERTEX));
			
			PHSR_VERTEX pVertex = m_pViewFace->pHead;
			memset(pVertex,0,sizeof(HSR_VERTEX));
			pVertex->pt     = pPointList->val.data.point;
			m_pViewFace->volume.x[LOW] = pVertex->pt->x;
			m_pViewFace->volume.y[LOW] = pVertex->pt->y;
			m_pViewFace->volume.z[LOW] = pVertex->pt->z;
			
			if(NULL == (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex())){
				pHSRApp->free_vertex(m_pViewFace->pHead);
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(pVertex->next,0,sizeof(HSR_VERTEX));
			pPointList = pPointList->next;
			pVertex = pVertex->next;
			pVertex->pt   = pPointList->val.data.point;
			
			if(NULL == (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex())){
				pHSRApp->free_vertex(m_pViewFace->pHead->next);
				pHSRApp->free_vertex(m_pViewFace->pHead);
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(pVertex->next,0,sizeof(HSR_VERTEX));
			pPointList = pPointList->next;
			pVertex = pVertex->next;
			pVertex->pt   = pPointList->val.data.point;
			m_pViewFace->volume.x[HIGH] = pVertex->pt->x;
			m_pViewFace->volume.y[HIGH] = pVertex->pt->y;
			m_pViewFace->volume.z[HIGH] = pVertex->pt->z;
			
			//--> check view volume
			assert( (m_pViewFace->volume.x[HIGH] >= m_pViewFace->volume.x[LOW]) &&
				(m_pViewFace->volume.y[HIGH] >= m_pViewFace->volume.y[LOW]) &&
				(m_pViewFace->volume.z[HIGH] >= m_pViewFace->volume.z[LOW]));
			//<--

			if(NULL == (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex())){
				pHSRApp->free_vertex(m_pViewFace->pHead->next->next);
				pHSRApp->free_vertex(m_pViewFace->pHead->next);
				pHSRApp->free_vertex(m_pViewFace->pHead);
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(pVertex->next,0,sizeof(HSR_VERTEX));
			pPointList = pPointList->next;
			pVertex = pVertex->next;
			pVertex->pt   = pPointList->val.data.point;
			
			if(NULL == (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex())){
				pHSRApp->free_vertex(m_pViewFace->pHead->next->next->next);
				pHSRApp->free_vertex(m_pViewFace->pHead->next->next);
				pHSRApp->free_vertex(m_pViewFace->pHead->next);
				pHSRApp->free_vertex(m_pViewFace->pHead);
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(pVertex->next,0,sizeof(HSR_VERTEX));
			pVertex = pVertex->next;
			pVertex->pt   = pList->val.data.point;
		}
		pPointList = pPointList->next;
		
		assert(pPointList && (df_number == pPointList->val.type));
		if(pPointList && (df_number == pPointList->val.type)) m_nScale = pPointList->val.data.number;
	}

	return true;
}

/**	
	@brief	set view information and create view face

	@author	BHK

	@date	?

	@param	ptCenter	a parameter of type const POINT_T
	@param	nWidth	a parameter of type const double
	@param	nHeight	a parameter of type const double
	@param	nScale	a parameter of type const double

	@return	bool	
*/
bool CHSRView::SetViewInfo(const POINT_T ptCenter,const double nWidth,const double nHeight,const double nScale , 
			   const CHSRViewFormat::ViewType& ViewType)
{
	const static double nTol=0.;
	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	
	SetViewBoundray(ptCenter,nWidth,nHeight);
	m_nScale = nScale;
	if(NULL != (m_pViewFace = static_cast<CHSRFace*>(pHSRApp->new_hsr_face())))
	{
		m_pViewFace->id = -1;
		
		if(CHSRViewFormat::RECTANGLE_VIEW == ViewType)
		{
			if(NULL == (m_pViewFace->pHead = pHSRApp->alloc_vertex())){
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(m_pViewFace->pHead,0,sizeof(HSR_VERTEX));
			
			//- first vertex ->
			PHSR_VERTEX pVertex = m_pViewFace->pHead;
			memset(pVertex,0,sizeof(HSR_VERTEX));
			if(pVertex->pt = pHSRApp->alloc_point()){
				pVertex->pt->x = ptCenter.x - nWidth*0.5 - nTol;
				pVertex->pt->y = ptCenter.y - nHeight*0.5- nTol;
				pVertex->pt->z = 0.;

				m_pViewFace->volume.x[LOW] = pVertex->pt->x;
				m_pViewFace->volume.y[LOW] = pVertex->pt->y;
				m_pViewFace->volume.z[LOW] = pVertex->pt->z;
			}else	return false;
			//<--
			
			//- second vertex -->
			if(NULL == (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex())){
				pHSRApp->free_vertex(m_pViewFace->pHead);
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(pVertex->next,0,sizeof(HSR_VERTEX));
			pVertex = pVertex->next;
			if(pVertex->pt = pHSRApp->alloc_point()){
				pVertex->pt->x = ptCenter.x + nWidth*0.5 + nTol;
				pVertex->pt->y = ptCenter.y - nHeight*0.5- nTol;
				pVertex->pt->z = 0.;
			}else	return false;
			//<--

			//- third vertex -->
			if(NULL == (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex())){
				pHSRApp->free_vertex(m_pViewFace->pHead->next);
				pHSRApp->free_vertex(m_pViewFace->pHead);
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(pVertex->next,0,sizeof(HSR_VERTEX));
			pVertex = pVertex->next;
			if(pVertex->pt = pHSRApp->alloc_point()){
				pVertex->pt->x = ptCenter.x + nWidth*0.5 + nTol;
				pVertex->pt->y = ptCenter.y + nHeight*0.5+ nTol;
				pVertex->pt->z = 0.;
				
				m_pViewFace->volume.x[HIGH] = pVertex->pt->x;
				m_pViewFace->volume.y[HIGH] = pVertex->pt->y;
				m_pViewFace->volume.z[HIGH] = pVertex->pt->z;
			}else	return false;
			
			//--> check view volume
			assert( (m_pViewFace->volume.x[HIGH] >= m_pViewFace->volume.x[LOW]) &&
				(m_pViewFace->volume.y[HIGH] >= m_pViewFace->volume.y[LOW]) &&
				(m_pViewFace->volume.z[HIGH] >= m_pViewFace->volume.z[LOW]));
			//<--
			
			
			//- fourth vertex -->
			if(NULL == (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex())){
				pHSRApp->free_vertex(m_pViewFace->pHead->next->next);
				pHSRApp->free_vertex(m_pViewFace->pHead->next);
				pHSRApp->free_vertex(m_pViewFace->pHead);
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(pVertex->next,0,sizeof(HSR_VERTEX));
			pVertex = pVertex->next;
			if(pVertex->pt = pHSRApp->alloc_point()){
				pVertex->pt->x = ptCenter.x - nWidth*0.5 - nTol;
				pVertex->pt->y = ptCenter.y + nHeight*0.5+ nTol;
				pVertex->pt->z = 0.;
			}else	return false;
			
			if(NULL == (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex())){
				pHSRApp->free_vertex(m_pViewFace->pHead->next->next->next);
				pHSRApp->free_vertex(m_pViewFace->pHead->next->next);
				pHSRApp->free_vertex(m_pViewFace->pHead->next);
				pHSRApp->free_vertex(m_pViewFace->pHead);
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(pVertex->next,0,sizeof(HSR_VERTEX));
			pVertex = pVertex->next;
			if(pVertex->pt = pHSRApp->alloc_point()){
				pVertex->pt->x = ptCenter.x - nWidth*0.5 - nTol;
				pVertex->pt->y = ptCenter.y - nHeight*0.5- nTol;
				pVertex->pt->z = 0.;
			}else	return false;
		}
		else if(CHSRViewFormat::QUADRANT1_VIEW == ViewType)
		{
			if(NULL == (m_pViewFace->pHead = pHSRApp->alloc_vertex())){
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(m_pViewFace->pHead,0,sizeof(HSR_VERTEX));
			
			//- first vertex ->
			PHSR_VERTEX pVertex = m_pViewFace->pHead;
			memset(pVertex,0,sizeof(HSR_VERTEX));
			if(pVertex->pt = pHSRApp->alloc_point()){
				pVertex->pt->x = ptCenter.x - nWidth*0.5 - nTol;
				pVertex->pt->y = ptCenter.y - nHeight*0.5- nTol;
				pVertex->pt->z = 0.;
			}else	return false;
			//<--
			
			//- second vertex -->
			if(NULL == (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex())){
				pHSRApp->free_vertex(m_pViewFace->pHead);
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(pVertex->next,0,sizeof(HSR_VERTEX));
			pVertex = pVertex->next;
			if(pVertex->pt = pHSRApp->alloc_point()){
				pVertex->pt->x = ptCenter.x + nWidth*0.5 + nTol;
				pVertex->pt->y = ptCenter.y - nHeight*0.5- nTol;
				pVertex->pt->z = 0.;
			}else	return false;
			//<--

			const double dAngle = (0.5 * PI / 32.f);
			for(int i = 1;i < 32;++i)
			{
				const double cs = cos(dAngle * i);
				const double sn = sin(dAngle * i);
				double x = ptCenter.x - nWidth*0.5 - nTol + nWidth * cs;
				double y = ptCenter.y - nHeight*0.5- nTol + nWidth * sn;

				if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
				{
					memset(pVertex->next,0,sizeof(HSR_VERTEX));
					pVertex = pVertex->next;
					if(pVertex->pt = pHSRApp->alloc_point())
					{
						pVertex->pt->x = x;
						pVertex->pt->y = y;
						pVertex->pt->z = 0.;
					}else	return false;
					//<--
				}
			}
			
			//- fourth vertex -->
			if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
			{
				memset(pVertex->next,0,sizeof(HSR_VERTEX));
				pVertex = pVertex->next;
				if(pVertex->pt = pHSRApp->alloc_point())
				{
					pVertex->pt->x = ptCenter.x - nWidth*0.5 - nTol;
					pVertex->pt->y = ptCenter.y + nHeight*0.5+ nTol;
					pVertex->pt->z = 0.;
				}else	return false;
			}
			
			if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
			{
				memset(pVertex->next,0,sizeof(HSR_VERTEX));
				pVertex = pVertex->next;
				if(pVertex->pt = pHSRApp->alloc_point()){
					pVertex->pt->x = ptCenter.x - nWidth*0.5 - nTol;
					pVertex->pt->y = ptCenter.y - nHeight*0.5- nTol;
					pVertex->pt->z = 0.;
				}else	return false;
			}

			m_pViewFace->volume.x[LOW]  = ptCenter.x - nWidth*0.5;
			m_pViewFace->volume.y[LOW]  = ptCenter.x - nWidth*0.5;
			m_pViewFace->volume.z[LOW]  = 0.f;
			m_pViewFace->volume.x[HIGH] = ptCenter.x + nWidth*0.5;
			m_pViewFace->volume.y[HIGH] = ptCenter.x + nWidth*0.5;
			m_pViewFace->volume.z[HIGH] = 0.f;
		}
		else if(CHSRViewFormat::QUADRANT2_VIEW == ViewType)
		{
			if(NULL == (m_pViewFace->pHead = pHSRApp->alloc_vertex()))
			{
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(m_pViewFace->pHead,0,sizeof(HSR_VERTEX));
			
			//- first vertex ->
			PHSR_VERTEX pVertex = m_pViewFace->pHead;
			memset(pVertex,0,sizeof(HSR_VERTEX));
			if(pVertex->pt = pHSRApp->alloc_point())
			{
				pVertex->pt->x = ptCenter.x - nWidth*0.5 - nTol;
				pVertex->pt->y = ptCenter.y - nHeight*0.5- nTol;
				pVertex->pt->z = 0.;
			}else	return false;
			//<--
			
			//- second vertex -->
			if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
			{
				memset(pVertex->next,0,sizeof(HSR_VERTEX));
				pVertex = pVertex->next;
				if(pVertex->pt = pHSRApp->alloc_point())
				{
					pVertex->pt->x = ptCenter.x + nWidth*0.5 + nTol;
					pVertex->pt->y = ptCenter.y - nHeight*0.5- nTol;
					pVertex->pt->z = 0.;
				}else	return false;
				//<--
			}

			//- third vertex -->
			if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
			{
				memset(pVertex->next,0,sizeof(HSR_VERTEX));
				pVertex = pVertex->next;
				if(pVertex->pt = pHSRApp->alloc_point())
				{
					pVertex->pt->x = ptCenter.x + nWidth*0.5 - nTol;
					pVertex->pt->y = ptCenter.y + nHeight*0.5+ nTol;
					pVertex->pt->z = 0.;
				}else	return false;
			}

			const double dAngle = (0.5 * PI / 32.f);
			for(int i = 1;i < 32;++i)
			{
				const double cs = cos(dAngle * i);
				const double sn = sin(dAngle * i);
				double x = ptCenter.x + nWidth*0.5 - nTol - nHeight * sn;
				double y = ptCenter.y - nHeight*0.5- nTol + nHeight * cs;

				if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
				{
					memset(pVertex->next,0,sizeof(HSR_VERTEX));
					pVertex = pVertex->next;
					if(pVertex->pt = pHSRApp->alloc_point())
					{
						pVertex->pt->x = x;
						pVertex->pt->y = y;
						pVertex->pt->z = 0.;
					}else	return false;
					//<--
				}
			}
						
			if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
			{
				memset(pVertex->next,0,sizeof(HSR_VERTEX));
				pVertex = pVertex->next;
				if(pVertex->pt = pHSRApp->alloc_point()){
					pVertex->pt->x = ptCenter.x - nWidth*0.5 - nTol;
					pVertex->pt->y = ptCenter.y - nHeight*0.5- nTol;
					pVertex->pt->z = 0.;
				}else	return false;
			}

			m_pViewFace->volume.x[LOW]  = ptCenter.x - nWidth*0.5;
			m_pViewFace->volume.y[LOW]  = ptCenter.x - nWidth*0.5;
			m_pViewFace->volume.z[LOW]  = 0.f;
			m_pViewFace->volume.x[HIGH] = ptCenter.x + nWidth*0.5;
			m_pViewFace->volume.y[HIGH] = ptCenter.x + nWidth*0.5;
			m_pViewFace->volume.z[HIGH] = 0.f;
		}
		else if(CHSRViewFormat::QUADRANT3_VIEW == ViewType)
		{
			if(NULL == (m_pViewFace->pHead = pHSRApp->alloc_vertex()))
			{
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(m_pViewFace->pHead,0,sizeof(HSR_VERTEX));
			
			//- first vertex ->
			PHSR_VERTEX pVertex = m_pViewFace->pHead;
			memset(pVertex,0,sizeof(HSR_VERTEX));
			if(pVertex->pt = pHSRApp->alloc_point())
			{
				pVertex->pt->x = ptCenter.x + nWidth*0.5 - nTol;
				pVertex->pt->y = ptCenter.y - nHeight*0.5- nTol;
				pVertex->pt->z = 0.;				
			}else	return false;
			//<--
			
			//- second vertex -->
			if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
			{
				memset(pVertex->next,0,sizeof(HSR_VERTEX));
				pVertex = pVertex->next;
				if(pVertex->pt = pHSRApp->alloc_point())
				{
					pVertex->pt->x = ptCenter.x + nWidth*0.5 + nTol;
					pVertex->pt->y = ptCenter.y + nHeight*0.5- nTol;
					pVertex->pt->z = 0.;
				}else	return false;
				//<--
			}

			//- third vertex -->
			if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
			{
				memset(pVertex->next,0,sizeof(HSR_VERTEX));
				pVertex = pVertex->next;
				if(pVertex->pt = pHSRApp->alloc_point())
				{
					pVertex->pt->x = ptCenter.x - nWidth*0.5 - nTol;
					pVertex->pt->y = ptCenter.y + nHeight*0.5+ nTol;
					pVertex->pt->z = 0.;
				}else	return false;
			}

			const double dAngle = (0.5 * PI / 32.f);
			for(int i = 1;i < 32;++i)
			{
				const double cs = cos(dAngle * i);
				const double sn = sin(dAngle * i);
				double x = ptCenter.x + nWidth*0.5 - nTol - nWidth * cs;
				double y = ptCenter.y + nHeight*0.5- nTol - nWidth * sn;

				if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
				{
					memset(pVertex->next,0,sizeof(HSR_VERTEX));
					pVertex = pVertex->next;
					if(pVertex->pt = pHSRApp->alloc_point())
					{
						pVertex->pt->x = x;
						pVertex->pt->y = y;
						pVertex->pt->z = 0.;
					}else	return false;
					//<--
				}
			}
						
			if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
			{
				memset(pVertex->next,0,sizeof(HSR_VERTEX));
				pVertex = pVertex->next;
				if(pVertex->pt = pHSRApp->alloc_point())
				{
					*(pVertex->pt) = *(m_pViewFace->pHead->pt);
				}else	return false;
			}

			m_pViewFace->volume.x[LOW]  = ptCenter.x - nWidth*0.5;
			m_pViewFace->volume.y[LOW]  = ptCenter.x - nWidth*0.5;
			m_pViewFace->volume.z[LOW]  = 0.f;
			m_pViewFace->volume.x[HIGH] = ptCenter.x + nWidth*0.5;
			m_pViewFace->volume.y[HIGH] = ptCenter.x + nWidth*0.5;
			m_pViewFace->volume.z[HIGH] = 0.f;
		}
		else if(CHSRViewFormat::QUADRANT4_VIEW == ViewType)
		{
			if(NULL == (m_pViewFace->pHead = pHSRApp->alloc_vertex()))
			{
				pHSRApp->delete_hsr_face(m_pViewFace);
				
				return false;
			}
			memset(m_pViewFace->pHead,0,sizeof(HSR_VERTEX));
			
			//- first vertex ->
			PHSR_VERTEX pVertex = m_pViewFace->pHead;
			memset(pVertex,0,sizeof(HSR_VERTEX));
			if(pVertex->pt = pHSRApp->alloc_point())
			{
				pVertex->pt->x = ptCenter.x - nWidth*0.5 - nTol;
				pVertex->pt->y = ptCenter.y - nHeight*0.5- nTol;
				pVertex->pt->z = 0.;
			}else	return false;
			//<--
			
			const double dAngle = (0.5 * PI / 32.f);
			for(int i = 1;i < 32;++i)
			{
				const double cs = cos(dAngle * i);
				const double sn = sin(dAngle * i);
				double x = ptCenter.x - nWidth*0.5 - nTol + nHeight * sn;
				double y = ptCenter.y + nHeight*0.5- nTol - nHeight * cs;

				if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
				{
					memset(pVertex->next,0,sizeof(HSR_VERTEX));
					pVertex = pVertex->next;
					if(pVertex->pt = pHSRApp->alloc_point())
					{
						pVertex->pt->x = x;
						pVertex->pt->y = y;
						pVertex->pt->z = 0.;
					}else	return false;
					//<--
				}
			}

			//- second vertex -->
			if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
			{
				memset(pVertex->next,0,sizeof(HSR_VERTEX));
				pVertex = pVertex->next;
				if(pVertex->pt = pHSRApp->alloc_point())
				{
					pVertex->pt->x = ptCenter.x + nWidth*0.5 + nTol;
					pVertex->pt->y = ptCenter.y + nHeight*0.5- nTol;
					pVertex->pt->z = 0.;
				}else	return false;
				//<--
			}

			//- third vertex -->
			if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
			{
				memset(pVertex->next,0,sizeof(HSR_VERTEX));
				pVertex = pVertex->next;
				if(pVertex->pt = pHSRApp->alloc_point())
				{
					pVertex->pt->x = ptCenter.x - nWidth*0.5 - nTol;
					pVertex->pt->y = ptCenter.y + nHeight*0.5+ nTol;
					pVertex->pt->z = 0.;
				}else	return false;
			}
						
			if(NULL != (pVertex->next = (PHSR_VERTEX)pHSRApp->alloc_vertex()))
			{
				memset(pVertex->next,0,sizeof(HSR_VERTEX));
				pVertex = pVertex->next;
				if(pVertex->pt = pHSRApp->alloc_point())
				{
					*(pVertex->pt) = *(m_pViewFace->pHead->pt);
				}else	return false;
			}

			m_pViewFace->volume.x[LOW]  = ptCenter.x - nWidth*0.5;
			m_pViewFace->volume.y[LOW]  = ptCenter.x - nWidth*0.5;
			m_pViewFace->volume.z[LOW]  = 0.f;
			m_pViewFace->volume.x[HIGH] = ptCenter.x + nWidth*0.5;
			m_pViewFace->volume.y[HIGH] = ptCenter.x + nWidth*0.5;
			m_pViewFace->volume.z[HIGH] = 0.f;
		}
	}

	return true;
}

/**	
	@brief	set view boundary edge

	@author	BHK

	@date	?

	@param	ptCenter	a parameter of type const POINT_T
	@param	nWidth	a parameter of type const double
	@param	nHeight	a parameter of type const double

	@return	void	
*/
void CHSRView::SetViewBoundray(const POINT_T ptCenter,const double nWidth,const double nHeight){
	m_nViewBoundaryEdge = 4;
	m_ViewBoundary[0].ptStart.x = ptCenter.x - nWidth*0.5;
	m_ViewBoundary[0].ptStart.y = ptCenter.y - nHeight*0.5;
	m_ViewBoundary[0].ptStart.z = 0;
	m_ViewBoundary[0].ptEnd.x = ptCenter.x + nWidth*0.5;
	m_ViewBoundary[0].ptEnd.y = ptCenter.y - nHeight*0.5;
	m_ViewBoundary[0].ptEnd.z = 0;
	m_ViewBoundary[1].ptStart.x = ptCenter.x + nWidth*0.5;
	m_ViewBoundary[1].ptStart.y = ptCenter.y - nHeight*0.5;
	m_ViewBoundary[1].ptStart.z = 0;
	m_ViewBoundary[1].ptEnd.x = ptCenter.x + nWidth*0.5;
	m_ViewBoundary[1].ptEnd.y = ptCenter.y + nHeight*0.5;
	m_ViewBoundary[1].ptEnd.z = 0;
	m_ViewBoundary[2].ptStart.x = ptCenter.x + nWidth*0.5;
	m_ViewBoundary[2].ptStart.y = ptCenter.y + nHeight*0.5;
	m_ViewBoundary[2].ptStart.z = 0;
	m_ViewBoundary[2].ptEnd.x = ptCenter.x - nWidth*0.5;
	m_ViewBoundary[2].ptEnd.y = ptCenter.y + nHeight*0.5;
	m_ViewBoundary[2].ptEnd.z = 0;
	m_ViewBoundary[3].ptStart.x = ptCenter.x - nWidth*0.5;
	m_ViewBoundary[3].ptStart.y = ptCenter.y + nHeight*0.5;
	m_ViewBoundary[3].ptStart.z = 0;
	m_ViewBoundary[3].ptEnd.x = ptCenter.x - nWidth*0.5;
	m_ViewBoundary[3].ptEnd.y = ptCenter.y - nHeight*0.5;
	m_ViewBoundary[3].ptEnd.z = 0;
}

/**	\brief	The CHSRView::Clipping function\n
	1. if line type is single line split center line only.\n
	2. else if line type is double line split double line and center line\n
	3. clipping each line segment

	\param	pMergeFace	a parameter of type PFACE

	\remarks\n
	;2002.03.18 - fix split line routin that skipped line's segment.

	\return	void	
*/
void CHSRView::Clipping(CHSRFace* pMergeFace){
	assert(m_pViewFace && "view face is invalid");
	assert(pMergeFace  && "face list is invalid");
	CHSRFace* pFace = NULL;
	PHSR_VERTEX pVt,pRenderVt = NULL;

	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	PIPELINEMODE LineMode  = pHSRApp->GetEnv()->GetPipeLineMode();
	try{
		for(pFace = pMergeFace;pFace;pFace = pFace->next){
			pRenderVt = pFace->pHead;
			if((pFace->type == PIPE || pFace->type == TEE || pFace->type == ELBOW) && pFace->pCenterline){
				if((SINGLE_LINE == LineMode) || 
					((BOTH == LineMode) && (pFace->radius <= pHSRApp->GetEnv()->GetCriticalRadius()))){
					pRenderVt = pFace->pCenterline;
				}
			}
			
			PHSR_VERTEX pTmpVt=NULL;
			if(pRenderVt == pFace->pCenterline){
				for(pVt = pRenderVt;(pVt && pVt->next);){
					pTmpVt = pVt->next;
					if(!m_pViewFace->SplitLine(pVt,pVt->next))
						pVt = pVt->next;
					//else	pVt = pTmpVt;
				}
				
				//			for(pVt = pFace->pCenterline;(pVt && pVt->next);){
				//				pTmpVt = pVt->next;
				//				m_pViewFace->SplitLine(pVt,pVt->next);
				//				pVt    = pTmpVt;
				//			}
			}else{
				for(pVt = pFace->pHead;(pVt && pVt->next);){
					pTmpVt = pVt->next;
					if(!m_pViewFace->SplitLine(pVt,pVt->next))
						//pVt = pTmpVt;
						pVt = pVt->next;
					//else	pVt = pTmpVt;
				}
				
				for(pVt = pFace->pCenterline;(pVt && pVt->next);){
					pTmpVt = pVt->next;
					if(!m_pViewFace->SplitLine(pVt,pVt->next))
						pVt = pVt->next;
					//else	pVt = pTmpVt;
				}
			}
			
			// split cross line
			for(pVt = pFace->pCrossline;pVt && pVt->next;pVt = pVt->next){
				pTmpVt = pVt->next;
				if(INTERSECTION == pVt->render) m_pViewFace->SplitLine(pVt,pVt->next);
				pVt    = pTmpVt;
			}
			
			// below codes are clipping line segments
			pRenderVt = pFace->pHead;
			if((pFace->type == PIPE || pFace->type == TEE || pFace->type == ELBOW) && pFace->pCenterline){
				if((SINGLE_LINE == LineMode) || 
					((BOTH == LineMode) && (pFace->radius <= pHSRApp->GetEnv()->GetCriticalRadius()))){
					pRenderVt = pFace->pCenterline;
				}
			}
			
			if(pRenderVt == pFace->pCenterline){
				for(pVt = pRenderVt;pVt && pVt->next;pVt = pVt->next){
					if(!m_pViewFace->IsHiddenLine(pVt,pVt->next,true))
						pVt->render = INVALID;
				}
			}else{
				
				for(pVt = pFace->pHead;pVt && pVt->next;pVt = pVt->next){
					if(!m_pViewFace->IsHiddenLine(pVt,pVt->next,true))
						pVt->render = INVALID;
				}
				
				for(pVt = pFace->pCenterline;pVt && pVt->next;pVt = pVt->next){
					if(!m_pViewFace->IsHiddenLine(pVt,pVt->next,true))
						pVt->render = INVALID;
				}
			}
			
			for(pVt = pFace->pCrossline;pVt && pVt->next;pVt = pVt->next){
				if((INTERSECTION) && !m_pViewFace->IsHiddenLine(pVt,pVt->next,true))
					pVt->render = INVALID;
			}
		}
	}catch(const exception&){
		//OuputDebugString(ex.what());
	}
}

/**	\brief	The CHSRView::GetViewScale function


	\return	double	
*/
double CHSRView::GetViewScale()
{
	return m_nScale;
}

/**	\brief	The CHSRView::GetViewArea function


	\return	RECT_T	
*/
RECT_T CHSRView::GetViewArea()
{
	assert(m_pViewFace && "m_pViewFace is NULL");
	RECT_T rect={0};

	if(m_pViewFace)
	{
		rect.x     = m_pViewFace->volume.x[LOW];
		rect.y     = m_pViewFace->volume.y[LOW];
		rect.width = m_pViewFace->volume.x[HIGH] - m_pViewFace->volume.x[LOW];
		rect.height= m_pViewFace->volume.y[HIGH] - m_pViewFace->volume.y[LOW];
	}

	return rect;
}

/**	\brief	The CHSRView::WriteViewBorder function

	\param	ret	a parameter of type PSQPRIMITIVE

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRView::WriteViewBorder(PSQPRIMITIVE ret)
{
	assert(m_pViewFace && "m_pViewFace is NULL");

	if(m_pViewFace){
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		PPOINT_TNODE polyline  = NULL,pTail=NULL;
		PHSR_VERTEX      ptr=NULL;
		PSQPRIMITIVE pPrimitive= NULL;
		if(NULL != (pPrimitive = pHSRApp->alloc_primitive())){
			ptr = m_pViewFace->pHead;

			if(NULL == polyline)
				polyline = pTail = PolylinePush(polyline,ptr->pt);
			else	pTail = PolylinePush(pTail,ptr->pt);
			ptr = ptr->next;
			
			assert(ptr && ptr->pt);
			if(NULL == polyline)
				polyline = pTail = PolylinePush(polyline,ptr->pt);
			else	pTail = PolylinePush(pTail,ptr->pt);
			ptr = ptr->next;
			
			assert(ptr && ptr->pt);
			if(NULL == polyline)
				polyline = pTail = PolylinePush(polyline,ptr->pt);
			else	pTail = PolylinePush(pTail,ptr->pt);
			ptr = ptr->next;
			
			assert(ptr && ptr->pt);
			if(NULL == polyline)
				polyline = pTail = PolylinePush(polyline,ptr->pt);
			else	pTail = PolylinePush(pTail,ptr->pt);
			ptr = ptr->next;
			
			assert(ptr && ptr->pt);
			if(NULL == polyline)
				polyline = pTail = PolylinePush(polyline,ptr->pt);
			else	pTail = PolylinePush(pTail,ptr->pt);
			
			pPrimitive->hdr.id    = ENM_POLYLINE;
			pPrimitive->hdr.display.lscale= 0.;
			pPrimitive->body.polyline    = polyline;
			
			ret = AppendPrimitive(ret,pPrimitive);
		}
	}

	return ret;
}

/**	\brief	The CHSRView::GetViewPoint function


	\return	VIEWPOINT	
*/
VIEWPOINT CHSRView::GetViewPoint()
{
	return m_ViewPoint;
}

/**	\brief	The CHSRView::SetViewPoint function

	\param	vpt	a parameter of type VIEWPOINT

	\return	void	
*/
void CHSRView::SetViewPoint(VIEWPOINT vpt)
{
	m_ViewPoint = vpt;
}

/**	\brief	The CHSRView::AppendPrimitive function\n
	append append-primitive to primitive list.

	\param	primitive	a parameter of type PSQPRIMITIVE
	\param	append	a parameter of type PSQPRIMITIVE

	\return	PSQPRIMITIVE(primitive list's header)
*/
PSQPRIMITIVE CHSRView::AppendPrimitive(PSQPRIMITIVE primitive,PSQPRIMITIVE append){
	assert(append);

	if(primitive && append){
		PSQPRIMITIVE p = NULL;

		for(p = primitive;p && p->next;p = p->next);
		if(p){
			p->next      = append;
			append->next = NULL;
		}
	}else if(append){
		append->next = NULL;
		return append;
	}

	return primitive;
}


void CHSRView::SetUOR(const double &nUOR)
{
	m_nUOR = nUOR;
}

const double CHSRView::GetUOR()
{
	return m_nUOR;
}
