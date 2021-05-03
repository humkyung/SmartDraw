// CAreaOptionPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include <Tokenizer.h>
#include "SmartDrawPropertySheet.h"
#include "AreaOptionPage.h"

using namespace PropertyPage;
// CAreaOptionPage dialog

IMPLEMENT_DYNAMIC(CAreaOptionPage, CSmartDrawPropertyPage)

CAreaOptionPage::CAreaOptionPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CAreaOptionPage::IDD)
{

}

CAreaOptionPage::~CAreaOptionPage()
{
}

void CAreaOptionPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_COMBO_TAG_RANGE , m_wndTagRangeComboBox);

	DDX_Control(pDX , IDC_SPIN_TOP_RANGE , m_wndTopSpinCtrl);
	DDX_Control(pDX , IDC_SPIN_BOTTOM_RANGE , m_wndBottomSpinCtrl);
	DDX_Control(pDX , IDC_SPIN_LEFT_RANGE , m_wndLeftSpinCtrl);
	DDX_Control(pDX , IDC_SPIN_RIGHT_RANGE , m_wndRightSpinCtrl);

	DDX_Control(pDX , IDC_CHECK_TOP_SIDE , m_btnTopSideCheck);
	DDX_Control(pDX , IDC_CHECK_BOTTOM_SIDE , m_btnBottomSideCheck);
	DDX_Control(pDX , IDC_CHECK_LEFT_SIDE , m_btnLeftSideCheck);
	DDX_Control(pDX , IDC_CHECK_RIGHT_SIDE , m_btnRightSideCheck);

	DDX_Control(pDX , IDC_CHECK_SEC_TOP_SIDE , m_btnSecTopSideCheck);
	DDX_Control(pDX , IDC_CHECK_SEC_BOTTOM_SIDE , m_btnSecBottomSideCheck);
	DDX_Control(pDX , IDC_CHECK_SEC_LEFT_SIDE , m_btnSecLeftSideCheck);
	DDX_Control(pDX , IDC_CHECK_SEC_RIGHT_SIDE , m_btnSecRightSideCheck);
}


BEGIN_MESSAGE_MAP(CAreaOptionPage, CSmartDrawPropertyPage)
	ON_WM_SIZE()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TOP_RANGE, &CAreaOptionPage::OnDeltaposTopRange)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BOTTOM_RANGE, &CAreaOptionPage::OnDeltaposBottomRange)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LEFT_RANGE, &CAreaOptionPage::OnDeltaposLeftRange)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RIGHT_RANGE, &CAreaOptionPage::OnDeltaposRightRange)
	ON_CBN_SELCHANGE(IDC_COMBO_TAG_RANGE, &CAreaOptionPage::OnCbnSelchangeComboTagRange)
	ON_EN_CHANGE(IDC_EDIT_TOP_RANGE, &CAreaOptionPage::OnEnChangeEditTopRange)
	ON_EN_CHANGE(IDC_EDIT_BOTTOM_RANGE, &CAreaOptionPage::OnEnChangeEditBottomRange)
	ON_EN_CHANGE(IDC_EDIT_LEFT_RANGE, &CAreaOptionPage::OnEnChangeEditLeftRange)
	ON_EN_CHANGE(IDC_EDIT_RIGHT_RANGE, &CAreaOptionPage::OnEnChangeEditRightRange)
END_MESSAGE_MAP()


// CAreaOptionPage message handlers

void CAreaOptionPage::OnSize(UINT nType, int cx, int cy)
{
	///CMFCPropertyPage::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.14
*/
int CAreaOptionPage::SaveData()
{
	static const CString sApp(_T("AREA_OPTIONS"));

	OSTRINGSTREAM_T oss;
	oss << m_oOtherArea.Top;
	::WritePrivateProfileString(sApp , _T("Outside_Top") , oss.str().c_str() , m_rIniFilePath);

	oss.str(_T("")); oss << m_oOtherArea.Bottom;
	::WritePrivateProfileString(sApp , _T("Outside_Bottom") , oss.str().c_str() , m_rIniFilePath);

	oss.str(_T("")); oss << m_oOtherArea.Left;
	::WritePrivateProfileString(sApp , _T("Outside_Left") , oss.str().c_str() , m_rIniFilePath);

	oss.str(_T("")); oss << m_oOtherArea.Right;
	::WritePrivateProfileString(sApp , _T("Outside_Right") , oss.str().c_str() , m_rIniFilePath);

	/// write line no tag area - 2014.09.11 added by humkyung
	oss.str(_T("")); oss << m_oLineNoArea.Top;
	::WritePrivateProfileString(sApp , _T("Line_No_Outside_Top") , oss.str().c_str() , m_rIniFilePath);

	oss.str(_T("")); oss << m_oLineNoArea.Bottom;
	::WritePrivateProfileString(sApp , _T("Line_No_Outside_Bottom") , oss.str().c_str() , m_rIniFilePath);

	oss.str(_T("")); oss << m_oLineNoArea.Left;
	::WritePrivateProfileString(sApp , _T("Line_No_Outside_Left") , oss.str().c_str() , m_rIniFilePath);

	oss.str(_T("")); oss << m_oLineNoArea.Right;
	::WritePrivateProfileString(sApp , _T("Line_No_Outside_Right") , oss.str().c_str() , m_rIniFilePath);
	/// up to here

	CString sExtTagSide;
	m_btnLeftSideCheck.GetCheck() ? sExtTagSide += ((sExtTagSide.IsEmpty()) ? _T("LEFT") : _T(",LEFT")) : sExtTagSide;
	m_btnTopSideCheck.GetCheck() ? sExtTagSide += ((sExtTagSide.IsEmpty()) ? _T("TOP") : _T(",TOP")) : sExtTagSide;
	m_btnRightSideCheck.GetCheck() ? sExtTagSide += ((sExtTagSide.IsEmpty()) ? _T("RIGHT") : _T(",RIGHT")) : sExtTagSide;
	m_btnBottomSideCheck.GetCheck() ? sExtTagSide += ((sExtTagSide.IsEmpty()) ? _T("BOTTOM") : _T(",BOTTOM")) : sExtTagSide;
	::WritePrivateProfileString(sApp , _T("Ext_Tag_Side") , sExtTagSide , m_rIniFilePath);

	sExtTagSide.Empty();
	m_btnSecLeftSideCheck.GetCheck() ? sExtTagSide += ((sExtTagSide.IsEmpty()) ? _T("LEFT") : _T(",LEFT")) : sExtTagSide;
	m_btnSecTopSideCheck.GetCheck() ? sExtTagSide += ((sExtTagSide.IsEmpty()) ? _T("TOP") : _T(",TOP")) : sExtTagSide;
	m_btnSecRightSideCheck.GetCheck() ? sExtTagSide += ((sExtTagSide.IsEmpty()) ? _T("RIGHT") : _T(",RIGHT")) : sExtTagSide;
	m_btnSecBottomSideCheck.GetCheck() ? sExtTagSide += ((sExtTagSide.IsEmpty()) ? _T("BOTTOM") : _T(",BOTTOM")) : sExtTagSide;
	::WritePrivateProfileString(sApp , _T("Ext_Sec_Tag_Side") , sExtTagSide , m_rIniFilePath);

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-18
    @function   CAreaOptionPage::OnInitDialog
    @return     BOOL
    @brief
******************************************************************************/
BOOL PropertyPage::CAreaOptionPage::OnInitDialog()
{
	static const CString sApp(_T("AREA_OPTIONS"));

	CSmartDrawPropertyPage::OnInitDialog();

	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_rIniFilePath = pParent->GetSettingFilePath();

	m_wndTopSpinCtrl.SetPos(10);
	m_wndTopSpinCtrl.SetRange(0 , 50);
	m_wndTopSpinCtrl.SetBuddy(GetDlgItem(IDC_EDIT_TOP_RANGE));
	m_wndBottomSpinCtrl.SetPos(10);
	m_wndBottomSpinCtrl.SetRange(0 , 50);
	m_wndBottomSpinCtrl.SetBuddy(GetDlgItem(IDC_EDIT_BOTTOM_RANGE));
	m_wndLeftSpinCtrl.SetRange(0 , 50);
	m_wndLeftSpinCtrl.SetPos(10);
	m_wndLeftSpinCtrl.SetBuddy(GetDlgItem(IDC_EDIT_LEFT_RANGE));
	m_wndRightSpinCtrl.SetRange(0 , 50);
	m_wndRightSpinCtrl.SetPos(10);
	m_wndRightSpinCtrl.SetBuddy(GetDlgItem(IDC_EDIT_RIGHT_RANGE));

	m_btnTopSideCheck.SetLabels(_T("ON") , _T("OFF"));
	m_btnBottomSideCheck.SetLabels(_T("ON") , _T("OFF"));
	m_btnLeftSideCheck.SetLabels(_T("ON") , _T("OFF"));
	m_btnRightSideCheck.SetLabels(_T("ON") , _T("OFF"));

	m_wndTagRangeComboBox.SetCurSel(0);

	CString rString;
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	m_oLineNoArea.Top = 15;
	if(::GetPrivateProfileString(sApp , _T("Line_No_Outside_Top") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		m_oLineNoArea.Top = ATOI_T(szBuf);
	}
	rString.Format(_T("%d") , m_oLineNoArea.Top);
	SetDlgItemText(IDC_EDIT_TOP_RANGE , rString);
	m_wndTopSpinCtrl.SetPos( m_oLineNoArea.Top );
	
	m_oLineNoArea.Bottom = 15;
	if(::GetPrivateProfileString(sApp , _T("Line_No_Outside_Bottom") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		m_oLineNoArea.Bottom = ATOI_T(szBuf);
	}
	rString.Format(_T("%d") , m_oLineNoArea.Bottom);
	SetDlgItemText(IDC_EDIT_BOTTOM_RANGE , rString);
	m_wndBottomSpinCtrl.SetPos( m_oLineNoArea.Bottom );
	
	m_oLineNoArea.Left = 15;
	if(::GetPrivateProfileString(sApp , _T("Line_No_Outside_Left") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		m_oLineNoArea.Left = ATOI_T(szBuf);
	}
	rString.Format(_T("%d") , m_oLineNoArea.Left);
	SetDlgItemText(IDC_EDIT_LEFT_RANGE , rString);
	m_wndLeftSpinCtrl.SetPos( m_oLineNoArea.Left );

	m_oLineNoArea.Right = 15;
	if(::GetPrivateProfileString(sApp , _T("Line_No_Outside_Right") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		m_oLineNoArea.Right = ATOI_T(szBuf);
	}
	rString.Format(_T("%d") , m_oLineNoArea.Right);
	SetDlgItemText(IDC_EDIT_RIGHT_RANGE , rString);
	m_wndRightSpinCtrl.SetPos( m_oLineNoArea.Right );

	/// get range area for other type - 2014.09.11 added by humkyung
	if(::GetPrivateProfileString(sApp , _T("Outside_Top") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		m_oOtherArea.Top = ATOI_T(szBuf);
	}
	
	if(::GetPrivateProfileString(sApp , _T("Outside_Bottom") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		m_oOtherArea.Bottom = ATOI_T(szBuf);
	}
	
	if(::GetPrivateProfileString(sApp , _T("Outside_Left") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		m_oOtherArea.Left = ATOI_T(szBuf);
	}

	if(::GetPrivateProfileString(sApp , _T("Outside_Right") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		m_oOtherArea.Right = ATOI_T(szBuf);
	}
	/// up to here

	vector<STRING_T> oResult;
	if(::GetPrivateProfileString(sApp , _T("Ext_Tag_Side") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		for(vector<STRING_T>::iterator itr = oResult.begin();itr != oResult.end();++itr)
		{
			if(_T("LEFT") == (*itr))
			{
				m_btnLeftSideCheck.SetCheck(TRUE);
			}
			else if(_T("TOP") == (*itr))
			{
				m_btnTopSideCheck.SetCheck(TRUE);
			}
			else if(_T("RIGHT") == (*itr))
			{
				m_btnRightSideCheck.SetCheck(TRUE);
			}
			else if(_T("BOTTOM") == (*itr))
			{
				m_btnBottomSideCheck.SetCheck(TRUE);
			}
		}
	}
	else
	{
		m_btnLeftSideCheck.SetCheck(TRUE);
		m_btnTopSideCheck.SetCheck(TRUE);
		m_btnRightSideCheck.SetCheck(TRUE);
		m_btnBottomSideCheck.SetCheck(TRUE);
	}

	if(::GetPrivateProfileString(sApp , _T("Ext_Sec_Tag_Side") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		for(vector<STRING_T>::iterator itr = oResult.begin();itr != oResult.end();++itr)
		{
			if(_T("LEFT") == (*itr))
			{
				m_btnSecLeftSideCheck.SetCheck(TRUE);
			}
			else if(_T("TOP") == (*itr))
			{
				m_btnSecTopSideCheck.SetCheck(TRUE);
			}
			else if(_T("RIGHT") == (*itr))
			{
				m_btnSecRightSideCheck.SetCheck(TRUE);
			}
			else if(_T("BOTTOM") == (*itr))
			{
				m_btnSecBottomSideCheck.SetCheck(TRUE);
			}
		}
	}
	else
	{
		m_btnSecLeftSideCheck.SetCheck(TRUE);
		m_btnSecTopSideCheck.SetCheck(TRUE);
		m_btnSecRightSideCheck.SetCheck(TRUE);
		m_btnSecBottomSideCheck.SetCheck(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2014-09-11
    @function   OnCbnSelchangeComboTagRange
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CAreaOptionPage::OnCbnSelchangeComboTagRange()
{
	const int iCurSel = m_wndTagRangeComboBox.GetCurSel();
	if(-1 != iCurSel)
	{
		CString rString;
		m_wndTagRangeComboBox.GetLBText(iCurSel , rString);
		if(CString(_T("Line No")) == rString)
		{
			m_wndTopSpinCtrl.SetPos( m_oLineNoArea.Top );
			m_wndBottomSpinCtrl.SetPos( m_oLineNoArea.Bottom );
			m_wndLeftSpinCtrl.SetPos( m_oLineNoArea.Left );
			m_wndRightSpinCtrl.SetPos( m_oLineNoArea.Right );

			rString.Format(_T("%d") , m_oLineNoArea.Top);
			SetDlgItemText(IDC_EDIT_TOP_RANGE , rString);
			rString.Format(_T("%d") , m_oLineNoArea.Bottom);
			SetDlgItemText(IDC_EDIT_BOTTOM_RANGE , rString);
			rString.Format(_T("%d") , m_oLineNoArea.Left);
			SetDlgItemText(IDC_EDIT_LEFT_RANGE , rString);
			rString.Format(_T("%d") , m_oLineNoArea.Right);
			SetDlgItemText(IDC_EDIT_RIGHT_RANGE , rString);
		}
		else
		{
			m_wndTopSpinCtrl.SetPos( m_oOtherArea.Top );
			m_wndBottomSpinCtrl.SetPos( m_oOtherArea.Bottom );
			m_wndLeftSpinCtrl.SetPos( m_oOtherArea.Left );
			m_wndRightSpinCtrl.SetPos( m_oOtherArea.Right );

			rString.Format(_T("%d") , m_oOtherArea.Top);
			SetDlgItemText(IDC_EDIT_TOP_RANGE , rString);
			rString.Format(_T("%d") , m_oOtherArea.Bottom);
			SetDlgItemText(IDC_EDIT_BOTTOM_RANGE , rString);
			rString.Format(_T("%d") , m_oOtherArea.Left);
			SetDlgItemText(IDC_EDIT_LEFT_RANGE , rString);
			rString.Format(_T("%d") , m_oOtherArea.Right);
			SetDlgItemText(IDC_EDIT_RIGHT_RANGE , rString);
		}
	}
}

void PropertyPage::CAreaOptionPage::OnDeltaposTopRange(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	CString rString;
	rString.Format(_T("%d") , pNMUpDown->iPos);
	SetDlgItemText(IDC_EDIT_TOP_RANGE , rString);

	*pResult = 0;
}

void PropertyPage::CAreaOptionPage::OnDeltaposBottomRange(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	CString rString;
	rString.Format(_T("%d") , pNMUpDown->iPos);
	SetDlgItemText(IDC_EDIT_BOTTOM_RANGE , rString);

	*pResult = 0;
}

void PropertyPage::CAreaOptionPage::OnDeltaposLeftRange(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	CString rString;
	rString.Format(_T("%d") , pNMUpDown->iPos);
	SetDlgItemText(IDC_EDIT_LEFT_RANGE , rString);

	*pResult = 0;
}

void PropertyPage::CAreaOptionPage::OnDeltaposRightRange(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	
	CString rString;
	rString.Format(_T("%d") , pNMUpDown->iPos);
	SetDlgItemText(IDC_EDIT_RIGHT_RANGE , rString);

	*pResult = 0;
}

void PropertyPage::CAreaOptionPage::OnEnChangeEditTopRange()
{
	const int iCurSel = m_wndTagRangeComboBox.GetCurSel();
	if(-1 != iCurSel)
	{
		CString rString , sValue;
		GetDlgItemText(IDC_EDIT_TOP_RANGE , sValue);
		const int iValue = ATOI_T(sValue);

		m_wndTagRangeComboBox.GetLBText(iCurSel , rString);
		if(CString(_T("Line No")) == rString)
		{
			m_oLineNoArea.Top = iValue;
		}
		else
		{
			m_oOtherArea.Top = iValue;
		}
	}
}

void PropertyPage::CAreaOptionPage::OnEnChangeEditBottomRange()
{
	const int iCurSel = m_wndTagRangeComboBox.GetCurSel();
	if(-1 != iCurSel)
	{
		CString rString , sValue;
		GetDlgItemText(IDC_EDIT_BOTTOM_RANGE , sValue);
		const int iValue = ATOI_T(sValue);

		m_wndTagRangeComboBox.GetLBText(iCurSel , rString);
		if(CString(_T("Line No")) == rString)
		{
			m_oLineNoArea.Bottom = iValue;
		}
		else
		{
			m_oOtherArea.Bottom = iValue;
		}
	}
}

void PropertyPage::CAreaOptionPage::OnEnChangeEditLeftRange()
{
	const int iCurSel = m_wndTagRangeComboBox.GetCurSel();
	if(-1 != iCurSel)
	{
		CString rString , sValue;
		GetDlgItemText(IDC_EDIT_LEFT_RANGE , sValue);
		const int iValue = ATOI_T(sValue);

		m_wndTagRangeComboBox.GetLBText(iCurSel , rString);
		if(CString(_T("Line No")) == rString)
		{
			m_oLineNoArea.Left = iValue;
		}
		else
		{
			m_oOtherArea.Left = iValue;
		}
	}
}

void PropertyPage::CAreaOptionPage::OnEnChangeEditRightRange()
{
	const int iCurSel = m_wndTagRangeComboBox.GetCurSel();
	if(-1 != iCurSel)
	{
		CString rString , sValue;
		GetDlgItemText(IDC_EDIT_RIGHT_RANGE , sValue);
		const int iValue = ATOI_T(sValue);

		m_wndTagRangeComboBox.GetLBText(iCurSel , rString);
		if(CString(_T("Line No")) == rString)
		{
			m_oLineNoArea.Right = iValue;
		}
		else
		{
			m_oOtherArea.Right = iValue;
		}
	}
}