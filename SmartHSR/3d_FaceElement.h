// FACEElement.h: interface for the C3d_FaceElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACEELEMENT_H__AC52BC65_6448_4A4B_BE15_14D59AA33ACD__INCLUDED_)
#define AFX_FACEELEMENT_H__AC52BC65_6448_4A4B_BE15_14D59AA33ACD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"

class C3d_FaceElement : public CHSRElement  
{
	enum
	{
		MAX_PTS = 32
	};
public:
	C3d_FaceElement();
	virtual ~C3d_FaceElement();
public:
	int GetVertexCount() const{
		return m_pts.size();
	}
	POINT_T GetVertexAt(const int& at)
	{
		return m_pts[at];
	}
	bool ParseLine(CHSRScanner* pHSRScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);
	int CreateLineSegments();

	list<CLineElement*> m_LineElementEntry;
	DECLARE_HSR_ELEMENT(C3d_FaceElement);
private:
	///int m_nVertices;
	vector<POINT_T> m_pts;
};

#endif // !defined(AFX_FACEELEMENT_H__AC52BC65_6448_4A4B_BE15_14D59AA33ACD__INCLUDED_)
