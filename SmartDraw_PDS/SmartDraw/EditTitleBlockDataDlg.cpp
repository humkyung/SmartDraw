// EditTitleBlockDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include <Tokenizer.h>
#include "SmartDrawDocData.h"
#include "SmartDrawDoc.h"
#include "EditTitleBlockDataDlg.h"

#include <list>
using namespace std;

// CEditTitleBlockDataDlg dialog

IMPLEMENT_DYNAMIC(CEditTitleBlockDataDlg, CDialog)

CEditTitleBlockDataDlg::CEditTitleBlockDataDlg(list<CaDraw_DraFile*> oDraFileList,CWnd* pParent /*=NULL*/)
	: CDialog(CEditTitleBlockDataDlg::IDD, pParent)
{
	m_oDraFileList.insert(m_oDraFileList.end() , oDraFileList.begin() , oDraFileList.end());
}

CEditTitleBlockDataDlg::~CEditTitleBlockDataDlg()
{
	try
	{
		for(map<CString,CGridCtrl*>::iterator itr = m_oBorderGridCtrlList.begin();itr != m_oBorderGridCtrlList.end();++itr)
		{
			itr->second->DestroyWindow();
			SAFE_DELETE(itr->second);
		}

		m_oBorderGridCtrlList.clear();

	}
	catch(...)
	{
	}
}

void CEditTitleBlockDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX , IDOK , m_btnOK);
	DDX_Control(pDX , IDCANCEL , m_btnCancel);
}


BEGIN_MESSAGE_MAP(CEditTitleBlockDataDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CEditTitleBlockDataDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CEditTitleBlockDataDlg message handlers
BOOL CEditTitleBlockDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect;
	GetDlgItem(IDC_STATIC_GRID_CTRL)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	if (!m_wndTabCtrl.Create (CMFCTabCtrl::STYLE_3D_VS2005, rect , this , WM_USER + 1))
	{
		TRACE0("Failed to create Properies Grid \n");
		return FALSE;      /// fail to create
	}
	m_wndTabCtrl.SetLocation(CMFCTabCtrl::LOCATION_TOP);
	m_wndTabCtrl.SetActiveTabBoldFont(TRUE);
	m_wndTabCtrl.EnableAutoColor(TRUE);
	m_wndTabCtrl.SetWindowPos(&(CWnd::wndTop) , 0 , 0 , 0 , 0 , SWP_NOSIZE | SWP_NOMOVE);
	m_wndTabCtrl.ShowWindow(SW_SHOW);

	static const CString sApp(_T("GENERAL"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	list<CString> oSettingFileList;

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	const CString sServerFolderPath = docData.GetServerFolderPath();
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	const CString sPrjName = pDoc->GetProjectName();
	for(list<CaDraw_DraFile*>::const_iterator itr = m_oDraFileList.begin();itr != m_oDraFileList.end();++itr)
	{
		const CString sIniFileName = (*itr)->GetSettingFileName();
		if(oSettingFileList.end() == find(oSettingFileList.begin() , oSettingFileList.end() , sIniFileName))
		{
			oSettingFileList.push_back(sIniFileName);
			InitGridCtrl(sIniFileName);
		}
	}

	LoadData();
		
	HICON hOKIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_OK), IMAGE_ICON, 16,16, 0 );
	HICON hOldIcon = m_btnOK.SetIcon(hOKIcon);
	if(hOldIcon) DestroyIcon(hOldIcon);
	HICON hCancelIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_CANCEL),IMAGE_ICON, 16,16, 0 );
	hOldIcon = m_btnCancel.SetIcon(hCancelIcon);
	if(hOldIcon) DestroyIcon(hOldIcon);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
	@brief		initialize grid control
	@author     humkyung
    @date       2015-07-17
    @function   InitGridCtrl
    @return     int
******************************************************************************/
int CEditTitleBlockDataDlg::InitGridCtrl(const CString& sSettingFileName)
{
	static const CString sApp(_T("GENERAL"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	const CString sServerFolderPath = docData.GetServerFolderPath();
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	const CString sPrjName = pDoc->GetProjectName();

	CRect rect;
	GetDlgItem(IDC_STATIC_GRID_CTRL)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	CGridCtrl* pGridCtrl = new CGridCtrl();
	if(TRUE == pGridCtrl->Create(rect , &m_wndTabCtrl , WM_USER + 2 + m_oBorderGridCtrlList.size()))
	{
		/// 그리드 컨트롤 폰트 코드
		CFont font;
		font.CreateFont(-11,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,_T("Arial")); 
		pGridCtrl->SetFont(&font);
		font.DeleteObject();

		pGridCtrl->SetFixedRowCount(1);
		pGridCtrl->SetFixedColumnCount(1);

		const CString sIniFilePath = sServerFolderPath + _T("\\") + sPrjName + _T("\\Setting\\") + sSettingFileName + _T(".ini");
		//const CString sDraIniFilePath = CString(STRING_T(pDraFile->GetFilePath() + _T(".ini")).c_str());
		if(GetPrivateProfileString(sApp , _T("Border File") , _T("") , szBuf , MAX_PATH , sIniFilePath))
		{
			CString sKey;
			vector<STRING_T> oResult;

			const CString sBorderCfgFilePath = sServerFolderPath + _T("\\") + sPrjName + _T("\\Border\\") + szBuf + _T(".cfg");
			const UINT uiCount = GetPrivateProfileInt(_T("TITLE BLOCK") , _T("Count") , 0 , sBorderCfgFilePath);
			pGridCtrl->SetColumnCount(uiCount + 1);
			for(int i = 0;i < int(uiCount);++i)
			{
				sKey.Format(_T("Data%d") , i);	
				if(GetPrivateProfileString(_T("TITLE BLOCK") , sKey , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath))
				{
					CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
					if(!oResult.empty()) pGridCtrl->SetItemText(0 , i + 1,oResult[0].c_str());
				}
			}
		}
		pGridCtrl->SetRowHeight(0, DEFAULT_GRID_ROW_HEIGHT);

		/// fill it up with stuff
		pGridCtrl->SetEditable(TRUE);
		pGridCtrl->EnableDragAndDrop(FALSE);
		pGridCtrl->SetTextBkColor(RGB(255, 255, 224));
		pGridCtrl->SetRowResize(FALSE);
		pGridCtrl->SetColumnResize(TRUE);
		pGridCtrl->SetListMode(FALSE);
		pGridCtrl->SetSingleRowSelection(TRUE);
		pGridCtrl->SetHeaderSort(FALSE);
		pGridCtrl->SetSingleColSelection(TRUE);
		///////////////////////////////////////////////////////////////////
		m_wndTabCtrl.AddTab(pGridCtrl , sSettingFileName);

		m_oBorderGridCtrlList.insert(make_pair(sSettingFileName , pGridCtrl));

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/******************************************************************************
	@brief		load title block data
	@author     humkyung
    @date       2015-07-20
    @function   LoadData
    @return     int
******************************************************************************/
int CEditTitleBlockDataDlg::LoadData()
{
	TCHAR szBuf[MAX_PATH+1]={'\0',};
	for(list<CaDraw_DraFile*>::const_iterator itr = m_oDraFileList.begin();itr != m_oDraFileList.end();++itr)
	{
		const CString sSettingFileName = (*itr)->GetSettingFileName();
		map<CString,CGridCtrl*>::const_iterator where = m_oBorderGridCtrlList.find(sSettingFileName);
		if(where != m_oBorderGridCtrlList.end())
		{
			CGridCtrl* pGridCtrl = where->second;
			const int iRowCount = pGridCtrl->GetRowCount();
			pGridCtrl->SetRowCount(iRowCount + 1);
			pGridCtrl->SetItemText(iRowCount , 0 , (*itr)->GetFileNameWithoutExt().c_str());
			pGridCtrl->SetRowHeight(iRowCount , DEFAULT_GRID_ROW_HEIGHT);

			const CString sDraIniFilePath = CString(STRING_T((*itr)->GetFilePath() + _T(".ini")).c_str());
			for(int col = 1;col < where->second->GetColumnCount();++col)
			{
				const CString sTitleBlockName = where->second->GetItemText(0 , col);
				if(GetPrivateProfileString(_T("Title Block Data") , sTitleBlockName , _T("") , szBuf , MAX_PATH , sDraIniFilePath))
				{
					where->second->SetItemText(iRowCount , col , szBuf);
				}
			}
		}
	}
	
	return ERROR_SUCCESS;
}

/******************************************************************************
	@brief		save title block data
	@author     humkyung
    @date       2015-07-20
    @function   LoadData
    @return     int
******************************************************************************/
int CEditTitleBlockDataDlg::SaveData()
{
	TCHAR szBuf[MAX_PATH+1]={'\0',};
	for(list<CaDraw_DraFile*>::const_iterator itr = m_oDraFileList.begin();itr != m_oDraFileList.end();++itr)
	{
		const CString sSettingFileName = (*itr)->GetSettingFileName();
		map<CString,CGridCtrl*>::const_iterator where = m_oBorderGridCtrlList.find(sSettingFileName);
		if(where != m_oBorderGridCtrlList.end())
		{
			CGridCtrl* pGridCtrl = where->second;
			const CString sDraFileName = (*itr)->GetFileNameWithoutExt().c_str();
			int row = 1;
			for(row = 1;row < pGridCtrl->GetRowCount();++row)
			{
				if(sDraFileName == pGridCtrl->GetItemText(row , 0)) break;
			}

			if(row < pGridCtrl->GetRowCount())
			{
				const CString sDraIniFilePath = CString(STRING_T((*itr)->GetFilePath() + _T(".ini")).c_str());
				for(int col = 1;col < where->second->GetColumnCount();++col)
				{
					const CString sTitleBlockName = where->second->GetItemText(0 , col);
					const CString sValue = where->second->GetItemText(row , col);
					WritePrivateProfileString(_T("Title Block Data") , sTitleBlockName , sValue , sDraIniFilePath);
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	save title block data
	@author	humkyung
	@date	2015.07.21
*/
void CEditTitleBlockDataDlg::OnBnClickedOk()
{
	SaveData();

	OnOK();
}
