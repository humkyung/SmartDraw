#pragma once


// CGenerateDraFromDrawingDlg dialog

class CGenerateDraFromDrawingDlg : public CDialog
{
	DECLARE_DYNAMIC(CGenerateDraFromDrawingDlg)
	typedef enum
	{
		PDS_HIDDEN		= 0x00
	}eFilter;
public:
	CGenerateDraFromDrawingDlg(const CString& sDraFolderPath , const CString& sDrawingFolderPath , 
		const CString& sProjectUOR , const CString& sDatabaseFilePath , CWnd* pParent = NULL);   // standard constructor
	virtual ~CGenerateDraFromDrawingDlg();

// Dialog Data
	enum { IDD = IDD_GENERATE_DRA_FROM_DRAWING };

	CComboBox m_cboFilter;	/// 2014.05.06 added by humkyung
	CMFCButton m_btnToggleSelection;
	CMFCListCtrl m_wndDrawingListCtrl;
	CMFCButton m_btnOK , m_btnCancel;
protected:
	CString m_sDraFolderPath , m_sDrawingFolderPath , m_sProjectUOR , m_sDatabaseFilePath;
	CImageList m_cImages;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CString m_sVadFolderPath;	/// 2014.05.06 added by humkyung
public:
	afx_msg void OnBnClickedButtonToggleSelection();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelendokComboFilter();
	afx_msg void OnEnChangeEditDrawingFileFolder();
};
