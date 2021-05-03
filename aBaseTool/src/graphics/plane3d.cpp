#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <math\ITKMath.h>
#include <graphics\ITKGeo.h>
#include <graphics\geometry.h>
#include <graphics\plane3d.h>

const VECTOR_T CPlane3d::XAXIS={1.,0.,0.};
const VECTOR_T CPlane3d::YAXIS={0.,1.,0.};
const VECTOR_T CPlane3d::ZAXIS={0.,0.,1.};

/**	\brief
*/
CPlane3d::CPlane3d(){
    	m_vecNorm.dx = 0.;
	m_vecNorm.dy = 0.;
	m_vecNorm.dz = 1.;
	m_nD         = 0.;
}

/**	\brief
*/
CPlane3d::CPlane3d(VECTOR_T& vec){
	memcpy(&m_vecNorm,&vec,sizeof(VECTOR_T));
	m_nD         = 0.;
}

/**	\brief
*/
CPlane3d::CPlane3d(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3){
	ComputePlaneEquation(pt1,pt2,pt3);
}

/**	\brief
*/
CPlane3d::~CPlane3d(){
}

/**	\brief	The CPlane3d::GetNormVector function


	\return	VECTOR_T	
*/
VECTOR_T CPlane3d::GetNormVector(){
	return m_vecNorm;
}

/**	\brief	The CPlane3d::SetNormVector function

	\param	dx	a parameter of type double
	\param	dy	a parameter of type double
	\param	dz	a parameter of type double

	\return	void	
*/
void CPlane3d::SetNormVector(double dx,double dy,double dz){
	m_vecNorm.dx = dx;
	m_vecNorm.dy = dy;
	m_vecNorm.dz = dz;
}

/**	\brief	The CPlane3d::SetNormVector function

	\param	vecNorm	a parameter of type VECTOR_T

	\return	void	
*/
void CPlane3d::SetNormVector(VECTOR_T vecNorm){
	m_vecNorm.dx = vecNorm.dx;
	m_vecNorm.dy = vecNorm.dy;
	m_vecNorm.dz = vecNorm.dz;
}

/**	\brief	The CPlane3d::GetRotatedAngleOfAxis function

	\param	alpha	a parameter of type double&
	\param	beta	a parameter of type double&

	\return	void	
*/
void CPlane3d::GetRotatedAngleOfAxis(double& alpha,double& beta){
	CGeometry::GetRotatedAngleOfAxis(m_vecNorm,alpha,beta);
}

/**	\brief	The CPlane3d::RotateAboutXYAxis function

	\param	pt	a parameter of type POINT_T
	\param	alpha	a parameter of type const double&
	\param	beta	a parameter of type const double&

	\return	POINT_T	
*/
POINT_T CPlane3d::RotateAboutXYAxis(POINT_T pt,const double& alpha,const double& beta){
	return CGeometry::RotateAboutXYAxis(pt,alpha,beta);
}

/**	\brief	The CPlane3d::RotateAboutXYAxis function

	\param	vec	a parameter of type const VECTOR_T&
	\param	alpha	a parameter of type double
	\param	beta	a parameter of type double

	\return	VECTOR_T	
*/
VECTOR_T CPlane3d::RotateAboutXYAxis(const VECTOR_T& vec,double alpha,double beta){
	return CGeometry::RotateAboutXYAxis(vec,alpha,beta);
}

/**	\brief	The CPlane3d::RotateAboutYXAxis function

	\param	pt	a parameter of type POINT_T
	\param	beta	a parameter of type double
	\param	alpha	a parameter of type double

	\return	POINT_T	
*/
POINT_T CPlane3d::RotateAboutYXAxis(POINT_T pt,double beta,double alpha){
	return CGeometry::RotateAboutYXAxis(pt,beta,alpha);
}

/**	\brief	The CPlane3d::RotateAboutYXAxis function

	\param	vec	a parameter of type VECTOR_T&
	\param	beta	a parameter of type double
	\param	alpha	a parameter of type double

	\return	VECTOR_T	
*/
VECTOR_T CPlane3d::RotateAboutYXAxis(VECTOR_T& vec,double beta,double alpha){
	return CGeometry::RotateAboutYXAxis(vec,beta,alpha);
}

/**	\brief	The CPlane3d::GetAngle function

	\param	vec	a parameter of type VECTOR_T&

	\remarks\n
	;vec must be on this plane.
	\return	double	
*/
double CPlane3d::GetAngle(VECTOR_T& vec){
	double alpha=0.,beta=0.;
	
	CGeometry::GetRotatedAngleOfAxis(m_vecNorm,alpha,beta);
	VECTOR_T vecRot=CGeometry::RotateAboutXYAxis(vec,alpha,beta);
	
	POINT_T  pt={0};
	pt.x = vecRot.dx;
	pt.y = vecRot.dy;
	pt.z = vecRot.dz;
	double nAngle = ITKGeo::GetRotatedAngleInXYPlane(pt);

	return nAngle;
}

/**	\brief	The CPlane3d::ComputePlaneEquation function\n
	Computes plane equation.

	\param	pt1	a parameter of type POINT_T&
	\param	pt2	a parameter of type POINT_T&
	\param	pt3	a parameter of type POINT_T&

	\return	bool	
*/
bool CPlane3d::ComputePlaneEquation(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3){
	bool bRet = false;

	assert(!((pt1.x == pt2.x) && (pt1.y == pt2.y) && (pt1.z == pt2.z)));
	assert(!((pt1.x == pt3.x) && (pt1.y == pt3.y) && (pt1.z == pt3.z)));
	if(!((pt1.x == pt2.x) && (pt1.y == pt2.y) && (pt1.z == pt2.z)) && 
		!((pt1.x == pt3.x) && (pt1.y == pt3.y) && (pt1.z == pt3.z))){
			
		double ii1= pt2.x - pt1.x,jj1= pt2.y - pt1.y,kk1= pt2.z - pt1.z;
		double ii2= pt3.x - pt1.x,jj2= pt3.y - pt1.y,kk2= pt3.z - pt1.z;
			
		double iicp=0.,jjcp=0.,kkcp=0.;
		CMath::GetCrossProduct(ii1,jj1,kk1,ii2,jj2,kk2,&iicp,&jjcp,&kkcp);
		assert(!((iicp == 0.0) && (jjcp == 0.0) && (kkcp == 0.0)));
		if(!((iicp==0.0) && (jjcp==0.0) && (kkcp==0.0))){
			// normalize plane equation 
			CMath::NormalizeVector(iicp,jjcp,kkcp,&(m_vecNorm.dx),&(m_vecNorm.dy),&(m_vecNorm.dz));
				
			//* compute D of plane equation
			m_nD = -(m_vecNorm.dx*pt1.x) - (m_vecNorm.dy*pt1.y) - (m_vecNorm.dz*pt1.z); 
				
			bRet = true;
		}
	}

	return bRet;
}

/**	\brief	The CPlane3d::WhichSideOfPlane function

	\param	pt	a parameter of type POINT_T&

	\return	SIGN_T	
*/
SIGN_T CPlane3d::WhichSideOfPlane(POINT_T& pt){
	#define TOLERANCE 0.001
	SIGN_T ret=STRADDLE;

	/*if(m_vecNorm.dx){
		double t=(pt.x - m_nD)/m_vecNorm.dx;
		if(t > TOLERANCE){
			ret = POSITIVE;
		}else if(t < -TOLERANCE){
			ret = NEGATIVE;
		}
	}else if(m_vecNorm.dy){
		double t=(pt.y - m_nD)/m_vecNorm.dy;
		if(t > TOLERANCE){
			ret = POSITIVE;
		}else if(t < -TOLERANCE){
			ret = NEGATIVE;
		}
	}else if(m_vecNorm.dz){
		double t=(pt.z - m_nD)/m_vecNorm.dz;
		if(t > TOLERANCE){
			ret = POSITIVE;
		}else if(t < -TOLERANCE){
			ret = NEGATIVE;
		}
	}
	*/

	double dp= m_vecNorm.dx*pt.x + m_vecNorm.dy*pt.y + m_vecNorm.dz*pt.z + m_nD;
   
	if(dp > TOLERANCE){
		ret=POSITIVE;
	}else if(dp < -TOLERANCE){
		ret=NEGATIVE;
	}
	
	return ret;
}