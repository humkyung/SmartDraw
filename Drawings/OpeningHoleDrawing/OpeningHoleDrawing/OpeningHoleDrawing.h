// OpeningHoleDrawing.h : main header file for the OpeningHoleDrawing DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// COpeningHoleDrawingApp
// See OpeningHoleDrawing.cpp for the implementation of this class
//

class COpeningHoleDrawingApp : public CWinApp
{
public:
	COpeningHoleDrawingApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
public:
	CString m_sDrawingName;
	CString m_sHsrType;
};
