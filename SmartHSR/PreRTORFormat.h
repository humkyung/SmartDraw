 // PreRTORFormat.h: interface for the CPreRTORFormat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRERTORFORMAT_H__E494FE45_524D_415B_B7A3_DC1B7AB0B857__INCLUDED_)
#define AFX_PRERTORFORMAT_H__E494FE45_524D_415B_B7A3_DC1B7AB0B857__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HiddenDef.h"
#include "Element.h"
#include "HSRFace.h"
#include "PreFormatScanner.h"

class CPreVIEWFormat;
class CPreRTORFormat : public CElement
{
public:
	CPreRTORFormat();
	virtual ~CPreRTORFormat();
public:
	bool ParseLine(CPreFormatScanner* pGPreFormatScanner);
	bool IsInVolume(const SQVOLUME volume);
	PFACE CreateFace(CPreVIEWFormat* pVIEWFormat,long& nID);
public:
	int m_nSegments;
private:
	SQPOINT  m_pt[2];
	SQPOINT  m_ptOrigin;
	VECTOR_T m_vecNorm;
	double   m_nWidth;
	double   m_nHeight;
};

#endif // !defined(AFX_PRERTORFORMAT_H__E494FE45_524D_415B_B7A3_DC1B7AB0B857__INCLUDED_)
