// HLineElement.h: interface for the CHLineElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HLINEELEMENT_H__9B4B8B48_2D4A_46BE_AED7_8DB43C747FD1__INCLUDED_)
#define AFX_HLINEELEMENT_H__9B4B8B48_2D4A_46BE_AED7_8DB43C747FD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "HSRFace.h"
#include "HSRScanner.h"
#include "HSRBoundingBox.h"	// Added by ClassView
#include "Element.h"

class CHLineElement : public CHSRElement
{
public:
	CHLineElement();
	virtual ~CHLineElement();
public:
	bool ParseLine(CHSRScanner* pHSRScanner);
	CHSRFace* CreateFace(CHSRViewFormat* pViewFormat,long& nID){ return NULL;}
	bool IsInVolume(const CIsVolume& volume);

	DECLARE_HSR_ELEMENT(CHLineElement);
protected:
	int CalcVolume();
public:
	CHSRBoundingBox GetBoundingBox();
	long    m_nGroup;
	POINT_T m_pt[2];
	bool    m_bVisible;
};

#endif // !defined(AFX_HLINEELEMENT_H__9B4B8B48_2D4A_46BE_AED7_8DB43C747FD1__INCLUDED_)
