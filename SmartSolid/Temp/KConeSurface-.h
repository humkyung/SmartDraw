#ifndef __KCONESURFACE_H__
#define __KCONESURFACE_H__

#include <SQLib.h>
#include "KSurface.h"
#include "KFacet.h"
#include "KGrid.h"

#include <list>
using namespace std;

class KConeSurface : public KSurface{
public:
	KConeSurface();
	KConeSurface(SQCONESURFACE& conesurface,bool bReversed=false);
	virtual ~KConeSurface();
public:
	list<KFacet*>* CreateFacets();
protected:
	void OnCreate();
	void CreateBoundaryCurvePoint(SURFACEPARA aryPara[],int& nPoints);
	void Reverse(SURFACEPARA para[],int nStart,int nEnd);
	int  CreateIntCurveBoundary(SURFACEPARA para[],PSQPRIMITIVE pPrimitive);
	int  CreateArcBoundary(SURFACEPARA para[],PSQPRIMITIVE pPrimitive);
	void CreateBoundary(PSQPRIMITIVE pPrimitive);
	bool HasSegment(list<EDGE>* plstEdge,const SQPOINT& pt1,const SQPOINT& pt2);
	
	void CreateGrids(list<KGrid*>& lstGrid,double tMin,double tMax);
	bool IsOutboundGrid(int& nIndex,KGrid* pGrid);
	KGrid* GetOutboundGrid(list<KGrid*>* plstGrid,const SQLINE& line);
	void RemoveOutboundTriangles(list<KDT*>* plstDT);
	void RemoveOutboundGrids(list<KGrid*>* plstGrid);

	SQPOINT func(double t,double phi);
	SURFACEPARA func(const SQPOINT& pt);

	bool IsBoundaryEdge(const SQLINE& line);
	bool IsSamePoint(const SQPOINT& pt1,const SQPOINT& pt2);
	int GetNumOfBoundary();
private:
	bool m_bReversed;
	SQCONESURFACE m_sfcCone;
	double     m_nStartAngle;

	int        m_nBoundaryPoint;
	int        m_nBoundary;
	BOUNDARY   m_Boundary[MAXBOUNDARY];
	snode<SQLINE>* m_pEdge;

	list<KFacet*> m_lstFacet;
};

#endif