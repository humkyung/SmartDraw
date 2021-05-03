// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "ProjectExplorer.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"

class MyPane : public CMFCRibbonStatusBarPane
{
public:
	MyPane(){};
	MyPane(		UINT   nCmdID, 		LPCTSTR lpszText, 		BOOL   bIsStatic = FALSE, HICON  hIcon = NULL,LPCTSTR lpszAlmostLargeText = NULL)
		: CMFCRibbonStatusBarPane(nCmdID,lpszText,bIsStatic,hIcon,lpszAlmostLargeText){}
	
	MyPane(		UINT   nCmdID,          LPCTSTR lpszText,         HBITMAP hBmpAnimationList,
		int   cxAnimation = 16, 
		COLORREF clrTrnsp= RGB(192,192,192),
		HICON  hIcon = NULL,   
		BOOL   bIsStatic = FALSE)
		:CMFCRibbonStatusBarPane(nCmdID,lpszText,hBmpAnimationList,cxAnimation,clrTrnsp,hIcon,bIsStatic){}

	MyPane(
		UINT   nCmdID,
		LPCTSTR lpszText, 
		UINT   uiAnimationListResID,
		int   cxAnimation = 16,
		COLORREF clrTrnsp= RGB(192,192,192),
		HICON  hIcon = NULL, 
		BOOL   bIsStatic = FALSE)
		:CMFCRibbonStatusBarPane(nCmdID,lpszText,
		uiAnimationListResID,cxAnimation,clrTrnsp, hIcon,bIsStatic){}

	~MyPane(){};

	virtual COLORREF OnFillBackground(CDC* pDC)override
	{
		return m_clrText;
	}

	COLORREF m_clrText;
};

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CProjectExplorer     m_wndProjectExplorer;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg LRESULT OnSmartDrawMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSmartDrawLog(WPARAM wParam, LPARAM lParam);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewProjectExplorer(CCmdUI *pCmdUI);
	afx_msg void OnViewProjectExplorer();
	afx_msg void OnUpdateViewPropertyWindow(CCmdUI *pCmdUI);
	afx_msg void OnViewPropertyWindow();
	afx_msg void OnUpdateViewOutputWindow(CCmdUI *pCmdUI);
	afx_msg void OnViewOutputWindow();
	DECLARE_MESSAGE_MAP()

	void InitializeRibbon();
	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	int DisplayMessage(LPCTSTR lpMsg);
};


