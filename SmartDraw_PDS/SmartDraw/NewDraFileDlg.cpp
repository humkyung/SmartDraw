// NewDraFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "NewDraFileDlg.h"


// CNewDraFileDlg dialog

IMPLEMENT_DYNAMIC(CNewDraFileDlg, CDialog)

CNewDraFileDlg::CNewDraFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewDraFileDlg::IDD, pParent)
{

}

CNewDraFileDlg::~CNewDraFileDlg()
{
}

void CNewDraFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX , IDC_EDIT_SDR_NAME , m_sSdrName);

	DDX_Control(pDX , IDOK , m_btnOK);
	DDX_Control(pDX , IDCANCEL , m_btnCancel);
}


BEGIN_MESSAGE_MAP(CNewDraFileDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CNewDraFileDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CNewDraFileDlg message handlers

void CNewDraFileDlg::OnBnClickedOk()
{
	UpdateData();

	OnOK();
}

BOOL CNewDraFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDI_DRA);
	HICON hOldIcon = SetIcon(hIcon , FALSE);
	if(hOldIcon) DestroyIcon(hOldIcon);

	{
		hIcon = (HICON) LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_OK),IMAGE_ICON,
			16,16,0);
		hOldIcon = m_btnOK.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_btnOK.SetIcon(hIcon);
	}
	{
		hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_CANCEL),IMAGE_ICON,
			16,16,0);
		hOldIcon = m_btnCancel.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_btnCancel.SetIcon(hIcon);
	}

	GetDlgItem(IDC_EDIT_SDR_NAME)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
