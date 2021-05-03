#include <string.h>
#include <math.h>
#include <ITKDef.h>
#include <graphics\geometry.h>
#include <graphics\ellipse3d.h>

/**	\brief
*/
CEllipse3d::CEllipse3d(){
	ptOrigin.x = 0.;
	ptOrigin.y = 0.;
	ptOrigin.z = 0.;

	nAxis[0] = 1.;
	nAxis[1] = 1.;
	nStartAngle= 0.;
	nSweepAngle= 360.;
	
	vecNorm.dx = 0.;
	vecNorm.dy = 0.;
	vecNorm.dz = 1.;
}

/**	\brief
*/
CEllipse3d::CEllipse3d(const ELLIPSE_T& ellipse){
	ptOrigin = ellipse.ptOrigin;
	nAxis[0] = ellipse.nAxis[0];
	nAxis[1] = ellipse.nAxis[1];
	nStartAngle = ellipse.nStartAngle;
	nSweepAngle = ellipse.nSweepAngle;
	vecNorm = ellipse.vecNorm;
	nRotate = ellipse.nRotate;
}

/**	\brief
*/
CEllipse3d::~CEllipse3d(){
}

/**	\brief	The CEllipse3d::CreateSegments function


	\return	void	
*/
void CEllipse3d::CreateSegments(){
	double nStartAng = DEG2RAD(nStartAngle);
	double nSweepAng = DEG2RAD(nSweepAngle);
	double nStep=nSweepAng/CCurve3d::NUM_OF_SEGMENTS;
	double c=cos(nStep);
	double s=sin(nStep);
	double ab=1.,ba=1.;
	
	if((0. != nAxis[0]) && (0. != nAxis[1])){
		ab=nAxis[0]/nAxis[1];
		ba=nAxis[1]/nAxis[0];
	}
	CCurve3d::m_pt[0].x = nAxis[0]*cos(nStartAng);
	CCurve3d::m_pt[0].y = nAxis[1]*sin(nStartAng);
	CCurve3d::m_pt[0].z = 0.;
	int i = 0;
	for(i = 1;i < (CCurve3d::NUM_OF_SEGMENTS);i++){
		CCurve3d::m_pt[i].x = CCurve3d::m_pt[i-1].x*c - ab*CCurve3d::m_pt[i-1].y*s;
		CCurve3d::m_pt[i].y = ba*CCurve3d::m_pt[i-1].x*s + CCurve3d::m_pt[i-1].y*c;
		CCurve3d::m_pt[i].z = 0.;
	}
	CCurve3d::m_pt[i].x = nAxis[0]*cos(nStartAng + nSweepAng);
	CCurve3d::m_pt[i].y = nAxis[1]*sin(nStartAng + nSweepAng);
	CCurve3d::m_pt[i].z = 0.;

	double nRotateAng=DEG2RAD(nRotate);
	c=cos(nRotateAng);
	s=sin(nRotateAng);
	for(i = 0;i < CCurve3d::NUM_OF_SEGMENTS + 1;i++){
		double x=CCurve3d::m_pt[i].x*c - CCurve3d::m_pt[i].y*s;
		double y=CCurve3d::m_pt[i].x*s + CCurve3d::m_pt[i].y*c;
		CCurve3d::m_pt[i].x = x;
		CCurve3d::m_pt[i].y = y;
	}

	double alpha=0.,beta=0;
	CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
	for(i = 0;i < CCurve3d::NUM_OF_SEGMENTS + 1;i++){
		CCurve3d::m_pt[i] = CGeometry::RotateAboutYXAxis(CCurve3d::m_pt[i],-beta,-alpha);
		CCurve3d::m_pt[i].x += ptOrigin.x;
		CCurve3d::m_pt[i].y += ptOrigin.y;
		CCurve3d::m_pt[i].z += ptOrigin.z;
	}
}
