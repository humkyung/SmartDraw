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

#pragma once

#include <map>
using namespace std;
/////////////////////////////////////////////////////////////////////////////
// COutputList window

class COutputList : public CMFCListCtrl
{
// Construction
public:
	COutputList();

// Implementation
public:
	virtual ~COutputList();
protected:

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeleteDrawing();
};

class COutputWnd : public CDockablePane
{
// Construction
public:
	COutputWnd();
	static COutputWnd* GetInstance();
// Attributes
protected:
	CFont m_Font;

	CMFCTabCtrl	m_wndTabs;

	COutputList		m_wndOutputDrawing;
	CMFCListCtrl	m_wndOutputLog;	/// 2013.06.09 added by humkyung
protected:
	void AdjustHorzScroll(CListBox& wndListBox);

// Implementation
public:
	virtual ~COutputWnd();

	int DisplayMessage(WPARAM wParam , LPARAM lParam);	/// 2013.06.09 added by humkyung
	CString m_sOutputFolder;
	void StartFolderWatcher(const CString& sOutputFolder,CSmartDrawDoc*);
	void FillDrawingWindow();
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
private:
	CImageList m_cImages;
	map<CString , int> m_oFileIconIndexMap;	/// 2014.06.29 added by humkyung
	HANDLE m_hThread;
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

