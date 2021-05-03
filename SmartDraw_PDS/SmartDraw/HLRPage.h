#pragma once

#include <SmartDrawPropertyPage.h>
#include <gui/GridCtrl/GridCtrl.h>
#include <ArrowComboBox.h>
#include <ColorCheck.h>
#include "CPPToolTip/PPToolTip.h"
#include "TitleBlockPage.h"
#include "CPPToolTip/PPToolTip.h"

// CHLRPage dialog
namespace PropertyPage
{
class CHLRPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CHLRPage)

public:
	CHLRPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHLRPage();

// Dialog Data
	enum { IDD = IDD_HLR_PAGE };
	CComboBox m_cboPipeSize;
	CColorCheck m_btnInsulationCheck;	/// 2014.08.25 added by humkyung
	CGridCtrl m_wndGridCtrl;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnRadioClickedButtonEqualOD();
	afx_msg void OnRadioClickedButtonFixedSize();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	int LoadData();
	int SaveData();
	virtual BOOL OnInitDialog();
private:
	int LoadLineStyle(CStringArray& oStrings , const CString& sLineStyleFilePath);
	int LoadLineWeight(CStringArray& oStrings , const CString& sLineWeightFilePath);
private:
	CPPToolTip m_tooltip;
};
};