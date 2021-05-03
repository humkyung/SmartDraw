/********************************************************************
	created:	2004/01/16
	created:	16:1:2004   12:19
	filename: 	D:\Project\PlantWAVE\OpenSolid\delaunay.h
	file path:	D:\Project\PlantWAVE\OpenSolid
	file base:	delaunay
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __OPEN3D_DELAUNAY_H__
#define __OPEN3D_DELAUNAY_H__

#include "DT.h"
#include "DelaunayEdge.h"

#include <list>
#include <queue>
using namespace std;
using namespace mesh;

class CDelaunay
{
public:
	CDelaunay();
	~CDelaunay();
public:
	void Initialize();
	bool AddDT(CDT* pTriangle);
	//void AppendDT(CDT* pDT);
	void Triangulate(POINT_T aryPoint[],long nPoints);
	void InsertPoints(POINT_T aryBoundaryPoint[],long nPoints);
	void InsertBoundary(LINE_T aryBoundary[],long nBoundary);
	list<CDT*>* GetListOfDT();
protected:
	void OnDestroy();

	void RemoveDuplicatedPoints(POINT_T aryPoint[],long& nPoints);
	void FindClosetPoints(CDelaunayEdge& edge,POINT_T aryPoint[],long nPoints);
	long FindIndexOfLeftPoint(POINT_T aryPoint[],long nPoints,POINT_T& pt1,POINT_T& pt2);
	long FindIndexOfDelaunayPoint(POINT_T aryPoint[],long nStart,long nPoints,POINT_T& pt1,POINT_T& pt2);
	bool DoOccurIntersection(CDelaunayEdge& edge1,CDelaunayEdge& edge2);
	double Distance(const POINT_T& pt1,const POINT_T& pt2);
	bool DoIntersect(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3,POINT_T& pt4);
	bool Collinear(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3);
	bool Left(POINT_T& pt1,POINT_T& pt2,POINT_T& check);
	bool Right(POINT_T& pt1,POINT_T& pt2,POINT_T& check);
	bool Xor(bool x,bool y);
	void CCW();
	bool IsLeftSidePoint(const POINT_T& pt,const LINE_T& line);
	bool AddEdge(const POINT_T& pt1,const POINT_T& pt2);
	void DeleteEdge(const POINT_T& pt1,const POINT_T& pt2);
	//bool PopEdge(CDelaunayEdge& edge);
	void PushEdge(queue<CDelaunayEdge>& queEdge,CDelaunayEdge& edge);
private:
	list<CDelaunayEdge> m_lstEdge,m_lstShareEdge;
	list<CDT*> m_lstDT;

	POINT_T m_ptCC[10240];
	long    m_nCC;

	static CDelaunayEdge _Edge[10240];
	static long _nEdge;
};

#endif