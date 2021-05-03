#pragma once

#include <LabelStatic.h>
#include <SmartDrawPropertyPage.h>

// CBorderPage dialog
namespace PropertyPage
{
class CBorderFileBrowseEdit : public CMFCEditBrowseCtrl
{
	virtual void OnBrowse();
public:
	CString m_rIniFilePath , m_rFilePath , m_rProjectName;
};

class CaDrawFileBrowseEdit : public CMFCEditBrowseCtrl
{
	virtual void OnBrowse();
public:
	typedef enum tagFileType
	{
		KEY_PLAN	= 0x01,
		NORTH_MARK	= 0x02,
		SCALE_BAR	= 0x03
	}FileType;

	FileType m_eFileType;
	CString m_rIniFilePath , m_rFilePath , m_rProjectName;
	CString m_rScale; //! for scale bar
};

class CBorderPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CBorderPage)

public:
	CBorderPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBorderPage();

// Dialog Data
	enum { IDD = IDD_BORDER_PAGE };
	CBorderFileBrowseEdit m_wndBorderFileEdit;
	CaDrawFileBrowseEdit  m_wndKeyPlanFileEdit , m_wndNorthMarkFileEdit , m_wndScaleBarFileEdit;
	CString m_rBorderDesc , m_sBorderScale , m_rScaleValue;
	CLabelStatic m_wndDenominatorStatic , m_wndXStatic , m_wndYStatic , m_wndScaleStatic;

	CMFCButton m_wndKeyPlanPickButton , m_wndNorthMarkPickButton , m_wndScaleBarPickButton;
protected:
	CString m_rBorderConfigFilePath;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnEnChangeEditScaleValue();
	afx_msg void OnBnClickedButtonOpenBorderFile();		/// 2014.06.21 added by humkyung
	afx_msg void OnBnClickedButtonKeyPlanPick();
	afx_msg void OnBnClickedButtonNorthMarkPick();
	afx_msg void OnEnChangeEditBorderFile();
	afx_msg void OnEnChangeEditKeyPlanFile();
	afx_msg void OnEnChangeEditNorthMarkFile();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int LoadData(void);
	int SaveData(void);
};
};
