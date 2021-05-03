#ifndef __KPLANE_H__
#define __KPLANE_H__

#include <SQLib.h>

class KPlane{
public:
	KPlane();
	virtual ~KPlane();
public:
	void RotateAboutZAxis(double alpha);
	void RotateAboutYAxis(double beta);
	void RotateAboutXAxis(double gamma);

	double GetRotated(double& nXAngle,double& nYAngle,double& nZAngle);
	bool IntersectToPlane(SQVECTOR& vector,SQPOINT& point,KPlane& plane);
protected:	
	SQVECTOR m_vecNormal;
	SQVECTOR m_vecXAxis;
	SQVECTOR m_vecYAxis;
	double   m_nD;

	static SQVECTOR XAXIS;
	static SQVECTOR YAXIS;
	static SQVECTOR ZAXIS;
};

#endif
