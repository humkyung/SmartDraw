// FACEMESHElement.h: interface for the CFACEMESHElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FACEMESHELEMENT_H__CEC0ABB9_A36B_4AEA_B84A_F4EC6D49BF4F__INCLUDED_)
#define AFX_FACEMESHELEMENT_H__CEC0ABB9_A36B_4AEA_B84A_F4EC6D49BF4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"

class CFaceMeshElement : public CHSRElement  
{
	typedef struct
	{
		int nSeq[4];
	}LoopIndex;
public:
	CFaceMeshElement();
	virtual ~CFaceMeshElement();
public:
	bool ParseLine(CHSRScanner* pPreFormatScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);

	DECLARE_HSR_ELEMENT(CFaceMeshElement);
private:
	int m_nFaces , m_nPoints;
	LoopIndex* m_pLoopIndex;
	POINT_T* m_ppt;
};

#endif // !defined(AFX_FACEMESHELEMENT_H__CEC0ABB9_A36B_4AEA_B84A_F4EC6D49BF4F__INCLUDED_)
