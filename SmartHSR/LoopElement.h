// LoopElement.h: interface for the CLoopElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOOPELEMENT_H__5AE51401_D01E_40F2_B919_0F4E31065006__INCLUDED_)
#define AFX_LOOPELEMENT_H__5AE51401_D01E_40F2_B919_0F4E31065006__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"
#include "HSRPoint.h"

class CLoopElement : public CHSRElement  
{
public:
	typedef enum
	{
		SOLID = 0x01,
		HOLE  = 0x02
	}CREATION;

	CLoopElement();
	virtual ~CLoopElement();

	virtual bool ParseLine(CHSRScanner* pHSRScanner);
	CHSRFace* CreateFace(CHSRViewFormat* pViewFormat,long& nID);
	bool IsInVolume(const CIsVolume& volume);
	int GetShapePointSet(vector<POINT_T>& pts , double& zhigh);
	CREATION GetCreationFlag() const
	{
		return m_creation;
	}

	DECLARE_HSR_ELEMENT(CLoopElement);
private:
	void CalcVolume();

	CREATION m_creation;
	list<CHSRElement*> m_ElementEntry;

	long m_log;
};

#endif // !defined(AFX_LOOPELEMENT_H__5AE51401_D01E_40F2_B919_0F4E31065006__INCLUDED_)
