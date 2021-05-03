#pragma once

#include <gui/GridCtrl/GridCtrl.h>
#include <gui/FilterEdit/FloatEdit.h>
#include <SmartDrawPropertyPage.h>
#include <TextStyleComboBox.h>
#include <LevelComboBox.h>
#include <LineStyleComboBox.h>
#include <LineWeightComboBox.h>
#include <SmartDrawColorButton.h>
#include "PictureWndEx.h"

// CRevisionChartPage dialog
namespace PropertyPage
{
class CRevisionChartPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CRevisionChartPage)

public:
	CRevisionChartPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRevisionChartPage();

// Dialog Data
	enum { IDD = IDD_REVISION_CHART_PAGE };
	
	CString m_sOriginX , m_sOriginY , m_sOriginJustify;
	CString m_sRowCount , m_sRowHeight;

	CPictureWndEx m_wndRevChartPicture;
	CMFCButton m_btnOriginPick;

	CTextStyleComboBox m_cboTextStyle;
	CLevelComboBox m_cboLabelLevel;
	CSmartDrawColorButton  m_btnLabelColor;
	CLineStyleComboBox m_cboLabelStyle;

	CLevelComboBox m_cboLeaderLevel;
	CSmartDrawColorButton  m_btnLeaderColor;
	CLineStyleComboBox m_cboLeaderStyle;
	CLineWeightComboBox m_cboLeaderWeight;

	CFloatEdit m_wndRevChartRowHeight;	/// 2014.11.15 added by humkyung

	CGridCtrl m_wndGridCtrl;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedButtonMovePrev();
	afx_msg void OnBnClickedButtonMoveNext();
	afx_msg void OnBnClickedButtonOriginPick();			/// 2014.08.25 added by humkyung
	DECLARE_MESSAGE_MAP()
public:
	int LoadData();
	int SaveData();
	virtual BOOL OnInitDialog();
private:
	CString m_sBorderCfgFilePath;
};
};