// DGNArc.h: interface for the CDGNArc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGNARC_H__854D4DE5_C69E_4177_9A96_0F898619D543__INCLUDED_)
#define AFX_DGNARC_H__854D4DE5_C69E_4177_9A96_0F898619D543__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQLib.h>
#include <DGNElement.h>
#include "DGNLib.h"
using namespace itk;

class CDGNArc : public CDGNElement
{
public:
	CDGNArc();
	virtual ~CDGNArc();
public:
	static long Parse(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);
	
	long size(PSQPRIMITIVE pPrimitive);
	VOLUME_T Volume(PSQPRIMITIVE pPrimitive);
	long Write(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);
private:
	long Write2D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);
	long Write3D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);
	
	QUAT_T  GetQuaternion(PSQPRIMITIVE pPrimitive);
	POINT_T GetEndPoint(PSQPRIMITIVE pPrimitive);
};

#endif // !defined(AFX_DGNARC_H__854D4DE5_C69E_4177_9A96_0F898619D543__INCLUDED_)
