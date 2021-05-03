// MUELToElbowType.h: interface for the CMUELToElbowType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUELTOELBOWTYPE_H__C2BFBEC2_01A5_45EA_8994_F90D350AD2CD__INCLUDED_)
#define AFX_MUELTOELBOWTYPE_H__C2BFBEC2_01A5_45EA_8994_F90D350AD2CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"
#include "HSRScanner.h"

class CMUELToElbowType  
{
public:
	CMUELToElbowType(CHSRScanner* pHSRScanner);
	virtual ~CMUELToElbowType();
private:
	POINT_T  m_pt[2];
	POINT_T  m_ptOrigin;
	double   m_nTorusRadius;
	double   m_nTubeRadius;
	VECTOR_T m_vecNorm;
	VECTOR_T m_vecAxis;
	
	double   m_nStartAng;
	double   m_nSweepAng;
};

#endif // !defined(AFX_MUELTOELBOWTYPE_H__C2BFBEC2_01A5_45EA_8994_F90D350AD2CD__INCLUDED_)
