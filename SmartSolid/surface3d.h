/********************************************************************
	created:	2002/04/10
	created:	10:4:2002   18:17
	filename: 	d:\Project\OpenSolid\CSurface3D.h
	file path:	d:\Project\OpenSolid
	file base:	CSurface3D
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __OPEN3D_SURFACE_H__
#define __OPEN3D_SURFACE_H__

#include <array.h>
#include "3DS.h"
#include <graphics/plane3d.h>
#include "SurfacePara.h"

#ifdef	OPENSOLID_EXPORTS
	#define	OPENSOLID_EXT_CLASS	__declspec(dllexport)
#else
	#define	OPENSOLID_EXT_CLASS	__declspec(dllimport)
#endif

#define MAX_SURF_BOUNDARY	10
#define	MAX_SURF_POINTS		1024

typedef enum{
	OUTER_BOUND=0,
	INNER_BOUND=1,
	ON_BOUND   =2
}BOUND_T;

class OPENSOLID_EXT_CLASS CSurface3D
{
public:
	CSurface3D(CFace3D* pFace=NULL,CBody* pParent=NULL);
	CSurface3D(int nParts,int nPoints,PPOINT_T pptPoints,CBody* pParent=NULL);
	CSurface3D(int idSurface,KArray<POINT_T>* aryBoundary,CBody* pParent=NULL);
	virtual ~CSurface3D();
public:
	//void Mesh(int nParts,int nPoints,PPOINT_T pptPoints);
	void operator+=(CFace3D* pFace);
	CBody* GetParent();
	void SetParent(CBody* pParent);
	CFace3D* GetFace();

	CSurface3D* Prev();
	CSurface3D* Next();

	friend class CBody;
protected:
	bool EnableToMakeFace(CPlane3d& plane,int nParts,int nPoints,PPOINT_T pptPoins);
	virtual POINT_T func(const CSurfacePara& para);
	//virtual CSurfacePara func(const POINT_T& pt);
protected:
	typedef struct tagEdge
	{
		CSurfacePara para[2];
	}EDGE;

	typedef struct tagBoundary{
		VECTOR_T    vecNorm;
		list<EDGE>* plstEdge;
	}BOUNDARY,* PBOUNDARY;

	long   m_nPoints,m_nFacets;
	static CSurfacePara para[1024];
	static POINT_T     pt[MAX_SURF_POINTS];
	static LINE_T      edge[1024];

	CFace3D* m_pFace;
	CBody* m_pParent;
	CSurface3D *m_pPrev,*m_pNext;

	friend class CTinyTorusSurfTester;
};

#endif
