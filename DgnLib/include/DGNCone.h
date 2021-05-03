// DGNCone.h: interface for the CDGNCone class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGNCONE_H__78851320_EC0B_4BA2_9DDD_D5F961668C6F__INCLUDED_)
#define AFX_DGNCONE_H__78851320_EC0B_4BA2_9DDD_D5F961668C6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQLib.h>
#include "DGNLib.h"
#include <DGNElement.h>

class CDGNCone : public CDGNElement
{
public:
	CDGNCone();
	virtual ~CDGNCone();
public:
	static long Parse(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);

	long size(PSQPRIMITIVE pPrimitive);
	VOLUME_T Volume(PSQPRIMITIVE pPrimitive){
		VOLUME_T varRet={0};return varRet;
	}
	bool StoreHeader(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
	long Write(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive);
protected:
	void CalculateVolumeBox(SQVOLUME& volume,PSQPRIMITIVE pPrimitive);
	QUAT_T GetQuaternion(PSQPRIMITIVE pPrimitive);
};

#endif // !defined(AFX_DGNCONE_H__78851320_EC0B_4BA2_9DDD_D5F961668C6F__INCLUDED_)
