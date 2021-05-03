#ifndef __GEO_H__
#define __GEO_H__

#define DONT_INTERSECT 0
#define DO_INTERSECT   1
#define COLLINEAR      2

class KGeometry1{
public:
	KGeometry1();
	~KGeometry1();
public:
	bool IsPointOnLine(double px,double py,double qx,double qy,double x,double y);
	int  IntersectLine(double x1,double y1,double x2,double y2,double x3,double y3,double x4,double y4,double* x,double* y,double tol = 0.0);//,double tol = 0.000000001);

	double DotProduct(double vtx1,double vty1,double vtz1,double vtx2,double vty2,double vtz2);
};

extern KGeometry1 theGeo;

#endif