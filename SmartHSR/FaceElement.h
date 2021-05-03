// FaceElement.h: interface for the CFaceElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACEELEMENT_H__5AE51401_D01E_40F2_B919_0F4E31065006__INCLUDED_)
#define AFX_FACEELEMENT_H__5AE51401_D01E_40F2_B919_0F4E31065006__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"

#define	MAX_FACE_VERTICES	512

class CFaceElement : public CHSRElement  
{
public:
	CFaceElement();
	virtual ~CFaceElement();

	virtual bool ParseLine(CHSRScanner* pHSRScanner);
	CHSRFace* CreateFace(CHSRViewFormat* pViewFormat,long& nID);
	bool IsInVolume(const CIsVolume& volume);
	
	DECLARE_HSR_ELEMENT(CFaceElement);
private:
	void CalcVolume();

	vector<POINT_T> m_PointList;
	vector<bool>    m_oPointVisibleList;

	long m_log;
};

#endif // !defined(AFX_FACEELEMENT_H__5AE51401_D01E_40F2_B919_0F4E31065006__INCLUDED_)
