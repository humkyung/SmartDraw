#ifndef __KMESH_H__
#define __KMESH_H__

#include <array.h>
#include <SQLib.h>
#include "KTriangle.h"
#include <list>
using namespace std;

class KMesh{
    	typedef struct tagEdge{
	    	SQPOINT ptStart;
		SQPOINT ptEnd;
	}EDGE,* PEDGE;
public:
	KMesh(int nShape,KArray<SQPOINT>** paryShape,int nBoundary,KArray<SQPOINT>** paryBoundary);
	~KMesh();
public:
	list<KTriangle>* Generate();
private:
	bool GetMeshTrianglePoint(SQPOINT& pt,EDGE& edge,list<KTriangle>& lstTriangle);
	double GetLength(EDGE& edge,SQPOINT& pt);
	bool IsExistEqualOrStraddleTriangle(list<KTriangle>& lstTriangle,KTriangle& triangle);
	bool IsPointsOnOneBoundary(SQPOINT& pt1,SQPOINT& pt2,SQPOINT& pt3);
	bool IsBoundaryEdge(EDGE& edge);
	bool IsPointOnEdge(EDGE& edge,SQPOINT& pt);
private:
	int m_nShape,m_nBoundary;
	KArray<SQPOINT>** m_paryShape;
	KArray<SQPOINT>** m_paryBoundary;
};

#endif
