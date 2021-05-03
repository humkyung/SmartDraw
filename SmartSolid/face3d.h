/********************************************************************
	created:	2004/01/16
	created:	16:1:2004   14:08
	filename: 	D:\Project\PlantWAVE\OpenSolid\face3d.h
	file path:	D:\Project\PlantWAVE\OpenSolid
	file base:	face3d
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __OPEN3D_FACE_H__
#define __OPEN3D_FACE_H__

#include "3DS.h"
#include "wedge.h"
#include <graphics/curve3d.h>

class CFace3D{
public:
	CFace3D(CWEdge* pWEdge=NULL,CSurface3D* pParent=NULL);
	CFace3D(int nParts,int nFacets,POINT_T* pPoints);
	virtual ~CFace3D();
public:
	CWEdge* operator()(int nIndex);
	void operator+=(CWEdge* pWEdge);

	CSurface3D* GetParent();
	void SetParent(CSurface3D* pParent);
	CWEdge* GetWEdge();

	CFace3D* Prev();
	CFace3D* Next();
public:
	SQVECTOR  m_vecNormal;
	
	friend class CSurface3D;
protected:
	CWEdge*   m_pWEdge;
	CSurface3D* m_pParent;
	CFace3D *m_pPrev,*m_pNext; 
};

#endif
