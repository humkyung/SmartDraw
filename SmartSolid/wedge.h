/********************************************************************
	created:	2004/01/16
	created:	16:1:2004   13:21
	filename: 	D:\Project\PlantWAVE\OpenSolid\wedge.h
	file path:	D:\Project\PlantWAVE\OpenSolid
	file base:	wedge
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __OPEN3D_WEDGE_H__
#define __OPEN3D_WEDGE_H__

#include "3DS.h"

typedef struct tagEdgeAttr{
    	unsigned char visible : 1;
	unsigned char r       : 7;
}EDGEATTR,* PEDGEATTR;

class CWEdge{
public:
    	CWEdge(PVERTEX pStart=NULL,PVERTEX pEnd=NULL,CFace3D* pParent=NULL);
	virtual ~CWEdge();
public:
	void operator+=(CWEdge* pWEdge);
	CFace3D* GetParent();
	void SetParent(CFace3D* pFace);
	double Length();
	void operator()(PVERTEX pStart,PVERTEX pEnd);
	CWEdge* Prev();
	CWEdge* Next();
public:
	EDGEATTR m_Attr;
	PVERTEX  m_pvtxStart;	// pointer of start vertex 
	PVERTEX  m_pvtxEnd;	// pointer of end vertex 

	friend class CFace3D;
protected:
	CFace3D* m_pParent;
	CWEdge *m_pPrev,*m_pNext;
};


#endif
