// PreSPHEREFormat.h: interface for the CSphereElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRESPHEREFORMAT_H__475C3276_67BF_4BAC_A462_F3B7636047A0__INCLUDED_)
#define AFX_PRESPHEREFORMAT_H__475C3276_67BF_4BAC_A462_F3B7636047A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "Element.h"
#include "HSRFace.h"
#include "HSRScanner.h"

#include <string>
using namespace std;

class CHSRViewFormat;
class CSphereElement : public CHSRElement
{
public:
	CSphereElement();
	virtual ~CSphereElement();
public:
	bool ParseLine(CHSRScanner* pHSRScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);

	DECLARE_HSR_ELEMENT(CSphereElement);
private:
	int CreateCircleEdge(POINT_T ptEdge[],POINT_T ptOrigin,double nRadius,VECTOR_T vecNorm);
public:
	int m_nSegments;
private:
	double  m_nRadius;
	POINT_T m_ptOrigin;
};

#endif // !defined(AFX_PRESPHEREFORMAT_H__475C3276_67BF_4BAC_A462_F3B7636047A0__INCLUDED_)
