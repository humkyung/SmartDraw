/********************************************************************
	created:	2004/01/14
	created:	14:1:2004   14:15
	filename: 	D:\Project\PlantWAVE\OpenSolid\body.h
	file path:	D:\Project\PlantWAVE\OpenSolid
	file base:	CBody
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __OPEN3D_BODY_H__
#define __OPEN3D_BODY_H__

#include "3DS.h"

class CBody{
public:
	CBody();
	virtual ~CBody();
public:
	void operator+=(CSurface3D* pSurface);
	void DumpToFile(const char* pFilePath);
	CBody* Prev();
	CBody* Next();
	
protected:
	CSurface3D* m_pSurface;
	CBody *m_pPrev,*m_pNext;
};

#endif
	