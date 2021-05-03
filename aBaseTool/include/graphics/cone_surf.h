#ifndef __CONESURF_H__
#define __CONESURF_H__

#define CONE_MAX_BOUNDARY 8

#include "../../SQStruct.h"

typedef struct tagConeSurf{
    	POINT_T  ptOrigin;
	VECTOR_T vecStart;
	VECTOR_T vecAxis;
	VECTOR_T vecDelta;
	typedef struct{
		double a,b;
	}_ratio;
	_ratio ratio;
	double nRadius;

	unsigned char nLoop;
	struct tagSQPrimitive* pBoundary[CONE_MAX_BOUNDARY];
}CONESURF_T,* PCONESURF_T;

#endif
