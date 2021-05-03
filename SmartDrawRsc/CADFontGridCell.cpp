#include "StdAfx.h"
#include <gui/GridCtrl/GridCtrl.h>
#include "CADFontGridCell.h"
#include "CADFontComboBox.h"

CString CCADFontGridCell::m_rIniFilePath;

IMPLEMENT_DYNCREATE(CCADFontGridCell, CGridCell)

CCADFontGridCell::CCADFontGridCell(void)
{
}

CCADFontGridCell::~CCADFontGridCell(void)
{
}

/**	
	@brief	Edit function

	@author	HumKyung.BAEK

	@date	2010.05.31

	@return	void	
*/
BOOL CCADFontGridCell::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
	int i=0;
	m_bEditing = TRUE;
	
	if(NULL == m_pEditWnd)
	{
		CCADFontComboBox* pComboBox = new CCADFontComboBox(CCADFontGridCell::m_rIniFilePath);
		BOOL b = pComboBox->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_OWNERDRAWFIXED | CBS_DROPDOWNLIST | CBS_HASSTRINGS , rect , (CWnd*)GetGrid() , 0x101);
		if(TRUE == b)
		{
			pComboBox->MoveWindow(rect.left, rect.top, rect.Width() , rect.Height() + 10*rect.Height());
			pComboBox->SetFont(GetGrid()->GetFont());

			const CString rCellText = GetText();
			const int nItem = pComboBox->FindString(-1 , rCellText);
			if(-1 != nItem)
				pComboBox->SetCurSel(nItem);
			else	pComboBox->SetCurSel(0);
		}
		

		m_pEditWnd = pComboBox;
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
	@brief	EndEdit function

	@author	HumKyung.BAEK

	@date	2010.05.31

	@return	void	
*/
void CCADFontGridCell::EndEdit()
{
	if (m_pEditWnd)
	{
		CCADFontComboBox* pComboBox = ((CCADFontComboBox*)m_pEditWnd);

		CString rText;
		pComboBox->GetWindowText(rText);
		const int at = rText.Find(' ');
		rText = rText.Left(at);
		SetText(rText);
		
		m_pEditWnd->ShowWindow(SW_HIDE);
	}

	m_bEditing = FALSE;
}