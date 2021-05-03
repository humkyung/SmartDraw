// LineNoLabelFormatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "LineNoLabelFormatDlg.h"


// CLineNoLabelFormatDlg dialog

IMPLEMENT_DYNAMIC(CLineNoLabelFormatDlg, CDialog)

CLineNoLabelFormatDlg::CLineNoLabelFormatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLineNoLabelFormatDlg::IDD, pParent)
{

}

CLineNoLabelFormatDlg::~CLineNoLabelFormatDlg()
{
}

void CLineNoLabelFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_BUTTON_ADD , m_wndAddButton);
	DDX_Control(pDX , IDC_LIST_LINE_NO_LABEL_ITEM , m_wndLineNoLabelEditListBox);
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CLineNoLabelFormatDlg, CDialog)
END_MESSAGE_MAP()


// CLineNoLabelFormatDlg message handlers

BOOL CLineNoLabelFormatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_ADD),
			IMAGE_ICON,
			0,
			0,
			0);
		HICON hOldIcon = m_wndAddButton.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndAddButton.SetIcon(hIcon);
	}

	m_wndEditListBoxBuddy.SubclassDlgItem( IDC_LISTBUDDY, this );
	m_wndEditListBoxBuddy.SetListbox( &m_wndLineNoLabelEditListBox );

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_OK),
			IMAGE_ICON,
			0,
			0,
			0);
		HICON hOldIcon = m_wndOKButton.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndOKButton.SetIcon(hIcon);
	}
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_CANCEL),
			IMAGE_ICON,
			0,
			0,
			0);
		HICON hOldIcon = m_wndCancelButton.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndCancelButton.SetIcon(hIcon);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
