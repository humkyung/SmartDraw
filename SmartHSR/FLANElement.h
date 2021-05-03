// FLANElement.h: interface for the CFLANElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLANELEMENT_H__6E122540_4F11_4A1F_9A9F_2E5013132724__INCLUDED_)
#define AFX_FLANELEMENT_H__6E122540_4F11_4A1F_9A9F_2E5013132724__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"

class CFLANElement : public CHSRElement  
{
public:
	CFLANElement();
	virtual ~CFLANElement();
public:
	bool ParseLine(CHSRScanner* pHSRScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);

	DECLARE_HSR_ELEMENT(CFLANElement);
public:
	int m_nSegments;
private:
	int     m_nPIPEs;
	double  m_nRadius[2];
	POINT_T m_pt[2][2];
};

#endif // !defined(AFX_FLANELEMENT_H__6E122540_4F11_4A1F_9A9F_2E5013132724__INCLUDED_)
