/********************************************************************
	created:	2004/01/16
	created:	16:1:2004   14:14
	filename: 	D:\Project\PlantWAVE\OpenSolid\facet.h
	file path:	D:\Project\PlantWAVE\OpenSolid
	file base:	facet
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __OPEN3D_FACET_H__
#define __OPEN3D_FACET_H__

#include <SQLib.h>

#define MAXFACETPOINTS	4

class CFacet{
public:
	CFacet();
	CFacet(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3);
	CFacet(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3,POINT_T& pt4);
	CFacet(const CFacet& facet);
	virtual ~CFacet();
public:
	int  m_nPoints;
	bool m_bShow[MAXFACETPOINTS];
	bool m_bDeleted;
	POINT_T m_pt[MAXFACETPOINTS];

	unsigned long m_nId;
};

#endif