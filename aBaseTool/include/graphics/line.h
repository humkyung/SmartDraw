#ifndef __LINE3D_H__
#define __LINE3D_H__

#include <graphics/curve3d.h>

enum{
	INNER_POINT = 1,
	OUTER_POINT = 2
};

typedef enum{
	DONT_INTERSECTION = 0,
	DO_INTERSECTION   = 1,
	COLLINEAR_LINE    = 2
}INTERSECTION_E;

typedef struct tagLine{
	POINT_T ptStart;	// start point
	POINT_T ptEnd;		// end point
}LINE_T,* PLINE_T;

DLL_EXPORT  double Length(const LINE_T& line);
DLL_EXPORT  INTERSECTION_E IntersectLine2D(double* x,double* y,double x1,double y1,double x2,double y2,double x3,double y3,double x4,double y4,double tol=0.);
DLL_EXPORT  INTERSECTION_E IntersectLine2D(POINT_T& pt,const LINE_T& line1,const LINE_T& line2);
DLL_EXPORT  INTERSECTION_E IntersectLine3D(POINT_T& pt,const LINE_T& line1,const LINE_T& line2);

#endif
