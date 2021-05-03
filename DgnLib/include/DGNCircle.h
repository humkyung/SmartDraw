// DGNCircle.h: interface for the CDGNCircle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGNCIRCLE_H__0AA66247_2F44_4A00_9685_FC3F6BD224A8__INCLUDED_)
#define AFX_DGNCIRCLE_H__0AA66247_2F44_4A00_9685_FC3F6BD224A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <DGNElement.h>
using namespace itk;

class CDGNCircle : public CDGNElement  
{
public:
	CDGNCircle();
	virtual ~CDGNCircle();
public:
	static long Parse(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);

	long size(PSQPRIMITIVE pPrimitive);
	VOLUME_T Volume(PSQPRIMITIVE pPrimitive);
	long Write(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
private:
	long Write2D(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
	long Write3D(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);

	QUAT_T GetQuaternion(PSQPRIMITIVE pPrimitive);
};

#endif // !defined(AFX_DGNCIRCLE_H__0AA66247_2F44_4A00_9685_FC3F6BD224A8__INCLUDED_)
