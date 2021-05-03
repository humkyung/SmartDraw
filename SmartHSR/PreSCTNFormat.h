// PreSCTNFormat.h: interface for the CPreSCTNFormat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRESCTNFORMAT_H__F4487627_E27A_4B3F_B4FD_C387A06E65A3__INCLUDED_)
#define AFX_PRESCTNFORMAT_H__F4487627_E27A_4B3F_B4FD_C387A06E65A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HiddenDef.h"
#include "HSRFace.h"
#include "HSRScanner.h"

class CHSRViewFormat;
class CPreSCTNFormat  
{
	typedef enum{
		H_TYPE = 1,
		L_TYPE = 2
	}SCTN_TYPE_E;
public:
	CPreSCTNFormat();
	virtual ~CPreSCTNFormat();
public:
	void ParseLine(CHSRScanner* pPreFormatScanner);
	bool IsInVolume(const VOLUME_T volume);
	PFACE CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);
protected:
	void Volume();
public:
	SCTN_TYPE_E m_enmType;

	POINT_T  m_pt[2];
	double   m_nWidth;
	double   m_nHeight;
	double   m_nSize[2];
	VECTOR_T m_vec;
	double   m_nRotate;
	
	VOLUME_T m_vol;
	POINT_T m_ptEDGE[13];
};

#endif // !defined(AFX_PRESCTNFORMAT_H__F4487627_E27A_4B3F_B4FD_C387A06E65A3__INCLUDED_)
