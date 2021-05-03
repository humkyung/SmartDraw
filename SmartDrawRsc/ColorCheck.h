#if !defined(AFX_COLORCHECK_H__A897D857_09A3_11D6_8CAD_00500439B545__INCLUDED_)
#define AFX_COLORCHECK_H__A897D857_09A3_11D6_8CAD_00500439B545__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorCheck.h : header file
//

#include "SmartDrawRscImp.h"

/////////////////////////////////////////////////////////////////////////////
// CColorCheck window

class ON_EXT_SMARTDRAWRSC CColorCheck : public CButton
{
	DECLARE_DYNAMIC(CColorCheck)

	// Construction
public:

	CColorCheck();
	virtual ~CColorCheck();

	int SetLabels(const CString& sOnLabel , const CString& sOffLabel);     
	virtual void SetCheck(int nCheck);
	virtual int GetCheck();
	COLORREF SetBkColor(COLORREF color);
	COLORREF SetArrowColor(COLORREF newColor);
	COLORREF SetMyTextColor(COLORREF txtColor);
	// Implementation
public:
	BOOL checkFlag;
	UINT oldAction;
	UINT oldState;
	BOOL drawFocus;
	COLORREF newColor, newArrowColor, newTextColor;
	// Generated message map functions
protected:
	void DrawCheckCaption(CDC *pDC, CRect R, const TCHAR *Buf, COLORREF TextColor);
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItem);     //override the standard function (no WM_DRAWITEM !!!)
	COLORREF GetDisabledColor() { return disabled; }
	//{{AFX_MSG(CColorCheck)
	//}}AFX_MSG
private:
	COLORREF disabled;
	DECLARE_MESSAGE_MAP()

	void DrawArrow(CDC* pDC);

	CString m_sLabels[2];
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

#endif
