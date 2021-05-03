#pragma once

#include <gui/EditTreeCtrl.h>
#include "DraViewSettingDlg.h"
#include "SmartDrawDoc.h"
#include "..\..\SmartDrawModule\aDraw_DraFile.h"
#include "DraViewSettingDlg.h"

// CDraFileSettingDlg dialog

class CViewEditTreeCtrl : public CEditTreeCtrl
{
	DECLARE_DYNAMIC(CViewEditTreeCtrl)

public:
	CViewEditTreeCtrl();
	virtual ~CViewEditTreeCtrl();
protected:
	void DisplayContextMenu(CPoint & point) ;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnViewNew();
	afx_msg void OnViewCopyEdit();
	afx_msg void OnViewDelete();
};

class CDraFileSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CDraFileSettingDlg)

public:
	CDraFileSettingDlg(const CString& rDraFilePath , CSmartDrawDoc* pDoc , const CString& sSettingFileName=_T("") , CWnd* pParent = NULL);   // standard constructor
	virtual ~CDraFileSettingDlg();

// Dialog Data
	enum { IDD = IDD_DRA };
	CString m_rDraFilePath;
	CString m_sSettingFileName;	/// 2012.04.20 added by humkyung
	CSmartDrawDoc* m_pDoc;

	CImageList m_cImages;		/// 2012.02.11 added by humkyung

	CMFCButton m_btnSaveView;
	CMFCButton m_wndOKButton , m_wndCancelButton;
	CViewEditTreeCtrl m_wndViewTreeCtrl;
	///CMFCTabCtrl m_wndTabs;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int UpdateViewDlgWith(CaDraw_View* pDraView);	/// 2012.02.12 added by humkyung
	CaDraw_DraFile* GetDraFile() const;				/// 2012.02.12 added by humkyung

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRClickTabView(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
private:
	HTREEITEM m_hRootOfView;	/// 2012.02.12 added by humkyung

	CaDraw_DraFile* m_pDraFile;
	CDraViewSettingDlg* m_pViewDlg;
public:
	afx_msg void OnNMDblclkTreeView(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSaveView();
	afx_msg void OnNMRClickTreeView(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnViewNew();
	afx_msg void OnUpdateViewNew(CCmdUI *pCmdUI);
};
