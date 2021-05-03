// CellLibraryFileBrowseEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDrawRsc.h"
#include "CellLibraryFileBrowseEdit.h"


// CCellLibraryFileBrowseEdit

IMPLEMENT_DYNAMIC(CCellLibraryFileBrowseEdit, CMFCEditBrowseCtrl)

CCellLibraryFileBrowseEdit::CCellLibraryFileBrowseEdit()
{

}

CCellLibraryFileBrowseEdit::~CCellLibraryFileBrowseEdit()
{
}


BEGIN_MESSAGE_MAP(CCellLibraryFileBrowseEdit, CMFCEditBrowseCtrl)
END_MESSAGE_MAP()


// CCellLibraryFileBrowseEdit message handlers
/******************************************************************************
    @author     humkyung
    @date       2012-03-14
    @function   OnBrowse
    @return     void
    @brief
******************************************************************************/
void CCellLibraryFileBrowseEdit::OnBrowse()
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CString sOutputFormat;
	if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		sOutputFormat = szBuf;
		sOutputFormat.MakeUpper();
	}

	if(_T("MSTN") == sOutputFormat)
	{
		// 파일 Open 
		CFileDialog dlg(TRUE,_T("cel,*.*"),_T(""),OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
			_T("Cell Library File(*.cel)|*.cel|모든 문서(*.*)|*.*||"));
		if(IDOK == dlg.DoModal())
		{
			m_rFilePath = dlg.GetPathName();
			SetWindowText(m_rFilePath);
		}
	}
	else if(_T("AUTOCAD") == sOutputFormat)
	{
		// 파일 Open 
		CFileDialog dlg(TRUE,_T("dwg,*.*"),_T(""),OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
			_T("AutoCAD File(*.dwg)|*.dwg|모든 문서(*.*)|*.*||"));
		if(IDOK == dlg.DoModal())
		{
			m_rFilePath = dlg.GetPathName();
			SetWindowText(m_rFilePath);
		}
	}
}

