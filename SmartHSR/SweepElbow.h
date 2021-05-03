// SweepElbow.h: interface for the CSweepElbow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SWEEPELBOW_H__CB3166CA_E315_4CD6_9420_5BFAC1BB47F8__INCLUDED_)
#define AFX_SWEEPELBOW_H__CB3166CA_E315_4CD6_9420_5BFAC1BB47F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"

class CSweepElbow : public CHSRElement  
{
	typedef struct
	{
		double     elev;
		POINT_T  pts[5];
		CHSRFace*  pFace;
	}FacePts;

	typedef struct
	{
		POINT_T pts[2];
	}Edge;
public:
	virtual CHSRFace* CreateFace(CHSRViewFormat* pViewFmt,long& nID);
	virtual bool IsInVolume(const CIsVolume& volume);
	virtual bool ParseLine(CHSRScanner* pHSRScanner);
	CSweepElbow();
	virtual ~CSweepElbow();

	DECLARE_HSR_ELEMENT(CSweepElbow);
private:
	void Volume();
	POINT_T m_lhs_origin;
	double  m_lhs_radius;
	vector<POINT_T>* m_lhs;
	POINT_T m_rhs_origin;
	double  m_rhs_radius;
	vector<POINT_T>* m_rhs;
};

#endif // !defined(AFX_SWEEPELBOW_H__CB3166CA_E315_4CD6_9420_5BFAC1BB47F8__INCLUDED_)
