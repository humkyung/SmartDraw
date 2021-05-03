/********************************************************************
	created:	2004/01/16
	created:	16:1:2004   13:26
	filename: 	D:\Project\PlantWAVE\OpenSolid\PolyCurve.h
	file path:	D:\Project\PlantWAVE\OpenSolid
	file base:	PolyCurve
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __OPEN3D_POLYCURVE_H__
#define __OPEN3D_POLYCURVE_H__

#include <SQLib.h>
#include <graphics/curve3d.h>
#include "libmesh/libmesh2d.h"
using namespace mesh;

class CPolyCurve : public CCurve3d{
public:
	CPolyCurve();
	CPolyCurve(long nPoints,POINT_T* ppt);
	~CPolyCurve();
public:
	long GetNumOfPoints();
	bool HasEdge(const POINT_T& ptStart,const POINT_T& ptEnd);
	bool HasSegment(const POINT_T& pt1,const POINT_T& pt2);
	void CreateSegments(){}

	void operator =(const VECTOR_T& vecNorm);
	bool operator ()(long nPoints,POINT_T* ppt);
	POINT_T& operator[](long nIndex);
protected:
	void OnDestroy();
protected:
	VECTOR_T m_vecNorm;
	long     m_nPoints;
	POINT_T* m_ppt;
};

#endif