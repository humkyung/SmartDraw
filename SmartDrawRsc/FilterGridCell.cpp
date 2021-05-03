// FilterGridCell.cpp: implementation of the CFilterGridCell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InPlaceFilterEdit.h"
#include "FilterGridCell.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CFilterGridCell, CGridCell)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilterGridCell::CFilterGridCell()
{

}

CFilterGridCell::~CFilterGridCell()
{

}

BOOL CFilterGridCell::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
	if ( m_bEditing )
	{      
		if (m_pEditWnd)
			m_pEditWnd->SendMessage ( WM_CHAR, nChar );    
	}  
	else  
	{   
		DWORD dwStyle = ES_LEFT;
		if (GetFormat() & DT_RIGHT) 
			dwStyle = ES_RIGHT;
		else if (GetFormat() & DT_CENTER) 
			dwStyle = ES_CENTER;
		
		m_bEditing = TRUE;
		
		// InPlaceEdit auto-deletes itself
		CGridCtrl* pGrid = GetGrid();
		m_pEditWnd = new CInPlaceFilterEdit((CWnd*)pGrid, rect, dwStyle, nID, nRow, nCol, GetText(), nChar);
	}
	return TRUE;
}