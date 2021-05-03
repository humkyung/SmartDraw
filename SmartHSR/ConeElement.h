// PreCONEFormat.h: interface for the CConeElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRECONEFORMAT_H__FD68657F_C1E5_4E88_A131_419E41B1A017__INCLUDED_)
#define AFX_PRECONEFORMAT_H__FD68657F_C1E5_4E88_A131_419E41B1A017__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQLib.h>
#include "HiddenDef.h"
#include "Element.h"
#include "HSRFace.h"
#include "HSRScanner.h"

class CHSRViewFormat;
class CConeElement : public CHSRElement
{
public:
	CConeElement(/*HSR_PRIMITIVE_E type*/);
	virtual ~CConeElement();
public:
	bool ParseLine(CHSRScanner* pGPreFormatScanner);
	bool IsInVolume(const CIsVolume& volume);

	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);

	DECLARE_HSR_ELEMENT(CConeElement);
protected:
	void Volume();
public:
	int m_nSegments;
private:
	void ClipAxisByElevation(POINT_T& ptStart , POINT_T& ptEnd , double* nRadius , const double& zhigh , const double& zlow);
	CHSRFace* CreateVerticalFace(CHSRViewFormat* pViewFormat,const CVect& vecNorm,
				const POINT_T ptRef[2],const double nRadiusRef[2],
				long& nID);
	HSR_PRIMITIVE_E m_eType;

	double   m_nRadius[2];
	POINT_T  m_pt[2];
	VECTOR_T m_vec;

	//double   m_matRotation[3][3];

	POINT_T m_ptEDGE[36];
};

#endif // !defined(AFX_PRECONEFORMAT_H__FD68657F_C1E5_4E88_A131_419E41B1A017__INCLUDED_)
