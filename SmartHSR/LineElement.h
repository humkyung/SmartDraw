// PreLINEFormat.h: interface for the CPreLINEFormat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRELINEFORMAT_H__82C02052_D1A7_47B3_81E5_43A54137D31C__INCLUDED_)
#define AFX_PRELINEFORMAT_H__82C02052_D1A7_47B3_81E5_43A54137D31C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "HSRFace.h"
#include "HSRScanner.h"
#include "HSRPoint.h"

#include <string>
#include "HSRBoundingBox.h"	// Added by ClassView
#include "Element.h"

using namespace std;
class CLineElement : public CHSRElement
{
public:
	CLineElement();
	virtual ~CLineElement();
public:
	bool ParseLine(CHSRScanner* pHSRScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pViewFormat,long& nID);

	DECLARE_HSR_ELEMENT(CLineElement);
public:
	CHSRBoundingBox GetBoundingBox();
	long m_nGroup;
	POINT_T m_pt[2];
	bool    m_bVisible;
private:
	int CalcVolume();
};

#endif // !defined(AFX_PRELINEFORMAT_H__82C02052_D1A7_47B3_81E5_43A54137D31C__INCLUDED_)
