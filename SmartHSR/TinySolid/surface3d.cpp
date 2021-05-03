#include "wedge.h"
#include "face3d.h"
#include "surface3d.h"

CSurfacePara CSurface3D::para[1024];
POINT_T CSurface3D::pt[1024]={0.};
LINE_T  CSurface3D::edge[1024]={0.};

/**	\brief
*/
CSurface3D::CSurface3D(CFace3D* pFace,CBody* pParent){
    	m_nPoints = 0;
	m_nFacets = 0;

	m_pFace   = pFace;
    	m_pParent = pParent;
    	
	m_pPrev = NULL;
	m_pNext = NULL;
}

/**	\brief
*/
CSurface3D::CSurface3D(int nParts,int nPoints,PPOINT_T pptPoints,CBody* pParent){
	m_nPoints = nParts*nPoints;
	m_nFacets = nParts;

	m_pFace   = NULL;
	m_pParent = pParent;
	m_pPrev   = NULL;
	m_pNext   = NULL;
	
//	Mesh(nParts,nPoints,pptPoints);
}

/**	\brief
*/
CSurface3D::CSurface3D(int idSurface,KArray<POINT_T>* paryBoundary,CBody* pParent){
    	m_pFace  = NULL;
	m_pParent= pParent;
	m_pPrev  = NULL;
	m_pNext  = NULL;

	//Mesh(idSurface,paryBoundary,pParent);
}

/**	\brief
*/
CSurface3D::~CSurface3D(){
}

/**	\brief	The CSurface3D::func function

	\param	para	a parameter of type const CSurfacePara&

	\return	POINT_T	
*/
POINT_T CSurface3D::func(const CSurfacePara& para){
	POINT_T ptRet={0.,0.,0.};

	return ptRet;
}

/**	\brief	The CSurface3D::func function

	\param	pt	a parameter of type const POINT_T&

	\return	CSurfacePara	
*/
//CSurfacePara CSurface3D::func(const POINT_T& pt){
//	CSurfacePara para={0.,0.};
//
//	return para;
//}

/**	\brief	The CSurface3D::operator+= function

	\param	pFace	a parameter of type CFace3D*

	\return	void	
*/
void CSurface3D::operator+=(CFace3D* pFace){
	if(m_pFace){
	    	CFace3D* ptr=NULL;
		for(ptr = m_pFace;ptr && ptr->Next();ptr = ptr->Next());
		if(ptr)	  ptr->m_pNext = pFace;
		if(pFace) pFace->m_pPrev = ptr;
	}else{
	    	m_pFace = pFace;
	}
}

/**	\brief	The CSurface3D::GetParent function


	\return	CBody*	
*/
CBody* CSurface3D::GetParent(){
    	return m_pParent;
}

/**	\brief	The CSurface3D::SetParent function

	\param	pParent	a parameter of type CBody*

	\return	void	
*/
void CSurface3D::SetParent(CBody* pParent){
    	m_pParent = pParent;
}

/**	\brief	The CSurface3D::GetFace function


	\return	CFace3D*	
*/
CFace3D* CSurface3D::GetFace(){
    	return m_pFace;
}

/**	\brief	The CSurface3D::Prev function


	\return	CSurface3D*	
*/
CSurface3D* CSurface3D::Prev(){
    	return m_pPrev;
}

/**	\brief	The CSurface3D::Next function


	\return	CSurface3D*	
*/
CSurface3D* CSurface3D::Next(){
    	return m_pNext;
}

/**	\brief	The CSurface3D::EnableToMakeFace function

	\param	plane	a parameter of type CPlane3d&
	\param	nParts	a parameter of type int
	\param	nPoints	a parameter of type int
	\param	pptPoints	a parameter of type PPOINT_T

	\return	bool	
*/
bool CSurface3D::EnableToMakeFace(CPlane3d& plane,int nParts,int nPoints,PPOINT_T pptPoints){
	assert(pptPoints && "pptPoints is NULL");

	if(pptPoints){
		bool bPositive=false;
		bool bNegative=false;

		for(int j=0;j < nParts;j++){
			for(int i=0;i < nPoints;i++){
				SIGN_T side=plane.WhichSideOfPlane(pptPoints[j*nPoints + i]);
				if(POSITIVE == side) bPositive=true;
				if(NEGATIVE == side) bNegative=true;
				if(bPositive && bNegative) return false;
			}
		}
	}

	return true;
}
