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

#ifdef	OPENSOLID_EXPORTS
	#define	OPENSOLID_EXT_CLASS	__declspec(dllexport)
#else
	#define	OPENSOLID_EXT_CLASS	__declspec(dllimport)
#endif

#include <SQLib.h>

#define MAXFACETPOINTS	4

class OPENSOLID_EXT_CLASS CFacet
{
public:
	CFacet();
	CFacet(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3);
	CFacet(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3,POINT_T& pt4);
	CFacet& operator=(const CFacet& rhs);
	CFacet(const CFacet& facet);
	virtual ~CFacet();
protected:
	RECT_T m_rect;
public:
	RECT_T rect() const;
	int  m_nPoints;
	bool m_bShow[MAXFACETPOINTS];
	bool m_bDeleted;
	POINT_T m_pt[MAXFACETPOINTS];

	unsigned long m_nId;
private:
	void CalcRect();
};

#endif