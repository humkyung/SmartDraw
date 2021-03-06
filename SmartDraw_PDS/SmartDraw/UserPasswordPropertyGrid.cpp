// UserPasswordPropertyGrid.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "UserPasswordPropertyGrid.h"


// CUserPasswordPropertyGrid

CUserPasswordPropertyGrid::CUserPasswordPropertyGrid(const CString& strName, const _variant_t& varValue,
                                   LPCTSTR lpszDescr, DWORD_PTR dwData, LPCTSTR lpszEditMask,
								   LPCTSTR lpszEditTemplate, LPCTSTR lpszValidChars) :
CMFCPropertyGridProperty(strName,varValue, lpszDescr,dwData,lpszEditMask,lpszEditTemplate, lpszValidChars)
{
}

CUserPasswordPropertyGrid::~CUserPasswordPropertyGrid()
{
}


// CUserPasswordPropertyGrid member functions

/**
	@brief

    @author humkyung
*/
void CUserPasswordPropertyGrid::OnDrawValue (CDC* pDC, CRect rect)
{
    ASSERT_VALID (this);
    ASSERT_VALID (pDC);
    ASSERT_VALID (m_pWndList);

	CString sValue(this->GetValue());
	if(!sValue.IsEmpty())
	{
		CString sPassword;
		for(int i = 0;i < sValue.GetLength();++i)
		{
			sPassword += _T("*");
		}

		rect.DeflateRect(3,0);
		pDC->DrawText ( sPassword , rect , DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
	}
}

/**
	@brief	create inplace edit

    @author humkyung

	@date	2012.12.27
*/
CWnd* CUserPasswordPropertyGrid::CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat)
{
	if(FALSE == IsAllowEdit()) return NULL;

	CEdit* pWndEdit = (CEdit*)CMFCPropertyGridProperty::CreateInPlaceEdit(rectEdit , bDefaultFormat);
	if(NULL != pWndEdit)
	{
		pWndEdit->SetPasswordChar('*');
	}
	bDefaultFormat = TRUE;

	return pWndEdit;
}