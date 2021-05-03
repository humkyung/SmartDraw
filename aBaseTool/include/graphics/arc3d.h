#ifndef __ARC3D_H__
#define __ARC3D_H__

#include "curve3d.h"
#include <math/vect.h>

typedef struct DLL_EXPORT tagArc{
    	POINT_T  ptOrigin;	// origin point of arc
	POINT_T  ptStart;	// start point of arc
	VECTOR_T vecNorm;	// normal vector of plane
	double   nSweep;	// sweep angle in degree
	double   nRatio;	// ratio
}ARC_T,* PARC_T;

class DLL_EXPORT CArc3d : public CCurve3d,public ARC_T{
public:
	CArc3d();
	CArc3d(const ARC_T& arc);
	CArc3d(const CArc3d& arc);
    	CArc3d(POINT_T ptOrigin,POINT_T ptStart,const double& nSweepAng);
	virtual ~CArc3d();
public:
	void CreateSegments();
	static void segments(POINT_T ptEDGE[],const POINT_T& ptOrigin,const POINT_T& ptStart,const VECTOR_T& vecNorm,const double nSweep,const int& nSegments);
	static POINT_T on(const POINT_T& ptOrigin,const VECTOR_T vecNorm,const double nRadius,const double& nAngle);

	PPOINT_T Revolve(VECTOR_T vecAxis,double nAngle,int nParts);
	static POINT_T OnPoint(const POINT_T ptOrigin,const double nRadius,const VECTOR_T vecAxis,const double& nAngle);
public:
	int m_nSegments;
private:
	long m_nPoints;
};

#endif
