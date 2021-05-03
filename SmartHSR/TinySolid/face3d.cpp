#include "wedge.h"
#include "face3d.h"

/**	\brief
*/
CFace3D::CFace3D(CWEdge* pWEdge,CSurface3D* pParent){
    	m_pWEdge   = pWEdge;
    	m_pParent  = pParent;

    	m_pPrev = NULL;
	m_pNext = NULL;
}

/**	\brief
*/
CFace3D::~CFace3D(){
}

//void CFace3D::Init(){
//	if(m_pCurve){
//	    	PSQPOINT pPoints=NULL;
		
//	    	KArray<SQPOINT>* paryPoints = m_pCurve->GetFacets();
//		if(pPoints = malloc(sizeof(SQPOINT) * paryPoints->Length())){
//			for(int i = 0;i < pPoints->Length() - 1;i++){
//			    	CWEdge *pWedge = new CWEdge(&pPoints[i],&pPoints[i + 1],this);
//				(*this) += pWEdge;
//			}
//		}
//	}
//}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
/**	\brief	The CFace3D::operator() function

	\param	nIndex	a parameter of type int

	\return	CWEdge*
*/
CWEdge* CFace3D::operator()(int nIndex){
    	int i=0;
	CWEdge* ptr=NULL;
	for(i=0,ptr=m_pWEdge;(i < nIndex) && ptr;ptr = ptr->Next());
	
	return ptr;
}

/**	\brief	The CFace3D::operator+= function\n
	append edge

	\param	pWEdge	a parameter of type CWEdge*

	\return	void	
*/
void CFace3D::operator+=(CWEdge* pWEdge){
    	if(m_pWEdge){
	    	CWEdge* ptr=NULL;
		for(ptr = m_pWEdge;ptr && ptr->Next();ptr = ptr->Next());
		if(ptr)    ptr->m_pNext = pWEdge;
		if(pWEdge) pWEdge->m_pPrev = ptr;
	}else{
	    	m_pWEdge = pWEdge;
	}
}

/**	\brief	The CFace3D::GetParent function


	\return	CSurface3D*	
*/
CSurface3D* CFace3D::GetParent(){
    	return m_pParent;
}

/**	\brief	The CFace3D::SetParent function

	\param	pParent	a parameter of type CSurface3D*

	\return	void	
*/
void CFace3D::SetParent(CSurface3D* pParent){
    	m_pParent = pParent;
}

/**	\brief	The CFace3D::GetWEdge function


	\return	CWEdge*	
*/
CWEdge* CFace3D::GetWEdge(){
	return m_pWEdge;
}

/**	\brief	The CFace3D::Prev function


	\return	CFace3D*	
*/
CFace3D* CFace3D::Prev(){
    	return m_pPrev;
}

/**	\brief	The CFace3D::Next function


	\return	CFace3D*
*/
CFace3D* CFace3D::Next(){
    	return m_pNext;
}
