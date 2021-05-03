// PreCTORFormat.h: interface for the CCTorElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRECTORFORMAT_H__2AF4C869_CE26_4A9D_96A2_1A98B07DE95A__INCLUDED_)
#define AFX_PRECTORFORMAT_H__2AF4C869_CE26_4A9D_96A2_1A98B07DE95A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "Element.h"
#include "HSRFace.h"
#include "HSRScanner.h"

class CHSRViewFormat;
class CCTorElement : public CHSRElement
{
	typedef enum
	{
		CTOR_T     = 1,
		EQP_TORS_T = 2
	}TYPE_E;
public:
	CCTorElement();
	virtual ~CCTorElement();
public:
	bool ParseLine(CHSRScanner* pGPreFormatScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);

	DECLARE_HSR_ELEMENT(CCTorElement);
private:
	CHSRFace* CreateCTORFace(CHSRViewFormat* pVIEWFormat,long& nID);
	CHSRFace* CreateEQP_TORSFace(CHSRViewFormat* pVIEWFormat,long& nID);
public:
	int m_nSegments;
private:
	HSR_LAYER_E GetHSRLayer();
	TYPE_E   m_enmType;

	POINT_T  m_pt[2];
	POINT_T  m_ptOrigin;
	VECTOR_T m_vecNorm;
	VECTOR_T m_vecAxis;
	double   m_nTubeRadius;
	double   m_nStartAng;
	double   m_nSweepAng;

	double  m_nRadius[2];
	POINT_T m_ptPIPE[2][2];
};



#endif // !defined(AFX_PRECTORFORMAT_H__2AF4C869_CE26_4A9D_96A2_1A98B07DE95A__INCLUDED_)











































