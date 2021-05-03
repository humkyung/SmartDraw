#include <math.h>
#include <windows.h>
//#include "d:\\project\\spoon_0_0_1\\utility\\profile.h"
#include <SQLib.h>
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include <graphics\line.h>
#include "libmesh2d.h"
#include "DT.h"

using namespace mesh;

const double CDT::BAD_ANGLE=5;
unsigned long CDT::ID = 0L;

/**	\brief
*/
CDT::CDT()
{
	m_nPoints = 0;
	m_bDeleted = false;
	m_nId = CDT::ID++;
	m_nEdgeId[0] = m_nEdgeId[1] = m_nEdgeId[2] = 0;
	m_nArea = -1;

#ifdef _DEBUG
//	PROFILE("CDT()");
//	OutputDebugString("CDT()");
#endif
}

/**	@brief
	@author	πÈ»Ï∞Ê

*/
CDT::CDT(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3)
{
	m_nPoints = 3;
	m_bDeleted = false;

	m_pt[0].x = _round(pt1.x,9);m_pt[0].y = _round(pt1.y,9);m_pt[0].z = _round(pt1.z,9);
	m_pt[1].x = _round(pt2.x,9);m_pt[1].y = _round(pt2.y,9);m_pt[1].z = _round(pt2.z,9);
	m_pt[2].x = _round(pt3.x,9);m_pt[2].y = _round(pt3.y,9);m_pt[2].z = _round(pt3.z,9);
	m_nEdgeId[0] = m_nEdgeId[1] = m_nEdgeId[2] = 0;
	m_nId = CDT::ID++;
	m_nArea = -1;

#ifdef _DEBUG
//	PROFILE("CDT()");
//	OutputDebugString("CDT()");
#endif
}

/**	\brief
*/
CDT::CDT(const CDT& dt)
{
	m_nPoints = dt.m_nPoints;
	m_bDeleted = dt.m_bDeleted;

	m_pt[0] = dt.m_pt[0];
	m_pt[1] = dt.m_pt[1];
	m_pt[2] = dt.m_pt[2];
	m_nId = CDT::ID++;
	m_nArea = -1;

#ifdef _DEBUG
//	PROFILE("CDT()");
//	OutputDebugString("CDT()");
#endif
}

/**	\brief
*/
CDT::~CDT()
{
#ifdef _DEBUG
//	PROFILE("~CDT()");
//	OutputDebugString("~CDT()");
#endif
}

/**	\brief	The CDT::IsApex function

	\param	pt	a parameter of type const POINT_T&

	\return	bool	
*/
bool CDT::IsApex(const POINT_T& pt)
{
	char szBuf[4][128]={0,};

	sprintf(szBuf[0],"%.15lf %.15lf",pt.x,pt.y);
	sprintf(szBuf[1],"%.15lf %.15lf",m_pt[0].x,m_pt[0].y);
	sprintf(szBuf[2],"%.15lf %.15lf",m_pt[1].x,m_pt[1].y);
	sprintf(szBuf[3],"%.15lf %.15lf",m_pt[2].x,m_pt[2].y);
//	if(0 == memcmp(&(m_pt[0]),&pt,sizeof(POINT_T))) return true;
//	if(0 == memcmp(&(m_pt[1]),&pt,sizeof(POINT_T))) return true;
//	if(0 == memcmp(&(m_pt[2]),&pt,sizeof(POINT_T))) return true;
	if(IsSamePoint(m_pt[0],pt) || IsSamePoint(m_pt[1],pt) || IsSamePoint(m_pt[2],pt)) return true;

	return false;
}

/**	\brief	The CDT::HasPos function

	\param	pt	a parameter of type const POINT_T&

	\return	bool	
*/
bool CDT::HasPosAtIndex(const POINT_T& pt,const int& nIndex)
{
	if(IsSamePoint(m_pt[nIndex],pt)) return true;
	return false;
}

/**	\brief	The CDT::HasSegment function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	int	
*/
int CDT::HasSegment(const POINT_T& pt1,const POINT_T& pt2)
{
	if(!IsSamePoint(pt1,pt2))
	{
		if(IsSamePoint(m_pt[0],pt1) && IsSamePoint(m_pt[1],pt2)) return 0;
		if(IsSamePoint(m_pt[1],pt1) && IsSamePoint(m_pt[2],pt2)) return 1;
		if(IsSamePoint(m_pt[2],pt1) && IsSamePoint(m_pt[0],pt2)) return 2;
		return -1;
	}else	return -1;
}

/**	\brief	The CDT::HasEdge function\n
	check whether has an edge created with pt1 and pt2 or not.

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	return true if has an edge.
*/
bool CDT::HasEdge(const POINT_T& pt1,const POINT_T& pt2)
{
	//if(0 != memcmp(&pt1,&pt2,sizeof(POINT_T))){
	if(!IsSamePoint(pt1,pt2))
	{
		bool bFirst=false;
		if(IsSamePoint(m_pt[0],pt1) || IsSamePoint(m_pt[1],pt1) || IsSamePoint(m_pt[2],pt1)) bFirst = true;
		//if(IsSamePos(m_pt[0],pt1) || IsSamePos(m_pt[1],pt1) || IsSamePos(m_pt[2],pt1)) bFirst = true;

		bool bSecond=false;
		if(IsSamePoint(m_pt[0],pt2) || IsSamePoint(m_pt[1],pt2) || IsSamePoint(m_pt[2],pt2)) bSecond = true;
		//if(IsSamePos(m_pt[0],pt2) || IsSamePos(m_pt[1],pt2) || IsSamePos(m_pt[2],pt2)) bSecond = true;
		
		return (bFirst && bSecond);
	}else	return false;
}

/**	\brief	The CDT::DoOccurIntersection function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	bool	
*/
bool CDT::DoOccurIntersection(const POINT_T& pt1,const POINT_T& pt2)
{
	bool bRet=false;

	POINT_T pt={0,};
	if(IsApex(pt1) && IsApex(pt2)) return false;

	//if((0 == memcmp(&(m_pt[0]),&pt1,sizeof(POINT_T))) || (0 == memcmp(&(m_pt[0]),&pt2,sizeof(POINT_T)))){
	if(IsSamePoint(m_pt[0],pt1) || IsSamePoint(m_pt[0],pt2))
	{
		INTERSECTION_E t=IntersectLine2D(&pt.x,&pt.y,
			m_pt[1].x,m_pt[1].y,m_pt[2].x,m_pt[2].y,
			pt1.x,pt1.y,pt2.x,pt2.y,0.);
		if((DO_INTERSECTION == t) && !(IsSamePoint(pt,pt1) || IsSamePoint(pt,pt2))) bRet = true;
	//}else if((0 == memcmp(&(m_pt[1]),&pt1,sizeof(POINT_T))) || (0 == memcmp(&(m_pt[1]),&pt2,sizeof(POINT_T)))){
	}
	else if(IsSamePoint(m_pt[1],pt1) || IsSamePoint(m_pt[1],pt2))
	{
		INTERSECTION_E t=IntersectLine2D(&pt.x,&pt.y,
			m_pt[0].x,m_pt[0].y,m_pt[2].x,m_pt[2].y,
			pt1.x,pt1.y,pt2.x,pt2.y,0.);
		if((DO_INTERSECTION == t) && !(IsSamePoint(pt,pt1) || IsSamePoint(pt,pt2))) bRet = true;
	//}else if((0 == memcmp(&(m_pt[2]),&pt1,sizeof(POINT_T))) || (0 == memcmp(&(m_pt[2]),&pt2,sizeof(POINT_T)))){
	}
	else if(IsSamePoint(m_pt[2],pt1) || IsSamePoint(m_pt[2],pt2))
	{
		INTERSECTION_E t=IntersectLine2D(&pt.x,&pt.y,
			m_pt[0].x,m_pt[0].y,m_pt[1].x,m_pt[1].y,
			pt1.x,pt1.y,pt2.x,pt2.y,0.);
		if((DO_INTERSECTION == t) && !(IsSamePoint(pt,pt1) || IsSamePoint(pt,pt2))) bRet = true;
	}
	else
	{
		INTERSECTION_E t=IntersectLine2D(&pt.x,&pt.y,
			m_pt[0].x,m_pt[0].y,m_pt[1].x,m_pt[1].y,
			pt1.x,pt1.y,pt2.x,pt2.y,0.);
		if((DO_INTERSECTION == t) && !(IsSamePoint(pt,pt1) || IsSamePoint(pt,pt2))) return true;

		t=IntersectLine2D(&pt.x,&pt.y,
			m_pt[1].x,m_pt[1].y,m_pt[2].x,m_pt[2].y,
			pt1.x,pt1.y,pt2.x,pt2.y,0.);
		if((DO_INTERSECTION == t) && !IsSamePoint(pt,pt1) && !IsSamePoint(pt,pt2)) return true;

		t=IntersectLine2D(&pt.x,&pt.y,
			m_pt[2].x,m_pt[2].y,m_pt[0].x,m_pt[0].y,
			pt1.x,pt1.y,pt2.x,pt2.y,0.);
		if((DO_INTERSECTION == t) && !IsSamePoint(pt,pt1) && !IsSamePoint(pt,pt2)) return true;
	}

	return bRet;
}

/**	\brief	The CDT::SetCCW function\n
	reorder positions of points.\n
	pt1 <-- pt3\n
	\    ^
	 \  /
	  \/
	  pt2

	\return	void	
*/
void CDT::SetCCW()
{
	if(Area(m_pt[0],m_pt[1],m_pt[2]) < 0.)
	{
		POINT_T pt={0.};

		pt = m_pt[0];
		m_pt[0] = m_pt[1];
		m_pt[1] = pt;
	}
}

/**	\brief	The CDT::Area function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&
	\param	pt3	a parameter of type const POINT_T&

	\return	double
*/
double CDT::Area(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3)
{
	double ux=pt2.x - pt1.x;
	double uy=pt2.y - pt1.y;
	double vx=pt3.x - pt1.x;
	double vy=pt3.y - pt1.y;
	
	const double area=(ux*vy - uy*vx)*0.5;

	return area;
}

/**	\brief	The CDT::FindCircumCenter function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&
	\param	pt3	a parameter of type const POINT_T&

	\return	POINT_T	
*/
POINT_T CDT::FindCircumCenter(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3)
{
	double p1=pt1.x*pt1.x + pt1.y*pt1.y;
	double p2=pt2.x*pt2.x + pt2.y*pt2.y;
	double p3=pt3.x*pt3.x + pt3.y*pt3.y;
	double cx=(p1*(pt2.y-pt3.y) + p2*(pt3.y-pt1.y) + p3*(pt1.y-pt2.y))/
		(2*Area(pt1,pt2,pt3)*2);
	double cy=(p1*(pt3.x-pt2.x) + p2*(pt1.x-pt3.x) + p3*(pt2.x-pt1.x))/
		(2*Area(pt1,pt2,pt3)*2);

	POINT_T pt={cx,cy,0.};

	return pt;
}

/**	\brief	The CDT::IsOnCircle function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&
	\param	pt3	a parameter of type const POINT_T&
	\param	pt	a parameter of type const POINT_T&

	\return	bool	
*/
bool CDT::IsOnCircle(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3,const POINT_T& pt)
{
	bool bRet=false;

	POINT_T ptCenter=FindCircumCenter(pt1,pt2,pt3);
	double dx=ptCenter.x - pt.x;
	double dy=ptCenter.y - pt.y;
	double dz=ptCenter.z - pt.z;
	double dist1=sqrt(dx*dx + dy*dy + dz*dz);
	dx=ptCenter.x - pt1.x;
	dy=ptCenter.y - pt1.y;
	dz=ptCenter.z - pt1.z;
	double dist2=sqrt(dx*dx + dy*dy + dz*dz);
	//if(fabs(dist1 - dist2) < CLibMesh2d::TOLERANCE) bRet = true;
	if(fabs(dist1 - dist2) < 0.) bRet = true;

	return bRet;
}

/**	\brief	The CDT::IsInsideCircle function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&
	\param	pt3	a parameter of type const POINT_T&
	\param	pt	a parameter of type const POINT_T&

	\return	bool	
*/
bool CDT::IsInsideCircle(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3,const POINT_T& pt)
{
	bool bRet=false;

	POINT_T ptCenter=FindCircumCenter(pt1,pt2,pt3);
	double dx=ptCenter.x - pt.x;
	double dy=ptCenter.y - pt.y;
	double dz=ptCenter.z - pt.z;
	double dist1=sqrt(dx*dx + dy*dy + dz*dz);
	dx=ptCenter.x - pt1.x;
	dy=ptCenter.y - pt1.y;
	dz=ptCenter.z - pt1.z;
	double dist2=sqrt(dx*dx + dy*dy + dz*dz);
	if((dist1 - dist2) < -CLibMesh2d::TOLERANCE) bRet = true;

	return bRet;
}

/**	\brief	The == function

	\param	dt	a parameter of type const CDT&

	\return	bool CDT::operator	
*/
bool CDT::operator ==(CDT& dt)
{
	bool bRet=false;
	
	bool bMatchEdge[3]={false};
	POINT_T vt[2][3]={0};
	vt[0][0] = m_pt[0];
	vt[0][1] = m_pt[1];
	vt[0][2] = m_pt[2];
	vt[1][0] = dt[0];
	vt[1][1] = dt[1];
	vt[1][2] = dt[2];
	
	if(	((vt[0][0].x == vt[1][0].x) && (vt[0][0].y == vt[1][0].y) && (vt[0][0].z == vt[1][0].z)) || 
		((vt[0][0].x == vt[1][1].x) && (vt[0][0].y == vt[1][1].y) && (vt[0][0].z == vt[1][1].z)) ||
		((vt[0][0].x == vt[1][2].x) && (vt[0][0].y == vt[1][2].y) && (vt[0][0].z == vt[1][2].z)))
	{
		bMatchEdge[0] = true;
	}
	
	if(	((vt[0][1].x == vt[1][0].x) && (vt[0][1].y == vt[1][0].y) && (vt[0][1].z == vt[1][0].z)) || 
		((vt[0][1].x == vt[1][1].x) && (vt[0][1].y == vt[1][1].y) && (vt[0][1].z == vt[1][1].z)) ||
		((vt[0][1].x == vt[1][2].x) && (vt[0][1].y == vt[1][2].y) && (vt[0][1].z == vt[1][2].z)))
	{
		bMatchEdge[1] = true;
	}
	
	if(	((vt[0][2].x == vt[1][0].x) && (vt[0][2].y == vt[1][0].y) && (vt[0][2].z == vt[1][0].z)) || 
		((vt[0][2].x == vt[1][1].x) && (vt[0][2].y == vt[1][1].y) && (vt[0][2].z == vt[1][1].z)) ||
		((vt[0][2].x == vt[1][2].x) && (vt[0][2].y == vt[1][2].y) && (vt[0][2].z == vt[1][2].z)))
	{
		bMatchEdge[2] = true;
	}
	
	bRet = (bMatchEdge[0] && bMatchEdge[1] && bMatchEdge[2]);
	
	return bRet;
}

/**	\brief	The CDT::operator() function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&
	\param	pt3	a parameter of type const POINT_T&

	\return	void	
*/
void CDT::operator()(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3)
{
	m_nPoints = 3;

    	m_pt[0] = pt1;
	m_pt[1] = pt2;
	m_pt[2] = pt3;
}

/**	\brief	The CDT::operator[] function

	\param	nIndex	a parameter of type int

	\return	POINT_T&	
*/
POINT_T& CDT::operator[](int nIndex)
{
    	assert((nIndex >= 0) && "nIndex is less 0.");
	assert((nIndex < 3)  && "nIndex is greather than 3.");
       
	if((nIndex >= 0) && (nIndex < 3))
	{
	    	return m_pt[nIndex];
	}	    
	
	return m_pt[0];
}

/**	\brief	The operator== function

	\param	dt1	a parameter of type CDT&
	\param	dt2	a parameter of type CDT&

	\return	bool	
*/
bool operator==(CDT& dt1,CDT& dt2)
{
	bool bRet=false;

	bool bMatchEdge[3]={false};
	POINT_T vt[2][3]={0};
	vt[0][0] = dt1[0];
	vt[0][1] = dt1[1];
	vt[0][2] = dt1[2];
	vt[1][0] = dt2[0];
	vt[1][1] = dt2[1];
	vt[1][2] = dt2[2];

	if(	((vt[0][0].x == vt[1][0].x) && (vt[0][0].y == vt[1][0].y) && (vt[0][0].z == vt[1][0].z)) || 
		((vt[0][0].x == vt[1][1].x) && (vt[0][0].y == vt[1][1].y) && (vt[0][0].z == vt[1][1].z)) ||
		((vt[0][0].x == vt[1][2].x) && (vt[0][0].y == vt[1][2].y) && (vt[0][0].z == vt[1][2].z)))
	{
			bMatchEdge[0] = true;
	}

	if(	((vt[0][1].x == vt[1][0].x) && (vt[0][1].y == vt[1][0].y) && (vt[0][1].z == vt[1][0].z)) || 
		((vt[0][1].x == vt[1][1].x) && (vt[0][1].y == vt[1][1].y) && (vt[0][1].z == vt[1][1].z)) ||
		((vt[0][1].x == vt[1][2].x) && (vt[0][1].y == vt[1][2].y) && (vt[0][1].z == vt[1][2].z)))
	{
			bMatchEdge[1] = true;
	}

	if(	((vt[0][2].x == vt[1][0].x) && (vt[0][2].y == vt[1][0].y) && (vt[0][2].z == vt[1][0].z)) || 
		((vt[0][2].x == vt[1][1].x) && (vt[0][2].y == vt[1][1].y) && (vt[0][2].z == vt[1][1].z)) ||
		((vt[0][2].x == vt[1][2].x) && (vt[0][2].y == vt[1][2].y) && (vt[0][2].z == vt[1][2].z)))
	{
			bMatchEdge[2] = true;
	}

	bRet = (bMatchEdge[0] && bMatchEdge[1] && bMatchEdge[2]);

	return bRet;
}

double _distance(const POINT_T& pt1,const POINT_T& pt2)
{
	const double dx = pt1.x - pt2.x;
	const double dy = pt1.y - pt2.y;
	const double dz = pt1.z - pt2.z;
	
	return sqrt(dx*dx + dy*dy + dz*dz);
}

/**	\brief	The CDT::FindIndexOfSmallestAngle function


	\return	int	
*/
int CDT::FindIndexOfSmallestAngle()
{
	double e[3]={0};

	for(int i=0;i < 3;i++)
		e[i] = _distance(m_pt[(i + 1)%3],m_pt[(i + 2)%3]);
	
	int i = 0;
	for(i = 0;i < 3;i++)
	{
		if((e[i] <= e[(i + 1)%3]) && (e[i] <= e[(i + 2)%3])) break;
	}

	return i;
}

/**	\brief	The CDT::Angle function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&
	\param	pt3	a parameter of type const POINT_T&

	\return	double	
*/
double CDT::Angle(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3)
{
	double ux=pt2.x - pt1.x;
	double uy=pt2.y - pt1.y;
	double vx=pt3.x - pt1.x;
	double vy=pt3.y - pt1.y;

	double dot=CMath::DotProduct(ux,uy,0.,vx,vy,0.);
	return acos(sqrt((dot*dot)/((ux*ux + uy*uy)*(vx*vx + vy*vy))))*(180./PI);
}

/**	\brief	The CDT::HasBadAngle function


	\return	bool	
*/
bool CDT::HasBadAngle()
{
	int nIndex=FindIndexOfSmallestAngle();
	double angle=Angle(m_pt[nIndex],m_pt[(nIndex + 1)%3],m_pt[(nIndex + 2)%3]);
	return (angle < CDT::BAD_ANGLE);
}

/**	\brief	The CDT::WhereLocatedPoint function

	\param	pt	a parameter of type const POINT_T&

	\return	LOCATE_T	
*/
LOCATE_T CDT::WhereLocatedPoint(const POINT_T& pt)
{
//	if((fabs(m_pt[0].x-pt.x) < CLibMesh2d::TOLERANCE) && (fabs(m_pt[0].y-pt.y) < CLibMesh2d::TOLERANCE)) return LOC_ON_POINT;
//	if((fabs(m_pt[1].x-pt.x) < CLibMesh2d::TOLERANCE) && (fabs(m_pt[1].y-pt.y) < CLibMesh2d::TOLERANCE)) return LOC_ON_POINT;
//	if((fabs(m_pt[2].x-pt.x) < CLibMesh2d::TOLERANCE) && (fabs(m_pt[2].y-pt.y) < CLibMesh2d::TOLERANCE)) return LOC_ON_POINT;
	if(0 == memcmp(&(m_pt[0]),&pt,sizeof(POINT_T))) return LOC_ON_POINT;
	if(0 == memcmp(&(m_pt[1]),&pt,sizeof(POINT_T))) return LOC_ON_POINT;
	if(0 == memcmp(&(m_pt[2]),&pt,sizeof(POINT_T))) return LOC_ON_POINT;
	
	if(CGeometry::IsPointOnLine(pt.x,pt.y,m_pt[0].x,m_pt[0].y,m_pt[1].x,m_pt[1].y)) return LOC_ON_EDGE;
	if(CGeometry::IsPointOnLine(pt.x,pt.y,m_pt[1].x,m_pt[1].y,m_pt[2].x,m_pt[2].y)) return LOC_ON_EDGE;
	if(CGeometry::IsPointOnLine(pt.x,pt.y,m_pt[2].x,m_pt[2].y,m_pt[0].x,m_pt[0].y)) return LOC_ON_EDGE;
	
	static POINT_T _pt[4]={0};
	for(int i=0;i < m_nPoints;i++)
	{
		_pt[i].x = m_pt[i].x - pt.x;
		_pt[i].y = m_pt[i].y - pt.y;
	}
	int i1=0;
	int Rcross = 0; // number of right edge/ray crossings
	int Lcross = 0; // number of left edge/ray crossings
	for(int i = 0;i < m_nPoints;i++) 
	{
		i1 = (i + m_nPoints - 1 )%m_nPoints;
		if((_pt[i].y > 0. ) != (_pt[i1].y > 0.)) 
		{
			double x=(_pt[i].x * (double)_pt[i1].y - _pt[i1].x * (double)_pt[i].y)/(double)(_pt[i1].y - _pt[i].y);

			if (x >= 0.) Rcross++;
		}
    
		if((_pt[i].y < 0) != (_pt[i1].y < 0)) 
		{ 
			double x=(_pt[i].x * _pt[i1].y - _pt[i1].x * _pt[i].y)/(double)(_pt[i1].y - _pt[i].y);

			if (x <= 0.) Lcross++;
		}
	}
  
	if((Rcross%2 ) != (Lcross%2)) return LOC_INNER_POINT;
	if((Rcross % 2) == 1) return LOC_INNER_POINT;
	else		      return LOC_OUTER_POINT;
}

void CDT::Set(const POINT_T &pt1, const POINT_T &pt2, const POINT_T &pt3)
{
	m_nPoints = 3;
	m_bDeleted = false;
	
	m_pt[0].x = _round(pt1.x,9);m_pt[0].y = _round(pt1.y,9);m_pt[0].z = _round(pt1.z,9);
	m_pt[1].x = _round(pt2.x,9);m_pt[1].y = _round(pt2.y,9);m_pt[1].z = _round(pt2.z,9);
	m_pt[2].x = _round(pt3.x,9);m_pt[2].y = _round(pt3.y,9);m_pt[2].z = _round(pt3.z,9);
	m_nEdgeId[0] = m_nEdgeId[1] = m_nEdgeId[2] = 0;
	m_nArea = -1;
}
