#pragma once


// CSmartDrawAnnoPropertyPage dialog

namespace PropertyPage
{
class CSmartDrawAnnoPropertyPage : public CDialog
{
	DECLARE_DYNAMIC(CSmartDrawAnnoPropertyPage)

public:
	CSmartDrawAnnoPropertyPage(UINT nIDTempalte , CWnd* pParent = NULL);   // standard constructor
	virtual ~CSmartDrawAnnoPropertyPage();

// Dialog Data
	enum { IDD = IDD_ADRAW_PROPERTYPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CString m_rSettingFilePath , m_rProjectName;
public:
	virtual int SaveData();
	virtual int LoadData(LPCTSTR sSettingFilePath);
};
};