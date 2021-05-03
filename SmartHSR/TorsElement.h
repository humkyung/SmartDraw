// PreTORSFormat.h: interface for the CTorsElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRETORSFORMAT_H__BDA51E19_0657_466F_BEFE_5E4B210B9F11__INCLUDED_)
#define AFX_PRETORSFORMAT_H__BDA51E19_0657_466F_BEFE_5E4B210B9F11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "Element.h"
#include "HSRFace.h"
#include "HSRScanner.h"

class CHSRViewFormat;
class CTemplateElement;
class CTorsElement : public CHSRElement  
{
public:
	CTorsElement();
	virtual ~CTorsElement();
public:
	bool ParseLine(CHSRScanner* pHSRScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);

	DECLARE_HSR_ELEMENT(CTorsElement);
private:
	void CreateCircleEdge(POINT_T ptEdge[],POINT_T ptOrigin,double nRadius,VECTOR_T vecNorm);
public:
	int m_nSegments;
private:
	POINT_T  m_pt[2];
	POINT_T  m_ptOrigin;
	double   m_nTorusRadius;
	double   m_nTubeRadius;
	VECTOR_T m_vecNorm;
	VECTOR_T m_vecAxis;

	double   m_nStartAng;
	double   m_nSweepAng;

	friend class CTemplateElement;
};

#endif // !defined(AFX_PRETORSFORMAT_H__BDA51E19_0657_466F_BEFE_5E4B210B9F11__INCLUDED_)
