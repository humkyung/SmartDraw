#pragma once

#include <gui/GridCtrl/GridCtrl.h>
#include <SmartDrawPropertyPage.h>
#include <TextStyleComboBox.h>
#include <LevelComboBox.h>
#include <LineStyleComboBox.h>
#include <LineWeightComboBox.h>
#include <SmartDrawColorButton.h>
#include <ColorCheck.h>
#include "PictureWndEx.h"

// CNozzleChartPage dialog
namespace PropertyPage
{
class CNozzleChartPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CNozzleChartPage)

public:
	CNozzleChartPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNozzleChartPage();

// Dialog Data
	enum { IDD = IDD_NOZZLE_CHART_PAGE };

	CString m_sOriginX , m_sOriginY , m_sOriginJustify;
	CString m_sEqpRowHeight , m_sNozzleRowHeight , m_sRowCount;

	CPictureWndEx m_wndNozzleChartPicture;
	CMFCButton m_btnOriginPick;
	
	CTextStyleComboBox m_cboEqpTextStyle , m_cboNozzleTextStyle;
	CLevelComboBox m_cboEqpLabelLevel , m_cboNozzleLabelLevel;
	CSmartDrawColorButton  m_btnEqpLabelColor , m_btnNozzleLabelColor;
	CLineStyleComboBox m_cboEqpLabelStyle , m_cboNozzleLabelStyle;
	
	CLevelComboBox m_cboLeaderLevel;
	CSmartDrawColorButton  m_btnLeaderColor;
	CLineStyleComboBox m_cboLeaderStyle;
	CLineWeightComboBox m_cboLeaderWeight;
	
	CColorCheck m_btnSkipEqpHasNoNozzle;	/// 2014.12.31 added by humkyung

	CMFCButton m_btnMovePrev , m_btnMoveNext;
	CGridCtrl m_wndGridCtrl;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedButtonMovePrev();
	afx_msg void OnBnClickedButtonMoveNext();
	afx_msg void OnBnClickedButtonOriginPick();			/// 2014.08.25 added by humkyung
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CString m_sBorderCfgFilePath;
	int LoadData();
	int SaveData();
};
};