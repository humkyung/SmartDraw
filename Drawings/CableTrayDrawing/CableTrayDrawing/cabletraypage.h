#pragma once

#include <afxeditbrowsectrl.h>
#include <SmartDrawPropertyPage.h>
#include <TextStyleComboBox.h>
#include <ArrowComboBox.h>
#include <SmartDrawColorButton.h>
#include <ColorCheck.h>
#include <CPPToolTip/PPToolTip.h>

// CCableTrayPage dialog
class CCableTrayPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CCableTrayPage)

	class CCellLibraryFileBrowseEdit : public CMFCEditBrowseCtrl
	{
		virtual void OnBrowse();
	public:
		CString m_rIniFilePath , m_rFilePath , m_rProjectName;
	};

public:
	CCableTrayPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCableTrayPage();

	static CCableTrayPage* instance;
// Dialog Data
	enum { IDD = IDD_CABLE_TRAY_PAGE };
	CTextStyleComboBox m_wndTextStyleComboBox;
	CCellLibraryFileBrowseEdit m_wndCellLibraryFileEdit;
	CColorCheck m_wndElecTagCheckBox , m_wndElecOutlineCheckBox;
	CColorCheck m_wndInstTagCheckBox , m_wndInstOutlineCheckBox;
	CColorCheck m_wndFittingTagCheckBox , m_wndUpDownMarkCheckBox;
	CSmartDrawColorButton m_wndTextColorButton , m_wndElecHatchColorButton , m_wndInstHatchColorButton;
	CArrowComboBox m_wndArrowComboBox;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void NotifyDisplayTooltip(NMHDR * pNMHDR, LRESULT * result);
	DECLARE_MESSAGE_MAP()
public:
	int LoadData(void);
	int SaveData(void);

	virtual BOOL OnInitDialog();
private:
	CPPToolTip m_tooltip;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnChangeEditCellLibraryFilePath();
	afx_msg void OnBnClickedCheckElec();
	afx_msg void OnBnClickedCheckElecOutline();
	afx_msg void OnBnClickedCheckInst();
	afx_msg void OnBnClickedCheckInstOutline();
};
