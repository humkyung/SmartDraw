#include <math.h>
#include <memory.h>
#include <graphics\geometry.h>
#include <graphics\circle3d.h>
#include <graphics/enum.h>

/**	\brief
*/
CCircle3d::CCircle3d(){
	m_nId = ENM_ITK_CIRCLE;

	ptOrigin.x = 0.;
	ptOrigin.y = 0.;
	ptOrigin.z = 0.;
	vecNorm.dx = 0;
	vecNorm.dy = 0;
	vecNorm.dz = 1;
	nRadius    = 1.;
}

/**	\brief
*/
CCircle3d::CCircle3d(const CIRCLE_T& circle){
	m_nId = ENM_ITK_CIRCLE;

	ptOrigin.x= circle.ptOrigin.x;
	ptOrigin.y= circle.ptOrigin.y;
	ptOrigin.z= circle.ptOrigin.z;
	vecNorm.dx= circle.vecNorm.dx;
	vecNorm.dy= circle.vecNorm.dy;
	vecNorm.dz= circle.vecNorm.dz;
	nRadius   = circle.nRadius;
}

/**	\brief
*/
CCircle3d::CCircle3d(const CCircle3d& circle){
	m_nId = ENM_ITK_CIRCLE;

	ptOrigin.x= circle.ptOrigin.x;
	ptOrigin.y= circle.ptOrigin.y;
	ptOrigin.z= circle.ptOrigin.z;
	vecNorm.dx= circle.vecNorm.dx;
	vecNorm.dy= circle.vecNorm.dy;
	vecNorm.dz= circle.vecNorm.dz;
	nRadius   = circle.nRadius;
}

/**	\brief
*/
CCircle3d::CCircle3d(double x,double y,double z,double radius){
	m_nId = ENM_ITK_CIRCLE;

	ptOrigin.x= x;
	ptOrigin.y= y;
	ptOrigin.z= z;
	vecNorm.dx= 0;
	vecNorm.dy= 0;
	vecNorm.dz= 1;
	nRadius   = radius;
}

/**	\brief
*/
CCircle3d::~CCircle3d(){
}

/**	\brief	The CCircle3d::GetOrigin function


	\return	POINT_T	
*/
POINT_T CCircle3d::GetOrigin(){
    	return ptOrigin;
}

/**	\brief	The CCircle3d::GetRadius function


	\return	double	
*/
double CCircle3d::GetRadius(){
    	return nRadius;
}

/**	\brief	The CCircle3d::SetOrigin function

	\param	x	a parameter of type const double
	\param	y	a parameter of type const double
	\param	z	a parameter of type const double

	\return	void	
*/
void CCircle3d::SetOrigin(const double x,const double y,const double z){
    	ptOrigin.x = x;
	ptOrigin.y = y;
	ptOrigin.z = z;
}

/**	\brief	The CCircle3d::SetOrigin function

	\param	ptOrigin	a parameter of type POINT_T

	\return	void	
*/
void CCircle3d::SetOrigin(POINT_T ptOrigin){
    	ptOrigin.x = ptOrigin.x;
	ptOrigin.y = ptOrigin.y;
	ptOrigin.z = ptOrigin.z;
}

/**	\brief	The CCircle3d::SetRadius function

	\param	nRadius	a parameter of type const double

	\return	void	
*/
void CCircle3d::SetRadius(const double& radius){
    	nRadius = radius;
}

/**	\brief	The CCircle3d::CreateSegments function


	\return	void	
*/
void CCircle3d::CreateSegments(){
	double nAngle=0.,nStep = 2*PI/(CCurve3d::NUM_OF_SEGMENTS);
	double c=cos(nStep);
	double s=sin(nStep);
	CCurve3d::m_pt[0].x = nRadius;
	CCurve3d::m_pt[0].y = 0.;
	CCurve3d::m_pt[0].z = 0.;
	int i = 0;
	for(i = 1;i < (CCurve3d::NUM_OF_SEGMENTS);i++){
		CCurve3d::m_pt[i].x = CCurve3d::m_pt[i-1].x*c - CCurve3d::m_pt[i-1].y*s;
		CCurve3d::m_pt[i].y = CCurve3d::m_pt[i-1].x*s + CCurve3d::m_pt[i-1].y*c;
		CCurve3d::m_pt[i].z = 0.;
		nAngle += nStep;
	}
	CCurve3d::m_pt[i].x = CCurve3d::m_pt[0].x;
	CCurve3d::m_pt[i].y = CCurve3d::m_pt[0].y;
	CCurve3d::m_pt[i].z = CCurve3d::m_pt[0].z;

	double alpha=0.,beta=0.;
	CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
	for(i = 0;i < (CCurve3d::NUM_OF_SEGMENTS) + 1;i++){
		CCurve3d::m_pt[i] = CGeometry::RotateAboutYXAxis(CCurve3d::m_pt[i],-beta,-alpha);
			
		CCurve3d::m_pt[i].x += ptOrigin.x;
		CCurve3d::m_pt[i].y += ptOrigin.y;
		CCurve3d::m_pt[i].z += ptOrigin.z;
	}
}

/**	\brief	The CCircle3D::Sweep function

	\param	vecAxis	a parameter of type VECTOR_T
	\param	nAngle	a parameter of type double
	\param	nParts	a parameter of type int

	\return	PPOINT_T	
*/
PPOINT_T CCircle3d::Sweep(VECTOR_T vecAxis,double nAngle,int nParts){
	int nPoints = GetNumOfPoints();

	VECTOR_T vecTemp={0,};
	double alpha=0.,beta=0.;
	CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
	int i = 0;
	for(i = 0;i < nPoints;i++){
		CCurve3d::m_pt[i] = CGeometry::RotateAboutXYAxis(CCurve3d::m_pt[i],alpha,beta);
	}

	double nStartAngle=0.,nStepAngle = nAngle/nParts;
	int j = 0;
	for(j = 1;j < nParts + 1;j++){
		for(int i = 0;i < nPoints;i++){
			CCurve3d::m_pt[j*nPoints + i].x = CCurve3d::m_pt[i].x*cos(nStartAngle) - CCurve3d::m_pt[i].y*sin(nStartAngle);
			CCurve3d::m_pt[j*nPoints + i].y = CCurve3d::m_pt[i].x*sin(nStartAngle) + CCurve3d::m_pt[i].y*cos(nStartAngle);
			CCurve3d::m_pt[j*nPoints + i].z = CCurve3d::m_pt[i].z;
		}
		nStartAngle += nStepAngle;
	}
	for(i = 0;i < nPoints;i++){
		CCurve3d::m_pt[j*nPoints + i].x = CCurve3d::m_pt[i].x*cos(nAngle) - CCurve3d::m_pt[i].y*sin(nAngle);
		CCurve3d::m_pt[j*nPoints + i].y = CCurve3d::m_pt[i].x*sin(nAngle) + CCurve3d::m_pt[i].y*cos(nAngle);
		CCurve3d::m_pt[j*nPoints + i].z = CCurve3d::m_pt[i].z;
	}
		

	for(j = 0;j < nParts + 1;j++){
		for(i = 0;i < nPoints;i++){
			CCurve3d::m_pt[j*nPoints + i] = CGeometry::RotateAboutYXAxis(CCurve3d::m_pt[j*nPoints + i],-beta,-alpha);
		}
	}

	return (CCurve3d::m_pt);
}
