// InsertRevNoDlg.cpp : implementation file
//

#include "stdafx.h"
#include <Tokenizer.h>
#include "SmartDraw.h"
#include "InsertRevNoDlg.h"
#include "SmartDrawDoc.h"

// CInsertRevNoDlg dialog

IMPLEMENT_DYNAMIC(CInsertRevNoDlg, CDialog)

CInsertRevNoDlg::CInsertRevNoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInsertRevNoDlg::IDD, pParent)
{

}

CInsertRevNoDlg::~CInsertRevNoDlg()
{
}

void CInsertRevNoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX , IDOK , m_btnOK);
	DDX_Control(pDX , IDCANCEL , m_btnCancel);
}


BEGIN_MESSAGE_MAP(CInsertRevNoDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_AUTO, &CInsertRevNoDlg::OnBnClickedRadioAuto)
	ON_BN_CLICKED(IDC_RADIO_SELECT, &CInsertRevNoDlg::OnBnClickedRadioSelect)
	ON_BN_CLICKED(IDOK, &CInsertRevNoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CInsertRevNoDlg message handlers

BOOL CInsertRevNoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->SetIcon((HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16,16, 0 ) , FALSE);

	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_AUTO);
	pButton->SetCheck(TRUE);
	OnBnClickedRadioAuto();

	CComboBox* pComboBox = (CComboBox*)(GetDlgItem(IDC_COMBO_REV_NO));
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	vector<CRevisionData*>* pRevDataList = pDoc->m_oRevisionSource.GetRevisionDataList();
	for(vector<CRevisionData*>::iterator itr = pRevDataList->begin();itr != pRevDataList->end();++itr)
	{
		pComboBox->AddString((*itr)->Value(_T("REV_NO")).c_str());
	}

	HICON hOKIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_OK), IMAGE_ICON, 16,16, 0 );
	m_btnOK.SetIcon(hOKIcon);
	
	HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_CANCEL),IMAGE_ICON, 16,16, 0 );
	m_btnCancel.SetIcon(hCancelIcon);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CInsertRevNoDlg::OnBnClickedRadioAuto()
{
	GetDlgItem(IDC_COMBO_REV_NO)->EnableWindow(FALSE);
}

void CInsertRevNoDlg::OnBnClickedRadioSelect()
{
	GetDlgItem(IDC_COMBO_REV_NO)->EnableWindow(TRUE);
}

/**
	@brief	insert selected rev. no to selected drawing(s)
	@author	humkyung
	@date	2014.11.16
*/
void CInsertRevNoDlg::OnBnClickedOk()
{
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	
	CString sSelectedRevNo , sValue , rString;
	CRevisionData* pRevData = NULL;
	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_SELECT);
	if(TRUE == pButton->GetCheck())
	{
		CComboBox* pComboBox = (CComboBox*)(GetDlgItem(IDC_COMBO_REV_NO));
		pComboBox->GetWindowText(sSelectedRevNo);
		pRevData = pDoc->m_oRevisionSource.GetRevDataOf(sSelectedRevNo.operator LPCTSTR());
		if(NULL != pRevData) sValue = pRevData->ToString().c_str();
	}

	static const STRING_T sApp(_T("Revision Data"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	vector<STRING_T> oResult;

	list<CaDraw_DraFile*> oSelectedDrawingList;
	pDoc->GetSelectedDrawings( oSelectedDrawingList );
	for(list<CaDraw_DraFile*>::const_iterator itr = oSelectedDrawingList.begin();itr != oSelectedDrawingList.end();++itr)
	{
		const STRING_T sDraIniFilePath = (*itr)->GetFilePath() + _T(".ini");
		const UINT uiCount = GetPrivateProfileInt(sApp.c_str() , _T("Count") , 0 , sDraIniFilePath.c_str()); 
		int i = 0;
		if(!sSelectedRevNo.IsEmpty() && (NULL != pRevData))
		{
			/// try to find rev. no in drawing, if found, update the revision data.
			for(i = 0;i < int(uiCount);++i)
			{
				rString.Format(_T("Data%d") , i);
				if(GetPrivateProfileString(sApp.c_str() , rString , _T("") , szBuf , MAX_PATH , sDraIniFilePath.c_str()))
				{
					CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
					if(!oResult.empty())
					{
						if(pRevData->Value(_T("REV_NO")) == oResult[0])
						{
							WritePrivateProfileString(sApp.c_str() , rString , sValue , sDraIniFilePath.c_str());
							break;
						}
					}
				}
			}
			/// up to here
		}
		else if(sSelectedRevNo.IsEmpty())
		{
			sValue.Empty();
			if(uiCount > 0)
			{
				rString.Format(_T("Data%d") , uiCount - 1);
				if(GetPrivateProfileString(sApp.c_str() , rString , _T("") , szBuf , MAX_PATH , sDraIniFilePath.c_str()))
				{
					CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
					if(!oResult.empty())
					{
						const STRING_T sLastRevNo = oResult[0];
						pRevData = pDoc->m_oRevisionSource.GetNextRevDataOf(sLastRevNo);
						if(NULL != pRevData)
						{
							sValue = pRevData->ToString().c_str();
							i = uiCount;
						}
					}
				}
			}
			else
			{
				pRevData = pDoc->m_oRevisionSource.GetRevDataOf(0);
				if(NULL != pRevData)
				{
					sValue = pRevData->ToString().c_str();
					i = uiCount;
				}
			}
		}

		if((i == uiCount) && !sValue.IsEmpty())
		{
			rString.Format(_T("Data%d") , uiCount);
			WritePrivateProfileString(sApp.c_str() , rString , sValue , sDraIniFilePath.c_str());
			rString.Format(_T("%d") , uiCount + 1);
			WritePrivateProfileString(sApp.c_str() , _T("Count") , rString , sDraIniFilePath.c_str());

			///#22(http://atools.co.kr:9002/redmine/issues/22) - 2016.03.28 added by humkyung
			CTokenizer<CIsComma>::Tokenize(oResult,sValue.operator LPCTSTR(),CIsComma());
			WritePrivateProfileString(_T("Title Block Data") , _T("REV") , oResult[0].c_str() , sDraIniFilePath.c_str());
			///up to here
		}
	}

	OnOK();
}
