#pragma once

#include "Mybitmapcombobox.h"
// CNewProjectDlg dialog

class CCADAppllicationBrowseEdit : public CMFCEditBrowseCtrl
{
	virtual void OnBrowse();
};

class CNewProjectDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewProjectDlg)

public:
	CNewProjectDlg(const CString& sProjectName , CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewProjectDlg();

// Dialog Data
	enum { IDD = IDD_NEW_PROJECT };

	CString m_sProjectName;
	CString m_sProjectDesc;
	CString m_sProjectUOR;	/// 2012.02.13 added by humkyung
	CString m_sModelOffsetX , m_sModelOffsetY , m_sModelOffsetZ;	/// 2014.11.09 added by humkyung
	CString m_sDrawingType;
	CString m_sOutputFormat;

	CComboBox m_wndDrawingTypeComboBox;	/// 2012.02.10 added by humkyung
	CMyBitmapComboBox m_wndOutputFormatComboBox;
	CCADAppllicationBrowseEdit m_wndCADApplication;
	CMFCButton m_wndOKButton , m_wndCancelButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboDrawingType();
private:
	bool m_bEdit;
};
