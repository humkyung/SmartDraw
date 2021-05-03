/********************************************************************
	created:	2004/01/16
	created:	16:1:2004   13:41
	filename: 	D:\Project\PlantWAVE\Module\OpenSolid\PlaneSurface.h
	file path:	D:\Project\PlantWAVE\Module\OpenSolid
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

#include <DelaunayMesh.h>

#include <vector>
using namespace std;
using namespace mesh;

#define MAX_PLANE_SURF_POINTS	2048

typedef struct
{
	int start , end;
}EdgeIndex;

class OPENSOLID_EXT_CLASS CPlaneSurface : public CSurface3D
{
public:
	CPlaneSurface();
	CPlaneSurface(PLANESURF_T& PlaneSurface);
	~CPlaneSurface();
public:
	CPolyCurve* GetBoundaryCurve(const long& index);
	CTinyFacetContainer* CreateFacets();
protected:
	void OnCreate();

	bool IsBoundaryEdge(const CIsPoint2d *ppt1,const CIsPoint2d *ppt2);
	bool AppendPoint(vector<POINT_T>& pts , POINT_T& pt);
private:
	PLANESURF_T m_sfcPlane;
	
	int m_nBoundary;

	vector<CPolyCurve*>* m_plstBoundaryCurve;

	long	m_nPoints,m_nEdge;

	vector<CIsPoint2d*>* m_pPointEntry;
	vector<pair<CIsPoint2d* , CIsPoint2d*> >* m_pDelaunayTriEdgeEntry;
	vector<EdgeIndex>* m_pEdgeIndexEntry;
};

#endif