// DGNSolid.h: interface for the CDGNSolid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGNSOLID_H__92D1CEA0_8EFF_466C_9D82_A936B2AB796C__INCLUDED_)
#define AFX_DGNSOLID_H__92D1CEA0_8EFF_466C_9D82_A936B2AB796C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQLib.h>
#include "DGNLib.h"
#include <DGNElement.h>

class CDGNSolid : public CDGNElement
{
public:
	CDGNSolid();
	virtual ~CDGNSolid();
public:
	long size(PSQPRIMITIVE pPrimitive);
	VOLUME_T Volume(PSQPRIMITIVE pPrimitive){VOLUME_T varRet;return varRet;}
	bool StoreHeader(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
	int  GetNumOfElements(PSQPRIMITIVE pPrimitive);
	long Write(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
};

#endif // !defined(AFX_DGNSOLID_H__92D1CEA0_8EFF_466C_9D82_A936B2AB796C__INCLUDED_)
