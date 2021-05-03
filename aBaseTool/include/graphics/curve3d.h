#ifndef __CURVE3D_H__
#define __CURVE3D_H__

#include "../ObjInterface.h"
#include "point.h"
#include "../math/vect.h"

#define	MAX_CURVE_POINTS 3072

class DLL_EXPORT CCurve3d : public CObjInterface
{
public:
	CCurve3d();
	virtual ~CCurve3d();
public:
	void SetSegments(const int nSegments);
	int GetNumOfPoints();
	virtual PPOINT_T GetFacets();
	virtual void CreateSegments()=0;
	virtual PPOINT_T Sweep(VECTOR_T vecAxis,double nAngle,int nParts);
	static PPOINT_T Revolve(const VECTOR_T& vecAxis,int nSize,PPOINT_T pptOriginal,const double& nAngle,const int& nParts);
	
	static const int GetNumOfSegments();
	static POINT_T& pt(const int& nIndex);
public:
	static int NUM_OF_SEGMENTS;
protected:
	static POINT_T m_pt[MAX_CURVE_POINTS];
};

#endif
