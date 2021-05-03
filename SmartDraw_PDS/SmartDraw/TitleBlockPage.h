#pragma once
#include <gui/GridCtrl/GridCtrl.h>
#include <gui/GridCtrl/GridCellCombo.h>
#include <SmartDrawColorButton.h>
#include <SmartDrawPropertyPage.h>
#include <TextStyleComboBox.h>
#include <LevelComboBox.h>
#include <LineStyleComboBox.h>
#include <LineWeightComboBox.h>

// CTitleBlockPage dialog
namespace PropertyPage
{

class CPickButton : public CMFCButton
{
	DECLARE_DYNCREATE(CPickButton)
public:
	CPickButton();
	~CPickButton(){}
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP();
public:
	CGridCell* m_pCell;
};

class CPickButtonGridCell : public CGridCell  
{
	DECLARE_DYNCREATE(CPickButtonGridCell)
public:
	CPickButtonGridCell();
	virtual ~CPickButtonGridCell();
public:
	virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
public:
	int m_iRow,m_iCol;
private:
	CPickButton m_wndPickButton;
};

class CTextStyleGridCell : public CGridCell  
{
	DECLARE_DYNCREATE(CTextStyleGridCell)
public:
	CTextStyleGridCell();
	virtual ~CTextStyleGridCell();
public:
	virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	virtual void EndEdit();

	static CString m_rIniFilePath;
private:
	CTextStyleComboBox m_wndTextStyleComboBox;
};

class CLevelGridCell : public CGridCell  
{
	DECLARE_DYNCREATE(CLevelGridCell)
public:
	CLevelGridCell();
	virtual ~CLevelGridCell();
public:
	virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	virtual void EndEdit();
private:
	CLevelComboBox m_cboLevel;
};

class CTextColorGridCell : public CGridCell  
{
	DECLARE_DYNCREATE(CTextColorGridCell)
public:
	CTextColorGridCell();
	virtual ~CTextColorGridCell();
public:
	int EnableAutomaticButton();
	CString GetColorIndex();
	BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd);
	virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	virtual void EndEdit();

	static CString m_rIniFilePath;
private:
	CSmartDrawColorButton m_wndTextColorButton;
public:
	int FillColors(const CString& rIniFilePath);
};

class CTitleBlockPage : public CSmartDrawPropertyPage
{
	DECLARE_DYNAMIC(CTitleBlockPage)

public:
	CTitleBlockPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTitleBlockPage();

// Dialog Data
	enum { IDD = IDD_TITLE_BLOCK_PAGE };

	CMFCButton m_wndNewTitleBlockItem , m_wndDeleteTitleBlockItem;
	CGridCtrl m_wndGridCtrl;
protected:
	int LoadData(void);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	afx_msg void OnBnClickedButtonNewTitleBlockItem();
	afx_msg void OnBnClickedButtonDeleteTitleBlockItem();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	int SaveData();
private:
	int NewTitleBlockItem(const CString& sLabel);	/// 2013.06.16 added by humkyung
};
};