// Flow_SlopeMarkPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "SmartDrawPropertySheet.h"
#include "Flow_SlopeMarkPage.h"
#include "SmartDrawDoc.h"

using namespace PropertyPage;
// CFlow_SlopeMarkPage dialog

IMPLEMENT_DYNAMIC(CFlow_SlopeMarkPage, CSmartDrawPropertyPage)

CFlow_SlopeMarkPage::CFlow_SlopeMarkPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CFlow_SlopeMarkPage::IDD, pParent)
{

}

CFlow_SlopeMarkPage::~CFlow_SlopeMarkPage()
{
}

void CFlow_SlopeMarkPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_STATIC_denominator , m_wndDenominatorStatic);

	DDX_Control(pDX , IDC_STATIC_FLOW_MARK , m_wndFlowMarkPicture);
	DDX_Control(pDX , IDC_STATIC_SLOPE_MARK , m_wndSlopeMarkPicture);

	DDX_Control(pDX , IDC_BUTTON_FLOW_MARK_COLOR , m_wndFlowMarkColorButton);
	DDX_Control(pDX , IDC_BUTTON_SLOPE_MARK_TEXT_COLOR , m_wndSlopeMarkTextColorButton);
	DDX_Control(pDX , IDC_COMBO_TEXT_STYLE , m_wndTextStyleComboBox);
}


BEGIN_MESSAGE_MAP(CFlow_SlopeMarkPage, CSmartDrawPropertyPage)
END_MESSAGE_MAP()


// CFlow_SlopeMarkPage message handlers

BOOL PropertyPage::CFlow_SlopeMarkPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();

	{
		LOGFONT LogFont;
		CFont BoldFont;
		BoldFont.CreateFont(16,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0, _T("Arial"));
		BoldFont.GetLogFont(&LogFont);
		m_wndDenominatorStatic.SetTransparent(TRUE);
		m_wndDenominatorStatic.SetFont(LogFont);
		m_wndDenominatorStatic.SetTextColor(RGB(0,0,255));
	}

	m_wndTextStyleComboBox.FillTextStyleList(m_sIniFilePath);
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_wndFlowMarkColorButton.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_wndSlopeMarkTextColorButton.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	LoadData();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int PropertyPage::CFlow_SlopeMarkPage::LoadData(void)
{
	m_wndFlowMarkPicture.Load(GetExecPath() + _T("\\Resource\\flow_mark.bmp"));
	m_wndFlowMarkPicture.Draw();
	m_wndSlopeMarkPicture.Load(GetExecPath() + _T("\\Resource\\slope_mark.bmp"));
	m_wndSlopeMarkPicture.Draw();

	return ERROR_SUCCESS;
}
