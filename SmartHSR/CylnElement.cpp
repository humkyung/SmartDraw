// PreCYLNFormat.cpp: implementation of the CCylnElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "CylnElement.h"
#include "SmartHSR.h"

IMPLEMENTS_HSR_ELEMENT(CCylnElement , CHSRElement , _T("__HSR_CYLN_ELEMENT__"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	
	@brief
*/
CCylnElement::CCylnElement(/*HSR_PRIMITIVE_E type*/)
{
	///m_eType = type;
	
	m_nSegments = 20;

	m_nRadius = 0.;
	m_pt[0].x = m_pt[0].y = m_pt[0].z = 0.;
	m_pt[1].x = m_pt[1].y = m_pt[1].z = 0.;

	m_vol.x[0] = m_vol.x[1] = 0.;
	m_vol.y[0] = m_vol.y[1] = 0.;
	m_vol.z[0] = m_vol.z[1] = 0.;
}

CCylnElement::~CCylnElement()
{

}

#ifdef VER_03
/**	\brief	The CCylnElement::ParseLine function

	\param	pHSRScanner	a parameter of type CHSRScanner*

	\return	bool	
*/
bool CCylnElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pGPreFormatScanner is NULL");
	bool bRet=false;
	
	if(pHSRScanner)
	{
		char* pVal=NULL;
		if(0 == STRNCMP_T(pHSRScanner->m_aryFactor[0],"<CYLN>",6))
		{
			m_eType = CYLINDER;
		}
		else if(0 == STRNCMP_T(pHSRScanner->m_aryFactor[0],"<CYLN_EQP>",10))
		{
			m_eType = EQP_CYLINDER;
		}

		if(pVal = pHSRScanner->Val("layer")) (*m_pstrLayer) = pVal;
		if(pVal = pHSRScanner->Val("radius")) m_nRadius = ATOF_T(pVal);
		if(pVal = pHSRScanner->Val("pt1"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[0].x),&(m_pt[0].y),&(m_pt[0].z));
		}
		if(pVal = pHSRScanner->Val("pt2"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[1].x),&(m_pt[1].y),&(m_pt[1].z));
		}
		Volume();
		
		bRet = true;
	}

	return bRet;
}
#else
/**	
	@brief	The CCylnElement::ParseLine function
	@param	pHSRScanner	a parameter of type CHSRScanner*
	@return	bool	
*/
bool CCylnElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pGPreFormatScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();

		if(0 == STRNCMP_T((*pHSRScanner->m_aryFactor)[0].c_str() , _T("CYLN") , 4))
		{
			m_eType = CYLINDER;
		}
		else if(0 == STRNCMP_T((*pHSRScanner->m_aryFactor)[0].c_str(),_T("<CYLN_EQP>"),10))
		{
			m_eType = EQP_CYLINDER;
		}

		if(_T("") == CHSRApp::m_strCurLayer)
		{
			(*m_pstrLayer) = (EQP_CYLINDER == m_eType) ? _T("EQP") : _T("OTHER");
		}
		else
		{
			(*m_pstrLayer) = CHSRApp::m_strCurLayer;
		}
		m_nRadius = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
		
		m_pt[0].x = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
		m_pt[0].y = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
		m_pt[0].z = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
		m_pt[1].x = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
		m_pt[1].y = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
		m_pt[1].z = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());

		if(pHSRScanner->m_nFactor > 8)
		{
			SetColorString((*pHSRScanner->m_aryFactor)[8]);
		}

		Volume();

		bRet = true;
	}

	return bRet;
}
#endif

/**	\brief	The CCylnElement::Volume function


	\return	void	
*/
void CCylnElement::Volume()
{
	m_vol.x[0] = m_vol.x[1] = m_pt[0].x;
	m_vol.y[0] = m_vol.y[1] = m_pt[0].y;
	m_vol.z[0] = m_vol.z[1] = m_pt[0].z;
	VECTOR_T vecAxis=
	{	m_pt[1].x - m_pt[0].x,
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
	@brief	The CCylnElement::IsInVolume function

	@author	humkyung

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CCylnElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;
}

#ifdef VER_03
/**	\brief	create 'CYLN' face

	\author 백흠경

	\param	pVIEWFormat	pointer of VIEW
	\param	nID	a parameter of type long&

	\remarks\n
	;2003.04.16 - check how volume is contained in model volume when located horizontal.
	;2005.01.11 - change the code checking z value and model volume to view volume.

	\return	PFACE	
*/
CHSRFace* CCylnElement::CreateFace(CHSRViewFormat* pViewFormat,long& nID)
{
	static POINT_T ptEDGE[36]={0.,};
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	if(pHSRApp)
	{
		///////////////////////////////
		//--> only top,front but except iso view
		VOLUME_T volModel=pViewFormat->GetModelVolume();
		POINT_T ptMin={volModel.x[0],volModel.y[0],volModel.z[0]};
		POINT_T ptMax={volModel.x[1],volModel.y[1],volModel.z[1]};
		ptMin = pViewFormat->MODEL2VIEW(ptMin);
		ptMax = pViewFormat->MODEL2VIEW(ptMax);
		const double zhigh=(ptMax.z > ptMin.z) ? ptMax.z : ptMin.z;
		const double zlow =(ptMin.z < ptMax.z) ? ptMin.z : ptMax.z;
		//<--
		///////////////////////////////

		POINT_T pt[2]={0.,};
		pt[0] = pViewFormat->MODEL2VIEW(m_pt[0]);
		pt[1] = pViewFormat->MODEL2VIEW(m_pt[1]);
		const double nRadius=pViewFormat->MODEL2VIEW(m_nRadius);
		const VOLUME_T view_volume=pViewFormat->GetViewVolume();
		
		bool   clipping[2] = {false , false};
		double clipping_z[2]={0,};

		VECTOR_T vec=
		{
			pt[1].x - pt[0].x,
			pt[1].y - pt[0].y,
			pt[1].z - pt[0].z
		};
		VECTOR_T vecZ={0.,0.,1.};
		if(!CMath::NormalizeVector(vec)) return pRet;
		ClipAxisByElevation(pt[0] , pt[1] , clipping[0] , clipping[1] , clipping_z[0] , clipping_z[1] , zhigh , zlow);

		double nDot=CMath::DotProduct(vec,vecZ);
		
		CHSRFace *pSide=NULL,*pTop=NULL,*pBottom=NULL;
		/// HORIZONTAL.
		if(fabs(nDot) < 0.001)
		{
			VECTOR_T vecCross={0};
			CMath::GetCrossProduct(vecCross,vec,vecZ);
			if(!CMath::NormalizeVector(vecCross)) return NULL;
			
			const VOLUME_T volModel=pViewFormat->GetModelVolume();
			// volume is contained model volume
			if((pt[0].z + nRadius) < view_volume.z[1])
			{
				ptEDGE[0].x = pt[0].x + vecCross.dx*nRadius;
				ptEDGE[0].y = pt[0].y + vecCross.dy*nRadius;
				ptEDGE[0].z = (pt[0].z + nRadius) + vecCross.dz*nRadius;
				ptEDGE[1].x = pt[0].x - vecCross.dx*nRadius;
				ptEDGE[1].y = pt[0].y - vecCross.dy*nRadius;
				ptEDGE[1].z = (pt[0].z + nRadius) - vecCross.dz*nRadius;
				ptEDGE[2].x = pt[1].x - vecCross.dx*nRadius;
				ptEDGE[2].y = pt[1].y - vecCross.dy*nRadius;
				ptEDGE[2].z = (pt[1].z + nRadius) - vecCross.dz*nRadius;
				ptEDGE[3].x = pt[1].x + vecCross.dx*nRadius;
				ptEDGE[3].y = pt[1].y + vecCross.dy*nRadius;
				ptEDGE[3].z = (pt[1].z + nRadius) + vecCross.dz*nRadius;
				ptEDGE[4]   = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
					if("PIPE" == (*m_pstrLayer))
						pSide->m_enmLayer = LAYER_PIPE_OTHER;
					else	pSide->m_enmLayer = CHSRLayer::GetLayerCode((*m_pstrLayer).c_str());

					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;

					pHSRApp->AppendFace(NULL,pSide);
				}
			}
			else if(pt[0].z < view_volume.z[1])
			{
				double h=pViewFormat->MODEL2VIEW(volModel.z[1] - m_pt[0].z);
				double r=sqrt(nRadius*nRadius - h*h);

				ptEDGE[0].x = pt[0].x + vecCross.dx*r;
				ptEDGE[0].y = pt[0].y + vecCross.dy*r;
				ptEDGE[0].z = volModel.z[1] + vecCross.dz*r;
				ptEDGE[1].x = pt[0].x - vecCross.dx*r;
				ptEDGE[1].y = pt[0].y - vecCross.dy*r;
				ptEDGE[1].z = volModel.z[1] - vecCross.dz*r;
				ptEDGE[2].x = pt[1].x - vecCross.dx*r;
				ptEDGE[2].y = pt[1].y - vecCross.dy*r;
				ptEDGE[2].z = volModel.z[1] - vecCross.dz*r;
				ptEDGE[3].x = pt[1].x + vecCross.dx*r;
				ptEDGE[3].y = pt[1].y + vecCross.dy*r;
				ptEDGE[3].z = volModel.z[1] + vecCross.dz*r;
				ptEDGE[4]   = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
					if("PIPE" == (*m_pstrLayer))
						pSide->m_enmLayer = LAYER_PIPE_OTHER;
					else	pSide->m_enmLayer = CHSRLayer::GetLayerCode((*m_pstrLayer).c_str());

					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;

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
					if("PIPE" == (*m_pstrLayer))
						pSide->m_enmLayer = LAYER_PIPE_OTHER;
					else	pSide->m_enmLayer = CHSRLayer::GetLayerCode((*m_pstrLayer).c_str());

					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;

					pHSRApp->AppendFace(NULL,pSide);
				}
			}
			else
			{
				double h=pViewFormat->MODEL2VIEW(m_pt[0].z - volModel.z[1]);
				double r=sqrt(nRadius*nRadius - h*h);

				ptEDGE[0].x = pt[0].x + vecCross.dx*r;
				ptEDGE[0].y = pt[0].y + vecCross.dy*r;
				ptEDGE[0].z = volModel.z[1] + vecCross.dz*r;
				ptEDGE[1].x = pt[0].x - vecCross.dx*r;
				ptEDGE[1].y = pt[0].y - vecCross.dy*r;
				ptEDGE[1].z = volModel.z[1] - vecCross.dz*r;
				ptEDGE[2].x = pt[1].x - vecCross.dx*r;
				ptEDGE[2].y = pt[1].y - vecCross.dy*r;
				ptEDGE[2].z = volModel.z[1] - vecCross.dz*r;
				ptEDGE[3].x = pt[1].x + vecCross.dx*r;
				ptEDGE[3].y = pt[1].y + vecCross.dy*r;
				ptEDGE[3].z = volModel.z[1] + vecCross.dz*r;
				ptEDGE[4]   = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
					if("PIPE" == (*m_pstrLayer))
						pSide->m_enmLayer = LAYER_PIPE_OTHER;
					else	pSide->m_enmLayer = CHSRLayer::GetLayerCode((*m_pstrLayer).c_str());

					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;

					pHSRApp->AppendFace(NULL,pSide);
				}
			}
		// VERTICAL.
		}
		else if(fabs(fabs(nDot) - 1) < CHSRElement::m_tol)
		{
			POINT_T ptOrigin=(pt[0].z > pt[1].z) ? pt[0] : pt[1];
			if(pTop=CHSRFace::CreateCircleFace(ptOrigin,nRadius,vec,m_nSegments))
			{
				if("PIPE" == (*m_pstrLayer))
					pTop->m_enmLayer = LAYER_PIPE_OTHER;
				else	pTop->m_enmLayer = CHSRLayer::GetLayerCode((*m_pstrLayer).c_str());

				pTop->type  = m_eType;
				pTop->id    = nID++;
				pTop->ele   = HSR_CIRCLE_SHAPE;
				pTop->ptCenter = ptOrigin;
				pTop->radius   = nRadius;

				pHSRApp->AppendFace(NULL,pTop);
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
				if("PIPE" == (*m_pstrLayer))
					pSide->m_enmLayer = LAYER_PIPE_OTHER;
				else	pSide->m_enmLayer = CHSRLayer::GetLayerCode((*m_pstrLayer).c_str());

				pSide->type  = m_eType;
				pSide->id    = nID++;
				pSide->ele   = HSR_RECTANGLE_SHAPE;
				pSide->radius= nRadius;

				pHSRApp->AppendFace(NULL,pSide);
			}

			if(pTop=CHSRFace::CreateCircleFace(pt[0],nRadius,vec,m_nSegments))
			{
				if("PIPE" == (*m_pstrLayer))
					pSide->m_enmLayer = LAYER_PIPE_OTHER;
				else	pSide->m_enmLayer = CHSRLayer::GetLayerCode((*m_pstrLayer).c_str());

				if(true == clipping[0])
				{
					for(PHSR_VERTEX pvt = pTop->pHead; pvt ; pvt = pvt->next)
					{
						const t = (clipping_z[0] - pvt->pt->z) / vec.dz;
						pvt->pt->x += vec.dx*t;
						pvt->pt->y += vec.dy*t;
						pvt->pt->z  = clipping_z[0];
					}
				}
				
				pTop->type    = m_eType;
				pTop->id      = nID++;
				pTop->ele     = HSR_POLYGON_SHAPE;
				pTop->ptCenter= pt[0];
				pTop->radius  = nRadius;

				pHSRApp->AppendFace(NULL,pTop);
			}
			
			if(pBottom=CHSRFace::CreateCircleFace(pt[1],nRadius,vec,m_nSegments))
			{
				if("PIPE" == (*m_pstrLayer))
					pBottom->m_enmLayer = LAYER_PIPE_OTHER;
				else	pBottom->m_enmLayer = CHSRLayer::GetLayerCode((*m_pstrLayer).c_str());

				if(true == clipping[1])
				{
					for(PHSR_VERTEX pvt = pBottom->pHead; pvt ; pvt = pvt->next)
					{
						const t = (clipping_z[1] - pvt->pt->z) / vec.dz;
						pvt->pt->x += vec.dx*t;
						pvt->pt->y += vec.dy*t;
						pvt->pt->z  = clipping_z[1];
					}
				}

				pBottom->type    = m_eType;
				pBottom->id      = nID++;
				pBottom->ele     = HSR_POLYGON_SHAPE;
				pBottom->ptCenter= pt[1];
				pBottom->radius  = nRadius;

				pHSRApp->AppendFace(NULL,pBottom);
			}
		}
	}

	return pRet;
}
#else

/**	@brief	실린더의 면을 생성한다.
	2003.04.16 - check how volume is contained in model volume when located horizontal.
	2005.01.11 - change the code checking z value and model volume to view volume.

	@author 백흠경

	@param	pVIEWFormat	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@return	PFACE	
*/
CHSRFace* CCylnElement::CreateFace(CHSRViewFormat* pViewFormat,long& nID)
{
	static POINT_T ptEDGE[36];
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	if(pHSRApp)
	{
		/// only top,front but except iso view
		CIsVolume volModel=pViewFormat->GetModelVolume();
		POINT_T ptMin={volModel.minx() , volModel.miny() , volModel.minz()};
		POINT_T ptMax={volModel.maxx() , volModel.maxy() , volModel.maxz()};
		ptMin = pViewFormat->MODEL2VIEW(ptMin);
		ptMax = pViewFormat->MODEL2VIEW(ptMax);
		const double zhigh=(ptMax.z > ptMin.z) ? ptMax.z : ptMin.z;
		const double zlow =(ptMin.z < ptMax.z) ? ptMin.z : ptMax.z;
		///

		POINT_T pt[2]={0.,};
		pt[0] = pViewFormat->MODEL2VIEW(m_pt[0]);
		pt[1] = pViewFormat->MODEL2VIEW(m_pt[1]);
		const double nRadius=pViewFormat->MODEL2VIEW(m_nRadius);
		const VOLUME_T view_volume=pViewFormat->GetViewVolume();
		
		bool   clipping[2] = {false , false};
		double clipping_z[2]={0,};
		
		VECTOR_T vec=
		{
			pt[1].x - pt[0].x,
			pt[1].y - pt[0].y,
			pt[1].z - pt[0].z
		};
		VECTOR_T vecZ={0.,0.,1.};
		if(!CMath::NormalizeVector(vec)) return pRet;
		ClipAxisByElevation(pt[0] , pt[1] , clipping[0] , clipping[1] , clipping_z[0] , clipping_z[1] , zhigh , zlow);
		double nDot=CMath::DotProduct(vec,vecZ);
		
		CHSRFace *pSide=NULL,*pTop=NULL,*pBottom=NULL;
		//! HORIZONTAL.
		if(fabs(nDot) < 0.001)
		{
			VECTOR_T vecCross={0};
			CMath::GetCrossProduct(vecCross,vec,vecZ);
			if(!CMath::NormalizeVector(vecCross)) return NULL;
			
			const CIsVolume volModel=pViewFormat->GetModelVolume();
			
			if((pt[0].z + nRadius) < view_volume.z[1])
			{
				//! volume is contained model volume.
				ptEDGE[0].x = nRadius;
				ptEDGE[0].y = 0;
				ptEDGE[0].z = 0;
				ptEDGE[1].x = nRadius * cos(DEG2RAD(45));
				ptEDGE[1].y = 0;
				ptEDGE[1].z = nRadius * sin(DEG2RAD(45));
				ptEDGE[2].x = 0;
				ptEDGE[2].y = 0;
				ptEDGE[2].z = nRadius;
				ptEDGE[3].x = -ptEDGE[1].x;
				ptEDGE[3].y = 0;
				ptEDGE[3].z = ptEDGE[1].z;
				ptEDGE[4].x = -nRadius;
				ptEDGE[4].y = 0;
				ptEDGE[4].z = 0;

				VECTOR_T YAxis   = {0.f , 1.f , 0.f};
				const double nAngle    = CMath::GetAngle(YAxis , vec);
				const VECTOR_T vecNorm = CrossProduct(YAxis , vec);
				double cs = 0.f , sn = 0.f;
				if(vecNorm.dz > 0.f)
				{
					cs = cos(DEG2RAD(nAngle));
					sn = sin(DEG2RAD(nAngle));
				}
				else
				{
					cs = cos(DEG2RAD(-nAngle));
					sn = sin(DEG2RAD(-nAngle));
				}
				
				for(int i = 0;i < 5;++i)
				{
					const double x = ptEDGE[i].x * cs - ptEDGE[i].y * sn;
					const double y = ptEDGE[i].x * sn + ptEDGE[i].y * cs;
					ptEDGE[i].x = x;
					ptEDGE[i].y = y;
				}

				const double dx = pt[1].x - pt[0].x;
				const double dy = pt[1].y - pt[0].y;
				const double dz = pt[1].z - pt[0].z;
				const double length = sqrt(dx * dx + dy * dy + dz * dz);
				for(int i = 0;i < 4;++i)
				{
					POINT_T pts[5];
					pts[0]    = ptEDGE[i];
					pts[0].x += pt[0].x;
					pts[0].y += pt[0].y;
					pts[0].z += pt[0].z;

					pts[1]    = pts[0];
					pts[1].x += vec.dx * length;
					pts[1].y += vec.dy * length;
					pts[1].z += vec.dz * length;

					pts[2].x = pt[0].x + ptEDGE[i + 1].x + vec.dx * length;
					pts[2].y = pt[0].y + ptEDGE[i + 1].y + vec.dy * length;
					pts[2].z = pt[0].z + ptEDGE[i + 1].z + vec.dz * length;

					pts[3] = ptEDGE[i + 1];
					pts[3].x += pt[0].x;
					pts[3].y += pt[0].y;
					pts[3].z += pt[0].z;
					pts[4] = pts[0];

					if(pSide=CHSRFace::CreateFace(5 , pts))
					{
						pSide->m_enmLayer = GetHSRLayer();
						
						pSide->type  = m_eType;
						pSide->id    = nID++;
						pSide->ele   = HSR_RECTANGLE_SHAPE;
						pSide->radius= nRadius;
						pSide->SetColor(*m_pstrColor);

						if(0 == i)
						{
							pSide->pHead->next->next->render = INVALID;
						}
						else if((i > 0) && (i < 3))
						{
							pSide->pHead->render             = INVALID;
							pSide->pHead->next->next->render = INVALID;
						}
						else if(3 == i)
						{
							pSide->pHead->render             = INVALID;
						}

						pHSRApp->AppendFace(NULL , pSide);
					}
				}
				pSide = NULL;
				
				/*
				ptEDGE[0].x = pt[0].x + vecCross.dx*nRadius;
				ptEDGE[0].y = pt[0].y + vecCross.dy*nRadius;
				ptEDGE[0].z = (pt[0].z + nRadius) + vecCross.dz*nRadius;
				ptEDGE[1].x = pt[0].x - vecCross.dx*nRadius;
				ptEDGE[1].y = pt[0].y - vecCross.dy*nRadius;
				ptEDGE[1].z = (pt[0].z + nRadius) - vecCross.dz*nRadius;
				ptEDGE[2].x = pt[1].x - vecCross.dx*nRadius;
				ptEDGE[2].y = pt[1].y - vecCross.dy*nRadius;
				ptEDGE[2].z = (pt[1].z + nRadius) - vecCross.dz*nRadius;
				ptEDGE[3].x = pt[1].x + vecCross.dx*nRadius;
				ptEDGE[3].y = pt[1].y + vecCross.dy*nRadius;
				ptEDGE[3].z = (pt[1].z + nRadius) + vecCross.dz*nRadius;
				ptEDGE[4]   = ptEDGE[0];
				*/

				/*
				ptEDGE[0].x = pt[0].x + vecCross.dx*nRadius;
				ptEDGE[0].y = pt[0].y + vecCross.dy*nRadius;
				ptEDGE[0].z = (pt[0].z) + vecCross.dz*nRadius;
				ptEDGE[1].x = pt[0].x - vecCross.dx*nRadius;
				ptEDGE[1].y = pt[0].y - vecCross.dy*nRadius;
				ptEDGE[1].z = (pt[0].z) - vecCross.dz*nRadius;
				ptEDGE[2].x = pt[1].x - vecCross.dx*nRadius;
				ptEDGE[2].y = pt[1].y - vecCross.dy*nRadius;
				ptEDGE[2].z = (pt[1].z) - vecCross.dz*nRadius;
				ptEDGE[3].x = pt[1].x + vecCross.dx*nRadius;
				ptEDGE[3].y = pt[1].y + vecCross.dy*nRadius;
				ptEDGE[3].z = (pt[1].z) + vecCross.dz*nRadius;
				ptEDGE[4]   = ptEDGE[0];

				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
					if("PIPE" == (*m_pstrLayer))
						pSide->m_enmLayer = LAYER_PIPE_OTHER;
					else	pSide->m_enmLayer = CHSRLayer::GetLayerCode((*m_pstrLayer).c_str());

					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;
				}
				*/
			}
			else if(pt[0].z < view_volume.z[1])
			{
				double h=pViewFormat->MODEL2VIEW(volModel.maxz() - m_pt[0].z);
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
					pSide->m_enmLayer = GetHSRLayer();

					pSide->type  = m_eType;
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
					pSide->m_enmLayer = GetHSRLayer();

					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;
					pSide->SetColor(*m_pstrColor);
				}
			}
			else
			{
				const double h = pViewFormat->MODEL2VIEW(min(fabs(m_pt[0].z - volModel.minz()) , fabs(m_pt[0].z - volModel.maxz())));
				const double r = sqrt(nRadius*nRadius - h*h);

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
					pSide->m_enmLayer = GetHSRLayer();

					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->radius= nRadius;
					pSide->SetColor(*m_pstrColor);
				}
			}
		// VERTICAL.
		}
		else if(fabs(fabs(nDot) - 1) < CHSRElement::m_tol)
		{
			POINT_T ptOrigin=(pt[0].z > pt[1].z) ? pt[0] : pt[1];
			if(pTop=CHSRFace::CreateCircleFace(ptOrigin,nRadius,vec,m_nSegments))
			{
				pTop->m_enmLayer = GetHSRLayer();

				pTop->type  = m_eType;
				pTop->id    = nID++;
				pTop->ele   = HSR_CIRCLE_SHAPE;
				pTop->ptCenter = ptOrigin;
				pTop->radius   = nRadius;
				pTop->SetColor(*m_pstrColor);
			}
		}
		else /// SLOPE.
		{
			/*
			vec.dx = pt[0].x - pt[1].x;
			vec.dy = pt[0].y - pt[1].y;
			vec.dz = pt[0].z - pt[1].z;
			*/

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
				pSide->m_enmLayer = GetHSRLayer();

				pSide->type  = m_eType;
				pSide->id    = nID++;
				pSide->ele   = HSR_RECTANGLE_SHAPE;
				pSide->radius= nRadius;
				pSide->SetColor(*m_pstrColor);
			}

			if(pTop=CHSRFace::CreateCircleFace(pt[0],nRadius,vec,m_nSegments))
			{
				pTop->m_enmLayer = GetHSRLayer();

				if(true == clipping[0])
				{
					for(PHSR_VERTEX pvt = pTop->pHead; pvt ; pvt = pvt->next)
					{
						const double t = (clipping_z[0] - pvt->pt->z) / vec.dz;
						pvt->pt->x += vec.dx*t;
						pvt->pt->y += vec.dy*t;
						pvt->pt->z  = clipping_z[0];
					}
				}

				pTop->type    = m_eType;
				pTop->id      = nID++;
				pTop->ele     = HSR_POLYGON_SHAPE;
				pTop->ptCenter= pt[0];
				pTop->radius  = nRadius;
				pTop->SetColor(*m_pstrColor);
			}
			
			if(pBottom=CHSRFace::CreateCircleFace(pt[1],nRadius,vec,m_nSegments))
			{
				pBottom->m_enmLayer = GetHSRLayer();

				if(true == clipping[1])
				{
					for(PHSR_VERTEX pvt = pBottom->pHead; pvt ; pvt = pvt->next)
					{
						const double t = (clipping_z[1] - pvt->pt->z) / vec.dz;
						pvt->pt->x += vec.dx*t;
						pvt->pt->y += vec.dy*t;
						pvt->pt->z  = clipping_z[1];
					}
				}

				pBottom->type    = m_eType;
				pBottom->id      = nID++;
				pBottom->ele     = HSR_POLYGON_SHAPE;
				pBottom->ptCenter= pt[1];
				pBottom->radius  = nRadius;
				pBottom->SetColor(*m_pstrColor);
			}
		}
		
		pRet = pHSRApp->m_clsPreProcessor.Run(pRet,m_eType,pTop,pSide,pBottom);
	}

	return pRet;
}

#endif

/**
 * ClipAxisByElevation:
 *
 * @param ptStart 
 * @param ptEnd 
 * @param clipStart 
 * @param clipEnd 
 * @param clipStartZ 
 * @param clipEndZ 
 * @param zhigh 
 * @param zlow 
 * @return void 
 */
void CCylnElement::ClipAxisByElevation(POINT_T &ptStart, POINT_T& ptEnd, bool& clipStart , bool& clipEnd , double& clipStartZ , double& clipEndZ , const double& zhigh , const double& zlow)
{
	VECTOR_T vec=
	{
		ptEnd.x - ptStart.x,
		ptEnd.y - ptStart.y,
		ptEnd.z - ptStart.z
	};
	if(!CMath::NormalizeVector(vec) || (0.0 == vec.dx)) return;
	
	if(ptStart.z > zhigh)
	{
		const double t=(zhigh - ptStart.z) / vec.dz;
		ptStart.x = ptStart.x + vec.dx*t;
		ptStart.y = ptStart.y + vec.dy*t;
		ptStart.z = ptStart.z + vec.dz*t;
		
		clipStart  = true;
		clipStartZ = zhigh;
	}
	else if(ptStart.z < zlow)
	{
		const double t=(zlow - ptStart.z) / vec.dz;
		ptStart.x = ptStart.x + vec.dx*t;
		ptStart.y = ptStart.y + vec.dy*t;
		ptStart.z = ptStart.z + vec.dz*t;
		
		clipStart  = true;
		clipStartZ = zlow;
	}
	if(ptEnd.z > zhigh)
	{
		const double t=(zhigh - ptStart.z) / vec.dz;
		ptEnd.x = ptStart.x + vec.dx*t;
		ptEnd.y = ptStart.y + vec.dy*t;
		ptEnd.z = ptStart.z + vec.dz*t;
		
		clipEnd  = true;
		clipEndZ = zhigh;
	}
	else if(ptEnd.z < zlow)
	{
		const double t=(zlow - ptStart.z) / vec.dz;
		ptEnd.x = ptStart.x + vec.dx*t;
		ptEnd.y = ptStart.y + vec.dy*t;
		ptEnd.z = ptStart.z + vec.dz*t;
		
		clipEnd  = true;
		clipEndZ = zlow;
	}
}

/**
	@brief	

	@author	BHK

	@date	2009.09.25
*/
int CCylnElement::Copy(CHSRElement* pCopied)
{
	assert(pCopied && "pCopied is NULL");

	if(pCopied)
	{
		CHSRElement::Copy(pCopied);

		if(pCopied->IsKindOf(CCylnElement::TypeString()))
		{
			CCylnElement* pCyln = static_cast<CCylnElement*>(pCopied);

			m_nSegments = pCyln->m_nSegments;
			m_eType = pCyln->m_eType;
			m_nRadius = pCyln->m_nRadius;
			m_pt[0] = pCyln->m_pt[0];
			m_pt[1] = pCyln->m_pt[1];

			m_vol = pCyln->m_vol;
		}
		
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}