// CADFontComboBox.cpp : implementation file
//

#include "stdafx.h"
#include <IsTools.h>
#include "SmartDrawRsc.h"
#include "CADFontComboBox.h"
#include "Resource.h"

#include <algorithm>
#include <vector>
using namespace std;

// CCADFontComboBox

IMPLEMENT_DYNAMIC(CCADFontComboBox, CBitmapComboBox)

CCADFontComboBox::CCADFontComboBox(const CString& rIniFilePath) : m_rIniFilePath(rIniFilePath)
{
	m_nItemHeight = 18;
}

CCADFontComboBox::~CCADFontComboBox()
{
}


BEGIN_MESSAGE_MAP(CCADFontComboBox, CBitmapComboBox)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CCADFontComboBox message handlers
int CCADFontComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(-1 == CBitmapComboBox::OnCreate(lpCreateStruct))
		return -1;
	
	CString rOutputFormat;
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		rOutputFormat = szBuf;
	}

	const CString rFontFolder = GetExecPath() + _T("\\Setting\\Fonts\\") + rOutputFormat;
	FillFontList(rFontFolder , rOutputFormat);

	return 0;
}

static bool __SortByFontNo(const CString& lhs , const CString& rhs)
{
	return (ATOI_T(lhs) < ATOI_T(rhs));
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date
*/
int CCADFontComboBox::FillFontList(const CString& rFontFolder , const CString& sOutputFormat)
{
	vector<CString> oFontFiles;

	CFileFind file;
	///! 확장자 *.vft 파일을 찾는다.
	BOOL bFound = file.FindFile(rFontFolder + _T("\\*.vft"));
	if(bFound)
	{
		USES_CONVERSION;

		TCHAR buf[33] = {'\0',};
		while(bFound)
		{
			bFound = file.FindNextFile();
			if(file.IsDirectory() || file.IsDots()) continue;

			/// get font family.
			FILE *fp = fopen(T2CA(file.GetFilePath()) , "rb");
			if(NULL != fp)
			{
				fseek(fp , 69 , SEEK_SET);
				fread(buf , sizeof(char) , 32 , fp);
				buf[32] = '\0';
				fclose(fp);
			}

			CString rString;
			rString.Format(_T("%s  %s") , file.GetFileTitle().MakeUpper() , buf);
			oFontFiles.push_back( rString );
		}

		stable_sort(oFontFiles.begin() , oFontFiles.end() , __SortByFontNo);
		for(vector<CString>::iterator itr = oFontFiles.begin();itr != oFontFiles.end();++itr)
		{
			this->AddString((_T("MSTN") == sOutputFormat) ? IDB_MSTN : IDB_SHX , (*itr));
		}
	}
	
	return ERROR_SUCCESS;
}