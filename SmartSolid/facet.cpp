#include "./libmesh/libmesh2d.h"
#include "facet.h"

/**	\brief
*/
CFacet::CFacet()
{
	m_nPoints  = 0;
	m_bShow[0] = true;
	m_bDeleted = false;

	m_rect.x = 0;
	m_rect.y = 0;
	m_rect.width = -1.f;
	m_rect.height= -1.f;
}

/**	\brief
*/
CFacet::CFacet(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3)
{
	m_nPoints = 3;

	m_pt[0].x=_round(pt1.x,9);
	m_pt[0].y=_round(pt1.y,9);
	m_pt[0].z=_round(pt1.z,9);
	m_bShow[0] = true;
	m_pt[1].x=_round(pt2.x,9);
	m_pt[1].y=_round(pt2.y,9);
	m_pt[1].z=_round(pt2.z,9);
	m_bShow[1] = true;
	m_pt[2].x=_round(pt3.x,9);
	m_pt[2].y=_round(pt3.y,9);
	m_pt[2].z=_round(pt3.z,9);
	m_bShow[2] = true;

	m_bDeleted = false;

	CalcRect();
}

/**	\brief
*/
CFacet::CFacet(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3,POINT_T& pt4)
{
	m_nPoints = 4;

	m_pt[0].x=_round(pt1.x,9);
	m_pt[0].y=_round(pt1.y,9);
	m_pt[0].z=_round(pt1.z,9);
	m_bShow[0] = true;
	m_pt[1].x=_round(pt2.x,9);
	m_pt[1].y=_round(pt2.y,9);
	m_pt[1].z=_round(pt2.z,9);
	m_bShow[1] = true;
	m_pt[2].x=_round(pt3.x,9);
	m_pt[2].y=_round(pt3.y,9);
	m_pt[2].z=_round(pt3.z,9);
	m_bShow[2] = true;
	m_pt[3].x=_round(pt4.x,9);
	m_pt[3].y=_round(pt4.y,9);
	m_pt[3].z=_round(pt4.z,9);
	m_bShow[3] = true;

	m_bDeleted = false;

	CalcRect();
}

/**
*/
CFacet& CFacet::operator=(const CFacet& rhs)
{
	if(this != &rhs)
	{
		m_nId     = rhs.m_nId;
		m_nPoints = rhs.m_nPoints;
		memcpy(m_pt , rhs.m_pt , sizeof(POINT_T)*m_nPoints);
		memcpy(m_bShow , rhs.m_bShow , sizeof(bool)*m_nPoints);
		
		m_bDeleted = rhs.m_bDeleted;

		m_rect = rhs.m_rect;
	}

	return (*this);
}

/**	@brief
*/
CFacet::CFacet(const CFacet& facet)
{
	(*this) = facet;
}

/**	\brief
*/
CFacet::~CFacet()
{
}

/**
	@brief	facet의 영역을 구한다.
	@author	백흠경

	@return	RECT_T(facet의 영역)
*/
void CFacet::CalcRect()
{
	double minx = m_pt[0].x , miny = m_pt[0].y;
	double maxx = m_pt[0].x , maxy = m_pt[0].y;
	if(m_pt[1].x < minx) minx = m_pt[1].x;
	if(m_pt[1].y < miny) miny = m_pt[1].y;
	if(m_pt[1].x > maxx) maxx = m_pt[1].x;
	if(m_pt[1].y > maxy) maxy = m_pt[1].y;

	if(m_pt[2].x < minx) minx = m_pt[2].x;
	if(m_pt[2].y < miny) miny = m_pt[2].y;
	if(m_pt[2].x > maxx) maxx = m_pt[2].x;
	if(m_pt[2].y > maxy) maxy = m_pt[2].y;
	
	m_rect.x = minx;
	m_rect.y = miny;
	m_rect.width = maxx - minx;
	m_rect.height= maxy - miny;
}

/**
	@brief	FACET의 영역값 리턴한다.
	@author	백흠경

	@return	RECT_T
*/
RECT_T CFacet::rect() const
{
	return m_rect;
}
