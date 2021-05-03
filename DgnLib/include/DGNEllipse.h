// DGNEllipse.h: interface for the CDGNEllipse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGNELLIPSE_H__AE7B0C04_D74A_4F4C_BADA_C0576B5103BC__INCLUDED_)
#define AFX_DGNELLIPSE_H__AE7B0C04_D74A_4F4C_BADA_C0576B5103BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <DGNElement.h>

class CDGNFile;
class CDGNEllipse : public CDGNElement  
{
public:
	CDGNEllipse();
	virtual ~CDGNEllipse();
public:
	static long Parse(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);

	long size(PSQPRIMITIVE pPrimitive);
	VOLUME_T Volume(PSQPRIMITIVE pPrimitive);
	long Write(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);
private:
	long Write2D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);
	long Write3D(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);
	
	QUAT_T GetQuaternion(PSQPRIMITIVE pPrimitive);
};

#endif // !defined(AFX_DGNELLIPSE_H__AE7B0C04_D74A_4F4C_BADA_C0576B5103BC__INCLUDED_)
