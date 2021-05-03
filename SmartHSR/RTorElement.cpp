// PreRTORFormat.cpp: implementation of the CRTorElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "RTorElement.h"
#include "SmartHSR.h"

IMPLEMENTS_HSR_ELEMENT(CRTorElement , CHSRElement , _T("RTOR"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRTorElement::CRTorElement()
{
	m_nSegments = 16;
}

CRTorElement::~CRTorElement()
{

}

#ifdef VER_03
/**	\brief	The CRTorElement::ParseLine function

	\param	pPreFormatScanner	a parameter of type CHSRScanner*

	\return	bool	
*/
bool CRTorElement::ParseLine(CHSRScanner* pPreFormatScanner){
	assert(pPreFormatScanner && "pPreFormatParse is NULL");
	bool bRet=false;

	if(pPreFormatScanner){
		char* pVal=NULL;

		if(pVal = pPreFormatScanner->Val("layer")) (*m_pstrLayer) = pVal;
		if(pVal = pPreFormatScanner->Val("pt1")){
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[0].x),&(m_pt[0].y),&(m_pt[0].z));
		}
		if(pVal = pPreFormatScanner->Val("pt2")){
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[1].x),&(m_pt[1].y),&(m_pt[1].z));
		}
		
		if(pVal = pPreFormatScanner->Val("origin pt")){
			sscanf(pVal,"%lf,%lf,%lf",&(m_ptOrigin.x),&(m_ptOrigin.y),&(m_ptOrigin.z));
		}

		if(pVal = pPreFormatScanner->Val("vec")){
			sscanf(pVal,"%lf,%lf,%lf",&(m_vecNorm.dx),&(m_vecNorm.dy),&(m_vecNorm.dz));
		}
		
		if(pVal = pPreFormatScanner->Val("width")) m_nWidth = ATOF_T(pVal);
		if(pVal = pPreFormatScanner->Val("height")) m_nHeight = ATOF_T(pVal);

		bRet = true;
	}

	return bRet;
}
#else
/**	
	@brief	The CRTorElement::ParseLine function

	@author	humkyung

	@param	pPreFormatScanner	a parameter of type CHSRScanner*

	@return	void	
*/
bool CRTorElement::ParseLine(CHSRScanner* pScanner)
{
	assert(pScanner && "pParse is NULL");
	bool bRet=false;
	
	if(pScanner)
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;

		m_pt[0].x = ATOF_T((*pScanner->m_aryFactor)[1].c_str());
		m_pt[0].y = ATOF_T((*pScanner->m_aryFactor)[2].c_str());
		m_pt[0].z = ATOF_T((*pScanner->m_aryFactor)[3].c_str());
		m_pt[1].x = ATOF_T((*pScanner->m_aryFactor)[4].c_str());
		m_pt[1].y = ATOF_T((*pScanner->m_aryFactor)[5].c_str());
		m_pt[1].z = ATOF_T((*pScanner->m_aryFactor)[6].c_str());
		
		m_ptOrigin.x= ATOF_T((*pScanner->m_aryFactor)[7].c_str());
		m_ptOrigin.y= ATOF_T((*pScanner->m_aryFactor)[8].c_str());
		m_ptOrigin.z= ATOF_T((*pScanner->m_aryFactor)[9].c_str());

		m_vecNorm.dx= ATOF_T((*pScanner->m_aryFactor)[10].c_str());
		m_vecNorm.dy= ATOF_T((*pScanner->m_aryFactor)[11].c_str());
		m_vecNorm.dz= ATOF_T((*pScanner->m_aryFactor)[12].c_str());
		
		m_nWidth = ATOF_T((*pScanner->m_aryFactor)[13].c_str());
		m_nHeight= ATOF_T((*pScanner->m_aryFactor)[14].c_str());

		bRet = true;
	}

	return bRet;
}
#endif

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool CRTorElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_pt[0].x >= volume.minx()) && (m_pt[0].x <= volume.maxx()) &&
		(m_pt[0].y >= volume.miny()) && (m_pt[0].y <= volume.maxy()) &&
		(m_pt[0].z >= volume.minz()) && (m_pt[0].z <= volume.maxz())) return true;
	
	if(	(m_pt[1].x >= volume.minx()) && (m_pt[1].x <= volume.maxx()) &&
		(m_pt[1].y >= volume.miny()) && (m_pt[1].y <= volume.maxy()) &&
		(m_pt[1].z >= volume.minz()) && (m_pt[1].z <= volume.maxz())) return true;
		
	return false;
}

/**	\brief	The CRTorElement::CreateFace function\n
	create face

	\param	pVIEWFormat	a parameter of type CPreViewFormat*
	\param	nID	a parameter of type long&

	\return	PFACE	
*/
CHSRFace* CRTorElement::CreateFace(CHSRViewFormat* pVIEWFormat,long& nID){
	assert(pVIEWFormat && "pVIEWFormat is NULL");
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pVIEWFormat){
		POINT_T ptOrigin=pVIEWFormat->MODEL2VIEW(m_ptOrigin);

		POINT_T pt[2]={0.,};
		pt[0] = pVIEWFormat->MODEL2VIEW(m_pt[0]);
		pt[1] = pVIEWFormat->MODEL2VIEW(m_pt[1]);
		
		double nWidth =pVIEWFormat->MODEL2VIEW(m_nWidth);
		double nHeight=pVIEWFormat->MODEL2VIEW(m_nHeight);

		CVect vec[4];
		vec[0].dx = pt[0].x - ptOrigin.x;vec[0].dy = pt[0].y - ptOrigin.y;vec[0].dz = pt[0].z - ptOrigin.z;
		vec[0].Normalize();
		vec[1].dx = pt[1].x - ptOrigin.x;vec[1].dy = pt[1].y - ptOrigin.y;vec[1].dz = pt[1].z - ptOrigin.z;
		vec[1].Normalize();
		
		VECTOR_T vecZ={0,0,1};
		VECTOR_T vecNorm  = pVIEWFormat->MODEL2VIEW(m_vecNorm);
		if(!CMath::NormalizeVector(vecNorm)) return NULL;

		POINT_T ptEDGE[2][64];
		double nDot=CMath::DotProduct(vecNorm,vecZ);
		if(fabs(nDot) < 0.001){	// parallel to Z axis
			ptEDGE[0][0].x = pt[0].x + vec[0].dx*nWidth*0.5 + vecNorm.dx*nHeight*0.5;
			ptEDGE[0][0].y = pt[0].y + vec[0].dy*nWidth*0.5 + vecNorm.dy*nHeight*0.5;
			ptEDGE[0][0].z = pt[0].z + vec[0].dz*nWidth*0.5 + vecNorm.dz*nHeight*0.5;
			ptEDGE[0][1].x = pt[0].x + vec[0].dx*nWidth*0.5 - vecNorm.dx*nHeight*0.5;
			ptEDGE[0][1].y = pt[0].y + vec[0].dy*nWidth*0.5 - vecNorm.dy*nHeight*0.5;
			ptEDGE[0][1].z = pt[0].z + vec[0].dz*nWidth*0.5 - vecNorm.dz*nHeight*0.5;
			ptEDGE[0][2].x = pt[1].x + vec[1].dx*nWidth*0.5 - vecNorm.dx*nHeight*0.5;
			ptEDGE[0][2].y = pt[1].y + vec[1].dy*nWidth*0.5 - vecNorm.dy*nHeight*0.5;
			ptEDGE[0][2].z = pt[1].z + vec[1].dz*nWidth*0.5 - vecNorm.dz*nHeight*0.5;
			ptEDGE[0][3].x = pt[1].x + vec[1].dx*nWidth*0.5 + vecNorm.dx*nHeight*0.5;
			ptEDGE[0][3].y = pt[1].y + vec[1].dy*nWidth*0.5 + vecNorm.dy*nHeight*0.5;
			ptEDGE[0][3].z = pt[1].z + vec[1].dz*nWidth*0.5 + vecNorm.dz*nHeight*0.5;
			ptEDGE[0][4] = ptEDGE[0][0];

			if(pRet=CHSRFace::CreateFace(5,ptEDGE[0])){
				pRet->m_enmLayer = GetHSRLayer();

				pRet->id     = nID++;
				pRet->ele    = HSR_POLYGON_SHAPE;
				pRet->m_enmLayer = LAYER_OTHER;
				pRet->SetColor(*m_pstrColor);
				
				pHSRApp->AppendFace(NULL,pRet);
			}
		}else if(fabs(fabs(nDot) - 1.) < 0.001){ // place in X-Y plane
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
			vec[2] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
			vec[3] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
			
			double nSweepAngle[3]={0,};
			nSweepAngle[0] = CGeometry::GetRotatedAngleInXYPlane(vec[2]);
			nSweepAngle[1] = CGeometry::GetRotatedAngleInXYPlane(vec[3]);
			
			POINT_T ptEDGE[2][64];
			nSweepAngle[2] = RAD2DEG(nSweepAngle[1] - nSweepAngle[0]);
			if(nSweepAngle[2] < 0.) nSweepAngle[2] += 360.;
			CArc3d::segments(ptEDGE[0],ptOrigin,pt[0],vecNorm,nSweepAngle[2],m_nSegments);
			
			VECTOR_T vecDir={0};
			int i = 0;
			for(i = 0;i < m_nSegments + 1;i++)
			{
				vecDir.dx = ptEDGE[0][i].x - ptOrigin.x;
				vecDir.dy = ptEDGE[0][i].y - ptOrigin.y;
				vecDir.dz = ptEDGE[0][i].z - ptOrigin.z;
				CMath::NormalizeVector(vecDir);
				
				ptEDGE[1][i].x = ptEDGE[0][i].x + vecDir.dx*nWidth*0.5;
				ptEDGE[1][i].y = ptEDGE[0][i].y + vecDir.dy*nWidth*0.5;
				ptEDGE[1][i].z = ptEDGE[0][i].z + vecDir.dz*nWidth*0.5;
				ptEDGE[0][i].x = ptEDGE[0][i].x - vecDir.dx*nWidth*0.5;
				ptEDGE[0][i].y = ptEDGE[0][i].y - vecDir.dy*nWidth*0.5;
				ptEDGE[0][i].z = ptEDGE[0][i].z - vecDir.dz*nWidth*0.5;
			}
			for(i = 0;i < m_nSegments + 1;i++)
			{
				ptEDGE[0][m_nSegments + i + 1] = ptEDGE[1][m_nSegments - i];
			}
			ptEDGE[0][m_nSegments + i + 1] = ptEDGE[0][0];

			if(pRet=CHSRFace::CreateFace(m_nSegments + i + 2,ptEDGE[0])){
				pRet->m_enmLayer = GetHSRLayer();

				pRet->id     = nID++;
				pRet->ele    = HSR_POLYGON_SHAPE;
				pRet->m_enmLayer = LAYER_OTHER;
				pRet->SetColor(*m_pstrColor);

				pHSRApp->AppendFace(NULL,pRet);
			}
		}else{
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
			vec[2] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
			vec[3] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
			
			double nSweepAngle[3]={0,};
			nSweepAngle[0] = CGeometry::GetRotatedAngleInXYPlane(vec[2]);
			nSweepAngle[1] = CGeometry::GetRotatedAngleInXYPlane(vec[3]);
			
			POINT_T ptEDGE[2][64];
			nSweepAngle[2] = RAD2DEG(nSweepAngle[1] - nSweepAngle[0]);
			if(nSweepAngle[2] < 0.) nSweepAngle[2] += 360.;
			CArc3d::segments(ptEDGE[0],ptOrigin,pt[0],vecNorm,nSweepAngle[2],m_nSegments);
			
			VECTOR_T vecDir={0};
			int i = 0;
			for(i = 0;i < m_nSegments + 1;i++)
			{
				vecDir.dx = ptEDGE[0][i].x - ptOrigin.x;
				vecDir.dy = ptEDGE[0][i].y - ptOrigin.y;
				vecDir.dz = ptEDGE[0][i].z - ptOrigin.z;
				
				ptEDGE[1][i].x = ptEDGE[0][i].x + (vecNorm.dx + vecDir.dx)*nWidth*0.5;
				ptEDGE[1][i].y = ptEDGE[0][i].y + (vecNorm.dy + vecDir.dy)*nWidth*0.5;
				ptEDGE[1][i].z = ptEDGE[0][i].z + (vecNorm.dz + vecDir.dz)*nWidth*0.5;
				ptEDGE[0][i].x = ptEDGE[0][i].x + (vecNorm.dx - vecDir.dx)*nWidth*0.5;
				ptEDGE[0][i].y = ptEDGE[0][i].y + (vecNorm.dx - vecDir.dy)*nWidth*0.5;
				ptEDGE[0][i].z = ptEDGE[0][i].z + (vecNorm.dz - vecDir.dz)*nWidth*0.5;
			}
			for(i=0;i < m_nSegments + 1;i++)
			{
				ptEDGE[0][m_nSegments + i + 1] = ptEDGE[1][m_nSegments - i];
			}
			ptEDGE[0][m_nSegments + i + 1] = ptEDGE[0][0];

			if(pRet=CHSRFace::CreateFace(m_nSegments + i + 2,ptEDGE[0])){
				pRet->m_enmLayer = GetHSRLayer();

				pRet->id     = nID++;
				pRet->ele    = HSR_POLYGON_SHAPE;
				pRet->m_enmLayer = LAYER_OTHER;
				pRet->SetColor(*m_pstrColor);

				pHSRApp->AppendFace(NULL,pRet);
			}
		}
	}

	return NULL;
}