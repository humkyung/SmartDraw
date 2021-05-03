// PreNormalFace.h: interface for the CHSRNormalFace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRENORMALFACE_H__E5CF986B_5AAE_44D9_8D0C_EB08A89B0761__INCLUDED_)
#define AFX_PRENORMALFACE_H__E5CF986B_5AAE_44D9_8D0C_EB08A89B0761__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "HSRFace.h"
#include "HSRScanner.h"
#include "LineElement.h"
#include "ArcElement.h"
#include "Enum.h"	// Added by ClassView

#define	MAX_FACE_LINES	512

class CHSRViewFormat;
class CMESHElement;
class CHSRNormalFace : public CHSRElement
{
public:
	CHSRNormalFace(long nGroup);
	virtual ~CHSRNormalFace();
public:
	bool ParseLine(CHSRScanner* pPreFormatScanner){return true;}

	int GetLineCount() const;	/// 2012.03.06 added by humkyung

	CLineElement* GetLastLineElement();
	void Init(const long nGroup);
	long  GetGroupNo(){return m_nGroup;}
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);
	bool IsInVolume(const CIsVolume& volume);
	bool operator+=(CLineElement* pLineElm);
	bool operator+=(CArcElement* pArcElm);
	LPCTSTR Layer();
	int GetShapePointSet(vector<POINT_T>& pts , double& zhigh);
protected:
	void CalcVolume();
private:
	HSR_LAYER_E GetHSRLayer();
	long     m_nGroup;
	vector<POINT_T>  m_PointEntry;
	POINT_T m_ptEDGE[MAX_FACE_LINES + 1];

	int m_nLINEs;
	CLineElement* m_paryLINE[MAX_FACE_LINES];

	friend CMESHElement;
};

#endif // !defined(AFX_PRENORMALFACE_H__E5CF986B_5AAE_44D9_8D0C_EB08A89B0761__INCLUDED_)
