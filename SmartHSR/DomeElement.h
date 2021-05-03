// PreDOMEFormat.h: interface for the CPreDOMEFormat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREDOMEFORMAT_H__DD11F7FA_727C_42AF_A5C3_F20A7DFA092F__INCLUDED_)
#define AFX_PREDOMEFORMAT_H__DD11F7FA_727C_42AF_A5C3_F20A7DFA092F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "Element.h"
#include "HSRFace.h"
#include "HSRScanner.h"

class CHSRViewFormat;
class CDomeElement : public CHSRElement
{
public:
	CDomeElement();
	virtual ~CDomeElement();
public:
	bool ParseLine(CHSRScanner* pGPreFormatScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);

	DECLARE_HSR_ELEMENT(CDomeElement);
public:
	int m_nSegments;
private:
	double m_nRadius;
	double m_nHeight;
	POINT_T m_ptOrigin;
	VECTOR_T m_vecNorm;
};

#endif // !defined(AFX_PREDOMEFORMAT_H__DD11F7FA_727C_42AF_A5C3_F20A7DFA092F__INCLUDED_)
