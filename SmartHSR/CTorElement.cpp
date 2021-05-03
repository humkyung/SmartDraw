// PreCTORFormat.cpp: implementation of the CCTorElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "CTorElement.h"
#include "SmartHSR.h"

IMPLEMENTS_HSR_ELEMENT(CCTorElement , CHSRElement , _T("CTOR"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCTorElement::CCTorElement()
{
	m_nSegments = 20;
}

CCTorElement::~CCTorElement()
{

}

#ifdef VER_03
/**	\brief	The CCTorElement::ParseLine function

	\param	pHSRScanner	a parameter of type CHSRScanner*

	\return	bool	
*/
bool CCTorElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		char* pVal=NULL;
		if(0 == strcmp(pHSRScanner->m_aryFactor[0],"<CTOR>"))
		{
			m_enmType = CCTorElement::CTOR_T;

			if(pVal = pHSRScanner->Val("layer")) (*m_pstrLayer) = pVal;
			if(pVal = pHSRScanner->Val("pt1"))
			{
				sscanf(pVal,"%lf,%lf,%lf",&(m_pt[0].x),&(m_pt[0].y),&(m_pt[0].z));
			}
			if(pVal = pHSRScanner->Val("pt2"))
			{
				sscanf(pVal,"%lf,%lf,%lf",&(m_pt[1].x),&(m_pt[1].y),&(m_pt[1].z));
			}
			
			if(pVal = pHSRScanner->Val("origin pt"))
			{
				sscanf(pVal,"%lf,%lf,%lf",&(m_ptOrigin.x),&(m_ptOrigin.y),&(m_ptOrigin.z));
			}
			
			if(pVal = pHSRScanner->Val("vec"))
			{
				sscanf(pVal,"%lf,%lf,%lf",&(m_vecNorm.dx),&(m_vecNorm.dy),&(m_vecNorm.dz));
			}

			if(pVal = pHSRScanner->Val("tube radius")) m_nTubeRadius = ATOF_T(pVal);

			bRet = true;
		}
		else if(0 == strcmp(pHSRScanner->m_aryFactor[0],"<TORS_EQP>"))
		{
			m_enmType = CCTorElement::EQP_TORS_T;

			m_ptOrigin.x= ATOF_T(pHSRScanner->m_aryFactor[1]);
			m_ptOrigin.y= ATOF_T(pHSRScanner->m_aryFactor[2]);
			m_ptOrigin.z= ATOF_T(pHSRScanner->m_aryFactor[3]);

			double nOutSideRadius=ATOF_T(pHSRScanner->m_aryFactor[4]);
			m_nTubeRadius =ATOF_T(pHSRScanner->m_aryFactor[5]);

			m_vecNorm.dx= ATOF_T(pHSRScanner->m_aryFactor[6]);
			m_vecNorm.dy= ATOF_T(pHSRScanner->m_aryFactor[7]);
			m_vecNorm.dz= ATOF_T(pHSRScanner->m_aryFactor[8]);

			m_vecAxis.dx= ATOF_T(pHSRScanner->m_aryFactor[9]);
			m_vecAxis.dy= ATOF_T(pHSRScanner->m_aryFactor[10]);
			m_vecAxis.dz= ATOF_T(pHSRScanner->m_aryFactor[11]);

			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(m_vecNorm,alpha,beta);
			
			VECTOR_T vecAxis=CGeometry::RotateAboutXYAxis(m_vecAxis,alpha,beta);
			double nStartAng=CGeometry::GetRotatedAngleInXYPlane(vecAxis);

			m_nStartAng = ATOF_T(pHSRScanner->m_aryFactor[12]);
			m_nSweepAng = ATOF_T(pHSRScanner->m_aryFactor[13]);
			nStartAng= ((m_nStartAng + nStartAng) > 2*PI) ? m_nStartAng + nStartAng - 2*PI : m_nStartAng + nStartAng;
			double nEndAng  = ((nStartAng + m_nSweepAng) > 2*PI) ? nStartAng + m_nSweepAng - 2*PI : nStartAng + m_nSweepAng;

			m_pt[0].x = nOutSideRadius;m_pt[0].y = 0.,m_pt[0].z = 0.;
			m_pt[1].x = nOutSideRadius;m_pt[1].y = 0.,m_pt[1].z = 0.;
			m_pt[0].x = cos(nStartAng)*nOutSideRadius;
			m_pt[0].y = sin(nStartAng)*nOutSideRadius;
			m_pt[1].x = cos(nEndAng)*nOutSideRadius;
			m_pt[1].y = sin(nEndAng)*nOutSideRadius;

			m_pt[0] = CGeometry::RotateAboutYXAxis(m_pt[0],-beta,-alpha);
			m_pt[0].x += m_ptOrigin.x;m_pt[0].y += m_ptOrigin.y;m_pt[0].z += m_ptOrigin.z;
			m_pt[1] = CGeometry::RotateAboutYXAxis(m_pt[1],-beta,-alpha);
			m_pt[1].x += m_ptOrigin.x;m_pt[1].y += m_ptOrigin.y;m_pt[1].z += m_ptOrigin.z;
		}
	}

	return bRet;
}
#else
/**
	@brief	ParseLine:
	@author	humkyung
	@return bool 
 */
bool CCTorElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pPreFormatParse is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;
		if(0 == STRCMP_T((*pHSRScanner->m_aryFactor)[0].c_str(),_T("<CTOR>")))
		{
			m_enmType = CCTorElement::CTOR_T;

			m_pt[0].x = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
			m_pt[0].y = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
			m_pt[0].z = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
			m_pt[1].x = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
			m_pt[1].y = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
			m_pt[1].z = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
			
			m_ptOrigin.x= ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());
			m_ptOrigin.y= ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());
			m_ptOrigin.z= ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str());
			
			m_vecNorm.dx= ATOF_T((*pHSRScanner->m_aryFactor)[10].c_str());
			m_vecNorm.dy= ATOF_T((*pHSRScanner->m_aryFactor)[11].c_str());
			m_vecNorm.dz= ATOF_T((*pHSRScanner->m_aryFactor)[12].c_str());
			
			m_nTubeRadius = ATOF_T((*pHSRScanner->m_aryFactor)[13].c_str());

			bRet = true;
		}
		else if(0 == STRCMP_T((*pHSRScanner->m_aryFactor)[0].c_str(),_T("TORUS")))
		{
			m_enmType = CCTorElement::CTOR_T;

			m_ptOrigin.x= ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
			m_ptOrigin.y= ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
			m_ptOrigin.z= ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());

			double nOutSideRadius=ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
			m_nTubeRadius =ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());

			m_vecNorm.dx= ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());
			m_vecNorm.dy= ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());
			m_vecNorm.dz= ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str());
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(m_vecNorm,alpha,beta);

			if(pHSRScanner->m_nFactor > 10)
			{
				SetColorString((*pHSRScanner->m_aryFactor)[10]);
			}

			m_nStartAng = 0;
			m_nSweepAng = 2*PI;
			
			m_pt[0].x = nOutSideRadius;m_pt[0].y = 0.,m_pt[0].z = 0.;
			m_pt[1].x = nOutSideRadius;m_pt[1].y = 0.,m_pt[1].z = 0.;
			m_pt[0].x = cos(m_nStartAng)*nOutSideRadius;
			m_pt[0].y = sin(m_nStartAng)*nOutSideRadius;
			m_pt[1].x = cos(m_nSweepAng)*nOutSideRadius;
			m_pt[1].y = sin(m_nSweepAng)*nOutSideRadius;

			m_pt[0] = CGeometry::RotateAboutYXAxis(m_pt[0],-beta,-alpha);
			m_pt[0].x += m_ptOrigin.x;m_pt[0].y += m_ptOrigin.y;m_pt[0].z += m_ptOrigin.z;
			m_pt[1] = CGeometry::RotateAboutYXAxis(m_pt[1],-beta,-alpha);
			m_pt[1].x += m_ptOrigin.x;m_pt[1].y += m_ptOrigin.y;m_pt[1].z += m_ptOrigin.z;

			bRet = true;
		}
		else if(0 == STRCMP_T((*pHSRScanner->m_aryFactor)[0].c_str(),_T("TORS_EQP")))
		{
			m_enmType = CCTorElement::EQP_TORS_T;

			m_ptOrigin.x= ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
			m_ptOrigin.y= ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
			m_ptOrigin.z= ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());

			double nOutSideRadius=ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
			m_nTubeRadius =ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());

			m_vecNorm.dx= ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
			m_vecNorm.dy= ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());
			m_vecNorm.dz= ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());

			m_vecAxis.dx= ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str());
			m_vecAxis.dy= ATOF_T((*pHSRScanner->m_aryFactor)[10].c_str());
			m_vecAxis.dz= ATOF_T((*pHSRScanner->m_aryFactor)[11].c_str());

			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(m_vecNorm,alpha,beta);
			
			VECTOR_T vecAxis=CGeometry::RotateAboutXYAxis(m_vecAxis,alpha,beta);
			double nStartAng=CGeometry::GetRotatedAngleInXYPlane(vecAxis);

			m_nStartAng = ATOF_T((*pHSRScanner->m_aryFactor)[12].c_str());
			m_nSweepAng = ATOF_T((*pHSRScanner->m_aryFactor)[13].c_str());
			nStartAng= ((m_nStartAng + nStartAng) > 2*PI) ? m_nStartAng + nStartAng - 2*PI : m_nStartAng + nStartAng;
			double nEndAng  = ((nStartAng + m_nSweepAng) > 2*PI) ? nStartAng + m_nSweepAng - 2*PI : nStartAng + m_nSweepAng;

			if(pHSRScanner->m_nFactor > 14)
			{
				SetColorString((*pHSRScanner->m_aryFactor)[14]);
			}

			m_pt[0].x = nOutSideRadius;m_pt[0].y = 0.,m_pt[0].z = 0.;
			m_pt[1].x = nOutSideRadius;m_pt[1].y = 0.,m_pt[1].z = 0.;
			m_pt[0].x = cos(nStartAng)*nOutSideRadius;
			m_pt[0].y = sin(nStartAng)*nOutSideRadius;
			m_pt[1].x = cos(nEndAng)*nOutSideRadius;
			m_pt[1].y = sin(nEndAng)*nOutSideRadius;

			m_pt[0] = CGeometry::RotateAboutYXAxis(m_pt[0],-beta,-alpha);
			m_pt[0].x += m_ptOrigin.x;m_pt[0].y += m_ptOrigin.y;m_pt[0].z += m_ptOrigin.z;
			m_pt[1] = CGeometry::RotateAboutYXAxis(m_pt[1],-beta,-alpha);
			m_pt[1].x += m_ptOrigin.x;m_pt[1].y += m_ptOrigin.y;m_pt[1].z += m_ptOrigin.z;

			bRet = true;
		}
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
bool CCTorElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_pt[0].x >= volume.minx()) && (m_pt[0].x <= volume.maxx()) &&
		(m_pt[0].y >= volume.miny()) && (m_pt[0].y <= volume.maxy()) &&
		(m_pt[0].z >= volume.minz()) && (m_pt[0].z <= volume.maxz())) return true;
	
	if(	(m_pt[1].x >= volume.minx()) && (m_pt[1].x <= volume.maxx()) &&
		(m_pt[1].y >= volume.miny()) && (m_pt[1].y <= volume.maxy()) &&
		(m_pt[1].z >= volume.minz()) && (m_pt[1].z <= volume.maxz())) return true;
		
	return false;
}

//	parameter	:
//	description	: create face
//	remarks		:
//	returns		:
CHSRFace* CCTorElement::CreateFace(CHSRViewFormat* pVIEWFormat,long& nID)
{
	CHSRFace* pRet=NULL;

	if(CCTorElement::CTOR_T == m_enmType)
	{
		pRet = CreateCTORFace(pVIEWFormat,nID);
	}
	else if(CCTorElement::EQP_TORS_T == m_enmType)
	{
		pRet = CreateEQP_TORSFace(pVIEWFormat,nID);
	}

	return pRet;
}

/**	
	@brief	create face

	@author	humkyung

	@param	pVIEWFormat	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@return	PFACE	
*/
CHSRFace* CCTorElement::CreateCTORFace(CHSRViewFormat* pVIEWFormat,long& nID)
{
	assert(pVIEWFormat && "pVIEWFormat is NULL");
	static POINT_T ptEDGE[2][64];
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pVIEWFormat)
	{
		POINT_T ptOrigin[3]={0.,};
		ptOrigin[0] = pVIEWFormat->MODEL2VIEW(m_ptOrigin);

		POINT_T pt[4]={0.,};
		pt[0] = pVIEWFormat->MODEL2VIEW(m_pt[0]);
		pt[1] = pVIEWFormat->MODEL2VIEW(m_pt[1]);
		
		double nTubeRadius=pVIEWFormat->MODEL2VIEW(m_nTubeRadius);

		CVect vec[4];
		vec[0].dx = pt[0].x - ptOrigin[0].x;vec[0].dy = pt[0].y - ptOrigin[0].y;vec[0].dz = pt[0].z - ptOrigin[0].z;
		vec[1].dx = pt[1].x - ptOrigin[0].x;vec[1].dy = pt[1].y - ptOrigin[0].y;vec[1].dz = pt[1].z - ptOrigin[0].z;
		
		CVect vecZ(0.,0.,1.);
		CVect vecNorm = pVIEWFormat->MODEL2VIEW(m_vecNorm);
		if(!CMath::NormalizeVector(vecNorm)) return NULL;
	
		double nDot=CMath::DotProduct(vecNorm,vecZ);
		if(fabs(nDot) < 0.001)
		{	/// parallel to Z axis
			ptEDGE[0][0].x = pt[0].x + vecNorm.dx*nTubeRadius;
			ptEDGE[0][0].y = pt[0].y + vecNorm.dy*nTubeRadius;
			ptEDGE[0][0].z = pt[0].z + vecNorm.dz*nTubeRadius;
			ptEDGE[0][1].x = pt[0].x - vecNorm.dx*nTubeRadius;
			ptEDGE[0][1].y = pt[0].y - vecNorm.dy*nTubeRadius;
			ptEDGE[0][1].z = pt[0].z - vecNorm.dz*nTubeRadius;
			ptEDGE[0][2].x = pt[1].x - vecNorm.dx*nTubeRadius;
			ptEDGE[0][2].y = pt[1].y - vecNorm.dy*nTubeRadius;
			ptEDGE[0][2].z = pt[1].z - vecNorm.dz*nTubeRadius;
			ptEDGE[0][3].x = pt[1].x + vecNorm.dx*nTubeRadius;
			ptEDGE[0][3].y = pt[1].y + vecNorm.dy*nTubeRadius;
			ptEDGE[0][3].z = pt[1].z + vecNorm.dz*nTubeRadius;
			ptEDGE[0][4] = ptEDGE[0][0];

			if(pRet=CHSRFace::CreateFace(5,ptEDGE[0]))
			{
				pRet->type   = CTOR;
				pRet->id     = nID++;
				pRet->ele    = HSR_POLYGON_SHAPE;
				pRet->m_enmLayer = GetHSRLayer();
				pRet->SetColor(*m_pstrColor);
				
				pHSRApp->AppendFace(NULL,pRet);
			}

			CVect vecRotate = vecNorm * vec[0];
			if(!vecRotate.Normalize()) return NULL;
			double nDot=vecRotate.Dot(vecZ);
			if(fabs(nDot) > 0.001)
			{
				if(pRet = CHSRFace::CreateCircleFace(pt[0],nTubeRadius,vecRotate,m_nSegments))
				{
					pRet->type = CTOR;
					pRet->id   = nID++;
					pRet->ele  = HSR_CIRCLE_SHAPE;
					pRet->ptCenter = pt[0];
					pRet->radius   = nTubeRadius;
					pRet->m_enmLayer = GetHSRLayer();
					pRet->SetColor(*m_pstrColor);

					pHSRApp->AppendFace(NULL,pRet);
				}
			}

			vecRotate = vecNorm * vec[1];
			if(!vecRotate.Normalize()) return NULL;
			nDot=vecRotate.Dot(vecZ);
			if(fabs(nDot) > 0.001)
			{
				if(pRet = CHSRFace::CreateCircleFace(pt[1],nTubeRadius,vecRotate,m_nSegments))
				{
					pRet->type = CTOR;
					pRet->id   = nID++;
					pRet->ele  = HSR_CIRCLE_SHAPE;
					pRet->ptCenter = pt[1];
					pRet->radius   = nTubeRadius;
					pRet->m_enmLayer = GetHSRLayer();
					pRet->SetColor(*m_pstrColor);
					
					pHSRApp->AppendFace(NULL,pRet);
				}
			}
		}
		else if(fabs(fabs(nDot) - 1.) < 0.001)
		{ /// place in X-Y plane
			vec[0].dx = pt[0].x - ptOrigin[0].x;
			vec[0].dy = pt[0].y - ptOrigin[0].y;
			vec[0].dz = pt[0].z - ptOrigin[0].z;
			if(!vec[0].Normalize()) return NULL;

			pt[2].x = pt[0].x + vec[0].dx*nTubeRadius;
			pt[2].y = pt[0].y + vec[0].dy*nTubeRadius;
			pt[2].z = pt[0].z + vec[0].dz*nTubeRadius;
			pt[3].x = pt[0].x - vec[0].dx*nTubeRadius;
			pt[3].y = pt[0].y - vec[0].dy*nTubeRadius;
			pt[3].z = pt[0].z - vec[0].dz*nTubeRadius;
			
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
			vec[0] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
			vec[1] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
			
			double nSweepAngle[3]={0,};
			nSweepAngle[0] = CGeometry::GetRotatedAngleInXYPlane(vec[0]);
			nSweepAngle[1] = CGeometry::GetRotatedAngleInXYPlane(vec[1]);
			nSweepAngle[2] = RAD2DEG(nSweepAngle[1] - nSweepAngle[0]);
			if(nSweepAngle[2] < 0.) nSweepAngle[2] += 360.;
			
			ptOrigin[0].z += nTubeRadius;
			pt[2].z += nTubeRadius;pt[3].z += nTubeRadius;
			CArc3d::segments(ptEDGE[0],ptOrigin[0],pt[2],vecNorm,nSweepAngle[2],m_nSegments);
			CArc3d::segments(ptEDGE[1],ptOrigin[0],pt[3],vecNorm,nSweepAngle[2],m_nSegments);
			int i=0;
			for(i=0;i < m_nSegments + 1;i++)
			{
				ptEDGE[0][m_nSegments + i + 1] = ptEDGE[1][m_nSegments - i];
			}
			ptEDGE[0][m_nSegments + i + 1] = ptEDGE[0][0];

			if(pRet=CHSRFace::CreateFace(m_nSegments + i + 2,ptEDGE[0]))
			{
				pRet->type   = CTOR;
				pRet->id     = nID++;
				pRet->ele    = HSR_POLYGON_SHAPE;
				pRet->m_enmLayer = GetHSRLayer();
				pRet->SetColor(*m_pstrColor);

				pHSRApp->AppendFace(NULL,pRet);
			}
		}
		else
		{
			double dx=pt[0].x - ptOrigin[0].x;
			double dy=pt[0].y - ptOrigin[0].y;
			double dz=pt[0].z - ptOrigin[0].z;
			double nTorusRadius=sqrt(dx*dx + dy*dy + dz*dz);
	
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
			vec[0] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
			vec[1] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
		
			double nStartAngle = CGeometry::GetRotatedAngleInXYPlane(vec[0]);
			double nEndAngle   = CGeometry::GetRotatedAngleInXYPlane(vec[1]);
			double nSweepAngle = nEndAngle - nStartAngle;
			if(nSweepAngle < 0.) nSweepAngle += 2*PI;
			double nStepAngle  = nSweepAngle/m_nSegments;
	
			CVect vecRotate,vecDir;
			double nAngle=nStartAngle;
			POINT_T ptCenter={0};
			int i=0;
			for(i=0;i < m_nSegments;i++,nAngle+= nStepAngle)
			{
				ptCenter = CArc3d::on(ptOrigin[0],vecNorm,nTorusRadius,nAngle);
				
				vecDir.dx = ptCenter.x - ptOrigin[0].x;
				vecDir.dy = ptCenter.y - ptOrigin[0].y;
				vecDir.dz = ptCenter.z - ptOrigin[0].z;
				vecRotate = vecNorm * vecDir;
				vec[3] = vecRotate.Perpendicular();
				if(!vec[3].Normalize()) return NULL;
				
				ptEDGE[0][i].x = ptCenter.x + vec[3].dx*nTubeRadius;
				ptEDGE[0][i].y = ptCenter.y + vec[3].dy*nTubeRadius;
				ptEDGE[0][i].z = ptCenter.z + vec[3].dz*nTubeRadius;
				ptEDGE[1][i].x = ptCenter.x - vec[3].dx*nTubeRadius;
				ptEDGE[1][i].y = ptCenter.y - vec[3].dy*nTubeRadius;
				ptEDGE[1][i].z = ptCenter.z - vec[3].dz*nTubeRadius;
			}
			vecDir.dx = pt[1].x - ptOrigin[0].x;
			vecDir.dy = pt[1].y - ptOrigin[0].y;
			vecDir.dz = pt[1].z - ptOrigin[0].z;
			vecRotate = vecNorm * vecDir;
			vec[3] = vecRotate.Perpendicular();
			if(!vec[3].Normalize()) return NULL;
			
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
			
			if(pRet=CHSRFace::CreateFace(m_nSegments + i + 2,ptEDGE[0]))
			{
				pRet->type   = CTOR;
				pRet->id     = nID++;
				pRet->ele    = HSR_POLYGON_SHAPE;
				pRet->m_enmLayer = GetHSRLayer();
				pRet->SetColor(*m_pstrColor);

				pHSRApp->AppendFace(NULL,pRet);
			}

			vec[0].dx = pt[0].x - ptOrigin[0].x;
			vec[0].dy = pt[0].y - ptOrigin[0].y;
			vec[0].dz = pt[0].z - ptOrigin[0].z;
			vecRotate = vecNorm * vec[0];
			if(!vecRotate.Normalize()) return NULL;
			double nDot=vecRotate.Dot(vecZ);
			if(fabs(nDot) > 0.001)
			{
				if(pRet = CHSRFace::CreateCircleFace(pt[0],nTubeRadius,vecRotate,m_nSegments))
				{
					pRet->type   = CTOR;
					pRet->id     = nID++;
					pRet->ele    = HSR_POLYGON_SHAPE;
					pRet->m_enmLayer = GetHSRLayer();
					pRet->SetColor(*m_pstrColor);

					pHSRApp->AppendFace(NULL,pRet);
				}
			}
			
			vec[1].dx = pt[0].x - ptOrigin[0].x;
			vec[1].dy = pt[0].y - ptOrigin[0].y;
			vec[1].dz = pt[0].z - ptOrigin[0].z;
			vecRotate = vecNorm * vec[0];
			if(!vecRotate.Normalize()) return NULL;
			nDot=vecRotate.Dot(vecZ);
			if(fabs(nDot) > 0.001)
			{
				if(pRet = CHSRFace::CreateCircleFace(pt[1],nTubeRadius,vecRotate,m_nSegments))
				{
					pRet->type   = CTOR;
					pRet->id     = nID++;
					pRet->ele    = HSR_POLYGON_SHAPE;
					pRet->m_enmLayer = GetHSRLayer();
					pRet->SetColor(*m_pstrColor);

					pHSRApp->AppendFace(NULL,pRet);
				}
			}
		}
	}

	return NULL;
}

/**	
	@brief	create torus for equipment

	@author	humkyung

	@param	pVIEWFormat	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@remarks\n
	;[2003.08.01] - if sweep angle is larger than PI, bottom up the normal vector.

	@return	PFACE	
*/
CHSRFace* CCTorElement::CreateEQP_TORSFace(CHSRViewFormat* pVIEWFormat,long& nID)
{
	assert(pVIEWFormat && "pVIEWFormat is NULL");
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pVIEWFormat)
	{
		//--> convert model points to view points
		POINT_T ptOrigin=pVIEWFormat->MODEL2VIEW(m_ptOrigin);

		POINT_T pt[2]={0.,};
		pt[0] = pVIEWFormat->MODEL2VIEW(m_pt[0]);
		pt[1] = pVIEWFormat->MODEL2VIEW(m_pt[1]);
		
		double nTubeRadius =pVIEWFormat->MODEL2VIEW(m_nTubeRadius);

		CVect vec[4],vecNorm;
		vec[0].dx = pt[0].x - ptOrigin.x;vec[0].dy = pt[0].y - ptOrigin.y;vec[0].dz = pt[0].z - ptOrigin.z;
		vec[0].Normalize();
		vec[1].dx = pt[1].x - ptOrigin.x;vec[1].dy = pt[1].y - ptOrigin.y;vec[1].dz = pt[1].z - ptOrigin.z;
		vec[1].Normalize();
		
		VECTOR_T vecZ={0,0,1};
		CMath::GetCrossProduct(vecNorm,vec[0],vec[1]);
		///CVect vecNorm  = pVIEWFormat->MODEL2VIEW(m_vecNorm);
		vecNorm.Normalize();
		if(m_nSweepAng > PI) vecNorm = -vecNorm;
		///<--

		POINT_T ptEDGE[2][64];
		double nDot=CMath::DotProduct(vecNorm,vecZ);
		if(fabs(nDot) < 0.001)
		{	// parallel to Z axis
			ptEDGE[0][0].x = pt[0].x + vecNorm.dx*nTubeRadius;
			ptEDGE[0][0].y = pt[0].y + vecNorm.dy*nTubeRadius;
			ptEDGE[0][0].z = pt[0].z + vecNorm.dz*nTubeRadius;
			ptEDGE[0][1].x = pt[0].x - vecNorm.dx*nTubeRadius;
			ptEDGE[0][1].y = pt[0].y - vecNorm.dy*nTubeRadius;
			ptEDGE[0][1].z = pt[0].z - vecNorm.dz*nTubeRadius;
			ptEDGE[0][2].x = pt[1].x - vecNorm.dx*nTubeRadius;
			ptEDGE[0][2].y = pt[1].y - vecNorm.dy*nTubeRadius;
			ptEDGE[0][2].z = pt[1].z - vecNorm.dz*nTubeRadius;
			ptEDGE[0][3].x = pt[1].x + vecNorm.dx*nTubeRadius;
			ptEDGE[0][3].y = pt[1].y + vecNorm.dy*nTubeRadius;
			ptEDGE[0][3].z = pt[1].z + vecNorm.dz*nTubeRadius;
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
		}
		else if(fabs(fabs(nDot) - 1.) < 0.001)
		{ // place in X-Y plane
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
			vec[2] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
			vec[3] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
			
			double nSweepAngle[3]={0,};
			nSweepAngle[0] = CGeometry::GetRotatedAngleInXYPlane(vec[2]);
			nSweepAngle[1] = CGeometry::GetRotatedAngleInXYPlane(vec[3]);
			
			///POINT_T ptEDGE[2][64];
			nSweepAngle[2] = RAD2DEG(nSweepAngle[1] - nSweepAngle[0]);
			if(nSweepAngle[2] < 0.) nSweepAngle[2] += 360.;
			CArc3d::segments(ptEDGE[0],ptOrigin,pt[0],vecNorm,nSweepAngle[2],m_nSegments);
			
			VECTOR_T vecDir={0};
			int i=0;
			for(i=0;i < m_nSegments + 1;i++)
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
			for(i=0;i < m_nSegments + 1;i++)
			{
				ptEDGE[0][m_nSegments + i + 1] = ptEDGE[1][m_nSegments - i];
			}
			ptEDGE[0][m_nSegments + i + 1] = ptEDGE[0][0];

			if(pRet=CHSRFace::CreateFace(m_nSegments + i + 2,ptEDGE[0]))
			{
				pRet->type   = FACE_T;
				pRet->id     = nID++;
				pRet->ele    = HSR_POLYGON_SHAPE;
				pRet->m_enmLayer = GetHSRLayer();
				pRet->SetColor(*m_pstrColor);

				pHSRApp->AppendFace(NULL,pRet);
			}
		}
		else
		{	/// SLOPE.
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
			int i=0;
			for(i=0;i < m_nSegments;i++)
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
			
			if(fabs(vecRotate.dz - vecZ.dz)	< 0.005)
			{
				vec[3] = vecNorm;
				CMath::NormalizeVector(vec[3]);
			}
			else
			{
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
			
			if(pRet=CHSRFace::CreateFace(m_nSegments + i + 2,ptEDGE[0]))
			{
				pRet->id     = nID++;
				pRet->type   = FACE_T;
				pRet->ele   = HSR_POLYGON_SHAPE;
				pRet->m_enmLayer = GetHSRLayer();
				pRet->SetColor(*m_pstrColor);

				pHSRApp->AppendFace(NULL,pRet);
			}
		}
	}
	return NULL;
}

/**
 * GetHSRLayer:
 *
 * @return HSR_LAYER_E 
 */
HSR_LAYER_E CCTorElement::GetHSRLayer()
{
	if((*m_pstrLayer) == _T("PIPE"))
	{
		return LAYER_PIPE_OTHER;
	}
	else if((*m_pstrLayer) == _T("EQP"))
	{
		return LAYER_EQUI;
	}
	else
	{
		return LAYER_OTHER;
	}
}
