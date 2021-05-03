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

// aDraw_CableTrayView.h : interface of the CSmartDrawView class
//


#pragma once

#include "DraTreeListCtrl.h"
#include "..\..\SmartDrawModule\aDraw_DraFile.h"

class CSmartDrawView : public CView
{
protected: // create from serialization only
	CSmartDrawView();
	DECLARE_DYNCREATE(CSmartDrawView)

// Attributes
public:
	CSmartDrawDoc* GetDocument() const;

// Operations
public:
	CImageList m_cImages;
	CDraTreeListCtrl m_wndDraTreeListCtrl;

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CSmartDrawView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnOptionSetting();
	afx_msg void OnUpdateOptionSetting(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	int InitTreeList(void);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	int OnGenerate();
	int GetAllDrawings(list<CaDraw_DraFile*>& oSelectedDrawingList);	/// 2016.03.07 added by humkyung
	int GetSelectedDrawings(list<CaDraw_DraFile*>& oSelectedDrawingList);
private:
	static UINT StatusThreadEntry(LPVOID pVoid);
	int StatusThread(void);
	int GenerateOutput(CaDraw_DraFile* pDraFile);
	int GenerateCmdFile(const CString& rFilePath , const CString& sCadFilePath , const CString& sHsrFilePath , const CString& sAnnoFilePath , 
		const CString& sOutputSettingFilePath , const double& dDrawingScale) const;
	CString GenerateSettingOutput(const CString& sIniFilePath);	/// 2014.06.29 added by humkyung
};

#ifndef _DEBUG  // debug version in aDraw_CableTrayView.cpp
inline CSmartDrawDoc* CSmartDrawView::GetDocument() const
   { return reinterpret_cast<CSmartDrawDoc*>(m_pDocument); }
#endif

