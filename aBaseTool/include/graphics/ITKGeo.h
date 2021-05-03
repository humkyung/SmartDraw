#ifndef __ITKGEO_H__
#define __ITKGEO_H__

#include <ITKDef.h>
#include <math/vect.h>
#include <math\quat.h>
#include "plane3d.h"
#include "point.h"
#include "line.h"
#include "circle3d.h"
#include "arc3d.h"
#include "ellipse3d.h"
#include "rect.h"
#include "volume.h"

#ifndef	DLL_EXPORT
#define	DLL_EXPORT __declspec(dllexport)
#endif

class DLL_EXPORT ITKGeo{
public:
	static double GetRotatedAngleInXYPlane(POINT_T pt);

	static bool IsOverlapRect(PRECT_T pRect1,PRECT_T pRect2);
	static bool IsPointInRect(const POINT_T& point,const RECT_T& rect);
	static bool IntersectLineToVolume(PLINE_T pLine,PSQVOLUME pVolume,PPOINT_T pRet);
	static bool ComputePlaneEquation(PPLANE_T pPlaneEquation,POINT_T pt1,POINT_T pt2,POINT_T pt3);
	static POINT_T GetMinDistPoint(const POINT_T& pt,int& nLoc,const LINE_T& line);
};

DLL_EXPORT const VECTOR_T& operator -(const POINT_T& pt1,const POINT_T& pt2);
DLL_EXPORT const POINT_T&  operator +(const POINT_T& pt,const VECTOR_T& vec);

#endif
