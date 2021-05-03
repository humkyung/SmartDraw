// NozzleChartPage.cpp : implementation file
//

#include "stdafx.h"
#include <Tokenizer.h>
#include <IsString.h>
#include "SmartDraw.h"
#include "SmartDrawDocData.h"
#include "NozzleChartPage.h"
#include "SmartDrawDoc.h"
#include "CADInteractive.h"

using namespace PropertyPage;
// CNozzleChartPage dialog

IMPLEMENT_DYNAMIC(CNozzleChartPage, CSmartDrawPropertyPage)

CNozzleChartPage::CNozzleChartPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CNozzleChartPage::IDD, pParent)
{

}

CNozzleChartPage::~CNozzleChartPage()
{
}

void CNozzleChartPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX , IDC_EDIT_ORIGIN_X , m_sOriginX);
	DDX_Text(pDX , IDC_EDIT_ORIGIN_Y , m_sOriginY);
	DDX_Text(pDX , IDC_EDIT_EQP_ROW_HEIGHT , m_sEqpRowHeight);
	DDX_Text(pDX , IDC_EDIT_NOZZLE_ROW_HEIGHT , m_sNozzleRowHeight);
	DDX_Text(pDX , IDC_EDIT_NOZZLE_CHART_ROW_COUNT , m_sRowCount);

	DDX_Control(pDX , IDC_STATIC_NOZZLE_CHART , m_wndNozzleChartPicture);
	DDX_Control(pDX , IDC_BUTTON_ORIGIN_PICK , m_btnOriginPick);
	DDX_Control(pDX , IDC_BUTTON_MOVE_PREV , m_btnMovePrev);
	DDX_Control(pDX , IDC_BUTTON_MOVE_NEXT , m_btnMoveNext);

	DDX_Control(pDX , IDC_COMBO_EQP_TEXT_STYLE , m_cboEqpTextStyle);
	DDX_Control(pDX , IDC_COMBO_EQP_LABEL_LEVEL , m_cboEqpLabelLevel);
	DDX_Control(pDX , IDC_BUTTON_EQP_LABEL_COLOR , m_btnEqpLabelColor);
	DDX_Control(pDX , IDC_COMBO_EQP_LABEL_STYLE , m_cboEqpLabelStyle);

	DDX_Control(pDX , IDC_COMBO_NOZZLE_TEXT_STYLE , m_cboNozzleTextStyle);
	DDX_Control(pDX , IDC_COMBO_NOZZLE_LABEL_LEVEL , m_cboNozzleLabelLevel);
	DDX_Control(pDX , IDC_BUTTON_NOZZLE_LABEL_COLOR , m_btnNozzleLabelColor);
	DDX_Control(pDX , IDC_COMBO_NOZZLE_LABEL_STYLE , m_cboNozzleLabelStyle);

	DDX_Control(pDX , IDC_COMBO_LEADER_LEVEL , m_cboLeaderLevel);
	DDX_Control(pDX , IDC_BUTTON_LEADER_COLOR , m_btnLeaderColor);
	DDX_Control(pDX , IDC_COMBO_LEADER_STYLE , m_cboLeaderStyle);
	DDX_Control(pDX , IDC_COMBO_LEADER_WEIGHT , m_cboLeaderWeight);

	DDX_Control(pDX , IDC_CHECK_SKIP_EQP_HAS_NO_NOZZLE , m_btnSkipEqpHasNoNozzle);	/// 2014.12.31 added by humkyung
}


BEGIN_MESSAGE_MAP(CNozzleChartPage, CSmartDrawPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_PREV , &CNozzleChartPage::OnBnClickedButtonMovePrev)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_NEXT , &CNozzleChartPage::OnBnClickedButtonMoveNext)
	ON_BN_CLICKED(IDC_BUTTON_ORIGIN_PICK, &CNozzleChartPage::OnBnClickedButtonOriginPick)
END_MESSAGE_MAP()


// CNozzleChartPage message handlers

BOOL PropertyPage::CNozzleChartPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();

	m_wndNozzleChartPicture.Load(GetExecPath() + _T("\\Resource\\NozzleChart.bmp"));
	m_wndNozzleChartPicture.Draw();

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_MOUSE_PICK),
			IMAGE_ICON,
			0,
			0,
			0);
		HICON hOldIcon = m_btnOriginPick.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}

	m_cboEqpTextStyle.FillTextStyleList(m_sIniFilePath);
	m_cboNozzleTextStyle.FillTextStyleList(m_sIniFilePath);
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_btnEqpLabelColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_btnNozzleLabelColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_btnLeaderColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	CRect rect;
	GetDlgItem(IDC_STATIC_NOZZLE_CHART_INFO)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.DeflateRect(7 , 32 , 7 , 7);
	BOOL b = m_wndGridCtrl.Create(rect , this , 0x100);
	if(TRUE == b)
	{
		/// 그리드 컨트롤 폰트 코드
		CFont font;
		font.CreateFont(-11,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,_T("Arial")); 
		m_wndGridCtrl.SetFont(&font);
		font.DeleteObject();

		m_wndGridCtrl.SetFixedRowCount(1);
		m_wndGridCtrl.SetFixedColumnCount(1);
		m_wndGridCtrl.SetRowCount(3);
		m_wndGridCtrl.SetColumnCount(7);
		m_wndGridCtrl.SetRowHeight(0 , DEFAULT_GRID_ROW_HEIGHT);
		m_wndGridCtrl.SetRowHeight(1 , DEFAULT_GRID_ROW_HEIGHT);
		m_wndGridCtrl.SetRowHeight(2 , DEFAULT_GRID_ROW_HEIGHT);

		m_wndGridCtrl.SetItemText(1 , 0 , _T("Label"));
		m_wndGridCtrl.SetItemText(2 , 0 , _T("Width"));

		m_wndGridCtrl.SetItemText(0 , 0 , _T("Column"));
		m_wndGridCtrl.SetColumnWidth(0 , 60);

		/// fill it up with stuff
		m_wndGridCtrl.SetEditable(TRUE);
		m_wndGridCtrl.EnableDragAndDrop(FALSE);
		m_wndGridCtrl.SetTextBkColor(RGB(255, 255, 224));
		m_wndGridCtrl.SetRowResize(FALSE);
		m_wndGridCtrl.SetColumnResize(TRUE);
		m_wndGridCtrl.SetListMode(FALSE);
		m_wndGridCtrl.SetSingleRowSelection(TRUE);
		m_wndGridCtrl.SetHeaderSort(FALSE);
		m_wndGridCtrl.SetSingleColSelection(TRUE);
		/////////////////////////////////////////////////////////////////
	}

	/// load contents - 2014.06.23 added by humkyung
	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	m_cboEqpLabelStyle.LoadContents(pDoc->GetLineStyleFilePath());
	m_cboNozzleLabelStyle.LoadContents(pDoc->GetLineStyleFilePath());
	m_cboLeaderStyle.LoadContents(pDoc->GetLineStyleFilePath());

	if(_T("MSTN") != sOutputFormat)
	{
		m_cboEqpLabelLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboNozzleLabelLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboLeaderLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
	}
	m_cboEqpLabelLevel.LoadContents(m_sIniFilePath);
	m_cboNozzleLabelLevel.LoadContents(m_sIniFilePath);
	m_cboLeaderLevel.LoadContents(m_sIniFilePath);
	/// up to here

	LoadData();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-30
    @function   CNozzleChartPage::LoadData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CNozzleChartPage::LoadData()
{
	const CString sApp(_T("Nozzle Chart"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	STRING_T sBorderCfgFilePath(m_sIniFilePath);
	if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		STRING_T sBorderFileName(szBuf);

		int at = sBorderCfgFilePath.rfind(_T("\\"));
		sBorderCfgFilePath = sBorderCfgFilePath.substr(0 , at);
		at = sBorderCfgFilePath.rfind(_T("\\"));
		sBorderCfgFilePath = sBorderCfgFilePath.substr(0 , at);
		sBorderCfgFilePath += _T("\\Border\\") + sBorderFileName + _T(".cfg");
	}else return ERROR_BAD_ENVIRONMENT;
	m_sBorderCfgFilePath = sBorderCfgFilePath.c_str();

	vector<STRING_T> oResult;

	if(GetPrivateProfileString(sApp , _T("Eqp_TextStyle") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		const int at = m_cboEqpTextStyle.FindString(-1 , szBuf);
		m_cboEqpTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("EqpLabel_Level") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		const int at = m_cboEqpLabelLevel.FindString(-1 , szBuf);
		m_cboEqpLabelLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("EqpLabel_Color") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		m_btnEqpLabelColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("EqpLabel_Style") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		const int at = m_cboEqpLabelStyle.FindString(-1 , szBuf);
		m_cboEqpLabelStyle.SetCurSel((-1 != at) ? at : 0);
	}
	
	if(GetPrivateProfileString(sApp , _T("Nozzle_TextStyle") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		const int at = m_cboNozzleTextStyle.FindString(-1 , szBuf);
		m_cboNozzleTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("NozzleLabel_Level") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		const int at = m_cboNozzleLabelLevel.FindString(-1 , szBuf);
		m_cboNozzleLabelLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("NozzleLabel_Color") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		m_btnNozzleLabelColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("NozzleLabel_Style") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		const int at = m_cboNozzleLabelStyle.FindString(-1 , szBuf);
		m_cboNozzleLabelStyle.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp , _T("Leader_Level") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		const int at = m_cboLeaderLevel.FindString(-1 , szBuf);
		m_cboLeaderLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Leader_Color") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		m_btnLeaderColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("Leader_Style") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		const int at = m_cboLeaderStyle.FindString(-1 , szBuf);
		m_cboLeaderStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Leader_Weight") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		const int at = m_cboLeaderWeight.FindString(-1 , szBuf);
		m_cboLeaderWeight.SetCurSel((-1 != at) ? at : 0);
	}

	m_sOriginX = m_sOriginY = _T("0");
	m_sOriginJustify = _T("UpperLeft");
	if(GetPrivateProfileString(sApp , _T("Origin") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		if(3 == oResult.size())
		{
			m_sOriginJustify = oResult[0].c_str();
			m_sOriginX = oResult[1].c_str();
			m_sOriginY = oResult[2].c_str();
		}
	}
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_ORIGIN_1);
		pButton->SetCheck(_T("UpperLeft") == m_sOriginJustify);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ORIGIN_2);
		pButton->SetCheck(_T("UpperRight") == m_sOriginJustify);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ORIGIN_3);
		pButton->SetCheck(_T("LowerLeft") == m_sOriginJustify);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ORIGIN_4);
		pButton->SetCheck(_T("LowerRight") == m_sOriginJustify);
	}

	m_sEqpRowHeight = _T("0");
	if(GetPrivateProfileString(sApp , _T("Eqp_RowHeight") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		m_sEqpRowHeight = szBuf;
	}
	m_sNozzleRowHeight = _T("0");
	if(GetPrivateProfileString(sApp , _T("Nozzle_RowHeight") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		m_sNozzleRowHeight = szBuf;
	}
	m_sRowCount = _T("0");
	if(GetPrivateProfileString(sApp , _T("RowCount") , _T("0") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		m_sRowCount = szBuf;
	}

	if(GetPrivateProfileString(sApp , _T("Column") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		m_wndGridCtrl.SetColumnCount( oResult.size() + 1 );
		for(int i = 0;i < int(oResult.size());++i)
		{
			m_wndGridCtrl.SetItemText(0 , i + 1 , oResult[i].c_str());
			m_wndGridCtrl.SetColumnWidth(i + 1 , 60);
		}
	}
	if(GetPrivateProfileString(sApp , _T("Label") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		for(int i = 0;i < int(oResult.size());++i)
		{
			m_wndGridCtrl.SetItemText(1 , i + 1 , oResult[i].c_str());
		}
	}
	if(GetPrivateProfileString(sApp , _T("Width") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		for(int i = 0;i < int(oResult.size());++i)
		{
			m_wndGridCtrl.SetItemText(2, i + 1 , oResult[i].c_str());
		}
	}

	/// 2014.12.31 added by humkyung
	if(GetPrivateProfileString(sApp , _T("Skip Eqp Has No Nozzle") , _T("False") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		m_btnSkipEqpHasNoNozzle.SetCheck(CString(_T("True")) == CString(szBuf));
	}
	/// up to here

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-30
    @function   CNozzleChartPage::SaveData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CNozzleChartPage::SaveData()
{
	const CString sApp(_T("Nozzle Chart"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	UpdateData();

	vector<STRING_T> oResult;

	CString rString;
	int at = m_cboEqpTextStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboEqpTextStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Eqp_TextStyle") , rString , m_sBorderCfgFilePath);
	}
	at = m_cboEqpLabelLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboEqpLabelLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("EqpLabel_Level") , rString , m_sBorderCfgFilePath);
	}
	at = m_btnEqpLabelColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("EqpLabel_Color") , rString , m_sBorderCfgFilePath);
	at = m_cboEqpLabelStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboEqpLabelStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("EqpLabel_Style") , rString , m_sBorderCfgFilePath);
	}
	
	at = m_cboNozzleTextStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboNozzleTextStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Nozzle_TextStyle") , rString , m_sBorderCfgFilePath);
	}
	at = m_cboNozzleLabelLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboNozzleLabelLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("NozzleLabel_Level") , rString , m_sBorderCfgFilePath);
	}
	at = m_btnNozzleLabelColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("NozzleLabel_Color") , rString , m_sBorderCfgFilePath);
	at = m_cboNozzleLabelStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboNozzleLabelStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("NozzleLabel_Style") , rString , m_sBorderCfgFilePath);
	}

	at = m_cboLeaderLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboLeaderLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Leader_Level") , rString , m_sBorderCfgFilePath);
	}
	at = m_btnLeaderColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("Leader_Color") , rString , m_sBorderCfgFilePath);
	at = m_cboLeaderStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboLeaderStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Leader_Style") , rString , m_sBorderCfgFilePath);
	}
	at = m_cboLeaderWeight.GetCurSel();
	if(-1 != at)
	{
		m_cboLeaderWeight.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Leader_Weight") , rString , m_sBorderCfgFilePath);
	}

	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_ORIGIN_1);
		if(pButton->GetCheck()) m_sOriginJustify = _T("UpperLeft");
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ORIGIN_2);
		if(pButton->GetCheck()) m_sOriginJustify = _T("UpperRight");
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ORIGIN_3);
		if(pButton->GetCheck()) m_sOriginJustify = _T("LowerLeft");
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ORIGIN_4);
		if(pButton->GetCheck()) m_sOriginJustify = _T("LowerRight");
	}
	rString.Format(_T("%s,%s,%s") , m_sOriginJustify , m_sOriginX , m_sOriginY);
	WritePrivateProfileString(sApp , _T("Origin") , rString , m_sBorderCfgFilePath);
	
	WritePrivateProfileString(sApp , _T("Eqp_RowHeight") , m_sEqpRowHeight , m_sBorderCfgFilePath);
	WritePrivateProfileString(sApp , _T("Nozzle_RowHeight") , m_sNozzleRowHeight , m_sBorderCfgFilePath);
	WritePrivateProfileString(sApp , _T("RowCount") , m_sRowCount , m_sBorderCfgFilePath);

	rString.Empty();
	const int iColumnCount = m_wndGridCtrl.GetColumnCount();
	for(int i = 1;i < iColumnCount;++i)
	{
		rString += m_wndGridCtrl.GetItemText(0 , i);
		if(i < iColumnCount - 1) rString += _T(",");
	}
	WritePrivateProfileString(sApp , _T("Column") , rString , m_sBorderCfgFilePath);

	rString.Empty();
	for(int i = 1;i < iColumnCount;++i)
	{
		rString += m_wndGridCtrl.GetItemText(1 , i);
		if(i < iColumnCount - 1) rString += _T(",");
	}
	WritePrivateProfileString(sApp , _T("Label") , rString , m_sBorderCfgFilePath);

	rString.Empty();
	for(int i = 1;i < iColumnCount;++i)
	{
		rString += m_wndGridCtrl.GetItemText(2 , i);
		if(i < iColumnCount - 1) rString += _T(",");
	}
	WritePrivateProfileString(sApp , _T("Width") , rString , m_sBorderCfgFilePath);

	/// 2014.06.23 added by humkyung
	m_cboEqpLabelLevel.SaveContents(m_sIniFilePath);
	m_cboNozzleLabelLevel.SaveContents(m_sIniFilePath);
	m_cboLeaderLevel.SaveContents(m_sIniFilePath);
	/// up to here

	/// 2014.12.31 added by humkyung
	rString = (TRUE == m_btnSkipEqpHasNoNozzle.GetCheck()) ? _T("True") : _T("False");
	WritePrivateProfileString(sApp , _T("Skip Eqp Has No Nozzle") , rString , m_sBorderCfgFilePath);
	/// up to here

	/// write project unit - 2016.02.02 added by humkyung
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	const STRING_T sPrjUnit = pDoc->GetProjectUnit();
	WritePrivateProfileString(sApp , _T("SizeUnit") , sPrjUnit.c_str() , m_sBorderCfgFilePath);
	/// up to here

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   CNozzleChartPage::OnBnClickedButtonMovePrev
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CNozzleChartPage::OnBnClickedButtonMovePrev()
{
	if(m_wndGridCtrl.GetSelectedCount() > 0)
	{
		CCellRange oCellRange = m_wndGridCtrl.GetSelectedCellRange();
		const int iCol = oCellRange.GetMinCol();
		if(iCol > 1)
		{
			CGridCellBase* pCell1 = m_wndGridCtrl.GetCell(0 , iCol);
			CGridCellBase* pCell2 = m_wndGridCtrl.GetCell(0 , iCol - 1);
			CString sText1 = pCell1->GetText();
			CString sText2 = pCell2->GetText();
			swap(sText1 , sText2);
			pCell1->SetText(sText1);
			pCell2->SetText(sText2);

			pCell1 = m_wndGridCtrl.GetCell(1 , iCol);
			pCell2 = m_wndGridCtrl.GetCell(1 , iCol - 1);
			sText1 = pCell1->GetText();
			sText2 = pCell2->GetText();
			swap(sText1 , sText2);
			pCell1->SetText(sText1);
			pCell2->SetText(sText2);

			pCell1 = m_wndGridCtrl.GetCell(2 , iCol);
			pCell2 = m_wndGridCtrl.GetCell(2 , iCol - 1);
			sText1 = pCell1->GetText();
			sText2 = pCell2->GetText();
			swap(sText1 , sText2);
			pCell1->SetText(sText1);
			pCell2->SetText(sText2);

			oCellRange.SetMinCol(iCol - 1);
			oCellRange.SetMaxCol(iCol - 1);
			m_wndGridCtrl.SetSelectedRange(oCellRange);
			m_wndGridCtrl.SetFocusCell(CCellID(1 , iCol - 1));

			m_wndGridCtrl.Invalidate();
		}
	}
}

void PropertyPage::CNozzleChartPage::OnBnClickedButtonMoveNext()
{
	if(m_wndGridCtrl.GetSelectedCount() > 0)
	{
		CCellRange oCellRange = m_wndGridCtrl.GetSelectedCellRange();
		const int iCol = oCellRange.GetMinCol();
		if(iCol < m_wndGridCtrl.GetColumnCount() - 1)
		{
			CGridCellBase* pCell1 = m_wndGridCtrl.GetCell(0 , iCol);
			CGridCellBase* pCell2 = m_wndGridCtrl.GetCell(0 , iCol + 1);
			CString sText1 = pCell1->GetText();
			CString sText2 = pCell2->GetText();
			swap(sText1 , sText2);
			pCell1->SetText(sText1);
			pCell2->SetText(sText2);

			pCell1 = m_wndGridCtrl.GetCell(1 , iCol);
			pCell2 = m_wndGridCtrl.GetCell(1 , iCol + 1);
			sText1 = pCell1->GetText();
			sText2 = pCell2->GetText();
			swap(sText1 , sText2);
			pCell1->SetText(sText1);
			pCell2->SetText(sText2);

			pCell1 = m_wndGridCtrl.GetCell(2 , iCol);
			pCell2 = m_wndGridCtrl.GetCell(2 , iCol + 1);
			sText1 = pCell1->GetText();
			sText2 = pCell2->GetText();
			swap(sText1 , sText2);
			pCell1->SetText(sText1);
			pCell2->SetText(sText2);

			oCellRange.SetMinCol(iCol + 1);
			oCellRange.SetMaxCol(iCol + 1);
			m_wndGridCtrl.SetSelectedRange(oCellRange);
			m_wndGridCtrl.SetFocusCell(CCellID(1 , iCol + 1));

			m_wndGridCtrl.Invalidate();
		}
	}
}

/**
	@brief	get origin point for nozzle chart
	@author	humkyung
	@date	2014.08.25
*/
void PropertyPage::CNozzleChartPage::OnBnClickedButtonOriginPick()
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
		{
			CSmartDrawDoc* pDoc = GetMDIActiveDocument();
			const CString sPrjName = pDoc->GetProjectName();
			const CString rBorderFilePath = docData.GetServerFolderPath() + _T("\\") + sPrjName + _T("\\Border\\") + szBuf;

			GetTempPath(MAX_PATH , szBuf);
			CString rCmdFilePath = szBuf + CString(_T("run.cmd"));
			double x = 0.0 , y = 0.0;
			int res = CCADInteractive::GetInputPoint(x , y , rBorderFilePath.operator LPCTSTR() , rCmdFilePath.operator LPCTSTR());
			if(ERROR_SUCCESS == res)
			{
				SetDlgItemText(IDC_EDIT_ORIGIN_X , IsString::TrimedTrailingZero(x).c_str());
				SetDlgItemText(IDC_EDIT_ORIGIN_Y , IsString::TrimedTrailingZero(y).c_str());
			}
		}
	}
}