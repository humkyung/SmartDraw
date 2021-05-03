#include "StdAfx.h"
#include <Tokenizer.h>
#include "SmartDrawRsc.h"
#include "TextStyleDlg.h"
#include "TextStyleComboBox.h"
#include "Resource.h"

#include <vector>
using namespace std;

IMPLEMENT_DYNAMIC(CTextStyleComboBox, CBitmapComboBox)


CTextStyleComboBox::CTextStyleComboBox() : m_nPrevSel(-1)
{
	m_nItemHeight = 18;
}

CTextStyleComboBox::~CTextStyleComboBox(void)
{
}

BEGIN_MESSAGE_MAP(CTextStyleComboBox, CBitmapComboBox)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(CBN_SELENDOK, &CTextStyleComboBox::OnCbnSelendok)
END_MESSAGE_MAP()

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.31
*/
int CTextStyleComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(-1 == CBitmapComboBox::OnCreate(lpCreateStruct))
		return -1;

	return 0;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.31
*/
int CTextStyleComboBox::FillTextStyleList(const CString& rIniFilePath)
{
	m_rIniFilePath = rIniFilePath;

	ResetContent();

	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	
	CString rTextStyleKeyName;
	if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , rIniFilePath))
	{
		rTextStyleKeyName = CString(szBuf) + _T("_TEXT_STYLE");
	}else	return ERROR_BAD_ENVIRONMENT;

	vector<STRING_T> oResult;
	if(GetPrivateProfileString(rTextStyleKeyName , _T("TextStyle List") , NULL , szBuf , MAX_PATH , rIniFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
	}
	stable_sort(oResult.begin() , oResult.end());

	for(int i = 0;i < int(oResult.size());++i)
	{
		AddString(IDB_TEXT_STYLE , oResult[i].c_str());
	}
	AddString(IDB_TEXT_STYLE , _T("New or Edit..."));

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.17
*/
void CTextStyleComboBox::OnCbnSelendok()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const int nItem = GetCurSel();
	if(-1 != nItem)
	{
		CString rString;
		GetLBText(nItem , rString);
		if(_T("New or Edit...") == rString)
		{
			CTextStyleDlg dlg(m_rIniFilePath);
			if(IDOK == dlg.DoModal())
			{
				FillTextStyleList(m_rIniFilePath);
			}
			if(-1 != m_nPrevSel) SetCurSel(m_nPrevSel);
		}else	m_nPrevSel = nItem;
	}
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.18
*/
int CTextStyleComboBox::SetCurSel(int nSelect)
{
	m_nPrevSel = nSelect;
	return CComboBox::SetCurSel(nSelect);
}
