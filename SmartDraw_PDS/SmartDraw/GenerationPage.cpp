// GenerationPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "SmartDrawDoc.h"
#include <Tokenizer.h>
#include <util/FileTools.h>
#include <util/SplitPath.h>
#include <gui/GridCtrl/GridCellCombo.h>
#include <gui/GridCtrl/GridCellCheckButton.h>
#include "SmartDrawPropertySheet.h"
#include "GenerationPage.h"
#include "AnnoFilterGridCellButton.h"

#include <fstream>
#include <string>
using namespace std;

using namespace PropertyPage;
// CGenerationPage dialog

IMPLEMENT_DYNAMIC(CGenerationPage, CSmartDrawPropertyPage)

CGenerationPage::CGenerationPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CGenerationPage::IDD, pParent)
{

}

CGenerationPage::~CGenerationPage()
{
	try
	{
	}
	catch(...)
	{
	}
}

void CGenerationPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);
	
	DDX_Control(pDX , IDC_BUTTON_NEW_LEVEL_ITEM , m_wndNewLevelItem);
	DDX_Control(pDX , IDC_BUTTON_DELETE_LEVEL_ITEM , m_wndDeleteLevelItem);
}


BEGIN_MESSAGE_MAP(CGenerationPage, CSmartDrawPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_NEW_LEVEL_ITEM, &CGenerationPage::OnBnClickedButtonNewLevelItem)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_LEVEL_ITEM, &CGenerationPage::OnBnClickedButtonDeleteLevelItem)
END_MESSAGE_MAP()


// CGenerationPage message handlers

BOOL PropertyPage::CGenerationPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	
	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_NEW),IMAGE_ICON,0,0,0);
		HICON hOldIcon = m_wndNewLevelItem.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndNewLevelItem.SetIcon(hIcon);
	}
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_DELETE),IMAGE_ICON,0,0,0);
		HICON hOldIcon = m_wndDeleteLevelItem.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndDeleteLevelItem.SetIcon(hIcon);
	}

	CRect rect;
	GetDlgItem(IDC_STATIC_ITEMS)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.DeflateRect(7 , 20 , 7 , 7);
	BOOL b = m_wndGridCtrl.Create(rect , this , 0x100);
	if(TRUE == b)
	{
		///그리드 컨트롤 폰트 코드
		{
			CFont font;
			font.CreateFont(-11,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,_T("Arial")); 
			m_wndGridCtrl.SetFont(&font);
			font.DeleteObject();
		}

		m_wndGridCtrl.SetFixedRowCount(1);
		m_wndGridCtrl.SetFixedColumnCount(1);
		m_wndGridCtrl.SetColumnCount(4);
		m_wndGridCtrl.SetRowHeight(0 , DEFAULT_GRID_ROW_HEIGHT);

		m_wndGridCtrl.SetItemText(0 , 0 , _T("Item"));
		m_wndGridCtrl.SetItemText(0 , 1 , _T("Dim. Level1"));
		m_wndGridCtrl.SetItemText(0 , 2 , _T("Dim. Level2"));
		m_wndGridCtrl.SetItemText(0 , 3 , _T("Dim. Level3"));

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
		m_wndGridCtrl.AutoSizeColumns();
	}

	GetDlgItem(IDC_STATIC_LOCATION_ORDER)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.DeflateRect(7 , 20 , 7 , 7);
	b = m_wndLocationOrderGridCtrl.Create(rect , this , IDC_STATIC_LOCATION_ORDER);
	if(TRUE == b)
	{
		///그리드 컨트롤 폰트 코드
		{
			CFont font;
			font.CreateFont(-11,0,0,0,400,FALSE,FALSE,FALSE,DEFAULT_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,_T("Arial")); 
			m_wndLocationOrderGridCtrl.SetFont(&font);
			font.DeleteObject();
		}

		m_wndLocationOrderGridCtrl.SetFixedRowCount(1);
		m_wndLocationOrderGridCtrl.SetRowCount(4);
		m_wndLocationOrderGridCtrl.SetFixedColumnCount(1);
		m_wndLocationOrderGridCtrl.SetColumnCount(2);
		m_wndLocationOrderGridCtrl.SetRowHeight(0 , DEFAULT_GRID_ROW_HEIGHT);

		m_wndLocationOrderGridCtrl.SetItemText(0 , 0 , _T("Item"));
		m_wndLocationOrderGridCtrl.SetItemText(0 , 1 , _T("Order"));
		m_wndLocationOrderGridCtrl.SetItemText(1 , 0 , _T("Structure"));
		m_wndLocationOrderGridCtrl.SetItemText(2 , 0 , _T("Equipment"));
		m_wndLocationOrderGridCtrl.SetItemText(3 , 0 , _T("Pipe"));

		m_wndLocationOrderGridCtrl.AutoSizeColumns();
	}

	GetDlgItem(IDC_STATIC_GENERATE_ITEMS)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.DeflateRect(7 , 20 , 7 , 7);
	b = m_wndGenerateItemsGridCtrl.Create(rect , this , IDC_STATIC_GENERATE_ITEMS);
	if(TRUE == b)
	{
		///그리드 컨트롤 폰트 코드
		{
			CFont font;
			font.CreateFont(-11,0,0,0,400,FALSE,FALSE,FALSE,HANGUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,_T("Arial")); 
			m_wndGenerateItemsGridCtrl.SetFont(&font);
			font.DeleteObject();
		}

		m_wndGenerateItemsGridCtrl.SetFixedRowCount(1);
		m_wndGenerateItemsGridCtrl.SetFixedColumnCount(1);
		m_wndGenerateItemsGridCtrl.SetColumnCount(4);
		m_wndGenerateItemsGridCtrl.SetRowHeight(0 , DEFAULT_GRID_ROW_HEIGHT);

		m_wndGenerateItemsGridCtrl.SetItemText(0 , 0 , _T("Item"));
		m_wndGenerateItemsGridCtrl.SetItemText(0 , 1 , _T("Generation"));
		m_wndGenerateItemsGridCtrl.SetItemText(0 , 2 , _T("Search Type"));
		m_wndGenerateItemsGridCtrl.SetItemText(0 , 3 , _T("Search Dist."));

		m_wndLocationOrderGridCtrl.AutoSizeColumns();
	}

	UpdateContents();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int PropertyPage::CGenerationPage::UpdateContents(void)
{
	const CString sApp(_T("GENERATION_OPTIONS"));

	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	
	vector<STRING_T> oResult1 , oResult2 , oResult3;
	if(GetPrivateProfileString(sApp , _T("Level1 Ele") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult1 , szBuf , CIsComma());
	}

	if(GetPrivateProfileString(sApp , _T("Level2 Ele") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult2 , szBuf , CIsComma());
	}

	if(GetPrivateProfileString(sApp , _T("Level3 Ele") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult3 , szBuf , CIsComma());
	}

	CStringArray options;
	this->GetLevelEleArray(options);

	const int iMaxRowCount = max(max(oResult1.size() , oResult2.size()) , oResult3.size());
	m_wndGridCtrl.SetRowCount(iMaxRowCount + 1);
	for(int i = 1;i < iMaxRowCount + 1;++i)
	{
		CString rString;
		rString.Format(_T("%d") , i);
		m_wndGridCtrl.SetItemText(i , 0 , rString);
		m_wndGridCtrl.SetRowHeight(i , DEFAULT_GRID_ROW_HEIGHT);

		/// set cell type
		m_wndGridCtrl.SetCellType(i , 1 , RUNTIME_CLASS(CGridCellCombo));
		CGridCellCombo *pCell = (CGridCellCombo*)m_wndGridCtrl.GetCell(i , 1);
		pCell->SetStyle(CBS_DROPDOWNLIST);
		pCell->SetOptions(options);

		m_wndGridCtrl.SetCellType(i , 2 , RUNTIME_CLASS(CGridCellCombo));
		pCell = (CGridCellCombo*)m_wndGridCtrl.GetCell(i , 2);
		pCell->SetStyle(CBS_DROPDOWNLIST);
		pCell->SetOptions(options);

		m_wndGridCtrl.SetCellType(i , 3 , RUNTIME_CLASS(CGridCellCombo));
		pCell = (CGridCellCombo*)m_wndGridCtrl.GetCell(i , 3);
		pCell->SetStyle(CBS_DROPDOWNLIST);
		pCell->SetOptions(options);
		/// up to here
	}
	
	int idx = 1;
	for(vector<STRING_T>::const_iterator itr = oResult1.begin();itr != oResult1.end();++itr,++idx)
	{
		m_wndGridCtrl.SetItemText(idx , 1 , itr->c_str());
	}

	idx = 1;
	for(vector<STRING_T>::const_iterator itr = oResult2.begin();itr != oResult2.end();++itr,++idx)
	{
		m_wndGridCtrl.SetItemText(idx , 2 , itr->c_str());
	}

	idx = 1;
	for(vector<STRING_T>::const_iterator itr = oResult3.begin();itr != oResult3.end();++itr,++idx)
	{
		m_wndGridCtrl.SetItemText(idx , 3 , itr->c_str());
	}

	CStringArray aLocationOrders;
	{
		aLocationOrders.Add(_T("Location Order1"));
		aLocationOrders.Add(_T("Location Order2"));
		aLocationOrders.Add(_T("Location Order3"));
	}

	for(int i = 1;i <= 3;++i)
	{
		/// set cell type
		{
			m_wndLocationOrderGridCtrl.SetCellType(i , 1 , RUNTIME_CLASS(CGridCellCombo));
			CGridCellCombo *pCell = (CGridCellCombo*)m_wndLocationOrderGridCtrl.GetCell(i , 1);
			pCell->SetStyle(CBS_DROPDOWNLIST);
			pCell->SetOptions(aLocationOrders);
		}
		/// up to here
	}
	if(GetPrivateProfileString(sApp , _T("Location Order1") , _T("Structure") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult1 , szBuf , CIsComma());
		for(vector<STRING_T>::const_iterator itr = oResult1.begin();itr != oResult1.end();++itr)
		{
			for(int i = 1;i < m_wndLocationOrderGridCtrl.GetRowCount();++i)
			{
				if(CString(itr->c_str()) == m_wndLocationOrderGridCtrl.GetItemText(i,0))
				{
					m_wndLocationOrderGridCtrl.SetItemText(i,1,aLocationOrders.GetAt(0));
				}
			}
		}
	}

	if(GetPrivateProfileString(sApp , _T("Location Order2") , _T("Equipment") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult2 , szBuf , CIsComma());
		for(vector<STRING_T>::const_iterator itr = oResult2.begin();itr != oResult2.end();++itr)
		{
			for(int i = 1;i < m_wndLocationOrderGridCtrl.GetRowCount();++i)
			{
				if(CString(itr->c_str()) == m_wndLocationOrderGridCtrl.GetItemText(i,0))
				{
					m_wndLocationOrderGridCtrl.SetItemText(i,1,aLocationOrders.GetAt(1));
				}
			}
		}
	}

	if(GetPrivateProfileString(sApp , _T("Location Order3") , _T("Pipe") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult3 , szBuf , CIsComma());
		for(vector<STRING_T>::const_iterator itr = oResult3.begin();itr != oResult3.end();++itr)
		{
			for(int i = 1;i < m_wndLocationOrderGridCtrl.GetRowCount();++i)
			{
				if(CString(itr->c_str()) == m_wndLocationOrderGridCtrl.GetItemText(i,0))
				{
					m_wndLocationOrderGridCtrl.SetItemText(i,1,aLocationOrders.GetAt(2));
				}
			}
		}
	}
	m_wndLocationOrderGridCtrl.AutoSizeColumns();

	///
	CStringArray aSearchTypes;
	{
		aSearchTypes.Add(_T("Horizontal"));
		aSearchTypes.Add(_T("Vertical"));
		aSearchTypes.Add(_T("Orthogonal"));
		aSearchTypes.Add(_T("Around"));
	}

	CStringArray aSearchDists;
	{
		aSearchDists.Add(_T("Near"));
		aSearchDists.Add(_T("Middle"));
		aSearchDists.Add(_T("Far"));
	}

	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	const int nGenerationItemCount = GetPrivateProfileInt(sApp , _T("Generation Item Count") , 0 , m_sIniFilePath);
	m_wndGenerateItemsGridCtrl.SetRowCount(nGenerationItemCount + 1);
	for(int i = 0;i < nGenerationItemCount;++i)
	{
		CString rKeyName;
		rKeyName.Format(_T("Generation Item%d") , i);
		
		/// set cell type
		{
			m_wndGenerateItemsGridCtrl.SetCellType(i + 1 , 2 , RUNTIME_CLASS(CGridCellCombo));
			CGridCellCombo *pCell = (CGridCellCombo*)m_wndGenerateItemsGridCtrl.GetCell(i + 1 , 2);
			pCell->SetStyle(CBS_DROPDOWNLIST);
			pCell->SetOptions(aSearchTypes);

			m_wndGenerateItemsGridCtrl.SetCellType(i + 1 , 3 , RUNTIME_CLASS(CGridCellCombo));
			pCell = (CGridCellCombo*)m_wndGenerateItemsGridCtrl.GetCell(i + 1 , 3);
			pCell->SetStyle(CBS_DROPDOWNLIST);
			pCell->SetOptions(aSearchDists);
		}
		/// up to here

		if(GetPrivateProfileString(sApp , rKeyName , NULL , szBuf , MAX_PATH , m_sIniFilePath))
		{	
			CTokenizer<CIsComma>::Tokenize(oResult1 , szBuf , CIsComma());
			if(oResult1.size() >= 2)
			{
				m_wndGenerateItemsGridCtrl.SetItemText(i+1 , 0 , oResult1[0].c_str());
				m_wndGenerateItemsGridCtrl.SetRowHeight(i+1 , DEFAULT_GRID_ROW_HEIGHT);
				
				BOOL bDisplay = TRUE;
				if(pDoc->IsFilterItem(oResult1[0]))
				{
					m_wndGenerateItemsGridCtrl.SetCellType(i+1 , 1 , RUNTIME_CLASS(CAnnoFilterGridCellButton));
					CAnnoFilterGridCellButton* pAnnoFilterButton = (CAnnoFilterGridCellButton*)(m_wndGenerateItemsGridCtrl.GetCell(i+1 , 1));
					CSplitPath path(m_sIniFilePath);
					pAnnoFilterButton->SetFilterItem(path.GetFileName().operator LPCTSTR(),oResult1[0]);
				}
				else
				{
					m_wndGenerateItemsGridCtrl.SetCellType(i + 1 , 1 , RUNTIME_CLASS(CGridCellCheckButton));
					CGridCellCheckButton* pCellCheckButton = (CGridCellCheckButton*)(m_wndGenerateItemsGridCtrl.GetCell(i + 1 , 1));
					pCellCheckButton->SetCheck(_T("True") == oResult1[1]);
					bDisplay = pCellCheckButton->GetCheck();
				}

				if(2 == oResult1.size()) oResult1.push_back(_T("Around"));
				if(3 == oResult1.size()) oResult1.push_back(_T("Near"));

				m_wndGenerateItemsGridCtrl.SetItemText(i + 1,2,oResult1[2].c_str());
				m_wndGenerateItemsGridCtrl.SetItemText(i + 1,3,oResult1[3].c_str());
				if(FALSE == bDisplay)
				{
					m_wndGenerateItemsGridCtrl.SetItemState(i+1 , 2 , m_wndGenerateItemsGridCtrl.GetItemState(i+1,2) | GVIS_READONLY);
					m_wndGenerateItemsGridCtrl.SetItemState(i+1 , 3 , m_wndGenerateItemsGridCtrl.GetItemState(i+1,2) | GVIS_READONLY);
				}
				else
				{
					m_wndGenerateItemsGridCtrl.SetItemState(i+1 , 2 , m_wndGenerateItemsGridCtrl.GetItemState(i+1,2) & ~GVIS_READONLY);
					m_wndGenerateItemsGridCtrl.SetItemState(i+1 , 3 , m_wndGenerateItemsGridCtrl.GetItemState(i+1,2) & ~GVIS_READONLY);
				}
			}
		}
	}
	m_wndGenerateItemsGridCtrl.AutoSizeColumns();
	
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-18
    @function   CGenerationPage::SaveData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CGenerationPage::SaveData()
{
	const CString sApp(_T("GENERATION_OPTIONS"));
	CSplitPath path(m_sIniFilePath);
	const STRING_T sSettingName(path.GetFileName().operator LPCTSTR());

	const int iRowCount = m_wndGridCtrl.GetRowCount();

	CString sLevelEleList;
	for(int i = 1;i < iRowCount;++i)
	{
		CString sItemText = m_wndGridCtrl.GetItemText(i , 1);
		if(sItemText.IsEmpty()) continue;
		sLevelEleList += (!sLevelEleList.IsEmpty() ? (_T(",") + sItemText) : sItemText);
	}
	if(!sLevelEleList.IsEmpty()) WritePrivateProfileString(sApp , _T("Level1 Ele") , sLevelEleList , m_sIniFilePath);

	sLevelEleList.Empty();
	for(int i = 1;i < iRowCount;++i)
	{
		CString sItemText = m_wndGridCtrl.GetItemText(i , 2);
		if(sItemText.IsEmpty()) continue;
		sLevelEleList += (!sLevelEleList.IsEmpty() ? (_T(",") + sItemText) : sItemText);
	}
	if(!sLevelEleList.IsEmpty()) WritePrivateProfileString(sApp , _T("Level2 Ele") , sLevelEleList , m_sIniFilePath);

	sLevelEleList.Empty();
	for(int i = 1;i < iRowCount;++i)
	{
		CString sItemText = m_wndGridCtrl.GetItemText(i , 3);
		if(sItemText.IsEmpty()) continue;
		sLevelEleList += (!sLevelEleList.IsEmpty() ? (_T(",") + sItemText) : sItemText);
	}
	if(!sLevelEleList.IsEmpty()) WritePrivateProfileString(sApp , _T("Level3 Ele") , sLevelEleList , m_sIniFilePath);

	/// 2015.01.01 added by humkyung
	CString sLocationOrderList[3];
	for(int i = 1;i < m_wndLocationOrderGridCtrl.GetRowCount();++i)
	{
		const CString sItemText = m_wndLocationOrderGridCtrl.GetItemText(i , 0);
		const CString sOrderText= m_wndLocationOrderGridCtrl.GetItemText(i , 1);
		if(sOrderText.IsEmpty()) continue;
		const int iIndex = (_T("Location Order1") == sOrderText) ? 0 : ((_T("Location Order2") == sOrderText) ? 1 : 2);
		sLocationOrderList[iIndex] += (!sLocationOrderList[iIndex].IsEmpty() ? (_T(",") + sItemText) : sItemText);
	}
	WritePrivateProfileString(sApp , _T("Location Order1") , sLocationOrderList[0] , m_sIniFilePath);
	WritePrivateProfileString(sApp , _T("Location Order2") , sLocationOrderList[1] , m_sIniFilePath);
	WritePrivateProfileString(sApp , _T("Location Order3") , sLocationOrderList[2] , m_sIniFilePath);
	/// up to here

	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	/// 2014.03.30 added by humkyung
	CString sKey , sValue;
	sValue.Format(_T("%d") , m_wndGenerateItemsGridCtrl.GetRowCount() - 1);
	WritePrivateProfileString(sApp , _T("Generation Item Count") , sValue , m_sIniFilePath);
	for(int i = 1;i < m_wndGenerateItemsGridCtrl.GetRowCount();++i)
	{
		sKey.Format(_T("Generation Item%d") , i - 1);
		const CString sItemText = m_wndGenerateItemsGridCtrl.GetItemText(i , 0);
		const CString sSearchType = m_wndGenerateItemsGridCtrl.GetItemText(i , 2);
		const CString sSearchDist = m_wndGenerateItemsGridCtrl.GetItemText(i , 3);
		if(pDoc->IsFilterItem(sItemText.operator LPCTSTR()))
		{
			sValue.Format(_T("%s,ByFilter,%s,%s") , sItemText , sSearchType , sSearchDist);
			((CAnnoFilterGridCellButton*)m_wndGenerateItemsGridCtrl.GetCell(i,1))->SaveData();
		}
		else
		{
			sValue.Format(_T("%s,%s,%s,%s") , sItemText , (TRUE == ((CGridCellCheckButton*)m_wndGenerateItemsGridCtrl.GetCell(i , 1))->GetCheck()) ? _T("True") : _T("False") , sSearchType , sSearchDist);
		}
		WritePrivateProfileString(sApp , sKey , sValue , m_sIniFilePath);
	}
	/// up to here

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-18
    @function   CGenerationPage::GetLevelEleArray
    @return     int
    @param      CStringArray&   options
    @brief
******************************************************************************/
int PropertyPage::CGenerationPage::GetLevelEleArray(CStringArray& options)
{
	CSmartDrawPropertySheet* pSheet = (CSmartDrawPropertySheet*)GetParent();
	const STRING_T sDrawingType = pSheet->GetDrawingType();
	CString sCommonAppDataPath = CFileTools::GetCommonAppDataPath();
	const STRING_T sEleFilePath = STRING_T(sCommonAppDataPath.operator LPCTSTR()) + _T("\\SmartDraw_PDS\\Drawings\\") + sDrawingType + _T(".ele");
	
	IFSTREAM_T ifile(sEleFilePath.c_str());
	if(ifile.is_open())
	{
		STRING_T aLine;
		while(!ifile.eof())
		{
			getline(ifile , aLine);
			if(ifile.eof()) break;
			options.Add(aLine.c_str());
		}
		ifile.close();
	}
	options.Add(_T("Global"));
	options.Add(_T(""));

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-18
    @function   CGenerationPage::OnBnClickedButtonNewLevelItem
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CGenerationPage::OnBnClickedButtonNewLevelItem()
{
	m_wndGridCtrl.SetRowCount( m_wndGridCtrl.GetRowCount() + 1 );
	for(int i = 1;i < m_wndGridCtrl.GetRowCount();++i)
	{
		CString rString;
		rString.Format(_T("%d") , i);
		m_wndGridCtrl.SetItemText(i , 0 , rString);
		m_wndGridCtrl.SetRowHeight(i , DEFAULT_GRID_ROW_HEIGHT);
	}

	CStringArray options;
	this->GetLevelEleArray(options);

	m_wndGridCtrl.SetCellType(m_wndGridCtrl.GetRowCount() - 1 , 1 , RUNTIME_CLASS(CGridCellCombo));
	CGridCellCombo *pCell = (CGridCellCombo*)m_wndGridCtrl.GetCell(m_wndGridCtrl.GetRowCount() - 1 , 1);
	pCell->SetStyle(CBS_DROPDOWNLIST);
	pCell->SetOptions(options);

	m_wndGridCtrl.SetCellType(m_wndGridCtrl.GetRowCount() - 1 , 2 , RUNTIME_CLASS(CGridCellCombo));
	pCell = (CGridCellCombo*)m_wndGridCtrl.GetCell(m_wndGridCtrl.GetRowCount() - 1 , 2);
	pCell->SetStyle(CBS_DROPDOWNLIST);
	pCell->SetOptions(options);

	m_wndGridCtrl.SetCellType(m_wndGridCtrl.GetRowCount() - 1 , 3 , RUNTIME_CLASS(CGridCellCombo));
	pCell = (CGridCellCombo*)m_wndGridCtrl.GetCell(m_wndGridCtrl.GetRowCount() - 1 , 3);
	pCell->SetStyle(CBS_DROPDOWNLIST);
	pCell->SetOptions(options);
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-18
    @function   CGenerationPage::OnBnClickedButtonDeleteLevelItem
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CGenerationPage::OnBnClickedButtonDeleteLevelItem()
{
	if(IDYES == AfxMessageBox(_T("Do you want to delete selected row?") , MB_YESNO | MB_ICONQUESTION))
	{
		CCellRange range = m_wndGridCtrl.GetSelectedCellRange();
		m_wndGridCtrl.DeleteRow(range.GetMinRow());
		m_wndGridCtrl.Invalidate();

		for(int i = 1;i < m_wndGridCtrl.GetRowCount();++i)
		{
			CString rString;
			rString.Format(_T("%d") , i);
			m_wndGridCtrl.SetItemText(i , 0 , rString);
			m_wndGridCtrl.SetRowHeight(i , DEFAULT_GRID_ROW_HEIGHT);
		}
	}
}

/******************************************************************************
    @brief		set enable when button set checked, otherwise set disable
	@author     humkyung
    @date       2015-01-07
    @function   CGenerationPage::OnNotify
    @return     BOOL
******************************************************************************/
BOOL PropertyPage::CGenerationPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    if(wParam == m_wndGenerateItemsGridCtrl.GetDlgCtrlID())
	{
		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);

		if((NM_CLICK == pGridView->hdr.code) && (1 == pGridView->iColumn))
		{
			CGridCellCheckButton* pCell = (CGridCellCheckButton*)m_wndGenerateItemsGridCtrl.GetCell(pGridView->iRow, pGridView->iColumn);
			if(FALSE == pCell->GetCheck())
			{
				m_wndGenerateItemsGridCtrl.SetItemState(pGridView->iRow , 2 , m_wndGenerateItemsGridCtrl.GetItemState(pGridView->iRow,2) | GVIS_READONLY);
				m_wndGenerateItemsGridCtrl.SetItemState(pGridView->iRow , 3 , m_wndGenerateItemsGridCtrl.GetItemState(pGridView->iRow,2) | GVIS_READONLY);
			}
			else
			{
				m_wndGenerateItemsGridCtrl.SetItemState(pGridView->iRow , 2 , m_wndGenerateItemsGridCtrl.GetItemState(pGridView->iRow,2) & ~GVIS_READONLY);
				m_wndGenerateItemsGridCtrl.SetItemState(pGridView->iRow , 3 , m_wndGenerateItemsGridCtrl.GetItemState(pGridView->iRow,2) & ~GVIS_READONLY);
			}

			m_wndGenerateItemsGridCtrl.RedrawRow(pGridView->iRow);
		}
	}

	return CSmartDrawPropertyPage::OnNotify(wParam , lParam , pResult);
}