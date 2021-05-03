#pragma once

#include <gui/PictureWndEx.h>
#include <TextStyleComboBox.h>
#include <ArrowComboBox.h>
#include "PluginColorButton.h"

// COpeningHoleGeneralDlg dialog

class COpeningHoleGeneralDlg : public CDialog
{
	DECLARE_DYNAMIC(COpeningHoleGeneralDlg)

public:
	COpeningHoleGeneralDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COpeningHoleGeneralDlg();

	int SaveData( const CString& sIniFilePath );
// Dialog Data
	enum { IDD = IDD_OPENINGHOLE_GENERAL };
	CPictureWndEx m_wndHoleGroupingType;
	CTextStyleComboBox m_wndTextStyleComboBox;
	CPluginColorButton m_btnLabelColor , m_btnHoleColor , m_btnLeaderColor , m_btnHoleGroupColor;
	CArrowComboBox m_wndArrowComboBox;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioHoleGroupBox();
	afx_msg void OnBnClickedRadioHoleGroupPolyline();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
