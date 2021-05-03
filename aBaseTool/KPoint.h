#ifndef __KPOINT_H__
#define __KPOINT_H__

#include <stdio.h>
#include <SQLib.h>

//	description	: point to location of 3-dimension.
//			  if not use z(in 2D-plane) z is setted to zero.
class KPoint{
public:
	KPoint();
	KPoint(KPoint& point);
	KPoint(double x,double y,double z = 0.);
	KPoint(SQPOINT& point);
	~KPoint();
public:
	double GetX();
	double GetY();
	double GetZ();
	void SetX(const double x);
	void SetY(const double y);
	void SetZ(const double z);

	bool operator  =(const KPoint& point);
	bool operator +=(const KPoint& point);
	bool operator -=(const KPoint& point);

	friend KPoint operator +(const KPoint& pt1,const KPoint& pt2);
protected:
	SQPOINT m_ptPoint;
};

#endif
