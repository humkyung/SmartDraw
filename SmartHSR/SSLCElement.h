// SSLCElement.h: interface for the CSSLCElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SSLCELEMENT_H__970F5FEC_4606_4E09_AAFF_15E1E399F689__INCLUDED_)
#define AFX_SSLCELEMENT_H__970F5FEC_4606_4E09_AAFF_15E1E399F689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"

class CSSLCElement : public CHSRElement  
{
public:
	CSSLCElement();
	virtual ~CSSLCElement();
public:
	bool ParseLine(CHSRScanner* pGPreFormatScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);

	DECLARE_HSR_ELEMENT(CSSLCElement);
public:
	int m_nSegments;
protected:
	void Volume();
private:
	HSR_PRIMITIVE_E m_eType;

	double  m_nRadius;
	POINT_T m_pt[2];
	double  m_nRotate;
	double  m_nXInc[2];
	double  m_nYInc[2];

	double   m_matRotation[3][3];
};

#endif // !defined(AFX_SSLCELEMENT_H__970F5FEC_4606_4E09_AAFF_15E1E399F689__INCLUDED_)
