// PreSCTNFormat.cpp: implementation of the CPreSCTNFormat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "hiddenDll.h"
#include "PreSCTNFormat.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CPreSCTNFormat::CPreSCTNFormat()
{
	m_vol.x[0] = m_vol.y[0] = m_vol.z[0] = 0.;
	m_vol.x[1] = m_vol.y[1] = m_vol.z[1] = 0.;
}

CPreSCTNFormat::~CPreSCTNFormat()
{

}

/**	\brief	The CPreSCTNFormat::ParseLine function

	\param	pPreFormatScanner	a parameter of type CHSRScanner*

	\return	void	
*/
void CPreSCTNFormat::ParseLine(CHSRScanner* pPreFormatScanner){
	assert(pPreFormatScanner && "pPreFormatScanner is NULL");

	if(pPreFormatScanner){
		m_enmType = ('H' == pPreFormatScanner->m_aryFactor[1][0]) ? H_TYPE : L_TYPE;

		m_pt[0].x = atof(pPreFormatScanner->m_aryFactor[2]);
		m_pt[0].y = atof(pPreFormatScanner->m_aryFactor[3]);
		m_pt[0].z = atof(pPreFormatScanner->m_aryFactor[4]);
		m_pt[1].x = atof(pPreFormatScanner->m_aryFactor[5]);
		m_pt[1].y = atof(pPreFormatScanner->m_aryFactor[6]);
		m_pt[1].z = atof(pPreFormatScanner->m_aryFactor[7]);
		
		m_nWidth  = atof(pPreFormatScanner->m_aryFactor[8]);
		m_nHeight = atof(pPreFormatScanner->m_aryFactor[9]);

		m_nSize[0]= atof(pPreFormatScanner->m_aryFactor[10]);
		m_nSize[1]= atof(pPreFormatScanner->m_aryFactor[11]);
		
		m_vec.dx = atof(pPreFormatScanner->m_aryFactor[12]);
		m_vec.dy = atof(pPreFormatScanner->m_aryFactor[13]);
		m_vec.dz = atof(pPreFormatScanner->m_aryFactor[14]);

		m_nRotate = atof(pPreFormatScanner->m_aryFactor[15]);
		
		Volume();
	}
}

/**	\brief	The CPreSCTNFormat::Volume function


	\return	void	
*/
void CPreSCTNFormat::Volume(){
	CVect vecAxis(m_pt[1].x - m_pt[0].x,m_pt[1].y - m_pt[0].y,m_pt[1].z - m_pt[0].z);
	vecAxis.Normalize();

	//double alpha=0.,beta=0.;
	//CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);

	CVect vecX(1,0,0),vecY(0,1,0);
	vecX = m_vec;
	vecY = CVect::operator *(vecAxis,vecX);

	//vecX = CGeometry::RotateAboutYXAxis(vecX,-beta,-alpha);
	//vecY = CGeometry::RotateAboutYXAxis(vecY,-beta,-alpha);

	POINT_T ptEDGE[2]={0,};
	ptEDGE[0].x = m_pt[0].x - vecX.dx*m_nWidth*0.5 - vecY.dx*m_nHeight*0.5;
	ptEDGE[0].y = m_pt[0].y - vecX.dy*m_nWidth*0.5 - vecY.dy*m_nHeight*0.5;
	ptEDGE[0].z = m_pt[0].z - vecX.dz*m_nWidth*0.5 - vecY.dz*m_nHeight*0.5;
	ptEDGE[1].x = m_pt[1].x + vecX.dx*m_nWidth*0.5 + vecY.dx*m_nHeight*0.5;
	ptEDGE[1].y = m_pt[1].y + vecX.dy*m_nWidth*0.5 + vecY.dy*m_nHeight*0.5;
	ptEDGE[1].z = m_pt[1].z + vecX.dz*m_nWidth*0.5 + vecY.dz*m_nHeight*0.5;

	m_vol.x[0] = (ptEDGE[0].x < ptEDGE[1].x) ? ptEDGE[0].x : ptEDGE[1].x;
	m_vol.x[1] = (ptEDGE[0].x > ptEDGE[1].x) ? ptEDGE[0].x : ptEDGE[1].x;
	m_vol.y[0] = (ptEDGE[0].y < ptEDGE[1].y) ? ptEDGE[0].y : ptEDGE[1].y;
	m_vol.y[1] = (ptEDGE[0].y > ptEDGE[1].y) ? ptEDGE[0].y : ptEDGE[1].y;
	m_vol.z[0] = (ptEDGE[0].z < ptEDGE[1].z) ? ptEDGE[0].z : ptEDGE[1].z;
	m_vol.z[1] = (ptEDGE[0].z > ptEDGE[1].z) ? ptEDGE[0].z : ptEDGE[1].z;
}

/**	\brief	The CPreSCTNFormat::IsInVolume function

	\param	volume	a parameter of type const VOLUME_T

	\return	bool	
*/
bool CPreSCTNFormat::IsInVolume(const VOLUME_T volume){
	if(	(m_vol.x[1] < volume.x[0]) || (m_vol.x[0] > volume.x[1]) ||
		(m_vol.y[1] < volume.y[0]) || (m_vol.y[0] > volume.y[1]) ||
		(m_vol.z[1] < volume.z[0]) || (m_vol.z[0] > volume.z[1])) return false;


	return true;
	/*
	if(	(m_pt[0].x >= volume.x[0]) && (m_pt[0].x <= volume.x[1]) &&
		(m_pt[0].y >= volume.y[0]) && (m_pt[0].y <= volume.y[1]) &&
		((m_pt[0].z+m_nHeight*0.5) >= volume.z[0]) && ((m_pt[0].z-m_nHeight*0.5) <= volume.z[1])) return true;
	
	if(	(m_pt[1].x >= volume.x[0]) && (m_pt[1].x <= volume.x[1]) &&
		(m_pt[1].y >= volume.y[0]) && (m_pt[1].y <= volume.y[1]) &&
		((m_pt[1].z+m_nHeight*0.5) >= volume.z[0]) && ((m_pt[1].z-m_nHeight*0.5) <= volume.z[1])) return true;
	
	return false;
	*/
}

/**	\brief	The CPreSCTNFormat::CreateFace function

	\param	pVIEWFormat	a parameter of type CPreViewFormat*
	\param	nID	a parameter of type long&

	\return	PFACE	
*/
PFACE CPreSCTNFormat::CreateFace(CHSRViewFormat* pVIEWFormat,long& nID){
	assert(pVIEWFormat && "pVIEWFormat is NULL");
	PFACE pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pVIEWFormat){
		CVect vecAxis(m_pt[1].x - m_pt[0].x,m_pt[1].y - m_pt[0].y,m_pt[1].z - m_pt[0].z);
		vecAxis.Normalize();
		
		double alpha=0.,beta=0.;
		CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
		
		VECTOR_T vecX={1,0,0},vecY={0,1,0};
		double nRad=DEG2RAD(m_nRotate);
		vecX.dx = cos(nRad);         vecX.dy = sin(nRad);
		vecY.dx = cos(nRad + PI*0.5);vecY.dy = sin(nRad + PI*0.5);
		vecX = CGeometry::RotateAboutYXAxis(vecX,-beta,-alpha);
		vecY = CGeometry::RotateAboutYXAxis(vecY,-beta,-alpha);

		m_ptEDGE[0].x = m_pt[0].x - vecX.dx*m_nWidth*0.5 + vecY.dx*m_nHeight*0.5;
		m_ptEDGE[0].y = m_pt[0].y - vecX.dy*m_nWidth*0.5 + vecY.dy*m_nHeight*0.5;
		m_ptEDGE[0].z = m_pt[0].z - vecX.dz*m_nWidth*0.5 + vecY.dz*m_nHeight*0.5;
		m_ptEDGE[1].x = m_ptEDGE[0].x + vecX.dx*m_nWidth;
		m_ptEDGE[1].y = m_ptEDGE[0].y + vecX.dy*m_nWidth;
		m_ptEDGE[1].z = m_ptEDGE[0].z + vecX.dz*m_nWidth;
		m_ptEDGE[2].x = m_ptEDGE[1].x - vecY.dx*m_nSize[0];
		m_ptEDGE[2].y = m_ptEDGE[1].y - vecY.dy*m_nSize[0];
		m_ptEDGE[2].z = m_ptEDGE[1].z - vecY.dz*m_nSize[0];
		m_ptEDGE[3].x = m_ptEDGE[2].x - vecX.dx*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[3].y = m_ptEDGE[2].y - vecX.dy*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[3].z = m_ptEDGE[2].z - vecX.dz*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[4].x = m_ptEDGE[3].x - vecY.dx*(m_nHeight - m_nSize[0]*2);
		m_ptEDGE[4].y = m_ptEDGE[3].y - vecY.dy*(m_nHeight - m_nSize[0]*2);
		m_ptEDGE[4].z = m_ptEDGE[3].z - vecY.dz*(m_nHeight - m_nSize[0]*2);
		m_ptEDGE[5].x = m_ptEDGE[4].x + vecX.dx*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[5].y = m_ptEDGE[4].y + vecX.dy*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[5].z = m_ptEDGE[4].z + vecX.dz*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[6].x = m_ptEDGE[5].x - vecY.dx*m_nSize[0];
		m_ptEDGE[6].y = m_ptEDGE[5].y - vecY.dy*m_nSize[0];
		m_ptEDGE[6].z = m_ptEDGE[5].z - vecY.dz*m_nSize[0];
		m_ptEDGE[7].x = m_ptEDGE[6].x - vecX.dx*m_nWidth;
		m_ptEDGE[7].y = m_ptEDGE[6].y - vecX.dy*m_nWidth;
		m_ptEDGE[7].z = m_ptEDGE[6].z - vecX.dz*m_nWidth;
		m_ptEDGE[8].x = m_ptEDGE[7].x + vecY.dx*m_nSize[0];
		m_ptEDGE[8].y = m_ptEDGE[7].y + vecY.dy*m_nSize[0];
		m_ptEDGE[8].z = m_ptEDGE[7].z + vecY.dz*m_nSize[0];
		m_ptEDGE[9].x = m_ptEDGE[8].x + vecX.dx*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[9].y = m_ptEDGE[8].y + vecX.dy*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[9].z = m_ptEDGE[8].z + vecX.dz*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[10].x = m_ptEDGE[9].x + vecY.dx*(m_nHeight - m_nSize[0]*2);
		m_ptEDGE[10].y = m_ptEDGE[9].y + vecY.dy*(m_nHeight - m_nSize[0]*2);
		m_ptEDGE[10].z = m_ptEDGE[9].z + vecY.dz*(m_nHeight - m_nSize[0]*2);
		m_ptEDGE[11].x = m_ptEDGE[10].x - vecX.dx*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[11].y = m_ptEDGE[10].y - vecX.dy*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[11].z = m_ptEDGE[10].z - vecX.dz*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[12] = m_ptEDGE[0];

		for(int i=0;i < 13;i++)	m_ptEDGE[i] = pVIEWFormat->MODEL2VIEW(m_ptEDGE[i]);
		PFACE pFace=CHSRFace::CreateFace(13,m_ptEDGE);
		if(pFace){
			pFace->id  = nID++;
			pFace->type= FACE_T;
			pFace->ele = POLYGON;
			
			pHSRApp->AppendFace(NULL,pFace);
		}

		m_ptEDGE[0].x = m_pt[1].x - vecX.dx*m_nWidth*0.5 + vecY.dx*m_nHeight*0.5;
		m_ptEDGE[0].y = m_pt[1].y - vecX.dy*m_nWidth*0.5 + vecY.dy*m_nHeight*0.5;
		m_ptEDGE[0].z = m_pt[1].z - vecX.dz*m_nWidth*0.5 + vecY.dz*m_nHeight*0.5;
		m_ptEDGE[1].x = m_ptEDGE[0].x + vecX.dx*m_nWidth;
		m_ptEDGE[1].y = m_ptEDGE[0].y + vecX.dy*m_nWidth;
		m_ptEDGE[1].z = m_ptEDGE[0].z + vecX.dz*m_nWidth;
		m_ptEDGE[2].x = m_ptEDGE[1].x - vecY.dx*m_nSize[0];
		m_ptEDGE[2].y = m_ptEDGE[1].y - vecY.dy*m_nSize[0];
		m_ptEDGE[2].z = m_ptEDGE[1].z - vecY.dz*m_nSize[0];
		m_ptEDGE[3].x = m_ptEDGE[2].x - vecX.dx*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[3].y = m_ptEDGE[2].y - vecX.dy*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[3].z = m_ptEDGE[2].z - vecX.dz*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[4].x = m_ptEDGE[3].x - vecY.dx*(m_nHeight - m_nSize[0]*2);
		m_ptEDGE[4].y = m_ptEDGE[3].y - vecY.dy*(m_nHeight - m_nSize[0]*2);
		m_ptEDGE[4].z = m_ptEDGE[3].z - vecY.dz*(m_nHeight - m_nSize[0]*2);
		m_ptEDGE[5].x = m_ptEDGE[4].x + vecX.dx*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[5].y = m_ptEDGE[4].y + vecX.dy*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[5].z = m_ptEDGE[4].z + vecX.dz*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[6].x = m_ptEDGE[5].x - vecY.dx*m_nSize[0];
		m_ptEDGE[6].y = m_ptEDGE[5].y - vecY.dy*m_nSize[0];
		m_ptEDGE[6].z = m_ptEDGE[5].z - vecY.dz*m_nSize[0];
		m_ptEDGE[7].x = m_ptEDGE[6].x - vecX.dx*m_nWidth;
		m_ptEDGE[7].y = m_ptEDGE[6].y - vecX.dy*m_nWidth;
		m_ptEDGE[7].z = m_ptEDGE[6].z - vecX.dz*m_nWidth;
		m_ptEDGE[8].x = m_ptEDGE[7].x + vecY.dx*m_nSize[0];
		m_ptEDGE[8].y = m_ptEDGE[7].y + vecY.dy*m_nSize[0];
		m_ptEDGE[8].z = m_ptEDGE[7].z + vecY.dz*m_nSize[0];
		m_ptEDGE[9].x = m_ptEDGE[8].x + vecX.dx*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[9].y = m_ptEDGE[8].y + vecX.dy*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[9].z = m_ptEDGE[8].z + vecX.dz*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[10].x = m_ptEDGE[9].x + vecY.dx*(m_nHeight - m_nSize[0]*2);
		m_ptEDGE[10].y = m_ptEDGE[9].y + vecY.dy*(m_nHeight - m_nSize[0]*2);
		m_ptEDGE[10].z = m_ptEDGE[9].z + vecY.dz*(m_nHeight - m_nSize[0]*2);
		m_ptEDGE[11].x = m_ptEDGE[10].x - vecX.dx*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[11].y = m_ptEDGE[10].y - vecX.dy*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[11].z = m_ptEDGE[10].z - vecX.dz*(m_nWidth - m_nSize[1])*0.5;
		m_ptEDGE[12] = m_ptEDGE[0];
		
		for(i=0;i < 13;i++)	m_ptEDGE[i] = pVIEWFormat->MODEL2VIEW(m_ptEDGE[i]);
		if(pFace=CHSRFace::CreateFace(13,m_ptEDGE)){
			pFace->id  = nID++;
			pFace->type= FACE_T;
			pFace->ele = POLYGON;
			
			pHSRApp->AppendFace(NULL,pFace);
		}

		POINT_T pt[2]={0,};
		pt[0] = pVIEWFormat->MODEL2VIEW(m_pt[0]);
		pt[1] = pVIEWFormat->MODEL2VIEW(m_pt[1]);
		
		VECTOR_T vec={0.},vecZ={0,0,1};
		vec.dx = pt[1].x - pt[0].x;
		vec.dy = pt[1].y - pt[0].y;
		vec.dz = pt[1].z - pt[0].z;
		CMath::NormalizeVector(vec);
		
		double nWidth =pVIEWFormat->MODEL2VIEW(m_nWidth);
		double nHeight=pVIEWFormat->MODEL2VIEW(m_nHeight);
		
		double nSize[2]={0,};
		nSize[0] = pVIEWFormat->MODEL2VIEW(m_nSize[0]);
		nSize[1] = pVIEWFormat->MODEL2VIEW(m_nSize[1]);
		
		double nDot=CMath::DotProduct(vec,vecZ);
		if(fabs(nDot) < 0.001){	// place on X-Y plane
			VECTOR_T vecCross = CMath::CrossProduct(vecZ,vec);
			CMath::NormalizeVector(vecCross);
			
			POINT_T ptMid={(pt[0].x + pt[1].x)*0.5,(pt[0].y + pt[1].y)*0.5,(pt[0].z + pt[1].z)*0.5 + nHeight*0.5};
			double dx=pt[1].x - pt[0].x;double dy=pt[1].y - pt[0].y;double dz=pt[1].z - pt[0].z;
			double nLen=sqrt(dx*dx + dy*dy + dz*dz);
			
			m_ptEDGE[0].x = ptMid.x + vec.dx*nLen*0.5 + vecCross.dx*nWidth*0.5;
			m_ptEDGE[0].y = ptMid.y + vec.dy*nLen*0.5 + vecCross.dy*nWidth*0.5;
			m_ptEDGE[0].z = ptMid.z + vec.dz*nLen*0.5 + vecCross.dz*nWidth*0.5;
			m_ptEDGE[1].x = ptMid.x + vec.dx*nLen*0.5 - vecCross.dx*nWidth*0.5;
			m_ptEDGE[1].y = ptMid.y + vec.dy*nLen*0.5 - vecCross.dy*nWidth*0.5;
			m_ptEDGE[1].z = ptMid.z + vec.dz*nLen*0.5 - vecCross.dz*nWidth*0.5;
			m_ptEDGE[2].x = ptMid.x - vec.dx*nLen*0.5 - vecCross.dx*nWidth*0.5;
			m_ptEDGE[2].y = ptMid.y - vec.dy*nLen*0.5 - vecCross.dy*nWidth*0.5;
			m_ptEDGE[2].z = ptMid.z - vec.dz*nLen*0.5 - vecCross.dz*nWidth*0.5;
			m_ptEDGE[3].x = ptMid.x - vec.dx*nLen*0.5 + vecCross.dx*nWidth*0.5;
			m_ptEDGE[3].y = ptMid.y - vec.dy*nLen*0.5 + vecCross.dy*nWidth*0.5;
			m_ptEDGE[3].z = ptMid.z - vec.dz*nLen*0.5 + vecCross.dz*nWidth*0.5;
			m_ptEDGE[4]   = m_ptEDGE[0];
			
			PFACE pFace=CHSRFace::CreateFace(5,m_ptEDGE);
			if(pFace){
				pFace->id  = nID++;
				pFace->type=FACE_T;
				pFace->ele = POLYGON;
				
				pHSRApp->AppendFace(NULL,pFace);
			}
		}else{
			VECTOR_T vecCross = CMath::CrossProduct(vecZ,vec);
			CMath::NormalizeVector(vecCross);
			
			POINT_T ptMid={(pt[0].x + pt[1].x)*0.5,(pt[0].y + pt[1].y)*0.5,(pt[0].z + pt[1].z)*0.5};
			double dx=pt[1].x - pt[0].x;double dy=pt[1].y - pt[0].y;double dz=pt[1].z - pt[0].z;
			double nLen=sqrt(dx*dx + dy*dy + dz*dz);
			
			m_ptEDGE[0].x = ptMid.x + vec.dx*nLen*0.5 + vecCross.dx*nWidth*0.5;// + vecY.dx*nHeight*0.5;
			m_ptEDGE[0].y = ptMid.y + vec.dy*nLen*0.5 + vecCross.dy*nWidth*0.5;// + vecY.dy*nHeight*0.5;
			m_ptEDGE[0].z = ptMid.z + vec.dz*nLen*0.5 + vecCross.dz*nWidth*0.5;// + vecY.dz*nHeight*0.5;
			m_ptEDGE[1].x = ptMid.x + vec.dx*nLen*0.5 - vecCross.dx*nWidth*0.5;// + vecY.dx*nHeight*0.5;
			m_ptEDGE[1].y = ptMid.y + vec.dy*nLen*0.5 - vecCross.dy*nWidth*0.5;// + vecY.dy*nHeight*0.5;
			m_ptEDGE[1].z = ptMid.z + vec.dz*nLen*0.5 - vecCross.dz*nWidth*0.5;// + vecY.dz*nHeight*0.5;
			m_ptEDGE[2].x = ptMid.x - vec.dx*nLen*0.5 - vecCross.dx*nWidth*0.5;// + vecY.dx*nHeight*0.5;
			m_ptEDGE[2].y = ptMid.y - vec.dy*nLen*0.5 - vecCross.dy*nWidth*0.5;// + vecY.dy*nHeight*0.5;
			m_ptEDGE[2].z = ptMid.z - vec.dz*nLen*0.5 - vecCross.dz*nWidth*0.5;// + vecY.dz*nHeight*0.5;
			m_ptEDGE[3].x = ptMid.x - vec.dx*nLen*0.5 + vecCross.dx*nWidth*0.5;// + vecY.dx*nHeight*0.5;
			m_ptEDGE[3].y = ptMid.y - vec.dy*nLen*0.5 + vecCross.dy*nWidth*0.5;// + vecY.dy*nHeight*0.5;
			m_ptEDGE[3].z = ptMid.z - vec.dz*nLen*0.5 + vecCross.dz*nWidth*0.5;// + vecY.dz*nHeight*0.5;
			m_ptEDGE[4]   = m_ptEDGE[0];
			
			PFACE pFace=CHSRFace::CreateFace(5,m_ptEDGE);
			if(pFace){
				pFace->id  = nID++;
				pFace->type=FACE_T;
				pFace->ele = POLYGON;
				
				pHSRApp->AppendFace(NULL,pFace);
			}
		}
	}

	return NULL;
}
