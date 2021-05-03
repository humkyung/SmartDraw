#ifndef __CONE_H__
#define __CONE_H__

#include "..\SQStruct.h"

typedef struct tagCone{
    	VECTOR_T vecAxis;
	POINT_T  ptOrigin[2];
	double   nRadius[2];
}CONE_T,* PCONE_T;

#endif
