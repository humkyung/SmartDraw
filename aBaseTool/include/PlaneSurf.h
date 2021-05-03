#ifndef __PLANESURF_H__
#define __PLANESURF_H__

#include "..\SQStruct.h"

typedef struct tagPlaneSurf{
	POINT_T  ptOrigin;
	VECTOR_T vecNorm;
    	struct tagSQPrimitive* pBoundary;
}PLANESURF_T,* PPLANESURF_T;

#endif
