#ifndef __PLANE_H__
#define __PLANE_H__

#include "..\SQStruct.h"

// plane equation.
// ax + by + cz + d = 0
typedef struct{
	double a,b,c,d;
}SQPLANE,PLANE_T,*PPLANE_T;

class CPlane : public PLANE_T{
public:
	CPlane(const SQVECTOR vecNorm,const POINT_T pt);
	virtual ~CPlane();
public:
	POINT_T  Project(const POINT_T pt);
	SQVECTOR Project(const SQVECTOR vec);
};

#endif
