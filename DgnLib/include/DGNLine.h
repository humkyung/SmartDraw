// DGNLine.h: interface for the CDGNLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGNLINE_H__A48C82AF_8120_4DF1_A28D_000D4955E9B9__INCLUDED_)
#define AFX_DGNLINE_H__A48C82AF_8120_4DF1_A28D_000D4955E9B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../TyPoint3d.h"
#include <DGNElement.h>

class CDGNLine : public CDGNElement  
{
public:
	CDGNLine();
	virtual ~CDGNLine();
public:
	bool Decode(CDGNFile* hFile);
	static long Parse(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);

	long size(PSQPRIMITIVE pPrimitive);
	VOLUME_T Volume(PSQPRIMITIVE pPrimitive);
	long Write(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);
private:
	long Write2D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);
	long Write3D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);

	CTyPoint3d m_start;
	CTyPoint3d m_end;
};

#endif // !defined(AFX_DGNLINE_H__A48C82AF_8120_4DF1_A28D_000D4955E9B9__INCLUDED_)
