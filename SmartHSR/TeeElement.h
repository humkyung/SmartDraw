// PreTEEFormat.h: interface for the CTeeElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRETEEFORMAT_H__064C7F53_6BDA_47B7_B73E_679E84578BF9__INCLUDED_)
#define AFX_PRETEEFORMAT_H__064C7F53_6BDA_47B7_B73E_679E84578BF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "HSRFace.h"
#include "Element.h"
#include "HSRScanner.h"

#define MAX_TEE_PARTS	5

class CHSRViewFormat;
class CTeeElement : public CHSRElement
{
public:
	CTeeElement();
	virtual ~CTeeElement();
public:
	bool ParseLine(CHSRScanner* pPreFormatScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);

	DECLARE_HSR_ELEMENT(CTeeElement);
private:
	bool Adjust();
	bool ParseBWLine(CHSRScanner* pPreFormatScanner);
	bool ParseSWLine(CHSRScanner* pPreFormatScanner);
	CHSRFace* CreateBWFace(CHSRViewFormat* pVIEWFormat,long& nID);
	CHSRFace* CreateSWFace(CHSRViewFormat* pVIEWFormat,long& nID);
	PHSR_VERTEX CreateTeeSymbol(const LINE_T &line, const double nRadius);
public:
	int m_nSegments;
private:
	int m_nPIPEs;
	bool m_bAdjusted;

	double m_nRadius[MAX_TEE_PARTS][2];
	POINT_T m_pt[MAX_TEE_PARTS][2];
};

#endif // !defined(AFX_PRETEEFORMAT_H__064C7F53_6BDA_47B7_B73E_679E84578BF9__INCLUDED_)
