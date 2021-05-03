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

// stdafx.cpp : source file that includes just the standard includes
// aDraw_CableTray.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#include <Tokenizer.h>
#include <util/SplitPath.h>
#include <util/FileTools.h>
#include "SmartDrawDoc.h"
#include "SmartDrawDocData.h"
#include "MainFrm.h"
#include "StringTable.h"

#include <vector>
#include <fstream>
using namespace std;

/******************************************************************************
    @author     humkyung
    @date       2011-09-07
    @class
    @function   GetMDIActiveDocument
    @return     CSmartDrawDoc*
    @brief
******************************************************************************/
CSmartDrawDoc* GetMDIActiveDocument()
{
	CSmartDrawDoc* pDoc = NULL;

	CWnd* pWndMain = AfxGetMainWnd();
	ASSERT(pWndMain);
	ASSERT(pWndMain->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx))); // Not an MDI app.

	CFrameWnd* pFrame = ((CMDIFrameWndEx*)pWndMain)->MDIGetActive();
	if(NULL != pFrame)
	{
		pDoc = (CSmartDrawDoc*)pFrame->GetActiveDocument(); // get the active document
	}
	
	return pDoc;
}

/**
	@brief	check if file exists
	@author	HumKyung.BAEK
	@date	2010.05.07
*/
BOOL FileExist(CString rFilePath)
{
	if(!rFilePath.IsEmpty())
	{
		HANDLE hFile;

		hFile= CreateFile(rFilePath,
						  0,						/// Open for reading
						  FILE_SHARE_READ,			/// Share for reading
						  NULL,						/// No security
						  OPEN_EXISTING,			/// Existing file only
						  FILE_ATTRIBUTE_NORMAL,	/// Normal file
						  NULL);					/// No template file

		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return TRUE;
		}
	}
	return FALSE;
}

/**
	@brief  Get Module Path
	@author	HumKyung.BAEK
	@date	2010.05.07
*/
CString GetExecPath(void)
{
	TCHAR szFileName[MAX_PATH + 1] = {'\0', };
	GetModuleFileName(NULL , szFileName , MAX_PATH);
	CString rFilePath = szFileName;
	int at = rFilePath.ReverseFind('\\');
	if(-1 != at) rFilePath.Left(at);
	CString rModulePath = rFilePath.Left(at);

	return rModulePath;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.07
*/
COLORREF DarkenColor(COLORREF col,const double& factor)
{
	if(factor>0.0&&factor<=1.0)
	{
		BYTE red,green,blue,lightred,lightgreen,lightblue;
		red = GetRValue(col);
		green = GetGValue(col);
		blue = GetBValue(col);
		lightred = (BYTE)(red-(factor*red));
		lightgreen = (BYTE)(green-(factor*green));
		lightblue = (BYTE)(blue-(factor*blue));
		col = RGB(lightred,lightgreen,lightblue);
	}
	return(col);
}

static DWORD WINAPI MonitorFileCreates(LPVOID pVoid)
{
	STRING_T x = (LPTSTR)(pVoid); 
	SHFILEINFO sfi;
	do
	{
		if(0 != SHGetFileInfo(x.c_str() , FILE_ATTRIBUTE_NORMAL , &sfi , sizeof(sfi) , SHGFI_ATTRIBUTES) )
		{
			break;
		}
		::Sleep( 100 );
		if(KEY_DOWN(VK_ESCAPE)) break;
	}while(1);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Executes the given command using CreateProcess() and WaitForSingleObject().
// Returns FALSE if the command could not be executed or if the exit code could not be determined.
BOOL executeCommandLine(CString cmdLine, DWORD & exitCode)
{
	PROCESS_INFORMATION processInformation = {0};
	STARTUPINFO startupInfo                = {0};
	startupInfo.cb                         = sizeof(startupInfo);
	int nStrBuffer                         = cmdLine.GetLength() + 50;


	// Create the process
	BOOL result = CreateProcess(NULL, cmdLine.GetBuffer(nStrBuffer), 
		NULL, NULL, FALSE, 
		NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, 
		NULL, NULL, &startupInfo, &processInformation);
	cmdLine.ReleaseBuffer();


	if (!result)
	{
		// CreateProcess() failed
		// Get the error from the system
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);

		// Display the error
		CString strError = (LPTSTR)lpMsgBuf;
		CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
		if(pFrame) pFrame->DisplayMessage(strError);

		TRACE(_T("::executeCommandLine() failed at CreateProcess()\nCommand=%s\nMessage=%s\n\n"), cmdLine, strError);

		// Free resources created by the system
		LocalFree(lpMsgBuf);

		// We failed.
		return FALSE;
	}
	else
	{
		// Successfully created the process.  Wait for it to finish.
		WaitForSingleObject( processInformation.hProcess, INFINITE );

		// Get the exit code.
		result = GetExitCodeProcess(processInformation.hProcess, &exitCode);

		// Close the handles.
		CloseHandle( processInformation.hProcess );
		CloseHandle( processInformation.hThread );

		if (!result)
		{
			// Could not get exit code.
			TRACE(_T("Executed command but couldn't get exit code.\nCommand=%s\n"), cmdLine);
			return FALSE;
		}

		// We succeeded.
		return TRUE;
	}
}

/**
	@brief	translate given str for locale
	@author	humkyung
	@date	2015.08.29
	@return	CString
*/
CString _TR(const CString& str)
{
	CStringTable& table = CStringTable::GetInstance();
	return table.GetString(str.operator LPCTSTR()).c_str();
}