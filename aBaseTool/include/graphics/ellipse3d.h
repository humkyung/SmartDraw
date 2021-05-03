#ifndef __ELLIPSE_H__
#define __ELLIPSE_H__

#include <math/vect.h>
#include <graphics/curve3d.h>

typedef struct tagEllipse{
	VECTOR_T vecNorm;	// normal vector of plane
    	POINT_T  ptOrigin;
	double	 nRotate;	// rotate angle in degree.
	double   nAxis[2];	// nAxis[0] - primary axis, nAxis[2] = secondary axis
	double   nStartAngle;	// start angle in degree.
	double   nSweepAngle;	// sweep angle in degree.
}ELLIPSE_T,* PELLIPSE_T;

class DLL_EXPORT CEllipse3d : public CCurve3d,public ELLIPSE_T{
public:
	CEllipse3d();
	CEllipse3d(const ELLIPSE_T& ellipse);
	~CEllipse3d();
public:
	void CreateSegments();
};

#endif
