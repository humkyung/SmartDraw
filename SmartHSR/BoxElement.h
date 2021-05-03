// BOXElement.h: interface for the CBOXElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOXELEMENT_H__855B8589_9E2B_43EF_9EF1_0A5E486DC66B__INCLUDED_)
#define AFX_BOXELEMENT_H__855B8589_9E2B_43EF_9EF1_0A5E486DC66B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"

class CBoxElement : public CHSRElement  
{
	typedef enum{
		BOX_RECTANGLE= 1,
		BOX_PHYRAMID = 2,
		BOX_TRIANGLE = 3
	}BOX_TYPE_E;
public:
	CBoxElement();
	virtual ~CBoxElement();
public:
	bool ParseLine(CHSRScanner* pHSRScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);

	DECLARE_HSR_ELEMENT(CBoxElement);
protected:
	void Volume();
public:
	BOX_TYPE_E m_eType;

	double   m_nLen[4];
	POINT_T  m_pt[2];
	VECTOR_T m_vec;
	VECTOR_T m_vecDist;
};

#endif // !defined(AFX_BOXELEMENT_H__855B8589_9E2B_43EF_9EF1_0A5E486DC66B__INCLUDED_)
