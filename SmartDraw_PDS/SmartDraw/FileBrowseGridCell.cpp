#include "stdafx.h"
#include "Resource.h"
#include "FileBrowseGridCell.h"

IMPLEMENT_DYNCREATE(CBrowseButton , CMFCButton)

BEGIN_MESSAGE_MAP(CBrowseButton, CMFCButton)
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
END_MESSAGE_MAP()

/**
	@brief	OnCreate

	@author HumKyung.BAEK	

	@date	2010.07.03

	@return		
**/
int CBrowseButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDI_FOLDER),
		IMAGE_ICON,
		0,
		0,
		0);
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
void CBrowseButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	AfxMessageBox(_T("NULL"));
	CMFCButton::OnLButtonUp(nFlags, point);
}

IMPLEMENT_DYNCREATE(CFileBrowseGridCell, CGridCell)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileBrowseGridCell::CFileBrowseGridCell() : CGridCell()
{
}

CFileBrowseGridCell::~CFileBrowseGridCell()
{

}

BOOL CFileBrowseGridCell::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd)
{
	if(m_wndBrowseButton.GetSafeHwnd())
	{
		CRect rectPickButton(rect);
		rectPickButton.left = rectPickButton.right - rectPickButton.Height();
		m_wndBrowseButton.MoveWindow(&rectPickButton);
	}
	else
	{
		CRect rectPickButton(rect);
		rectPickButton.left = rectPickButton.right - rectPickButton.Height();

		CGridCtrl* pGrid = GetGrid();
		m_wndBrowseButton.Create(_T("...") , WS_CHILD | WS_VISIBLE , rectPickButton , (CWnd*)pGrid , 0x10);
	}

	rect.right -= rect.Height();
	return CGridCell::Draw(pDC , nRow , nCol , rect , bEraseBkgnd);
}