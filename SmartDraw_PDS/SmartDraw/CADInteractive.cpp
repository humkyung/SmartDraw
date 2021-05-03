#include "StdAfx.h"
#include <Tokenizer.h>
#include <util/SplitPath.h>
#include <util/FileTools.h>
#include "CADInteractive.h"
#include "SmartDrawDocData.h"
#include "SmartDrawDoc.h"
#include "ProgressAnimationDlg.h"

#include <fstream>
using namespace std;

CCADInteractive::CCADInteractive(void)
{
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

/**
	@brief	
	@author humkyung
	@date	2014.06.21
	@param	
	@return		
*/
static UINT gfncOpenDwgFile(LPVOID pVoid)
{
	CProgressAnimationDlg& anidlg = CProgressAnimationDlg::GetInstance();

	if(NULL == anidlg.GetSafeHwnd())
	{
		anidlg.DoModal();
	}
	
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-28
    @function   GetInputPoint
    @return     int
    @param      double& x
    @param      double& y
    @brief
******************************************************************************/
int CCADInteractive::GetInputPoint(double& x , double& y , const STRING_T& sBorderFilePath , const STRING_T& sCommandFilePath)
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	GetTempPath(MAX_PATH , szBuf);
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	{
		IFSTREAM_T ifile(STRING_T(szBuf + STRING_T(_T("aPickTool.dat"))).c_str());
		if(ifile.is_open())
		{
			STRING_T aLine;
			getline(ifile , aLine);

			vector<STRING_T> oResult;
			CTokenizer<CIsComma>::Tokenize(oResult , aLine , CIsComma());
			if(3 == oResult.size())
			{
				x = ATOF_T(oResult[0].c_str());
				y = ATOF_T(oResult[1].c_str());
			}
			ifile.close();
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}