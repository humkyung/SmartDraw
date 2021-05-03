// ArcElement.cpp: implementation of the CArcElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include "SmartHSR.h"
#include "ArcElement.h"
#include "LineElement.h"

IMPLEMENTS_HSR_ELEMENT(CArcElement , CHSRElement , _T("ARC"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArcElement::CArcElement()
{

}

CArcElement::~CArcElement()
{
	try
	{
		for(list<CLineElement*>::iterator itr = m_LineElementEntry.begin();itr != m_LineElementEntry.end();++itr)
		{
			delete (*itr);
		}
		m_LineElementEntry.clear();
	}
	catch(...)
	{
	}
}

#ifdef VER_03
bool CArcElement::ParseLine(CHSRScanner *pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet = false;

	if(pHSRScanner)
	{
	}

	return bRet;
}
#else
bool CArcElement::ParseLine(CHSRScanner *pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet = false;

	if(pHSRScanner)
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;
		m_nGroup = ATOI_T((*pHSRScanner->m_aryFactor)[1].c_str());
		
		m_arc.ptOrigin.x = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
		m_arc.ptOrigin.y = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
		m_arc.ptOrigin.z = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
		m_arc.ptStart.x = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
		m_arc.ptStart.y = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
		m_arc.ptStart.z = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());
		m_arc.nSweep = ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());
		
		m_arc.vecNorm.dx = ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str());
		m_arc.vecNorm.dy = ATOF_T((*pHSRScanner->m_aryFactor)[10].c_str());
		m_arc.vecNorm.dz = ATOF_T((*pHSRScanner->m_aryFactor)[11].c_str());
		
		m_arc.nRatio = 1.f;
		if(pHSRScanner->m_nFactor > 13)
		{
			/// primary axis,secondary axis
			m_arc.nRatio = ATOF_T((*pHSRScanner->m_aryFactor)[12].c_str()) / ATOF_T((*pHSRScanner->m_aryFactor)[13].c_str());
			if(pHSRScanner->m_nFactor > 14)
			{
				SetColorString((*pHSRScanner->m_aryFactor)[14]);
			}
		}
		else if(pHSRScanner->m_nFactor > 12)
		{
			SetColorString((*pHSRScanner->m_aryFactor)[12]);
		}
		
		bRet = true;
	}

	return bRet;
}
#endif

/**
 * IsInVolume:
 *
 * @param volume 
 * @return bool 
 */
bool CArcElement::IsInVolume(const CIsVolume& volume)
{
	const double dx = m_arc.ptStart.x - m_arc.ptOrigin.x;
	const double dy = m_arc.ptStart.y - m_arc.ptOrigin.y;
	const double dz = m_arc.ptStart.z - m_arc.ptOrigin.z;
	const double radius = sqrt(dx*dx + dy*dy + dz*dz);

	if(m_arc.ptOrigin.x + radius < volume.minx()) return false;
	if(m_arc.ptOrigin.x - radius > volume.maxx()) return false;
	if(m_arc.ptOrigin.y + radius < volume.miny()) return false;
	if(m_arc.ptOrigin.y - radius > volume.maxy()) return false;
	if(m_arc.ptOrigin.z + radius < volume.minz()) return false;
	if(m_arc.ptOrigin.z - radius > volume.maxz()) return false;

	return true;
}

/**
	@brief	CreateFace:
	@author	humkyung
	@date	?
	@param	
	@return	CHSRFace*
 */
CHSRFace* CArcElement::CreateFace(CHSRViewFormat* pViewFormat,long& nID)
{
	assert(pViewFormat && "pViewFormat is NULL");
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pViewFormat)
	{
		if(CHSRElement::STRUCTURE_MODEL_T == this->m_model.modelType) return pRet;	/// 2015.03.23 added by humkyung

		double nTol=0.01;
		CHSRFace *pSide=NULL,*pTop=NULL,*pBottom=NULL;

		VECTOR_T vec[3]={0.,} , vecZ={0.,0.,1.};
		POINT_T ptCenter = pViewFormat->MODEL2VIEW(m_arc.ptOrigin);
		POINT_T ptStart  = pViewFormat->MODEL2VIEW(m_arc.ptStart);
		const double dx = ptStart.x - ptCenter.x;
		const double dy = ptStart.y - ptCenter.y;
		const double dz = ptStart.z - ptCenter.z;
		const double radius = sqrt(dx*dx + dy*dy + dz*dz);
		VECTOR_T _vecNorm = pViewFormat->MODEL2VIEW(m_arc.vecNorm);
		
		///--> 2003.01.02
		CVect vecNorm; // normal vector of top,bottom face
		vecNorm.dx = _vecNorm.dx;
		vecNorm.dy = _vecNorm.dy;
		vecNorm.dz = _vecNorm.dz;
		if(!CMath::NormalizeVector(vecNorm)) return NULL;
		
		pRet = CHSRFace::CreateCircleFace(ptCenter , radius , vecNorm , 16);
		if(pRet)
		{
			pRet->id  = nID++;
			pRet->m_enmLayer = GetHSRLayer();
			pRet->type= OTHER;
			pRet->SetColor(*m_pstrColor);
			
			double nDot=CMath::DotProduct(vecNorm,vecZ);
			if(fabs(fabs(nDot) - 1) < 0.001)	// stand up
			{
				pRet->ele = HSR_CIRCLE_SHAPE;
				pRet->ptCenter = ptCenter;
				pRet->radius   = radius;
			}
			else
			{
				pRet->ele = HSR_POLYGON_SHAPE;
			}

			pHSRApp->AppendFace(NULL,pRet);
		}
	}

	return NULL;
}

/**
	@brief	segmentize arc3d.

	@author	humkyung

	@date	2009.09.25

	@param	

	@return
*/
int CArcElement::CreateLineElements()
{
	static POINT_T pts[32] = {'\0',};

	//! clear already created lines. - 2011.04.01 added by humkyung
	{
		for(list<CLineElement*>::iterator itr = m_LineElementEntry.begin();itr != m_LineElementEntry.end();++itr)
		{
			delete (*itr);
		}
		m_LineElementEntry.clear();
	}
	/// up to here

	CArc3d arc3d(m_arc);

	int i = 0;
	arc3d.CreateSegments();
	for(i = 0;i < arc3d.GetNumOfPoints();++i)
	{
		pts[i] = CCurve3d::pt(i);
	}
	
	const int iCount = arc3d.GetNumOfPoints();
	for(i = 0;i < iCount - 1;++i)
	{
		CLineElement* pLineElm = new CLineElement;
		if(pLineElm)
		{
			pLineElm->SetLayerString((*m_pstrLayer));
			pLineElm->m_pt[0] = pts[i];
			pLineElm->m_pt[1] = pts[i + 1];

			/// set point' code - 2012.01.11 added by humkyung
			pLineElm->m_pt[0].reserved = (0 == i) ? PT_PRIMARY : PT_CONSTRUCTION;
			pLineElm->m_pt[1].reserved = ((iCount-2) == i) ? PT_PRIMARY : PT_CONSTRUCTION;
			/// up to here

			pLineElm->SetLayerString((*m_pstrLayer));
			
			m_LineElementEntry.push_back(pLineElm);
		}
	}

	return ERROR_SUCCESS;
}
