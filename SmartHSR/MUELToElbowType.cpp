// MUELToElbowType.cpp: implementation of the CMUELToElbowType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MUELToElbowType.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMUELToElbowType::CMUELToElbowType(CHSRScanner* pScanner)
{
	assert(pScanner && "pcanner is NULL");
	bool bRet=false;
	
	if(pScanner)
	{
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

			POINT_T ptMid , ptCross;
			ptMid.x = (m_pt[0].x + m_pt[1].x)*0.5;
			ptMid.y = (m_pt[0].y + m_pt[1].y)*0.5;
			ptMid.z = (m_pt[0].z + m_pt[1].z)*0.5;
			double dx = ptMid.x - m_ptOrigin.x;
			double dy = ptMid.y - m_ptOrigin.y;
			double dz = ptMid.z - m_ptOrigin.z;
			const double L1 = sqrt(dx*dx + dy*dy + dz*dz);
			const double L  = m_nTorusRadius*m_nTorusRadius / L1;
			dx /= L1; dy /= L1; dz /= L1;
			ptCross.x = m_ptOrigin.x + dx*L;
			ptCross.y = m_ptOrigin.y + dy*L;
			ptCross.z = m_ptOrigin.z + dz*L;

			/// convert to elbow format
			pScanner->m_aryFactor->clear();
			pScanner->m_aryFactor->push_back(_T("<ELBO>"));
			TCHAR szBuf[MAX_PATH + 1]={'\0',};
			SPRINTF_T(szBuf , _T("%lf") , m_pt[0].x);	pScanner->m_aryFactor->push_back(szBuf);
			SPRINTF_T(szBuf , _T("%lf") , m_pt[0].y);	pScanner->m_aryFactor->push_back(szBuf);
			SPRINTF_T(szBuf , _T("%lf") , m_pt[0].z);	pScanner->m_aryFactor->push_back(szBuf);
			SPRINTF_T(szBuf , _T("%lf") , m_pt[1].x);	pScanner->m_aryFactor->push_back(szBuf);
			SPRINTF_T(szBuf , _T("%lf") , m_pt[1].y);	pScanner->m_aryFactor->push_back(szBuf);
			SPRINTF_T(szBuf , _T("%lf") , m_pt[1].z);	pScanner->m_aryFactor->push_back(szBuf);
			SPRINTF_T(szBuf , _T("%lf") , ptCross.x);	pScanner->m_aryFactor->push_back(szBuf);
			SPRINTF_T(szBuf , _T("%lf") , ptCross.y);	pScanner->m_aryFactor->push_back(szBuf);
			SPRINTF_T(szBuf , _T("%lf") , ptCross.z);	pScanner->m_aryFactor->push_back(szBuf);
			SPRINTF_T(szBuf , _T("%lf") , m_ptOrigin.x);	pScanner->m_aryFactor->push_back(szBuf);
			SPRINTF_T(szBuf , _T("%lf") , m_ptOrigin.y);	pScanner->m_aryFactor->push_back(szBuf);
			SPRINTF_T(szBuf , _T("%lf") , m_ptOrigin.z);	pScanner->m_aryFactor->push_back(szBuf);
			SPRINTF_T(szBuf , _T("%lf") , m_nTorusRadius);	pScanner->m_aryFactor->push_back(szBuf);
			SPRINTF_T(szBuf , _T("%lf") , m_nTubeRadius);	pScanner->m_aryFactor->push_back(szBuf);
			pScanner->m_nFactor = 15;
		}
	}
}

CMUELToElbowType::~CMUELToElbowType()
{
	
}
