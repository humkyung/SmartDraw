// SmartDrawPDSModule.h : main header file for the SmartDrawPDSModule DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CaDraw_PDSModuleApp
// See aDraw_PDSModule.cpp for the implementation of this class
//

class CaDraw_PDSModuleApp : public CWinApp
{
public:
	CaDraw_PDSModuleApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
