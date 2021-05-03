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

// WebColorWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ColorChooser.h"
#include "WebColorWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebColorWnd

CWebColorWnd::CWebColorWnd()
{
}

CWebColorWnd::~CWebColorWnd()
{
}


BEGIN_MESSAGE_MAP(CWebColorWnd, CListBox)
	//{{AFX_MSG_MAP(CWebColorWnd)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebColorWnd message handlers

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:		CWebColorWnd::Create
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
BOOL CWebColorWnd::Create(CPoint pt, CWnd *pWnd, UINT nID)
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
	
	nIndex = AddString(_T("Black"));
	SetItemData(nIndex, RGB(0,0,0));

	nIndex = AddString(_T("DimGray"));
	SetItemData(nIndex, RGB (105, 105, 105));

	nIndex = AddString(_T("Gray"));
	SetItemData(nIndex, RGB (200, 200, 200));

	nIndex = AddString(_T("DarkGray"));
	SetItemData(nIndex, RGB (169, 169, 169));

	nIndex = AddString(_T("Silver"));
	SetItemData(nIndex, RGB (192, 192, 192));

	nIndex = AddString(_T("LightGray"));
	SetItemData(nIndex, RGB (211, 211, 211));

	nIndex = AddString(_T("Gainsboro"));
	SetItemData(nIndex, RGB (220, 220, 220));
	
	nIndex = AddString(_T("WhiteSmoke"));
	SetItemData(nIndex, RGB (245, 245, 245));

	nIndex = AddString(_T("White"));
	SetItemData(nIndex, RGB (255, 255, 255));

///////////////////////////////////////////////////

	nIndex = AddString(_T("RosyBrown"));
	SetItemData(nIndex, RGB (188, 143, 143));

	nIndex = AddString(_T("IndianRed"));
	SetItemData(nIndex, RGB (205, 92, 92));
	
	nIndex = AddString(_T("Brown"));
	SetItemData(nIndex, RGB (165, 42, 42));

	nIndex = AddString(_T("Firebrick"));
	SetItemData(nIndex, RGB (178, 34, 34));

	nIndex = AddString(_T("LightCoral"));
	SetItemData(nIndex, RGB (240, 128, 128));

	nIndex = AddString(_T("Maroon"));
	SetItemData(nIndex, RGB (128, 0, 0));
	
	nIndex = AddString(_T("DarkRed"));
	SetItemData(nIndex, RGB (139, 0, 0));

	nIndex = AddString(_T("Red"));
	SetItemData(nIndex, RGB (255, 0, 0));

	nIndex = AddString(_T("Snow"));
	SetItemData(nIndex, RGB (255, 250, 250));

	nIndex = AddString(_T("MistyRose"));
	SetItemData(nIndex, RGB (255, 228, 225));
	
	nIndex = AddString(_T("Salmon"));
	SetItemData(nIndex, RGB (250, 128, 114));

	nIndex = AddString(_T("Tomato"));
	SetItemData(nIndex, RGB (255, 99, 71));

	nIndex = AddString(_T("DarkSalmon"));
	SetItemData(nIndex, RGB (233, 150, 122));

	nIndex = AddString(_T("Coral"));
	SetItemData(nIndex, RGB (255, 127, 80));
	
	nIndex = AddString(_T("OrangeRed"));
	SetItemData(nIndex, RGB (255, 69, 0));
	
	nIndex = AddString(_T("LightSalmon"));
	SetItemData(nIndex, RGB (255, 160, 122));
	
	nIndex = AddString(_T("Sienna"));
	SetItemData(nIndex, RGB (160, 82, 45));
	
	nIndex = AddString(_T("SeaShell"));
	SetItemData(nIndex, RGB (255, 245, 238));

	nIndex = AddString(_T("Chocolate"));
	SetItemData(nIndex, RGB (210,105,30));

	nIndex = AddString(_T("SaddleBrown"));
	SetItemData(nIndex, RGB (139,69,19));

	nIndex = AddString(_T("SandyBrown"));
	SetItemData(nIndex, RGB (244,164,96));

	nIndex = AddString(_T("PeachPuff"));
	SetItemData(nIndex, RGB (255,218,185));

	nIndex = AddString(_T("Peru"));
	SetItemData(nIndex, RGB (205,133,63));

	nIndex = AddString(_T("Linen"));
	SetItemData(nIndex, RGB (250,240,230));

	nIndex = AddString(_T("Bisque"));
	SetItemData(nIndex, RGB (255,228,196));

	nIndex = AddString(_T("DarkOrange"));
	SetItemData(nIndex, RGB (255,140,0));

	nIndex = AddString(_T("BurlyWood"));
	SetItemData(nIndex, RGB (222,184,135));

	nIndex = AddString(_T("Tan"));
	SetItemData(nIndex, RGB (210,180,140));

	nIndex = AddString(_T("AntiqueWhite"));
	SetItemData(nIndex, RGB (250,235,215));

	nIndex = AddString(_T("NavajoWhite"));
	SetItemData(nIndex, RGB (255,222,173));

	nIndex = AddString(_T("BlanchedAlmond"));
	SetItemData(nIndex, RGB (255,235,205));

	nIndex = AddString(_T("PapayaWhip"));
	SetItemData(nIndex, RGB (255,239,213));

	nIndex = AddString(_T("Moccasin"));
	SetItemData(nIndex, RGB (255,228,181));

	nIndex = AddString(_T("Orange"));
	SetItemData(nIndex, RGB (255,165,0));

	nIndex = AddString(_T("Wheat"));
	SetItemData(nIndex, RGB (245,222,179));

	nIndex = AddString(_T("OldLace"));
	SetItemData(nIndex, RGB (253,245,230));

	nIndex = AddString(_T("FloraWhite"));
	SetItemData(nIndex, RGB (255,250,240));

	nIndex = AddString(_T("DarkGoldenrod"));
	SetItemData(nIndex, RGB (184,134,11));

	nIndex = AddString(_T("Goldenrod"));
	SetItemData(nIndex, RGB (218,165,32));

	nIndex = AddString(_T("CornSilk"));
	SetItemData(nIndex, RGB (255,248,220));
	
	nIndex = AddString(_T("Gold"));
	SetItemData(nIndex, RGB (255,215,0));

	nIndex = AddString(_T("Khaki"));
	SetItemData(nIndex, RGB (240,230,140));

	nIndex = AddString(_T("LemonChaffon"));
	SetItemData(nIndex, RGB (255,250,205));
	
	nIndex = AddString(_T("PaleGoldenrod"));
	SetItemData(nIndex, RGB (238,232,170));

	nIndex = AddString(_T("DarkKhaki"));
	SetItemData(nIndex, RGB (189,183,107));

	nIndex = AddString(_T("Beige"));
	SetItemData(nIndex, RGB (245,245,220));

	nIndex = AddString(_T("LightGoldenrod"));
	SetItemData(nIndex, RGB (250,250,210));

	nIndex = AddString(_T("Olive"));
	SetItemData(nIndex, RGB (128,128,0));

	nIndex = AddString(_T("Yellow"));
	SetItemData(nIndex, RGB (255,255,0));

	nIndex = AddString(_T("LightYellow"));
	SetItemData(nIndex, RGB (255,255,224));

	nIndex = AddString(_T("Ivory"));
	SetItemData(nIndex, RGB (255,255,240));

	nIndex = AddString(_T("OliveDrab"));
	SetItemData(nIndex, RGB (107,142,35));

	nIndex = AddString(_T("YellowGreen"));
	SetItemData(nIndex, RGB (154,205,50));

	nIndex = AddString(_T("DarkOliveGreen"));
	SetItemData(nIndex, RGB (85,107,47));

	nIndex = AddString(_T("GreenYellow"));
	SetItemData(nIndex, RGB (173,255,47));
	
	nIndex = AddString(_T("Chartreuse"));
	SetItemData(nIndex, RGB (127,255,0));

	nIndex = AddString(_T("LawnGreen"));
	SetItemData(nIndex, RGB (124,252,0));

	nIndex = AddString(_T("DarkSeaGreen"));
	SetItemData(nIndex, RGB (143,188,143));

	nIndex = AddString(_T("ForestGreen"));
	SetItemData(nIndex, RGB (34,139,34));

	nIndex = AddString(_T("LimeGreen"));
	SetItemData(nIndex, RGB (50,205,50));

	nIndex = AddString(_T("LightGreen"));
	SetItemData(nIndex, RGB (144,238,144));
	
	nIndex = AddString(_T("PaleGreen"));
	SetItemData(nIndex, RGB (152,251,152));

	nIndex = AddString(_T("DarkGreen"));
	SetItemData(nIndex, RGB (0,100,0));

	nIndex = AddString(_T("Green"));
	SetItemData(nIndex, RGB (0,128,0));

	nIndex = AddString(_T("Lime"));
	SetItemData(nIndex, RGB (0,255,0));

	nIndex = AddString(_T("HoneyDew"));
	SetItemData(nIndex, RGB (240,255,240));

	nIndex = AddString(_T("SeaGreen"));
	SetItemData(nIndex, RGB (46,139,87));

	nIndex = AddString(_T("MediumSeaGreen"));
	SetItemData(nIndex, RGB (60,179,113));

	nIndex = AddString(_T("SpringGreen"));
	SetItemData(nIndex, RGB (0,255,127));

	nIndex = AddString(_T("MintCream"));
	SetItemData(nIndex, RGB (245,255,250));

	nIndex = AddString(_T("MediumSpringGreen"));
	SetItemData(nIndex, RGB (0,250,154));

	nIndex = AddString(_T("MediumAquamarine"));
	SetItemData(nIndex, RGB (102,205,170));

	nIndex = AddString(_T("Aquamarine"));
	SetItemData(nIndex, RGB (127,255,212));

	nIndex = AddString(_T("Turquoise"));
	SetItemData(nIndex, RGB (64,224,208));

	nIndex = AddString(_T("LightSeaGreen"));
	SetItemData(nIndex, RGB (32,178,170));

	nIndex = AddString(_T("MediumTurquoise"));
	SetItemData(nIndex, RGB (72,209,204));

	nIndex = AddString(_T("DarkSlateGray"));
	SetItemData(nIndex, RGB (47,79,79));

	nIndex = AddString(_T("PaleTurquoise"));
	SetItemData(nIndex, RGB (175,238,238));

	nIndex = AddString(_T("Teal"));
	SetItemData(nIndex, RGB (0,128,128));

	nIndex = AddString(_T("DarkCyan"));
	SetItemData(nIndex, RGB (0,139,139));

	nIndex = AddString(_T("Aqua"));
	SetItemData(nIndex, RGB (0,255,255));

	nIndex = AddString(_T("Cyan"));
	SetItemData(nIndex, RGB (0,255,255));

	nIndex = AddString(_T("LightCyan"));
	SetItemData(nIndex, RGB (224,255,255));

	nIndex = AddString(_T("Azure"));
	SetItemData(nIndex, RGB (240,255,255));

	nIndex = AddString(_T("DarkTurqoise"));
	SetItemData(nIndex, RGB (0,206,209));

	nIndex = AddString(_T("CadetBlue"));
	SetItemData(nIndex, RGB (95,158,160));

	nIndex = AddString(_T("PowderBlue"));
	SetItemData(nIndex, RGB (176,224,230));

	nIndex = AddString(_T("LightBlue"));
	SetItemData(nIndex, RGB (173,216,230));

	nIndex = AddString(_T("DeepSkyBlue"));
	SetItemData(nIndex, RGB (0,191,255));

	nIndex = AddString(_T("SkyBlue"));
	SetItemData(nIndex, RGB (135,206,235));

	nIndex = AddString(_T("LightSkyBlue"));
	SetItemData(nIndex, RGB (135,206,250));

	nIndex = AddString(_T("SteelBlue"));
	SetItemData(nIndex, RGB (70,130,180));

	nIndex = AddString(_T("AliceBlue"));
	SetItemData(nIndex, RGB (240,248,255));
	
	nIndex = AddString(_T("DodgerBlue"));
	SetItemData(nIndex, RGB (30,144,255));

	nIndex = AddString(_T("SlateGray"));
	SetItemData(nIndex, RGB (112,128,144));

	nIndex = AddString(_T("LightSlateGray"));
	SetItemData(nIndex, RGB (119,136,153));

	nIndex = AddString(_T("LightSteelBlue"));
	SetItemData(nIndex, RGB (176,196,222));

	nIndex = AddString(_T("CornflowerBlue"));
	SetItemData(nIndex, RGB (100,149,237));

	nIndex = AddString(_T("RoyalBlue"));
	SetItemData(nIndex, RGB (65,105,225));

	nIndex = AddString(_T("MidnightBlue"));
	SetItemData(nIndex, RGB (25,25,112));

	nIndex = AddString(_T("Lavender"));
	SetItemData(nIndex, RGB (230,230,250));

	nIndex = AddString(_T("Navy"));
	SetItemData(nIndex, RGB (0,0,128));

	nIndex = AddString(_T("DarkBlue"));
	SetItemData(nIndex, RGB (0,0,139));

	nIndex = AddString(_T("MediumBlue"));
	SetItemData(nIndex, RGB (0,0,205));

	nIndex = AddString(_T("Blue"));
	SetItemData(nIndex, RGB (0,0,255));

	nIndex = AddString(_T("GhostWhite"));
	SetItemData(nIndex, RGB (248,248,255));

	nIndex = AddString(_T("SlateBlue"));
	SetItemData(nIndex, RGB (106,90,205));

	nIndex = AddString(_T("DarkSlateBlue"));
	SetItemData(nIndex, RGB (72,61,139));

	nIndex = AddString(_T("MediumBlue"));
	SetItemData(nIndex, RGB (0,0,205));

	nIndex = AddString(_T("MediumSlateBlue"));
	SetItemData(nIndex, RGB (123,104,238));

	nIndex = AddString(_T("MediumPurple"));
	SetItemData(nIndex, RGB (147,112,219));

	nIndex = AddString(_T("BlueVoilet"));
	SetItemData(nIndex, RGB (138,43,226));

	nIndex = AddString(_T("Indigo"));
	SetItemData(nIndex, RGB (75,0,130));

	nIndex = AddString(_T("DarkOrchid"));
	SetItemData(nIndex, RGB (153,50,204));

	nIndex = AddString(_T("DarkViolet"));
	SetItemData(nIndex, RGB (148,0,211));

	nIndex = AddString(_T("MediumOrchid"));
	SetItemData(nIndex, RGB (186,85,211));

	nIndex = AddString(_T("Thistle"));
	SetItemData(nIndex, RGB (216,191,216));

	nIndex = AddString(_T("Plum"));
	SetItemData(nIndex, RGB (221,160,221));

	nIndex = AddString(_T("Violet"));
	SetItemData(nIndex, RGB (238,130,238));

	nIndex = AddString(_T("Purple"));
	SetItemData(nIndex, RGB (128,0,128));

	nIndex = AddString(_T("DarkMagenta"));
	SetItemData(nIndex, RGB (139,0,139));

	nIndex = AddString(_T("Magenta"));
	SetItemData(nIndex, RGB (255,0,255));

	nIndex = AddString(_T("Fuchsia"));
	SetItemData(nIndex, RGB (255,0,255));

	nIndex = AddString(_T("Orchid"));
	SetItemData(nIndex, RGB (218,112,214));

	nIndex = AddString(_T("MeduimVioletRed"));
	SetItemData(nIndex, RGB (199,21,133));

	nIndex = AddString(_T("DeepPink"));
	SetItemData(nIndex, RGB (255,20,147));

	nIndex = AddString(_T("HotPink"));
	SetItemData(nIndex, RGB (255,105,180));

	nIndex = AddString(_T("LavenderBush"));
	SetItemData(nIndex, RGB (255,240,245));

	nIndex = AddString(_T("PaleVioletRed"));
	SetItemData(nIndex, RGB (218,112,214));

	nIndex = AddString(_T("Crimson"));
	SetItemData(nIndex, RGB (220,20,60));

	nIndex = AddString(_T("Pink"));
	SetItemData(nIndex, RGB (255,192,203));

	nIndex = AddString(_T("LightPink"));
	SetItemData(nIndex, RGB (255,182,193));

	return TRUE;
}

void CWebColorWnd::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// Required Override	
}

void CWebColorWnd::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct) 
{
	// Required Override	
	CListBox::DeleteItem(lpDeleteItemStruct);
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:		CWebColorWnd::DrawItem
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
void CWebColorWnd::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
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

	 	 COLORREF crColor  = GetItemData(lpDrawItemStruct->itemID);

		 HBRUSH hBr = CreateSolidBrush(crColor);
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
// FUNCTION:		CWebColorWnd::OnSelchange
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
void CWebColorWnd::OnSelchange() 
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
	cn.color |= WEB_COLOR_MASK;

	GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM) &cn); 
	
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:		CWebColorWnd::SetCurrentColor
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
void  CWebColorWnd::SetCurrentColor(COLORREF crCurrent)
{
	// Mask off system color flag
	crCurrent &= ~WEB_COLOR_MASK;


	int nCnt = GetCount();
	for (int n=0; n < nCnt; n++)
	{
		if (GetItemData(n) == crCurrent)
		{
			SetCurSel(n);
			break;
		}	
	}
}