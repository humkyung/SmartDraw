#if !defined(AFX_INPLACEFILTEREDIT_H__AC056BD0_F19E_41F1_8891_2D5574A94C50__INCLUDED_)
#define AFX_INPLACEFILTEREDIT_H__AC056BD0_F19E_41F1_8891_2D5574A94C50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InPlaceFilterEdit.h : header file
//
#include <gui/FilterEdit/FilterEdit.h>
#include "SmartDrawRscImp.h"
/////////////////////////////////////////////////////////////////////////////
// CInPlaceFilterEdit window

class ON_EXT_SMARTDRAWRSC CInPlaceFilterEdit : public CFilterEdit
{
// Construction
public:
	CInPlaceFilterEdit(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID,
                 int nRow, int nColumn, CString sInitText, UINT nFirstChar);

// Attributes
public:

// Operations
public:
	void EndEdit();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceFilterEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceFilterEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceFilterEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
    int     m_nRow;
    int     m_nColumn;
    CString m_sInitText;
    UINT    m_nLastChar;
    BOOL    m_bExitOnArrows;
    CRect   m_Rect;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPLACEFILTEREDIT_H__AC056BD0_F19E_41F1_8891_2D5574A94C50__INCLUDED_)
