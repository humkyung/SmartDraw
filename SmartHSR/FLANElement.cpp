// FLANElement.cpp: implementation of the CFLANElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include "SmartHSR.h"
#include "FLANElement.h"

IMPLEMENTS_HSR_ELEMENT(CFLANElement , CHSRElement , _T("__FLAN_HSR_ELEMENT__"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**	\brief
*/
CFLANElement::CFLANElement()
{
	m_nSegments = 16;
	m_nPIPEs    = 2;
}

/**	\brief
*/
CFLANElement::~CFLANElement()
{

}

/**	\brief	The CFLANElement::ParseLine function

	\param	pHSRScanner	a parameter of type CHSRScanner*

	\remarks\n
	;2003.03.28 - append code reading SO FLANGE.

	\return	bool	
*/
bool CFLANElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;

	if(pHSRScanner && (15 == pHSRScanner->m_nFactor))
	{
		m_nPIPEs = 2;

		m_nRadius[0] = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
		m_pt[0][0].x = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
		m_pt[0][0].y = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
		m_pt[0][0].z = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
		m_pt[0][1].x = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
		m_pt[0][1].y = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
		m_pt[0][1].z = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());

		m_nRadius[1] = ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());
		m_pt[1][0].x = ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str());
		m_pt[1][0].y = ATOF_T((*pHSRScanner->m_aryFactor)[10].c_str());
		m_pt[1][0].z = ATOF_T((*pHSRScanner->m_aryFactor)[11].c_str());
		m_pt[1][1].x = ATOF_T((*pHSRScanner->m_aryFactor)[12].c_str());
		m_pt[1][1].y = ATOF_T((*pHSRScanner->m_aryFactor)[13].c_str());
		m_pt[1][1].z = ATOF_T((*pHSRScanner->m_aryFactor)[14].c_str());

		bRet = true;
	}else if(pHSRScanner && (8 == pHSRScanner->m_nFactor)){
		m_nPIPEs = 1;
		
		m_nRadius[0] = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
		m_pt[0][0].x = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
		m_pt[0][0].y = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
		m_pt[0][0].z = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
		m_pt[0][1].x = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
		m_pt[0][1].y = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
		m_pt[0][1].z = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());

		bRet = true;
	}

	return bRet;
}

/**	
	@brief	The CFLANElement::IsInVolume function

	@author	humkyung

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CFLANElement::IsInVolume(const CIsVolume& volume)
{
	for(int i=0;i < m_nPIPEs;i++){
		if(	(m_pt[i][0].x >= volume.minx()) && (m_pt[i][0].x <= volume.maxx()) &&
			(m_pt[i][0].y >= volume.miny()) && (m_pt[i][0].y <= volume.maxy()) &&
			(m_pt[i][0].z >= volume.minz()) && (m_pt[i][0].z <= volume.maxz())) return true;
		
		if(	(m_pt[i][1].x >= volume.minx()) && (m_pt[i][1].x <= volume.maxx()) &&
			(m_pt[i][1].y >= volume.miny()) && (m_pt[i][1].y <= volume.maxy()) &&
			(m_pt[i][1].z >= volume.minz()) && (m_pt[i][1].z <= volume.maxz())) return true;
	}

	return false;
}

/**	
	@brief	The CFLANElement::CreateFace function

	@param	pVIEWFormat	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@remarks\n
	;2003.03.28 - increase elevation of outline about horizontal FLANGE

	@return	PFACE	
*/
CHSRFace* CFLANElement::CreateFace(CHSRViewFormat* pVIEWFormat,long& nID)
{
	assert(pVIEWFormat && "pVIEWFormat is NULL");
	static POINT_T ptEdge[36];
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	if(pVIEWFormat)
	{	
		POINT_T pt[2]={0.,};
		double  nRadius=0.;
		for(int count=0;count < m_nPIPEs;count++)
		{
			pt[0]  = pVIEWFormat->MODEL2VIEW(m_pt[count][0]);
			pt[1]  = pVIEWFormat->MODEL2VIEW(m_pt[count][1]);
			nRadius= pVIEWFormat->MODEL2VIEW(m_nRadius[count]);
			
			VECTOR_T vec={0.},vecZ={0.,0.,1.};
			vec.dx=pt[1].x - pt[0].x;
			vec.dy=pt[1].y - pt[0].y;
			vec.dz=pt[1].z - pt[0].z;
			if(!CMath::NormalizeVector(vec)) return NULL;
			double nDot=CMath::DotProduct(vec,vecZ);
			
			CHSRFace *pSide=NULL,*pTop=NULL,*pBottom=NULL;
			if(fabs(fabs(nDot) - 1) < 0.001)
			{
				POINT_T ptOrigin=(pt[0].z > pt[1].z) ? pt[0] : pt[1];
				if(pTop=CHSRFace::CreateCircleFace(ptOrigin,nRadius,vec,m_nSegments))
				{
					pTop->CalcPlaneEquation();
					pTop->type  = FLANGE;
					pTop->id    = nID++;
					//pTop->ele   = (pTop->IsPerpendicularToViewPoint()) ? SECTION : CIRCLE;;
					pTop->ele   = HSR_CIRCLE_SHAPE;//(pTop->IsPerpendicularToViewPoint()) ? CIRCLE : CIRCLE;
					pTop->ptCenter = ptOrigin;
					pTop->radius   = nRadius;
					pTop->SetColor(*m_pstrColor);
				}
			}
			else if(fabs(nDot) < 0.001)
			{
				VECTOR_T vecCross={0};
				CMath::GetCrossProduct(vecCross,vec,vecZ);
				CMath::NormalizeVector(vecCross);
				
				ptEdge[0].x = pt[0].x + vecCross.dx*nRadius;
				ptEdge[0].y = pt[0].y + vecCross.dy*nRadius;
				ptEdge[0].z = (pt[0].z + nRadius) + vecCross.dz*nRadius;
				ptEdge[1].x = pt[0].x - vecCross.dx*nRadius;
				ptEdge[1].y = pt[0].y - vecCross.dy*nRadius;
				ptEdge[1].z = (pt[0].z + nRadius) - vecCross.dz*nRadius;
				ptEdge[2].x = pt[1].x - vecCross.dx*nRadius;
				ptEdge[2].y = pt[1].y - vecCross.dy*nRadius;
				ptEdge[2].z = (pt[1].z + nRadius) - vecCross.dz*nRadius;
				ptEdge[3].x = pt[1].x + vecCross.dx*nRadius;
				ptEdge[3].y = pt[1].y + vecCross.dy*nRadius;
				ptEdge[3].z = (pt[1].z + nRadius) + vecCross.dz*nRadius;
				ptEdge[4].x = pt[0].x + vecCross.dx*nRadius;
				ptEdge[4].y = pt[0].y + vecCross.dy*nRadius;
				ptEdge[4].z = (pt[0].z + nRadius) + vecCross.dz*nRadius;
				
				pSide=CHSRFace::CreateFace(5,ptEdge);
				if(pSide){
					pSide->type  = FLANGE;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;
					pSide->SetColor(*m_pstrColor);
				}
			}
			else
			{
				VECTOR_T vecCross={0};
				CMath::GetCrossProduct(vecCross,vec,vecZ);
				CMath::NormalizeVector(vecCross);
				
				ptEdge[0].x = pt[0].x + vecCross.dx*nRadius;
				ptEdge[0].y = pt[0].y + vecCross.dy*nRadius;
				ptEdge[0].z = pt[0].z + vecCross.dz*nRadius;
				ptEdge[1].x = pt[0].x - vecCross.dx*nRadius;
				ptEdge[1].y = pt[0].y - vecCross.dy*nRadius;
				ptEdge[1].z = pt[0].z - vecCross.dz*nRadius;
				ptEdge[2].x = pt[1].x - vecCross.dx*nRadius;
				ptEdge[2].y = pt[1].y - vecCross.dy*nRadius;
				ptEdge[2].z = pt[1].z - vecCross.dz*nRadius;
				ptEdge[3].x = pt[1].x + vecCross.dx*nRadius;
				ptEdge[3].y = pt[1].y + vecCross.dy*nRadius;
				ptEdge[3].z = pt[1].z + vecCross.dz*nRadius;
				ptEdge[4].x = pt[0].x + vecCross.dx*nRadius;
				ptEdge[4].y = pt[0].y + vecCross.dy*nRadius;
				ptEdge[4].z = pt[0].z + vecCross.dz*nRadius;
				
				pSide=CHSRFace::CreateFace(5,ptEdge);
				if(pSide)
				{
					pSide->type  = FLANGE;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;
					pSide->SetColor(*m_pstrColor);
				}
				
				if(pTop=CHSRFace::CreateCircleFace(pt[0],nRadius,vec,m_nSegments))
				{
					pTop->type    = PIPE;
					pTop->id      = nID++;
					pTop->ele     = HSR_CIRCLE_SHAPE;
					pTop->ptCenter= pt[0];
					pTop->radius  = nRadius;
					pTop->SetColor(*m_pstrColor);
				}
				
				if(pBottom=CHSRFace::CreateCircleFace(pt[1],nRadius,vec,m_nSegments))
				{
					pBottom->type    = PIPE;
					pBottom->id      = nID++;
					pBottom->ele     = HSR_CIRCLE_SHAPE;
					pBottom->ptCenter= pt[1];
					pBottom->radius  = nRadius;
					pBottom->SetColor(*m_pstrColor);
				}
			}
			pRet = pHSRApp->m_clsPreProcessor.Run(pRet,FLANGE,pTop,pSide,pBottom);
		}
	}
	
	return pRet;
}