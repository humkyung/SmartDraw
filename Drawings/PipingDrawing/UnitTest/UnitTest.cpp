// UnitTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "UnitTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

typedef LPCTSTR (__stdcall *GetDrawingName)();
typedef int (__stdcall *Annotate)(LPCTSTR pOutputFilePath , LPCTSTR pDraFilePath , LPCTSTR pPrjName , LPCTSTR pPrjFolderPath);

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		HMODULE hModule = AfxLoadLibrary(_T("e:\\PROJECT\\SolutionWare\\Projects\\2012.01.SmartDraw\\lib\\PipingDrawing_vc90_debug.dll"));
		if(hModule)
		{
			GetDrawingName pfGetDrawingName = (GetDrawingName)GetProcAddress(hModule , _T("_GetDrawingName@0"));
			if(NULL != pfGetDrawingName)
			{
				AfxMessageBox(pfGetDrawingName());
			}

			Annotate pfAnnotate = (Annotate)GetProcAddress(hModule , _T("_Annotate@16"));
			if(NULL != pfAnnotate) pfAnnotate(
				_T("e:\\6000-001.anno") , 
				_T("e:\\PROJECT\\SolutionWare\\Projects\\2012.01.SmartDraw\\SmartDraw_PDS\\Data\\DEMO_A\\Dra\\6000-001.dra") , 
				_T("DEMO_A"),
				_T("e:\\PROJECT\\SolutionWare\\Projects\\2012.01.SmartDraw\\SmartDraw_PDS\\Data\\DEMO_A"));

		}
	}

	return nRetCode;
}
