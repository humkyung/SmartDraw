#ifndef __OPEN3D_DT_H__
#define __OPEN3D_DT_H__

//	author		: baek hum kyung
//	date		: 2002.03.18 ~  
//	description	: describes 2D delaunay triangle.

#include "../facet.h"

typedef enum{
	LOC_OUTER_POINT = 0,
	LOC_ON_POINT    = 1,
	LOC_ON_EDGE     = 2,
	LOC_INNER_POINT = 3
}LOCATE_T;

class CDT : public CFacet
{
public:
	CDT();
	CDT(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3);
	CDT(const CDT& dt);
	~CDT();
public:
	bool IsApex(const POINT_T& pt);
	bool HasPosAtIndex(const POINT_T& pt,const int& nIndex);
	bool HasEdge(const POINT_T& pt1,const POINT_T& pt2);
	int HasSegment(const POINT_T& pt1,const POINT_T& pt2);
	bool DoOccurIntersection(const POINT_T& pt1,const POINT_T& pt2);
	void SetCCW();
	bool operator ==(CDT& dt);
	void operator ()(const POINT_T& vt1,const POINT_T& vt2,const POINT_T& vt3);
	POINT_T& operator[](int nIndex);
	bool HasBadAngle();

	static double  Area(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3);
	static POINT_T FindCircumCenter(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3);
	static bool IsInsideCircle(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3,const POINT_T& pt);
	static bool IsOnCircle(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3,const POINT_T& pt);
	CDT::LOCATE_T WhereLocatedPoint(const POINT_T& pt);

	friend bool operator==(CDT& dt1,CDT& dt2);
public:
	void Set(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3);
	bool m_bDeleted;
	long m_nEdgeId[MAXFACETPOINTS];
protected:
	int FindIndexOfSmallestAngle();
	double Angle(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3);
private:
	double m_nArea;
	static const double BAD_ANGLE;
	static unsigned long ID;
};

#endif