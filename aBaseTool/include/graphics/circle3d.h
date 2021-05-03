#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include <stdio.h>
#include "point.h"
#include "curve3d.h"

typedef struct tagCircle{
	POINT_T  ptOrigin;	// origin point
	double   nRadius;	// radius
	VECTOR_T vecNorm;	// normal vector
}CIRCLE_T,* PCIRCLE_T;

//	description	: 
class DLL_EXPORT CCircle3d : public CCurve3d,public CIRCLE_T{
public:
	CCircle3d();
	CCircle3d(const CIRCLE_T& circle);
	CCircle3d(const CCircle3d& circle);
	CCircle3d(double x,double y,double z,double radius);
	virtual ~CCircle3d();
public:
	POINT_T GetOrigin();
	double GetRadius();
	void SetOrigin(double x,double y,double z);
	void SetOrigin(POINT_T ptCenter);
	void SetRadius(const double& radius);

	void CreateSegments();
	PPOINT_T Sweep(VECTOR_T vecAxis,double nAngle,int nParts);
};

#endif
