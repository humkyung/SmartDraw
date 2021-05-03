// CableTrayPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include <Tokenizer.h>
#include <DgnV7Lib.h>
#include "SmartDrawDocData.h"
#include "SmartDrawPropertySheet.h"
#include "CableTrayPage.h"
#include "SmartDrawDoc.h"

using namespace PropertyPage;
// CCableTrayPage dialog

/******************************************************************************
    @author     humkyung
    @date       2012-02-20
    @function   CCellLibraryFileBrowseEdit::OnBrowse
    @return     void
    @brief
******************************************************************************/
void CCableTrayPage::CCellLibraryFileBrowseEdit::OnBrowse()
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CString rOutputFormat;
	if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		rOutputFormat = szBuf;
		rOutputFormat.MakeUpper();
	}

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	if(_T("MSTN") == rOutputFormat)
	{
		// 파일 Open 
		CFileDialog dlg(TRUE,_T("cel,*.*"),_T(""),OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
			_T("Microstation Cell Libraries(*.cel)|*.cel|모든 문서(*.*)|*.*||"));
		if(IDOK == dlg.DoModal())
		{
			m_rFilePath = dlg.GetPathName();
			SetWindowText(m_rFilePath);
		}
	}
	else if(_T("AUTOCAD") == rOutputFormat)
	{
		// 파일 Open 
		CFileDialog dlg(TRUE,_T("dwg,*.*"),_T(""),OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
			_T("AutoCAD File(*.dwg)|*.dwg|모든 문서(*.*)|*.*||"));
		if(IDOK == dlg.DoModal())
		{
			m_rFilePath = dlg.GetPathName();
			SetWindowText(m_rFilePath);
		}
	}
}

IMPLEMENT_DYNAMIC(CCableTrayPage, CSmartDrawPropertyPage)

CCableTrayPage::CCableTrayPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CCableTrayPage::IDD, pParent)
{

}

CCableTrayPage::~CCableTrayPage()
{
}

void CCableTrayPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_COMBO_TEXT_STYLE , m_wndTextStyleComboBox);
	DDX_Control(pDX , IDC_BUTTON_TEXT_COLOR , m_wndTextColorButton);
	DDX_Control(pDX , IDC_COMBO_ARROW_TYPE , m_wndArrowComboBox);
	DDX_Control(pDX , IDC_EDIT_CELL_LIBRARY_FILE_PATH , m_wndCellLibraryFileEdit);
	DDX_Control(pDX , IDC_BUTTON_ELEC_HATCH_COLOR , m_wndElecHatchColorButton);
	DDX_Control(pDX , IDC_BUTTON_INST_HATCH_COLOR , m_wndInstHatchColorButton);
	DDX_Control(pDX , IDC_CHECK_ELEC , m_wndElecTagCheckBox);
	DDX_Control(pDX , IDC_CHECK_ELEC_OUTLINE , m_wndElecOutlineCheckBox);
	DDX_Control(pDX , IDC_CHECK_INST , m_wndInstTagCheckBox);
	DDX_Control(pDX , IDC_CHECK_INST_OUTLINE , m_wndInstOutlineCheckBox);
	DDX_Control(pDX , IDC_CHECK_FITTING_TYPE , m_wndFittingTagCheckBox);
	DDX_Control(pDX , IDC_CHECK_UPDOWN_MARK , m_wndUpDownMarkCheckBox);
}


BEGIN_MESSAGE_MAP(CCableTrayPage, CSmartDrawPropertyPage)
	ON_NOTIFY (UDM_TOOLTIP_DISPLAY, NULL, NotifyDisplayTooltip)
	ON_EN_CHANGE(IDC_EDIT_CELL_LIBRARY_FILE_PATH, &CCableTrayPage::OnEnChangeEditCellLibraryFilePath)
	ON_BN_CLICKED(IDC_CHECK_ELEC, &CCableTrayPage::OnBnClickedCheckElec)
	ON_BN_CLICKED(IDC_CHECK_ELEC_OUTLINE, &CCableTrayPage::OnBnClickedCheckElecOutline)
	ON_BN_CLICKED(IDC_CHECK_INST, &CCableTrayPage::OnBnClickedCheckInst)
	ON_BN_CLICKED(IDC_CHECK_INST_OUTLINE, &CCableTrayPage::OnBnClickedCheckInstOutline)
END_MESSAGE_MAP()


// CCableTrayPage message handlers

BOOL PropertyPage::CCableTrayPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();

	CSmartDrawAnnoPropertySheet *pParent = (CSmartDrawAnnoPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();
	m_sProjectName     = pParent->GetProjectName();

	m_wndElecTagCheckBox.SetLabels(_T("ON") , _T("OFF"));
	m_wndElecOutlineCheckBox.SetLabels(_T("ON") , _T("OFF"));
	m_wndInstTagCheckBox.SetLabels(_T("ON") , _T("OFF"));
	m_wndInstOutlineCheckBox.SetLabels(_T("ON") , _T("OFF"));
	m_wndFittingTagCheckBox.SetLabels(_T("ON") , _T("OFF"));
	m_wndUpDownMarkCheckBox.SetLabels(_T("ON") , _T("OFF"));

	m_wndTextStyleComboBox.FillTextStyleList(m_sIniFilePath);
	
	m_wndCellLibraryFileEdit.m_rIniFilePath = m_sIniFilePath;
	m_wndCellLibraryFileEdit.m_rProjectName = m_sProjectName;
	m_wndCellLibraryFileEdit.EnableFileBrowseButton(); // To show file open dialog

	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	m_wndTextColorButton.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_wndElecHatchColorButton.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));
	m_wndInstHatchColorButton.FillColors(pDoc->GetColorListOf(m_sIniFilePath.operator LPCTSTR()));

	m_tooltip.Create(this);
	m_tooltip.AddTool(GetDlgItem(IDC_COMBO_TEXT_STYLE), _T("Tooltip to the control IDC_COMBO_TEXT_STYLE"));
	m_tooltip.SetNotify();

	this->LoadData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-20
    @function   CCableTrayPage::LoadData
    @return     int
    @param      void
    @brief
******************************************************************************/
int PropertyPage::CCableTrayPage::LoadData(void)
{
	static const STRING_T sApp(_T("ANNOTATION_OPTIONS"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	if(GetPrivateProfileString(sApp.c_str() , _T("CableT_Cell_Library") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_wndCellLibraryFileEdit.SetWindowText(szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("CableT_Row_Count") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_ROW_COUNT , szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("CableT_Row_Height") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_ROW_HEIGHT , szBuf);
	}
	
	/// for Electric Cable Tray
	if(GetPrivateProfileString(sApp.c_str() , _T("Elec_Cable_Tray_Label") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_ELEC_LABEL , szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Elec_Cable_Tray_EL_Prefix") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_ELEC_EL_PREFIX , szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Elec_Cable_Tray_Tag_OnOff") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_wndElecTagCheckBox.SetCheck(0 == STRICMP_T(_T("ON") , szBuf));
		this->OnBnClickedCheckElec();
	}
	SetDlgItemText(IDC_EDIT_ELEC_LINE_WEIGHT , _T("0"));
	if(GetPrivateProfileString(sApp.c_str() , _T("Elec_CableT_Outline_Weight") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_ELEC_LINE_WEIGHT , szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Elec_CableT_Outline_OnOff") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_wndElecOutlineCheckBox.SetCheck(0 == STRICMP_T(_T("ON") , szBuf));
		this->OnBnClickedCheckElecOutline();
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Elec_CableT_FileName_Rule") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_ELEC_CABLE_TRAY_FILE_NAME , szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Elec_CableT_CellName") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_ELEC_CELL_NAME);
		const int at = pComboBox->FindString(-1 , szBuf);
		if(-1 != at) pComboBox->SetCurSel(at);
	}
	UINT uiColorIdx = GetPrivateProfileInt(sApp.c_str() , _T("Elec_CableT_HatchColor") , 0 , m_sIniFilePath);
	m_wndElecHatchColorButton.SetColorIndex(uiColorIdx);

	/// for Inst. Cable Tray
	if(GetPrivateProfileString(sApp.c_str() , _T("Inst_Cable_Tray_Label") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_INST_LABEL , szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Inst_Cable_Tray_EL_Prefix") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_INST_EL_PREFIX , szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Inst_Cable_Tray_Tag_OnOff") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_wndInstTagCheckBox.SetCheck(0 == STRICMP_T(_T("ON") , szBuf));
		this->OnBnClickedCheckInst();
	}

	SetDlgItemText(IDC_EDIT_INST_LINE_WEIGHT , _T("0"));
	if(GetPrivateProfileString(sApp.c_str() , _T("Inst_CableT_Outline_Weight") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_INST_LINE_WEIGHT , szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Inst_CableT_Outline_OnOff") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_wndInstOutlineCheckBox.SetCheck(0 == STRICMP_T(_T("ON") , szBuf));
		this->OnBnClickedCheckInstOutline();
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Inst_CableT_FileName_Rule") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		SetDlgItemText(IDC_EDIT_INST_CABLE_TRAY_FILE_NAME , szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Inst_CableT_CellName") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_INST_CELL_NAME);
		const int at = pComboBox->FindString(-1 , szBuf);
		if(-1 != at) pComboBox->SetCurSel(at);
	}
	uiColorIdx = GetPrivateProfileInt(sApp.c_str() , _T("Inst_CableT_HatchColor") , 0 , m_sIniFilePath);
	m_wndInstHatchColorButton.SetColorIndex(uiColorIdx);

	if(GetPrivateProfileString(sApp.c_str() , _T("CableT_Fitting_Tag_OnOff") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_wndFittingTagCheckBox.SetCheck(0 == STRICMP_T(_T("ON") , szBuf));
	}
	
	if(GetPrivateProfileString(sApp.c_str() , _T("CableT_UpDown_Mark_OnOff") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		m_wndUpDownMarkCheckBox.SetCheck(0 == STRICMP_T(_T("ON") , szBuf));
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Cable_Tray_TextStyle") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int nFound = m_wndTextStyleComboBox.FindString(-1 , szBuf);
		if(-1 != nFound) m_wndTextStyleComboBox.SetCurSel(nFound);
	}

	uiColorIdx = GetPrivateProfileInt(sApp.c_str() , _T("CableT_TextColor") , 0 , m_sIniFilePath);
	m_wndTextColorButton.SetColorIndex(uiColorIdx);

	if(GetPrivateProfileString(sApp.c_str() , _T("Cable_Tray_ArrowType") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		const int nFound = m_wndArrowComboBox.FindString(-1 , szBuf);
		if(-1 != nFound) m_wndArrowComboBox.SetCurSel(nFound);
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-20
    @function   CCableTrayPage::SaveData
    @return     int
    @param      void
    @brief
******************************************************************************/
int PropertyPage::CCableTrayPage::SaveData(void)
{
	static const STRING_T sApp(_T("ANNOTATION_OPTIONS"));

	UpdateData(TRUE);

	CString rString;
	
	m_wndCellLibraryFileEdit.GetWindowText(rString);
	WritePrivateProfileString(sApp.c_str() , _T("CableT_Cell_Library") , rString , m_sIniFilePath);

	GetDlgItemText(IDC_EDIT_ROW_COUNT , rString);
	WritePrivateProfileString(sApp.c_str() , _T("CableT_Row_Count") , rString , m_sIniFilePath);

	GetDlgItemText(IDC_EDIT_ROW_HEIGHT , rString);
	WritePrivateProfileString(sApp.c_str() , _T("CableT_Row_Height") , rString , m_sIniFilePath);

	/// for Electric Cable Tray
	GetDlgItemText(IDC_EDIT_ELEC_LABEL , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Elec_Cable_Tray_Label") , rString , m_sIniFilePath);
	GetDlgItemText(IDC_EDIT_ELEC_EL_PREFIX , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Elec_Cable_Tray_EL_Prefix") , rString , m_sIniFilePath);
	{
		if(m_wndElecTagCheckBox.GetCheck())
		{
			WritePrivateProfileString(sApp.c_str() , _T("Elec_Cable_Tray_Tag_OnOff") , _T("ON") , m_sIniFilePath);
		}
		else
		{
			WritePrivateProfileString(sApp.c_str() , _T("Elec_Cable_Tray_Tag_OnOff") , _T("OFF") , m_sIniFilePath);
		}
	}
	GetDlgItemText(IDC_EDIT_ELEC_LINE_WEIGHT , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Elec_CableT_Outline_Weight") , rString , m_sIniFilePath);
	
	CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_ELEC_OUTLINE);
	WritePrivateProfileString(sApp.c_str() , _T("Elec_CableT_Outline_OnOff") , (m_wndElecOutlineCheckBox.GetCheck() ? _T("ON") : _T("OFF")) , m_sIniFilePath);

	GetDlgItemText(IDC_EDIT_ELEC_CABLE_TRAY_FILE_NAME , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Elec_CableT_FileName_Rule") , rString , m_sIniFilePath);

	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_ELEC_CELL_NAME);
	{
		const int iCurSel = pComboBox->GetCurSel();
		if(-1 != iCurSel)
		{
			pComboBox->GetLBText(iCurSel , rString);
			WritePrivateProfileString(sApp.c_str() , _T("Elec_CableT_CellName") , rString , m_sIniFilePath);
		}
	}
	{
		const int iColorIdx = m_wndElecHatchColorButton.GetColorIndex();
		rString.Format(_T("%d") , iColorIdx);
		WritePrivateProfileString(sApp.c_str() , _T("Elec_CableT_HatchColor") , rString , m_sIniFilePath);
	}

	/// for Inst. Cable Tray
	GetDlgItemText(IDC_EDIT_INST_LABEL , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Inst_Cable_Tray_Label") , rString , m_sIniFilePath);
	GetDlgItemText(IDC_EDIT_INST_EL_PREFIX , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Inst_Cable_Tray_EL_Prefix") , rString , m_sIniFilePath);
	{
		if(m_wndInstTagCheckBox.GetCheck())
		{
			WritePrivateProfileString(sApp.c_str() , _T("Inst_Cable_Tray_Tag_OnOff") , _T("ON") , m_sIniFilePath);
		}
		else
		{
			WritePrivateProfileString(sApp.c_str() , _T("Inst_Cable_Tray_Tag_OnOff") , _T("OFF") , m_sIniFilePath);
		}
	}
	GetDlgItemText(IDC_EDIT_INST_LINE_WEIGHT , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Inst_CableT_Outline_Weight") , rString , m_sIniFilePath);

	WritePrivateProfileString(sApp.c_str() , _T("Inst_CableT_Outline_OnOff") , (m_wndInstOutlineCheckBox.GetCheck() ? _T("ON") : _T("OFF")) , m_sIniFilePath);

	GetDlgItemText(IDC_EDIT_INST_CABLE_TRAY_FILE_NAME , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Inst_CableT_FileName_Rule") , rString , m_sIniFilePath);

	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_INST_CELL_NAME);
	{
		const int iCurSel = pComboBox->GetCurSel();
		if(-1 != iCurSel)
		{
			pComboBox->GetLBText(iCurSel , rString);
			WritePrivateProfileString(sApp.c_str() , _T("Inst_CableT_CellName") , rString , m_sIniFilePath);
		}
	}
	{
		const int iColorIdx = m_wndInstHatchColorButton.GetColorIndex();
		rString.Format(_T("%d") , iColorIdx);
		WritePrivateProfileString(sApp.c_str() , _T("Inst_CableT_HatchColor") , rString , m_sIniFilePath);
	}

	WritePrivateProfileString(sApp.c_str() , _T("CableT_Fitting_Tag_OnOff") , (m_wndFittingTagCheckBox.GetCheck() ? _T("ON") : _T("OFF")) , m_sIniFilePath);
	WritePrivateProfileString(sApp.c_str() , _T("CableT_UpDown_Mark_OnOff") , (m_wndUpDownMarkCheckBox.GetCheck() ? _T("ON") : _T("OFF")) , m_sIniFilePath);

	const int nSel = m_wndTextStyleComboBox.GetCurSel();
	if(-1 != nSel) m_wndTextStyleComboBox.GetLBText(nSel , rString);
	WritePrivateProfileString(sApp.c_str() , _T("Cable_Tray_TextStyle") , rString , m_sIniFilePath);

	{
		const int iColorIdx = m_wndTextColorButton.GetColorIndex();
		rString.Format(_T("%d") , iColorIdx);
		WritePrivateProfileString(sApp.c_str() , _T("CableT_TextColor") , rString , m_sIniFilePath);
	}

	{
		const int nSel = m_wndArrowComboBox.GetCurSel();
		if(-1 != nSel) m_wndArrowComboBox.GetLBText(nSel , rString);
		WritePrivateProfileString(sApp.c_str() , _T("Cable_Tray_ArrowType") , rString , m_sIniFilePath);
	}

	return 0;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-20
    @function   CCableTrayPage::NotifyDisplayTooltip
    @return     void
    @param      NMHDR   *
    @param      pNMHDR  LRESULT
    @param      *       result
    @brief
******************************************************************************/
void PropertyPage::CCableTrayPage::NotifyDisplayTooltip(NMHDR * pNMHDR, LRESULT * result)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	*result = 0;	
	NM_PPTOOLTIP_DISPLAY * pNotify = (NM_PPTOOLTIP_DISPLAY*)pNMHDR;

	const int iCtrlID = CWnd::FromHandle(pNotify->hwndTool)->GetDlgCtrlID();
	switch (iCtrlID)
	{
		case IDC_COMBO_TEXT_STYLE:
		{
		//Changes the tooltip's text
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(iCtrlID);
			const int nSel = pComboBox->GetCurSel();
			if(-1 != nSel)
			{
				CString rTextStyle , rToolTipString;
				pComboBox->GetLBText(nSel , rTextStyle);

				CString rTextStyleKeyName;
				if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
				{
					rTextStyleKeyName = CString(szBuf) + _T("_TEXT_STYLE");

					vector<string> oResult;
					if(GetPrivateProfileString(rTextStyleKeyName , rTextStyle , NULL , szBuf , MAX_PATH , m_sIniFilePath))
					{
						CTokenizer<CIsComma>::Tokenize(oResult , szBuf  , CIsComma());

						rToolTipString.Format(_T("<table><tr><td valign=vcenter><icon idres=141 width=32 height=32></td>"\
							"<td><center><h2>%s Text Style</h2><br><hr color=blue></center>"\
							"<br>Font = %s"\
							"<br>Text Height = %s"\
							"<br>Width Factor= %s"\
							"<br>Oblique = %s"\
							"<br>Weight = %s"\
							"</td></tr></table>") ,
							rTextStyle , 
							oResult[0].c_str() , oResult[1].c_str() , oResult[2].c_str() , oResult[3].c_str() , oResult[4].c_str()
							);
						pNotify->ti->sTooltip = rToolTipString;
					}
				}
			}
			break;
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-20
    @function   CCableTrayPage::PreTranslateMessage
    @return     BOOL
    @param      MSG*    pMsg
    @brief
******************************************************************************/
BOOL PropertyPage::CCableTrayPage::PreTranslateMessage(MSG* pMsg)
{
	m_tooltip.RelayEvent(pMsg); 

	return CSmartDrawPropertyPage::PreTranslateMessage(pMsg);
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-01
    @function   CCableTrayPage::OnEnChangeEditCellLibraryFilePath
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CCableTrayPage::OnEnChangeEditCellLibraryFilePath()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBitmapComboBox::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	auto_ptr<CDgnDocument> oDgnDoc(CDgnDocument::CreateInstance());
	CComboBox* pElecCellNameComboBox = (CComboBox*)(GetDlgItem(IDC_COMBO_ELEC_CELL_NAME));
	pElecCellNameComboBox->ResetContent();
	CComboBox* pInstCellNameComboBox = (CComboBox*)(GetDlgItem(IDC_COMBO_INST_CELL_NAME));
	pInstCellNameComboBox->ResetContent();

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
			pElecCellNameComboBox->AddString(pCellLibrary->name() + CString(_T(" / ")) + pCellLibrary->desc());
			pInstCellNameComboBox->AddString(pCellLibrary->name() + CString(_T(" / ")) + pCellLibrary->desc());
		}
	}

	pElecCellNameComboBox->SetCurSel(0);
	pInstCellNameComboBox->SetCurSel(0);
}

void PropertyPage::CCableTrayPage::OnBnClickedCheckElec()
{
	///m_wndElecTagCheckBox.SetCheck(!m_wndElecTagCheckBox.GetCheck());
	///m_wndElecTagCheckBox.SetWindowText(m_wndElecTagCheckBox.GetCheck() ? _T("ON") : _T("OFF"));
	GetDlgItem(IDC_EDIT_ELEC_LABEL)->EnableWindow(m_wndElecTagCheckBox.GetCheck());
	GetDlgItem(IDC_EDIT_ELEC_EL_PREFIX)->EnableWindow(m_wndElecTagCheckBox.GetCheck());
	///m_wndElecTagCheckBox.Invalidate();
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-02
    @function   CCableTrayPage::OnBnClickedCheckElecOutline
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CCableTrayPage::OnBnClickedCheckElecOutline()
{
	GetDlgItem(IDC_EDIT_ELEC_LINE_WEIGHT)->EnableWindow(m_wndElecOutlineCheckBox.GetCheck());
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-02
    @class      PropertyPage
    @function   CCableTrayPage::OnBnClickedCheckInst
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CCableTrayPage::OnBnClickedCheckInst()
{
	GetDlgItem(IDC_EDIT_INST_LABEL)->EnableWindow(m_wndInstTagCheckBox.GetCheck());
	GetDlgItem(IDC_EDIT_INST_EL_PREFIX)->EnableWindow(m_wndInstTagCheckBox.GetCheck());
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-02
    @class      PropertyPage
    @function   CCableTrayPage::OnBnClickedCheckInstOutline
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CCableTrayPage::OnBnClickedCheckInstOutline()
{
	GetDlgItem(IDC_EDIT_INST_LINE_WEIGHT)->EnableWindow(m_wndInstOutlineCheckBox.GetCheck());
}