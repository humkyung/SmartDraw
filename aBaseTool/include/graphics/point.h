#ifndef	__POINT_H__
#define	__POINT_H__

#include <ObjInterface.h>

#define	PT_PRIMARY		0
#define	PT_CONSTRUCTION	1

typedef struct tagPoint
{
	double x,y,z;
	unsigned char reserved;
}POINT_T,* PPOINT_T;

class DLL_EXPORT CPoint3d : public CObjInterface,public POINT_T{
public:
	CPoint3d();
	virtual ~CPoint3d();
public:
	static const double Distance(const POINT_T& pt1,const POINT_T& pt2);
};

DLL_EXPORT POINT_T operator +(const POINT_T pt1,const POINT_T& pt2);
DLL_EXPORT POINT_T operator *(const double nVal,const POINT_T& pt);
DLL_EXPORT bool IsEqual(const POINT_T& x,const POINT_T& y,double nTol=0.);

#endif
