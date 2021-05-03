//----------------------------------------------------------------------------
// N O L D U S   I N F O R M A T I O N   T E C H N O L O G Y   B . V .
//----------------------------------------------------------------------------
// Filename:      BitmapPickerCombo.cpp
// Project:       EthoVision
// Module:        BitmapPicker
// Programmer:    Anneke Sicherer-Roetman
// Version:       1.00
// Revision Date: 06-10-1999
//----------------------------------------------------------------------------
// Description:   Definition of class CMyBitmapComboBox
//                See CMyBitmapComboBox.h
//----------------------------------------------------------------------------
// Acknowledgements: based on Joel Wahlberg's CIconComboBox
//                  (joel.wahlberg@enator.se)
//----------------------------------------------------------------------------
// Revision history:
// 06-10-1999 - First implementation
//----------------------------------------------------------------------------
// Bugs: ........
//----------------------------------------------------------------------------
// @doc
//----------------------------------------------------------------------------

#include "stdafx.h"
#include <assert.h>
#include "MyBitmapComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
                   
//----------------------------------------------------------------------------
// Function DrawBitmap
// @func    draws bitmap at specified point in specified device context
// @rdesc   nothing
// @parm    const CBitmap | *bitmap | bitmap to draw
// @parm    const CDC     | *pDC    | device context to draw in
// @parm    const CPoint  | &point  | top left point of bitmap
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 06-10-1999 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
static void DrawBitmap(const CBitmap *bitmap, const CDC *pDC, const CPoint &point)
{
	BITMAP bm; ((CBitmap*)bitmap)->GetBitmap(&bm);
	int w = bm.bmWidth; 
	int h = bm.bmHeight;
	CDC memDC; memDC.CreateCompatibleDC((CDC*)pDC);
	CBitmap *pBmp = memDC.SelectObject((CBitmap*)bitmap);
	((CDC*)pDC)->BitBlt(point.x, point.y, w, h, &memDC, 0, 0, MERGECOPY);
	memDC.SelectObject(pBmp);
}

//----------------------------------------------------------------------------
// Function DrawBitmap
// @func    draws bitmap centered in specified rectangle in specified device context
// @rdesc   nothing
// @parm    const CBitmap | *bitmap | bitmap to draw
// @parm    const CDC     | *pDC    | device context to draw in
// @parm    const CRect   | &rect   | rectangle to center in
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 06-10-1999 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
static void DrawBitmap(const CBitmap *bitmap, const CDC *pDC, const CRect &rect)
{
	BITMAP bm; ((CBitmap*)bitmap)->GetBitmap(&bm);
	int w = bm.bmWidth; 
	int h = bm.bmHeight;
	CPoint point;
	point.x = rect.left + ((rect.right - rect.left) / 2) - (w / 2);
	point.y = rect.top + ((rect.bottom - rect.top) / 2) - (h / 2);
	DrawBitmap(bitmap, pDC, point);
}

//----------------------------------------------------------------------------
// Function CMyBitmapComboBox::CMyBitmapComboBox
// @mfunc   constructor
// @xref    <c CMyBitmapComboBox>
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 06-10-1999 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
CMyBitmapComboBox::CMyBitmapComboBox():
CComboBox(),
m_nItemWidth(0) , m_nItemHeight(0)
{
}

BEGIN_MESSAGE_MAP(CMyBitmapComboBox, CComboBox)
//{{AFX_MSG_MAP(CBitComboBox)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------------------
// Function CMyBitmapComboBox::AddBitmap
// @mfunc   adds bitmap (and string) item to combobox
// @rdesc   index of item (-1 on failure) (int)
// @parm    const CBitmap | *bitmap | bitmap to add
// @parm    const char    | *string | string to add (default NULL)
// @xref    <c CMyBitmapComboBox>
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman
// @revs 
// 06-10-1999 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
int CMyBitmapComboBox::AddString(UINT id, const TCHAR *string)
{
	assert(string && "string is NULL");

	if(string)
	{
		
		int nItem = CComboBox::InsertString(GetCount() , string);
		if (nItem != CB_ERR && nItem != CB_ERRSPACE) 
		{
			SetItemData(nItem , id);
		}

		return nItem;
	}

	return -1;
}

//----------------------------------------------------------------------------
// Function CMyBitmapComboBox::DrawItem
// @mfunc   Called by MFC when visual aspect of combo box changes 
// @rdesc   nothing
// @parm    LPDRAWITEMSTRUCT | lpDIS | standard parameter
// @xref    <c CMyBitmapComboBox>
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman <nl>
// after: Icon Picker Combo Box by Joel Wahlberg <nl>
// http://www.codeguru.com/combobox/icon_combobox.shtml
// @revs 
// 06-10-1999 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
void CMyBitmapComboBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	
	if (!IsWindowEnabled()) 
	{
		CBrush brDisabled(RGB(192,192,192)); // light gray
		CBrush* pOldBrush = pDC->SelectObject(&brDisabled);
		CPen penDisabled(PS_SOLID, 1, RGB(192,192,192));
		CPen* pOldPen = pDC->SelectObject(&penDisabled);
		OutputBitmap(lpDIS, false);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
		return;
	}
	
	// Selected
	if ((lpDIS->itemState & ODS_SELECTED) 
		&& (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))) 
	{
		CBrush brHighlight(::GetSysColor(COLOR_HIGHLIGHT)); 
		CBrush* pOldBrush = pDC->SelectObject(&brHighlight);
		CPen penHighlight(PS_SOLID, 1, ::GetSysColor(COLOR_HIGHLIGHT));
		CPen* pOldPen = pDC->SelectObject(&penHighlight);
		pDC->Rectangle(&lpDIS->rcItem);
		pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		OutputBitmap(lpDIS, true);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}
	
	// De-Selected
	if (!(lpDIS->itemState & ODS_SELECTED) 
		&& (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))) 
	{
		CBrush brWindow(::GetSysColor(COLOR_WINDOW)); 
		CBrush* pOldBrush = pDC->SelectObject(&brWindow);
		CPen penHighlight(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOW));
		CPen* pOldPen = pDC->SelectObject(&penHighlight);
		pDC->Rectangle(&lpDIS->rcItem);
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		OutputBitmap(lpDIS, false);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}
	
	// Focus
	if (lpDIS->itemAction & ODA_FOCUS) 
		pDC->DrawFocusRect(&lpDIS->rcItem);
}

void CMyBitmapComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{ 
	lpMIS->itemWidth = (m_nItemWidth);
	lpMIS->itemHeight = (m_nItemHeight);
}

//----------------------------------------------------------------------------
// Function CMyBitmapComboBox::OutputBitmap
// @mfunc   draws bitmap (and string) in item
// @rdesc   nothing
// @parm    LPDRAWITEMSTRUCT | lpDIS    | item data
// @parm    bool             | selected | is the item selected?
// @xref    <c CMyBitmapComboBox>
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman <nl>
// after: Icon Picker Combo Box by Joel Wahlberg <nl>
// http://www.codeguru.com/combobox/icon_combobox.shtml
// @revs 
// 06-10-1999 - First implementation
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------
void CMyBitmapComboBox::OutputBitmap(LPDRAWITEMSTRUCT lpDIS, bool selected)
{
	const CBitmap *bitmap = (const CBitmap*)(lpDIS->itemData);
	if (bitmap && bitmap != (const CBitmap *)(0xffffffff)) 
	{
		CDC* pDC = CDC::FromHandle(lpDIS->hDC);
		CString string; 
		if (lpDIS->itemID != -1) 
			GetLBText(lpDIS->itemID, string); 

		CBitmap bitmap;
		BITMAP bitImage;
		
		const UINT nIDResource = (UINT)(GetItemData(lpDIS->itemID));
		bitmap.LoadBitmap(nIDResource);
		bitmap.GetBitmap(&bitImage);
		if (string.IsEmpty()) 
		{
			DrawBitmap(&bitmap, pDC, lpDIS->rcItem);
		}
		else 
		{
			CPoint point;
			point.x = lpDIS->rcItem.left;
			point.y = lpDIS->rcItem.top;
			
			DrawBitmap(&bitmap, pDC, point);
			CRect rcText(lpDIS->rcItem); 
			rcText.DeflateRect(bitImage.bmWidth + 2, 0, 0, 0);
			pDC->DrawText(string , rcText, DT_SINGLELINE |DT_VCENTER ); 
		}
	}
}               

//----------------------------------------------------------------------------

#ifdef _DEBUG
void CMyBitmapComboBox::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();
	
	// ensure some styles are set
	// modifying style here has NO effect!?!
	ASSERT(GetStyle() & CBS_DROPDOWNLIST);
	ASSERT(GetStyle() & CBS_OWNERDRAWFIXED);
	ASSERT(GetStyle() & CBS_HASSTRINGS);
}
#endif

//----------------------------------------------------------------------------
