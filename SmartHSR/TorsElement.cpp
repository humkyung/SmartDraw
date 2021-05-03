// PreTORSFormat.cpp: implementation of the CTorsElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "TorsElement.h"
#include "SmartHSR.h"

IMPLEMENTS_HSR_ELEMENT(CTorsElement , CHSRElement , _T("TORS"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CTorsElement::CTorsElement()
{
	m_nSegments = 16;
}

/**	\brief
*/
CTorsElement::~CTorsElement()
{

}

/**	
	@brief	parse <TORS>,... and <MUEL>,... line

	@author	HumKyung.BAEK

	@param	pHSRScanner	a parameter of type CHSRScanner*

	@return	void	
*/
bool CTorsElement::ParseLine(CHSRScanner* pScanner)
{
	assert(pScanner && "pcanner is NULL");
	bool bRet=false;
	
	if(pScanner)
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;
		
		m_ptOrigin.x= ATOF_T((*pScanner->m_aryFactor)[1].c_str());
		m_ptOrigin.y= ATOF_T((*pScanner->m_aryFactor)[2].c_str());
		m_ptOrigin.z= ATOF_T((*pScanner->m_aryFactor)[3].c_str());
		
		m_nTorusRadius = ATOF_T((*pScanner->m_aryFactor)[4].c_str());
		m_nTubeRadius  = ATOF_T((*pScanner->m_aryFactor)[5].c_str());
		
		m_vecNorm.dx = ATOF_T((*pScanner->m_aryFactor)[6].c_str());
		m_vecNorm.dy = ATOF_T((*pScanner->m_aryFactor)[7].c_str());
		m_vecNorm.dz = ATOF_T((*pScanner->m_aryFactor)[8].c_str());
		m_vecAxis.dx = ATOF_T((*pScanner->m_aryFactor)[9].c_str());
		m_vecAxis.dy = ATOF_T((*pScanner->m_aryFactor)[10].c_str());
		m_vecAxis.dz = ATOF_T((*pScanner->m_aryFactor)[11].c_str());

		m_nStartAng = ATOF_T((*pScanner->m_aryFactor)[12].c_str());
		m_nSweepAng = ATOF_T((*pScanner->m_aryFactor)[13].c_str());
		double nEndAng = ((m_nStartAng + m_nSweepAng) > 2*PI) ? m_nStartAng + m_nSweepAng - 2*PI : m_nStartAng + m_nSweepAng;

		if(fabs(m_nSweepAng - 2*PI) > 0.001)
		{
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(m_vecNorm,alpha,beta);
			
			VECTOR_T vecAxis=CGeometry::RotateAboutXYAxis(m_vecAxis,alpha,beta);
			double nStartAng=CGeometry::GetRotatedAngleInXYPlane(vecAxis);

			m_nStartAng = ATOF_T((*pScanner->m_aryFactor)[12].c_str());
			m_nSweepAng = ATOF_T((*pScanner->m_aryFactor)[13].c_str());
			nStartAng= ((m_nStartAng + nStartAng) > 2*PI) ? m_nStartAng + nStartAng - 2*PI : m_nStartAng + nStartAng;
			double nEndAng  = ((nStartAng + m_nSweepAng) > 2*PI) ? nStartAng + m_nSweepAng - 2*PI : nStartAng + m_nSweepAng;

			m_pt[0].x = m_nTorusRadius;m_pt[0].y = 0.,m_pt[0].z = 0.;
			m_pt[1].x = m_nTorusRadius;m_pt[1].y = 0.,m_pt[1].z = 0.;
			m_pt[0].x = cos(nStartAng)*m_nTorusRadius;
			m_pt[0].y = sin(nStartAng)*m_nTorusRadius;
			m_pt[1].x = cos(nEndAng)*m_nTorusRadius;
			m_pt[1].y = sin(nEndAng)*m_nTorusRadius;

			m_pt[0] = CGeometry::RotateAboutYXAxis(m_pt[0],-beta,-alpha);
			m_pt[0].x += m_ptOrigin.x;m_pt[0].y += m_ptOrigin.y;m_pt[0].z += m_ptOrigin.z;
			m_pt[1] = CGeometry::RotateAboutYXAxis(m_pt[1],-beta,-alpha);
			m_pt[1].x += m_ptOrigin.x;m_pt[1].y += m_ptOrigin.y;m_pt[1].z += m_ptOrigin.z;
		}
		bRet = true;
	}

	return bRet;
}

/**	\brief	The CTorsElement::IsInVolume function

	\param	volume	a parameter of type const VOLUME_T

	\return	bool	
*/
bool CTorsElement::IsInVolume(const CIsVolume& volume)
{
	if(fabs(m_nSweepAng - 2*PI) > 0.001)
	{
		if(	(m_pt[0].x >= volume.minx()) && (m_pt[0].x <= volume.maxx()) &&
			(m_pt[0].y >= volume.miny()) && (m_pt[0].y <= volume.maxy()) &&
			(m_pt[0].z >= volume.minz()) && (m_pt[0].z <= volume.maxz())) return true;
		
		if(	(m_pt[1].x >= volume.minx()) && (m_pt[1].x <= volume.maxx()) &&
			(m_pt[1].y >= volume.miny()) && (m_pt[1].y <= volume.maxy()) &&
			(m_pt[1].z >= volume.minz()) && (m_pt[1].z <= volume.maxz())) return true;
	}
	else
	{
		if(	(m_ptOrigin.x >= volume.minx()) && (m_ptOrigin.x <= volume.maxx()) &&
		(m_ptOrigin.y >= volume.miny()) && (m_ptOrigin.y <= volume.maxy()) &&
		(m_ptOrigin.z >= volume.minz()) && (m_ptOrigin.z <= volume.maxz())) return true;
	}
	
	return false;
}

/**	
	@brief	create torus face

	@author	HumKyung.BAEK

	@param	pVIEWFormat	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@return	PFACE	
*/
CHSRFace* CTorsElement::CreateFace(CHSRViewFormat* pVIEWFormat,long& nID)
{
	assert(pVIEWFormat && "pVIEWFormat is NULL");
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pVIEWFormat)
	{
		if(fabs(m_nSweepAng - 2*PI) > 0.001)
		{
			//--> convert model points to view points
			POINT_T ptOrigin=pVIEWFormat->MODEL2VIEW(m_ptOrigin);

			POINT_T pt[2]={0.,};
			pt[0] = pVIEWFormat->MODEL2VIEW(m_pt[0]);
			pt[1] = pVIEWFormat->MODEL2VIEW(m_pt[1]);
			
			double nTubeRadius =pVIEWFormat->MODEL2VIEW(m_nTubeRadius);

			CVect vec[4];
			vec[0].dx = pt[0].x - ptOrigin.x;vec[0].dy = pt[0].y - ptOrigin.y;vec[0].dz = pt[0].z - ptOrigin.z;
			vec[0].Normalize();
			vec[1].dx = pt[1].x - ptOrigin.x;vec[1].dy = pt[1].y - ptOrigin.y;vec[1].dz = pt[1].z - ptOrigin.z;
			vec[1].Normalize();
			
			VECTOR_T vecZ={0,0,1};
			CVect vecNorm  = pVIEWFormat->MODEL2VIEW(m_vecNorm) , vecAxis;
			vecNorm.Normalize();

			if(fabs(m_nSweepAng - PI) < 0.001)
			{
				vecAxis.dx = pt[0].x - ptOrigin.x;
				vecAxis.dy = pt[0].y - ptOrigin.y;
				vecAxis.dz = pt[0].z - ptOrigin.z;
				vecAxis.Normalize();
				vecAxis = vecNorm * vecAxis;
				vecAxis.Normalize();
			}
			else
			{
				vecAxis.dx = vecAxis.dy = vecAxis.dz = 0.f;
			}
			
			const double dx=pt[0].x - ptOrigin.x;
			const double dy=pt[0].y - ptOrigin.y;
			const double dz=pt[0].z - ptOrigin.z;
			const double nTorusRadius=sqrt(dx*dx + dy*dy + dz*dz);
			//<--

			POINT_T ptEDGE[2][64];
			const double nDot=CMath::DotProduct(vecNorm,vecZ);
			if(fabs(nDot) < 0.001)
			{	// parallel to Z axis
				ptEDGE[0][0].x = pt[0].x + vecAxis.dx*nTorusRadius + vecNorm.dx*nTubeRadius;
				ptEDGE[0][0].y = pt[0].y + vecAxis.dy*nTorusRadius + vecNorm.dy*nTubeRadius;
				ptEDGE[0][0].z = pt[0].z + vecAxis.dz*nTorusRadius + vecNorm.dz*nTubeRadius;
				ptEDGE[0][1].x = pt[0].x + vecAxis.dx*nTorusRadius - vecNorm.dx*nTubeRadius;
				ptEDGE[0][1].y = pt[0].y + vecAxis.dy*nTorusRadius - vecNorm.dy*nTubeRadius;
				ptEDGE[0][1].z = pt[0].z + vecAxis.dz*nTorusRadius - vecNorm.dz*nTubeRadius;
				ptEDGE[0][2].x = pt[1].x + vecAxis.dx*nTorusRadius - vecNorm.dx*nTubeRadius;
				ptEDGE[0][2].y = pt[1].y + vecAxis.dy*nTorusRadius - vecNorm.dy*nTubeRadius;
				ptEDGE[0][2].z = pt[1].z + vecAxis.dz*nTorusRadius - vecNorm.dz*nTubeRadius;
				ptEDGE[0][3].x = pt[1].x + vecAxis.dx*nTorusRadius + vecNorm.dx*nTubeRadius;
				ptEDGE[0][3].y = pt[1].y + vecAxis.dy*nTorusRadius + vecNorm.dy*nTubeRadius;
				ptEDGE[0][3].z = pt[1].z + vecAxis.dz*nTorusRadius + vecNorm.dz*nTubeRadius;
				ptEDGE[0][4] = ptEDGE[0][0];

				if(pRet=CHSRFace::CreateFace(5,ptEDGE[0]))
				{
					pRet->type = TORUS;
					pRet->id   = nID++;
					pRet->ele  = HSR_POLYGON_SHAPE;
					pRet->m_enmLayer = GetHSRLayer();
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
					
					ptEDGE[1][i].x = ptEDGE[0][i].x + vecDir.dx*nTubeRadius;
					ptEDGE[1][i].y = ptEDGE[0][i].y + vecDir.dy*nTubeRadius;
					ptEDGE[1][i].z = ptEDGE[0][i].z + vecDir.dz*nTubeRadius;
					ptEDGE[0][i].x = ptEDGE[0][i].x - vecDir.dx*nTubeRadius;
					ptEDGE[0][i].y = ptEDGE[0][i].y - vecDir.dy*nTubeRadius;
					ptEDGE[0][i].z = ptEDGE[0][i].z - vecDir.dz*nTubeRadius;
				}
				for(i = 0;i < m_nSegments + 1;i++)
				{
					ptEDGE[0][m_nSegments + i + 1] = ptEDGE[1][m_nSegments - i];
				}
				ptEDGE[0][m_nSegments + i + 1] = ptEDGE[0][0];

				if(pRet=CHSRFace::CreateFace(m_nSegments + i + 2,ptEDGE[0]))
				{
					pRet->type   = TORUS;
					pRet->id     = nID++;
					pRet->ele    = HSR_POLYGON_SHAPE;
					pRet->m_enmLayer = GetHSRLayer();
					pRet->SetColor(*m_pstrColor);

					pHSRApp->AppendFace(NULL,pRet);
				}
			}else{	// SLOPE.
				double dx=pt[0].x - ptOrigin.x;
				double dy=pt[0].y - ptOrigin.y;
				double dz=pt[0].z - ptOrigin.z;
				double nTorusRadius=sqrt(dx*dx + dy*dy + dz*dz);
		
				VECTOR_T vec[4]={0,};
				vec[0].dx = pt[0].x - ptOrigin.x;vec[0].dy = pt[0].y - ptOrigin.y;vec[0].dz = pt[0].z - ptOrigin.z;
				vec[1].dx = pt[1].x - ptOrigin.x;vec[1].dy = pt[1].y - ptOrigin.y;vec[1].dz = pt[1].z - ptOrigin.z;
		
				VECTOR_T vecRotate={0.},vecZ={0.,0.,1.};
				CMath::GetCrossProduct(vecNorm,vec[0],vec[1]);
				CMath::NormalizeVector(vecNorm);

				double alpha=0.,beta=0.;
				CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
				vec[0] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
				vec[1] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
			
				double nStartAngle = CGeometry::GetRotatedAngleInXYPlane(vec[0]);
				double nEndAngle   = CGeometry::GetRotatedAngleInXYPlane(vec[1]);
				double nSweepAngle = nEndAngle - nStartAngle;
				if(nSweepAngle < 0.) nSweepAngle += 2*PI;
				double nStepAngle  = nSweepAngle/m_nSegments;

				POINT_T ptEDGE[2][64];
				double nAngle=nStartAngle;
				POINT_T ptCenter={0};
				int i = 0;
				for(i = 0;i < m_nSegments;i++)
				{
					ptCenter = CArc3d::on(ptOrigin,vecNorm,nTorusRadius,nAngle);
					nAngle  += nStepAngle;
					
					vec[0].dx = ptCenter.x - ptOrigin.x;
					vec[0].dy = ptCenter.y - ptOrigin.y;
					vec[0].dz = ptCenter.z - ptOrigin.z;
					CMath::GetCrossProduct(vecRotate,vecNorm,vec[0]);
					
					CMath::GetCrossProduct(vec[3],vecRotate,vecZ);
					CMath::NormalizeVector(vec[3]);
					
					double nDot = CMath::DotProduct(vecRotate,vec[3]);
					vec[3].dx -= nDot*vecRotate.dx;
					vec[3].dy -= nDot*vecRotate.dy;
					vec[3].dz -= nDot*vecRotate.dz;
					
					ptEDGE[0][i].x = ptCenter.x + vec[3].dx*nTubeRadius;
					ptEDGE[0][i].y = ptCenter.y + vec[3].dy*nTubeRadius;
					ptEDGE[0][i].z = ptCenter.z + vec[3].dz*nTubeRadius;
					ptEDGE[1][i].x = ptCenter.x - vec[3].dx*nTubeRadius;
					ptEDGE[1][i].y = ptCenter.y - vec[3].dy*nTubeRadius;
					ptEDGE[1][i].z = ptCenter.z - vec[3].dz*nTubeRadius;
				}
				vec[0].dx = pt[1].x - ptOrigin.x;
				vec[0].dy = pt[1].y - ptOrigin.y;
				vec[0].dz = pt[1].z - ptOrigin.z;
				CMath::GetCrossProduct(vecRotate,vecNorm,vec[0]);
				CMath::NormalizeVector(vecRotate);
		
				CMath::GetCrossProduct(vec[3],vecRotate,vecZ);
				CMath::NormalizeVector(vec[3]);
				
				if(fabs(vecRotate.dz - vecZ.dz)	< 0.005){
					vec[3] = vecNorm;
					CMath::NormalizeVector(vec[3]);
				}else{
					CMath::GetCrossProduct(vec[3],vecRotate,vecZ);
					CMath::NormalizeVector(vec[3]);
				}
				
				double nDot = CMath::DotProduct(vecRotate,vec[3]);
				vec[3].dx -= nDot*vecRotate.dx;
				vec[3].dy -= nDot*vecRotate.dy;
				vec[3].dz -= nDot*vecRotate.dz;
		
				ptEDGE[0][i].x = pt[1].x + vec[3].dx*nTubeRadius;
				ptEDGE[0][i].y = pt[1].y + vec[3].dy*nTubeRadius;
				ptEDGE[0][i].z = pt[1].z + vec[3].dz*nTubeRadius;
				ptEDGE[1][i].x = pt[1].x - vec[3].dx*nTubeRadius;
				ptEDGE[1][i].y = pt[1].y - vec[3].dy*nTubeRadius;
				ptEDGE[1][i].z = pt[1].z - vec[3].dz*nTubeRadius;

				for(i=0;i < m_nSegments + 1;i++)
				{
					ptEDGE[0][m_nSegments + i + 1] = ptEDGE[1][m_nSegments - i];
				}
				ptEDGE[0][m_nSegments + i + 1] = ptEDGE[0][0];
				
				if(pRet=CHSRFace::CreateFace(m_nSegments + i + 2,ptEDGE[0])){
					pRet->id    = nID++;
					pRet->type  = TORUS;
					pRet->ele   = HSR_POLYGON_SHAPE;
					pRet->m_enmLayer = GetHSRLayer();
					pRet->SetColor(*m_pstrColor);

					pHSRApp->AppendFace(NULL,pRet);
				}
			}
		}else{
			assert(pVIEWFormat && "pVIEWFormat is NULL");
			CHSRFace* pRet=NULL;
			CHSRApp* pHSRApp = CHSRApp::GetInstance();
			
			if(pVIEWFormat){
				POINT_T ptOrigin[3]={0.,};
				ptOrigin[0]         =pVIEWFormat->MODEL2VIEW(m_ptOrigin);
				double  nTubeRadius =pVIEWFormat->MODEL2VIEW(m_nTubeRadius);
				double  nTorusRadius=pVIEWFormat->MODEL2VIEW(m_nTorusRadius);
				
				VECTOR_T vecNorm={0};
				VECTOR_T _vec[2]={0.};
				vecNorm = pVIEWFormat->MODEL2VIEW(m_vecNorm);
				CMath::NormalizeVector(vecNorm);
				_vec[1] = pVIEWFormat->MODEL2VIEW(m_vecAxis);
				
				VECTOR_T vec={0},vecRotate={0.};
				VECTOR_T vecZ={0.,0.,1.};
				CMath::GetCrossProduct(_vec[0],vecNorm,_vec[1]);
				CMath::GetCrossProduct(vecRotate,vecNorm,_vec[0]);
				CMath::NormalizeVector(vecRotate);
				
				double nRadius[2]={0,};
				double nDot=CMath::DotProduct(vecNorm.dx,vecNorm.dy,vecNorm.dz,vecZ.dx,vecZ.dy,vecZ.dz);
				if(fabs(nDot) < 0.001)
				{
					CMath::GetCrossProduct(vec,vecRotate,_vec[0]);
					CMath::NormalizeVector(vec);
					
					VECTOR_T vecAxis={0};
					CMath::GetCrossProduct(vecAxis,vecNorm,vecZ);
					CMath::NormalizeVector(vecAxis);
					POINT_T ptEDGE[5];
					ptEDGE[0].x = ptOrigin[0].x + vecNorm.dx*nTubeRadius + vecAxis.dx*nTorusRadius;
					ptEDGE[0].y = ptOrigin[0].y + vecNorm.dy*nTubeRadius + vecAxis.dy*nTorusRadius;
					ptEDGE[0].z = ptOrigin[0].z + vecNorm.dz*nTubeRadius + vecAxis.dz*nTorusRadius;
					ptEDGE[1].x = ptOrigin[0].x - vecNorm.dx*nTubeRadius + vecAxis.dx*nTorusRadius;
					ptEDGE[1].y = ptOrigin[0].y - vecNorm.dy*nTubeRadius + vecAxis.dy*nTorusRadius;
					ptEDGE[1].z = ptOrigin[0].z - vecNorm.dz*nTubeRadius + vecAxis.dz*nTorusRadius;
					ptEDGE[2].x = ptOrigin[0].x - vecNorm.dx*nTubeRadius - vecAxis.dx*nTorusRadius;
					ptEDGE[2].y = ptOrigin[0].y - vecNorm.dy*nTubeRadius - vecAxis.dy*nTorusRadius;
					ptEDGE[2].z = ptOrigin[0].z - vecNorm.dz*nTubeRadius - vecAxis.dz*nTorusRadius;
					ptEDGE[3].x = ptOrigin[0].x + vecNorm.dx*nTubeRadius - vecAxis.dx*nTorusRadius;
					ptEDGE[3].y = ptOrigin[0].y + vecNorm.dy*nTubeRadius - vecAxis.dy*nTorusRadius;
					ptEDGE[3].z = ptOrigin[0].z + vecNorm.dz*nTubeRadius - vecAxis.dz*nTorusRadius;
					ptEDGE[4]   = ptEDGE[0];
					
					if(pRet=CHSRFace::CreateFace(5,ptEDGE)){
						pRet->type  = TORUS;
						pRet->id    = nID++;
						pRet->ele   = HSR_POLYGON_SHAPE;
						pRet->m_enmLayer = GetHSRLayer();
						pRet->SetColor(*m_pstrColor);
						
						pHSRApp->AppendFace(NULL,pRet);
						return NULL;
					}
					nRadius[0] = nRadius[1] = nTorusRadius;
				}else if(fabs(fabs(nDot) - 1.) < 0.001){
					ptOrigin[1] = ptOrigin[0];
					ptOrigin[2] = ptOrigin[0];
					
					vec = m_vecNorm;
					nRadius[0] = nTorusRadius - nTubeRadius;
					nRadius[1] = nTorusRadius + nTubeRadius;
				}else{
					CMath::GetCrossProduct(vec,vecRotate,vecZ);
					if(CMath::NormalizeVector(vec)){	
						ptOrigin[1].x = ptOrigin[0].x + vec.dx*nTubeRadius;
						ptOrigin[1].y = ptOrigin[0].y + vec.dy*nTubeRadius;
						ptOrigin[1].z = ptOrigin[0].z + vec.dz*nTubeRadius;
						ptOrigin[2].x = ptOrigin[0].x - vec.dx*nTubeRadius;
						ptOrigin[2].y = ptOrigin[0].y - vec.dy*nTubeRadius;
						ptOrigin[2].z = ptOrigin[0].z - vec.dz*nTubeRadius;	
					}else{
						ptOrigin[1].x = ptOrigin[0].x + vecNorm.dx*nTubeRadius;
						ptOrigin[1].y = ptOrigin[0].y + vecNorm.dy*nTubeRadius;
						ptOrigin[1].z = ptOrigin[0].z + vecNorm.dz*nTubeRadius;
						ptOrigin[2].x = ptOrigin[0].x - vecNorm.dx*nTubeRadius;
						ptOrigin[2].y = ptOrigin[0].y - vecNorm.dy*nTubeRadius;
						ptOrigin[2].z = ptOrigin[0].z - vecNorm.dz*nTubeRadius;
					}
					
					nRadius[0] = nRadius[1] = nTorusRadius;
				}
				
				POINT_T ptEdge[3][32];
				CreateCircleEdge(ptEdge[0],ptOrigin[1],nRadius[0],vecNorm);
				CreateCircleEdge(ptEdge[1],ptOrigin[2],nRadius[1],vecNorm);
				
				int nCount=0;
				for(int i=0;i < m_nSegments*0.5 + 1;i++,nCount++) ptEdge[2][nCount]=ptEdge[0][i];
				for(int i = int(m_nSegments*0.5);i >= 0;i--,nCount++)      ptEdge[2][nCount]=ptEdge[1][i];
				ptEdge[2][nCount++]=ptEdge[2][0];
				CHSRFace* pTORS=CHSRFace::CreateFace(nCount,ptEdge[2]);
				if(pTORS)
				{
					pTORS->CalcPlaneEquation();
					pTORS->type  = TORUS;
					pTORS->id    = nID++;
					pTORS->ele   = HSR_POLYGON_SHAPE;
					pTORS->m_enmLayer = GetHSRLayer();
					pTORS->SetColor(*m_pstrColor);
					
					pHSRApp->AppendFace(NULL,pTORS);
				}
				
				nCount=0;
				for(int i = int(m_nSegments*0.5);i < m_nSegments + 1;i++,nCount++) ptEdge[2][nCount]=ptEdge[0][i];
				for(int i = m_nSegments;i >= m_nSegments*0.5;i--,nCount++)        ptEdge[2][nCount]=ptEdge[1][i];
				ptEdge[2][nCount++]=ptEdge[2][0];
				if(pTORS=CHSRFace::CreateFace(nCount,ptEdge[2])){
					pTORS->CalcPlaneEquation();
					pTORS->type  = TORUS;
					pTORS->id    = nID++;
					pTORS->ele   = HSR_POLYGON_SHAPE;
					pTORS->m_enmLayer = GetHSRLayer();
					pTORS->SetColor(*m_pstrColor);
					
					pHSRApp->AppendFace(NULL,pTORS);
				}
			}
		}
	}
	
	return NULL;
}

/**	\brief	The CTorsElement::CreateCircleEdge function

	\param	ptEdge[]	a parameter of type POINT_T
	\param	ptOrigin	a parameter of type POINT_T
	\param	nRadius	a parameter of type double
	\param	vecNorm	a parameter of type VECTOR_T

	\return	void	
*/
void CTorsElement::CreateCircleEdge(POINT_T ptEdge[],POINT_T ptOrigin,double nRadius,VECTOR_T vecNorm){
	double alpha=0.,beta=0;
	CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
	
	double nStep    = 2*PI/m_nSegments;
	int i=0;
	for(double t=0;t < 2*PI;t += nStep,i++){
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
}