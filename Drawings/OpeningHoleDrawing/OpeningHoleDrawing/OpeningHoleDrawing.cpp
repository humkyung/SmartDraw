// OpeningHoleDrawing.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include <IsTools.h>
#include <util/FileTools.h>
#include <SmartDrawAnnoEnv.h>

#include <SmartDrawPDSModuleImpl.h>
#include <SmartDrawAnno.h>

#include <RevisionChart.h>
#include <TitleBlockData.h>

#include "OpeningHoleDrawing.h"
#include "OpeningHoleDrawingImpl.h"
#include "OpeningHoleAnnoTerritory.h"
#include "OpeningHoleDrawingOption.h"
#include "OpeningHoleGroupData.h"
#include "OpeningHoleInternalTerritory.h"
#include "OpeningHoleChart.h"
#include "OpeningHolePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// COpeningHoleDrawingApp

BEGIN_MESSAGE_MAP(COpeningHoleDrawingApp, CWinApp)
END_MESSAGE_MAP()


// COpeningHoleDrawingApp construction

COpeningHoleDrawingApp::COpeningHoleDrawingApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_sDrawingName = _T("OpeningHole");
	m_sHsrType = _T("Generate");
}


// The one and only COpeningHoleDrawingApp object

COpeningHoleDrawingApp theApp;


// COpeningHoleDrawingApp initialization

BOOL COpeningHoleDrawingApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

COpeningHoleDrawingImpl::COpeningHoleDrawingImpl()
{
}

COpeningHoleDrawingImpl::~COpeningHoleDrawingImpl()
{
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   GenerateCmdFile
    @return     int
    @param      const       CString&
    @param      sFilePath   const
    @param      CString&    sAnnoFilePath
    @brief
******************************************************************************/
static int GenerateCmdFile(const CString& sCmdFilePath , const CString& sOutputFilePath , const CString& sHsrFilePath , const CString& sAnnoFilePath)
{
	OFSTREAM_T ofile(sCmdFilePath.operator LPCTSTR());
	if(ofile.is_open())
	{
		ofile << _T("NEWFILE ") << sOutputFilePath << std::endl;
		ofile << _T("MDL LOAD aDgnWriter \"") << sHsrFilePath  << _T("\"") << std::endl;
		ofile << _T("MDL LOAD aDgnWriter \"") << sAnnoFilePath << _T("\"") << std::endl;
		
		ofile.close();
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-09
    @class      COpeningHoleDrawingImpl
    @function   Annotate
    @return     int
    @param      const STRING_T& sOutputFilePath
	@param		const STRING_T& sHsrFilePath
    @param      CaDraw_DraFile* pDraFile
    @brief
******************************************************************************/
int COpeningHoleDrawingImpl::Annotate(const STRING_T& sOutputFilePath , const STRING_T& sHsrFilePath , CaDraw_DraFile* pDraFile)
{
	assert(pDraFile && "pDraFile is NULL");

	if(pDraFile)
	{
		this->ParseHsrTextFile(sHsrFilePath);
		
		COpeningHoleGroupData::m_iGroupCount = 1;

		pDraFile->CreateLogicView();
		list<CaDraw_LogicView*> oLogicViewList;
		pDraFile->GetLogicViewList(&oLogicViewList);
		for(list<CaDraw_LogicView*>::iterator itr = oLogicViewList.begin();itr != oLogicViewList.end();++itr)
		{
			COpeningHoleAnnoTerritory* pAnnoTerritory = new COpeningHoleAnnoTerritory(*itr , new COpeningHoleInternalTerritory);
			if(NULL != pAnnoTerritory)
			{
				pAnnoTerritory->Prepare();
				pAnnoTerritory->Annotate(*m_pHsrLineList);
				m_pAnnoTerritoryList->push_back(pAnnoTerritory);
			}
		}

		this->Write(sOutputFilePath);

		/// clear
		for(list<CAnnoTerritory*>::iterator itr = m_pAnnoTerritoryList->begin();itr != m_pAnnoTerritoryList->end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_pAnnoTerritoryList->clear();
		/// up to here

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   Write
    @return     int
    @param      const STRING_T& sOutputFilePath
    @brief
******************************************************************************/
int COpeningHoleDrawingImpl::Write(const STRING_T& sOutputFilePath)
{
	OFSTREAM_T ofile(sOutputFilePath.c_str());
	if(ofile.is_open())
	{
		ofile.precision( 5 );
		ofile.setf(ios_base:: fixed, ios_base:: floatfield);

		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		env.m_extDimStyle.Write(ofile);
		env.m_intDimStyle.Write(ofile);
		
		for(list<CAnnoTerritory*>::iterator itr = m_pAnnoTerritoryList->begin();itr != m_pAnnoTerritoryList->end();++itr)
		{
			(*itr)->Write(ofile);
		}

		CTitleBlockData oTitleBlockData;
		oTitleBlockData.Load(m_sIniFilePath.c_str() , m_sSdrFileName.c_str());
		oTitleBlockData.Write(ofile);

		CRevisionChart oRevChart;
		oRevChart.Load(m_sIniFilePath.c_str() , m_sSdrFileName.c_str());
		oRevChart.Write(ofile);

		/// write hole chart
		COpeningHoleChart oHoleChart;
		for(list<CAnnoTerritory*>::iterator itr = m_pAnnoTerritoryList->begin();itr != m_pAnnoTerritoryList->end();++itr)
		{
			COpeningHoleAnnoTerritory* pTerritory = static_cast<COpeningHoleAnnoTerritory*>(*itr);
			COpeningHoleInternalTerritory* pIntTerritory = static_cast<COpeningHoleInternalTerritory*>(pTerritory->GetInternalTerritory());
			oHoleChart.m_oHoleChartItemGroupEntry.insert(oHoleChart.m_oHoleChartItemGroupEntry.begin() , 
				pIntTerritory->m_oOpeningHoleGroupDataList.begin() , pIntTerritory->m_oOpeningHoleGroupDataList.end());
		}
		oHoleChart.Write(ofile);

		ofile.close();

		return ERROR_SUCCESS;
	}

	return ERROR_FILE_NOT_FOUND;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-09
    @class
    @function   GetModelFolderPathWith
    @return     STRING_T
    @param      CaDraw_Entity::MODEL_TYPE_T modelType
    @param      const                       STRING_T&
    @param      sPrjFilePath
    @brief
******************************************************************************/
STRING_T GetModelFolderPathWith(CaDraw_Entity::MODEL_TYPE_T modelType , const STRING_T& sPrjFilePath)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	
	memset(szBuf , '\0' , sizeof(TCHAR)*(MAX_PATH + 1));
	switch(modelType)
	{
	case CaDraw_Entity::PIPE_MODEL_T:
		GetPrivateProfileString(_T("Folder") , _T("Pipe") , _T("") , szBuf , MAX_PATH , sPrjFilePath.c_str());
		break;
	case CaDraw_Entity::EQUIPMENT_MODEL_T:
		GetPrivateProfileString(_T("Folder") , _T("Equipment") , _T("") , szBuf , MAX_PATH , sPrjFilePath.c_str());
		break;
	case CaDraw_Entity::STRUCTURE_MODEL_T:
		GetPrivateProfileString(_T("Folder") , _T("Structure") , _T("") , szBuf , MAX_PATH , sPrjFilePath.c_str());
		break;
	case CaDraw_Entity::CABLE_T_MODEL_T:
		GetPrivateProfileString(_T("Folder") , _T("CableT") , _T("") , szBuf , MAX_PATH , sPrjFilePath.c_str());
		break;
	case CaDraw_Entity::MISC_MODEL_T:
		GetPrivateProfileString(_T("Folder") , _T("Misc") , _T("") , szBuf , MAX_PATH , sPrjFilePath.c_str());
		break;
	case CaDraw_Entity::DRAWING_MODEL_T:
		GetPrivateProfileString(_T("Folder") , _T("Drawing") , _T("") , szBuf , MAX_PATH , sPrjFilePath.c_str());
		break;
	};

	return szBuf;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-09
    @class
    @function   GetDrawingName
    @return     LPCTSTR
    @brief
******************************************************************************/
extern "C" __declspec(dllexport) 
LPCTSTR __stdcall GetDrawingName()
{
	return (theApp.m_sDrawingName.operator LPCTSTR());
}

extern "C" __declspec(dllexport) 
LPCTSTR __stdcall GetDialogList()
{
	return (theApp.m_sDrawingName.operator LPCTSTR());
}

extern "C" __declspec(dllexport) 
HWND __stdcall CreatePropertyPage(LPCTSTR pName , LPCTSTR pIniFilePath , HWND hParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(NULL == COpeningHolePage::instance)
	{ 
		CWnd *pParent = CWnd::FromHandle(hParent);
		COpeningHolePage::instance = new COpeningHolePage(pParent);
		COpeningHolePage::instance->Create(pIniFilePath , COpeningHolePage::IDD , pParent); 
		COpeningHolePage::instance->ShowWindow(SW_SHOW); 
	}

	return COpeningHolePage::instance->GetSafeHwnd();
}

extern "C" __declspec(dllexport) 
int __stdcall SaveSetting()
{
	if(NULL != COpeningHolePage::instance)
	{
		COpeningHolePage::instance->SaveData();
	}

	return ERROR_SUCCESS;
}

extern "C" __declspec(dllexport) 
void __stdcall ClosePropertyPage(LPCTSTR pName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(NULL != COpeningHolePage::instance)
	{ 
		COpeningHolePage::instance->DestroyWindow();
		delete COpeningHolePage::instance; 
		COpeningHolePage::instance = NULL;
	} 
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-09
    @function   GetHsrType
    @return     LPCTSTR
    @brief
******************************************************************************/
extern "C" __declspec(dllexport) 
LPCTSTR __stdcall GetHsrType()
{
	return (theApp.m_sHsrType.operator LPCTSTR());
}

extern "C" __declspec(dllexport) 
int __stdcall Annotate(LPCTSTR pOutputFilePath , LPCTSTR pDraFilePath , LPCTSTR pPrjName , LPCTSTR pPrjFolderPath)
{
	assert(pOutputFilePath && "pOutputFilePath is NULL");
	assert(pDraFilePath && "pDraFilePath is NULL");
	assert(pPrjName && "pPrjName is NULL");
	assert(pPrjFolderPath && "pPrjFolderPath is NULL");

	if(pOutputFilePath && pDraFilePath && pPrjName && pPrjFolderPath)
	{
		TCHAR szBuf[MAX_PATH + 1]={'\0',};
		STRING_T sPrjFilePath(pPrjFolderPath + STRING_T(_T("\\Setting\\")) + pPrjName + STRING_T(_T(".prj")));
		if(0 == GetPrivateProfileString(_T("Database") , _T("Access File") , _T("") , szBuf , MAX_PATH , sPrjFilePath.c_str()))
		{
			return ERROR_BAD_ENVIRONMENT;
		}
		STRING_T sDatabaseFilePath(szBuf) , sIniFilePath;

		auto_ptr<CaDraw_DraFile> pDraFile(new CaDraw_DraFile(pDraFilePath));
		if(ERROR_SUCCESS == pDraFile->Parse())
		{
			sIniFilePath = (pPrjFolderPath + STRING_T(_T("\\Setting\\")) + pDraFile->GetSettingFileName() + STRING_T(_T(".ini")));
			
			COpeningHoleDrawingOption& oOpeningHoleEnv = COpeningHoleDrawingOption::GetInstance();
			oOpeningHoleEnv.Parse(sIniFilePath);

			list<CaDraw_View*> oViewList;
			pDraFile->GetViewList(&oViewList);
			for(list<CaDraw_View*>::iterator itr = oViewList.begin();itr != oViewList.end();++itr)
			{
				list<CaDraw_View::MODEL_T>* pModelList = (*itr)->GetModelList();
				for(list<CaDraw_View::MODEL_T>::iterator jtr = pModelList->begin();jtr != pModelList->end();++jtr)
				{
					CaDraw_Entity::MODEL_TYPE_T modelType = (jtr->modelType);
					STRING_T sModelFolderPath = GetModelFolderPathWith(modelType , sPrjFilePath);
					STRING_T rModelFilePath = sModelFolderPath + _T("\\") + jtr->rModelName.c_str();

					CSmartDrawPDSModuleImpl oCadModule;
					oCadModule.ExtractEntities(*itr , modelType , rModelFilePath , sIniFilePath , sDatabaseFilePath);
				}
			}

			const STRING_T sFileNameWithoutExt = pDraFile->GetFileNameWithoutExt();
			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// at this time, .hsr file is already created by SmartHSR
			STRING_T sHsrFilePath = pPrjFolderPath + STRING_T(_T("\\Working\\")) + sFileNameWithoutExt + _T(".hsr");
			
			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// run annotation
			const STRING_T sAnnoFilePath = pPrjFolderPath + STRING_T(_T("\\Working\\")) + sFileNameWithoutExt + _T(".anno");
			STRING_T sDrawingModelFolderPath = GetModelFolderPathWith(CaDraw_Entity::DRAWING_MODEL_T , sPrjFilePath);
			{
				CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
				env.Parse(sIniFilePath);

				pDraFile->SetExtTagSide( env.m_area.m_iExtTagSide );
				auto_ptr<COpeningHoleDrawingImpl> oDrawing( new COpeningHoleDrawingImpl() );
				oDrawing->Annotate(sAnnoFilePath.c_str() , sHsrFilePath.c_str() , pDraFile.get());
			}

			/// write hsr + anno to output file
			/// copy border file to output file
			TCHAR szBuf[MAX_PATH + 1]={'\0',};
			if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
			{
				::CopyFile(pPrjFolderPath + CString(_T("\\Border\\")) + szBuf , pOutputFilePath , FALSE);
			}
			else
			{
				/// use seed file
				::CopyFile(CFileTools::GetAppDataPath() + _T("\\SmartDraw for PDS\\Seed\\seed3d.dgn") , pOutputFilePath , FALSE);
			}
			/// up to here
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}