// HLRPage.cpp : implementation file
//

#include "stdafx.h"
#include <Tokenizer.h>
#include <gui/GridCtrl/GridCellCheckButton.h>
#include "SmartDraw.h"
#include "SmartDrawDoc.h"
#include "SmartDrawDocData.h"
#include "HLRPage.h"

#include <fstream>
using namespace std;

using namespace PropertyPage;
// CHLRPage dialog
static const int ROW_HEIGHT = 25;
static const STRING_T sKeys[]=
{
	_T("Pipe center line") , _T("Pipe") , _T("Pipe hidden line") , _T("Pipe symbol line") , _T("Pipe symbol hidden line"),
	_T("Structure") , _T("Structure hidden line") , _T("Pipe Misc.") , _T("Pipe Misc. hidden line"),
	_T("Equipment") , _T("Equipment hidden line") ,
	_T("Instrument"), _T("Instrument hidden line"),
	_T("Misc.") , _T("Misc. hidden line") , _T("View line") , _T("Weld Mark")
};

IMPLEMENT_DYNAMIC(CHLRPage, CSmartDrawPropertyPage)

CHLRPage::CHLRPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CHLRPage::IDD, pParent)
{

}

CHLRPage::~CHLRPage()
{
}

void CHLRPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_COMBO_PIPE_SIZE , m_cboPipeSize);
	DDX_Control(pDX , IDC_CHECK_INSULATION_LEVEL_DISPLAY , m_btnInsulationCheck);	/// 2012.10.17 added by humkyung
}


BEGIN_MESSAGE_MAP(CHLRPage, CSmartDrawPropertyPage)
	ON_BN_CLICKED(IDC_RADIO_EQUAL_OD, &PropertyPage::CHLRPage::OnRadioClickedButtonEqualOD)
	ON_BN_CLICKED(IDC_RADIO_FIXED_SIZE, &PropertyPage::CHLRPage::OnRadioClickedButtonFixedSize)
END_MESSAGE_MAP()


// CHLRPage message handlers

BOOL PropertyPage::CHLRPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	list<CSmartDrawDocData::SizeRecord>* pSizeRecordList = docData.GetSizeRecordList();
	for(list<CSmartDrawDocData::SizeRecord>::iterator itr = pSizeRecordList->begin();itr != pSizeRecordList->end();++itr)
	{
		CString rString(itr->inch.c_str() + CString(_T("-")) + itr->metric.c_str() + CString(_T("mm")));
		m_cboPipeSize.AddString(rString);
	}

	m_btnInsulationCheck.SetLabels(_T("ON") , _T("OFF"));

	CRect rect;
	GetDlgItem(IDC_STATIC_LINE_PROPERTY)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.DeflateRect(7 , 20 , 7 , 7);
	BOOL b = m_wndGridCtrl.Create(rect , this , 0x100);
	if(TRUE == b)
	{
		///그리드 컨트롤 폰트 코드
		CFont font;
		font.CreateFont(-11,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,_T("Arial")); 
		m_wndGridCtrl.SetFont(&font);
		font.DeleteObject();

		m_wndGridCtrl.SetFixedRowCount(1);
		m_wndGridCtrl.SetFixedColumnCount(1);
		m_wndGridCtrl.SetColumnCount(6);
		m_wndGridCtrl.SetRowCount(SIZE_OF_ARRAY(sKeys) + 1);
		for(int i = 0;i < m_wndGridCtrl.GetRowCount();++i)
		{
			m_wndGridCtrl.SetRowHeight(i , ROW_HEIGHT);
			if(0 == i)
			{
				m_wndGridCtrl.SetItemText(0 , 0 , _T("Item"));
			}
			else
			{
				m_wndGridCtrl.SetItemText(i , 0 , sKeys[i - 1].c_str());
			}
		}

		m_wndGridCtrl.SetItemText(0 , 1 , _T("Level"));
		m_wndGridCtrl.SetColumnWidth(1 , 100);
		m_wndGridCtrl.SetItemText(0 , 2 , _T("Color"));
		m_wndGridCtrl.SetColumnWidth(2 , 100);
		m_wndGridCtrl.SetItemText(0 , 3 , _T("Style"));
		m_wndGridCtrl.SetColumnWidth(3 , 100);
		m_wndGridCtrl.SetItemText(0 , 4 , _T("Weight"));
		m_wndGridCtrl.SetColumnWidth(4 , 50);
		m_wndGridCtrl.SetItemText(0 , 5 , _T("ON/OFF"));
		m_wndGridCtrl.SetColumnWidth(5 , 50);

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
		m_wndGridCtrl.AutoSizeColumn(0);
		/////////////////////////////////////////////////////////////////
	}
	
	m_tooltip.Create(this);
	m_tooltip.AddTool(GetDlgItem(IDC_EDIT_FIXED_SIZE), _T("Pipe Size in mm"));
	m_tooltip.SetNotify();

	LoadData();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL PropertyPage::CHLRPage::PreTranslateMessage(MSG* pMsg)
{
	m_tooltip.RelayEvent(pMsg); 

	return CSmartDrawPropertyPage::PreTranslateMessage(pMsg);
}

/******************************************************************************
    @brief		load line style
	@author     humkyung
    @date       2014-06-23
    @function   LoadLineStyle
    @return     int
******************************************************************************/
int PropertyPage::CHLRPage::LoadLineStyle(CStringArray& oStrings , const CString& sLineStyleFilePath)
{
	oStrings.RemoveAll();
	
	IFSTREAM_T ifile(sLineStyleFilePath.operator LPCTSTR());
	if(ifile.is_open())
	{
		vector<STRING_T> oResult;
		STRING_T aLine;
		while(!ifile.eof())
		{
			getline(ifile , aLine);
			if(ifile.eof()) break;
			if(aLine.empty() || (0 == aLine.find(_T(";;")))) continue;	/// skip comment
			if('*' == aLine[0])
			{
				CTokenizer<CIsComma>::Tokenize(oResult , aLine , CIsComma());
				if(2 == oResult.size())
				{
					oStrings.Add(oResult[0].substr(1).c_str());
				}
			}
		}
	}
	
	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		load line weight
	@author     humkyung
    @date       2014-06-24
    @function   LoadLineWeight
    @return     int
******************************************************************************/
int PropertyPage::CHLRPage::LoadLineWeight(CStringArray& oStrings , const CString& sLineWeightFilePath)
{
	oStrings.RemoveAll();

	IFSTREAM_T ifile(sLineWeightFilePath.operator LPCTSTR());
	if(ifile.is_open())
	{
		vector<STRING_T> oResult;
		STRING_T aLine;
		while(!ifile.eof())
		{
			getline(ifile , aLine);
			if(ifile.eof()) break;
			if(aLine.empty()) continue;

			oStrings.Add(aLine.c_str());
		}
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   CHLRPage::LoadData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CHLRPage::LoadData()
{
	static const STRING_T sApp(_T("HLR_OPTIONS"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	if(GetPrivateProfileString(sApp.c_str() , _T("DisplayStyle") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL_SYMBOL_LINE);
		pButton->SetCheck(STRING_T(_T("All symbol line")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL_AS_IT_WAS);
		pButton->SetCheck(STRING_T(_T("All as it was")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_DEPEND_ON_SIZE);
		pButton->SetCheck(STRING_T(_T("Depend on size")) == szBuf);
	}
	
	/// 2014.08.25 added by humkyung
	GetPrivateProfileString(sApp.c_str() , _T("InsulationLevel") , _T("") , szBuf , MAX_PATH , m_sIniFilePath);
	SetDlgItemText(IDC_EDIT_INSULATION_LEVEL , szBuf);
	GetPrivateProfileString(sApp.c_str() , _T("InsulationDisplay") , _T("Display"), szBuf , MAX_PATH , m_sIniFilePath);
	(_T("Remove") == CString(szBuf)) ? m_btnInsulationCheck.SetCheck(FALSE) : m_btnInsulationCheck.SetCheck(TRUE);
	/// up to here

	if(GetPrivateProfileString(sApp.c_str() , _T("Import hidden level") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_IMPORT_HIDDEN_LEVEL , szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("CriticalSize") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboPipeSize.FindString(-1 , szBuf);
		m_cboPipeSize.SetCurSel((-1 != at) ? at : 0);
	}

	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	CStringArray aLineStyle , aLineWeight;
	LoadLineStyle(aLineStyle , pDoc->GetLineStyleFilePath());
	LoadLineWeight(aLineWeight , pDoc->GetLineWeightFilePath());

	vector<STRING_T> oResult;
	for(int i = 0;i < SIZE_OF_ARRAY(sKeys);++i)
	{
		BOOL bCheck(FALSE);
		if(GetPrivateProfileString(sApp.c_str() , sKeys[i].c_str() , _T("1,1,0,0,False") , szBuf , MAX_PATH , m_sIniFilePath))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(oResult.size() >= 4)
			{
				m_wndGridCtrl.SetItemText(i + 1 , 1 , oResult[0].c_str());
				m_wndGridCtrl.SetItemText(i + 1 , 2 , oResult[1].c_str());
				{
					m_wndGridCtrl.SetCellType(i + 1 , 2 , RUNTIME_CLASS(CTextColorGridCell));
					CTextColorGridCell *pCell = (CTextColorGridCell*) m_wndGridCtrl.GetCell(i + 1 , 2);
					if(pCell)
					{
						pCell->EnableAutomaticButton();
						pCell->m_rIniFilePath = m_sIniFilePath;
						pCell->FillColors(m_sIniFilePath);
					}
				}
				m_wndGridCtrl.SetItemText(i + 1 , 3 , oResult[2].c_str());
				{
					m_wndGridCtrl.SetCellType(i + 1 , 3 , RUNTIME_CLASS(CGridCellCombo));
					CGridCellCombo *pCell = (CGridCellCombo*) m_wndGridCtrl.GetCell(i + 1 , 3);
					pCell->SetStyle(CBS_DROPDOWNLIST);
					pCell->SetOptions(aLineStyle);
				}
				m_wndGridCtrl.SetItemText(i + 1 , 4 , oResult[3].c_str());
				{
					m_wndGridCtrl.SetCellType(i + 1 , 4 , RUNTIME_CLASS(CGridCellCombo));
					CGridCellCombo *pCell = (CGridCellCombo*) m_wndGridCtrl.GetCell(i + 1 , 4);
					pCell->SetStyle(CBS_DROPDOWNLIST);
					pCell->SetOptions(aLineWeight);
				}

				/// 2014.06.05 added by humkyung
				m_wndGridCtrl.SetCellType(i + 1 , 5 , RUNTIME_CLASS(CGridCellCheckButton));
				CGridCellCheckButton* pCellCheckButton = (CGridCellCheckButton*)(m_wndGridCtrl.GetCell(i + 1 , 5));
				pCellCheckButton->SetCheck((4 == oResult.size()) || ((5 == oResult.size()) && (_T("True") == oResult[4])));
				/// up to here
				bCheck = pCellCheckButton->GetCheck();
			}
		}

		for(int col = 1;col < m_wndGridCtrl.GetColumnCount() - 1;++col)
		{
			if(FALSE == bCheck)
			{
				m_wndGridCtrl.SetItemState(i+1 , col , m_wndGridCtrl.GetItemState(i+1,col) | GVIS_READONLY);
			}
			else
			{
				m_wndGridCtrl.SetItemState(i+1 , col , m_wndGridCtrl.GetItemState(i+1,col) & ~GVIS_READONLY);
			}
		}
	}

	/// read weld mark size - 2014.11.15 added by humkyung
	if(GetPrivateProfileString(sApp.c_str() , _T("Weld Mark Size") , _T("1") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_WELD_MARK_SIZE , szBuf);
	}
	/// up to here

	/// read symbol line property - 2015.08.30 added by humkyung
	GetPrivateProfileString(sApp.c_str() , _T("Symbolline Display") , _T("EQUAL_OD") , szBuf , MAX_PATH , m_sIniFilePath);
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_EQUAL_OD);
		pButton->SetCheck(CString(_T("EQUAL_OD")) == szBuf);
		pButton = (CButton*)GetDlgItem(IDC_RADIO_FIXED_SIZE);
		pButton->SetCheck(CString(_T("FIXED_SIZE")) == szBuf);

		GetDlgItem(IDC_EDIT_FIXED_SIZE)->EnableWindow(CString(_T("FIXED_SIZE")) == szBuf);
	}
	const int iFixedSize = GetPrivateProfileInt(sApp.c_str() , _T("Fixed Size") , 0 , m_sIniFilePath);
	{
		CString str;
		str.Format(_T("%d") , iFixedSize);
		SetDlgItemText(IDC_EDIT_FIXED_SIZE , str);
	}
	/// up to here

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   CHLRPage::SaveData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CHLRPage::SaveData()
{
	static const STRING_T sApp(_T("HLR_OPTIONS"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CString rString;
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL_SYMBOL_LINE);
		if(pButton->GetCheck()) rString = _T("All symbol line");
		pButton = (CButton*)GetDlgItem(IDC_RADIO_ALL_AS_IT_WAS);
		if(pButton->GetCheck()) rString = _T("All as it was");
		pButton = (CButton*)GetDlgItem(IDC_RADIO_DEPEND_ON_SIZE);
		if(pButton->GetCheck()) rString = _T("Depend on size");
	}
	WritePrivateProfileString(sApp.c_str() , _T("DisplayStyle") , rString , m_sIniFilePath);
	
	/// 2014.08.25 added by humkyung
	GetDlgItemText(IDC_EDIT_INSULATION_LEVEL , rString);
	WritePrivateProfileString(sApp.c_str() , _T("InsulationLevel") , rString , m_sIniFilePath);
	WritePrivateProfileString(sApp.c_str() , _T("InsulationDisplay") , (TRUE == m_btnInsulationCheck.GetCheck()) ? _T("Display") : _T("Remove") , m_sIniFilePath);
	/// up to here

	GetDlgItemText(IDC_EDIT_IMPORT_HIDDEN_LEVEL , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Import hidden level") , rString , m_sIniFilePath);

	int at = m_cboPipeSize.GetCurSel();
	if(-1 != at)
	{
		m_cboPipeSize.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("CriticalSize") , rString , m_sIniFilePath);
	}

	const int iCount = m_wndGridCtrl.GetRowCount();
	for(int i = 1;i < iCount;++i)
	{
		rString.Empty();

		rString += m_wndGridCtrl.GetItemText(i , 1) + _T(",");
		
		CTextColorGridCell* pCell = (CTextColorGridCell*)(m_wndGridCtrl.GetCell(i , 2));
		rString += pCell->GetColorIndex() + _T(",");
		
		rString += m_wndGridCtrl.GetItemText(i , 3) + _T(",");
		rString += m_wndGridCtrl.GetItemText(i , 4) + _T(",");
		rString += (TRUE == ((CGridCellCheckButton*)m_wndGridCtrl.GetCell(i , 5))->GetCheck()) ? _T("True") : _T("False");

		WritePrivateProfileString(sApp.c_str() , m_wndGridCtrl.GetItemText(i , 0) , rString , m_sIniFilePath);
	}

	/// save weld mark size - 2014.11.15 added by humkyung
	GetDlgItemText(IDC_EDIT_WELD_MARK_SIZE , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Weld Mark Size") , rString , m_sIniFilePath);
	/// up to here

	/// save symbolline property - 2015.08.30 added by humkyung
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_EQUAL_OD);
		if(pButton->GetCheck()) rString = _T("EQUAL_OD");
		pButton = (CButton*)GetDlgItem(IDC_RADIO_FIXED_SIZE);
		if(pButton->GetCheck()) rString = _T("FIXED_SIZE");
	}
	WritePrivateProfileString(sApp.c_str() , _T("Symbolline Display") , rString , m_sIniFilePath);

	GetDlgItemText(IDC_EDIT_FIXED_SIZE , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Fixed Size") , rString , m_sIniFilePath);
	/// up to here

	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		set enable when button set checked, otherwise set disable
	@author     humkyung
    @date       2015-01-07
    @function   CHLRPage::OnNotify
    @return     BOOL
******************************************************************************/
BOOL PropertyPage::CHLRPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    if(wParam == m_wndGridCtrl.GetDlgCtrlID())
	{
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);

		if((NM_CLICK == pGridView->hdr.code) && (5 == pGridView->iColumn))
		{
			CGridCellCheckButton* pCell = (CGridCellCheckButton*)m_wndGridCtrl.GetCell(pGridView->iRow, pGridView->iColumn);
			for(int col = 1;col < m_wndGridCtrl.GetColumnCount() - 1;++col)
			{
				if(FALSE == pCell->GetCheck())
				{
					m_wndGridCtrl.SetItemState(pGridView->iRow , col , m_wndGridCtrl.GetItemState(pGridView->iRow,col) | GVIS_READONLY);
				}
				else
				{
					m_wndGridCtrl.SetItemState(pGridView->iRow , col , m_wndGridCtrl.GetItemState(pGridView->iRow,col) & ~GVIS_READONLY);
				}
			}

			m_wndGridCtrl.RedrawRow(pGridView->iRow);
		}
	}

	return CSmartDrawPropertyPage::OnNotify(wParam , lParam , pResult);
}

void PropertyPage::CHLRPage::OnRadioClickedButtonEqualOD()
{
	GetDlgItem(IDC_EDIT_FIXED_SIZE)->EnableWindow(FALSE);
}

void PropertyPage::CHLRPage::OnRadioClickedButtonFixedSize()
{
	GetDlgItem(IDC_EDIT_FIXED_SIZE)->EnableWindow(TRUE);
}