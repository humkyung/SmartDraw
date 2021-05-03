// BorderPage.cpp : implementation file
//

#include "stdafx.h"
#include <isstring.h>
#include <Tokenizer.h>
#include <DgnV7Lib.h>
#include <util/SplitPath.h>
#include <util/FileTools.h>
#include <util/FileVersion.h>
#include "SmartDraw.h"
#include "SmartDrawPropertySheet.h"
#include "SmartDrawDocData.h"
#include "SmartDrawDoc.h"
#include "BorderPage.h"
#include "StringTable.h"
#include "SmartDrawPropertySheet.h"
#include "CADInteractive.h"

#include <fstream>
#include <string>
using namespace std;

using namespace PropertyPage;
// CBorderPage dialog

/**
	@brief	create a palette file from dgn file
	@author	humkyung
	@date
*/
static bool CreatePaletteFile(const CString& sPaletteFilePath , const CString& sDgnFilePath)
{
	bool res = false;

	CDgnFile dgnFile;
	if(dgnFile.Open(sDgnFilePath , CDgnFile::modeRead))
	{
		for(;dgnFile.ReadElement();)
		{
			const int nType = dgnFile.GetType();
			if((DGNT_GROUP_DATA == nType) && (1 == dgnFile.GetLevel()))
			{
				CDgnColorTable dgnColorTable;
				dgnColorTable.Parse(&dgnFile);

				OFSTREAM_T ofile(sPaletteFilePath);
				if(ofile.is_open())
				{
					unsigned char colors[256][3];
					dgnColorTable.GetColorTable(colors);
					for(int i = 0;i < 256;++i)
					{
						ofile << int(colors[i][0]) << _T(",") << int(colors[i][1]) << _T(",") << int(colors[i][2]) << std::endl;
					}
					ofile.close();

					res = true;
				}
				//break;
			}
		}
	}

	return res;
}

/**
	@brief	when select a border file
	@author	humkyung
*/
void CBorderFileBrowseEdit::OnBrowse()
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	CString rOutputFormat = pDoc->GetOutputFormatString().c_str();
	
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	CStringTable& table = CStringTable::GetInstance();

	CString sBorderFilePath;
	this->GetWindowText(sBorderFilePath);
	CSplitPath path(sBorderFilePath);
	CString sBorderFileFolder = path.GetDrive() + path.GetDirectory();
	if(pDoc->IsMSTNOutput())
	{
		/// 파일 Open 
		CFileDialog dlg(TRUE,table.GetString(_T("IDS_BORDER_FILE_EXT")).c_str(),_T(""),OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
			table.GetString(_T("IDS_BORDER_FILE_FILTER")).c_str());
		dlg.m_ofn.lpstrInitialDir = sBorderFileFolder;
		if(IDOK == dlg.DoModal())
		{
			m_rFilePath = dlg.GetPathName();
			
			const CString rDestFilePath = docData.GetServerFolderPath() + _T("\\") + m_rProjectName + _T("\\Border\\") + dlg.GetFileName();
			CString rPaletteFilePath = rDestFilePath;
			const int nFound = rPaletteFilePath.ReverseFind('.');
			if(-1 != nFound)
			{
				rPaletteFilePath = rPaletteFilePath.Left(nFound) + _T(".PAL");
			}
			
			/*try
			{*/
				bool bCreatePaletteFile = false;
				if(TRUE == ::CopyFile(m_rFilePath , rDestFilePath , FALSE))
				{
					bCreatePaletteFile = CreatePaletteFile(rPaletteFilePath , rDestFilePath);
				}

				if(false == bCreatePaletteFile)
				{
					::CopyFile(GetExecPath() + _T("\\Setting\\MSTN.PAL") , rPaletteFilePath , FALSE);
					bCreatePaletteFile = true;
				}
				
				/// copy default border configuration file - 2012.04.28 added by humkyung
				::CopyFile(GetExecPath() + _T("\\Backup\\border.dgn.cfg") , rDestFilePath + _T(".cfg") ,TRUE);
				/// up to here

				::WritePrivateProfileString(_T("GENERAL") , _T("Border File") , dlg.GetFileName() , m_rIniFilePath);
				/// reload palette file
				{
					CSmartDrawDoc* pDoc = GetMDIActiveDocument();
					pDoc->GetColorListOf(m_rIniFilePath.operator LPCTSTR() , true);
				}

				SetWindowText(dlg.GetFileName());	/// display just file name
				((CBorderPage*)GetParent())->LoadData();
				{
					CSmartDrawPropertySheet* pSheet = (CSmartDrawPropertySheet*)(GetParent()->GetParent());
					pSheet->OnBorderChanged();
				}
			/*}
			catch(const std::exception& ex)
			{
				AfxMessageBox(ex.what() , MB_OK);
			}*/
		}
	}
	else if(pDoc->IsAutoCADOutput())
	{
		/// 파일 Open 
		CFileDialog dlg(TRUE,_T("dwg,*.*"),_T(""),OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
			_T("AutoCAD File(*.dwg)|*.dwg|모든 문서(*.*)|*.*||"));
		dlg.m_ofn.lpstrInitialDir = sBorderFileFolder;
		if(IDOK == dlg.DoModal())
		{
			m_rFilePath = dlg.GetPathName();
			SetWindowText(dlg.GetFileName());

			CString rDestFilePath = docData.GetServerFolderPath() + _T("\\") + m_rProjectName + _T("\\Border\\") + dlg.GetFileName();
			::CopyFile(m_rFilePath , rDestFilePath , FALSE);

			::WritePrivateProfileString(_T("GENERAL") , _T("Border File") , dlg.GetFileName() , m_rIniFilePath);
			((CBorderPage*)GetParent())->LoadData();
		}
	}
}

/**
	@brief	fill a combobox with cell name
	@author	humkyung
	@date	2014.06.05
*/
static int FillCellNameInComboBox(CComboBox* pComboBox , LPCTSTR pCellLibFilePath)
{
	assert(pComboBox && pCellLibFilePath && "pComboBox or pCellLibFilePath is NULL");

	if(pComboBox && pCellLibFilePath)
	{
		auto_ptr<CDgnDocument> oDgnDoc(CDgnDocument::CreateInstance());
		pComboBox->ResetContent();
		
		unsigned char r=0 , g=0 , b=0;
		vector<int> oParsingTypes;
		{
			oParsingTypes.push_back(DGNT_CELL_LIBRARY);
		}
		oDgnDoc->Read(pCellLibFilePath , CDgnDocument::UOR_BASE , &oParsingTypes);
		const int iCount = oDgnDoc->GetEntityCount();
		for(int i = 0;i < iCount;++i)
		{
			CDgnElement* pDgnElm = oDgnDoc->GetEntityAt(i);
			if(pDgnElm->IsKindOf(CDgnCellLibrary::TypeString()))
			{
				CDgnCellLibrary* pCellLibrary = static_cast<CDgnCellLibrary*>(pDgnElm);
				pComboBox->AddString(CString(pCellLibrary->name()));
			}
		}

		pComboBox->SetCurSel(0);

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

void CaDrawFileBrowseEdit::OnBrowse()
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	CString rOutputFormat , rBorderConfigFilePath;
	if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
	{
		rOutputFormat = szBuf;
		rOutputFormat.MakeUpper();

		if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , NULL , szBuf , MAX_PATH , m_rIniFilePath))
		{
			CString rBorderFileName(szBuf);
			rBorderConfigFilePath = docData.GetServerFolderPath() + _T("\\") + m_rProjectName + _T("\\Border\\") + rBorderFileName + _T(".cfg");
		}
	}

	CString rDestFilePath;
	if(!rBorderConfigFilePath.IsEmpty() && (_T("MSTN") == rOutputFormat)/* && (!m_rScale.IsEmpty() && (CaDrawFileBrowseEdit::SCALE_BAR == m_FileType))*/)
	{
		/// 파일 Open 
		CFileDialog dlg(TRUE,_T("*.cel,*.dgn,*.*"),_T(""),OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
			_T("Cell Lib File(*.cel),dgn File(*.dgn)|*.cel;*.dgn|모든 문서(*.*)|*.*||"));
		if(IDOK == dlg.DoModal())
		{
			m_rFilePath = dlg.GetPathName();
			
			switch(m_eFileType)
			{
				case CaDrawFileBrowseEdit::KEY_PLAN:
					rDestFilePath = docData.GetServerFolderPath() + _T("\\") + m_rProjectName + _T("\\Border\\KeyPlan\\") + dlg.GetFileName();
					::CopyFile(m_rFilePath , rDestFilePath , FALSE);
					::WritePrivateProfileString(_T("Key Plan") , _T("FileName") , rDestFilePath , rBorderConfigFilePath);
				break;
				case CaDrawFileBrowseEdit::NORTH_MARK:
					rDestFilePath = docData.GetServerFolderPath() + _T("\\") + m_rProjectName + _T("\\Border\\NorthMark\\") + dlg.GetFileName();
					::CopyFile(m_rFilePath , rDestFilePath , FALSE);
					::WritePrivateProfileString(_T("North Mark") , _T("FileName") , rDestFilePath , rBorderConfigFilePath);
				break;
				case CaDrawFileBrowseEdit::SCALE_BAR:
					rDestFilePath = docData.GetServerFolderPath() + _T("\\") + m_rProjectName + _T("\\Border\\ScaleBar\\") + m_rScale + _T(".dgn");
					::CopyFile(m_rFilePath , rDestFilePath , FALSE);
					::WritePrivateProfileString(_T("Scale Bar") , _T("FileName") , rDestFilePath , rBorderConfigFilePath);
				break;
			}
			SetWindowText(rDestFilePath);
		}
	}
	else if(!rBorderConfigFilePath.IsEmpty() && (_T("AUTOCAD") == rOutputFormat) && (!m_rScale.IsEmpty() && (CaDrawFileBrowseEdit::SCALE_BAR == m_eFileType)))
	{
		/// 파일 Open 
		CFileDialog dlg(TRUE,_T("dwg,*.*"),_T(""),OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
			_T("AutoCAD File(*.dwg)|*.dwg|모든 문서(*.*)|*.*||"));
		if(IDOK == dlg.DoModal())
		{
			m_rFilePath = dlg.GetPathName();
			SetWindowText(dlg.GetFileName());

			switch(m_eFileType)
			{
				case CaDrawFileBrowseEdit::KEY_PLAN:
					rDestFilePath = docData.GetServerFolderPath() + _T("\\") + m_rProjectName + _T("\\Border\\KeyPlan\\") + dlg.GetFileName();
					::CopyFile(m_rFilePath , rDestFilePath , FALSE);
					::WritePrivateProfileString(_T("Key Plan") , _T("FileName") , dlg.GetFileName() , rBorderConfigFilePath);
					break;
				case CaDrawFileBrowseEdit::NORTH_MARK:
					rDestFilePath = docData.GetServerFolderPath() + _T("\\") + m_rProjectName + _T("\\Border\\NorthMark\\") + dlg.GetFileName();
					::CopyFile(m_rFilePath , rDestFilePath , FALSE);
					::WritePrivateProfileString(_T("North Mark") , _T("FileName") , dlg.GetFileName() , rBorderConfigFilePath);
					break;
				case CaDrawFileBrowseEdit::SCALE_BAR:
					rDestFilePath = docData.GetServerFolderPath() + _T("\\") + m_rProjectName + _T("\\Border\\ScaleBar\\") + m_rScale + _T(".dwg");
					::CopyFile(m_rFilePath , rDestFilePath , FALSE);
					::WritePrivateProfileString(_T("Scale Bar") , _T("FileName") , dlg.GetFileName() , rBorderConfigFilePath);
					break;
			}
		}
	}
}

IMPLEMENT_DYNAMIC(CBorderPage, CSmartDrawPropertyPage)

CBorderPage::CBorderPage(CWnd* pParent /*=NULL*/)
	: CSmartDrawPropertyPage(CBorderPage::IDD, pParent)
{

}

CBorderPage::~CBorderPage()
{
}

void CBorderPage::DoDataExchange(CDataExchange* pDX)
{
	CSmartDrawPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_EDIT_BORDER_FILE_NAME , m_wndBorderFileEdit);
	DDX_Control(pDX , IDC_EDIT_KEY_PLAN_FILE , m_wndKeyPlanFileEdit);
	DDX_Control(pDX , IDC_EDIT_NORTH_MARK_FILE , m_wndNorthMarkFileEdit);
	DDX_Text(pDX , IDC_EDIT_BORDER_DESC , m_rBorderDesc);
	DDX_Text(pDX , IDC_EDIT_BORDER_SCALE , m_sBorderScale);	/// 2014.06.09 added by humkyung

	DDX_Text(pDX , IDC_EDIT_SCALE_VALUE , m_rScaleValue);
	DDX_Control(pDX , IDC_EDIT_SCALE_BAR_FILE , m_wndScaleBarFileEdit);

	DDX_Control(pDX , IDC_STATIC_denominator , m_wndDenominatorStatic);
	DDX_Control(pDX , IDC_STATIC_X , m_wndXStatic);
	DDX_Control(pDX , IDC_STATIC_Y , m_wndYStatic);
	DDX_Control(pDX , IDC_STATIC_SCALE , m_wndScaleStatic);

	DDX_Control(pDX , IDC_BUTTON_KEY_PLAN_PICK , m_wndKeyPlanPickButton);
	DDX_Control(pDX , IDC_BUTTON_NORTH_MARK_PICK , m_wndNorthMarkPickButton);
	DDX_Control(pDX , IDC_BUTTON_SCALE_BAR_PICK , m_wndScaleBarPickButton);
}


BEGIN_MESSAGE_MAP(CBorderPage, CSmartDrawPropertyPage)
	ON_EN_CHANGE(IDC_EDIT_SCALE_VALUE, &CBorderPage::OnEnChangeEditScaleValue)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_BORDER_FILE, &CBorderPage::OnBnClickedButtonOpenBorderFile)
	ON_BN_CLICKED(IDC_BUTTON_KEY_PLAN_PICK, &CBorderPage::OnBnClickedButtonKeyPlanPick)
	ON_BN_CLICKED(IDC_BUTTON_NORTH_MARK_PICK, &CBorderPage::OnBnClickedButtonNorthMarkPick)
	ON_EN_CHANGE(IDC_EDIT_BORDER_FILE_NAME, &CBorderPage::OnEnChangeEditBorderFile)
	ON_EN_CHANGE(IDC_EDIT_KEY_PLAN_FILE, &CBorderPage::OnEnChangeEditKeyPlanFile)
	ON_EN_CHANGE(IDC_EDIT_NORTH_MARK_FILE, &CBorderPage::OnEnChangeEditNorthMarkFile)
END_MESSAGE_MAP()


// CBorderPage message handlers

BOOL PropertyPage::CBorderPage::OnInitDialog()
{
	CSmartDrawPropertyPage::OnInitDialog();
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	CSmartDrawPropertySheet *pParent = (CSmartDrawPropertySheet*)GetParent();
	m_sIniFilePath = pParent->GetSettingFilePath();
	m_sProjectName     = pParent->GetProjectName();

	m_wndBorderFileEdit.m_rIniFilePath = m_sIniFilePath;
	m_wndBorderFileEdit.m_rProjectName = m_sProjectName;
	m_wndBorderFileEdit.EnableFileBrowseButton(); /// To show file open dialog

	m_wndKeyPlanFileEdit.m_eFileType = CaDrawFileBrowseEdit::KEY_PLAN;
	m_wndKeyPlanFileEdit.m_rIniFilePath = m_sIniFilePath;
	m_wndKeyPlanFileEdit.m_rProjectName = m_sProjectName;
	m_wndKeyPlanFileEdit.EnableFileBrowseButton();
	GetDlgItem(IDC_COMBO_KEY_PLAN_CELL_NAME)->EnableWindow(pDoc->IsMSTNOutput());

	m_wndNorthMarkFileEdit.m_eFileType = CaDrawFileBrowseEdit::NORTH_MARK;
	m_wndNorthMarkFileEdit.m_rIniFilePath = m_sIniFilePath;
	m_wndNorthMarkFileEdit.m_rProjectName = m_sProjectName;
	m_wndNorthMarkFileEdit.EnableFileBrowseButton();
	GetDlgItem(IDC_COMBO_NORTH_MARK_CELL_NAME)->EnableWindow(pDoc->IsMSTNOutput());

	{
		LOGFONT LogFont;
		CFont BoldFont;
		BoldFont.CreateFont(16,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0, _T("Arial"));
		BoldFont.GetLogFont(&LogFont);
		m_wndDenominatorStatic.SetTransparent(TRUE);
		m_wndDenominatorStatic.SetFont(LogFont);
		m_wndDenominatorStatic.SetTextColor(RGB(0,0,255));
		m_wndXStatic.SetTransparent(TRUE);
		m_wndXStatic.SetFont(LogFont);
		m_wndXStatic.SetTextColor(RGB(0,0,0));
		m_wndYStatic.SetTransparent(TRUE);
		m_wndYStatic.SetFont(LogFont);
		m_wndYStatic.SetTextColor(RGB(0,0,0));
		m_wndScaleStatic.SetTransparent(TRUE);
		m_wndScaleStatic.SetFont(LogFont);
		m_wndScaleStatic.SetTextColor(RGB(0,0,0));
	}
	m_wndScaleBarFileEdit.m_eFileType = CaDrawFileBrowseEdit::SCALE_BAR;
	m_wndScaleBarFileEdit.m_rIniFilePath = m_sIniFilePath;
	m_wndScaleBarFileEdit.m_rProjectName = m_sProjectName;
	m_wndScaleBarFileEdit.EnableFileBrowseButton();

	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_PAPER_SIZE);
	for(list<CSmartDrawDocData::PaperSize>::iterator itr = docData.m_PaperSizeList.begin();itr != docData.m_PaperSizeList.end();++itr)
	{
		pComboBox->AddString(itr->name.c_str());
	}

	m_wndScaleBarFileEdit.EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SCALE_BAR_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SCALE_BAR_Y)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SCALE_BAR_PICK)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SCALE_BAR_SCALE)->EnableWindow(FALSE);

	{
		HICON hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_MOUSE_PICK),IMAGE_ICON,0,0,0);
		HICON hOldIcon = m_wndKeyPlanPickButton.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		
		hOldIcon = m_wndNorthMarkPickButton.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);

		hOldIcon = m_wndScaleBarPickButton.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);

		hIcon = (HICON)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDI_OPEN),IMAGE_ICON,16,16, 0 );
		((CButton*)GetDlgItem(IDC_BUTTON_OPEN_BORDER_FILE))->SetIcon(hIcon);
	}

	LoadData();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.14
*/
int PropertyPage::CBorderPage::LoadData(void)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();

	if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , NULL , szBuf , MAX_PATH , m_sIniFilePath))
	{
		GetDlgItem(IDC_EDIT_BORDER_FILE_NAME)->SetWindowText(szBuf);

		CString rBorderFileName(szBuf);
		m_rBorderConfigFilePath = docData.GetServerFolderPath() + _T("\\") + m_sProjectName + _T("\\Border\\") + rBorderFileName + _T(".cfg");

		if(GetPrivateProfileString(_T("GENERAL") , _T("PAPER_SIZE") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_PAPER_SIZE);
			const int nFound = pComboBox->FindString(-1 , szBuf);
			pComboBox->SetCurSel(nFound);
		}
		if(GetPrivateProfileString(_T("GENERAL") , _T("BORDER_DESC") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
		{
			m_rBorderDesc = szBuf;
		}
		if(GetPrivateProfileString(_T("GENERAL") , _T("Border_Scale") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
		{
			m_sBorderScale = szBuf;
		}

		vector<STRING_T> oResult;
		if(GetPrivateProfileString(_T("Key Plan") , _T("FileName") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
		{
			m_wndKeyPlanFileEdit.SetWindowText(szBuf);
		}
		if(pDoc->IsMSTNOutput())
		{
			if(GetPrivateProfileString(_T("Key Plan") , _T("CellName") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
			{
				CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_KEY_PLAN_CELL_NAME);
				const int iFound = pComboBox->FindStringExact(0 , szBuf);
				(-1 != iFound) ? pComboBox->SetCurSel(iFound) : pComboBox->SetCurSel(0);
			}
		}
		else
		{
			GetDlgItem(IDC_COMBO_KEY_PLAN_CELL_NAME)->SetWindowText(_T("NONE"));
		}

		if(GetPrivateProfileString(_T("Key Plan") , _T("Pos") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(2 == oResult.size())
			{
				SetDlgItemText(IDC_EDIT_KEY_PLAN_X , oResult[0].c_str());
				SetDlgItemText(IDC_EDIT_KEY_PLAN_Y , oResult[1].c_str());
			}
		}
		if(GetPrivateProfileString(_T("Key Plan") , _T("Scale") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
		{
			SetDlgItemText(IDC_EDIT_KEY_PLAN_SCALE , szBuf);
		}

		if(GetPrivateProfileString(_T("North Mark") , _T("FileName") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
		{
			m_wndNorthMarkFileEdit.SetWindowText(szBuf);
		}
		if(pDoc->IsMSTNOutput())
		{
			if(GetPrivateProfileString(_T("North Mark") , _T("CellName") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
			{
				CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_NORTH_MARK_CELL_NAME);
				const int iFound = pComboBox->FindStringExact(0 , szBuf);
				(-1 != iFound) ? pComboBox->SetCurSel(iFound) : pComboBox->SetCurSel(0);
			}
		}
		else
		{
			GetDlgItem(IDC_COMBO_NORTH_MARK_CELL_NAME)->SetWindowText(_T("NONE"));
		}
		if(GetPrivateProfileString(_T("North Mark") , _T("Pos") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(2 == oResult.size())
			{
				SetDlgItemText(IDC_EDIT_NORTH_MARK_X , oResult[0].c_str());
				SetDlgItemText(IDC_EDIT_NORTH_MARK_Y , oResult[1].c_str());
			}
		}
		if(GetPrivateProfileString(_T("North Mark") , _T("Scale") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
		{
			SetDlgItemText(IDC_EDIT_NORTH_MARK_SCALE , szBuf);
		}

		if(GetPrivateProfileString(_T("Scale Bar") , _T("FileName") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
		{
			m_wndScaleBarFileEdit.SetWindowText(szBuf);
		}
		if(GetPrivateProfileString(_T("Scale Bar") , _T("ScaleValue") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
		{
			SetDlgItemText(IDC_EDIT_SCALE_VALUE , szBuf);
		}
		if(GetPrivateProfileString(_T("Scale Bar") , _T("Pos") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(2 == oResult.size())
			{
				SetDlgItemText(IDC_EDIT_SCALE_BAR_X , oResult[0].c_str());
				SetDlgItemText(IDC_EDIT_SCALE_BAR_Y , oResult[1].c_str());
			}
		}
		if(GetPrivateProfileString(_T("Scale Bar") , _T("Scale") , NULL , szBuf , MAX_PATH , m_rBorderConfigFilePath))
		{
			SetDlgItemText(IDC_EDIT_SCALE_BAR_SCALE , szBuf);
		}
	}

	return ERROR_SUCCESS;
}

void PropertyPage::CBorderPage::OnEnChangeEditScaleValue()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	UpdateData();
	m_wndScaleBarFileEdit.m_rScale = m_rScaleValue;
	
	m_wndScaleBarFileEdit.EnableWindow(!m_rScaleValue.IsEmpty());
	GetDlgItem(IDC_EDIT_SCALE_BAR_X)->EnableWindow(!m_rScaleValue.IsEmpty());
	GetDlgItem(IDC_EDIT_SCALE_BAR_Y)->EnableWindow(!m_rScaleValue.IsEmpty());
	GetDlgItem(IDC_BUTTON_SCALE_BAR_PICK)->EnableWindow(!m_rScaleValue.IsEmpty());
	GetDlgItem(IDC_EDIT_SCALE_BAR_SCALE)->EnableWindow(!m_rScaleValue.IsEmpty());
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-01
    @function   CBorderPage::SaveData
    @return     int
    @param      void
    @brief
******************************************************************************/
int PropertyPage::CBorderPage::SaveData(void)
{
	UpdateData();

	CString rString , rTemp;

	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_PAPER_SIZE);
	const int nSel = pComboBox->GetCurSel();
	if(-1 != nSel)
	{
		pComboBox->GetLBText(nSel , rString);
		WritePrivateProfileString(_T("GENERAL") , _T("PAPER_SIZE") , rString , m_rBorderConfigFilePath);
	}
	WritePrivateProfileString(_T("GENERAL") , _T("BORDER_DESC") , m_rBorderDesc , m_rBorderConfigFilePath);
	WritePrivateProfileString(_T("GENERAL") , _T("Border_Scale") , m_sBorderScale , m_rBorderConfigFilePath);	/// 2014.06.09 added by humkyung
	
	m_wndKeyPlanFileEdit.GetWindowText(rString);
	WritePrivateProfileString(_T("Key Plan") , _T("FileName") , rString , m_rBorderConfigFilePath);
	GetDlgItem(IDC_COMBO_KEY_PLAN_CELL_NAME)->GetWindowText(rString);
	WritePrivateProfileString(_T("Key Plan") , _T("CellName") , rString , m_rBorderConfigFilePath);

	GetDlgItemText(IDC_EDIT_KEY_PLAN_X , rTemp);
	rString = rTemp;
	GetDlgItemText(IDC_EDIT_KEY_PLAN_Y , rTemp);
	rString += _T(",") + rTemp;
	WritePrivateProfileString(_T("Key Plan") , _T("Pos") , rString , m_rBorderConfigFilePath);
	GetDlgItemText(IDC_EDIT_KEY_PLAN_SCALE , rString);
	WritePrivateProfileString(_T("Key Plan") , _T("Scale") , rString , m_rBorderConfigFilePath);

	m_wndNorthMarkFileEdit.GetWindowText(rString);
	WritePrivateProfileString(_T("North Mark") , _T("FileName") , rString , m_rBorderConfigFilePath);
	GetDlgItem(IDC_COMBO_NORTH_MARK_CELL_NAME)->GetWindowText(rString);
	WritePrivateProfileString(_T("North Mark") , _T("CellName") , rString , m_rBorderConfigFilePath);

	GetDlgItemText(IDC_EDIT_NORTH_MARK_X , rTemp);
	rString = rTemp;
	GetDlgItemText(IDC_EDIT_NORTH_MARK_Y , rTemp);
	rString += _T(",") + rTemp;
	WritePrivateProfileString(_T("North Mark") , _T("Pos") , rString , m_rBorderConfigFilePath);
	GetDlgItemText(IDC_EDIT_NORTH_MARK_SCALE , rString);
	WritePrivateProfileString(_T("North Mark") , _T("Scale") , rString , m_rBorderConfigFilePath);

	m_wndScaleBarFileEdit.GetWindowText(rString);
	WritePrivateProfileString(_T("Scale Bar") , _T("FileName") , rString , m_rBorderConfigFilePath);
	GetDlgItemText(IDC_EDIT_SCALE_VALUE , rString);
	WritePrivateProfileString(_T("Scale Bar") , _T("ScaleValue") , rString , m_rBorderConfigFilePath);
	GetDlgItemText(IDC_EDIT_SCALE_BAR_X , rTemp);
	rString = rTemp;
	GetDlgItemText(IDC_EDIT_SCALE_BAR_Y , rTemp);
	rString += _T(",") + rTemp;
	WritePrivateProfileString(_T("Scale Bar") , _T("Pos") , rString , m_rBorderConfigFilePath);
	GetDlgItemText(IDC_EDIT_SCALE_BAR_SCALE , rString);
	WritePrivateProfileString(_T("Scale Bar") , _T("Scale") , rString , m_rBorderConfigFilePath);

	return 0;
}

/**
	@brief	open border file
	@author	humkyung
	@date	2014.06.21
*/
void PropertyPage::CBorderPage::OnBnClickedButtonOpenBorderFile()
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	CString rString;
	GetDlgItemText(IDC_EDIT_BORDER_FILE_NAME,rString);
	const CString sBorderFilePath = docData.GetServerFolderPath() + _T("\\") + m_sProjectName + _T("\\Border\\") + rString;

	if(CFileTools::DoesFileExist(sBorderFilePath))
	{
		CSmartDrawDoc* pDoc = GetMDIActiveDocument();
		CString rMSTNJFilePath = docData.GetMSTNJFilePath();
		if(pDoc->IsMSTNOutput())
		{
			/// open mstn j and write outputs
			const CString sMSTNJPath = docData.GetMSTNJFilePath();
			if(!sMSTNJPath.IsEmpty())
			{
				/// delete already exist file
				STRING_T sDataFile(CFileTools::GetTempPath().operator LPCTSTR() + STRING_T(_T("aPickTool.dat")));
				::DeleteFile(sDataFile.c_str());
				/// up to here

				const long length = GetShortPathName(CFileTools::GetTempPath(), NULL, 0);
				if (length > 0)
				{
					TCHAR* buffer = new TCHAR[length];
					if(GetShortPathName(CFileTools::GetTempPath(), buffer, length) > 0)
					{
						/// write run command file - 2016.02.11 added by humkyung
						const CString sCommandFilePath = CString(buffer) + _T("run.cmd");
						OFSTREAM_T ofile(sCommandFilePath.operator LPCTSTR());
						if(ofile.is_open())
						{
							ofile << _T("MDL LOAD aPickTool \"") << CString(buffer) << _T("aPickTool.dat\" ") << std::endl;
							ofile.close();
						}
						/// up to here

						const CString sArguments(_T("\"") + sBorderFilePath + _T("\" -S") + CString(buffer) + _T("run.cmd"));
						::ShellExecute(NULL , _T("open") , sMSTNJPath , sArguments , NULL , SW_SHOW);
						delete []buffer;
					}
				}
			}
			/// up to here
		}
		else if(pDoc->IsAutoCADOutput())
		{
			const CString sCADAppPath = pDoc->GetCADAppPath();
			CFileVersion version;
			version.Open(sCADAppPath);
			int pos = 0;
			CString sVerNo = version.GetProductVersion().Tokenize(_T(".") , pos).TrimLeft('R');
			
			const STRING_T sAutoCADPluginsPath = CFileTools::GetCommonAppDataPath() + _T("\\Autodesk\\ApplicationPlugins");
			const STRING_T sPluginPath = sAutoCADPluginsPath + _T("\\SmartDraw\\") + STRING_T(sVerNo.operator LPCTSTR());
			const STRING_T sStartUpFileName = sPluginPath + _T("\\StartUp.scr");
			
			OFSTREAM_T oFile(sStartUpFileName.c_str());
			{
				STRING_T sDllFilePath = sAutoCADPluginsPath + _T("\\SmartDraw\\") + STRING_T(sVerNo.operator LPCTSTR()) + _T("\\DwgWriter.NET.dll");
				IsString::ReplaceOf(sDllFilePath , _T("\\") , _T("/"));
				oFile << _T("(command \"netload\" \"") << sDllFilePath << _T("\") ");
				oFile.close();
			}

			STRING_T sParam;
			sParam = sBorderFilePath.operator LPCTSTR() + STRING_T(_T(" /b \"")) + sStartUpFileName + _T("\"");
			::ShellExecute(NULL , _T("open") , sCADAppPath , sParam.c_str() , NULL , SW_SHOW);
		}
	}
	else
	{
		CStringTable& table = CStringTable::GetInstance();
	}
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.28
*/
void PropertyPage::CBorderPage::OnBnClickedButtonKeyPlanPick()
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		CString rMSTNJFilePath = docData.GetMSTNJFilePath();
		
		CString rString;
		GetDlgItemText(IDC_EDIT_BORDER_FILE_NAME,rString);
		const CString rDestDgnFilePath = docData.GetServerFolderPath() + _T("\\") + m_sProjectName + _T("\\Border\\") + rString;

///		CString rString;
///		GetDlgItemText(IDC_EDIT_BORDER_FILE_NAME , rString);

		GetTempPath(MAX_PATH , szBuf);
		CString rCmdFilePath = szBuf + CString(_T("run.cmd"));
		double x = 0.0 , y = 0.0;
		int res = CCADInteractive::GetInputPoint(x , y , rDestDgnFilePath.operator LPCTSTR() , rCmdFilePath.operator LPCTSTR());
		if(ERROR_SUCCESS == res)
		{
			CString sString;
			sString.Format(_T("%lf") , x);
			SetDlgItemText(IDC_EDIT_KEY_PLAN_X , sString);
			sString.Format(_T("%lf") , y);
			SetDlgItemText(IDC_EDIT_KEY_PLAN_Y , sString);
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-01
    @function   CBorderPage::OnBnClickedButtonNorthMarkPick
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CBorderPage::OnBnClickedButtonNorthMarkPick()
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		CString rMSTNJFilePath = docData.GetMSTNJFilePath();
		
		CString rString;
		GetDlgItemText(IDC_EDIT_BORDER_FILE_NAME,rString);
		const CString rDestDgnFilePath = docData.GetServerFolderPath() + _T("\\") + m_sProjectName + _T("\\Border\\") + rString;

		GetTempPath(MAX_PATH , szBuf);
		CString rCmdFilePath = szBuf + CString(_T("run.cmd"));
		double x = 0.0 , y = 0.0;
		int res = CCADInteractive::GetInputPoint(x , y , rDestDgnFilePath.operator LPCTSTR() , rCmdFilePath.operator LPCTSTR());
		if(ERROR_SUCCESS == res)
		{
			CString sString;
			sString.Format(_T("%lf") , x);
			SetDlgItemText(IDC_EDIT_NORTH_MARK_X , sString);
			sString.Format(_T("%lf") , y);
			SetDlgItemText(IDC_EDIT_NORTH_MARK_Y , sString);
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-28
    @function   CBorderPage::OnEnChangeEditBorderFile
    @return     void
    @brief
******************************************************************************/
void PropertyPage::CBorderPage::OnEnChangeEditBorderFile()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	//this->LoadData();
}

void PropertyPage::CBorderPage::OnEnChangeEditKeyPlanFile()
{
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	CString sFileName;
	GetDlgItemText(IDC_EDIT_KEY_PLAN_FILE , sFileName);
	FillCellNameInComboBox((CComboBox*)GetDlgItem(IDC_COMBO_KEY_PLAN_CELL_NAME) , sFileName);
}

void PropertyPage::CBorderPage::OnEnChangeEditNorthMarkFile()
{
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	CString sFileName;
	GetDlgItemText(IDC_EDIT_NORTH_MARK_FILE , sFileName);
	FillCellNameInComboBox((CComboBox*)GetDlgItem(IDC_COMBO_NORTH_MARK_CELL_NAME) , sFileName);
}