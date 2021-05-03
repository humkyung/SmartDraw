#ifndef __RECT_H__
#define __RECT_H__

#include "../ObjInterface.h"
#include <graphics/point.h>

typedef struct tagRect{
	double x,y;		// x,y - corner of rectangle
	double width,height;	// width,height of rectangle
}RECT_T,* PRECT_T;

class DLL_EXPORT CRect2d : public CObjInterface,public tagRect{
public:
	CRect2d();
	virtual ~CRect2d();
public:
	bool IsInnerPoint(const POINT_T& pt);
	bool IsOverlapped(const CRect2d& rect);
};

#endif
