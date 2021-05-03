/////////////////////////////////////////////////////////////////////////////
//
// Disclaimer Notice
// ------------------
// The Author cannot guarantee this software is error free.
// The author also disclaims all responsibility for damages or 
// loss of profits caused by the failure of this software
//. Your usage of these modifications indicates your willingness to 
// accept complete responsibility.
//
//
// Developed by: Norm Almond for www.codeproject.com 
//
// Please use this software freely, if you fix or make any modifications to
// the software you can let me know so I can improve future versions.
//
// Version 1.00 - First Released 03 March 2001
//
/////////////////////////////////////////////////////////////////////////////

// SysColorWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ColorChooser.h"
#include "SysColorWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSysColorWnd

CSysColorWnd::CSysColorWnd()
{
}

CSysColorWnd::~CSysColorWnd()
{
}


BEGIN_MESSAGE_MAP(CSysColorWnd, CListBox)
	//{{AFX_MSG_MAP(CSysColorWnd)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysColorWnd message handlers

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:		CSysColorWnd::Create
// 
// DESCRIPTION:		Window creaation function
// 
// ACCESSIBILTY:	Public
// 
// MODIFICATIONS:
// Name                  Date       Version    Comments
// N T ALMOND            03MAR2001  1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
BOOL CSysColorWnd::Create(CPoint pt, CWnd *pWnd, UINT nID)
{
	// Style for listbox
	DWORD dwStyle = WS_VSCROLL | 
		WS_BORDER |
		WS_TABSTOP | 
		WS_CHILD | 
		LBS_OWNERDRAWFIXED | 
		LBS_HASSTRINGS | 
		LBS_NOTIFY | 
		LBS_WANTKEYBOARDINPUT |
		LBS_NOINTEGRALHEIGHT;


	if (!CListBox::Create(dwStyle, CRect(pt.x, pt.y, pt.x, pt.y), pWnd, nID))
		return FALSE;


	// Now add listbox items
	int nIndex;
	
	nIndex = AddString(_T("Scroll Bars"));
	SetItemData(nIndex, COLOR_SCROLLBAR);

	nIndex = AddString(_T("Desktop"));
	SetItemData(nIndex, COLOR_DESKTOP);

	nIndex = AddString(_T("Active Title Bar"));
	SetItemData(nIndex, COLOR_ACTIVECAPTION);

	nIndex = AddString(_T("Inactive Title Bar"));
	SetItemData(nIndex, COLOR_INACTIVECAPTION);

	nIndex = AddString(_T("Menu Bar"));
	SetItemData(nIndex, COLOR_MENU);

	nIndex = AddString(_T("Window Background"));
	SetItemData(nIndex, COLOR_WINDOW);

	nIndex = AddString(_T("Window Frame"));
	SetItemData(nIndex, COLOR_WINDOWFRAME);

	nIndex = AddString(_T("Menu Text"));
	SetItemData(nIndex, COLOR_MENUTEXT);

	nIndex = AddString(_T("Window Text"));
	SetItemData(nIndex, COLOR_WINDOWTEXT);

	nIndex = AddString(_T("Active Title Bar Text"));
	SetItemData(nIndex, COLOR_CAPTIONTEXT);

	nIndex = AddString(_T("Active Border"));
	SetItemData(nIndex, COLOR_ACTIVEBORDER);

	nIndex = AddString(_T("Inactive Border"));
	SetItemData(nIndex, COLOR_INACTIVEBORDER);

	nIndex = AddString(_T("Application Workspace"));
	SetItemData(nIndex, COLOR_APPWORKSPACE);

	nIndex = AddString(_T("Highlight"));
	SetItemData(nIndex, COLOR_HIGHLIGHT);
	
	nIndex = AddString(_T("Highlight Text"));
	SetItemData(nIndex, COLOR_HIGHLIGHTTEXT);

	nIndex = AddString(_T("Button Face"));
	SetItemData(nIndex, COLOR_3DFACE);

	nIndex = AddString(_T("Button Shadow"));
	SetItemData(nIndex, COLOR_BTNSHADOW);

	nIndex = AddString(_T("Disabled Text"));
	SetItemData(nIndex, COLOR_GRAYTEXT);
	
	nIndex = AddString(_T("Button Text"));
	SetItemData(nIndex, COLOR_BTNTEXT);
	
	nIndex = AddString(_T("Inactive Title Text"));
	SetItemData(nIndex, COLOR_INACTIVECAPTIONTEXT);
	
	nIndex = AddString(_T("Button Highlight"));
	SetItemData(nIndex, COLOR_3DHIGHLIGHT);

	nIndex = AddString(_T("Button Dark Shadow"));
	SetItemData(nIndex, COLOR_3DDKSHADOW);

	nIndex = AddString(_T("Button Light Shadow"));
	SetItemData(nIndex, COLOR_3DLIGHT);

	nIndex = AddString(_T("Tooltip Text"));
	SetItemData(nIndex, COLOR_INFOTEXT);

	nIndex = AddString(_T("Tooltip"));
	SetItemData(nIndex, COLOR_INFOBK);

	return TRUE;
}

void CSysColorWnd::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// Required Override	
}

void CSysColorWnd::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct) 
{
	// Required Override	
	CListBox::DeleteItem(lpDeleteItemStruct);
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:		CSysColorWnd::DrawItem
// 
// DESCRIPTION:		Ownerdrawn listbox routine
// 
// ACCESSIBILTY:	Public
// 
// MODIFICATIONS:
// Name                  Date       Version    Comments
// N T ALMOND            03MAR2001  1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
void CSysColorWnd::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   if ( lpDrawItemStruct->itemID == -1 )
      return; 
   
	CString  strItemText;
	CRect    rcItem( lpDrawItemStruct->rcItem );

	COLORREF clrItemText, clrOldTextColor;

	CFont* pFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));

	HFONT hOldFont = (HFONT)::SelectObject(lpDrawItemStruct->hDC, (HFONT)GetStockObject(DEFAULT_GUI_FONT));

	CRect rcColor = rcItem;
	rcColor.right = rcItem.Height();
	rcColor.DeflateRect(1,1);
	rcColor.OffsetRect(1,0);

	CRect rcText = rcItem;
	rcText.left = rcColor.right + 1;
   
   // Act upon the item state
   SetBkMode( lpDrawItemStruct->hDC, TRANSPARENT );
   
	switch ( lpDrawItemStruct->itemAction )
    { 
      case ODA_SELECT: 
      case ODA_DRAWENTIRE: 
      
         // Is the item selected? 
         if ( lpDrawItemStruct->itemState & ODS_SELECTED ) 
         { 
            clrItemText = GetSysColor( COLOR_HIGHLIGHTTEXT );
            // Clear the rectangle
            FillRect(  lpDrawItemStruct->hDC, 
                      &rcItem, 
                       (HBRUSH)(COLOR_ACTIVECAPTION+1) );
         } 
         else
         { 
            clrItemText = GetSysColor( COLOR_WINDOWTEXT );
            // Clear the rectangle
            FillRect(  lpDrawItemStruct->hDC, 
                      &rcItem, 
                       (HBRUSH)(COLOR_WINDOW+1) );
         } 

		 HPEN hOldPen = (HPEN) SelectObject(lpDrawItemStruct->hDC,GetStockObject(BLACK_PEN));

	 	 INT nIndex  = GetItemData(lpDrawItemStruct->itemID);

		 HBRUSH hBr = CreateSolidBrush(GetSysColor(nIndex));
		 HBRUSH hOldBr = (HBRUSH) SelectObject(lpDrawItemStruct->hDC,hBr);

		 Rectangle(lpDrawItemStruct->hDC, rcColor.left, rcColor.top, rcColor.right, rcColor.bottom);

		 SelectObject(lpDrawItemStruct->hDC, hOldPen);
		 SelectObject(lpDrawItemStruct->hDC, hOldBr);
		 ::DeleteObject(hBr);


         clrOldTextColor = SetTextColor( lpDrawItemStruct->hDC,
			                                clrItemText );

		 CString strItem;
		 GetText(lpDrawItemStruct->itemID, strItem);
            DrawText(  lpDrawItemStruct->hDC,
                       strItem,
                       strItem.GetLength(),
                      &rcText,
                       DT_LEFT | DT_SINGLELINE | DT_VCENTER );


         break; 
   
   }

	::SelectObject(lpDrawItemStruct->hDC, hOldFont);
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:		CSysColorWnd::OnSelchange
// 
// DESCRIPTION:		Called when selection changed
// 
// ACCESSIBILTY:	Public
// 
// MODIFICATIONS:
// Name                  Date       Version    Comments
// N T ALMOND            03MAR2001  1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
void CSysColorWnd::OnSelchange() 
{
	int nSel = GetCurSel();
	if (nSel == LB_ERR)
		return;

	COLOR_NOTIFY cn;

    cn.hdr.hwndFrom = GetSafeHwnd();
    cn.hdr.idFrom   = GetDlgCtrlID();
    cn.hdr.code     = COLOR_CHANGED;
	cn.color = GetItemData(nSel);

	// Set System Color Indicator
	cn.color |= SYSTEM_COLOR_MASK;

	GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM) &cn); 
	
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:		CSysColorWnd::SetCurrentColor
// 
// DESCRIPTION:		Sets current Color
// 
// ACCESSIBILTY:	Public
// 
// MODIFICATIONS:
// Name                  Date       Version    Comments
// N T ALMOND            03MAR2001  1.0        Origin
// 
////////////////////////////////////////////////////////////////////////////////
void  CSysColorWnd::SetCurrentColor(COLORREF crCurrent)
{
	// Mask off system color flag
	crCurrent &= ~SYSTEM_COLOR_MASK;
	SetCurSel(crCurrent);
}