// aDraw_PDSModule.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <assert.h>
#include "SmartDrawPDSModule.h"

#include "SmartDrawPDSModuleImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CaDraw_PDSModuleApp

BEGIN_MESSAGE_MAP(CaDraw_PDSModuleApp, CWinApp)
END_MESSAGE_MAP()


// CaDraw_PDSModuleApp construction

CaDraw_PDSModuleApp::CaDraw_PDSModuleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CaDraw_PDSModuleApp object

CaDraw_PDSModuleApp theApp;


// CaDraw_PDSModuleApp initialization

BOOL CaDraw_PDSModuleApp::InitInstance()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CWinApp::InitInstance();

	return TRUE;
}

/**
	@brief	write dump file(xml format) from given model
	@author	humkyung
	@return	int
*/
extern "C" __declspec(dllexport)
int __stdcall WriteModelToXml(const LPCTSTR pXmlFilePath , const LPCTSTR pDgnFilePath , const int iDiscipline)
{
	assert(pXmlFilePath && "pXmlFilePath is NULL");
	assert(pDgnFilePath && "pDgnFilePath is NULL");

	if(pXmlFilePath && pDgnFilePath)
	{
		CSmartDrawPDSModuleImpl oImpl;
		return oImpl.WriteModelToXml(pXmlFilePath , pDgnFilePath , iDiscipline);
	}

	return ERROR_INVALID_PARAMETER;
}