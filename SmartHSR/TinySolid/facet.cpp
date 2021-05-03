#include "./libmesh/libmesh2d.h"
#include "facet.h"

/**	\brief
*/
CFacet::CFacet(){
	m_nPoints  = 0;
	m_bShow[0] = true;
	m_bDeleted = false;
}

/**	\brief
*/
CFacet::CFacet(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3){
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
}

/**	\brief
*/
CFacet::CFacet(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3,POINT_T& pt4){
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
}

/**	\brief
*/
CFacet::CFacet(const CFacet& facet){
	m_nId = facet.m_nId;
	m_nPoints = facet.m_nPoints;
	memcpy(m_pt,facet.m_pt,sizeof(POINT_T)*m_nPoints);
	memcpy(m_bShow,facet.m_bShow,sizeof(bool)*m_nPoints);

	m_bDeleted = facet.m_bDeleted;
}

/**	\brief
*/
CFacet::~CFacet(){
}
