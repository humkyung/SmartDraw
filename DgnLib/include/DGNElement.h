// DGNElement.h: interface for the CDGNElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGNELEMENT_H__D3A8A2E6_3B89_4401_B594_B0464FBAE455__INCLUDED_)
#define AFX_DGNELEMENT_H__D3A8A2E6_3B89_4401_B594_B0464FBAE455__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef	__DLL_EXPORT__
#define	__DLL_EXPORT__ __declspec(dllexport)
#endif

#include <SQLib.h>
#include "DGNEnum.h"
#include "../TyVolume.h"
#include "DGNAttrData.h"

#define SIZE_OF_LINE	30

class CDGNFile;
class __DLL_EXPORT__ CDGNElement 
{
public:
	CDGNElement();
	virtual ~CDGNElement();
public:
	virtual long size(PSQPRIMITIVE pPrimitive)=0;
	virtual VOLUME_T Volume(PSQPRIMITIVE pPrimitive)=0;
	virtual long Write(CDGNFile* hDGN, PSQPRIMITIVE pPrimitive)=0;
	bool operator =(PSQPRIMITIVE pPrimitive);

	bool GetHeader(CDGNFile* hDGN,PSQPRIMITIVE pPrimitive);
	static void WriteBounds(CDGNFile* hDGN,const VOLUME_T vol);
public:
	static void ParseHeader(PSQPRIMITIVE pPrimitive,CDGNFile* hDGN);
	static void ParseDMRS(PSQPRIMITIVE pPrimitive,CDGNFile* hDGN);
	void AddUserData(PDGNATTRDATA pUserData);
	virtual bool Encode(CDGNFile* hFile);
	virtual bool Decode(CDGNFile* hFile);
	int m_nType;
protected:
	PDGNATTRDATA GetDMRS(CDGNFile* hFile);

	DGNHDR m_header;
	CTyVolume m_volume;
	PDGNATTRDATA m_pUserData;
	PSQPRIMITIVE m_pPrimitive;
};

#endif // !defined(AFX_DGNELEMENT_H__D3A8A2E6_3B89_4401_B594_B0464FBAE455__INCLUDED_)
