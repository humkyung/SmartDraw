// DGNPolyline.h: interface for the CDGNPointString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGNPOLYLINE_H__E538BF54_B90B_4BFE_AF15_5093B7DC3143__INCLUDED_)
#define AFX_DGNPOLYLINE_H__E538BF54_B90B_4BFE_AF15_5093B7DC3143__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <DGNElement.h>

class CDGNPointString : public CDGNElement  
{
public:
	CDGNPointString();
	virtual ~CDGNPointString();
public:
	static long Parse(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);
	long size(PSQPRIMITIVE pPrimitive);
	VOLUME_T Volume(PSQPRIMITIVE pPrimitive);
	long Write(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
private:
	long Write2D(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
	long Write3D(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
};

#endif // !defined(AFX_DGNPOLYLINE_H__E538BF54_B90B_4BFE_AF15_5093B7DC3143__INCLUDED_)
