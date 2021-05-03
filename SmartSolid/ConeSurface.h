/********************************************************************
	created:	2002/04/10
	created:	10:4:2002   18:13
	filename: 	D:\Project\PlantWAVE\Module\OpenSolid\ConeSurface.h
	file path:	D:\Project\PlantWAVE\Module\OpenSolid
	file base:	ConeSurface
	file ext:	h
	author:		
	
	purpose:	
	updated:	[2003.07.10] - delete 'm_Boundary' variable in the class.
*********************************************************************/

#ifndef __OPEN3D_CONESURFACE_H__
#define __OPEN3D_CONESURFACE_H__

#include <SQLib.h>
#include "surface3d.h"
#include "facet.h"
#include "libmesh/libmesh2d.h"
#include "libmesh/grid.h"
#include "libmesh/TinyFacetContainer.h"
#include <IsPoint2d.h>

#include <list>
using namespace mesh;
using namespace std;

typedef enum
{
	TINY_SOLID_CONE = 1,
	TINY_SURF_CONE  = 2
}ENM_TINY_CONE_TYPE;

class CConeSurface : public CSurface3D
{
public:
	CConeSurface();
	CConeSurface(CONESURF_T& conesurface,bool bReversed=false);
	virtual ~CConeSurface();
public:
	CTinyFacetContainer* CreateFacets();
protected:
	void OnCreate();
	void CreateBoundaryCurvePoint(CMeshPos2d aryPara[],int& nPoints);
	int  CreateIntCurveBoundary(CMeshPos2d para[],PSQPRIMITIVE pPrimitive);
	int  CreateArcBoundary(CMeshPos2d para[],PSQPRIMITIVE pPrimitive);
	void CreateBoundary(/*PSQPRIMITIVE pPrimitive*/);
	bool HasSegment(list<EDGE>* plstEdge,const POINT_T& pt1,const POINT_T& pt2);
	
	void CreateGrids(list<CGrid*>& lstGrid,double tMin,double tMax , vector<CIsPoint2d*>& PointEntry);
	bool IsOutboundGrid(int& nIndex,CGrid* pGrid);
	CGrid* GetOutboundGrid(list<CGrid*>* plstGrid,const LINE_T& line);
	void RemoveOutboundGrids(list<CGrid*>* plstGrid);

	POINT_T func(double t,double phi);
	CMeshPos2d func(const POINT_T& pt);

	bool IsBoundaryEdge(const LINE_T& line);
	int GetNumOfBoundary();
private:
	bool m_bReversed;
	CONESURF_T m_sfcCone;
	double     m_nStartAngle;

	int        m_nBoundaryPoint;
	int        m_nBoundary;
	snode<LINE_T>* m_pEdge;

	list<CFacet*> m_lstFacet;
};

extern ENM_TINY_CONE_TYPE GetConeType(PSQPRIMITIVE pPrimitive);
#endif