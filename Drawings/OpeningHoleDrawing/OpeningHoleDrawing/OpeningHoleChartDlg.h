#pragma once

#include <afxbutton.h>
#include <gui/PictureWndEx.h>
#include <TextStyleComboBox.h>
#include <ArrowComboBox.h>
#include <ColorCheck.h>
#include "PluginColorButton.h"
// COpeningHoleChartDlg dialog

class COpeningHoleChartDlg : public CDialog
{
	DECLARE_DYNAMIC(COpeningHoleChartDlg)

public:
	COpeningHoleChartDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COpeningHoleChartDlg();

	int SaveData( const CString& sIniFilePath );
// Dialog Data
	enum { IDD = IDD_OPENINGHOLE_CHART };

	CPictureWndEx m_wndHoleChartPicture;
	CColorCheck m_btnHoleChartCheck;
	CMFCButton m_btnPosPick;
	CTextStyleComboBox m_wndTextStyleComboBox;
	CPluginColorButton m_btnChartColor , m_btnLabelColor;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonPick();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
