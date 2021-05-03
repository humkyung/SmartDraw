// DimensionPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include <Tokenizer.h>
#include <SmartDrawAnnoTextStyle.h>
#include "SmartDrawPropertySheet.h"
#include "DimensionPage.h"
#include "SmartDrawDocData.h"
#include "SmartDrawDoc.h"

using namespace PropertyPage;
// CDimensionPage dialog

IMPLEMENT_DYNAMIC(CIntDimStylePage, CDimensionPage)

CIntDimStylePage::CIntDimStylePage(CWnd* pParent /*=NULL*/) : CDimensionPage(pParent)
{
	m_sDimStyle = _T("Int Dim Style");
}
CIntDimStylePage::~CIntDimStylePage()
{}

IMPLEMENT_DYNAMIC(CExtDimStylePage, CDimensionPage)
CExtDimStylePage::CExtDimStylePage(CWnd* pParent /*=NULL*/) : CDimensionPage(pParent)
{
	m_sDimStyle = _T("Ext Dim Style");
}
CExtDimStylePage::~CExtDimStylePage()
{}

IMPLEMENT_DYNAMIC(CDimensionPage, CSmartDrawPropertyPage)

CDimensionPage::CDimensionPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CDimensionPage::IDD, pParent)
{
}

CDimensionPage::~CDimensionPage()
{
	try
	{
	}
	catch(...)
	{
	}
}

void CDimensionPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_COMBO_TEXT_LEVEL , m_cboTextLevel);
	DDX_Control(pDX , IDC_BUTTON_TEXT_COLOR , m_wndTextColorButton);
	DDX_Control(pDX , IDC_BUTTON_DIM_LINE_COLOR , m_wndDimLineColorButton);
	DDX_Control(pDX , IDC_BUTTON_EXT_LINE_COLOR , m_wndExtLineColorButton);
	DDX_Control(pDX , IDC_COMBO_ARROW_TYPE , m_wndArrowTypeComboBox);
	DDX_Control(pDX , IDC_COMBO_TEXT_STYLE , m_wndTextStyleComboBox);

	DDX_Text(pDX , IDC_EDIT_TEXT_OFFSET , m_rTextOffset);
	DDX_Text(pDX , IDC_EDIT_DIM_LINE_DISTANCE , m_rDimLineDistance);
	DDX_Text(pDX , IDC_EDIT_EXT_LINE_EXTENSION , m_rExtLineExtension);
	DDX_Text(pDX , IDC_EDIT_EXT_LINE_OFFSET , m_rExtLineOffset);

	DDX_Text(pDX , IDC_EDIT_ARROW_SIZE , m_rArrowSize);

	DDX_Control(pDX , IDC_CHECK_AUTO_SIZE , m_wndAutoSize);	/// 2014.11.16 added by humkyung
}


BEGIN_MESSAGE_MAP(CDimensionPage, CSmartDrawPropertyPage)
END_MESSAGE_MAP()


// CDimensionPage message handlers
/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.22
*/
int PropertyPage::CDimensionPage::LoadData(void)
{
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	const CString rApp("DIMENSION_STYLE");

	vector<STRING_T> oResult;
	if(GetPrivateProfileString(rApp , m_sDimStyle , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
	}

	if((13 == oResult.size()) || (14 == oResult.size()))
	{
		int nFound = m_wndTextStyleComboBox.FindString(-1 , oResult[0].c_str());
		m_wndTextStyleComboBox.SetCurSel((-1 != nFound) ? nFound : 0);

		int at = m_cboTextLevel.FindString(-1 , oResult[1].c_str());
		m_cboTextLevel.SetCurSel((-1 != at) ? at : 0);

		m_wndTextColorButton.SetColorIndex( ATOI_T(oResult[2].c_str()) );
		m_rTextOffset = oResult[3].c_str();

		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_PRECISION);
		nFound = pComboBox->FindString(-1 , oResult[4].c_str());
		if(-1 != nFound) pComboBox->SetCurSel((-1 != nFound) ? nFound : 0);

		m_wndDimLineColorButton.SetColorIndex(ATOI_T(oResult[5].c_str()));
		m_rDimLineDistance = oResult[6].c_str();
		
		m_wndExtLineColorButton.SetColorIndex( ATOI_T(oResult[7].c_str()) );
		m_rExtLineOffset = oResult[8].c_str();
		m_rExtLineExtension = oResult[9].c_str();
	
		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_ARROW_TYPE);
			nFound = pComboBox->FindString(-1 , oResult[10].c_str());
			pComboBox->SetCurSel((-1 != nFound) ? nFound : 0);
		}

		m_rArrowSize = oResult[11].c_str();
		oResult[12].c_str();

		/// 2014.11.16 added by humkyung
		if(14 == oResult.size()) m_wndAutoSize.SetCheck((_T("ON") == oResult[13]));
		/// up to here
	}

	return 0;
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.22
*/
BOOL PropertyPage::CDimensionPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();
	
	m_wndTextStyleComboBox.FillTextStyleList(m_sIniFilePath);
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_wndTextColorButton.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_wndDimLineColorButton.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_wndExtLineColorButton.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	
	/// fill level data - 2014.06.23 added by humkyung
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	if(_T("MSTN") != sOutputFormat) m_cboTextLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
	m_cboTextLevel.LoadContents(m_sIniFilePath);
	/// up to here

	m_wndAutoSize.SetLabels(_T("ON") , _T("OFF"));

	LoadData();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.22
*/
int PropertyPage::CDimensionPage::SaveData(void)
{
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	static const CString rApp("DIMENSION_STYLE");
	
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	
	UpdateData();

	CString rString , sDimStyle;
	int nSel = m_wndTextStyleComboBox.GetCurSel();
	if(-1 != nSel)
	{
		m_wndTextStyleComboBox.GetLBText(nSel , rString);

		CSmartDrawAnnoTextStyle oTextStyle;
		oTextStyle.Parse(rString.operator LPCTSTR() , m_sIniFilePath.operator LPCTSTR());

		sDimStyle += rString + _T(",");
	}
	
	int at = m_cboTextLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboTextLevel.GetLBText(at , rString);
		sDimStyle += rString + _T(",");
	}
	else
	{
		sDimStyle += _T(",");
	}

	rString.Format(_T("%d") , m_wndTextColorButton.GetColorIndex());
	sDimStyle += rString + _T(",");
	sDimStyle += m_rTextOffset + _T(","); 
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_PRECISION);
	nSel = pComboBox->GetCurSel();
	if(-1 != nSel)
	{
		pComboBox->GetLBText(nSel , rString);
		sDimStyle += rString + _T(","); 
	}
	else
	{
		sDimStyle += _T(","); 
	}

	rString.Format(_T("%d") , m_wndDimLineColorButton.GetColorIndex());
	sDimStyle += rString + _T(",") + m_rDimLineDistance + _T(",");

	rString.Format(_T("%d") , m_wndExtLineColorButton.GetColorIndex());
	sDimStyle += rString + _T(",");
	sDimStyle += m_rExtLineOffset + _T(",");
	sDimStyle += m_rExtLineExtension + _T(",");

	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_ARROW_TYPE);
	nSel = pComboBox->GetCurSel();
	if(-1 != nSel)
	{
		pComboBox->GetLBText(nSel , rString);
		sDimStyle += rString + _T(",");
	}
	sDimStyle += m_rArrowSize + _T(",") + m_rArrowSize;

	sDimStyle += CString(_T(",")) + (m_wndAutoSize.GetCheck() ? _T("ON") : _T("OFF"));

	WritePrivateProfileString(rApp , m_sDimStyle , sDimStyle , m_sIniFilePath);
	m_cboTextLevel.SaveContents(m_sIniFilePath);	/// 2014.06.23 added by humkyung

	return ERROR_SUCCESS;
}
