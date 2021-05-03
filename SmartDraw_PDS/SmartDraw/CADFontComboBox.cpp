// CADFontComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "CADFontComboBox.h"


// CCADFontComboBox

IMPLEMENT_DYNAMIC(CCADFontComboBox, CBitmapComboBox)

CCADFontComboBox::CCADFontComboBox(const CString& rIniFilePath) : m_rIniFilePath(rIniFilePath)
{

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
	if(-1 == CBitmapComboBox::OnCreate(lpCreateStruct))
		return -1;
	
	CString rOutputFormat;
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		rOutputFormat = szBuf;
	}

	const CString rFontFolder = GetExecPath() + _T("\\Setting\\Fonts\\") + rOutputFormat;
	FillFontList(rFontFolder);

	return 0;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date
*/
int CCADFontComboBox::FillFontList(const CString& rFontFolder)
{
	CFileFind file;
	///! 확장자 *.vft 파일을 찾는다.
	BOOL bFound = file.FindFile(rFontFolder + _T("\\*.vft"));
	if(bFound)
	{
		char buf[33] = {'\0',};
		while(bFound)
		{
			bFound = file.FindNextFile();
			if(file.IsDirectory() || file.IsDots()) continue;

			//! get font family.
			FILE *fp = fopen(file.GetFilePath() , "rb");
			fseek(fp , 69 , SEEK_SET);
			fread(buf , sizeof(char) , 32 , fp);
			buf[32] = '\0';
			fclose(fp);

			CString rString;
			rString.Format(_T("%s  %s") , file.GetFileTitle().MakeUpper() , buf);
			AddString(IDB_MSTN , rString);
		}
	}
	
	return ERROR_SUCCESS;
}