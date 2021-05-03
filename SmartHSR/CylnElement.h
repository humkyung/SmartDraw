// PreCYLNFormat.h: interface for the CCylnElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRECYLNFORMAT_H__0CB61B18_EC5A_43C0_AD1C_514D917FB9B7__INCLUDED_)
#define AFX_PRECYLNFORMAT_H__0CB61B18_EC5A_43C0_AD1C_514D917FB9B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "Element.h"
#include "HSRFace.h"
#include "HSRScanner.h"

class CHSRViewFormat;
class CCylnElement : public CHSRElement 
{
public:
	CCylnElement(/*HSR_PRIMITIVE_E type*/);
	virtual ~CCylnElement();
public:
	bool ParseLine(CHSRScanner* pGPreFormatScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);
	virtual int Copy(CHSRElement* pCopied);

	DECLARE_HSR_ELEMENT(CCylnElement);
public:
	int m_nSegments;
protected:
	void Volume();
private:
	void ClipAxisByElevation(POINT_T& ptStart , POINT_T& ptEnd,bool& clipStart , bool& clipEnd , double& clipStartZ , double& clipEndZ , const double& zhigh , const double& zlow);
	HSR_PRIMITIVE_E m_eType;

	double  m_nRadius;
	POINT_T m_pt[2];
	//double   m_matRotation[3][3];
};

#endif // !defined(AFX_PRECYLNFORMAT_H__0CB61B18_EC5A_43C0_AD1C_514D917FB9B7__INCLUDED_)
