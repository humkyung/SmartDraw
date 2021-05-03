#include "KPlane.h"

SQVECTOR KPlane::XAXIS={1.,0.,0.};
SQVECTOR KPlane::YAXIS={0.,1.,0.};
SQVECTOR KPlane::ZAXIS={0.,0.,1.};

KPlane::KPlane(){
    	m_vecXAxis.dx = 1.;
	m_vecXAxis.dy = 0.;
	m_vecXAxis.dz = 0.;
	m_nD          = 0.;

	m_vecYAxis.dx = 0.;
	m_vecYAxis.dy = 1.;
	m_vecYAxis.dz = 0.;

	m_vecZAxis.dx = 0.;
	m_vecZAxis.dy = 0.;
	m_vecZAxis.dz = 1.;
}

KPlane::~KPlane(){
}

void KPlane::RotateAboutZAxis(double alpha){
}

void KPlane::RotateAboutYAxis(double beta){
}

void KPlane::RotateAboutXAxis(double gamma){
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KPlane::GetRotatedAngle(double& alpha,double& beta){
    	SQVECTOR axis={0};
       	
	memcpy(&axis,m_vecNorm,sizeof(SQVECTOR));
    	NormalizeVector(axis);
	double d=sqrt(axis.dy*axis.dy + axis.dz*axis.dz);
	alpha = acos(axis.dz / d);
	beta  = acos(d);
}

//	parameter	:
//	description	: first rotate about y axis by beta angle,
//			: rotate about z axis by alpha angle.
//			: | -1      0            0   | | cos(beta)  0 sin(beta)| | x |
//			: |  0 cos(alpha) -sin(alpha)|*|    0       1     0    |*| y |
//			: |  0 sin(alpha) cos(alpah) | | -sin(beta) 0 cos(beta)| | z |
//	remarks		:
//	returns		:
SQPOINT KPlane::Rotate(SQPOINT pt,double alpha,double beta){
    	SQPOINT ptRet={0};

	ptRet.x = pt.x*cos(beta) + pt.z*sin(beta);
	ptRet.y = pt.x*sin(alpha)*sin(beta) + pt.y*cos(alpha) - pt.z*sin(alpha)*cos(beta);
	ptRet.z = -pt.x*cos(alpha)*sin(beta) + pt.y*sin(alpha) + pt.z*cos(alpha)*cos(beta);

	return ptRet;
}

//	parameter	:
//	description	: Computes plane equation.
//	remarks		: 
//	returns		: 
bool KPlane::ComputePlaneEquation(SQPOINT pt1,SQPOINT pt2,SQPOINT pt3){
	bool bRet = false;

	assert(!((pt1.x == pt2.x) && (pt1.y == pt2.y) && (pt1.z == pt2.z)));
	assert(!((pt1.x == pt3.x) && (pt1.y == pt3.y) && (pt1.z == pt3.z)));
	if(!((pt1.x == pt2.x) && (pt1.y == pt2.y) && (pt1.z == pt2.z)) && 
		!((pt1.x == pt3.x) && (pt1.y == pt3.y) && (pt1.z == pt3.z))){
			
		double ii1= pt2.x - pt1.x,jj1= pt2.y - pt1.y,kk1= pt2.z - pt1.z;
		double ii2= pt3.x - pt1.x,jj2= pt3.y - pt1.y,kk2= pt3.z - pt1.z;
			
		double iicp=0.,jjcp=0.,kkcp=0.;
		GetCrossProduct(ii1,jj1,kk1,ii2,jj2,kk2,&iicp,&jjcp,&kkcp);
		assert(!((iicp == 0.0) && (jjcp == 0.0) && (kkcp == 0.0)));
		if(!((iicp==0.0) && (jjcp==0.0) && (kkcp==0.0))){
			// normalize plane equation 
			NormalizeVector(iicp,jjcp,kkcp,&(m_vecNormal.dx),&(m_vecNormal.dy),&(m_vecNormal.dz));
				
			//* compute D of plane equation
			m_nD = -(m_vecNormal.dx*pt1.x) - (m_vecNormal.dy*pt1.y) - (m_vecNormal.dz*pt1.z); 
				
			bRet = true;
		}
	}

	return bRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool KPlane::IntersectToPlane(SQVECTOR& vector,SQPOINT& point,KPlane& plane){
    	GetCrossProduct(m_vecNormal.dx,m_vecNormal.dy,m_vecNormal.dz,plane.m_vecNormal.dx,plane.m_vecNormal.dy,plane.m_vecNormal.dz,&(vector.dx),&(vector.dy),&(vector.dz));
	assert(!((vector.dx == 0.) && (vector.dy == 0.) && (vector.dz == 0.)));
	bool bRet=false;

	if(!((vector.dx == 0.) && (vector.dy == 0.) && (vector.dz == 0.))){
	    	point.x = (m_vecNormal.dx*plane.m_nD - plane.m_vecNormal.dy*m_nD)/
		    (m_vecNormal.dx*plane.m_vecNormal.dy + m_vecNormal.dy*plane.m_vecNormal.dx);
		point.y = (plane.m_vecNormal.dx*m_nD - m_vecNormal.dx*plane.m_nD)/
		    (m_vecNormal.dx*plane.m_vecNormal.dy + m_vecNormal.dy*plane.m_vecNormal.dx);
		point.z = 0.;

		point.z = -(m_vecNormal.dx*point.x + m_vecNormal.dy*point.y + m_nD)/m_vecNormal.dz;

		bRet = true;
	}

	return bRet;
}
