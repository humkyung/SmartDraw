// RevisionChartPage.cpp : implementation file
//

#include "stdafx.h"
#include <Tokenizer.h>
#include <IsString.h>
#include "SmartDraw.h"
#include "SmartDrawDocData.h"
#include "RevisionChartPage.h"
#include "SmartDrawDoc.h"
#include "CADInteractive.h"
#include "StringTable.h"

using namespace PropertyPage;
// CRevisionChartPage dialog

IMPLEMENT_DYNAMIC(CRevisionChartPage, CSmartDrawPropertyPage)

CRevisionChartPage::CRevisionChartPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CRevisionChartPage::IDD, pParent)
{

}

CRevisionChartPage::~CRevisionChartPage()
{
}

void CRevisionChartPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Text(pDX , IDC_EDIT_ORIGIN_X , m_sOriginX);
	DDX_Text(pDX , IDC_EDIT_ORIGIN_Y , m_sOriginY);
	DDX_Text(pDX , IDC_EDIT_REV_CHART_ROW_COUNT , m_sRowCount);
	DDX_Text(pDX , IDC_EDIT_REV_CHART_ROW_HEIGHT, m_sRowHeight);

	DDX_Control(pDX , IDC_STATIC_REV_CHART , m_wndRevChartPicture);
	DDX_Control(pDX , IDC_BUTTON_ORIGIN_PICK , m_btnOriginPick);

	DDX_Control(pDX , IDC_COMBO_TEXT_STYLE , m_cboTextStyle);
	DDX_Control(pDX , IDC_COMBO_LABEL_LEVEL , m_cboLabelLevel);
	DDX_Control(pDX , IDC_BUTTON_LABEL_COLOR , m_btnLabelColor);
	DDX_Control(pDX , IDC_COMBO_LABEL_STYLE , m_cboLabelStyle);

	DDX_Control(pDX , IDC_COMBO_LEADER_LEVEL , m_cboLeaderLevel);
	DDX_Control(pDX , IDC_BUTTON_LEADER_COLOR , m_btnLeaderColor);
	DDX_Control(pDX , IDC_COMBO_LEADER_STYLE , m_cboLeaderStyle);
	DDX_Control(pDX , IDC_COMBO_LEADER_WEIGHT , m_cboLeaderWeight);

	DDX_Control(pDX , IDC_EDIT_REV_CHART_ROW_HEIGHT , m_wndRevChartRowHeight);	/// 2014.11.15 added by humkyung
}


BEGIN_MESSAGE_MAP(CRevisionChartPage, CSmartDrawPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_PREV , &CRevisionChartPage::OnBnClickedButtonMovePrev)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_NEXT , &CRevisionChartPage::OnBnClickedButtonMoveNext)
	ON_BN_CLICKED(IDC_BUTTON_ORIGIN_PICK, &CRevisionChartPage::OnBnClickedButtonOriginPick)
END_MESSAGE_MAP()


// CRevisionChartPage message handlers

BOOL PropertyPage::CRevisionChartPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();

	m_wndRevChartPicture.Load(GetExecPath() + _T("\\Resource\\RevisionChart.bmp"));
	m_wndRevChartPicture.Draw();

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

	m_cboTextStyle.FillTextStyleList(m_sIniFilePath);
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_btnLabelColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_btnLeaderColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	CRect rect;
	GetDlgItem(IDC_STATIC_REV_CHART_INFO)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.DeflateRect(7 , 10 , 7 , 7);
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
		m_wndGridCtrl.SetColumnCount(sizeof(CRevisionChart::COLUMNS)/sizeof(STRING_T) + 1);
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

	/// fill level data - 2014.06.23 added by humkyung
	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	m_cboLabelStyle.LoadContents(pDoc->GetLineStyleFilePath());
	m_cboLeaderStyle.LoadContents(pDoc->GetLineStyleFilePath());

	if(_T("MSTN") != sOutputFormat)
	{
		m_cboLabelLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboLeaderLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
	}
	m_cboLabelLevel.LoadContents(m_sIniFilePath);
	m_cboLeaderLevel.LoadContents(m_sIniFilePath);
	/// up to here

	CStringTable& table = CStringTable::GetInstance();
	const STRING_T str = table.GetString(_T("IDS_INVALID_STRING"));
	m_wndRevChartRowHeight.CreateToolTip(this , str.c_str());

	LoadData();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   CRevisionChartPage::LoadData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CRevisionChartPage::LoadData()
{
	const CString sApp(_T("Revision Chart"));
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
	}else	return ERROR_BAD_ENVIRONMENT;
	m_sBorderCfgFilePath = sBorderCfgFilePath.c_str();

	vector<STRING_T> oResult;

	if(GetPrivateProfileString(sApp , _T("TextStyle") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		const int at = m_cboTextStyle.FindString(-1 , szBuf);
		m_cboTextStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Label_Level") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		const int at = m_cboLabelLevel.FindString(-1 , szBuf);
		m_cboLabelLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp , _T("Label_Color") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		m_btnLabelColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp , _T("Label_Style") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		const int at = m_cboLabelStyle.FindString(-1 , szBuf);
		m_cboLabelStyle.SetCurSel((-1 != at) ? at : 0);
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

	if(GetPrivateProfileString(sApp , _T("DisplayStyle") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_DISPLAY_STYLE1);
		pButton->SetCheck(STRING_T(_T("Only Data")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_DISPLAY_STYLE2);
		pButton->SetCheck(STRING_T(_T("Data+Label")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_DISPLAY_STYLE3);
		pButton->SetCheck(STRING_T(_T("Data+Label+Line")) == szBuf);
	}

	if(GetPrivateProfileString(sApp , _T("RowCount") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		m_sRowCount = szBuf;
	}
	if(GetPrivateProfileString(sApp , _T("RowHeight") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		m_sRowHeight = szBuf;
	}

	if(GetPrivateProfileString(sApp , _T("Label") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		m_wndGridCtrl.SetColumnCount( oResult.size() + 1 );
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
			m_wndGridCtrl.SetItemText(2 , i + 1 , oResult[i].c_str());
		}
	}

	if(GetPrivateProfileString(sApp , _T("Column") , _T("") , szBuf , MAX_PATH , m_sBorderCfgFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		for(int col = 1;col < int(oResult.size()) + 1;++col)
		{
			m_wndGridCtrl.SetItemText(0 , col , oResult[col - 1].c_str());
			m_wndGridCtrl.SetColumnWidth(col , 70);
		}
	}
	else
	{
		for(int col = 1;col < m_wndGridCtrl.GetColumnCount();++col)
		{
			m_wndGridCtrl.SetItemText(0 , col , CRevisionChart::COLUMNS[col - 1].c_str());
			m_wndGridCtrl.SetColumnWidth(col , 70);
		}
	}
		
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   CRevisionChartPage::SaveData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CRevisionChartPage::SaveData()
{
	const CString sApp(_T("Revision Chart"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	UpdateData();

	CString rString;
	int at = m_cboTextStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboTextStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("TextStyle") , rString , m_sBorderCfgFilePath);
	}
	at = m_cboLabelLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboLabelLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Label_Level") , rString , m_sBorderCfgFilePath);
	}
	at = m_btnLabelColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp , _T("Label_Color") , rString , m_sBorderCfgFilePath);
	at = m_cboLabelStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboLabelStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp , _T("Label_Style") , rString , m_sBorderCfgFilePath);
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

	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_DISPLAY_STYLE1);
		if(pButton->GetCheck()) rString = _T("Only Data");
		pButton = (CButton*)GetDlgItem(IDC_RADIO_DISPLAY_STYLE2);
		if(pButton->GetCheck()) rString = _T("Data+Label");
		pButton = (CButton*)GetDlgItem(IDC_RADIO_DISPLAY_STYLE3);
		if(pButton->GetCheck()) rString = _T("Data+Label+Line");
	}
	WritePrivateProfileString(sApp , _T("DisplayStyle") , rString , m_sBorderCfgFilePath);
	
	WritePrivateProfileString(sApp , _T("RowCount") , m_sRowCount , m_sBorderCfgFilePath);
	WritePrivateProfileString(sApp , _T("RowHeight") , m_sRowHeight , m_sBorderCfgFilePath);

	rString.Empty();
	for(int i = 1;i < m_wndGridCtrl.GetColumnCount();++i)
	{
		rString += m_wndGridCtrl.GetItemText(1 , i);
		if(i < m_wndGridCtrl.GetColumnCount() - 1) rString += _T(",");
	}
	WritePrivateProfileString(sApp , _T("Label") , rString , m_sBorderCfgFilePath);
	
	rString.Empty();
	for(int i = 1;i < m_wndGridCtrl.GetColumnCount();++i)
	{
		rString += m_wndGridCtrl.GetItemText(2 , i);
		if(i < m_wndGridCtrl.GetColumnCount() - 1) rString += _T(",");
	}
	WritePrivateProfileString(sApp , _T("Width") , rString , m_sBorderCfgFilePath);

	rString.Empty();
	for(int i = 1;i < m_wndGridCtrl.GetColumnCount();++i)
	{
		rString += m_wndGridCtrl.GetItemText(0 , i);
		if(i < m_wndGridCtrl.GetColumnCount() - 1) rString += _T(",");
	}
	WritePrivateProfileString(sApp , _T("Column") , rString , m_sBorderCfgFilePath);

	/// 2014.06.23 added by humkyung
	m_cboLabelLevel.SaveContents(m_sIniFilePath);
	m_cboLeaderLevel.SaveContents(m_sIniFilePath);
	/// up to here

	return ERROR_SUCCESS;
}

/**
	@brief	get origin point for revision chart
	@author	humkyung
	@date	2014.08.25
*/
void PropertyPage::CRevisionChartPage::OnBnClickedButtonOriginPick()
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

/******************************************************************************
    @brief		move selected columns to previous
	@author     humkyung
    @date       2014-11-17
    @function   CRevisionChartPage::OnBnClickedButtonMovePrev
    @return     void
******************************************************************************/
void PropertyPage::CRevisionChartPage::OnBnClickedButtonMovePrev()
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

/******************************************************************************
    @brief		move selected columns to next
	@author     humkyung
    @date       2014-11-17
    @function   CRevisionChartPage::OnBnClickedButtonMoveNext
    @return     void
******************************************************************************/
void PropertyPage::CRevisionChartPage::OnBnClickedButtonMoveNext()
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