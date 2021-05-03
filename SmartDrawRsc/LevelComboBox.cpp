// LevelComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDrawRsc.h"
#include "LevelComboBox.h"
#include <Tokenizer.h>

#include <fstream>
#include <vector>
using namespace std;

static vector<STRING_T> g_LevelList;

// CLevelComboBox

IMPLEMENT_DYNAMIC(CLevelComboBox, CComboBox)

CLevelComboBox::CLevelComboBox(const DWORD& dwStyle) : m_dwStyle(dwStyle)
{

}

CLevelComboBox::~CLevelComboBox()
{
}


BEGIN_MESSAGE_MAP(CLevelComboBox, CComboBox)
	ON_WM_CREATE()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

// CLevelComboBox message handlers

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   OnCreate
    @return     int
    @param      LPCREATESTRUCT  lpCreateStruct
    @brief
******************************************************************************/
int CLevelComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   PreSubclassWindow
    @return     void
    @brief
******************************************************************************/
void CLevelComboBox::PreSubclassWindow()
{
	CComboBox::PreSubclassWindow();

	/*DWORD dwStyle = this->GetStyle();
	ASSERT((dwStyle & CBS_DROPDOWNLIST) && ("dropdownlist is unset"));
	ASSERT(!(dwStyle & CBS_SORT) && "sort is set");*/
}

UINT CLevelComboBox::OnGetDlgCode() 
{
	return DLGC_WANTALLKEYS;
}

/******************************************************************************
    @author     humkyung
	@brief		modify combobox style
    @date       2014-06-23
    @function   SetOutputFormat
    @return     int
******************************************************************************/
BOOL CLevelComboBox::ModifyStyle(const DWORD& dwRemove , const DWORD& dwAdd)
{
	CComboBox::ModifyStyle(dwRemove , dwAdd);
	return CLevelComboBox::RecreateComboBox(this);
}

/******************************************************************************
    @author     humkyung
	@brief		reset level list
    @date       2014-06-24
    @function   ResetContents
    @return     int
******************************************************************************/
int CLevelComboBox::ResetContents()
{
	g_LevelList.clear();
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
	@brief		fill contents which is combined with some values seperated by semi colon
    @date       2014-06-23
    @function   LoadContents
    @return     int
******************************************************************************/
int CLevelComboBox::LoadContents(const CString& sIniFilePath)
{
	ASSERT(GetSafeHwnd() && "Window is invalid");

	if(g_LevelList.empty())
	{
		TCHAR szBuf[MAX_PATH + 1]={'\0',};
		if(GetPrivateProfileString(_T("GENERAL") , _T("Levels") , NULL , szBuf , MAX_PATH , sIniFilePath))
		{
			CTokenizer<CIsFromString>::Tokenize(g_LevelList , szBuf , CIsFromString(_T(";")));
			for(vector<STRING_T>::iterator itr = g_LevelList.begin();itr != g_LevelList.end();++itr)
			{
				this->AddString(itr->c_str());
			}

			return ERROR_SUCCESS;
		}
	}
	else
	{
		for(vector<STRING_T>::iterator itr = g_LevelList.begin();itr != g_LevelList.end();++itr)
		{
			AddString(itr->c_str());
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/******************************************************************************
    @author     humkyung
	@brief		save contents to setting file
    @date       2014-06-23
    @function   SaveContents
    @return     int
******************************************************************************/
int CLevelComboBox::SaveContents(const CString& sIniFilePath)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	if(GetPrivateProfileString(_T("GENERAL") , _T("Levels") , NULL , szBuf , MAX_PATH , sIniFilePath))
	{
		CString rString;
		this->GetWindowText(rString);

		CString str(szBuf);
		if(-1 != str.Find(rString)) return ERROR_SUCCESS;	/// just return if level already exists
		str = str.TrimRight(';');
		str += _T(";") + rString;							/// add a new level
		
		WritePrivateProfileString(_T("GENERAL") , _T("Levels") , str , sIniFilePath);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

// recreate the combo box by copying styles etc, and list items
// and applying them to a newly created control
BOOL CLevelComboBox::RecreateComboBox(CComboBox* pCombo, LPVOID lpParam/*=0*/)
{
	if (pCombo == NULL)
		return FALSE;
	if (pCombo->GetSafeHwnd() == NULL)
		return FALSE;

	CWnd* pParent = pCombo->GetParent();
	if (pParent == NULL)
		return FALSE;

	// get current attributes
	DWORD dwStyle = pCombo->GetStyle();
	DWORD dwStyleEx = pCombo->GetExStyle();
	CRect rc;
	pCombo->GetDroppedControlRect(&rc);
	pParent->ScreenToClient(&rc);	// map to client co-ords
	UINT nID = pCombo->GetDlgCtrlID();
	CFont* pFont = pCombo->GetFont();
	CWnd* pWndAfter = pCombo->GetNextWindow(GW_HWNDPREV);

	// get the currently selected text (and whether it is a valid list selection)
	CString sCurText;
	int nCurSel = pCombo->GetCurSel();
	BOOL bItemSelValid = nCurSel != -1;
	if (bItemSelValid)
		pCombo->GetLBText(nCurSel, sCurText);
	else
		pCombo->GetWindowText(sCurText);

	// copy the combo box items into a temp combobox (not sorted)
	// along with each item's data
	CComboBox comboNew;
	if (! comboNew.CreateEx(dwStyleEx, _T("COMBOBOX"), _T(""), 
		dwStyle, rc, pParent, nID, lpParam))
		return FALSE;
	comboNew.SetFont(pFont);
	int nNumItems = pCombo->GetCount();
	for (int n = 0; n < nNumItems; n++)
	{
		CString sText;
		pCombo->GetLBText(n, sText);
		int nNewIndex = comboNew.AddString(sText);
		comboNew.SetItemData(nNewIndex, pCombo->GetItemData(n));
	}
	// re-set selected text
	if (bItemSelValid)
		comboNew.SetCurSel(comboNew.FindStringExact(-1, sCurText));
	else
		comboNew.SetWindowText(sCurText);

	// destroy the existing window, then attach the new one
	pCombo->DestroyWindow();
	HWND hwnd = comboNew.Detach();
	pCombo->Attach(hwnd);

	// position correctly in z-order
	pCombo->SetWindowPos(pWndAfter == NULL ? 
		&CWnd::wndBottom : 
	pWndAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;
}