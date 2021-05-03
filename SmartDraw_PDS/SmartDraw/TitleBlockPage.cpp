// TitleBlockPage.cpp : implementation file
//

#include "stdafx.h"
#include <IsString.h>
#include <Tokenizer.h>
#include <gui/GridCtrl/GridCellCombo.h>
#include "SmartDraw.h"
#include "SmartDrawDoc.h"
#include "SmartDrawDocData.h"
#include "SmartDrawPropertySheet.h"
#include "TitleBlockPage.h"
#include "CADInteractive.h"

#include <fstream>
using namespace std;

using namespace PropertyPage;
// CTitleBlockPage dialog
static const int ROW_HEIGHT = 25;

IMPLEMENT_DYNCREATE(CPickButton , CMFCButton)
BEGIN_MESSAGE_MAP(CPickButton, CMFCButton)
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
END_MESSAGE_MAP()

CPickButton::CPickButton()
{
	m_pCell = NULL;
}

/**
	@brief	OnCreate
	@author HumKyung.BAEK	
	@date	2010.07.03
	@return		
**/
int CPickButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_MOUSE_PICK),IMAGE_ICON,0,0,0);
	HICON hOldIcon = SetIcon(hIcon);
	if(hOldIcon) DestroyIcon(hOldIcon);

	return ERROR_SUCCESS;
}

/**
	@brief	LBUTTON UP
	@author HumKyung.BAEK	
	@date	2010.05.12
	@return		
**/
void CPickButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		CString rMSTNJFilePath = docData.GetMSTNJFilePath();
		CSmartDrawPropertyPage* pParent = (CTitleBlockPage*)(GetParent()->GetParent());
		const CString sIniFilePath = pParent->GetIniFilePath();
		if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , _T("") , szBuf , MAX_PATH , sIniFilePath))
		{
			CSmartDrawDoc* pDoc = GetMDIActiveDocument();
			const CString sPrjName = pDoc->GetProjectName();

			CString rString;
			GetDlgItemText(IDC_EDIT_BORDER_FILE_NAME,rString);
			const CString rBorderFilePath = docData.GetServerFolderPath() + _T("\\") + sPrjName + _T("\\Border\\") + szBuf;

			GetTempPath(MAX_PATH , szBuf);
			CString rCmdFilePath = szBuf + CString(_T("run.cmd"));
			double x = 0.0 , y = 0.0;
			int res = CCADInteractive::GetInputPoint(x , y , rBorderFilePath.operator LPCTSTR() , rCmdFilePath.operator LPCTSTR());
			if((ERROR_SUCCESS == res) && (NULL != m_pCell))
			{
				STRING_T sValue = IsString::TrimedTrailingZero(x) + _T(",") + IsString::TrimedTrailingZero(y);
				m_pCell->SetText(sValue.c_str());
				m_pCell->GetGrid()->RedrawCell(((CPickButtonGridCell*)m_pCell)->m_iRow,((CPickButtonGridCell*)m_pCell)->m_iCol);
			}
		}
	}

	CMFCButton::OnLButtonUp(nFlags, point);
}

IMPLEMENT_DYNCREATE(CPickButtonGridCell, CGridCell)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPickButtonGridCell::CPickButtonGridCell() : CGridCell()
{
	m_iRow = m_iCol = -1;
	m_wndPickButton.m_pCell = this;
}

CPickButtonGridCell::~CPickButtonGridCell()
{

}

BOOL CPickButtonGridCell::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd)
{
	if(m_wndPickButton.GetSafeHwnd())
	{
		CRect rectPickButton(rect);
		rectPickButton.left = rectPickButton.right - rectPickButton.Height();
		m_wndPickButton.MoveWindow(&rectPickButton);
	}
	else
	{
		m_iRow = nRow;
		m_iCol = nCol;

		CRect rectPickButton(rect);
		rectPickButton.left = rectPickButton.right - rectPickButton.Height();

		CGridCtrl* pGrid = GetGrid();
		m_wndPickButton.Create(_T("...") , WS_CHILD | WS_VISIBLE , rectPickButton , (CWnd*)pGrid , 0x10);
	}

	rect.right -= rect.Height();
	return CGridCell::Draw(pDC , nRow , nCol , rect , bEraseBkgnd);
}

CString CTextStyleGridCell::m_rIniFilePath;
IMPLEMENT_DYNCREATE(CTextStyleGridCell, CGridCell)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextStyleGridCell::CTextStyleGridCell() : CGridCell()
{
}

CTextStyleGridCell::~CTextStyleGridCell()
{

}

BOOL CTextStyleGridCell::Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar)
{
	int i=0;
	m_bEditing = TRUE;
	if(NULL == m_pEditWnd)
	{
		if(NULL == m_wndTextStyleComboBox.GetSafeHwnd())
		{
			int nHeight = rect.Height();
			rect.bottom = rect.bottom + 5*nHeight + ::GetSystemMetrics(SM_CYHSCROLL);

			BOOL b = m_wndTextStyleComboBox.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST |  CBS_OWNERDRAWFIXED | CBS_HASSTRINGS ,rect , (CWnd*)GetGrid() , 101);
			if(TRUE == b)
			{
				m_wndTextStyleComboBox.MoveWindow(rect.left, rect.top, rect.Width() , rect.Height() + 5*nHeight + ::GetSystemMetrics(SM_CYHSCROLL));
				m_wndTextStyleComboBox.SetFont(GetGrid()->GetFont());
				m_wndTextStyleComboBox.FillTextStyleList(CTextStyleGridCell::m_rIniFilePath);

				CString rCellText = GetText();
				if(FALSE == rCellText.IsEmpty())
				{
					const int nFound = m_wndTextStyleComboBox.FindString(-1 , rCellText);
					if(-1 != nFound) m_wndTextStyleComboBox.SetCurSel(nFound);
				}
				else
				{
					m_wndTextStyleComboBox.SetCurSel(0);
				}
			}

			m_pEditWnd = &m_wndTextStyleComboBox;
			m_pEditWnd->SetFocus();
		}
		else
		{
			m_wndTextStyleComboBox.FillTextStyleList(CTextStyleGridCell::m_rIniFilePath);

			CString rCellText = GetText();
			if(FALSE == rCellText.IsEmpty())
			{
				const int nFound = m_wndTextStyleComboBox.FindString(-1 , rCellText);
				if(-1 != nFound) m_wndTextStyleComboBox.SetCurSel(nFound);
			}
			else
			{
				m_wndTextStyleComboBox.SetCurSel(0);
			}

			m_pEditWnd = &m_wndTextStyleComboBox;
			m_pEditWnd->SetFocus();
		}

	}
	else
	{
		CRect wndRect;
		m_pEditWnd->GetWindowRect(&wndRect);
		int height = wndRect.bottom - wndRect.top;
		rect.bottom = rect.top + height;
		m_pEditWnd->MoveWindow(rect);
		m_pEditWnd->ShowWindow(SW_NORMAL);
		m_pEditWnd->SetFocus();
	}

	return TRUE;
}

void CTextStyleGridCell::EndEdit()
{
	if (m_pEditWnd)
	{
		CTextStyleComboBox* pComboBox = ((CTextStyleComboBox*)m_pEditWnd);
		CString rText;
		const int nIndex = pComboBox->GetCurSel();
		if(-1 != nIndex )
		{
			pComboBox->GetLBText(nIndex , rText);
			SetText(rText);

			m_pEditWnd->ShowWindow(SW_HIDE);
			m_pEditWnd = NULL;
		}
		else
		{
			SetText(_T("Standard"));
			m_pEditWnd->ShowWindow(SW_HIDE);
			m_pEditWnd = NULL;
		}
	}

	m_bEditing = FALSE;
}

IMPLEMENT_DYNCREATE(CLevelGridCell, CGridCell)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLevelGridCell::CLevelGridCell() : CGridCell()
{
	int d = 1;
}

CLevelGridCell::~CLevelGridCell()
{

}

BOOL CLevelGridCell::Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar)
{
	int i=0;
	m_bEditing = TRUE;
	if(NULL == m_pEditWnd)
	{
		if(NULL == m_cboLevel.GetSafeHwnd())
		{
			const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST;/// |  CBS_OWNERDRAWFIXED | CBS_HASSTRINGS; 
			rect.bottom += 320;
			BOOL b = m_cboLevel.Create(dwStyle , rect , (CWnd*)GetGrid() , nID);
			if(TRUE == b)
			{
				m_cboLevel.SetFont(GetGrid()->GetFont());
				CString rString;
				for(int i = 1;i <= 63;++i)
				{
					rString.Format(_T("%d") , i);
					m_cboLevel.InsertString(i - 1 , rString);
				}
			}
		}

		CString rCellText = GetText();
		const int at = m_cboLevel.FindString(-1 , rCellText);
		m_cboLevel.SetCurSel((-1 != at) ? at : 0);

		m_pEditWnd = &m_cboLevel;
		m_pEditWnd->SetFocus();
	}
	else
	{
		CRect wndRect;
		m_pEditWnd->GetWindowRect(&wndRect);
		int height = wndRect.bottom - wndRect.top;
		rect.bottom = rect.top + height;
		m_pEditWnd->MoveWindow(rect);
		m_pEditWnd->ShowWindow(SW_NORMAL);
		m_pEditWnd->SetFocus();
	}

	return TRUE;
}

void CLevelGridCell::EndEdit()
{
	if (m_pEditWnd)
	{
		CString rString;
		const int at = m_cboLevel.GetCurSel();
		if(-1 != at)
		{
			m_cboLevel.GetLBText(at , rString);
			SetText(rString);
		}

		m_pEditWnd->ShowWindow(SW_HIDE);
		m_pEditWnd = NULL;
	}

	m_bEditing = FALSE;
}

CString CTextColorGridCell::m_rIniFilePath;
IMPLEMENT_DYNCREATE(CTextColorGridCell, CGridCell)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextColorGridCell::CTextColorGridCell() : CGridCell()
{
	int d = 1;
}

CTextColorGridCell::~CTextColorGridCell()
{

}

BOOL CTextColorGridCell::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd)
{
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	
	if(m_wndTextColorButton.GetSafeHwnd())
	{
		m_wndTextColorButton.MoveWindow(rect , FALSE);
	}
	else
	{
		BOOL b = m_wndTextColorButton.Create(_T("") , WS_CHILD | WS_VISIBLE , rect , (CWnd*)GetGrid() , nRow*nCol);
		if(TRUE == b)
		{
			vector<CSmartDrawColorButton::RGBQuad>* pColorList = pDoc->GetColorListOf(CTextStyleGridCell::m_rIniFilePath.operator LPCTSTR());
			m_wndTextColorButton.FillColors(pColorList);
		}
		CString rCellText = GetText();
		if(FALSE == rCellText.IsEmpty())
		{
			if(_T("-1") != rCellText)
				m_wndTextColorButton.SetColorIndex(ATOI_T(rCellText));
			else
				m_wndTextColorButton.SetColor((COLORREF)-1);
		}
		else
		{
			m_wndTextColorButton.SetColorIndex(0);
		}
	}
	
	m_wndTextColorButton.EnableWindow((GetGrid()->IsCellEditable(nRow, nCol) && !IsEditing()));
	m_wndTextColorButton.Invalidate();
	
	rect.SetRectEmpty();
	return CGridCell::Draw(pDC , nRow , nCol , rect , FALSE);
}

BOOL CTextColorGridCell::Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar)
{
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	int i=0;
	m_bEditing = TRUE;
	if(NULL == m_pEditWnd)
	{
		if(NULL == m_wndTextColorButton.GetSafeHwnd())
		{
			BOOL b = m_wndTextColorButton.Create(_T("") , WS_CHILD | WS_VISIBLE , rect , (CWnd*)GetGrid() , nID);
			if(TRUE == b)
			{
				m_wndTextColorButton.FillColors(pDoc->GetColorListOf(CTextColorGridCell::m_rIniFilePath.operator LPCTSTR()));
			}
		}

		CString rCellText = GetText();
		if(FALSE == rCellText.IsEmpty())
		{
			m_wndTextColorButton.SetColorIndex(ATOI_T(rCellText));
		}
		else
		{
			m_wndTextColorButton.SetColorIndex(0);
		}

		m_pEditWnd = &m_wndTextColorButton;
		m_pEditWnd->SetFocus();
	}
	else
	{
		CRect wndRect;
		m_pEditWnd->GetWindowRect(&wndRect);
		int height = wndRect.bottom - wndRect.top;
		rect.bottom = rect.top + height;
		m_pEditWnd->MoveWindow(rect);
		m_pEditWnd->ShowWindow(SW_NORMAL);
		m_pEditWnd->SetFocus();
	}

	return TRUE;
}

/**
	@brief	
	@author HumKyung.BAEK	
	@date	2010.07.27
	@return		
**/
CString CTextColorGridCell::GetColorIndex()
{
	CString sColorIndex;
	if(m_wndTextColorButton.GetSafeHwnd())
	{
		const int iColorIndex = m_wndTextColorButton.GetColorIndex();
		if(-1 != iColorIndex)
		{
			sColorIndex.Format(_T("%d") , iColorIndex);
		}
		else
		{
			sColorIndex =  _T("-1");
		}
	}
	else
	{
		sColorIndex = this->GetText();
	}

	return sColorIndex;
}

/**
	@brief	enable automatic color button
	@author	humkyung
	@date	2015.03.22
*/
int CTextColorGridCell::EnableAutomaticButton()
{
	m_wndTextColorButton.EnableAutomaticButton(_T("Automatic"), (COLORREF)-1);
	return ERROR_SUCCESS;
}

void CTextColorGridCell::EndEdit()
{
	if (m_pEditWnd)
	{
		CString rColorIndex = GetColorIndex();
		SetText(rColorIndex);

		m_pEditWnd->ShowWindow(SW_HIDE);
		m_pEditWnd = NULL;
	}

	m_bEditing = FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CTitleBlockPage, CSmartDrawPropertyPage)

CTitleBlockPage::CTitleBlockPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CTitleBlockPage::IDD, pParent)
{

}

CTitleBlockPage::~CTitleBlockPage()
{
}

void CTitleBlockPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_BUTTON_NEW_TITLE_BLOCK_ITEM , m_wndNewTitleBlockItem);
	DDX_Control(pDX , IDC_BUTTON_DELETE_TITLE_BLOCK_ITEM , m_wndDeleteTitleBlockItem);
}


BEGIN_MESSAGE_MAP(CTitleBlockPage, CSmartDrawPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW_TITLE_BLOCK_ITEM, &CTitleBlockPage::OnBnClickedButtonNewTitleBlockItem)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_TITLE_BLOCK_ITEM, &CTitleBlockPage::OnBnClickedButtonDeleteTitleBlockItem)
END_MESSAGE_MAP()


// CTitleBlockPage message handlers

BOOL PropertyPage::CTitleBlockPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	
	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();
	CTextStyleGridCell::m_rIniFilePath = m_sIniFilePath;
	CTextColorGridCell::m_rIniFilePath = m_sIniFilePath;

	CRect rect;
	GetDlgItem(IDC_STATIC_TITLE_BLOCK)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.DeflateRect(7 , 50 , 7 , 7);
	if(TRUE == m_wndGridCtrl.Create(rect , this , 0x100))
	{
		/// 그리드 컨트롤 폰트 코드
		CFont font;
		font.CreateFont(-11,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,_T("Arial")); 
		m_wndGridCtrl.SetFont(&font);
		font.DeleteObject();

		m_wndGridCtrl.SetFixedRowCount(1);
		m_wndGridCtrl.SetFixedColumnCount(1);
		m_wndGridCtrl.SetColumnCount(7);
		m_wndGridCtrl.SetRowHeight(0 , ROW_HEIGHT);

		m_wndGridCtrl.SetItemText(0 , 0 , _T("Label"));
		m_wndGridCtrl.SetItemText(0 , 1 , _T("Origin"));
		m_wndGridCtrl.SetItemText(0 , 2 , _T("Text Style"));
		m_wndGridCtrl.SetItemText(0 , 3 , _T("Justify"));
		m_wndGridCtrl.SetItemText(0 , 4 , _T("Rotation"));
		m_wndGridCtrl.SetItemText(0 , 5 , _T("Level"));
		m_wndGridCtrl.SetItemText(0 , 6 , _T("Color"));

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

		LoadData();
		m_wndGridCtrl.SetColumnWidth(4 , m_wndGridCtrl.GetColumnWidth(4) - 30);
		///m_wndGridCtrl.AutoSizeColumns();
	}

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_NEW),
			IMAGE_ICON,
			0,
			0,
			0);
		HICON hOldIcon = m_wndNewTitleBlockItem.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndNewTitleBlockItem.SetIcon(hIcon);
	}
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDI_DELETE),
			IMAGE_ICON,
			0,
			0,
			0);
		HICON hOldIcon = m_wndDeleteTitleBlockItem.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndDeleteTitleBlockItem.SetIcon(hIcon);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-19
    @class      PropertyPage
    @function   CTitleBlockPage::ReadData
    @return     int
    @param      void
    @brief
******************************************************************************/
int PropertyPage::CTitleBlockPage::LoadData(void)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
		const CString rProjectName(pParent->GetProjectName());

		CString rBorderConfigFilePath = docData.GetServerFolderPath() + _T("\\") + rProjectName + _T("\\Border\\") + CString(szBuf) + _T(".cfg");

		const UINT uiCount = GetPrivateProfileInt(_T("Title Block") , _T("Count") , 0 , rBorderConfigFilePath);
		m_wndGridCtrl.SetRowCount(1);
		for(int row = 1;row <= int(uiCount);++row)
		{
			CString rKey;
			rKey.Format(_T("Data%d") , row - 1);
			if(GetPrivateProfileString(_T("Title Block") , rKey , NULL , szBuf , MAX_PATH , rBorderConfigFilePath))
			{
				vector<STRING_T> oResult;
				CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
				if(m_wndGridCtrl.GetColumnCount() + 1 == oResult.size())
				{
					NewTitleBlockItem(oResult[0].c_str());
					
					m_wndGridCtrl.SetItemText(row , 1 , oResult[1].c_str() + CString(_T(",")) + oResult[2].c_str());
					m_wndGridCtrl.SetItemText(row , 2 , oResult[3].c_str());
					m_wndGridCtrl.SetItemText(row , 3 , oResult[4].c_str());
					m_wndGridCtrl.SetItemText(row , 4 , oResult[5].c_str());
					m_wndGridCtrl.SetItemText(row , 5 , oResult[6].c_str());
					m_wndGridCtrl.SetItemText(row , 6 , oResult[7].c_str());
				}
			}
		}
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	create a new title block item
	@author humkyung
	@date	2013.06.16
	@return		
**/
int PropertyPage::CTitleBlockPage::NewTitleBlockItem(const CString& sLabel)
{
	const int iItem = m_wndGridCtrl.GetRowCount();
	m_wndGridCtrl.SetRowCount( iItem + 1);

	m_wndGridCtrl.SetItemText(iItem , 0 , sLabel);
	for(int col = 0;(col < m_wndGridCtrl.GetColumnCount() + 1);++col)
	{
		if(1 == col)
		{
			m_wndGridCtrl.SetCellType(iItem , col , RUNTIME_CLASS(CPickButtonGridCell));
			m_wndGridCtrl.SetRowHeight(iItem , ROW_HEIGHT);
		}
		else
		{
			if(2 == col)
			{
				m_wndGridCtrl.SetCellType(iItem , col , RUNTIME_CLASS(CTextStyleGridCell));
			}
			else if(3 == col)
			{
				m_wndGridCtrl.SetCellType(iItem , col , RUNTIME_CLASS(CGridCellCombo));

				CStringArray options;
				options.Add(_T("Left Top"));
				options.Add(_T("Left Center"));
				options.Add(_T("Left Bottom"));
				options.Add(_T("Center Top"));
				options.Add(_T("Center Center"));
				options.Add(_T("Center Bottom"));
				options.Add(_T("Right Top"));
				options.Add(_T("Right Center"));
				options.Add(_T("Right Bottom"));

				CGridCellCombo *pCell = (CGridCellCombo*) m_wndGridCtrl.GetCell(iItem , col);
				pCell->SetOptions(options);
				pCell->SetStyle(CBS_DROPDOWNLIST);
			}
			else if(4 == col)
			{
				m_wndGridCtrl.SetCellType(iItem , col , RUNTIME_CLASS(CGridCellCombo));

				CStringArray options;
				options.Add(_T("0"));
				options.Add(_T("90"));
				options.Add(_T("180"));
				options.Add(_T("270"));

				CGridCellCombo *pCell = (CGridCellCombo*) m_wndGridCtrl.GetCell(iItem , col);
				pCell->SetOptions(options);
				pCell->SetStyle(CBS_DROPDOWNLIST);
			}
			else if(5 == col)
			{
				m_wndGridCtrl.SetCellType(iItem , col , RUNTIME_CLASS(CLevelGridCell));
			}
			else if(6 == col)
			{
				m_wndGridCtrl.SetCellType(iItem , col , RUNTIME_CLASS(CTextColorGridCell));
			}

			if(0 == col)
			{
				m_wndGridCtrl.SetRowHeight(iItem , ROW_HEIGHT);
			}
			else
			{
				m_wndGridCtrl.SetRowHeight(iItem , ROW_HEIGHT);
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	OnBnClickedButtonNewTitleBlockItem

	@author humkyung

	@date	2013.06.15

	@return		
**/
void PropertyPage::CTitleBlockPage::OnBnClickedButtonNewTitleBlockItem()
{
	CString str;
	GetDlgItemText(IDC_EDIT_TITLE_BLOCK_ITEM_LABEL , str);
	if(!str.IsEmpty())
	{
		str.MakeUpper();

		const int iCount = m_wndGridCtrl.GetRowCount();
		for(int i = 1;i < iCount;++i)
		{
			const CString sLabel = m_wndGridCtrl.GetItemText(i , 0).MakeUpper();
			if(sLabel == str)
			{
				AfxMessageBox(_T("There is same label") , MB_OK | MB_ICONWARNING);
				return;
			}
		}
		NewTitleBlockItem(str);
	}
	else
	{
		AfxMessageBox(_T("Label is empty") , MB_OK | MB_ICONWARNING);
	}
}

/**
	@brief	OnBnClickedButtonDeleteTitleBlockItem
	@author HumKyung.BAEK	
	@date	2010.07.03
	@return		
**/
void PropertyPage::CTitleBlockPage::OnBnClickedButtonDeleteTitleBlockItem()
{
	CCellRange range = m_wndGridCtrl.GetSelectedCellRange();
	if(range.Count() > 0)
	{
		const int iRow = range.GetMinRow();
		const CString sLabel = m_wndGridCtrl.GetItemText(iRow,0);
		if((sLabel == _T("DWG.NO 0")) || (sLabel == _T("DWG.NO 90")) || (sLabel == _T("SCALE")) || (sLabel == _T("REV")))
		{
			AfxMessageBox(_TR("Can't delete selected item") , MB_OK | MB_ICONWARNING);
		}
		else
		{
			m_wndGridCtrl.DeleteRow(iRow);
			m_wndGridCtrl.Invalidate();
		}
	}
}

/**
	@brief	
	@author HumKyung.BAEK	
	@date	2010.07.27
	@return		
**/
int PropertyPage::CTitleBlockPage::SaveData()
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
		const CString rProjectName(pParent->GetProjectName());

		CString rBorderConfigFilePath = docData.GetServerFolderPath() + _T("\\") + rProjectName + _T("\\Border\\") + CString(szBuf) + _T(".cfg");

		CString rString;
		int nCount = m_wndGridCtrl.GetRowCount() - 1;
		rString.Format(_T("%d") , nCount);
		WritePrivateProfileString(_T("Title Block") , _T("Count") , rString , rBorderConfigFilePath);

		for(int i = 1;i <= nCount;++i)
		{
			rString = _T("");
			for(int j = 0;j < m_wndGridCtrl.GetColumnCount();++j)
			{
				if(j < m_wndGridCtrl.GetColumnCount() - 1)
				{
					rString += CString(m_wndGridCtrl.GetCell(i,j)->GetText()) + _T(",");
				}
				else
				{
					CGridCellBase* pCell = m_wndGridCtrl.GetCell(i,j);
					if(pCell->IsKindOf(RUNTIME_CLASS(CTextColorGridCell)))
					{
						CTextColorGridCell* pTextColorGridCell = static_cast<CTextColorGridCell*>(pCell);
						rString += pTextColorGridCell->GetColorIndex();
					}
					else
					{
						rString += pCell->GetText();
					}
				}
			}
			if(_T(",") == rString.Right(1)) rString = rString.Left(rString.GetLength() - 1);

			CString rKeyName;
			rKeyName.Format(_T("Data%d") , i - 1);
			WritePrivateProfileString(_T("Title Block") , rKeyName , rString , rBorderConfigFilePath);
		}
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	
	@author HumKyung.BAEK	
	@date	2010.07.28
	@return		
**/
int PropertyPage::CTextColorGridCell::FillColors(const CString& rIniFilePath)
{
	if(m_wndTextColorButton.GetSafeHwnd())
	{
		CSmartDrawDoc* pDoc = GetMDIActiveDocument();

		m_wndTextColorButton.FillColors(pDoc->GetColorListOf(rIniFilePath.operator LPCTSTR()));
	}

	return ERROR_SUCCESS;
}
