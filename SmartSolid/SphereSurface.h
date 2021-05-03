/********************************************************************
	created:	2002/04/10
	created:	10:4:2002   19:08
	filename: 	d:\Project\PlantWAVE\Module\OpenSolid\CSphereSurface.h
	file path:	d:\Project\PlantWAVE\Module\OpenSolid
	file base:	CSphereSurface
	file ext:	h
	author:		
	
	purpose:	
	updated:	2006.01.20
*********************************************************************/

#ifndef __OPEN3D_SPHERESURFACE_H__
#define __OPEN3D_SPHERESURFACE_H__

#include "PolyCurve.h"
#include "surface3d.h"
#include "facet.h"
#include "libmesh/libmesh2d.h"
#include "libmesh/DT.h"
#include "libmesh/TinyFacetContainer.h"

#include <list>
using namespace mesh;
using namespace std;

class CSphereSurface : public CSurface3D
{
public:
	CSphereSurface();
	CSphereSurface(SQSPHERESURFACE& spheresurface,bool bReversed=false);
	~CSphereSurface();
public:
	CTinyFacetContainer* CreateFacets();
protected:
	bool OnCreate();
	void CreateBoundaryCurvePoint(CMeshPos2d aryPara[],int& nPoints);
	void CreateBoundary(PSQPRIMITIVE pBoundary);
	int  refine(CSurfacePara aryPara[],CSurfacePara& para1,CSurfacePara& para2);
	POINT_T func(CMeshPos2d& para);
	POINT_T func(double u,double v);
	CMeshPos2d func(const POINT_T& pt);

	bool HasSegment(list<EDGE_T>* plstEdge,const POINT_T& pt1,const POINT_T& pt2);
	int  IsOutboundTriangle(int& nOutboundIndex,CDT* pDT);
	void RemoveOutboundTriangles(list<CDT*>* plstDT);
private:
	bool m_bReversed;
	SQSPHERESURFACE m_sfcSphere;

	int        m_nBoundary;
	BOUNDARY_T*  m_paryBoundary;
};

#endif