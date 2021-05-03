// AnnoFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "SmartDrawDoc.h"
#include <gui/GridCtrl/GridCellCombo.h>
#include "AnnoFilterDlg.h"
#include "afxdialogex.h"

#include "soci.h"
#include "soci-sqlite3.h"

#include <list>

// CAnnoFilterDlg dialog

IMPLEMENT_DYNAMIC(CAnnoFilterDlg, CDialog)

CAnnoFilterDlg::CAnnoFilterDlg(const STRING_T& sSettingName,const STRING_T& sFilterItem,AnnoFilters* pAnnoFilters,CWnd* pParent /*=NULL*/)
	: CDialog(CAnnoFilterDlg::IDD, pParent)
{
	m_sSettingName = sSettingName;
	m_sFilterItem = sFilterItem;
	m_pAnnoFilters = new AnnoFilters;
	for(AnnoFilters::iterator itr = pAnnoFilters->begin();itr != pAnnoFilters->end();++itr)
	{
		m_pAnnoFilters->push_back((*itr)->Clone());
	}
}

CAnnoFilterDlg::~CAnnoFilterDlg()
{
	try
	{
		for(AnnoFilters::iterator itr = m_pAnnoFilters->begin();itr != m_pAnnoFilters->end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		SAFE_DELETE(m_pAnnoFilters);
	}
	catch(...)
	{
	}
}

void CAnnoFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_BUTTON_NEW_FILTER , m_wndNewFilter);
	DDX_Control(pDX , IDC_BUTTON_DELETE_FILTER , m_wndDeleteFilter);
	DDX_Control(pDX , IDC_BUTTON_FILTER_UP , m_wndFilterUp);
	DDX_Control(pDX , IDC_BUTTON_FILTER_DOWN , m_wndFilterDown);
	DDX_Control(pDX , IDOK , m_wndOKButton);
	DDX_Control(pDX , IDCANCEL , m_wndCancelButton);
}


BEGIN_MESSAGE_MAP(CAnnoFilterDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_NEW_FILTER, &CAnnoFilterDlg::OnBnClickedButtonNewFilter)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_FILTER, &CAnnoFilterDlg::OnBnClickedButtonDeleteFilter)
	ON_BN_CLICKED(IDC_BUTTON_FILTER_UP, &CAnnoFilterDlg::OnBnClickedButtonFilterUp)
	ON_BN_CLICKED(IDC_BUTTON_FILTER_DOWN, &CAnnoFilterDlg::OnBnClickedButtonFilterDown)
END_MESSAGE_MAP()


// CAnnoFilterDlg message handlers

BOOL CAnnoFilterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_NEW),IMAGE_ICON,0,0,0);
		HICON hOldIcon = m_wndNewFilter.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndNewFilter.SetIcon(hIcon);
	}
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_DELETE),IMAGE_ICON,0,0,0);
		HICON hOldIcon = m_wndDeleteFilter.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndDeleteFilter.SetIcon(hIcon);
	}

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_UP),IMAGE_ICON,0,0,0);
		HICON hOldIcon = m_wndFilterUp.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndFilterUp.SetIcon(hIcon);
	}
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_DOWN),IMAGE_ICON,0,0,0);
		HICON hOldIcon = m_wndFilterDown.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndFilterDown.SetIcon(hIcon);
	}

	CRect rect;
	GetDlgItem(IDC_STATIC_ANNO_FILTER)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.DeflateRect(7 , 20 , 7 , 7);
	BOOL b = m_wndGridCtrl.Create(rect , this , IDC_STATIC_ANNO_FILTER);
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
		m_wndGridCtrl.SetFixedColumnCount(0);
		m_wndGridCtrl.SetColumnCount(3);
		m_wndGridCtrl.SetRowHeight(0 , DEFAULT_GRID_ROW_HEIGHT);

		m_wndGridCtrl.SetItemText(0 , 0 , _T("Property"));
		m_wndGridCtrl.SetItemText(0 , 1 , _T("Operator"));
		m_wndGridCtrl.SetItemText(0 , 2 , _T("Value"));
	}

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_OK),IMAGE_ICON,16,16,0);
		HICON hOldIcon = m_wndOKButton.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndOKButton.SetIcon(hIcon);
	}
	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_CANCEL),IMAGE_ICON,16,16,0);
		HICON hOldIcon = m_wndCancelButton.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		m_wndCancelButton.SetIcon(hIcon);
	}

	UpdateContents();

	return TRUE;
}

/**
	@brief	fill cells with filter
	@author	humkyung
	@date	2016.03.19
*/
int CAnnoFilterDlg::FillCellsWithFilter(const int& iRow,CAnnoFilter* pAnnoFilter)
{
	assert((NULL != pAnnoFilter) && "Filter is null");
	if(NULL != pAnnoFilter)
	{
		CSmartDrawDoc* pDoc = GetMDIActiveDocument();

		CStringArray oPropArray;
		list<STRING_T> oPropList;
		pDoc->GetPropsOf(oPropList,m_sFilterItem);
		for(list<STRING_T>::const_iterator itr = oPropList.begin();itr != oPropList.end();++itr)
		{
			oPropArray.Add(itr->c_str());
		}

		/// set cell type
		m_wndGridCtrl.SetCellType(iRow , 0 , RUNTIME_CLASS(CGridCellCombo));
		CGridCellCombo *pCell = (CGridCellCombo*)m_wndGridCtrl.GetCell(iRow , 0);
		pCell->SetStyle(CBS_DROPDOWNLIST);
		pCell->SetOptions(oPropArray);
		pCell->SetText(pAnnoFilter->GetProperty());

		CStringArray oOperArray;
		list<STRING_T> oOperList;
		pDoc->GetOperatorsOf(oOperList,pAnnoFilter->GetProperty());
		for(list<STRING_T>::const_iterator itr = oOperList.begin();itr != oOperList.end();++itr)
		{
			oOperArray.Add(itr->c_str());
		}
		m_wndGridCtrl.SetCellType(iRow , 1 , RUNTIME_CLASS(CGridCellCombo));
		pCell = (CGridCellCombo*)m_wndGridCtrl.GetCell(iRow , 1);
		pCell->SetStyle(CBS_DROPDOWNLIST);
		pCell->SetOptions(oOperArray);
		pCell->SetText(pAnnoFilter->GetOperator());

		list<STRING_T> oValueList;
		pDoc->GetValuesOf(oValueList,pAnnoFilter->GetProperty());
		if(!oValueList.empty())
		{
			CStringArray oValueArray;
			for(list<STRING_T>::const_iterator itr = oValueList.begin();itr != oValueList.end();++itr)
			{
				oValueArray.Add(itr->c_str());
			}

			CGridCell *pDefaultCell = (CGridCell*)m_wndGridCtrl.GetCell(iRow , 2);
			if(!pDefaultCell->IsKindOf(RUNTIME_CLASS(CGridCellCombo)))
			{
				m_wndGridCtrl.SetCellType(iRow , 2 , RUNTIME_CLASS(CGridCellCombo));
			}
			pCell = (CGridCellCombo*)m_wndGridCtrl.GetCell(iRow , 2);
			pCell->SetOptions(oValueArray);
		}
		else
		{
			m_wndGridCtrl.SetCellType(iRow , 2 , RUNTIME_CLASS(CGridCell));
		}
		m_wndGridCtrl.GetCell(iRow , 2)->SetText(pAnnoFilter->GetValue());

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

int CAnnoFilterDlg::UpdateContents()
{
	int iRow = 1;
	m_wndGridCtrl.SetRowCount(m_pAnnoFilters->size()+1);
	for(AnnoFilters::const_iterator itr = m_pAnnoFilters->begin();itr != m_pAnnoFilters->end();++itr)
	{
		m_wndGridCtrl.SetRowHeight(iRow , DEFAULT_GRID_ROW_HEIGHT);	
		this->FillCellsWithFilter(iRow++,(*itr));
	}

	return ERROR_SUCCESS;
}

void CAnnoFilterDlg::OnBnClickedButtonNewFilter()
{
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	CAnnoFilter* pAnnoFilter = new CAnnoFilter(m_wndGridCtrl.GetRowCount(),_T(""),_T(""),_T(""));
	m_pAnnoFilters->push_back(pAnnoFilter);

	m_wndGridCtrl.SetRowCount(m_wndGridCtrl.GetRowCount()+1);
	const int iRow = m_wndGridCtrl.GetRowCount()-1;
	m_wndGridCtrl.SetRowHeight(iRow , DEFAULT_GRID_ROW_HEIGHT);	

	this->FillCellsWithFilter(iRow,pAnnoFilter);
}
 
/******************************************************************************
    @brief		set enable when button set checked, otherwise set disable
	@author     humkyung
    @date       2016.03.18
    @return     BOOL
******************************************************************************/
BOOL CAnnoFilterDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    if(wParam == m_wndGridCtrl.GetDlgCtrlID())
	{
		CSmartDrawDoc* pDoc = GetMDIActiveDocument();

		NM_GRIDVIEW* pGridView = (NM_GRIDVIEW*)(lParam);
		if((GVN_ENDLABELEDIT == pGridView->hdr.code) && (0 == pGridView->iColumn))
		{
			AnnoFilters::iterator where = find_if(m_pAnnoFilters->begin(),m_pAnnoFilters->end(),[=](CAnnoFilter* param){ return param->GetSeq() == pGridView->iRow;});
			if(where != m_pAnnoFilters->end())
			{
				CGridCellCombo* pCell = (CGridCellCombo*)m_wndGridCtrl.GetCell(pGridView->iRow, pGridView->iColumn);
				const STRING_T sProp = pCell->GetText();
				(*where)->SetProperty(sProp);

				this->FillCellsWithFilter(pGridView->iRow,*where);
				m_wndGridCtrl.RedrawRow(pGridView->iRow);
			}
		}
		else if((GVN_ENDLABELEDIT == pGridView->hdr.code) && (1 == pGridView->iColumn))
		{
			AnnoFilters::iterator where = find_if(m_pAnnoFilters->begin(),m_pAnnoFilters->end(),[=](CAnnoFilter* param){ return param->GetSeq() == pGridView->iRow;});
			if(where != m_pAnnoFilters->end())
			{
				CGridCellCombo* pCell = (CGridCellCombo*)m_wndGridCtrl.GetCell(pGridView->iRow, pGridView->iColumn);
				const STRING_T sOper = pCell->GetText();
				(*where)->SetOperator(sOper);

				this->FillCellsWithFilter(pGridView->iRow,*where);
				m_wndGridCtrl.RedrawRow(pGridView->iRow);
			}
		}
		else if((GVN_ENDLABELEDIT == pGridView->hdr.code) && (2 == pGridView->iColumn))
		{
			AnnoFilters::iterator where = find_if(m_pAnnoFilters->begin(),m_pAnnoFilters->end(),[=](CAnnoFilter* param){ return param->GetSeq() == pGridView->iRow;});
			if(where != m_pAnnoFilters->end())
			{
				CGridCellCombo* pCell = (CGridCellCombo*)m_wndGridCtrl.GetCell(pGridView->iRow, pGridView->iColumn);
				const STRING_T sValue = pCell->GetText();
				(*where)->SetValue(sValue);
			}
		}
	}

	return CDialog::OnNotify(wParam , lParam , pResult);
}

/******************************************************************************
	@brief
	@author     humkyung
	@date       2016-03-18
	@return     void
******************************************************************************/
void CAnnoFilterDlg::OnBnClickedButtonDeleteFilter()
{
	if(IDYES == AfxMessageBox(_T("Do you want to delete selected filter?") , MB_YESNO | MB_ICONQUESTION))
	{
		CCellRange range = m_wndGridCtrl.GetSelectedCellRange();
		const int iRow = range.GetMinRow();
		AnnoFilters::iterator where = find_if(m_pAnnoFilters->begin(),m_pAnnoFilters->end(),[=](CAnnoFilter* param){ return param->GetSeq() == iRow;});
		{
			SAFE_DELETE(*where);
			m_pAnnoFilters->erase(where);
			for(AnnoFilters::iterator itr = m_pAnnoFilters->begin();itr != m_pAnnoFilters->end();++itr)
			{
				if((*itr)->GetSeq() > iRow) (*itr)->SetSeq((*itr)->GetSeq() - 1);
			}

			m_wndGridCtrl.DeleteRow(iRow);
			m_wndGridCtrl.Invalidate();
		}
	}
}

/**
	@brief	move up selected row
	@author	humkyung
	@date	2016.03.19
*/
void CAnnoFilterDlg::OnBnClickedButtonFilterUp()
{
	CCellRange oCellRange = m_wndGridCtrl.GetSelectedCellRange();
	if(oCellRange.GetMinRow() > 1)
	{
		const int iRow = oCellRange.GetMinRow();
		AnnoFilters::iterator where = find_if(m_pAnnoFilters->begin(),m_pAnnoFilters->end(),[=](CAnnoFilter* param){ return param->GetSeq() == iRow;});
		AnnoFilters::iterator there = find_if(m_pAnnoFilters->begin(),m_pAnnoFilters->end(),[=](CAnnoFilter* param){ return param->GetSeq() == iRow-1;});
		if((where != m_pAnnoFilters->end()) && (there != m_pAnnoFilters->end()))
		{
			this->FillCellsWithFilter(iRow-1,*where);
			(*where)->SetSeq(iRow-1);
			this->FillCellsWithFilter(iRow,*there);
			(*there)->SetSeq(iRow);

			oCellRange.SetMinRow(iRow - 1);
			oCellRange.SetMaxRow(iRow - 1);
			m_wndGridCtrl.SetSelectedRange(oCellRange);
			m_wndGridCtrl.SetFocusCell(CCellID(iRow - 1 , oCellRange.GetMinCol()));

			m_wndGridCtrl.Invalidate();
		}
	}
}

/**
	@brief	move down selected row
	@author	humkyung
	@date	2016.03.19
*/
void CAnnoFilterDlg::OnBnClickedButtonFilterDown()
{
	CCellRange oCellRange = m_wndGridCtrl.GetSelectedCellRange();
	if(oCellRange.GetMinRow() < m_wndGridCtrl.GetRowCount() - 1)
	{
		const int iRow = oCellRange.GetMinRow();
		AnnoFilters::iterator where = find_if(m_pAnnoFilters->begin(),m_pAnnoFilters->end(),[=](CAnnoFilter* param){ return param->GetSeq() == iRow;});
		AnnoFilters::iterator there = find_if(m_pAnnoFilters->begin(),m_pAnnoFilters->end(),[=](CAnnoFilter* param){ return param->GetSeq() == iRow+1;});
		if((where != m_pAnnoFilters->end()) && (there != m_pAnnoFilters->end()))
		{
			this->FillCellsWithFilter(iRow+1,*where);
			(*where)->SetSeq(iRow+1);
			this->FillCellsWithFilter(iRow,*there);
			(*there)->SetSeq(iRow);

			oCellRange.SetMinRow(iRow + 1);
			oCellRange.SetMaxRow(iRow + 1);
			m_wndGridCtrl.SetSelectedRange(oCellRange);
			m_wndGridCtrl.SetFocusCell(CCellID(iRow + 1 , oCellRange.GetMinCol()));

			m_wndGridCtrl.Invalidate();
		}
	}
}
