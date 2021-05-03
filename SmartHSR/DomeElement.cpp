// PreDOMEFormat.cpp: implementation of the CDomeElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "DomeElement.h"
#include "SmartHSR.h"

IMPLEMENTS_HSR_ELEMENT(CDomeElement , CHSRElement , _T("DOME"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CDomeElement::CDomeElement()
{
	m_nSegments = 20;
}

CDomeElement::~CDomeElement()
{

}

#ifdef VER_03
/**	\brief	The CDomeElement::ParseLine function

	\param	pHSRScanner	a parameter of type CHSRScanner*

	\return	bool	
*/
bool CDomeElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;
	
	if(pHSRScanner)
	{
		char* pVal=NULL;
		
		if(pVal = pHSRScanner->Val("layer")) (*m_pstrLayer) = pVal;
		if(pVal = pHSRScanner->Val("radius")) m_nRadius = ATOF_T(pVal);
		if(pVal = pHSRScanner->Val("height")) m_nHeight = ATOF_T(pVal);
		
		if(pVal = pHSRScanner->Val("origin pt"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_ptOrigin.x),&(m_ptOrigin.y),&(m_ptOrigin.z));
		}
		
		if(pVal = pHSRScanner->Val("vec"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_vecNorm.dx),&(m_vecNorm.dy),&(m_vecNorm.dz));
		}
		
		bRet = true;
	}
	
	return bRet;
}
#else
/**	\brief	The CDomeElement::ParseLine function

	\param	pHSRScanner	a parameter of type CHSRScanner*

	\return	bool	
*/
bool CDomeElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{		
		m_nRadius = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
		m_nHeight = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
		
		m_ptOrigin.x = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
		m_ptOrigin.y = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
		m_ptOrigin.z = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
		
		m_vecNorm.dx = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
		m_vecNorm.dy = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());
		m_vecNorm.dz = ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());

		if(pHSRScanner->m_nFactor > 9)
		{
			SetColorString((*pHSRScanner->m_aryFactor)[9]);
		}

		(*m_pstrLayer) = CHSRApp::m_strCurLayer;

		bRet = true;
	}

	return bRet;
}
#endif

/**	
	@brief	The CDomeElement::IsInVolume function

	@author	humkyung

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CDomeElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_ptOrigin.x >= volume.minx()) && (m_ptOrigin.x <= volume.maxx()) &&
		(m_ptOrigin.y >= volume.miny()) && (m_ptOrigin.y <= volume.maxy()) &&
		(m_ptOrigin.z >= volume.minz()) && (m_ptOrigin.z <= volume.maxz())) return true;
	
	return false;
}

/**	
	@brief	The CDomeElement::CreateFace function

	@author	humkyung

	@param	pVIEWFormat	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@remarks\n
	;2003.04.16 - check how volume is contained in model volume when located horizontal.

	@return	PFACE	
*/
CHSRFace* CDomeElement::CreateFace(CHSRViewFormat* pVIEWFormat,long& nID)
{
	assert(pVIEWFormat && "pVIEWFormat is NULL");
	static POINT_T ptEDGE[36] = {'\0',};
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	const long body = nID;	/// 2012.01.13 added by humkyung
	if(pVIEWFormat)
	{
		VECTOR_T vecZ={0.,0.,1.};
		CHSRFace *pSide=NULL,*pTop=NULL;
		
		const double  nRadius =pVIEWFormat->MODEL2VIEW(m_nRadius);
		double  nHeight =pVIEWFormat->MODEL2VIEW(m_nHeight);
		POINT_T ptOrigin=pVIEWFormat->MODEL2VIEW(m_ptOrigin);
		VECTOR_T vecNorm=pVIEWFormat->MODEL2VIEW(m_vecNorm);
		if(!Normalize(vecNorm)) return pRet;
		double nDot=CMath::DotProduct(vecNorm,vecZ);
		/// HORIZONTAL.
		if(fabs(nDot) < 0.001)
		{
			double nStartAngle=1.5*PI;;
			double nSweepAngle=PI;
			double nStep=nSweepAngle/m_nSegments;
			double c=cos(nStep);
			double s=sin(nStep);
			
			const CIsVolume volModel=pVIEWFormat->GetModelVolume();
			if((m_ptOrigin.z + m_nRadius) < volModel.maxz())
			{
				double ab=nHeight/nRadius;
				double ba=nRadius/nHeight;
				
				ptEDGE[0].x = nRadius*cos(nStartAngle);
				ptEDGE[0].y = nRadius*sin(nStartAngle);
				ptEDGE[0].z = 0.;
				for(int i = 1;i < m_nSegments + 1;i++)
				{
					ptEDGE[i].x = ptEDGE[i-1].x*c - ab*ptEDGE[i-1].y*s;
					ptEDGE[i].y = ba*ptEDGE[i-1].x*s + ptEDGE[i-1].y*c;
					ptEDGE[i].z = 0.;
				}
				
				double nRad=CGeometry::GetRotatedAngleInXYPlane(vecNorm);
				double c_r=cos(nRad),s_r=sin(nRad);
				int i = 0;
				for(i = 0;i < m_nSegments + 1;i++)
				{
					POINT_T tmp=ptEDGE[i];
					ptEDGE[i].x = tmp.x*c_r - tmp.y*s_r;
					ptEDGE[i].y = tmp.x*s_r + tmp.y*c_r;
					ptEDGE[i].x += ptOrigin.x;
					ptEDGE[i].y += ptOrigin.y;
					ptEDGE[i].z += ptOrigin.z + nRadius*0.5;
				}
				ptEDGE[i] = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(i + 1,ptEDGE))
				{
					pSide->m_enmLayer = GetHSRLayer();

					pSide->type  = DOME;
					pSide->id    = nID++;
					pSide->ele   = HSR_POLYGON_SHAPE;
					pSide->body  = this;	/// 2012.01.13 added by humkyung
					pSide->SetColor(*m_pstrColor);
				}
			}
			else if(m_ptOrigin.z < volModel.maxz())
			{
				///--> upper side
				double h =pVIEWFormat->MODEL2VIEW(volModel.maxz() - m_ptOrigin.z);
				double _r=sqrt(nRadius*nRadius - h*h);
				double _h=sqrt(nHeight*nHeight - h*h);

				double ab=_h/_r;
				double ba=_r/_h;
				
				ptEDGE[0].x = _r*cos(nStartAngle);
				ptEDGE[0].y = _r*sin(nStartAngle);
				ptEDGE[0].z = 0.;
				for(int i = 1;i < m_nSegments + 1;i++)
				{
					ptEDGE[i].x = ptEDGE[i-1].x*c - ab*ptEDGE[i-1].y*s;
					ptEDGE[i].y = ba*ptEDGE[i-1].x*s + ptEDGE[i-1].y*c;
					ptEDGE[i].z = 0.;
				}
				
				double nRad=CGeometry::GetRotatedAngleInXYPlane(vecNorm);
				int i = 0;
				for(i = 0;i < m_nSegments + 1;i++)
				{
					POINT_T tmp=ptEDGE[i];
					ptEDGE[i].x = tmp.x*cos(nRad) - tmp.y*sin(nRad);
					ptEDGE[i].y = tmp.x*sin(nRad) - tmp.y*cos(nRad);
					ptEDGE[i].x += ptOrigin.x;
					ptEDGE[i].y += ptOrigin.y;
					ptEDGE[i].z += ptOrigin.z + nRadius*0.5;
				}
				ptEDGE[i] = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(i + 1,ptEDGE))
				{
					pSide->m_enmLayer = GetHSRLayer();

					pSide->type  = DOME;
					pSide->id    = nID++;
					pSide->ele   = HSR_POLYGON_SHAPE;
					pSide->body  = this;	/// 2012.01.13 added by humkyung
					pSide->SetColor(*m_pstrColor);

					pHSRApp->AppendFace(NULL,pSide);
				}
				///<-- upper side end

				ab=nHeight/nRadius;
				ba=nRadius/nHeight;
				
				ptEDGE[0].x = nRadius*cos(nStartAngle);
				ptEDGE[0].y = nRadius*sin(nStartAngle);
				ptEDGE[0].z = 0.;
				for(int i = 1;i < m_nSegments + 1;i++)
				{
					ptEDGE[i].x = ptEDGE[i-1].x*c - ab*ptEDGE[i-1].y*s;
					ptEDGE[i].y = ba*ptEDGE[i-1].x*s + ptEDGE[i-1].y*c;
					ptEDGE[i].z = 0.;
				}
				
				nRad=CGeometry::GetRotatedAngleInXYPlane(vecNorm);
				for(int i = 0;i < m_nSegments + 1;i++)
				{
					POINT_T tmp=ptEDGE[i];
					ptEDGE[i].x = tmp.x*cos(nRad) - tmp.y*sin(nRad);
					ptEDGE[i].y = tmp.x*sin(nRad) - tmp.y*cos(nRad);
					ptEDGE[i].x += ptOrigin.x;
					ptEDGE[i].y += ptOrigin.y;
					ptEDGE[i].z += ptOrigin.z + nRadius*0.5;
				}
				ptEDGE[i] = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(i + 1,ptEDGE))
				{
					pSide->m_enmLayer = GetHSRLayer();

					pSide->type  = DOME;
					pSide->id    = nID++;
					pSide->ele   = HSR_POLYGON_SHAPE;
					pSide->body  = this;	/// 2012.01.13 added by humkyung
					pSide->SetColor(*m_pstrColor);
				}
			}
			else
			{
				double h =pVIEWFormat->MODEL2VIEW(m_ptOrigin.z - volModel.maxz());
				double _r=sqrt(nRadius*nRadius - h*h);
				double _h=sqrt(nHeight*nHeight - h*h);

				double ab=_h/_r;
				double ba=_r/_h;
				
				ptEDGE[0].x = _r*cos(nStartAngle);
				ptEDGE[0].y = _r*sin(nStartAngle);
				ptEDGE[0].z = 0.;
				for(int i = 1;i < m_nSegments + 1;i++)
				{
					ptEDGE[i].x = ptEDGE[i-1].x*c - ab*ptEDGE[i-1].y*s;
					ptEDGE[i].y = ba*ptEDGE[i-1].x*s + ptEDGE[i-1].y*c;
					ptEDGE[i].z = 0.;
				}
				
				double nRad=CGeometry::GetRotatedAngleInXYPlane(vecNorm);
				int i = 0;
				for(i = 0;i < m_nSegments + 1;i++)
				{
					POINT_T tmp=ptEDGE[i];
					ptEDGE[i].x = tmp.x*cos(nRad) - tmp.y*sin(nRad);
					ptEDGE[i].y = tmp.x*sin(nRad) - tmp.y*cos(nRad);
					ptEDGE[i].x += ptOrigin.x;
					ptEDGE[i].y += ptOrigin.y;
					ptEDGE[i].z += ptOrigin.z + nRadius*0.5;
				}
				ptEDGE[i] = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(i + 1,ptEDGE))
				{
					pSide->m_enmLayer = GetHSRLayer();

					pSide->type  = DOME;
					pSide->id    = nID++;
					pSide->ele   = HSR_POLYGON_SHAPE;
					pSide->body  = this;	/// 2012.01.13 added by humkyung
					pSide->SetColor(*m_pstrColor);
				}
			}
		}
		else
		{ /// OTHERS.
			double nStartAngle=0.;
			double nSweepAngle=2*PI;
			double nStep=nSweepAngle/m_nSegments;
			double c=cos(nStep);
			double s=sin(nStep);
			double ab=1.,ba=1.;
			
			ptEDGE[0].x = nRadius*cos(nStartAngle);
			ptEDGE[0].y = nRadius*sin(nStartAngle);
			ptEDGE[0].z = 0.;
			int i = 0;
			for(i = 1;i < m_nSegments;i++)
			{
				ptEDGE[i].x = ptEDGE[i-1].x*c - ab*ptEDGE[i-1].y*s;
				ptEDGE[i].y = ba*ptEDGE[i-1].x*s + ptEDGE[i-1].y*c;
				ptEDGE[i].z = 0.;
			}
			ptEDGE[i].x = nRadius*cos(nStartAngle + nSweepAngle);
			ptEDGE[i].y = nRadius*sin(nStartAngle + nSweepAngle);
			ptEDGE[i].z = 0.;
			
			double nRad=CGeometry::GetRotatedAngleInXYPlane(vecNorm);
			nRad -= PI*0.5;
			double c_r=cos(nRad),s_r=sin(nRad);
			for(int i = 0;i < m_nSegments + 1;i++)
			{
				POINT_T tmp=ptEDGE[i];
				ptEDGE[i].x = tmp.x*c_r - tmp.y*s_r;
				ptEDGE[i].y = tmp.x*s_r - tmp.y*c_r;
				ptEDGE[i].x += ptOrigin.x;
				ptEDGE[i].y += ptOrigin.y;
				ptEDGE[i].z += ptOrigin.z;
			}

			pTop=CHSRFace::CreateFace(m_nSegments + 1,ptEDGE);
			if(pTop)
			{
				pTop->m_enmLayer = GetHSRLayer();
				pTop->type  = DOME;
				pTop->id    = nID++;
				pTop->ele   = (pTop->IsPerpendicularToViewPoint()) ? HSR_CIRCLE_SHAPE : HSR_POLYGON_SHAPE;
				pTop->ptCenter = ptOrigin;
				pTop->radius   = nRadius;
				pTop->body  = this;	/// 2012.01.13 added by humkyung
				pTop->SetColor(*m_pstrColor);
			}
		}

		pHSRApp->m_clsPreProcessor.Run(NULL,DOME,pTop,pSide,NULL);
	}

	return pRet;
}