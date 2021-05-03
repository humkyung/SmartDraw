// PlatformHatchingPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include <Tokenizer.h>
#include <DgnV7Lib.h>
#include "PlatformHatchingPage.h"
#include "SmartDrawDoc.h"

using namespace PropertyPage;
// CPlatformHatchingPage dialog

IMPLEMENT_DYNAMIC(CPlatformHatchingPage, CSmartDrawPropertyPage)

CPlatformHatchingPage::CPlatformHatchingPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CPlatformHatchingPage::IDD, pParent)
{

}

CPlatformHatchingPage::~CPlatformHatchingPage()
{
}

void CPlatformHatchingPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX , IDC_BUTTON_PF_HATCHING_TEXT_COLOR , m_wndPFHatchingTextColorButton);
	DDX_Control(pDX , IDC_BUTTON_PF_HATCHING_COLOR , m_wndPFHatchingColorButton);
	DDX_Control(pDX , IDC_COMBO_PF_HATCHING_TEXT_STYLE , m_wndPFHatchingTextStyleComboBox);
	DDX_Control(pDX , IDC_EDIT_PF_HATCHING_CELL_LIBRARY , m_wndCellLibraryFileEdit);

	DDX_Control(pDX , IDC_COMBO_STR_PF_LEVEL , m_cboGratingPFLevel);
	DDX_Control(pDX , IDC_COMBO_CONCRETE_PF_LEVEL , m_cboConcretePFLevel);
	DDX_Control(pDX , IDC_COMBO_EQP_PF_LEVEL , m_cboEqpPFLevel);

	DDX_Control(pDX , IDC_STATIC_PLATFORM_HATCHING , m_wndPlatformHatchingPicture);
	DDX_Control(pDX , IDC_COMBO_PF_HATCHING_STYLE  , m_cboHatchingStyle);
	DDX_Control(pDX , IDC_COMBO_PF_HATCHING_WEIGHT , m_cboHatchingWeight);
	DDX_Control(pDX , IDC_COMBO_PF_HATCHING_SIZE , m_cboHatchingSize);	/// 2014.04.11 added by humkyung

	DDX_Control(pDX , IDC_COMBO_PF_HATCHING_LEADER_LEVEL , m_cboLeaderLevel);
	DDX_Control(pDX , IDC_BUTTON_PF_HATCHING_LEADER_COLOR , m_btnLeaderColor);
	DDX_Control(pDX , IDC_COMBO_PF_HATCHING_LEADER_STYLE , m_cboLeaderStyle);
	DDX_Control(pDX , IDC_COMBO_PF_HATCHING_LEADER_WEIGHT, m_cboLeaderWeight);

	DDX_Control(pDX , IDC_COMBO_PF_HATCHING_ARROW_TYPE , m_cboArrowType);
}


BEGIN_MESSAGE_MAP(CPlatformHatchingPage, CSmartDrawPropertyPage)
	ON_EN_CHANGE(IDC_EDIT_PF_HATCHING_CELL_LIBRARY , &CPlatformHatchingPage::OnEnChangeEditPFCellLibraryFilePath)
END_MESSAGE_MAP()


// CPlatformHatchingPage message handlers

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   CPlatformHatchingPage::PreTranslateMessage
    @return     BOOL
    @param      MSG*    pMsg
    @brief
******************************************************************************/
BOOL PropertyPage::CPlatformHatchingPage::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE) return TRUE;
		if(pMsg->wParam == VK_RETURN) return TRUE;
	}

	return CSmartDrawPropertyPage::PreTranslateMessage(pMsg);
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   CPlatformHatchingPage::OnInitDialog
    @return     BOOL
    @brief
******************************************************************************/
BOOL PropertyPage::CPlatformHatchingPage::OnInitDialog()
{
	static const STRING_T sApp(_T("ANNOTATION_OPTIONS"));
	
	CSmartDrawPropertyPage::OnInitDialog();
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	m_wndPlatformHatchingPicture.Load(GetExecPath() + _T("\\Resource\\platform_hatching.bmp"));
	m_wndPlatformHatchingPicture.Draw();

	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_wndPFHatchingTextColorButton.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_wndPFHatchingColorButton.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_wndPFHatchingTextStyleComboBox.FillTextStyleList(m_sIniFilePath);
	m_btnLeaderColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	m_wndCellLibraryFileEdit.m_sIniFilePath = m_sIniFilePath;
	m_wndCellLibraryFileEdit.EnableFileBrowseButton(); /// To show file open dialog

	/// fill level data - 2014.06.23 added by humkyung
	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	m_cboHatchingStyle.LoadContents(pDoc->GetLineStyleFilePath());
	m_cboLeaderStyle.LoadContents(pDoc->GetLineStyleFilePath());

	if(_T("MSTN") != sOutputFormat)
	{
		m_cboGratingPFLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboEqpPFLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
		m_cboLeaderLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
	}
	m_cboGratingPFLevel.LoadContents(m_sIniFilePath);
	m_cboEqpPFLevel.LoadContents(m_sIniFilePath);
	m_cboLeaderLevel.LoadContents(m_sIniFilePath);
	/// up to here

	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_TextStyle") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_wndPFHatchingTextStyleComboBox.FindStringExact(-1 , szBuf);
		m_wndPFHatchingTextStyleComboBox.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_TextLevel") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox *pCombo = (CComboBox*)(GetDlgItem(IDC_COMBO_PF_HATCHING_TEXT_LEVEL));
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_TextColor") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_wndPFHatchingTextColorButton.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_TextOffset") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_PF_HATCHING_TEXT_OFFSET , szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Level") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox *pCombo = (CComboBox*)(GetDlgItem(IDC_COMBO_PF_HATCHING_LEVEL));
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Color") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_wndPFHatchingColorButton.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Style") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboHatchingStyle.FindString(-1 , szBuf);
		m_cboHatchingStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Weight") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboHatchingWeight.FindString(-1 , szBuf);
		m_cboHatchingWeight.SetCurSel((-1 != at) ? at : 0);
	}

	/// refer(http://www.solutionware.co.kr:8080/projects/SmartDraw/ticket/6) - 2014.04.12 added by humkyung
	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Size") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboHatchingSize.FindString(-1 , szBuf);
		m_cboHatchingSize.SetCurSel((-1 != at) ? at : 0);
	}
	else
	{
		m_cboHatchingSize.SetCurSel(0);
	}
	/// up to here

	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_Hatching_CellLibrary") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_wndCellLibraryFileEdit.SetWindowText(szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_Hatching_CellName") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)(GetDlgItem(IDC_COMBO_PF_HATCHING_CELL_NAME));
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Grating_Platform_Level") , _T("21") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboGratingPFLevel.FindString(-1 , szBuf);
		m_cboGratingPFLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Concrete_Platform_Level") , _T("21") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboGratingPFLevel.FindString(-1 , szBuf);
		m_cboGratingPFLevel.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Equipment_Platform_Level") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboEqpPFLevel.FindString(-1 , szBuf);
		m_cboEqpPFLevel.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_Leader_Level") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderLevel.FindString(-1 , szBuf);
		m_cboLeaderLevel.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_Leader_Color") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_btnLeaderColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_Leader_Style") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderStyle.FindString(-1 , szBuf);
		m_cboLeaderStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_Leader_Weight") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboLeaderWeight.FindString(-1 , szBuf);
		m_cboLeaderWeight.SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_Arrow_Type") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboArrowType.FindString(-1 , szBuf);
		m_cboArrowType.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Platform_Arrow_Size") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_PF_HATCHING_ARROW_SIZE , szBuf);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   CHatchingPage::SaveData
    @return     int
    @brief
******************************************************************************/
int PropertyPage::CPlatformHatchingPage::SaveData()
{
	static const STRING_T sApp(_T("ANNOTATION_OPTIONS"));

	CString rString;
	int at = m_wndPFHatchingTextStyleComboBox.GetCurSel();
	if(-1 != at)
	{
		m_wndPFHatchingTextStyleComboBox.GetLBText(at , rString);
		::WritePrivateProfileString(sApp.c_str() , _T("Platform_TextStyle") , rString , m_sIniFilePath);
	}

	CComboBox *pCombo = (CComboBox*)(GetDlgItem(IDC_COMBO_PF_HATCHING_TEXT_LEVEL));
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		::WritePrivateProfileString(sApp.c_str() , _T("Platform_TextLevel") , rString , m_sIniFilePath);
	}

	int iColorIdx = m_wndPFHatchingTextColorButton.GetColorIndex();
	rString.Format(_T("%d") , iColorIdx);	
	::WritePrivateProfileString(sApp.c_str() , _T("Platform_TextColor") , rString , m_sIniFilePath);

	GetDlgItemText(IDC_EDIT_PF_HATCHING_TEXT_OFFSET , rString);
	::WritePrivateProfileString(sApp.c_str() , _T("Platform_TextOffset") , rString , m_sIniFilePath);

	pCombo = (CComboBox*)(GetDlgItem(IDC_COMBO_PF_HATCHING_LEVEL));
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		::WritePrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Level") , rString , m_sIniFilePath);
	}

	iColorIdx = m_wndPFHatchingColorButton.GetColorIndex();
	rString.Format(_T("%d") , iColorIdx);	
	::WritePrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Color") , rString , m_sIniFilePath);

	at = m_cboHatchingStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboHatchingStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Style") , rString , m_sIniFilePath);
	}
	at = m_cboHatchingWeight.GetCurSel();
	if(-1 != at)
	{
		m_cboHatchingWeight.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Weight") , rString , m_sIniFilePath);
	}

	/// save platform hatching size - 2014.04.12 added by humkyung
	at = m_cboHatchingSize.GetCurSel();
	if(-1 != at)
	{
		m_cboHatchingSize.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Size") , rString , m_sIniFilePath);
	}
	/// up to here

	m_wndCellLibraryFileEdit.GetWindowText(rString);
	::WritePrivateProfileString(sApp.c_str() , _T("Platform_Hatching_CellLibrary") , rString , m_sIniFilePath);

	pCombo = (CComboBox*)(GetDlgItem(IDC_COMBO_PF_HATCHING_CELL_NAME));
	at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		::WritePrivateProfileString(sApp.c_str() , _T("Platform_Hatching_CellName") , rString , m_sIniFilePath);
	}

	at = m_cboGratingPFLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboGratingPFLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Structure_Platform_Level") , rString , m_sIniFilePath);
	}

	at = m_cboEqpPFLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboEqpPFLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Equipment_Platform_Level") , rString , m_sIniFilePath);
	}

	at = m_cboLeaderLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboLeaderLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Platform_Leader_Level") , rString , m_sIniFilePath);
	}
	at = m_btnLeaderColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp.c_str() , _T("Platform_Leader_Color") , rString , m_sIniFilePath);
	
	at = m_cboLeaderStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboLeaderStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Platform_Leader_Style") , rString , m_sIniFilePath);
	}

	at = m_cboLeaderWeight.GetCurSel();
	if(-1 != at)
	{
		m_cboLeaderWeight.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Platform_Leader_Weight") , rString , m_sIniFilePath);
	}

	at = m_cboArrowType.GetCurSel();
	if(-1 != at)
	{
		m_cboArrowType.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Platform_Arrow_Type") , rString , m_sIniFilePath);
	}
	GetDlgItemText(IDC_EDIT_PF_HATCHING_ARROW_SIZE , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Platform_Arrow_Size") , rString , m_sIniFilePath);

	/// 2014.06.23 added by humkyung
	m_cboGratingPFLevel.SaveContents(m_sIniFilePath);
	m_cboEqpPFLevel.SaveContents(m_sIniFilePath);
	m_cboLeaderLevel.SaveContents(m_sIniFilePath);
	/// up to here

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   CPlatformHatchingPage::OnEnChangeEditPFCellLibraryFilePath
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CPlatformHatchingPage::OnEnChangeEditPFCellLibraryFilePath()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBitmapComboBox::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	auto_ptr<CDgnDocument> oDgnDoc(CDgnDocument::CreateInstance());
	CComboBox* pPFCellNameComboBox = (CComboBox*)(GetDlgItem(IDC_COMBO_PF_HATCHING_CELL_NAME));
	///CComboBox* pEqpCellNameComboBox = (CComboBox*)(GetDlgItem(IDC_COMBO_EQP_HATCHING_CELL_NAME));
	pPFCellNameComboBox->ResetContent();
	///pEqpCellNameComboBox->ResetContent();
	
	unsigned char r=0 , g=0 , b=0;
	vector<int> oParsingTypes;
	{
		oParsingTypes.push_back(DGNT_CELL_LIBRARY);
	}
	CString sString;
	m_wndCellLibraryFileEdit.GetWindowText(sString);
	oDgnDoc->Read(sString , CDgnDocument::UOR_BASE , &oParsingTypes);
	const int iCount = oDgnDoc->GetEntityCount();
	for(int i = 0;i < iCount;++i)
	{
		CDgnElement* pDgnElm = oDgnDoc->GetEntityAt(i);
		if(pDgnElm->IsKindOf(CDgnCellLibrary::TypeString()))
		{
			CDgnCellLibrary* pCellLibrary = static_cast<CDgnCellLibrary*>(pDgnElm);
			pPFCellNameComboBox->AddString(CString(pCellLibrary->name()) + CString(_T(" / ")) + pCellLibrary->desc());
		}
	}

	pPFCellNameComboBox->SetCurSel(0);
	///pEqpCellNameComboBox->SetCurSel(0);
}