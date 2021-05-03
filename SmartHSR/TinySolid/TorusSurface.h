/********************************************************************
	created:	2002/04/10
	created:	10:4:2002   18:15
	filename: 	D:\Project\OpenSolid\KTorusSurface.h
	file path:	D:\Project\OpenSolid
	file base:	KTorusSurface
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __OPEN3D_TORUSSURFACE_H__
#define __OPEN3D_TORUSSURFACE_H__

#include <SQLib.h>
#include "PolyCurve.h"
#include "surface3d.h"
#include "libmesh/libmesh2d.h"
#include "libmesh/DT.h"
#include "libmesh/TinyFacetContainer.h"

#include <list>
using namespace mesh;
using namespace std;

class CTorusSurface : public CSurface3D{
public:
	CTorusSurface();
	CTorusSurface(SQTORUSSURFACE& torussurface,bool bReversed=false);
	~CTorusSurface();
public:
	CTinyFacetContainer* CreateFacets();
	list<CFacet*>* GetFacets();
protected:
	bool OnCreate();
	void CreateBoundaryCurvePoint(CMeshPos2d aryPara[],int& nPoints);
	void CreateBoundary(PSQPRIMITIVE pPrimitive);
	POINT_T func(CSurfacePara& para);
	POINT_T func(double u,double v);
	CMeshPos2d func(POINT_T& pt);

	bool IsBoundaryEdge(EDGE_T edge);
	bool HasSegment(list<EDGE>* plstEdge,const POINT_T& pt1,const POINT_T& pt2);
	CDT* FindTriangleHasEdge(int& nIndex,list<CDT*>* plstDT,EDGE_T& edge);
	int  IsOutboundTriangle(int& nOutboundIndex,EDGE_T& BoundaryEdge,CDT* pTriangle);
	void RemoveOutboundTriangles(list<CDT*>* plstTriangle);
private:
	bool m_bReversed;
	SQTORUSSURFACE m_sfcTorus;
	
	double m_nUStartAngle,m_nUEndAngle;
	double m_nUStepAngle,m_nVStepAngle;
	double m_nUOffsetAngle;

	int m_nBoundary;
	BOUNDARY_T* m_paryBoundary;

	list<CFacet*>     m_lstFacet;
};

#endif