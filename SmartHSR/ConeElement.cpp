// PreCONEFormat.cpp: implementation of the CConeElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "SmartHSR.h"
#include "ConeElement.h"

IMPLEMENTS_HSR_ELEMENT(CConeElement , CHSRElement , _T("__HSR_CONE_ELEMENT__"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CConeElement::CConeElement(/*HSR_PRIMITIVE_E type*/)
{
	///m_eType = type;
	m_nSegments = 20;

	m_vol.x[0] = m_vol.x[1] = 0.;
	m_vol.y[0] = m_vol.y[1] = 0.;
	m_vol.z[0] = m_vol.z[1] = 0.;
}

CConeElement::~CConeElement()
{

}

#ifdef VER_03
/**	
	@brief	The CConeElement::ParseLine function

	@author	BHK

	@date	?

	@param	pHSRScanner	a parameter of type CHSRScanner*

	@return	bool	
*/
bool CConeElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pGPreFormatScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		char* pVal=NULL;
		if(0 == strncmp(pHSRScanner->m_aryFactor[0],"<CONE>",6))
		{
			m_eType = CONE;

			if(pVal = pHSRScanner->Val("layer")) (*m_pstrLayer) = pVal;
			if("EQP" == (*m_pstrLayer))
				m_eType = EQP;
			else	m_eType = OTHER;
			if(pVal = pHSRScanner->Val("radius1")) m_nRadius[0] = ATOF_T(pVal);
			if(pVal = pHSRScanner->Val("radius2")) m_nRadius[1] = ATOF_T(pVal);
			
			if(pVal = pHSRScanner->Val("pt1"))
			{
				sscanf(pVal,"%lf,%lf,%lf",&(m_pt[0].x),&(m_pt[0].y),&(m_pt[0].z));
			}
			if(pVal = pHSRScanner->Val("pt2"))
			{
				sscanf(pVal,"%lf,%lf,%lf",&(m_pt[1].x),&(m_pt[1].y),&(m_pt[1].z));
			}
			if(pVal = pHSRScanner->Val("vec"))
			{
				sscanf(pVal,"%lf,%lf,%lf",&(m_vec.dx),&(m_vec.dy),&(m_vec.dz));
			}

			Volume();

			bRet = true;
		}
		else if(0 == strncmp(pHSRScanner->m_aryFactor[0],"<CONE_EQP>",10))
		{
			m_eType = CONE_EQP;

			if(pVal = pHSRScanner->Val("layer")) (*m_pstrLayer) = pVal;
			if("EQP" == (*m_pstrLayer))
				m_eType = EQP;
			else	m_eType = OTHER;
			if(pVal = pHSRScanner->Val("radius1")) m_nRadius[0] = ATOF_T(pVal);
			if(pVal = pHSRScanner->Val("radius2")) m_nRadius[1] = ATOF_T(pVal);
			
			if(pVal = pHSRScanner->Val("pt1"))
			{
				sscanf(pVal,"%lf,%lf,%lf",&(m_pt[0].x),&(m_pt[0].y),&(m_pt[0].z));
			}
			if(pVal = pHSRScanner->Val("pt2"))
			{
				sscanf(pVal,"%lf,%lf,%lf",&(m_pt[1].x),&(m_pt[1].y),&(m_pt[1].z));
			}
			if(pVal = pHSRScanner->Val("vec"))
			{
				sscanf(pVal,"%lf,%lf,%lf",&(m_vec.dx),&(m_vec.dy),&(m_vec.dz));
			}
			
			Volume();
			
			bRet = true;
		}
	}

	return bRet;
}
#else
/**	
	@brief	The CConeElement::ParseLine function
	@param	pHSRScanner	a parameter of type CHSRScanner*
	@return	void	
*/
bool CConeElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pGPreFormatScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		if(0 == STRNCMP_T((*pHSRScanner->m_aryFactor)[0].c_str() , _T("CONE") , 4))
		{
			if(_T("") == CHSRApp::m_strCurLayer)
			{
				(*m_pstrLayer) = _T("OTHER");
			}else	(*m_pstrLayer) = CHSRApp::m_strCurLayer;

			m_nRadius[0] = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
			m_nRadius[1] = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
			
			m_pt[0].x = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
			m_pt[0].y = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
			m_pt[0].z = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
			m_pt[1].x = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
			m_pt[1].y = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());
			m_pt[1].z = ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());
			
			m_vec.dx = m_vec.dy = m_vec.dz = 0.;
			if(pHSRScanner->m_nFactor > 9)
			{
				m_vec.dx = ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str());
				m_vec.dy = ATOF_T((*pHSRScanner->m_aryFactor)[10].c_str());
				m_vec.dz = ATOF_T((*pHSRScanner->m_aryFactor)[11].c_str());
			}

			if(pHSRScanner->m_nFactor > 12)
			{
				SetColorString((*pHSRScanner->m_aryFactor)[12]);
			}

			Volume();
			bRet = true;
		}
		else if(0 == STRNCMP_T((*pHSRScanner->m_aryFactor)[0].c_str(),_T("<CONE_EQP>"),10))
		{
			if(_T("") == CHSRApp::m_strCurLayer)
			{
				(*m_pstrLayer) = _T("EQP");
			}else	(*m_pstrLayer) = CHSRApp::m_strCurLayer;
			//(*m_pstrLayer) = CHSRApp::m_strCurLayer;
			//if("EQP" == (*m_pstrLayer))
			//	m_eType = EQP;
			//else	m_eType = OTHER;

			m_nRadius[0] = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
			m_nRadius[1] = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
			
			m_pt[0].x = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
			m_pt[0].y = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
			m_pt[0].z = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
			m_pt[1].x = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
			m_pt[1].y = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());
			m_pt[1].z = ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());
			
			if(pHSRScanner->m_nFactor > 9)
			{
				m_vec.dx = ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str());
				m_vec.dy = ATOF_T((*pHSRScanner->m_aryFactor)[10].c_str());
				m_vec.dz = ATOF_T((*pHSRScanner->m_aryFactor)[11].c_str());
			}
			else
			{
				m_vec.dx = m_vec.dy = m_vec.dz = 0.;
			}

			Volume();
			bRet = true;
		}
	}
	
	return bRet;
}
#endif

/**	\brief	The CConeElement::Volume function


	\return	void	
*/
void CConeElement::Volume()
{
	POINT_T pt[2]={0,};
	pt[0] = m_pt[0];pt[1] = m_pt[1];
	//pt[1].x += m_vec.dx;pt[1].y += m_vec.dy;pt[1].z += m_vec.dz;

	VECTOR_T vecAxis=
	{	pt[1].x - pt[0].x,
		pt[1].y - pt[0].y,
		pt[1].z - pt[0].z
	};
	CMath::NormalizeVector(vecAxis);
		
	double alpha=0.,beta=0.;
	CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
	VECTOR_T vecX={1.,0.,0.},vecY={0.,1.,0.};
	vecX = CGeometry::RotateAboutYXAxis(vecX,-beta,-alpha);
	vecY = CGeometry::RotateAboutYXAxis(vecY,-beta,-alpha);
		
	POINT_T ptEDGE[8]={0.,};
	ptEDGE[0].x = m_pt[0].x - vecX.dx*m_nRadius[0] - vecY.dx*m_nRadius[0];
	ptEDGE[0].y = m_pt[0].y - vecX.dy*m_nRadius[0] - vecY.dy*m_nRadius[0];
	ptEDGE[0].z = m_pt[0].z - vecX.dz*m_nRadius[0] - vecY.dz*m_nRadius[0];
	ptEDGE[1].x = m_pt[0].x - vecX.dx*m_nRadius[0] + vecY.dx*m_nRadius[0];
	ptEDGE[1].y = m_pt[0].y - vecX.dy*m_nRadius[0] + vecY.dy*m_nRadius[0];
	ptEDGE[1].z = m_pt[0].z - vecX.dz*m_nRadius[0] + vecY.dz*m_nRadius[0];
	ptEDGE[2].x = m_pt[0].x + vecX.dx*m_nRadius[0] + vecY.dx*m_nRadius[0];
	ptEDGE[2].y = m_pt[0].y + vecX.dy*m_nRadius[0] + vecY.dy*m_nRadius[0];
	ptEDGE[2].z = m_pt[0].z + vecX.dz*m_nRadius[0] + vecY.dz*m_nRadius[0];
	ptEDGE[3].x = m_pt[0].x + vecX.dx*m_nRadius[0] - vecY.dx*m_nRadius[0];
	ptEDGE[3].y = m_pt[0].y + vecX.dy*m_nRadius[0] - vecY.dy*m_nRadius[0];
	ptEDGE[3].z = m_pt[0].z + vecX.dz*m_nRadius[0] - vecY.dz*m_nRadius[0];
	ptEDGE[4].x = m_pt[1].x - vecX.dx*m_nRadius[1] - vecY.dx*m_nRadius[1];
	ptEDGE[4].y = m_pt[1].y - vecX.dy*m_nRadius[1] - vecY.dy*m_nRadius[1];
	ptEDGE[4].z = m_pt[1].z - vecX.dz*m_nRadius[1] - vecY.dz*m_nRadius[1];
	ptEDGE[5].x = m_pt[1].x - vecX.dx*m_nRadius[1] + vecY.dx*m_nRadius[1];
	ptEDGE[5].y = m_pt[1].y - vecX.dy*m_nRadius[1] + vecY.dy*m_nRadius[1];
	ptEDGE[5].z = m_pt[1].z - vecX.dz*m_nRadius[1] + vecY.dz*m_nRadius[1];
	ptEDGE[6].x = m_pt[1].x + vecX.dx*m_nRadius[1] + vecY.dx*m_nRadius[1];
	ptEDGE[6].y = m_pt[1].y + vecX.dy*m_nRadius[1] + vecY.dy*m_nRadius[1];
	ptEDGE[6].z = m_pt[1].z + vecX.dz*m_nRadius[1] + vecY.dz*m_nRadius[1];
	ptEDGE[7].x = m_pt[1].x + vecX.dx*m_nRadius[1] - vecY.dx*m_nRadius[1];
	ptEDGE[7].y = m_pt[1].y + vecX.dy*m_nRadius[1] - vecY.dy*m_nRadius[1];
	ptEDGE[7].z = m_pt[1].z + vecX.dz*m_nRadius[1] - vecY.dz*m_nRadius[1];
	
	m_vol.x[0] = m_vol.x[1] = ptEDGE[0].x;
	m_vol.y[0] = m_vol.y[1] = ptEDGE[0].y;
	m_vol.z[0] = m_vol.z[1] = ptEDGE[0].z;
	for(int i=1;i < 8;i++)
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
	@brief	The CConeElement::IsInVolume function

	@author	humkyung

	@param	volume	a parameter of type const SQVOLUME

	@return	bool	
*/
bool CConeElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;
}

/**	
	@brief	The CConeElement::CreateFace function

	@author	HumKyung.BAEK

	@param	pViewFormat	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@return	PFACE	
*/
CHSRFace* CConeElement::CreateFace(CHSRViewFormat* pViewFormat,long& nID)
{
	assert(pViewFormat && "pViewFormat is NULL");
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pViewFormat)
	{
		///////////////////////////////
		//--> only top,front but except iso view
		CIsVolume volModel=pViewFormat->GetModelVolume();
		POINT_T ptMin={volModel.minx() , volModel.miny() , volModel.minz()};
		POINT_T ptMax={volModel.maxx() , volModel.maxy() , volModel.maxz()};
		ptMin = pViewFormat->MODEL2VIEW(ptMin);
		ptMax = pViewFormat->MODEL2VIEW(ptMax);
		const double zhigh=(ptMax.z > ptMin.z) ? ptMax.z : ptMin.z;
		const double zlow =(ptMin.z < ptMax.z) ? ptMin.z : ptMax.z;
		//<--
		///////////////////////////////

		double nTol=0.01;
		CHSRFace *pSide=NULL,*pTop=NULL,*pBottom=NULL;

		VECTOR_T vec[3]={0.,} , vecZ={0.,0.,1.};
		POINT_T pt[2]={0,};
		pt[0] = pViewFormat->MODEL2VIEW(m_pt[0]);
		pt[1] = pViewFormat->MODEL2VIEW(m_pt[1]);

		double nRadius[2]={0.,};
		nRadius[0]=pViewFormat->MODEL2VIEW(m_nRadius[0]);
		nRadius[1]=pViewFormat->MODEL2VIEW(m_nRadius[1]);

		ClipAxisByElevation(pt[0] , pt[1] , nRadius , zhigh , zlow);
		
		//--> 2003.01.02
		CVect vecNorm; // normal vector of top,bottom face
		vecNorm.dx=pt[1].x - pt[0].x;
		vecNorm.dy=pt[1].y - pt[0].y;
		vecNorm.dz=pt[1].z - pt[0].z;
		if(!CMath::NormalizeVector(vecNorm)) return NULL;
		
		// get center of top face
		vec[0]= pViewFormat->MODEL2VIEW(m_vec);
		pt[1].x += vec[0].dx; pt[1].y += vec[0].dy; pt[1].z += vec[0].dz;
		//<--
		
		double nDot=CMath::DotProduct(vecNorm,vecZ);
		if(fabs(fabs(nDot) - 1) < 0.01)	// stand up
		{
			nDot=CMath::DotProduct(vecNorm,vecZ);
			if(fabs(nDot) > nTol)
			{
				pRet = CreateVerticalFace(pViewFormat,vecNorm,pt,nRadius,nID);
				return pRet;
			}
		}
		else if(fabs(nDot) < 0.01) // place in X-Y plane
		{
			VECTOR_T vecCross={0};
			CMath::GetCrossProduct(vecCross,vecNorm,vecZ);
			CMath::NormalizeVector(vecCross);
			
			if(nRadius[0] < 0.001)
			{
				m_ptEDGE[0]   = pt[0];
				m_ptEDGE[0].z = (pt[0].z + nRadius[1]);
				m_ptEDGE[1].x = pt[1].x - vecCross.dx*nRadius[1];
				m_ptEDGE[1].y = pt[1].y - vecCross.dy*nRadius[1];
				m_ptEDGE[1].z = (pt[1].z + nRadius[1]) - vecCross.dz*nRadius[1];
				m_ptEDGE[2].x = pt[1].x + vecCross.dx*nRadius[1];
				m_ptEDGE[2].y = pt[1].y + vecCross.dy*nRadius[1];
				m_ptEDGE[2].z = (pt[1].z + nRadius[1]) + vecCross.dz*nRadius[1];
				m_ptEDGE[3]   = m_ptEDGE[0];
				if(pSide=CHSRFace::CreateFace(4,m_ptEDGE))
				{
					pSide->m_enmLayer = GetHSRLayer();

					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->SetColor(*m_pstrColor);
				}
			}
			else if(nRadius[1] < 0.001)
			{
				m_ptEDGE[0].x = pt[0].x + vecCross.dx*nRadius[0];
				m_ptEDGE[0].y = pt[0].y + vecCross.dy*nRadius[0];
				m_ptEDGE[0].z = (pt[0].z + nRadius[0]) + vecCross.dz*nRadius[0];
				m_ptEDGE[1].x = pt[0].x - vecCross.dx*nRadius[0];
				m_ptEDGE[1].y = pt[0].y - vecCross.dy*nRadius[0];
				m_ptEDGE[1].z = (pt[0].z + nRadius[0]) - vecCross.dz*nRadius[0];
				m_ptEDGE[2]   = pt[1];
				m_ptEDGE[2].z = (pt[1].z + nRadius[0]);
				m_ptEDGE[3]   = m_ptEDGE[0];
				if(pSide=CHSRFace::CreateFace(4,m_ptEDGE))
				{
					pSide->m_enmLayer = GetHSRLayer();

					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->SetColor(*m_pstrColor);
				}
			}
			else
			{
				double zhigh=((pt[0].z + nRadius[0]) > (pt[1].z + nRadius[1])) ? (pt[0].z + nRadius[0]) : (pt[1].z + nRadius[1]);

				m_ptEDGE[0].x = pt[0].x + vecCross.dx*nRadius[0];
				m_ptEDGE[0].y = pt[0].y + vecCross.dy*nRadius[0];
				m_ptEDGE[0].z = zhigh;
				m_ptEDGE[1].x = pt[0].x - vecCross.dx*nRadius[0];
				m_ptEDGE[1].y = pt[0].y - vecCross.dy*nRadius[0];
				m_ptEDGE[1].z = zhigh;
				m_ptEDGE[2].x = pt[1].x - vecCross.dx*nRadius[1];
				m_ptEDGE[2].y = pt[1].y - vecCross.dy*nRadius[1];
				m_ptEDGE[2].z = zhigh;
				m_ptEDGE[3].x = pt[1].x + vecCross.dx*nRadius[1];
				m_ptEDGE[3].y = pt[1].y + vecCross.dy*nRadius[1];
				m_ptEDGE[3].z = zhigh;
				m_ptEDGE[4]   = m_ptEDGE[0];
				if(pSide=CHSRFace::CreateFace(5,m_ptEDGE))
				{
					pSide->m_enmLayer = GetHSRLayer();

					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->SetColor(*m_pstrColor);
				}
			}
		}
		else
		{
			nDot=CMath::DotProduct(vecNorm,vecZ);
			if(fabs(nDot) > nTol)
			{
				if((nRadius[0] > 0.001) && (pTop = CHSRFace::CreateCircleFace(pt[0],nRadius[0],vecNorm,m_nSegments)))
				{
					pTop->m_enmLayer = GetHSRLayer();

					pTop->type     = m_eType;
					pTop->id       = nID++;
					pTop->ele      = HSR_POLYGON_SHAPE;
					pTop->ptCenter = pt[0]; 
					pTop->radius   = nRadius[0];
					pTop->SetColor(*m_pstrColor);
				}
				
				if((nRadius[1] > 0.001) && (pBottom = CHSRFace::CreateCircleFace(pt[1],nRadius[1],vecNorm,m_nSegments)))
				{
					pBottom->m_enmLayer = GetHSRLayer();

					pBottom->type     = m_eType;
					pBottom->id       = nID++;
					pBottom->ele      = HSR_POLYGON_SHAPE;
					pBottom->ptCenter = pt[1]; 
					pBottom->radius   = nRadius[1];
					pBottom->SetColor(*m_pstrColor);
				}
			}
			
			VECTOR_T vecCross={0};
			CMath::GetCrossProduct(vecCross,vecNorm,vecZ);
			if(!CMath::NormalizeVector(vecCross)) return pRet;
			
			if(nRadius[0] < 0.001)
			{
				m_ptEDGE[0]   = pt[0];
				m_ptEDGE[1].x = pt[1].x - vecCross.dx*nRadius[1];
				m_ptEDGE[1].y = pt[1].y - vecCross.dy*nRadius[1];
				m_ptEDGE[1].z = pt[1].z - vecCross.dz*nRadius[1];
				m_ptEDGE[2].x = pt[1].x + vecCross.dx*nRadius[1];
				m_ptEDGE[2].y = pt[1].y + vecCross.dy*nRadius[1];
				m_ptEDGE[2].z = pt[1].z + vecCross.dz*nRadius[1];
				m_ptEDGE[3]   = m_ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(4,m_ptEDGE))
				{
					pSide->m_enmLayer = GetHSRLayer();

					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->SetColor(*m_pstrColor);
				}
			}
			else if(nRadius[1] < 0.001)
			{
				m_ptEDGE[0].x = pt[0].x + vecCross.dx*nRadius[0];
				m_ptEDGE[0].y = pt[0].y + vecCross.dy*nRadius[0];
				m_ptEDGE[0].z = pt[0].z + vecCross.dz*nRadius[0];
				m_ptEDGE[1].x = pt[0].x - vecCross.dx*nRadius[0];
				m_ptEDGE[1].y = pt[0].y - vecCross.dy*nRadius[0];
				m_ptEDGE[1].z = pt[0].z - vecCross.dz*nRadius[0];
				m_ptEDGE[2]   = pt[1];
				m_ptEDGE[3]   = m_ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(4,m_ptEDGE))
				{
					pSide->m_enmLayer = GetHSRLayer();

					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->SetColor(*m_pstrColor);
				}
			}
			else
			{
				m_ptEDGE[0].x = pt[0].x + vecCross.dx*nRadius[0];
				m_ptEDGE[0].y = pt[0].y + vecCross.dy*nRadius[0];
				m_ptEDGE[0].z = pt[0].z + vecCross.dz*nRadius[0];
				m_ptEDGE[1].x = pt[0].x - vecCross.dx*nRadius[0];
				m_ptEDGE[1].y = pt[0].y - vecCross.dy*nRadius[0];
				m_ptEDGE[1].z = pt[0].z - vecCross.dz*nRadius[0];
				m_ptEDGE[2].x = pt[1].x - vecCross.dx*nRadius[1];
				m_ptEDGE[2].y = pt[1].y - vecCross.dy*nRadius[1];
				m_ptEDGE[2].z = pt[1].z - vecCross.dz*nRadius[1];
				m_ptEDGE[3].x = pt[1].x + vecCross.dx*nRadius[1];
				m_ptEDGE[3].y = pt[1].y + vecCross.dy*nRadius[1];
				m_ptEDGE[3].z = pt[1].z + vecCross.dz*nRadius[1];
				m_ptEDGE[4].x = pt[0].x + vecCross.dx*nRadius[0];
				m_ptEDGE[4].y = pt[0].y + vecCross.dy*nRadius[0];
				m_ptEDGE[4].z = pt[0].z + vecCross.dz*nRadius[0];
				
				if(pSide=CHSRFace::CreateFace(5,m_ptEDGE))
				{
					pSide->m_enmLayer = GetHSRLayer();

					pSide->type  = m_eType;
					pSide->id    = nID++;
					pSide->ele   = HSR_RECTANGLE_SHAPE;
					pSide->SetColor(*m_pstrColor);
				}
			}
		}

		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		if(pHSRApp)
			pRet = pHSRApp->m_clsPreProcessor.Run(pRet,CONE,pTop,pSide,pBottom);
	}

	return pRet;
}

/**
 * CreateTopFace:
 *
 * @return CHSRFace* 
 */
CHSRFace* CConeElement::CreateVerticalFace(CHSRViewFormat* pViewFormat,const CVect& vecNorm,
				      const POINT_T ptRef[2],const double nRadiusRef[2],
				      long& nID)
{
	CHSRFace* pRet=NULL;
	CHSRFace *pSide=NULL,*pTop=NULL,*pBottom=NULL;

	const double nTol=0.01;
	POINT_T pt[2];
	double nRadius[2];

	pt[0] = ptRef[0]		; pt[1] = ptRef[1];
	nRadius[0] = nRadiusRef[0]	; nRadius[1] = nRadiusRef[1];

	VECTOR_T vecDir;
	vecDir.dx=(pt[1].x - pt[0].x);
	vecDir.dy=(pt[1].y - pt[0].y);
	vecDir.dz=(pt[1].z - pt[0].z);
	
	const double length = sqrt(vecDir.dx*vecDir.dx + vecDir.dy*vecDir.dy + vecDir.dz*vecDir.dz);
	vecDir.dx /= length; vecDir.dy /= length; vecDir.dz /= length;
	
	if(pt[0].z > pt[1].z)
	{
		swap(pt[0],pt[1]);
		swap(nRadius[0],nRadius[1]);
		//vecDir.dx = -vecDir.dx;
		//vecDir.dy = -vecDir.dy;
		//vecDir.dz = -vecDir.dz;
	}
	/*
	const double ratio = (nRadius[1] - nRadius[0]) / length;

	VOLUME_T viewVolume = pViewFormat->GetViewVolume();
	if(pt[1].z > viewVolume.z[HIGH])
	{
		const double t=(viewVolume.z[HIGH] - pt[1].z)/vecDir.dz;

		nRadius[1] += ratio*(viewVolume.z[HIGH] - pt[1].z);
		pt[1].x += t*vecDir.dx;
		pt[1].y += t*vecDir.dy;
		pt[1].z = viewVolume.z[HIGH];
	}

	if(pt[0].z < viewVolume.z[LOW])
	{
		const double t=(viewVolume.z[LOW] - pt[0].z)/vecDir.dz;

		nRadius[0] += ratio*(viewVolume.z[LOW] - pt[0].z);
		pt[0].x += t*vecDir.dx;
		pt[0].y += t*vecDir.dy;
		pt[0].z = viewVolume.z[LOW];
	}
	*/

	if((pt[0].z > pt[1].z) && (nRadius[0] > nRadius[1]))
	{
		if(pTop = CHSRFace::CreateCircleFace(pt[0],nRadius[0],vecNorm,m_nSegments))
		{
			pTop->m_enmLayer = GetHSRLayer();
			
			pTop->type     = m_eType;
			pTop->id       = nID++;
			pTop->ele      = HSR_CIRCLE_SHAPE;
			pTop->ptCenter = pt[0]; 
			pTop->radius   = nRadius[0];
			pTop->SetColor(*m_pstrColor);
		}
	}
	else if((pt[1].z > pt[0].z) && (nRadius[1] > nRadius[0]))
	{
		if(pTop = CHSRFace::CreateCircleFace(pt[1],nRadius[1],vecNorm,m_nSegments))
		{
			pTop->m_enmLayer = GetHSRLayer();
			
			pTop->type     = m_eType;
			pTop->id       = nID++;
			pTop->ele      = HSR_CIRCLE_SHAPE;
			pTop->ptCenter = pt[1]; 
			pTop->radius= nRadius[1];
			pTop->SetColor(*m_pstrColor);
		}			
	}
	else
	{
		if((m_nRadius[0] > 0.) && (pTop = CHSRFace::CreateCircleFace(pt[0],nRadius[0],vecNorm,m_nSegments)))
		{
			pTop->m_enmLayer = GetHSRLayer();
			
			pTop->type     = m_eType;
			pTop->id       = nID++;
			pTop->ele      = HSR_CIRCLE_SHAPE;
			pTop->ptCenter = pt[0]; 
			pTop->radius   = nRadius[0];
			pTop->SetColor(*m_pstrColor);
		}
		
		if((m_nRadius[1] > 0.) && (pBottom = CHSRFace::CreateCircleFace(pt[1],nRadius[1],vecNorm,m_nSegments)))
		{
			pBottom->m_enmLayer = GetHSRLayer();
			
			pBottom->type  = m_eType;
			pBottom->id    = nID++;
			pBottom->ele   = HSR_CIRCLE_SHAPE;
			pBottom->ptCenter = pt[1]; 
			pBottom->radius= nRadius[1];
			pBottom->SetColor(*m_pstrColor);
		}
	}

	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	if(pHSRApp)
		pRet = pHSRApp->m_clsPreProcessor.Run(pRet,CONE,pTop,pSide,pBottom);

	return pRet;
}

/**
 * ClipAxisByElevation:
 *
 * @param ptStart 
 * @param ptEnd 
 * @param zhigh 
 * @param zlow 
 * @return void 
 */
void CConeElement::ClipAxisByElevation(POINT_T &ptStart, POINT_T &ptEnd , double* nRadius , const double &zhigh, const double &zlow)
{
	VECTOR_T vec=
	{
		ptEnd.x - ptStart.x,
		ptEnd.y - ptStart.y,
		ptEnd.z - ptStart.z
	};

	const double length = sqrt(vec.dx*vec.dx + vec.dy*vec.dy + vec.dz*vec.dz);
	const double ratio = (nRadius[1] - nRadius[0]) / length;
	if(!CMath::NormalizeVector(vec)) return;
	
	//--> reposition the pipe point for clipping
	if(ptStart.z > zhigh)
	{
		const double t=(zhigh - ptStart.z) / vec.dz;

		nRadius[0] += ratio*(zhigh - ptStart.z);
		ptStart.x = ptStart.x + vec.dx*t;
		ptStart.y = ptStart.y + vec.dy*t;
		ptStart.z = ptStart.z + vec.dz*t;
	}
	else if(ptStart.z < zlow)
	{
		const double t=(zlow - ptStart.z) / vec.dz;
		
		nRadius[0] += ratio*(zhigh - ptStart.z);
		ptStart.x = ptStart.x + vec.dx*t;
		ptStart.y = ptStart.y + vec.dy*t;
		ptStart.z = ptStart.z + vec.dz*t;
	}
	if(ptEnd.z > zhigh)
	{
		const double t=(zhigh - ptStart.z) / vec.dz;

		nRadius[1] = nRadius[0] + ratio*(zhigh - ptStart.z);
		ptEnd.x = ptStart.x + vec.dx*t;
		ptEnd.y = ptStart.y + vec.dy*t;
		ptEnd.z = ptStart.z + vec.dz*t;
	}
	else if(ptEnd.z < zlow)
	{
		const double t=(zlow - ptStart.z) / vec.dz;

		nRadius[1] = nRadius[0] + ratio*(zlow - ptStart.z);
		ptEnd.x = ptStart.x + vec.dx*t;
		ptEnd.y = ptStart.y + vec.dy*t;
		ptEnd.z = ptStart.z + vec.dz*t;
	}
}
