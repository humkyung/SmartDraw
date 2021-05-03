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

// aDraw_CableTray.h : main header file for the aDraw_CableTray application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "StdAfx.h"

#include "resource.h"       // main symbols
#include "singleinstance.h"

#include <comdef.h>
#include <afxtempl.h>

// CSmartDrawApp:
// See aDraw_CableTray.cpp for the implementation of this class
//
class CSmartDrawApp : public CWinAppEx , public CSingleInstance
{
public:
	CSmartDrawApp();

// Overrides
public:
	virtual BOOL InitInstance();
	int OpenNewDocument(const CString& rFilePath);
// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	CImageList m_aDrawImages;	/// 2012.02.11 added by humkyung
	
	HRESULT CallManagedFunction(BSTR szMethodName, int iNoOfParams, VARIANT * pvArgs, VARIANT * pvRet);	/// 2012.09.17 added by humkyung

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
private:
	int CreateImageList(void);
	int LoadPlugins();		/// 2012.03.11 added by humkyung
private:
};

extern CSmartDrawApp theApp;
