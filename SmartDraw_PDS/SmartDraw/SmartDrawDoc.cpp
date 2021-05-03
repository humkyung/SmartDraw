// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// CSmartDrawDoc.cpp : implementation of the CSmartDrawDoc class
//

#include "stdafx.h"
#include "SmartDraw.h"

#include <Tokenizer.h>
#include <util/FileTools.h>
#include <util/excel9/ExcelAutomation.h>
#include <util/SplitPath.h>

#include <aDraw_PipeDiaTable.h>
#include <SmartDrawPDSModuleImpl.h>
#include "SmartDrawDoc.h"
#include "SmartDrawDocData.h"
#include "SmartDrawView.h"
#include "ProjectSettingSheet.h"
#include "GenerateDraFromDrawingDlg.h"
#include "RevisionManagerDlg.h"
#include "ProjectExplorer.h"
#include "PropertiesWnd.h"
#include "OutputWnd.h"
#include "InsertRevNoDlg.h"
#include "PipeDiaEditorDlg.h"
#include "FillMatchlineDataDlg.h"
#include "PluginManager.h"

#include "soci.h"
#include "soci-sqlite3.h"

#include "soapLicWebServiceSoapProxy.h"

#include <fstream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern "C" __declspec(dllimport) int __stdcall Encode(LMS_ENCODE* param);
extern "C" __declspec(dllimport) int __stdcall Decode(LMS_ENCODE* param);

// CSmartDrawDoc

IMPLEMENT_DYNCREATE(CSmartDrawDoc, CDocument)

BEGIN_MESSAGE_MAP(CSmartDrawDoc, CDocument)
	ON_COMMAND(ID_PROJECT_SETTING , &CSmartDrawDoc::OnProjectSetting)
	ON_COMMAND(ID_GENERATION_DRA, &CSmartDrawDoc::OnGenerateDra)
	ON_COMMAND(ID_EDIT_REVISION_DATA , &CSmartDrawDoc::OnEditRevisionData)
	ON_UPDATE_COMMAND_UI(ID_GENERATION_HLR, &CSmartDrawDoc::OnUpdateGenerateHLR)
	ON_COMMAND(ID_GENERATION_HLR, &CSmartDrawDoc::OnGenerateHLR)
	ON_UPDATE_COMMAND_UI(ID_GENERATION_ANNOTATION, &CSmartDrawDoc::OnUpdateGenerateAnnotation)
	ON_COMMAND(ID_GENERATION_ANNOTATION, &CSmartDrawDoc::OnGenerateAnnotation)
	ON_COMMAND(ID_GENERATION_GENERATE, &CSmartDrawDoc::OnGenerate)
	ON_UPDATE_COMMAND_UI(ID_GENERATION_GENERATE, &CSmartDrawDoc::OnUpdateGenerate)
	ON_COMMAND(ID_INSERT_REV_NO, &CSmartDrawDoc::OnInsertRevNo)
	ON_UPDATE_COMMAND_UI(ID_INSERT_REV_NO, &CSmartDrawDoc::OnUpdateInsertRevNo)
	ON_COMMAND(ID_UPDATE_TITLE_BLOCK, &CSmartDrawDoc::OnUpdateTitleBlock)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_TITLE_BLOCK, &CSmartDrawDoc::OnUpdateUpdateTitleBlock)
	ON_COMMAND(ID_FILL_UP_MATCH_LINE_DATA, &CSmartDrawDoc::OnFillMatchlineData)
	ON_UPDATE_COMMAND_UI(ID_FILL_UP_MATCH_LINE_DATA, &CSmartDrawDoc::OnUpdateFillMatchlineData)
	ON_COMMAND(ID_GENERATE_REPORT , &CSmartDrawDoc::OnGenerateReport)
	ON_UPDATE_COMMAND_UI(ID_GENERATE_REPORT , &CSmartDrawDoc::OnUpdateGenerateReport)
	ON_COMMAND(ID_EDIT_PIPE_DIA_TABLE , &CSmartDrawDoc::OnEditPipeDiaTable)
	/// 2014.05.10 added by humkyung
	ON_COMMAND(IDS_RIBBON_MANUAL , &CSmartDrawDoc::OnRibbonManual)
END_MESSAGE_MAP()

static CSmartDrawDoc* __instance__ = NULL;
// CSmartDrawDoc construction/destruction

CSmartDrawDoc::CSmartDrawDoc()
{
	m_iGenerationType = CSmartDrawDoc::HLR | CSmartDrawDoc::ANNOTATION;
	m_sProjectUnit = _T("InchFeet");
	m_sProjectCode = _T("ANSI");	///#57: 2016.02.13 - added by humkyung
	__instance__ = this;
}

CSmartDrawDoc::~CSmartDrawDoc()
{
	try
	{
		for(map<STRING_T , vector<CSmartDrawColorButton::RGBQuad>* >::iterator itr = m_oColorMap.begin();itr != m_oColorMap.end();++itr)
		{
			SAFE_DELETE( itr->second );
		}
		m_oColorMap.clear();
	}
	catch(...)
	{
	}
}

CSmartDrawDoc* CSmartDrawDoc::GetInstance()
{
	return __instance__;
}

BOOL CSmartDrawDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}


// CSmartDrawDoc serialization

void CSmartDrawDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CSmartDrawDoc diagnostics

#ifdef _DEBUG
void CSmartDrawDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSmartDrawDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSmartDrawDoc commands

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   OnOpenDocument
    @return     BOOL
    @param      LPCTSTR lpszPathName
    @brief
******************************************************************************/
BOOL CSmartDrawDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	const CString rServerFolderPath = docData.GetServerFolderPath();
	
	///if (!CDocument::OnOpenDocument(lpszPathName))
	///	return FALSE;

	m_rProjectName = lpszPathName;
	const int at = m_rProjectName.ReverseFind('\\');
	if(-1 != at)
	{
		m_rProjectName = m_rProjectName.Right(m_rProjectName.GetLength() - at - 1);
	}
	m_sProjectSettingFilePath = rServerFolderPath + _T("\\") + m_rProjectName + _T("\\Setting\\") + m_rProjectName + _T(".prj");
	{
		m_oModelFolderPathMap.clear();

		TCHAR szBuf[MAX_PATH + 1]={'\0'};
		if(GetPrivateProfileString(_T("Folder") , _T("Pipe") , _T("") , szBuf , MAX_PATH , m_sProjectSettingFilePath))
		{
			m_oModelFolderPathMap.insert(make_pair(CaDraw_Entity::PIPE_MODEL_T , szBuf));
		}
		if(GetPrivateProfileString(_T("Folder") , _T("Equipment") , _T("") , szBuf , MAX_PATH , m_sProjectSettingFilePath))
		{
			m_oModelFolderPathMap.insert(make_pair(CaDraw_Entity::EQUIPMENT_MODEL_T , szBuf));
		}
		if(GetPrivateProfileString(_T("Folder") , _T("Structure") , _T("") , szBuf , MAX_PATH , m_sProjectSettingFilePath))
		{
			m_oModelFolderPathMap.insert(make_pair(CaDraw_Entity::STRUCTURE_MODEL_T , szBuf));
		}
		if(GetPrivateProfileString(_T("Folder") , _T("CableT") , _T("") , szBuf , MAX_PATH , m_sProjectSettingFilePath))
		{
			m_oModelFolderPathMap.insert(make_pair(CaDraw_Entity::CABLE_T_MODEL_T , szBuf));
		}
		if(GetPrivateProfileString(_T("Folder") , _T("Misc") , _T("") , szBuf , MAX_PATH , m_sProjectSettingFilePath))
		{
			m_oModelFolderPathMap.insert(make_pair(CaDraw_Entity::MISC_MODEL_T , szBuf));
		}
		if(GetPrivateProfileString(_T("Folder") , _T("Drawing") , _T("") , szBuf , MAX_PATH , m_sProjectSettingFilePath))
		{
			m_oModelFolderPathMap.insert(make_pair(CaDraw_Entity::DRAWING_MODEL_T , szBuf));
		}
	}
	m_sDatabaseFilePath = rServerFolderPath + _T("\\") + m_rProjectName + _T("\\Database\\") + m_rProjectName + _T(".mdb");
	m_sRefFilePath = rServerFolderPath + _T("\\") + m_rProjectName + _T("\\Database\\") + m_rProjectName + _T(".db3");
	/// copy ref file if not exists
	if(!CFileTools::DoesFileExist(m_sRefFilePath.c_str()))
	{
		const CString sExecPath = GetExecPath();
		::CopyFile(sExecPath + _T("\\Setting\\ref.db3") , m_sRefFilePath.c_str() , TRUE);
	}
	/// up to here
	///create anno filters table - 2016.03.18 added by humkyung
	soci::session oSession(*soci::factory_sqlite3() , m_sRefFilePath.c_str());
	{
		oSession << _T("CREATE TABLE if not exists AnnoFilters") << 
					_T("(") << 
					_T("Seq integer not null,") <<
					_T("Setting varchar(32) not null,") << 
					_T("Item varchar(32) not null,") << 
					_T("Prop varchar(16) not null,") << 
					_T("Oper varchar(16) not null,") <<
					_T("Value varchar(255)") <<
					_T(")");

		oSession << _T("CREATE TABLE if not exists PrjConfigs") << 
					_T("(") << 
					_T("Name varchar(32) not null,") << 
					_T("Value varchar(255)") <<
					_T(")");
	}
	///up to here
	
	const STRING_T sReportFilePath = rServerFolderPath + _T("\\") + m_rProjectName + _T("\\Setting\\") + m_rProjectName + _T(".xlsx");
	/// copy report file if not exists
	if(!CFileTools::DoesFileExist(sReportFilePath.c_str()))
	{
		const CString sExecPath = GetExecPath();
		::CopyFile(sExecPath + _T("\\Backup\\report.xlsx") , sReportFilePath.c_str() , TRUE);
	}
	/// up to here

	const CString sServerMDBFilePath = rServerFolderPath + CString(_T("\\")) + PROJECT_MDB_NAME;
	const CString sConnectionString = CString(PROVIDER) + _T("Data Source=") + sServerMDBFilePath + DB_PASSWORD;
	CADODB adoDB;
	if(adoDB.DBConnect(sConnectionString.operator LPCTSTR()))
	{
		CString sSql;
		sSql.Format(_T("SELECT DISTINCT C_PROJECT_DESC,C_PROJECT_UOR,C_MODEL_OFFSET_X,C_MODEL_OFFSET_Y,C_MODEL_OFFSET_Z,C_DRAWING_TYPE,C_OUTPUT_FORMAT,C_PASSWORD,C_CAD_APPLICATION,UNIT FROM %s WHERE C_PROJECT_NAME='%s'") , _T("T_SMARTDRAW_PROJECT") ,  m_rProjectName);
		adoDB.OpenQuery(sSql.operator LPCTSTR());
		
		long lRecordCount = 0L;
		adoDB.GetRecordCount(&lRecordCount);
		if(1 == lRecordCount)
		{
			adoDB.GetFieldValue(0 , _T("C_PROJECT_DESC") , &m_sProjectDesc);
			adoDB.GetFieldValue(0 , _T("C_PROJECT_UOR") , &m_sProjectUOR);
			if(m_sProjectUOR.empty()) m_sProjectUOR = _T("80");
			adoDB.GetFieldValue(0 , _T("C_MODEL_OFFSET_X") , &m_sModelOffsetX);
			if(m_sModelOffsetX.empty()) m_sModelOffsetX = _T("0");
			adoDB.GetFieldValue(0 , _T("C_MODEL_OFFSET_Y") , &m_sModelOffsetY);
			if(m_sModelOffsetY.empty()) m_sModelOffsetY = _T("0");
			adoDB.GetFieldValue(0 , _T("C_MODEL_OFFSET_Z") , &m_sModelOffsetZ);
			if(m_sModelOffsetZ.empty()) m_sModelOffsetZ = _T("0");
			adoDB.GetFieldValue(0 , _T("C_DRAWING_TYPE") , &m_sDrawingType);
			adoDB.GetFieldValue(0 , _T("C_OUTPUT_FORMAT") , &m_sOutputFormat);
			adoDB.GetFieldValue(0 , _T("C_PASSWORD") , &m_sPassword);				/// 2012.12.27 added by humkyung
			adoDB.GetFieldValue(0 , _T("C_CAD_APPLICATION") , &m_sCADApplication);	/// 2015.03.31 added by humkyung
			adoDB.GetFieldValue(0 , _T("UNIT") , &m_sProjectUnit);					/// 2016.01.27 added by humkyung
			adoDB.GetFieldValue(0 , _T("CODE") , &m_sProjectCode);					///#57: 2016.02.13 added by humkyung
			adoDB.GetFieldValue(0 , _T("C_CAD_APPLICATION") , &m_sCADApplication);
		}
		else
		{
			return FALSE;
		}
	}

	/// load revision source data - 2012.03.31 added by humkyung
	const STRING_T sRevisionSourceData = rServerFolderPath + _T("\\") + m_rProjectName + _T("\\Setting\\RevisionSourceData.XML");
	m_oRevisionSource.Load(sRevisionSourceData.c_str());
	/// up to here

	CProjectExplorer* pProjectExplorer = CProjectExplorer::GetInstance();
	pProjectExplorer->ActivateTreeItem(this);

	CPropertiesWnd* pPropertiesWnd = CPropertiesWnd::GetInstance();
	if(NULL != pPropertiesWnd)
	{
		pPropertiesWnd->Reset();
		pPropertiesWnd->ShowPane(TRUE , FALSE, FALSE);
	}

	COutputWnd* pOutputWnd = COutputWnd::GetInstance();
	if(NULL != pOutputWnd)
	{
		pOutputWnd->StartFolderWatcher(rServerFolderPath + _T("\\") + m_rProjectName + _T("\\Output") , this);
		pOutputWnd->FillDrawingWindow();
	}

	/// update current project - 2012.12.28 added by humkyung
	::WritePrivateProfileString(_T("SmartDraw for PDS") , _T("CurPrjName") , m_rProjectName , docData.GetAppSettingFilePath());
	/// up to here

	return TRUE;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.10
	@param	
	@return
*/
CString CSmartDrawDoc::GetProjectName(void) const
{
	return m_rProjectName;
}

/**	
	@brief	
	@author	humkyung
	@date	2015.04.02
	@param	
	@return
*/
CString CSmartDrawDoc::GetCADAppPath() const
{
	return m_sCADApplication.c_str();
}

/**	
	@brief	set cad application path
	@author	humkyung
	@date	2016.03.10
	@param	
	@return	void
*/
void CSmartDrawDoc::SetCADAppPath(const STRING_T& sFilePath)
{
	m_sCADApplication = sFilePath;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-16
    @function   GetDrawingType
    @return     STRING_T
    @brief
******************************************************************************/
STRING_T CSmartDrawDoc::GetDrawingType() const
{
	return m_sDrawingType;
}

/******************************************************************************
    @brief		return output format string
	@author     humkyung
    @date       2014-06-21
    @function   GetOutputFormat
    @return     STRING_T
******************************************************************************/
STRING_T CSmartDrawDoc::GetOutputFormatString() const
{
	return m_sOutputFormat;
}

/******************************************************************************
    @brief		return true if output format is MSTN
	@author     humkyung
    @date       2014-06-25
    @function   IsMSTNOutput
    @return     bool
******************************************************************************/
bool CSmartDrawDoc::IsMSTNOutput() const
{
	return (0 == IsString::CompareNoCase(_T("MSTN") , m_sOutputFormat));
}

/******************************************************************************
    @brief		return true if output format is AutoCAD
	@author     humkyung
    @date       2014-06-25
    @function   IsAutoCADOutput
    @return     bool
******************************************************************************/
bool CSmartDrawDoc::IsAutoCADOutput() const
{
	return (0 == IsString::CompareNoCase(_T("AutoCAD") , m_sOutputFormat));
}

/******************************************************************************
    @brief		return default file extension
	@author     humkyung
    @date       2014-06-26
    @function   GetDefaultExt
    @return     CString
******************************************************************************/
CString CSmartDrawDoc::GetDefaultExt() const
{
	return IsMSTNOutput() ? _T(".dgn") : (IsAutoCADOutput() ? _T(".dwg") : _T(""));
}

/******************************************************************************
    @brief		return line style file path
	@author     humkyung
    @date       2014-06-23
    @function   GetLineStyleFilePath
    @return     STRING_T
******************************************************************************/
CString CSmartDrawDoc::GetLineStyleFilePath() const
{
	const CString sExecPath = GetExecPath();
	return (_T("MSTN") == m_sOutputFormat) ? (sExecPath + _T("\\Setting\\MSTN.LST")) : (sExecPath + _T("\\Setting\\AUTOCAD.LST"));
}

/******************************************************************************
    @brief		return line weight file path
	@author     humkyung
    @date       2014-07-02
    @function   GetLineWeightFilePath
    @return     STRING_T
******************************************************************************/
CString CSmartDrawDoc::GetLineWeightFilePath() const
{
	const CString sExecPath = GetExecPath();
	return IsMSTNOutput() ? (sExecPath + _T("\\Setting\\MSTN.LWT")) : (sExecPath + _T("\\Setting\\AUTOCAD.LWT"));
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-15
    @function   GetColorMapOf
    @return     vector<CSmartDrawColorButton::RGBQuad>*
    @param      const           STRING_T&
    @param      sIniFilePath
    @brief
******************************************************************************/
vector<CSmartDrawColorButton::RGBQuad>* CSmartDrawDoc::GetColorListOf(const STRING_T& sIniFilePath , const bool& bReload)
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	CString sOutputFormat = this->GetOutputFormatString().c_str();
	
	CString rPaletteFilePath;
	if(_T("AUTOCAD") == sOutputFormat.MakeUpper())
	{
		rPaletteFilePath = GetExecPath() + _T("\\Setting\\AUTOCAD.PAL");
	}
	else if(_T("MSTN") == sOutputFormat.MakeUpper())
	{
		rPaletteFilePath = sIniFilePath.c_str();
		int nFound = rPaletteFilePath.ReverseFind('\\');
		if(-1 != nFound)
		{
			rPaletteFilePath = rPaletteFilePath.Left(nFound);
			nFound = rPaletteFilePath.ReverseFind('\\');
			if(-1 != nFound)
			{
				rPaletteFilePath = rPaletteFilePath.Left(nFound) + _T("\\Border");
			}
		}
		if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
		{
			CString rString(szBuf);
			nFound = rString.ReverseFind('.');
			if(-1 != nFound)
			{
				rString = rString.Left(nFound) + _T(".PAL");
				rPaletteFilePath += _T("\\") + rString;
			}
		}

		if(FALSE == FileExist(rPaletteFilePath))
		{
			rPaletteFilePath = GetExecPath() + _T("\\Setting\\MSTN.PAL");
		}
	}

	/// remove color list when reload value is true - 2014.06.05
	if(true == bReload)
	{
		map<STRING_T , vector<CSmartDrawColorButton::RGBQuad>* >::iterator where = m_oColorMap.find(rPaletteFilePath.operator LPCTSTR());
		if(where != m_oColorMap.end())
		{
			SAFE_DELETE(where->second);
			m_oColorMap.erase(where);
		}
	}
	/// up to here

	map<STRING_T , vector<CSmartDrawColorButton::RGBQuad>* >::iterator where = m_oColorMap.find(rPaletteFilePath.operator LPCTSTR());
	if(where == m_oColorMap.end())
	{
		IFSTREAM_T ifile(rPaletteFilePath.operator LPCTSTR());
		if(ifile.is_open())
		{
			vector<CSmartDrawColorButton::RGBQuad>* pColorList = new vector<CSmartDrawColorButton::RGBQuad>();

			vector<STRING_T> oResult;
			STRING_T aLine;
			while(!ifile.eof())
			{
				getline(ifile , aLine);
				if(ifile.eof()) break;

				CTokenizer<CIsComma>::Tokenize(oResult , aLine , CIsComma());
				if(3 == oResult.size())
				{
					CSmartDrawColorButton::RGBQuad rgbquad;

					rgbquad.r = ATOI_T(oResult[0].c_str());
					rgbquad.g = ATOI_T(oResult[1].c_str());
					rgbquad.b = ATOI_T(oResult[2].c_str());
					pColorList->push_back(rgbquad);
				}
			}

			m_oColorMap.insert(make_pair(rPaletteFilePath.operator LPCTSTR() , pColorList));
			return pColorList;
		}

		return NULL;
	}

	return where->second;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   OnProjectSetting
    @return     void
    @brief
******************************************************************************/
void CSmartDrawDoc::OnProjectSetting()
{
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	CProjectSettingSheet dlg(0 , this->GetProjectName() , m_sProjectSettingFilePath , AfxGetMainWnd());
	if(IDOK == dlg.DoModal())
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   OnGenerateDra
    @return     void
    @brief
******************************************************************************/
void CSmartDrawDoc::OnGenerateDra()
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	if(GetPrivateProfileString(_T("Folder") , _T("Drawing") , _T("") , szBuf , MAX_PATH , m_sProjectSettingFilePath))
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		const CString rServerFolderPath = docData.GetServerFolderPath();

		CGenerateDraFromDrawingDlg dlg(rServerFolderPath + _T("\\") + m_rProjectName + _T("\\Dra") , szBuf , m_sProjectUOR.c_str() , m_sDatabaseFilePath.c_str());
		if(IDOK == dlg.DoModal())
		{
			this->UpdateAllViews(NULL);
		}
	}
	else
	{
		AfxMessageBox(_T("Draw folder is unset") , MB_OK | MB_ICONEXCLAMATION);
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-28
    @function   OnEditRevisionData
    @return     void
    @brief
******************************************************************************/
void CSmartDrawDoc::OnEditRevisionData()
{
	CRevisionManagerDlg dlg;
	if(IDOK == dlg.DoModal())
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   GetSelectedDrawings
    @return     int
    @param      list<CaDraw_DraFile*>&  oSelectedDrawingList
    @brief
******************************************************************************/
int CSmartDrawDoc::GetSelectedDrawings(list<CaDraw_DraFile*>& oSelectedDrawingList)
{
	POSITION pos = GetFirstViewPosition();
	while(pos)
	{
		CView* pView = GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSmartDrawView)))
		{
			CSmartDrawView* pSmartDrawView = static_cast<CSmartDrawView*>(pView);
			pSmartDrawView->GetSelectedDrawings(oSelectedDrawingList);
		}
	}

	return ERROR_SUCCESS;
}

void CSmartDrawDoc::OnUpdateGenerateHLR(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(CSmartDrawDoc::HLR == (m_iGenerationType & CSmartDrawDoc::HLR));
}

/**	
	@brief	set generation type to HLR
	@author	humkyung
	@date	2014.08.25
	@param	pCmdUI
	@return
*/
void CSmartDrawDoc::OnGenerateHLR()
{
	m_iGenerationType ^= CSmartDrawDoc::HLR;
}

void CSmartDrawDoc::OnUpdateGenerateAnnotation(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(CSmartDrawDoc::ANNOTATION == (m_iGenerationType & CSmartDrawDoc::ANNOTATION));
}

/**	
	@brief	set generation type to Annotation
	@author	humkyung
	@date	2014.08.25
	@param	pCmdUI
	@return
*/
void CSmartDrawDoc::OnGenerateAnnotation()
{
	m_iGenerationType ^= CSmartDrawDoc::ANNOTATION;
}

/**	
	@brief	disable generate button when there is no selected drawings
	@author	humkyung
	@date	2014.05.30
	@param	pCmdUI
	@return
*/
void CSmartDrawDoc::OnUpdateGenerate(CCmdUI *pCmdUI)
{
	POSITION pos = GetFirstViewPosition();
	while(pos)
	{
		CView* pView = GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSmartDrawView)))
		{
			CSmartDrawView* pDrawView = static_cast<CSmartDrawView*>(pView);
			list<CaDraw_DraFile*> oSelectedDrawingList;
			pDrawView->GetSelectedDrawings(oSelectedDrawingList);
			pCmdUI->Enable((m_iGenerationType > 0) && (oSelectedDrawingList.size() > 0));
		}
	}
}

/**	
	@brief	generate selected drawings
	@author	humkyung
	@date	2010.05.14
	@param	
	@return
*/
void CSmartDrawDoc::OnGenerate()
{
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();

	bool bIsValid = true;
	/*
#ifdef _DEBUG
	bIsValid = true;
#else
	Packet packet;
	InitializePacket(&packet);
	strcpy((char*)(packet.ID) , docData.GetUser().operator LPCSTR());
	strcpy((char*)(packet.Password) , docData.GetPassword().operator LPCSTR());
	strcpy((char*)(packet.ActivateCode) , docData.GetToken().c_str());

	LMS_ENCODE lms;
	lms.size = sizeof(Packet);
	memcpy(lms.buf,&packet,sizeof(Packet));
	Encode(&lms);

	_ns1__IsValidToken request;
	{
		request.oLMS = new ns1__LMSStuctType();
		request.oLMS->buf = new xsd__base64Binary();
		request.oLMS->buf->__ptr = new unsigned char[LMS_BUF_SIZE];
		memcpy(request.oLMS->buf->__ptr,lms.buf,lms.size);
		request.oLMS->buf->__size = LMS_BUF_SIZE;
		request.oLMS->size = lms.size;
	}
	_ns1__IsValidTokenResponse response;

	LicWebServiceSoapProxy soapProxy;
	if(SOAP_OK == soapProxy.IsValidToken(LICENDPOINT,LICENDPOINT"/IsValidToken",&request,response))
	{
		bIsValid = response.IsValidTokenResult;
	}
	else
	{
		AfxMessageBox(soapProxy.soap_fault_detail());
	}
	delete request.oLMS->buf->__ptr;
	delete request.oLMS->buf;
	delete request.oLMS;
#endif
	*/

	if(bIsValid)
	{
		POSITION pos = GetFirstViewPosition();
		while(pos)
		{
			CView* pView = GetNextView(pos);
			if(pView->IsKindOf(RUNTIME_CLASS(CSmartDrawView)))
			{
				CSmartDrawView* pDrawView = static_cast<CSmartDrawView*>(pView);
				pDrawView->OnGenerate();
			}
		}
	}
	else
	{
		AfxMessageBox(_TR("Token is invalid"));
	}
}

/**	
	@brief	disable insert rev. no button when there is no selected drawings
	@author	humkyung
	@date	2014.11.16
	@param	pCmdUI
	@return	none
*/
void CSmartDrawDoc::OnUpdateInsertRevNo(CCmdUI *pCmdUI)
{
	POSITION pos = GetFirstViewPosition();
	while(pos)
	{
		CView* pView = GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSmartDrawView)))
		{
			CSmartDrawView* pDrawView = static_cast<CSmartDrawView*>(pView);
			list<CaDraw_DraFile*> oSelectedDrawingList;
			pDrawView->GetSelectedDrawings(oSelectedDrawingList);
			pCmdUI->Enable((m_iGenerationType > 0) && (oSelectedDrawingList.size() > 0));
		}
	}
}

/**	
	@brief	insert rev. no to selected drawings
	@author	humkyung
	@date	2014.11.16
	@param	
	@return	none
*/
void CSmartDrawDoc::OnInsertRevNo()
{
	CInsertRevNoDlg dlg;
	dlg.DoModal();
}

/**	
	@brief	disable the button when there is no drawings
	@author	humkyung
	@date	2016.03.25
	@param	pCmdUI
	@return	none
*/
void CSmartDrawDoc::OnUpdateUpdateTitleBlock(CCmdUI *pCmdUI)
{
	POSITION pos = GetFirstViewPosition();
	while(pos)
	{
		CView* pView = GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSmartDrawView)))
		{
			CSmartDrawView* pDrawView = static_cast<CSmartDrawView*>(pView);
			list<CaDraw_DraFile*> oAllDrawingList;
			pDrawView->GetAllDrawings(oAllDrawingList);
			pCmdUI->Enable(oAllDrawingList.size() > 0);
		}
	}
}

/**	
	@brief	update title block data for all drawings
	@author	humkyung
	@date	2016.03.25
	@param	
	@return	none
*/
void CSmartDrawDoc::OnUpdateTitleBlock()
{
	this->BeginWaitCursor();
	
	try
	{
		POSITION pos = GetFirstViewPosition();
		while(pos)
		{
			CView* pView = GetNextView(pos);
			if(pView->IsKindOf(RUNTIME_CLASS(CSmartDrawView)))
			{
				CSmartDrawView* pDrawView = static_cast<CSmartDrawView*>(pView);
				list<CaDraw_DraFile*> oAllDrawingList;
				pDrawView->GetAllDrawings(oAllDrawingList);

				const STRING_T sConnectionString = STRING_T(PROVIDER) + _T("Data Source=") + m_sDatabaseFilePath + DB_PASSWORD;
				CADODB adoDB;
				if(TRUE == adoDB.DBConnect(sConnectionString.c_str()))
				{
					const STRING_T sApp(_T("Title Block Data"));
					for(list<CaDraw_DraFile*>::const_iterator itr = oAllDrawingList.begin();itr != oAllDrawingList.end();++itr)
					{
						const STRING_T sFilePath = (*itr)->GetFilePath();
						const STRING_T sDraIniFilePath = sFilePath + _T(".ini");
						CSplitPath path(sFilePath.c_str());
						CString sDwgFileName = (path.GetFileName() + _T(".dgn")).MakeLower();
						
						CString sql;
						sql.Format(_T("SELECT drawing_title,default_scale FROM PDTABLE_121 where lcase(drawing_file_spec)='%s'") , sDwgFileName);
						if(TRUE == adoDB.OpenQuery(sql.operator LPCTSTR()))
						{
							STRING_T sValue;
							adoDB.GetFieldValue(0 , _T("drawing_title") , &sValue);
							::WritePrivateProfileString(sApp.c_str(),_T("DWG.NO 0"),sValue.c_str(),sDraIniFilePath.c_str());
							::WritePrivateProfileString(sApp.c_str(),_T("DWG.NO 90"),sValue.c_str(),sDraIniFilePath.c_str());
							adoDB.GetFieldValue(0 , _T("default_scale") , &sValue);
							::WritePrivateProfileString(sApp.c_str(),_T("SCALE"),sValue.c_str(),sDraIniFilePath.c_str());
						}

						///#22(http://atools.co.kr:9002/redmine/issues/22) - 2016.03.28 added by humkyung
						const UINT uiCount = GetPrivateProfileInt(_T("Revision Data") , _T("Count") , 0 , sDraIniFilePath.c_str()); 
						if(uiCount > 0)
						{
							TCHAR szBuf[MAX_PATH + 1]={'\0',};
							vector<STRING_T> oResult;

							CString rString;
							rString.Format(_T("Data%d") , uiCount-1);
							GetPrivateProfileString(_T("Revision Data") , rString , _T("") , szBuf , MAX_PATH , sDraIniFilePath.c_str());
							CTokenizer<CIsComma>::Tokenize(oResult,szBuf,CIsComma());
							if(!oResult.empty())
							{
								WritePrivateProfileString(sApp.c_str() , _T("REV") , oResult[0].c_str() , sDraIniFilePath.c_str());
							}
						}
						///up to here
					}
					this->UpdateAllViews(NULL);
					break;
				}
			}
		}
	}
	catch(...){}

	this->EndWaitCursor();
}

/**	
	@brief	disable the button when there is no drawings
	@author	humkyung
	@date	2014.11.19
	@param	pCmdUI
	@return	none
*/
void CSmartDrawDoc::OnUpdateFillMatchlineData(CCmdUI *pCmdUI)
{
	POSITION pos = GetFirstViewPosition();
	while(pos)
	{
		CView* pView = GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSmartDrawView)))
		{
			CSmartDrawView* pDrawView = static_cast<CSmartDrawView*>(pView);
			list<CaDraw_DraFile*> oAllDrawingList;
			pDrawView->GetAllDrawings(oAllDrawingList);
			pCmdUI->Enable(oAllDrawingList.size() > 0);
		}
	}
}

/**	
	@brief	fill matchline data to all drawings
	@author	humkyung
	@date	2014.11.19
	@param	
	@return	none
*/
void CSmartDrawDoc::OnFillMatchlineData()
{
	this->BeginWaitCursor();
	
	try
	{
		CFillMatchlineDataDlg dlg;
		if(IDOK == dlg.DoModal())
		{
			const STRING_T sMatchLineDwgPrefix = dlg.m_sMatchLineDwgPrefix.operator LPCTSTR();
			const STRING_T sNoMatchLineDwg = dlg.m_sNoMatchLineDwg.operator LPCTSTR();

			POSITION pos = GetFirstViewPosition();
			while(pos)
			{
				CView* pView = GetNextView(pos);
				if(pView->IsKindOf(RUNTIME_CLASS(CSmartDrawView)))
				{
					CSmartDrawView* pDrawView = static_cast<CSmartDrawView*>(pView);
					list<CaDraw_DraFile*> oAllDrawingList;
					pDrawView->GetAllDrawings(oAllDrawingList);
					for(list<CaDraw_DraFile*>::iterator itr = oAllDrawingList.begin();itr != oAllDrawingList.end();++itr)
					{
						(*itr)->ResetMatchLineData(sNoMatchLineDwg);
						for(list<CaDraw_DraFile*>::const_iterator jtr = oAllDrawingList.begin();jtr != oAllDrawingList.end();++jtr)
						{
							if(itr == jtr) continue;
							(*itr)->FillMatchLineData(*jtr,sMatchLineDwgPrefix,sNoMatchLineDwg);
						}
						(*itr)->Save();
					}
					this->UpdateAllViews(NULL);
					break;
				}
			}
		}
	}
	catch(...){}

	this->EndWaitCursor();
}

/**	
	@brief	disable generate button when there is no selected drawings
	@author	humkyung
	@date	2014.11.20
	@param	pCmdUI
	@return	none
*/
void CSmartDrawDoc::OnUpdateGenerateReport(CCmdUI *pCmdUI)
{
	POSITION pos = GetFirstViewPosition();
	while(pos)
	{
		CView* pView = GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSmartDrawView)))
		{
			CSmartDrawView* pDrawView = static_cast<CSmartDrawView*>(pView);
			list<CaDraw_DraFile*> oSelectedDrawingList;
			pDrawView->GetSelectedDrawings(oSelectedDrawingList);
			pCmdUI->Enable(oSelectedDrawingList.size() > 0);
		}
	}
}

/**	
	@brief	generate line list for selected drawings
	@author	humkyung
	@date	2011.11.20
	@param	
	@return	none
*/
void CSmartDrawDoc::OnGenerateReport()
{
	POSITION pos = GetFirstViewPosition();
	while(pos)
	{
		CView* pView = GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSmartDrawView)))
		{
			CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
			const CString sPrjFolderPath = docData.GetServerFolderPath() + _T("\\") + m_rProjectName;
			
			BeginWaitCursor();

			list<pair<STRING_T,STRING_T>> aFilePathList;

			CPluginManager& oPluginManager = CPluginManager::GetInstance();
			const STRING_T& sDrawingType = this->GetDrawingType();
			Plugin* pPlugin = oPluginManager.GetPluginOf(sDrawingType);
			if((NULL != pPlugin) && (NULL != pPlugin->pfInitReport))
			{
				pPlugin->pfInitReport(m_sRefFilePath.c_str());
			
				CSmartDrawView* pDrawView = static_cast<CSmartDrawView*>(pView);
				list<CaDraw_DraFile*> oSelectedDrawingList;			
				pDrawView->GetSelectedDrawings(oSelectedDrawingList);
				for(list<CaDraw_DraFile*>::const_iterator itr = oSelectedDrawingList.begin();itr != oSelectedDrawingList.end();++itr)
				{
					auto_ptr<CaDraw_DraFile> pDraFile(new CaDraw_DraFile((*itr)->GetFilePath()));
					if(ERROR_SUCCESS == pDraFile->Parse())
					{
						const CIsPoint3d ptModelOffset(ATOF_T(m_sModelOffsetX.c_str()) , ATOF_T(m_sModelOffsetX.c_str()) , ATOF_T(m_sModelOffsetX.c_str()));
						const CString sIniFilePath = (sPrjFolderPath + CString(_T("\\Setting\\")) + (*itr)->GetSettingFileName() + CString(_T(".ini")));
						
						pPlugin->pfReport(pDraFile.get(),ptModelOffset,sPrjFolderPath,m_rProjectName,(*itr)->GetSettingFileName());
					}
				}
			}

			{
				const CString sExecFilePath = GetExecPath() + _T("\\DataExchange.exe");
				const CString sTemplateExlPath = docData.GetServerFolderPath() + _T("\\") + m_rProjectName + _T("\\Setting\\") + m_rProjectName + _T(".xlsx");
				const CString sArgs = (_T("\"") + sExecFilePath + _T("\" \"Report\" \"") + CString(this->GetRefFilePath().c_str()) + _T("\" \"") + sTemplateExlPath + _T("\""));

				DWORD exitCode = 0;
				executeCommandLine(sArgs , exitCode);
			}

			EndWaitCursor();
		}
	}
}

/**	
	@brief	show pipe dia. edit table dialog
	@author	humkyung
	@date	2015.07.15
	@param	
	@return	none
*/
void CSmartDrawDoc::OnEditPipeDiaTable()
{
	CPipeDiaEditorDlg dlg;
	dlg.DoModal();
}

/**	
	@brief	open manual
	@author	HumKyung
	@date	2014.05.10
	@param	
	@return
*/
void CSmartDrawDoc::OnRibbonManual()
{
	CString sExecPath;
	CFileTools::GetExecutableDirectory(sExecPath);
	const CString sManualFilePath = sExecPath + _T("Backup\\SmartDraw.pdf");
	::ShellExecute(NULL , _T("open") , sManualFilePath , NULL , NULL , SW_SHOW);
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-15
    @class      CSmartDrawDoc
    @function   GetModelFolderPathWith
    @return     STRING_T
    @param      const       CaDraw_Entity::MODEL_TYPE_T&
    @param      modelType
    @brief
******************************************************************************/
STRING_T CSmartDrawDoc::GetModelFolderPathWith(const CaDraw_Entity::MODEL_TYPE_T& modelType)
{
	map<CaDraw_Entity::MODEL_TYPE_T , STRING_T>::iterator where = m_oModelFolderPathMap.find(modelType);
	if(where != m_oModelFolderPathMap.end())
	{
		return where->second;
	}

	return STRING_T();
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-10
    @class      CSmartDrawDoc
    @function   GetPrjSettingFilePath
    @return     CString
    @brief
******************************************************************************/
CString CSmartDrawDoc::GetPrjSettingFilePath() const
{
	return m_sProjectSettingFilePath;
}

/******************************************************************************
    @author     humkyung
    @date       2012-09-19
    @class      CSmartDrawDoc
    @function   GetAccessFilePath
    @return     STRING_T
    @brief		return access file path
******************************************************************************/
STRING_T CSmartDrawDoc::GetAccessFilePath() const
{
	return m_sDatabaseFilePath;
}

/******************************************************************************
    @brief		return reference file path
	@author     humkyung
    @date       2016-02-15
    @return     STRING_T
******************************************************************************/
STRING_T CSmartDrawDoc::GetRefFilePath() const
{
	return m_sRefFilePath;
}

/******************************************************************************
    @brief		return model offset x
	@author     humkyung
    @date       2014-11-09
    @class      CSmartDrawDoc
    @function   GetModelOffsetX
    @return     STRING_T
******************************************************************************/
STRING_T CSmartDrawDoc::GetModelOffsetX() const
{
	return m_sModelOffsetX;
}

/******************************************************************************
    @brief		return model offset y
	@author     humkyung
    @date       2014-11-09
    @class      CSmartDrawDoc
    @function   GetModelOffsetX
    @return     STRING_T
******************************************************************************/
STRING_T CSmartDrawDoc::GetModelOffsetY() const
{
	return m_sModelOffsetY;
}

/******************************************************************************
    @brief		return model offset z
	@author     humkyung
    @date       2014-11-09
    @class      CSmartDrawDoc
    @function   GetModelOffsetX
    @return     STRING_T
******************************************************************************/
STRING_T CSmartDrawDoc::GetModelOffsetZ() const
{
	return m_sModelOffsetZ;
}

/******************************************************************************
    @brief		set model offset x
	@author     humkyung
    @date       2014-11-09
    @class      CSmartDrawDoc
    @function   SetModelOffsetX
    @return     int
******************************************************************************/
int CSmartDrawDoc::SetModelOffsetX(const STRING_T& sModelOffsetX)
{
	m_sModelOffsetX = sModelOffsetX;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		set model offset y
	@author     humkyung
    @date       2014-11-09
    @class      CSmartDrawDoc
    @function   SetModelOffsetY
    @return     int
******************************************************************************/
int CSmartDrawDoc::SetModelOffsetY(const STRING_T& sModelOffsetY)
{
	m_sModelOffsetY = sModelOffsetY;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		set model offset z
	@author     humkyung
    @date       2014-11-09
    @class      CSmartDrawDoc
    @function   SetModelOffsetZ
    @return     int
******************************************************************************/
int CSmartDrawDoc::SetModelOffsetZ(const STRING_T& sModelOffsetZ)
{
	m_sModelOffsetZ = sModelOffsetZ;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		return password
	@author     humkyung
    @date       2012-12-27
    @class      CSmartDrawDoc
    @function   GetAccessFilePath
    @return     STRING_T
******************************************************************************/
STRING_T CSmartDrawDoc::GetPassword() const
{
	return m_sPassword;
}

/******************************************************************************
    @brief		return project desc
	@author     humkyung
    @date       2012-12-27
    @class      CSmartDrawDoc
    @function   GetAccessFilePath
    @return     STRING_T
******************************************************************************/
STRING_T CSmartDrawDoc::GetProjectDesc() const
{
	return m_sProjectDesc;
}

/**
	@brief	return the generation type
	@author	humkyung
	@date	2014.08.25
	@return	int
*/
int CSmartDrawDoc::GetGenerationType() const
{
	return m_iGenerationType;
}

/**
	@brief	return true if given item is filter item
	@author	humkyung
	@date	2016.03.17
	@return	bool
*/
bool CSmartDrawDoc::IsFilterItem(const STRING_T& sItem) const
{
	if(0 == STRICMP_T(_T("Pipe") , sItem.c_str())) return true;
	if(0 == STRICMP_T(_T("Equipment") , sItem.c_str())) return true;
	if(0 == STRICMP_T(_T("Nozzle") , sItem.c_str())) return true;
	if(0 == STRICMP_T(_T("Structure") , sItem.c_str())) return true;
	if(0 == STRICMP_T(_T("Instrument") , sItem.c_str())) return true;
	if(0 == STRICMP_T(_T("Valve") , sItem.c_str())) return true;
	if(0 == STRICMP_T(_T("Specialty") , sItem.c_str())) return true;
	if(0 == STRICMP_T(_T("Support") , sItem.c_str())) return true;
	if(0 == STRICMP_T(_T("Reducer") , sItem.c_str())) return true;

	return false;
}

/**
	@brief	return properties of given item
	@author	humkyung
	@date	2016.03.17
	@return	int	
*/
int CSmartDrawDoc::GetPropsOf(list<STRING_T>& oPropList,const STRING_T& sItem) const
{
	oPropList.clear();
	if(0 == STRICMP_T(_T("Pipe") , sItem.c_str()))
	{
		oPropList.push_back(_T("Name"));
		oPropList.push_back(_T("Size"));
		oPropList.push_back(_T("Spec"));
		oPropList.push_back(_T("None"));
	}
	else if(0 == STRICMP_T(_T("Equipment") , sItem.c_str()))
	{
		oPropList.push_back(_T("Name"));
		oPropList.push_back(_T("HasNozzle"));
		oPropList.push_back(_T("None"));
	}
	else if(0 == STRICMP_T(_T("Nozzle") , sItem.c_str()))
	{
		oPropList.push_back(_T("Name"));
		oPropList.push_back(_T("None"));
	}
	else if(0 == STRICMP_T(_T("Structure") , sItem.c_str()))
	{
		oPropList.push_back(_T("Name"));
		oPropList.push_back(_T("None"));
	}
	else if(0 == STRICMP_T(_T("Instrument") , sItem.c_str()))
	{
		oPropList.push_back(_T("Name"));
		oPropList.push_back(_T("None"));
	}
	else if(0 == STRICMP_T(_T("Valve") , sItem.c_str()))
	{
		oPropList.push_back(_T("Name"));
		oPropList.push_back(_T("None"));
	}
	else if(0 == STRICMP_T(_T("Specialty") , sItem.c_str()))
	{
		oPropList.push_back(_T("Name"));
		oPropList.push_back(_T("None"));
	}
	else if(0 == STRICMP_T(_T("Support") , sItem.c_str()))
	{
		oPropList.push_back(_T("Name"));
		oPropList.push_back(_T("None"));
	}
	else if(0 == STRICMP_T(_T("Reducer") , sItem.c_str()))
	{
		oPropList.push_back(_T("None"));
	}

	return ERROR_SUCCESS;
}

/**
	@brief	return operators of given property 
	@author	humkyung
	@date	2016.03.17
	@return	int
*/
int CSmartDrawDoc::GetOperatorsOf(list<STRING_T>& oOperList,const STRING_T& sProperty) const
{
	oOperList.clear();
	if(0 == STRICMP_T(_T("Name") , sProperty.c_str()))
	{
		oOperList.push_back(_T("Equals"));
		oOperList.push_back(_T("Contains"));
		oOperList.push_back(_T("Begins with"));
		oOperList.push_back(_T("Ends with"));
		oOperList.push_back(_T("Not Equals"));
		oOperList.push_back(_T("Not Contains"));
	}
	else if(0 == STRICMP_T(_T("Spec") , sProperty.c_str()))
	{
		oOperList.push_back(_T("Equals"));
		oOperList.push_back(_T("Contains"));
		oOperList.push_back(_T("Begins with"));
		oOperList.push_back(_T("Ends with"));
		oOperList.push_back(_T("Not Equals"));
		oOperList.push_back(_T("Not Contains"));
	}
	else if(0 == STRICMP_T(_T("Size") , sProperty.c_str()))
	{
		oOperList.push_back(_T("Equals"));
		oOperList.push_back(_T("Over"));
		oOperList.push_back(_T("Less"));
	}
	else if(0 == STRICMP_T(_T("HasNozzle") , sProperty.c_str()))
	{
		oOperList.push_back(_T("Equals"));
	}
	else if(0 == STRICMP_T(_T("None") , sProperty.c_str()))
	{
		oOperList.push_back(_T("Const"));
	}

	return ERROR_SUCCESS;
}

/**
	@brief	return values of given property 
	@author	humkyung
	@date	2016.03.19
	@return	int
*/
int CSmartDrawDoc::GetValuesOf(list<STRING_T>& oValueList,const STRING_T& sProperty) const
{
	oValueList.clear();
	if(0 == STRICMP_T(_T("Size") , sProperty.c_str()))
	{
		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		list<CSmartDrawDocData::SizeRecord>* pSizeRecordList = docData.GetSizeRecordList();
		for(list<CSmartDrawDocData::SizeRecord>::const_iterator itr = pSizeRecordList->begin();itr != pSizeRecordList->end();++itr)
		{
			oValueList.push_back(itr->inch + _T("-") + itr->metric + _T("mm"));
		}
	}
	else if(0 == STRICMP_T(_T("HasNozzle"),sProperty.c_str()))
	{
		oValueList.push_back(_T("True"));
		oValueList.push_back(_T("False"));
	}
	else if(0 == STRICMP_T(_T("None"),sProperty.c_str()))
	{
		oValueList.push_back(_T("True"));
		oValueList.push_back(_T("False"));
	}

	return ERROR_SUCCESS;
}

/**
	@brief	open drawing
	@author	humkyung
	@date	2014.06.02
	@return	int
*/
int CSmartDrawDoc::OpenDrawing(const CString& sDrawingFilePath)
{
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	if(this->IsMSTNOutput())
	{
		const CString sUSTNFilePath(this->GetCADAppPath());
		if(!sUSTNFilePath.IsEmpty())
		{
			::ShellExecute(NULL , _T("open") , sUSTNFilePath , sDrawingFilePath , NULL , SW_SHOW);
		}
		else
		{
			AfxMessageBox(_TR("Can't find USTN file") , MB_OK|MB_ICONWARNING);
		}
	}
	else
	{
		::ShellExecute(NULL , _T("open") , sDrawingFilePath , NULL , NULL , SW_SHOW);
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		return project unit
	@author     humkyung
    @date       2016-01-27
    @return     STRING_T
******************************************************************************/
STRING_T CSmartDrawDoc::GetProjectUnit() const
{
	return m_sProjectUnit;
}

/******************************************************************************
    @brief		set project unit
	@author     humkyung
    @date       2016-01-27
    @return     int
******************************************************************************/
int CSmartDrawDoc::SetProjectUnit(const STRING_T& sProjectUnit)
{
	m_sProjectUnit = sProjectUnit;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		return project code
	@author     humkyung
    @date       2016-02-12
    @return     STRING_T
******************************************************************************/
STRING_T CSmartDrawDoc::GetProjectCode() const
{
	return m_sProjectCode;
}

/******************************************************************************
    @brief		set project code
	@author     humkyung
    @date       2016-02-12
    @return     int
******************************************************************************/
int CSmartDrawDoc::SetProjectCode(const STRING_T& sProjectCode)
{
	m_sProjectCode = sProjectCode;
	LoadPipeDiaTable();
	
	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		load pipe O.D table
	@author     humkyung
    @date       2016-02-16
    @return     int
******************************************************************************/
int CSmartDrawDoc::LoadPipeDiaTable() const
{
	/// load pipe O.D table
	soci::session oSession(*soci::factory_sqlite3() , this->GetRefFilePath().c_str());

	CaDraw_PipeDiaTable& aPipeDiaTable = CaDraw_PipeDiaTable::GetInstance();
	aPipeDiaTable.Reset();
	{
		OSTRINGSTREAM_T oss;
		oss << _T("select FromSize,ToSize,OD from ") << m_sProjectCode.c_str();
		soci::rowset<soci::row> rs(oSession.prepare << oss.str());
		for(soci::rowset<soci::row>::const_iterator itr = rs.begin();itr != rs.end();++itr)
		{
			const double dFromSize = itr->get<double>(0);
			const double dToSize = itr->get<double>(1);
			const double dOD = itr->get<double>(2);
			aPipeDiaTable.Add(dFromSize , dToSize , dOD);
		}
	}
	/// up to here

	return ERROR_SUCCESS;
}