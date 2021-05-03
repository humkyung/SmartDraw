#ifndef __SQPOINT_H__
#define __SQPOINT_H__

#include <stdio.h>

//	description	: point to location of 3-dimension.
//			  if not use z(in 2D-plane) z is setted to zero.
class SQPoint{

typedef struct tagPoint{
	double x,y,z;
}_POINT,* _PPOINT;

public:
	SQPoint();
	SQPoint(double x,double y,double z = 0.);
	SQPoint(SQPoint& point);
	~SQPoint();
public:
	double GetX();
	double GetY();
	double GetZ();
	void SetX(const double x);
	void SetY(const double y);
	void SetZ(const double z);

	void SetPoint(const double x,const double y,const double z = 0.);
	void SetPoint(const SQPoint point);

	bool IsAdjacent(const SQPoint& point,const double tol);

	double Distance(const SQPoint& point);

	bool operator =(const SQPoint& point);
	bool operator +=(const SQPoint& point);
	bool operator -=(const SQPoint& point);

	friend SQPoint operator +(const SQPoint& pt1,const SQPoint& pt2);
protected:
	_POINT m_ptPoint;
};

#endif