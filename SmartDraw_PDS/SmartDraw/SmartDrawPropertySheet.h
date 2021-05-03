#pragma once

#include "StdAfx.h"
#include <util/Markup/Markup.h>
#include "BorderPage.h"
#include "TitleBlockPage.h"
#include "GridNoPage.h"
#include "KeyPlanHatchingPage.h"
#include "AreaOptionPage.h"
#include "DimensionPage.h"
#include "AnnotationPage.h"
#include "LineNoPage.h"
#include "InsideViewLineNoPage.h"
#include "OutsideViewLineNoPage.h"
#include "InstrumentPage.h"
#include "ValvePage.h"
#include "SupportPage.h"
#include "NozzlePage.h"
#include "ReducerPage.h"
#include "InsideEquipmentPage.h"
#include "OutsideEquipmentPage.h"
#include "StructurePage.h"
///#include "CableTrayPage.h"
#include "SpecialItemPage.h"
#include "Flow_SlopeMarkPage.h"
#include "RevisionChartPage.h"
#include "NozzleChartPage.h"
#include "GenerationPage.h"
#include "HLRPage.h"

#include "VerticalLineHatchingPage.h"
#include "PlatformHatchingPage.h"
#include "EquipmentHatchingPage.h"
#include "ViewAnnoPage.h"

#include <list>
#include <map>
using namespace std;
using namespace PropertyPage;
// CSmartDrawPropertySheet dialog

class CPropertyTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CPropertyTreeCtrl)
public:
	CPropertyTreeCtrl(){}
	~CPropertyTreeCtrl(){}
protected:
	DECLARE_MESSAGE_MAP()
};

class CSmartDrawPropertySheet : public CDialog
{
	DECLARE_DYNAMIC(CSmartDrawPropertySheet)

public:
	CSmartDrawPropertySheet(const int& nSelectedPage , const CString& rProjectName , const CString& rSettingFilePath , const STRING_T& sDrawingType ,
		CWnd* pParent = NULL);   // standard constructor
	virtual ~CSmartDrawPropertySheet();

// Dialog Data
	enum { IDD = IDD_PROPERTY_SHEET };

	CPropertyTreeCtrl m_wndPropertyTreeCtrl;

	CMFCButton m_wndOKButton , m_wndCancelButton;

	CBorderPage				m_wndBorderPage;
	CTitleBlockPage			m_wndTitleBlockPage;
	CGridNoPage				m_wndGridNoPage;
	CKeyPlanHatchingPage	m_wndKeyPlanHatchingPage;
	CAreaOptionPage			m_wndAreaOptionPage;
	CIntDimStylePage		m_wndIntDimStylePage;
	CExtDimStylePage		m_wndExtDimStylePage;
	CHLRPage				m_wndHLRPage;
	CAnnotationPage			m_wndAnnotationPage;
	CLineNoPage				m_wndLineNoPage;
	CInsideViewLineNoPage	m_wndInsideViewLineNoPage;
	COutsideViewLineNoPage	m_wndOutsideViewLineNoPage;
	CInstrumentPage			m_wndInstrumentPage;
	CValvePage				m_wndValvePage;
	CSupportPage			m_wndSupportPage;
	CNozzlePage				m_wndNozzlePage;
	CReducerPage			m_wndReducerPage;
	CInsideEquipmentPage	m_wndInsideEqpPage;
	COutsideEquipmentPage	m_wndOutsideEqpPage;
	CStructurePage			m_wndStructurePage;
	///CCableTrayPage			m_wndCableTrayPage;
	CSpecialItemPage		m_wndSpecialItemPage;
	CFlow_SlopeMarkPage		m_wndFlow_SlopeMarkPage;
	CRevisionChartPage		m_wndRevisionChartPage;
	CNozzleChartPage		m_wndNozzleChartPage;
	CGenerationPage			m_wndGenerationPage;
	
	CVerticalLineHatchingPage m_wndVerticalLineHatchingPage;
	CPlatformHatchingPage	  m_wndPlatformHatchingPage;
	CEquipmentHatchingPage	  m_wndEquipmentHatchingPage;
	CViewAnnoPage			m_wndViewAnnoPage;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
private:
	STRING_T m_sTitle , m_sDrawingType;
	map<int , CSmartDrawPropertyPage*> m_PageIndexMap;
	map<HTREEITEM , CSmartDrawPropertyPage*> m_oPageItemMap;

	CDialog* m_pActivePage;
	int m_nSelectedPage;
	CString m_rProjectName , m_sIniFilePath;
	list<CDialog*> m_PageList;

	HTREEITEM CreatePageOf(const STRING_T& sTitle , const STRING_T& sDisplay , HTREEITEM hParent);
	int CreatePages(CMarkup& xml , HTREEITEM hParent);
	HTREEITEM AddPage(const UINT& nID , const CString& rLabel , CSmartDrawPropertyPage* pPage , const int& nImage , HTREEITEM hParent);
	int SetActivePage(HTREEITEM hItem);
public:
	STRING_T GetDrawingType() const;
	CString GetProjectName() const
	{
		return m_rProjectName;
	}
	CString GetSettingFilePath() const
	{
		return m_sIniFilePath;
	}
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedTreePage(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnOpenBorder();
	afx_msg void OnBorderChanged();
	afx_msg void OnNMRClickTreePage(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateBorderOpen(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
