/********************************************************************
	created:	2004/01/14
	created:	14:1:2004   14:15
	filename: 	D:\Project\PlantWAVE\OpenSolid\CTinyCone.h
	file path:	D:\Project\PlantWAVE\OpenSolid
	file base:	CTinyCone
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __OPEN3D_CONE_H__
#define __OPEN3D_CONE_H__

#include <SQLib.h>
#include "facet.h"
#include "body.h"
#include "libmesh/TinyFacetContainer.h"

#include <list>
using namespace std;

class CTinyCone/* : public CBody*/{
public:
	CTinyCone();
	CTinyCone(const CONE_T& cone);
	virtual ~CTinyCone();
public:
	static bool IsKindOf(CONE_T& cone,PSQPRIMITIVE pPrimitive);
	CTinyFacetContainer* CreateFacets();
protected:
	void OnCreate();
private:
	CONE_T m_sldCone;
};

#endif
