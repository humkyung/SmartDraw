#ifndef __SPHERESURF_H__
#define __SPHERESURF_H__

#include "..\SQStruct.h"
#define	SPHERE_MAX_BOUNDARY	8

typedef struct tagSQSphere
{
   	POINT_T  ptOrigin;
	SQVECTOR vecNorm;
	double   nRadius;
	
	unsigned char nLoop;
	struct tagSQPrimitive* pBoundary[SPHERE_MAX_BOUNDARY];
}SQSPHERESURFACE,SPHERESURF_T,* PSQSPHERESURFACE;

#endif
