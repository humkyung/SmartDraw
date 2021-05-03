// BmpDlgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BmpDlg.h"
#include "BmpDlgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBmpDlgDlg dialog

CBmpDlgDlg::CBmpDlgDlg(CWnd* pParent /*=NULL*/)
	: CBitmapDialog(CBmpDlgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBmpDlgDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBmpDlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CBitmapDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBmpDlgDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBmpDlgDlg, CBitmapDialog)
	//{{AFX_MSG_MAP(CBmpDlgDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Center, OnCenter)
	ON_BN_CLICKED(IDC_Stretch, OnStretch)
	ON_BN_CLICKED(IDC_Tile, OnTile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpDlgDlg message handlers

BOOL CBmpDlgDlg::OnInitDialog()
{
	CBitmapDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetBitmap(IDB_DlgBackground);
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBmpDlgDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CBitmapDialog::OnPaint();
	}
}

HCURSOR CBmpDlgDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CBmpDlgDlg::OnCenter() 
{
	// m_nType is a protected member of CBitmapDialog
	m_nType = CBitmapDialog::BITMAP_CENTER;
	// force the background to repaint
	Invalidate();
}

void CBmpDlgDlg::OnStretch() 
{
	// m_nType is a protected member of CBitmapDialog
	m_nType = CBitmapDialog::BITMAP_STRETCH;
	// force the background to repaint
	Invalidate();
}

void CBmpDlgDlg::OnTile() 
{
	// m_nType is a protected member of CBitmapDialog
	m_nType = CBitmapDialog::BITMAP_TILE;
	// force the background to repaint
	Invalidate();
}
