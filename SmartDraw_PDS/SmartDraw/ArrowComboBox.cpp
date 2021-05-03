// ArrowComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "ArrowComboBox.h"


// CArrowComboBox

IMPLEMENT_DYNAMIC(CArrowComboBox, CBitmapComboBox)

CArrowComboBox::CArrowComboBox()
{

}

CArrowComboBox::~CArrowComboBox()
{
}


BEGIN_MESSAGE_MAP(CArrowComboBox, CBitmapComboBox)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CArrowComboBox message handlers
/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.31
*/
int CArrowComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(-1 == CBitmapComboBox::OnCreate(lpCreateStruct))
		return -1;

	return 0;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.17
*/
int CArrowComboBox::FillArrowList(void)
{
	AddString(IDB_NO_ARROW , _T("NONE"));
	AddString(IDB_ARROW_1 , _T("CLOSE"));
	AddString(IDB_ARROW_2 , _T("OPEN30"));
	AddString(IDB_ARROW_3 , _T("DOTSMALL"));
	AddString(IDB_ARROW_4 , _T("HOLLOWSMALL"));
	AddString(IDB_ARROW_5 , _T("SLASH"));

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.22
*/
void CArrowComboBox::PreSubclassWindow()
{
	FillArrowList();

	CBitmapComboBox::PreSubclassWindow();
}
