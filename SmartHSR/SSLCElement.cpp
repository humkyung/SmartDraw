// SSLCElement.cpp: implementation of the CSSLCElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "SmartHSR.h"
#include "SSLCElement.h"

IMPLEMENTS_HSR_ELEMENT(CSSLCElement , CHSRElement , _T("__SSLC_HSR_ELEMENT__"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CSSLCElement::CSSLCElement()
{
	m_nSegments = 16;
	m_eType = CYLINDER;
}

CSSLCElement::~CSSLCElement()
{

}

#ifdef VER_03
/**	\brief	The CSSLCElement::ParseLine function

	\param	pHSRScanner	a parameter of type CHSRScanner*

	\return	bool	
*/
bool CSSLCElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pGPreFormatScanner is NULL");
	bool bRet=false;
	
	if(pHSRScanner)
	{
		char* pVal=NULL;
		
		if(pVal = pHSRScanner->Val("layer"))
			m_strLayer = pVal;
		else	return false;
		if(pVal = pHSRScanner->Val("radius"))
			m_nRadius = ATOF_T(pVal);
		else	return false;
		
		if(pVal = pHSRScanner->Val("pt1")){
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[0].x),&(m_pt[0].y),&(m_pt[0].z));
		}else	return false;
		if(pVal = pHSRScanner->Val("pt2")){
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[1].x),&(m_pt[1].y),&(m_pt[1].z));
		}else	return false;
		
		if(pVal = pHSRScanner->Val("rotate"))
			m_nRotate = ATOF_T(pVal);
		else	return false;
		if(pVal = pHSRScanner->Val("x1 inc"))
			m_nXInc[0]= ATOF_T(pVal);
		else	return false;
		if(pVal = pHSRScanner->Val("y1 inc"))
			m_nYInc[0]= ATOF_T(pVal);
		else	return false;
		if(pVal = pHSRScanner->Val("x2 inc"))
			m_nXInc[1]= ATOF_T(pVal);
		else	return false;
		if(pVal = pHSRScanner->Val("y2 inc"))
			m_nYInc[1]= ATOF_T(pVal);
		else	return false;
		
		Volume();
		bRet = true;
	}
	
	return bRet;
}
#else
/**	\brief	The CSSLCElement::ParseLine function

	\param	pHSRScanner	a parameter of type CHSRScanner*

	\return	bool	
*/
bool CSSLCElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pGPreFormatScanner is NULL");
	bool bRet=false;
	
	if(pHSRScanner)
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();

		m_nRadius = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
		
		m_pt[0].x = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
		m_pt[0].y = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
		m_pt[0].z = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
		m_pt[1].x = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
		m_pt[1].y = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
		m_pt[1].z = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());
		m_nRotate = ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());
		m_nXInc[0]= ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str());
		m_nYInc[0]= ATOF_T((*pHSRScanner->m_aryFactor)[10].c_str());
		m_nXInc[1]= ATOF_T((*pHSRScanner->m_aryFactor)[11].c_str());
		m_nYInc[1]= ATOF_T((*pHSRScanner->m_aryFactor)[12].c_str());

		Volume();
		bRet = true;
	}

	return bRet;
}
#endif

/**	\brief	The CSSLCElement::Volume function


	\return	void	
*/
void CSSLCElement::Volume()
{
	m_vol.x[0] = m_vol.x[1] = m_pt[0].x;
	m_vol.y[0] = m_vol.y[1] = m_pt[0].y;
	m_vol.z[0] = m_vol.z[1] = m_pt[0].z;
	VECTOR_T vecAxis=
	{	
		m_pt[1].x - m_pt[0].x,
		m_pt[1].y - m_pt[0].y,
		m_pt[1].z - m_pt[0].z
	};
	CMath::NormalizeVector(vecAxis);
		
	double alpha=0.,beta=0.;
	CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
	VECTOR_T vecX={1.,0.,0.},vecY={0.,1.,0.};
	vecX = CGeometry::RotateAboutYXAxis(vecX,-beta,-alpha);
	vecY = CGeometry::RotateAboutYXAxis(vecY,-beta,-alpha);
		
	POINT_T ptEDGE[8]={0.,};
	ptEDGE[0].x = m_pt[0].x - vecX.dx*m_nRadius - vecY.dx*m_nRadius;
	ptEDGE[0].y = m_pt[0].y - vecX.dy*m_nRadius - vecY.dy*m_nRadius;
	ptEDGE[0].z = m_pt[0].z - vecX.dz*m_nRadius - vecY.dz*m_nRadius;
	ptEDGE[1].x = m_pt[0].x - vecX.dx*m_nRadius + vecY.dx*m_nRadius;
	ptEDGE[1].y = m_pt[0].y - vecX.dy*m_nRadius + vecY.dy*m_nRadius;
	ptEDGE[1].z = m_pt[0].z - vecX.dz*m_nRadius + vecY.dz*m_nRadius;
	ptEDGE[2].x = m_pt[0].x + vecX.dx*m_nRadius + vecY.dx*m_nRadius;
	ptEDGE[2].y = m_pt[0].y + vecX.dy*m_nRadius + vecY.dy*m_nRadius;
	ptEDGE[2].z = m_pt[0].z + vecX.dz*m_nRadius + vecY.dz*m_nRadius;
	ptEDGE[3].x = m_pt[0].x + vecX.dx*m_nRadius - vecY.dx*m_nRadius;
	ptEDGE[3].y = m_pt[0].y + vecX.dy*m_nRadius - vecY.dy*m_nRadius;
	ptEDGE[3].z = m_pt[0].z + vecX.dz*m_nRadius - vecY.dz*m_nRadius;
	ptEDGE[4].x = m_pt[1].x - vecX.dx*m_nRadius - vecY.dx*m_nRadius;
	ptEDGE[4].y = m_pt[1].y - vecX.dy*m_nRadius - vecY.dy*m_nRadius;
	ptEDGE[4].z = m_pt[1].z - vecX.dz*m_nRadius - vecY.dz*m_nRadius;
	ptEDGE[5].x = m_pt[1].x - vecX.dx*m_nRadius + vecY.dx*m_nRadius;
	ptEDGE[5].y = m_pt[1].y - vecX.dy*m_nRadius + vecY.dy*m_nRadius;
	ptEDGE[5].z = m_pt[1].z - vecX.dz*m_nRadius + vecY.dz*m_nRadius;
	ptEDGE[6].x = m_pt[1].x + vecX.dx*m_nRadius + vecY.dx*m_nRadius;
	ptEDGE[6].y = m_pt[1].y + vecX.dy*m_nRadius + vecY.dy*m_nRadius;
	ptEDGE[6].z = m_pt[1].z + vecX.dz*m_nRadius + vecY.dz*m_nRadius;
	ptEDGE[7].x = m_pt[1].x + vecX.dx*m_nRadius - vecY.dx*m_nRadius;
	ptEDGE[7].y = m_pt[1].y + vecX.dy*m_nRadius - vecY.dy*m_nRadius;
	ptEDGE[7].z = m_pt[1].z + vecX.dz*m_nRadius - vecY.dz*m_nRadius;
		
	for(int i=0;i < 8;i++)
	{
		if(m_vol.x[0] > ptEDGE[i].x) m_vol.x[0] = ptEDGE[i].x;
		if(m_vol.x[1] < ptEDGE[i].x) m_vol.x[1] = ptEDGE[i].x;
		if(m_vol.y[0] > ptEDGE[i].y) m_vol.y[0] = ptEDGE[i].y;
		if(m_vol.y[1] < ptEDGE[i].y) m_vol.y[1] = ptEDGE[i].y;
		if(m_vol.z[0] > ptEDGE[i].z) m_vol.z[0] = ptEDGE[i].z;
		if(m_vol.z[1] < ptEDGE[i].z) m_vol.z[1] = ptEDGE[i].z;
	}
}

/**	
	@brief	The CSSLCElement::IsInVolume function

	@author	humkyung

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CSSLCElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;
}

/**	\brief	The CSSLCElement::CreateFace function\n
	create 'SSLC' face

	\param	pVIEWFormat	pointer of VIEW
	\param	nID	a parameter of type long&

	\return	PFACE	
*/
CHSRFace* CSSLCElement::CreateFace(CHSRViewFormat* pVIEWFormat,long& nID)
{
	static POINT_T ptEDGE[36];
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	if(pHSRApp)
	{		
		POINT_T pt[2]={0.,};
		pt[0] = pVIEWFormat->MODEL2VIEW(m_pt[0]);
		pt[1] = pVIEWFormat->MODEL2VIEW(m_pt[1]);
		double nRadius=pVIEWFormat->MODEL2VIEW(m_nRadius);
		
		VECTOR_T vec=
		{
			pt[1].x - pt[0].x,
			pt[1].y - pt[0].y,
			pt[1].z - pt[0].z
		};
		VECTOR_T vecZ={0.,0.,1.};
		if(!CMath::NormalizeVector(vec)) return NULL;
		double nDot=CMath::DotProduct(vec,vecZ);
		
		CHSRFace *pSide=NULL,*pTop=NULL,*pBottom=NULL;
		// HORIZONTAL.
		if(fabs(nDot) < 0.001)
		{
			VECTOR_T vecCross={0};
			CMath::GetCrossProduct(vecCross,vec,vecZ);
			if(!CMath::NormalizeVector(vecCross)) return NULL;
			
			const CIsVolume volModel=pVIEWFormat->GetModelVolume();
			// this elemnet's volume is inner model volume.
			if((m_pt[0].z + m_nRadius) < volModel.maxz())
			{
				double tmp[2][2]={0,};

				double nRotate=DEG2RAD(m_nRotate);
				double alpha=0.,beta=0.;
				CGeometry::GetRotatedAngleOfAxis(vec,alpha,beta);

				tmp[0][0] = tmp[1][0] = vecCross.dx;
				tmp[0][1] = tmp[1][1] = vecCross.dy;
				if(fabs(m_nXInc[0]) > 0.0001)
				{
					double nRad=DEG2RAD(m_nXInc[0]);
					if((fabs(m_nRotate - 90) < 1) || (fabs(m_nRotate - 270) < 1.)) nRad = -nRad;
					tmp[0][0] = cos(nRad)*vecCross.dx - sin(nRad)*vecCross.dy;
					tmp[0][1] = sin(nRad)*vecCross.dx + cos(nRad)*vecCross.dy;
				}
				if(fabs(m_nYInc[0]) > 0.0001)
				{
					double nRad=DEG2RAD(m_nYInc[0]);
					if((fabs(m_nRotate - 90) < 1) || (fabs(m_nRotate - 270) < 1.)) nRad = -nRad;
					tmp[0][0] = cos(nRad)*vecCross.dx + sin(nRad)*vecCross.dy;
					tmp[0][1] = -sin(nRad)*vecCross.dx+ cos(nRad)*vecCross.dy;
				}
				if(fabs(m_nXInc[1]) > 0.0001)
				{
					double nRad=DEG2RAD(m_nXInc[1]);
					if((fabs(m_nRotate - 90) < 1.) || (fabs(m_nRotate - 270) < 1.)) nRad = -nRad;
					tmp[1][0] = cos(nRad)*vecCross.dx + sin(nRad)*vecCross.dy;
					tmp[1][1] = -sin(nRad)*vecCross.dx+ cos(nRad)*vecCross.dy;
				}
				if(fabs(m_nYInc[1]) > 0.0001)
				{
					double nRad=DEG2RAD(m_nYInc[1]);
					if((fabs(m_nRotate - 90) < 1.) || (fabs(m_nRotate - 270) < 1.)) nRad = -nRad;
					tmp[1][0] = cos(nRad)*vecCross.dx - sin(nRad)*vecCross.dy;
					tmp[1][1] = sin(nRad)*vecCross.dx+ cos(nRad)*vecCross.dy;
				}

				ptEDGE[0].x = pt[0].x + tmp[1][0]*nRadius;
				ptEDGE[0].y = pt[0].y + tmp[1][1]*nRadius;
				ptEDGE[0].z = (pt[0].z + nRadius) + vecCross.dz*nRadius;
				ptEDGE[1].x = pt[0].x - tmp[1][0]*nRadius;
				ptEDGE[1].y = pt[0].y - tmp[1][1]*nRadius;
				ptEDGE[1].z = (pt[0].z + nRadius) - vecCross.dz*nRadius;
				ptEDGE[2].x = pt[1].x - tmp[0][0]*nRadius;
				ptEDGE[2].y = pt[1].y - tmp[0][1]*nRadius;
				ptEDGE[2].z = (pt[1].z + nRadius) - vecCross.dz*nRadius;
				ptEDGE[3].x = pt[1].x + tmp[0][0]*nRadius;
				ptEDGE[3].y = pt[1].y + tmp[0][1]*nRadius;
				ptEDGE[3].z = (pt[1].z + nRadius) + vecCross.dz*nRadius;
				ptEDGE[4]   = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
#ifdef VER_03
					if("EQP" == m_strLayer)
						pSide->m_enmLayer = LAYER_EQUI;
					else	pSide->m_enmLayer = LAYER_OTHER;
#endif
					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;
					pSide->SetColor(*m_pstrColor);
				}
			}
			else if(m_pt[0].z < volModel.maxz())
			{
				double h=pVIEWFormat->MODEL2VIEW(volModel.maxz() - m_pt[0].z);
				double r=sqrt(nRadius*nRadius - h*h);

				ptEDGE[0].x = pt[0].x + vecCross.dx*r;
				ptEDGE[0].y = pt[0].y + vecCross.dy*r;
				ptEDGE[0].z = volModel.maxz() + vecCross.dz*r;
				ptEDGE[1].x = pt[0].x - vecCross.dx*r;
				ptEDGE[1].y = pt[0].y - vecCross.dy*r;
				ptEDGE[1].z = volModel.maxz() - vecCross.dz*r;
				ptEDGE[2].x = pt[1].x - vecCross.dx*r;
				ptEDGE[2].y = pt[1].y - vecCross.dy*r;
				ptEDGE[2].z = volModel.maxz() - vecCross.dz*r;
				ptEDGE[3].x = pt[1].x + vecCross.dx*r;
				ptEDGE[3].y = pt[1].y + vecCross.dy*r;
				ptEDGE[3].z = volModel.maxz() + vecCross.dz*r;
				ptEDGE[4]   = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
#ifdef VER_03
					if("EQP" == m_strLayer)
						pSide->m_enmLayer = LAYER_EQUI;
					else	pSide->m_enmLayer = LAYER_OTHER;
#endif
					pSide->type  = CYLINDER;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;
					pSide->SetColor(*m_pstrColor);

					pHSRApp->AppendFace(NULL,pSide);
				}

				ptEDGE[0].x = pt[0].x + vecCross.dx*nRadius;
				ptEDGE[0].y = pt[0].y + vecCross.dy*nRadius;
				ptEDGE[0].z = pt[0].z + vecCross.dz*nRadius;
				ptEDGE[1].x = pt[0].x - vecCross.dx*nRadius;
				ptEDGE[1].y = pt[0].y - vecCross.dy*nRadius;
				ptEDGE[1].z = pt[0].z - vecCross.dz*nRadius;
				ptEDGE[2].x = pt[1].x - vecCross.dx*nRadius;
				ptEDGE[2].y = pt[1].y - vecCross.dy*nRadius;
				ptEDGE[2].z = pt[1].z - vecCross.dz*nRadius;
				ptEDGE[3].x = pt[1].x + vecCross.dx*nRadius;
				ptEDGE[3].y = pt[1].y + vecCross.dy*nRadius;
				ptEDGE[3].z = pt[1].z + vecCross.dz*nRadius;
				ptEDGE[4]   = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
#ifdef VER_03
					if("EQP" == m_strLayer)
						pSide->m_enmLayer = LAYER_EQUI;
					else	pSide->m_enmLayer = LAYER_OTHER;
#endif
					pSide->type  = CYLINDER;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;
					pSide->SetColor(*m_pstrColor);
				}
			}
			else
			{
				double h=pVIEWFormat->MODEL2VIEW(m_pt[0].z - volModel.maxz());
				double r=sqrt(nRadius*nRadius - h*h);

				ptEDGE[0].x = pt[0].x + vecCross.dx*r;
				ptEDGE[0].y = pt[0].y + vecCross.dy*r;
				ptEDGE[0].z = volModel.maxz() + vecCross.dz*r;
				ptEDGE[1].x = pt[0].x - vecCross.dx*r;
				ptEDGE[1].y = pt[0].y - vecCross.dy*r;
				ptEDGE[1].z = volModel.maxz() - vecCross.dz*r;
				
				ptEDGE[2].x = pt[1].x - vecCross.dx*r;
				ptEDGE[2].y = pt[1].y - vecCross.dy*r;
				ptEDGE[2].z = volModel.maxz() - vecCross.dz*r;
				ptEDGE[3].x = pt[1].x + vecCross.dx*r;
				ptEDGE[3].y = pt[1].y + vecCross.dy*r;
				ptEDGE[3].z = volModel.maxz() + vecCross.dz*r;
				ptEDGE[4]   = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
					pSide->type  = CYLINDER;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;
					pSide->SetColor(*m_pstrColor);
				}
			}
		// VERTICAL.
		}
		else if(fabs(fabs(nDot) - 1) < 0.001)
		{
			POINT_T ptOrigin=(pt[0].z > pt[1].z) ? pt[0] : pt[1];
			if(pTop=CHSRFace::CreateCircleFace(ptOrigin,nRadius,vec,m_nSegments))
			{
#ifdef VER_03
				if("EQP" == m_strLayer)
					pTop->m_enmLayer = LAYER_EQUI;
				else	pTop->m_enmLayer = LAYER_OTHER;
#endif
				pTop->type  = CYLINDER;
				pTop->id    = nID++;
				pTop->ele   = HSR_CIRCLE_SHAPE;
				pTop->ptCenter = ptOrigin;
				pTop->radius   = nRadius;
				pTop->SetColor(*m_pstrColor);
			}
		// SLOPE.
		}
		else
		{
			VECTOR_T vecCross={0};
			CMath::GetCrossProduct(vecCross,vec,vecZ);
			if(!CMath::NormalizeVector(vecCross)) return NULL;
			
			ptEDGE[0].x = pt[0].x + vecCross.dx*nRadius;
			ptEDGE[0].y = pt[0].y + vecCross.dy*nRadius;
			ptEDGE[0].z = pt[0].z + vecCross.dz*nRadius;
			ptEDGE[1].x = pt[0].x - vecCross.dx*nRadius;
			ptEDGE[1].y = pt[0].y - vecCross.dy*nRadius;
			ptEDGE[1].z = pt[0].z - vecCross.dz*nRadius;
			ptEDGE[2].x = pt[1].x - vecCross.dx*nRadius;
			ptEDGE[2].y = pt[1].y - vecCross.dy*nRadius;
			ptEDGE[2].z = pt[1].z - vecCross.dz*nRadius;
			ptEDGE[3].x = pt[1].x + vecCross.dx*nRadius;
			ptEDGE[3].y = pt[1].y + vecCross.dy*nRadius;
			ptEDGE[3].z = pt[1].z + vecCross.dz*nRadius;
			ptEDGE[4].x = pt[0].x + vecCross.dx*nRadius;
			ptEDGE[4].y = pt[0].y + vecCross.dy*nRadius;
			ptEDGE[4].z = pt[0].z + vecCross.dz*nRadius;
			
			pSide=CHSRFace::CreateFace(5,ptEDGE);
			if(pSide)
			{
#ifdef VER_03
				if("EQP" == m_strLayer)
					pSide->m_enmLayer = LAYER_EQUI;
				else	pSide->m_enmLayer = LAYER_OTHER;
#endif
				pSide->type  = CYLINDER;
				pSide->id    = nID++;
				pSide->ele   = HSR_RECTANGLE_SHAPE;
				pSide->radius= nRadius;
				pSide->SetColor(*m_pstrColor);
			}

			if(pTop=CHSRFace::CreateCircleFace(pt[0],nRadius,vec,m_nSegments))
			{
#ifdef VER_03
				if("EQP" == m_strLayer)
					pTop->m_enmLayer = LAYER_EQUI;
				else	pTop->m_enmLayer = LAYER_OTHER;
#endif
				pTop->type    = CYLINDER;
				pTop->id      = nID++;
				pTop->ele     = HSR_CIRCLE_SHAPE;
				pTop->ptCenter= pt[0];
				pTop->radius  = nRadius;
				pTop->SetColor(*m_pstrColor);
			}
			
			if(pBottom=CHSRFace::CreateCircleFace(pt[1],nRadius,vec,m_nSegments))
			{
#ifdef VER_03
				if("EQP" == m_strLayer)
					pBottom->m_enmLayer = LAYER_EQUI;
				else	pBottom->m_enmLayer = LAYER_OTHER;
#endif
				pBottom->type    = CYLINDER;
				pBottom->id      = nID++;
				pBottom->ele     = HSR_CIRCLE_SHAPE;
				pBottom->ptCenter= pt[1];
				pBottom->radius  = nRadius;
				pBottom->SetColor(*m_pstrColor);
			}
		}
		pRet = pHSRApp->m_clsPreProcessor.Run(pRet,m_eType,pTop,pSide,pBottom);
	}

	return pRet;
}