#ifndef __ITK_GEOMETRY_H__
#define __ITK_GEOMETRY_H__

#include <itkdef.h>
#include <math\vect.h>
#include <math/quat.h>
#include <graphics/point.h>
#include <graphics/line.h>
#include <graphics/plane3d.h>
#include <graphics/volume.h>

using namespace itk;

#ifndef	DLL_EXPORT
#define	DLL_EXPORT __declspec(dllexport)
#endif

class DLL_EXPORT CGeometry{
public:
	//typedef enum{
	//	COLLINEAR = 0,
	//	INTERSECT = 1
	//}INTERSECTION_E;
public:
	CGeometry();
	~CGeometry();
public:
	static bool     IsSameValue(double val1,double val2,double tol);
	static bool     IsSamePoint(const POINT_T& pt1,const POINT_T& pt2,double tol);
	static double   GetRotatedAngleInXYPlane(const POINT_T& pt);
	static double   GetRotatedAngleInXYPlane(const VECTOR_T& vec);
	static void     GetRotatedAngleOfAxis(const VECTOR_T& vecAxis,double& alpha,double& beta);
	static VECTOR_T	RotateOnXYPlane(const VECTOR_T vec,double nDeg);
	static POINT_T  RotateAboutXYAxis(POINT_T& pt,const double& xangle,const double& yangle);
	static VECTOR_T RotateAboutXYAxis(const VECTOR_T& pt,double xangle,double yangle);
	static POINT_T  RotateAboutYXAxis(POINT_T& pt,double beta,double alpha);
	static VECTOR_T RotateAboutYXAxis(VECTOR_T& pt,double beta,double alpha);
	static POINT_T  RotateAboutZAxis(const POINT_T& pt,const double theta);
	static VECTOR_T RotateAboutZAxis(const VECTOR_T& pt,const double theta);
	static POINT_T  Rotate(POINT_T pt,QUAT_T quat);
	static VECTOR_T Rotate(VECTOR_T vec,QUAT_T quat);
	
	static bool     IsPointOnLine(double x,double y,double px,double py,double qx,double qy);

	static INTERSECTION_E IntersectLine2D(double* x,double* y,double x1,double y1,double x2,double y2,double x3,double y3,double x4,double y4,const double tol);
	static INTERSECTION_E IntersectLine2D(POINT_T& pt,const LINE_T& line1,const LINE_T& line2);
	static bool IntersectLineWithPlane(POINT_T* pt,LINE_T line,PLANE_T plane);
	static bool IntersectLineToVolume(PLINE_T pLine,PSQVOLUME pVolume,PPOINT_T pRet);

	static bool IsLeftSidePoint(const POINT_T& pt,const LINE_T& line);
	static bool IsRightSidePoint(const POINT_T& pt,const LINE_T& line);

	static bool IsEqualPoint2D(POINT_T& x,POINT_T& y,double tol=0.);
private:
	const static VECTOR_T AXIS;
	const static int NUM_OF_DIGITS;
};

#endif
