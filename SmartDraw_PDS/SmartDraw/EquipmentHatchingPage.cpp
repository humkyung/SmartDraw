// EquipmentHatchingPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include <Tokenizer.h>
#include <DgnV7Lib.h>
#include "EquipmentHatchingPage.h"
#include "SmartDrawDoc.h"

using namespace PropertyPage;
// CEquipmentHatchingPage dialog

IMPLEMENT_DYNAMIC(CEquipmentHatchingPage, CSmartDrawPropertyPage)

CEquipmentHatchingPage::CEquipmentHatchingPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CEquipmentHatchingPage::IDD, pParent) , m_iHatchingScale(1)
{

}

CEquipmentHatchingPage::~CEquipmentHatchingPage()
{
}

void CEquipmentHatchingPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_EDIT_EQP_HATCHING_CELL_LIBRARY , m_wndCellLibraryFileEdit);

	DDX_Control(pDX , IDC_STATIC_EQUIPMENT_HATCHING , m_wndEquipmentHatchingPicture);
	DDX_CBIndex(pDX , IDC_COMBO_EQP_HATCHING_SCALE , m_iHatchingScale);
	DDX_Control(pDX , IDC_BUTTON_EQUIPMENT_HATCHING_COLOR , m_btnHatchingColor);
	DDX_Control(pDX , IDC_COMBO_EQP_HATCHING_LEVEL , m_cboHatchingLevel);
	DDX_Control(pDX , IDC_COMBO_EQP_HATCHING_STYLE , m_cboHatchingStyle);
	DDX_Control(pDX , IDC_COMBO_EQP_HATCHING_WEIGHT , m_cboHatchingWeight);
}


BEGIN_MESSAGE_MAP(CEquipmentHatchingPage, CSmartDrawPropertyPage)
	ON_EN_CHANGE(IDC_EDIT_EQP_HATCHING_CELL_LIBRARY , &CEquipmentHatchingPage::OnEnChangeEditEqpCellLibraryFilePath)
END_MESSAGE_MAP()


// CEquipmentHatchingPage message handlers

BOOL PropertyPage::CEquipmentHatchingPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	static const STRING_T sApp(_T("Hatching"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	m_wndCellLibraryFileEdit.m_sIniFilePath = m_sIniFilePath;
	m_wndCellLibraryFileEdit.EnableFileBrowseButton(); /// To show file open dialog
	if(GetPrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_CellLibrary") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_wndCellLibraryFileEdit.SetWindowText(szBuf);
	}

	m_wndEquipmentHatchingPicture.Load(GetExecPath() + _T("\\Resource\\equipment_hatching.bmp"));
	m_wndEquipmentHatchingPicture.Draw();
	
	/// load contents - 2014.06.23 added by humkyung
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	const CString sOutputFormat = pDoc->GetOutputFormatString().c_str();
	m_cboHatchingStyle.LoadContents(pDoc->GetLineStyleFilePath());
	
	if(_T("MSTN") != sOutputFormat) m_cboHatchingLevel.ModifyStyle(CBS_SORT|CBS_DROPDOWNLIST,CBS_DROPDOWN|WS_VSCROLL);
	m_cboHatchingLevel.LoadContents(m_sIniFilePath);
	/// up to here

	if(GetPrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_CellName") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox* pCombo = (CComboBox*)(GetDlgItem(IDC_COMBO_EQP_HATCHING_CELL_NAME));
		const int at = pCombo->FindStringExact(-1 , szBuf);
		pCombo->SetCurSel((-1 != at) ? at : 0);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Offset") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_EQP_HATCHING_OFFSET , szBuf);
	}

	m_iHatchingScale = GetPrivateProfileInt(sApp.c_str() , _T("Eqp_Hatching_Scale") , 1 , m_sIniFilePath);
	m_iHatchingScale -= 1;

	if(GetPrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Level") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboHatchingLevel.FindString(-1 , szBuf);
		m_cboHatchingLevel.SetCurSel((-1 != at) ? at : 0);
	}

	m_btnHatchingColor.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	if(GetPrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Color") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_btnHatchingColor.SetColorIndex(ATOI_T(szBuf));
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Style") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboHatchingStyle.FindString(-1 , szBuf);
		m_cboHatchingStyle.SetCurSel((-1 != at) ? at : 0);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Weight") , _T("") , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int at = m_cboHatchingWeight.FindString(-1 , szBuf);
		m_cboHatchingWeight.SetCurSel((-1 != at) ? at : 0);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int PropertyPage::CEquipmentHatchingPage::SaveData()
{
	static const STRING_T sApp(_T("Hatching"));
	UpdateData(TRUE);

	CString rString;

	m_wndCellLibraryFileEdit.GetWindowText(rString);
	WritePrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_CellLibrary") , rString , m_sIniFilePath);
	CComboBox* pCombo = (CComboBox*)(GetDlgItem(IDC_COMBO_EQP_HATCHING_CELL_NAME));
	int at = pCombo->GetCurSel();
	if(-1 != at)
	{
		pCombo->GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_CellName") , rString , m_sIniFilePath);
	}

	GetDlgItemText(IDC_EDIT_EQP_HATCHING_OFFSET , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Offset") , rString , m_sIniFilePath);

	((CComboBox*)GetDlgItem(IDC_COMBO_EQP_HATCHING_SCALE))->GetLBText(m_iHatchingScale , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Scale") , rString , m_sIniFilePath);

	at = m_cboHatchingLevel.GetCurSel();
	if(-1 != at)
	{
		m_cboHatchingLevel.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Level") , rString , m_sIniFilePath);
	}

	at = m_btnHatchingColor.GetColorIndex();
	rString.Format(_T("%d") , at);
	WritePrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Color") , rString , m_sIniFilePath);
	
	at = m_cboHatchingStyle.GetCurSel();
	if(-1 != at)
	{
		m_cboHatchingStyle.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Style") , rString , m_sIniFilePath);
	}

	at = m_cboHatchingWeight.GetCurSel();
	if(-1 != at)
	{
		m_cboHatchingWeight.GetLBText(at , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Weight") , rString , m_sIniFilePath);
	}

	m_cboHatchingLevel.SaveContents(m_sIniFilePath);	/// 2014.06.23 added by humkyung

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   CEquipmentHatchingPage::OnEnChangeEditEqpCellLibraryFilePath
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CEquipmentHatchingPage::OnEnChangeEditEqpCellLibraryFilePath()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBitmapComboBox::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	auto_ptr<CDgnDocument> oDgnDoc(CDgnDocument::CreateInstance());
	CComboBox* pEqpCellNameComboBox = (CComboBox*)(GetDlgItem(IDC_COMBO_EQP_HATCHING_CELL_NAME));
	pEqpCellNameComboBox->ResetContent();
	
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
			pEqpCellNameComboBox->AddString(CString(pCellLibrary->name()) + CString(_T(" / ")) + CString(pCellLibrary->desc()));
		}
	}

	pEqpCellNameComboBox->SetCurSel(0);
}