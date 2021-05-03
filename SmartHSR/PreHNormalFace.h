// PreHNormalFace.h: interface for the CPreHNormalFace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREHNORMALFACE_H__43DCDFF2_E5B1_491E_A1E1_E136C05D1704__INCLUDED_)
#define AFX_PREHNORMALFACE_H__43DCDFF2_E5B1_491E_A1E1_E136C05D1704__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "HSRFace.h"
#include "HSRScanner.h"
#include "HLineElement.h"
#include "Enum.h"	// Added by ClassView

#define	MAX_FACE_HLINES	512

class CHSRViewFormat;
class CPreHNormalFace  
{
public:
	CPreHNormalFace(const long& nGroup);
	virtual ~CPreHNormalFace();
public:
	void Init(const long nGroup);
	long GetGroupNo(){return m_nGroup;}
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);
	bool IsInVolume(const CIsVolume& volume);
	bool operator+=(CHLineElement* pPreHLINEFormat);
	const TCHAR* Layer();
	void SetLayerString(const STRING_T& strLayer)
	{
		m_strLayer = strLayer;
	}

	CHSRElement::MODEL_T m_model;
protected:
	void CalcVolume();
private:
	HSR_LAYER_E GetHSRLayer();
	STRING_T m_strLayer;
	long m_nGroup;
	POINT_T m_ptEDGE[MAX_FACE_HLINES + 1];
	VOLUME_T m_vol;

	int m_nLINEs;
	CHLineElement* m_paryHLINE[MAX_FACE_HLINES];
};

#endif // !defined(AFX_PREHNORMALFACE_H__43DCDFF2_E5B1_491E_A1E1_E136C05D1704__INCLUDED_)
