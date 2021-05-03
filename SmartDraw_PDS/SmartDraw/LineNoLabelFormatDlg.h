#pragma once

#include <gui/EditListBox/EditListBox.h>

// CLineNoLabelFormatDlg dialog

class CLineNoLabelFormatDlg : public CDialog
{
	DECLARE_DYNAMIC(CLineNoLabelFormatDlg)

public:
	CLineNoLabelFormatDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLineNoLabelFormatDlg();

// Dialog Data
	enum { IDD = IDD_LINE_NO_LABEL_FORMAT };
	CMFCButton m_wndAddButton;
	EditListBox::CEditListBoxBuddy	m_wndEditListBoxBuddy;
	EditListBox::CEditListBox m_wndLineNoLabelEditListBox;
	CMFCButton m_wndOKButton , m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
