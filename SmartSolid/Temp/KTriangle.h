#ifndef __KTRIANGLE_H__
#define __KTRIANGLE_H__

#include <SQLib.h>
#include "KPlane.h"

class KTriangle : public KPlane{
public:
	KTriangle();
	KTriangle(SQPOINT& pt1,SQPOINT& pt2,SQPOINT& pt3);
	KTriangle(KTriangle& triangle);
	virtual ~KTriangle();
public:
	bool HasEdge(SQPOINT& pt1,SQPOINT& pt2);
	bool DoOccurIntersection(SQPOINT& pt1,SQPOINT& pt2);
	void SetCCW();
	void operator ()(SQPOINT& vt1,SQPOINT& vt2,SQPOINT& vt3);
	SQPOINT& operator[](int nIndex);
	bool HasBadAngle();

	static double  Area(const SQPOINT& pt1,const SQPOINT& pt2,const SQPOINT& pt3);
	static SQPOINT FindCircumCenter(const SQPOINT& pt1,const SQPOINT& pt2,const SQPOINT& pt3);
	static bool IsInsideCircle(const SQPOINT& pt1,const SQPOINT& pt2,const SQPOINT& pt3,const SQPOINT& pt);
	static bool IsOnCircle(const SQPOINT& pt1,const SQPOINT& pt2,const SQPOINT& pt3,const SQPOINT& pt);

	friend bool operator==(KTriangle& tri1,KTriangle& tri2);
protected:
	int FindIndexOfSmallestAngle();
	double Angle(const SQPOINT& pt1,const SQPOINT& pt2,const SQPOINT& pt3);
protected:
	SQPOINT m_pt[3];
};

#endif
