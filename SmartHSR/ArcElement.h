// ArcElement.h: interface for the CArcElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARCELEMENT_H__2A9083FD_41EE_4248_A5BE_B0AE2F6B4AB3__INCLUDED_)
#define AFX_ARCELEMENT_H__2A9083FD_41EE_4248_A5BE_B0AE2F6B4AB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "HSRFace.h"
#include "Element.h"
#include "HsrScanner.h"

#include <string>
using namespace std;

class CArcElement : public CHSRElement
{
public:
	int CreateLineElements();
	virtual bool ParseLine(CHSRScanner* pHSRScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pViewFormat,long& nID);
	CArcElement();
	virtual ~CArcElement();

	POINT_T GetCenterPoint() const
	{
		return m_arc.ptOrigin;
	}

	POINT_T GetStartPoint() const
	{
		return m_arc.ptStart;
	}

	DECLARE_HSR_ELEMENT(CArcElement);
	
	list<CLineElement*> m_LineElementEntry;
private:
	long m_nGroup;

	ARC_T m_arc;
};

#endif // !defined(AFX_ARCELEMENT_H__2A9083FD_41EE_4248_A5BE_B0AE2F6B4AB3__INCLUDED_)
