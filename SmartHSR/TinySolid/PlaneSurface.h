/********************************************************************
	created:	2004/01/16
	created:	16:1:2004   13:41
	filename: 	D:\Project\PlantWAVE\OpenSolid\PlaneSurface.h
	file path:	D:\Project\PlantWAVE\OpenSolid
	file base:	PlaneSurface
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __OPEN3D_PLANESURFACE_H__
#define __OPEN3D_PLANESURFACE_H__

#include <SQLib.h>
#include "PolyCurve.h"
#include "surface3d.h"
#include "facet.h"
#include "libmesh/libmesh2d.h"
#include "libmesh/DT.h"
#include "libmesh/TinyFacetContainer.h"

#include <list>
using namespace std;
using namespace mesh;

#define MAX_PLANE_SURF_POINTS	524

class CPlaneSurface : public CSurface3D{
public:
	CPlaneSurface();
	CPlaneSurface(PLANESURF_T& PlaneSurface);
	~CPlaneSurface();
public:
	CTinyFacetContainer* CreateFacets();
protected:
	void OnCreate();

	bool IsBoundaryEdge(const POINT_T& pt1,const POINT_T& pt2);
	BOUND_T IsOutboundTriangle(int& nOutboundIndex,CDT* pDT);
	void RemoveOutboundTriangles(list<CDT*>* plstDT);
	bool AppendPoint(POINT_T& pt);
private:
	PLANESURF_T m_sfcPlane;
	
	int m_nBoundary;

	list<CPolyCurve*> m_lstBoundaryCurve;
	//list<CFacet*> m_lstFacet;

	long	m_nPoints,m_nEdge;
	POINT_T m_pt[MAX_PLANE_SURF_POINTS];
	LINE_T	m_lnBoundary[524];
};

#endif