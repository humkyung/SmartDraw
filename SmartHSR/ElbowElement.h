// PreELBOWFormat.h: interface for the CElbowElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREELBOWFORMAT_H__58B022A6_56FF_4616_BF40_8F5DA38BFF3E__INCLUDED_)
#define AFX_PREELBOWFORMAT_H__58B022A6_56FF_4616_BF40_8F5DA38BFF3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "HSRFace.h"
#include "Element.h"
#include "HSRScanner.h"

class CHSRViewFormat;
class CElbowElement : public CHSRElement
{
public:
	typedef enum
	{
		BW = 1,
		SW = 2
	}ELBOW_TYPE_E;

	CElbowElement();
	CElbowElement(ELBOW_TYPE_E type,POINT_T ptOrigin,double nTorusRadius,double nTubeRadius);
	virtual ~CElbowElement();
public:
	bool ParseLine(CHSRScanner* pHSRScanner);
	void CalcVolume();
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pViewFmt,long& nID);
	virtual int Copy(CHSRElement* pCopied);

	DECLARE_HSR_ELEMENT(CElbowElement);
private:
	bool ParseBWLine(CHSRScanner* pHSRScanner);
	bool ParseSWLine(CHSRScanner* pHSRScanner);
	CHSRFace* CreateBWFace(CHSRViewFormat* pViewFmt,long& nID);
	CHSRFace* CreateSWFace(CHSRViewFormat* pViewFmt,long& nID);
	void  CreateSWPIPE(CHSRViewFormat* pViewFmt,long& nID);
	CHSRFace* CreateCapFace(CHSRViewFormat* pViewFmt,POINT_T ptOrigin,VECTOR_T vecNorm,double nRadius);
	void  StoreArcPoints(POINT_T ptEDGE[],POINT_T ptOrigin,POINT_T ptStart,VECTOR_T vecNorm,double nSweep);
public:
	void SetCrossPoint(POINT_T ptCross);
	void SetPosition(POINT_T ptStart,POINT_T ptEnd);
	int m_nSegments;
private:
	ELBOW_TYPE_E m_eType;

	POINT_T m_pt[2];
	POINT_T m_ptCross;
	POINT_T m_ptOrigin;
	double  m_nTorusRadius;
	double  m_nTubeRadius;
	
	double  m_nRadius[2];
	POINT_T m_ptPIPE[2][2];

	POINT_T m_ptEDGE[2][64];
	POINT_T m_ptCenter[64];
};

#endif // !defined(AFX_PREELBOWFORMAT_H__58B022A6_56FF_4616_BF40_8F5DA38BFF3E__INCLUDED_)
