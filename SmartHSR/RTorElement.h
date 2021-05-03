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
#include "HSRScanner.h"

class CHSRViewFormat;
class CRTorElement : public CHSRElement
{
public:
	CRTorElement();
	virtual ~CRTorElement();
public:
	bool ParseLine(CHSRScanner* pGPreFormatScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);

	DECLARE_HSR_ELEMENT(CRTorElement);
public:
	int m_nSegments;
private:
	POINT_T  m_pt[2];
	POINT_T  m_ptOrigin;
	VECTOR_T m_vecNorm;
	double   m_nWidth;
	double   m_nHeight;
};

#endif // !defined(AFX_PRERTORFORMAT_H__E494FE45_524D_415B_B7A3_DC1B7AB0B857__INCLUDED_)
