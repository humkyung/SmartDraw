// PlaneSurfElement.h: interface for the CPlaneSurfElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLANESURFELEMENT_H__9A7D3408_0E7D_4ABF_88FE_4C06887C17A7__INCLUDED_)
#define AFX_PLANESURFELEMENT_H__9A7D3408_0E7D_4ABF_88FE_4C06887C17A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"

class CPlaneSurfElement : public CHSRElement  
{
public:
	CPlaneSurfElement();
	virtual ~CPlaneSurfElement();

	virtual bool ParseLine(CHSRScanner* pHSRScanner);
	CHSRFace* CreateFace(CHSRViewFormat* pViewFormat,long& nID);
	bool IsInVolume(const CIsVolume& volume);

	DECLARE_HSR_ELEMENT(CPlaneSurfElement);
private:
	void CalcVolume();

	list<CHSRElement*> m_BoundaryElemEntry;
};

#endif // !defined(AFX_PLANESURFELEMENT_H__9A7D3408_0E7D_4ABF_88FE_4C06887C17A7__INCLUDED_)
