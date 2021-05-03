// DGNParser.h: interface for the CDGNParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGNPARSER_H__846224D3_B9B9_4189_AD8F_242E13B4DF21__INCLUDED_)
#define AFX_DGNPARSER_H__846224D3_B9B9_4189_AD8F_242E13B4DF21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef	DLL_EXPORT
#define	DLL_EXPORT __declspec(dllexport)
#endif

#include <SQLib.h>

class CDGNScanner;
class DLL_EXPORT CDGNParser  
{
public:
	CDGNParser();
	virtual ~CDGNParser();
public:
	PSQPRIMITIVE Parse(CDGNFile* hDGN);
	PSQPRIMITIVE Parse(CDGNFile* hDGN,PSQVOLUME pVolume);

	long ParseElement(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);
protected:
	bool ParseCore(CDGNFile* hDGN, DGNElemCore *psElement);
	long ParseColorTable(CDGNFile* hDGN);
	long ParseApplicationElements(CDGNFile* hDGN);

	long ParseComplexShapeOrChain(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);
	long ParseDimension(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);
	long ParseGroupData(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);
	long ParseSolid(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);
	long ParseSurface(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);
	long ParseBSplineSurfaceHeader(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);
	long ParseBSplinePole(POINT_T* pPoints,CDGNFile* hDGN);
	long ParseBSplineKnot(CDGNFile* hDGN,double* pKnot,const int nPoints);
	long ParseBSplineWeightFactor(CDGNFile* hDGN,double* pWeight,const int nPoints);
	long ParseBSplineSurfaceBoundary(PPOINT_T pPoints,CDGNFile* hDGN);
	long ParseCellHeader(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);
	long ParseTCB(CDGNFile* hDGN);
	long ParseRasterHeader(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);

	int  GetNumOfElements(PSQPRIMITIVE pPrimitive);

	PSQPRIMITIVE ClippingByVolume(PSQPRIMITIVE pPrimitive,PSQVOLUME pVolume);
	PSQPRIMITIVE ClippingPrimitiveByVolume(PSQPRIMITIVE pDest,PSQPRIMITIVE pSrc,PSQVOLUME pVolume);
	
	bool IsPipe(PSQPRIMITIVE pPrimitive);
	bool IsElbow(PSQPRIMITIVE pPrimitive);
	bool IsTee(PSQPRIMITIVE pPrimitive);
	bool IsEquipment(PSQPRIMITIVE pPrimitive);
private:
	bool IsCable(PSQPRIMITIVE pPrimitive);
	bool IsNozzle(PSQPRIMITIVE pPrimitive);
	bool IsInstrument(PSQPRIMITIVE pPrimitive) const;
	CDGNScanner* m_pDGNScanner;
};

#endif // !defined(AFX_DGNPARSER_H__846224D3_B9B9_4189_AD8F_242E13B4DF21__INCLUDED_)
