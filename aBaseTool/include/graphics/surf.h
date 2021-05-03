#ifndef __SURFACE_H__
#define __SURFACE_H__

#include "..\..\SQStruct.h"
#include "..\PlaneSurf.h"
#include "cone_surf.h"
#include "..\SphereSurf.h"
#include "..\TorusSurf.h"

enum{
	SURF_OF_REVOLUTION=1,
	SWEEP_SURF        =2,
	PLANE_SURF        =3,
	CONE_SURF	  =4,
	SPHERE_SURF	  =5,
	TORUS_SURF	  =6
};

typedef struct tagSQSurface{
	unsigned char nType;
	unsigned char nBoundary;

	typedef struct tagSurfaceElement{
		struct tagSQPrimitive* pBoundaryCurve;
		struct tagSQPrimitive* pRuleCurve;
		
		struct tagSurfaceElement* next;
	}SURFELM_T,* PSURFELM_T;

	PSURFELM_T pElement;
}SQSURFACE,* PSQSURFACE,SURF_T,* PSURF_T;

#endif
