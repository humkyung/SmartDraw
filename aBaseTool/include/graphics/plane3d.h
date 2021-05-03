#ifndef __PLANE3D_H__
#define __PLANE3D_H__

#include "../ObjInterface.h"
#include <math/vect.h>
#include <graphics/point.h>

typedef enum {
	NEGATIVE= -1,
	ZERO    = 0,
	STRADDLE = 0,
	POSITIVE= 1
}SIGN_T;

typedef struct{
	double a,b,c,d;
}PLANE_T,* PPLANE_T;

class DLL_EXPORT CPlane3d : public CObjInterface{
public:
	CPlane3d();
	CPlane3d(VECTOR_T& vec);
	CPlane3d(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3);
	virtual ~CPlane3d();
public:
	VECTOR_T GetNormVector();
	void SetNormVector(double dx,double dy,double dz);
	void SetNormVector(VECTOR_T vecNorm);

	void GetRotatedAngleOfAxis(double& alpha,double& beta);
	POINT_T  RotateAboutXYAxis(POINT_T pt,const double& beta,const double& alpha);
	VECTOR_T RotateAboutXYAxis(const VECTOR_T& vec,double alpha,double beta);
	POINT_T  RotateAboutYXAxis(POINT_T pt,double beta,double alpha);
	VECTOR_T RotateAboutYXAxis(VECTOR_T& vec,double beta,double alpha);

	double GetAngle(VECTOR_T& vec);
	bool ComputePlaneEquation(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3);
	SIGN_T WhichSideOfPlane(POINT_T& pt);
public:	
	VECTOR_T m_vecNorm;
	double   m_nD;

	const static VECTOR_T XAXIS;
	const static VECTOR_T YAXIS;
	const static VECTOR_T ZAXIS;
};

#endif
