#pragma once

#include <gui/GridCtrl/GridCtrl.h>
#include <SmartDrawPropertyPage.h>

// CGenerationPage dialog
namespace PropertyPage
{
class CGenerationPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CGenerationPage)

public:
	CGenerationPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGenerationPage();

	int SaveData();

// Dialog Data
	enum { IDD = IDD_GENERATION_PAGE };
	CMFCButton m_wndNewLevelItem , m_wndDeleteLevelItem;
	CGridCtrl m_wndGridCtrl , m_wndLocationOrderGridCtrl;
	CGridCtrl m_wndGenerateItemsGridCtrl;	/// 2014.03.30 added by humkyung
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedButtonNewLevelItem();
	afx_msg void OnBnClickedButtonDeleteLevelItem();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
private:
	int GetLevelEleArray(CStringArray& options);
	int UpdateContents(void);
};
};