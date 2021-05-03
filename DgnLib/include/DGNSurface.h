// DGNSurface.h: interface for the CDGNSurface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGNSURFACE_H__9D1EE907_0A70_4763_A57D_7529B1EE62ED__INCLUDED_)
#define AFX_DGNSURFACE_H__9D1EE907_0A70_4763_A57D_7529B1EE62ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQLib.h>
#include "DGNLib.h"
#include "DGNElement.h"
#include "DGNFile.h"

class CDGNSurface : public CDGNElement
{
public:
	CDGNSurface();
	virtual ~CDGNSurface();
public:
	VOLUME_T Volume(PSQPRIMITIVE pPrimitive);
	bool StoreHeader(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
	int  GetNumOfElements(PSQPRIMITIVE pPrimitive);
	
	long size(PSQPRIMITIVE pPrimitive);
	long Write(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
protected:
};

#endif // !defined(AFX_DGNSURFACE_H__9D1EE907_0A70_4763_A57D_7529B1EE62ED__INCLUDED_)
