//----------------------------------------------------------------------------
// N O L D U S   I N F O R M A T I O N   T E C H N O L O G Y   B . V .
//----------------------------------------------------------------------------
// Filename:      BitmapPickerCombo.h
// Project:       EthoVision
// Module:        BitmapPicker
// Programmer:    Anneke Sicherer-Roetman
// Version:       1.00
// Revision Date: 06-10-1999
//----------------------------------------------------------------------------
// Description:   Declaration of class CBitmapCombo
//----------------------------------------------------------------------------
// Revision history:
// 06-10-1999 - First implementation
//----------------------------------------------------------------------------
// Bugs: ........
//----------------------------------------------------------------------------
// @doc
//----------------------------------------------------------------------------

#if !defined(AFX_CBitmapCombo_H__8AAE34F7_7B02_11D3_A615_0060085FE616__INCLUDED_)
#define AFX_CBitmapCombo_H__8AAE34F7_7B02_11D3_A615_0060085FE616__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000 

//----------------------------------------------------------------------------
// @class         CBitmapCombo |
//                combobox to select bitmaps from
// @base          public | CComboBox
//----------------------------------------------------------------------------
// @prog 
// Anneke Sicherer-Roetman <nl>
// after: Icon Picker Combo Box by Joel Wahlberg <nl>
// http://www.codeguru.com/combobox/icon_combobox.shtml
// @revs 
// 06-10-1999 - First implementation
//----------------------------------------------------------------------------
// @ex Here's how to use this control in a dialog |
// 1. Include CBitmapCombo.cpp and CBitmapCombo.h in your project
// 2. In the resource editor create a regular combobox with at least the 
//    following styles: DropList, OwnerDraw Variable, Has Strings 
// 3. Create a control member variable for the combobox in VC's classwizard
// 4. Replace CComboBox with CBitmapCombo in your dialog's .h file
// 5. In OnInitDialog use the AddBitmap or InsertBitmap member functions
//    to add bitmaps (and strings) to the combobox
//----------------------------------------------------------------------------
// @todo 
//----------------------------------------------------------------------------

class CBitmapComboBox : public CComboBox
{
	// @access Public Member Functions and Variables
public:
	
	// @cmember
	// constructor
	CBitmapComboBox();
	
	// @cmember,mfunc
	// destructor
	virtual ~CBitmapComboBox() {}
	
	int AddString(UINT id , const TCHAR *string = NULL);
	
	// @cmember
	// adds bitmap (and string) item to combobox at specified index
	
	// @access Protected Member Functions and Variables
protected:
	
	// @cmember
	// Called by MFC when visual aspect of combo box changes 
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	// @cmember
	// Called by MFC when combo box is created
	//{{AFX_MSG(CBitComboBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#ifdef _DEBUG
		// @cmember,mfunc
		// for assertion only
		virtual void PreSubclassWindow();
#endif
protected:	
	int m_nItemWidth, m_nItemHeight; // @cmember size of items
	// @access Private Member Functions and Variables
private:
	// @cmember
	// draws bitmap (and string) in item
	void OutputBitmap(LPDRAWITEMSTRUCT lpDIS, bool selected);
	
	// @cmember
	// sets overall item size
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CBitmapCombo_H__8AAE34F7_7B02_11D3_A615_0060085FE616__INCLUDED_)
