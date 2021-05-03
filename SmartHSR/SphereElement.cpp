// PreSPHEREFormat.cpp: implementation of the CSphereElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <graphics\geometry.h>
#include "SmartHSR.h"
#include "SphereElement.h"

IMPLEMENTS_HSR_ELEMENT(CSphereElement , CHSRElement , _T("__SPHERE_HSR_ELEMENT__"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CSphereElement::CSphereElement()
{
	m_nSegments = 16;
}

CSphereElement::~CSphereElement()
{

}

/**	\brief	The CSphereElement::ParseLine function

	\param	pHSRScanner	a parameter of type CHSRScanner*

	\return	void	
*/
#ifdef VER_03
bool CSphereElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		char* pVal=NULL;
		
		m_strLayer = pHSRScanner->Val("layer");
		if(pVal = pHSRScanner->Val("pt"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_ptOrigin.x),&(m_ptOrigin.y),&(m_ptOrigin.z));
		}
		if(pVal = pHSRScanner->Val("radius")) m_nRadius = ATOF_T(pVal)*0.5;

		bRet = true;
	}

	return bRet;
}
#else
/******************************************************************************
    @author     humkyung
    @date       2012-03-07
    @class      CSphereElement
    @function   ParseLine
    @return     bool
    @param      CHSRScanner*    pHSRScanner
    @brief
******************************************************************************/
bool CSphereElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;
		m_nRadius=ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());

		m_ptOrigin.x = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
		m_ptOrigin.y = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
		m_ptOrigin.z = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());

		if(pHSRScanner->m_nFactor > 5)
		{
			SetColorString((*pHSRScanner->m_aryFactor)[5]);
		}

		bRet = true;
	}

	return bRet;
}
#endif

/**	
	@brief	The CSphereElement::IsInVolume function

	@author	humkyung

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CSphereElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_ptOrigin.x >= volume.minx()) && (m_ptOrigin.x <= volume.maxx()) &&
		(m_ptOrigin.y >= volume.miny()) && (m_ptOrigin.y <= volume.maxy()) &&
		(m_ptOrigin.z >= volume.minz()) && (m_ptOrigin.z <= volume.maxz()))
		return true;

	return false;
}

/**	
	@brief	The CSphereElement::CreateFace function

	@author	BHK

	@date	?

	@param	pViewFormat	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@return	PFACE	
*/
CHSRFace* CSphereElement::CreateFace(CHSRViewFormat* pViewFormat,long& nID)
{
	assert(pViewFormat && "pViewFormat is NULL");
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pViewFormat)
	{
		double nRadius  =pViewFormat->MODEL2VIEW(m_nRadius);
		POINT_T ptOrigin=pViewFormat->MODEL2VIEW(m_ptOrigin);
		VECTOR_T vecNorm={0.,0.,1.};

		POINT_T ptEdge[32];
		int nCount=CreateCircleEdge(ptEdge,ptOrigin,nRadius,vecNorm);
		
		CHSRFace* pSphere=CHSRFace::CreateFace(nCount,ptEdge);
		if(pSphere)
		{
			pSphere->type  = FACE_T;
			pSphere->id    = nID++;
			pSphere->ele   = HSR_CIRCLE_SHAPE;
			pSphere->ptCenter = ptOrigin;
			pSphere->radius   = nRadius;
			pSphere->m_enmLayer = GetHSRLayer();
			pSphere->SetColor(*m_pstrColor);

			pHSRApp->AppendFace(NULL,pSphere);
		}
	}

	return pRet;
}

/**	\brief	The CSphereElement::CreateCircleEdge function

	\param	ptEdge[]	a parameter of type POINT_T
	\param	ptOrigin	a parameter of type POINT_T
	\param	nRadius	a parameter of type double
	\param	vecNorm	a parameter of type VECTOR_T

	\return	int	
*/
int CSphereElement::CreateCircleEdge(POINT_T ptEdge[],POINT_T ptOrigin,double nRadius,VECTOR_T vecNorm)
{
	int nCount=0;
	
	double alpha=0.,beta=0;
	CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
	
	double nStep    = 2*PI/m_nSegments;
	int i=0;
	for(double t=0;t < 2*PI;t += nStep,i++,nCount++)
	{
		ptEdge[i].x = nRadius*cos(t);
		ptEdge[i].y = nRadius*sin(t);
		ptEdge[i].z = 0.;
		ptEdge[i] = CGeometry::RotateAboutYXAxis(ptEdge[i],-beta,-alpha);
		ptEdge[i].x = ptOrigin.x + ptEdge[i].x;
		ptEdge[i].y = ptOrigin.y + ptEdge[i].y;
		ptEdge[i].z = ptOrigin.z + ptEdge[i].z;
	}
	ptEdge[i].x = nRadius*cos(2*PI);
	ptEdge[i].y = nRadius*sin(2*PI);
	ptEdge[i].z = 0.;
	ptEdge[i]   = CGeometry::RotateAboutYXAxis(ptEdge[i],-beta,-alpha);
	ptEdge[i].x = ptOrigin.x + ptEdge[i].x;
	ptEdge[i].y = ptOrigin.y + ptEdge[i].y;
	ptEdge[i].z = ptOrigin.z + ptEdge[i].z;
	nCount++;

	return nCount;
}