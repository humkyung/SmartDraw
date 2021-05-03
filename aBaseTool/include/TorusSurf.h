#ifndef __TORUSSURF_H__
#define __TORUSSURF_H__

#include "..\SQStruct.h"

#define TORUS_MAX_BOUNDARY 8

typedef struct tagSQTorusSurface
{
   	POINT_T  ptOrigin;
	SQVECTOR vecNorm;
	double   nTorusRadius;
	double   nTubeRadius;
	
	unsigned char nLoop;
	struct tagSQPrimitive* pBoundary[TORUS_MAX_BOUNDARY];
}SQTORUSSURFACE,TORUSSURF_T,* PSQTORUSSURFACE; 

#endif
