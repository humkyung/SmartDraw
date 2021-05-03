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

#include <IsTools.h>
#include <gui/ResizeWnd.h>
#include "ViewTree.h"
#include "UserPasswordPropertyGrid.h"

#include <map>
using namespace std;

class CProjectExplorerToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CProjectExplorer : public CDockablePane
{
// Construction
public:
	CProjectExplorer();

	static CProjectExplorer* GetInstance();	/// 2012.02.13 added by humkyung
	int ActivateTreeItem(CSmartDrawDoc* pDoc);///const STRING_T& sProjectName , const STRING_T& sProjectDesc , const STRING_T& sDrawingType);	/// 2012.02.13 added by humkyung

	void AdjustLayout();
	void OnChangeVisualStyle();

// Attributes
protected:
	CFont m_fntPropList;

	CViewTree m_wndFileView;
	CImageList m_FileViewImages;
	CProjectExplorerToolBar m_wndToolBar;

	CResizeWnd m_wndResize;

	CMFCPropertyGridCtrl m_wndPropList;

protected:
	int UpdateTreeItem(const STRING_T& sPrjName , const STRING_T& sPrjDesc);	/// 2014.06.03 added by humkyung
	void FillProjectTree();
	void InitPropList();
	void SetPropListFont();

	map<STRING_T , HTREEITEM> m_oDrawingTypeMap;	/// 2012.02.11 added by humkyung
	map<STRING_T , HTREEITEM> m_oProjectNameMap;	/// 2014.06.04 added by humkyung
// Implementation
public:
	virtual ~CProjectExplorer();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnHorzResize();
	afx_msg LRESULT OnPropertyChanged(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnProjectNew();
	afx_msg void OnProjectDelete();
	afx_msg void OnProjectEdit();
	afx_msg void OnUpdateProjectEdit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateProjectDelete(CCmdUI *pCmdUI);
};

