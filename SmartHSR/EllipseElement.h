// EllipseElement.h: interface for the CEllipseElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELLIPSEELEMENT_H__2A9083FD_41EE_4248_A5BE_B0AE2F6B4AB3__INCLUDED_)
#define AFX_ELLIPSEELEMENT_H__2A9083FD_41EE_4248_A5BE_B0AE2F6B4AB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "HSRFace.h"
#include "Element.h"
#include "HsrScanner.h"
#include <graphics/ellipse3d.h>

#include <string>
using namespace std;

class CEllipseElement : public CHSRElement
{
	enum
	{
		SEGMENTS = 16
	};
public:
	int FindLinePassingDataSet(PPOINT_T anchor , const int& nCount , PPOINT_T pts);
	int CreateLineElements();
	virtual bool ParseLine(CHSRScanner* pHSRScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pViewFormat,long& nID);
	CEllipseElement();
	virtual ~CEllipseElement();

	POINT_T GetOriginPoint() const
	{
		return m_ellipse3d.ptOrigin;
	}

	DECLARE_HSR_ELEMENT(CEllipseElement);
	
	list<CLineElement*> m_LineElementEntry;
private:
	int ConstructVertexList(PPOINT_T pts);
	long m_nGroup;

	CEllipse3d m_ellipse3d;
	double m_matrix[3][3];
};

#endif // !defined(AFX_ELLIPSEELEMENT_H__2A9083FD_41EE_4248_A5BE_B0AE2F6B4AB3__INCLUDED_)
