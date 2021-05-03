/********************************************************************
	created:	2002/01/01
	filename: 	D:\Project\PlantWave\hiddenDll\HSRExport.h
	file path:	D:\Project\PlantWave\hiddenDll
	file base:	HSRExport
	file ext:	h
	author:		
	
	purpose	:
	update	:	[2002.12.17] - add IsSingleLine(const double nRadius)
*********************************************************************/

#if !defined(AFX_KEXPORT_H__6BDD01E6_D0AF_43CC_8E11_CC053A293838__INCLUDED_)
#define AFX_KEXPORT_H__6BDD01E6_D0AF_43CC_8E11_CC053A293838__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <IsTools.h>

#include <SQLib.h>
#include "HiddenDef.h"
#include "HSRFace.h"
#include "HSRRule.h"
#include "ViewBoundary.h"

#include <list>
using namespace std;

class CHSRExport
{
public:
	CHSRExport();
	virtual ~CHSRExport();
public:
	STRING_T GetVersion() const
	{
		return m_sVersion;
	}

	bool Add(HSR_LAYER_E enmLineKind,RENDERTYPE render,PSQPRIMITIVE pPrimitive);
	void WriteViewBoundary();
	void WriteViewBoundary(CViewBoundary* pViewBoundray);
	void Clear();
	virtual bool WritePrimitiveAsAsciiFile(LPCTSTR pFileName);
	//bool WritePrimitiveAsBinaryFile(LPCTSTR pFileName);
	PSQPRIMITIVE WriteFaceListToPrimitiveList(CHSRFace* pFaceList);
private:
	inline bool IsSingleLine(const double nRadius);
	PSQPRIMITIVE WritePipeToDWG(PSQPRIMITIVE ret,CHSRFace* pVertex);
	PSQPRIMITIVE WriteElbowToDWG(PSQPRIMITIVE ret,CHSRFace* pVertex);
	PSQPRIMITIVE WriteSWElbowToDWG(PSQPRIMITIVE ret,CHSRFace* pFace);
	PSQPRIMITIVE WriteTeeToDWG(PSQPRIMITIVE ret,CHSRFace* pVertex);
	PSQPRIMITIVE WriteTeeXToDWG(PSQPRIMITIVE ret,CHSRFace* pVertex);

	void WriteVertexListToDWG(CHSRFace* pFace , PHSR_VERTEX pVertex,HSR_LAYER_E kind);
	PSQPRIMITIVE WritePolylineToDWG(PSQPRIMITIVE ret,PHSR_VERTEX svt,PHSR_VERTEX evt,CHSRElement* body,const int& ltype,const int& lcolor);

	void WriteVertexListAsCenterlineToDWG(CHSRFace* pFace , PHSR_VERTEX pVertex,HSR_LAYER_E kind);

	PHSR_VERTEX FindStartVertex(PHSR_VERTEX pVertex);
private:
	PSQPRIMITIVE WriteFaceToPrimitive(PSQPRIMITIVE ret, CHSRFace* pFace,HSR_LAYER_E kind);
	PSQPRIMITIVE WriteDomeToDWG(PSQPRIMITIVE pPrimitiveList,CHSRFace* pFace,HSR_LAYER_E kind);
	bool WriteElbowSymbolToDWG(CHSRFace* pFace,HSR_LAYER_E kind,int ltype,int lcolor);
	PSQPRIMITIVE WriteBoxToDWG(PSQPRIMITIVE pPrimitiveList,CHSRFace* pFace,HSR_LAYER_E kind);
private:
	PSQPRIMITIVE WriteMiterToDWG(PSQPRIMITIVE ret,CHSRFace* pFace);
	PSQPRIMITIVE WriteArcToDWG(PSQPRIMITIVE ret , CHSRFace* pFace , PHSR_VERTEX pStartVt,PHSR_VERTEX pEndVt);
	CHSRRule m_clsRule;

	const static double _TOLER;
protected:
	int GetEdgeCount(PSQPRIMITIVE pPrimitive);

	PSQPRIMITIVE m_pShowPrimitiveKind[24],m_pHiddenPrimitiveKind[24];
	STRING_T m_sVersion;
};

#endif // !defined(AFX_KEXPORT_H__6BDD01E6_D0AF_43CC_8E11_CC053A293838__INCLUDED_)
