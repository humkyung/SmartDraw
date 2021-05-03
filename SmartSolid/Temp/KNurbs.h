#ifndef __KNURBS_H__
#define __KNURBS_H__

#include <SQLib.h>
#include "KCurve.h"

class KNURBS : public NURBS,public KCurve{
public:
	KNURBS();
	KNURBS(const NURBS& nurbs);
	KNURBS(const KNURBS& nurbs);
	virtual ~KNURBS();
public:
	void CreateSegments(){};
	void InsertKnot(const double u);
	SQPOINT operator()(double u);

	int GetNumOfCtrlPoints();
	int GetDegree();
	NURBS::WPOINT* GetCtrlPoints();
	double* GetKnots();

	void SetWeights(const double* pWeights);
protected:
	int GetKnotMultiplicity(int span,double u);
	int FindKnotSpan(double u);
};

#endif