#include "StdAfx.h"
#include <assert.h>
#include <tchar.h>
#include <util\Markup\Markup.h>
#include "aDraw_DraFile.h"

#include "soci.h"
#include "soci-sqlite3.h"

#include <Tokenizer.h>
#include <IsTools.h>
#include <vector>
#include <sstream>
#include <fstream>

CaDraw_DraFile::CaDraw_DraFile(const STRING_T& rFilePath)
{
	m_iExtTagSide = (CaDraw_DraFile::LEFT_EXT_SIDE | CaDraw_DraFile::RIGHT_EXT_SIDE | CaDraw_DraFile::TOP_EXT_SIDE | CaDraw_DraFile::BOTTOM_EXT_SIDE);
	m_eViewCoords = CaDraw_View::COORD_TOPLEFT_BOTTOMRIGHT;

	m_pFilePath = new STRING_T(rFilePath);
	m_pSettingFileName = new STRING_T;
	m_pViewList = new list<CaDraw_View*>;
	m_pLogicViewList = new list<CaDraw_LogicView*>;
}

CaDraw_DraFile::~CaDraw_DraFile(void)
{
	try
	{
		Clear();
		SAFE_DELETE(m_pFilePath);
		if(NULL != m_pSettingFileName) SAFE_DELETE(m_pSettingFileName);
		SAFE_DELETE( m_pViewList );
		if(NULL != m_pLogicViewList) SAFE_DELETE(m_pLogicViewList);
	}
	catch(...)
	{

	}
}

/**	
	@brief	remove a view from dra file
	@author	HumKyung.BAEK
	@date	2010.05.12
	@param	
	@return
*/
int CaDraw_DraFile::RemoveView(const STRING_T& sViewName)
{
	for(list<CaDraw_View*>::iterator itr = m_pViewList->begin();itr != m_pViewList->end();++itr)
	{
		if(sViewName == (*itr)->name())
		{
			SAFE_DELETE(*itr);
			m_pViewList->erase(itr);
			return ERROR_SUCCESS;
		}
	}

	return ERROR_INVALID_PARAMETER;
}

/**	
	@brief	change view name

	@author	HumKyung.BAEK

	@date	2010.06.05

	@param	

	@return
*/
int CaDraw_DraFile::ChangeViewName(const STRING_T& rNewViewName, const STRING_T& rViewName)
{
	if(ERROR_SUCCESS == CopyView(rNewViewName , rViewName))
	{
		RemoveView(rViewName);
		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**	
	@brief	copy a view
	@author	HumKyung.BAEK
	@date	2010.05.12
	@param	
	@return
*/
CaDraw_View* CaDraw_DraFile::CopyView(const STRING_T& rNewViewName, const STRING_T& rViewName)
{
	CaDraw_View* pDraView = this->GetView(rViewName);
	if((NULL == this->GetView(rNewViewName)) && (NULL != pDraView))
	{
		CaDraw_View* pNewDraView = static_cast<CaDraw_View*>(CaDraw_View::CreateInstance());
		if(NULL != pNewDraView)
		{
			pNewDraView->Copy(pDraView);
			pNewDraView->SetName(rNewViewName);
			
			m_pViewList->push_back( pNewDraView );
			
			return pNewDraView;
		}
	}

	return NULL;
}

/**	
	@brief	get total reference files
	@author	HumKyung.BAEK
	@date	2010.05.14
	@param	
	@return
*/
int CaDraw_DraFile::GetTotalReferenceFile(list<STRING_T>& TotalReferenceFile)
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	TotalReferenceFile.clear();

	vector<STRING_T> oResult;
	if(GetPrivateProfileString(_T("GENERAL") , _T("TOTAL_VIEW_LIST") , NULL , szBuf , MAX_PATH , m_pFilePath->c_str()))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());

		for(vector<STRING_T>::iterator itr = oResult.begin();itr != oResult.end();++itr)
		{
			if(GetPrivateProfileString(itr->c_str() , _T("Ref File Count") , NULL , szBuf , MAX_PATH , m_pFilePath->c_str()))
			{
				const int nRefFileCount = ATOI_T(szBuf);
				for(int i = 0;i < nRefFileCount;++i)
				{
					OSTRINGSTREAM_T oss;
					oss << _T("Ref File") << i;
					if(GetPrivateProfileString(itr->c_str() , oss.str().c_str() , NULL , szBuf , MAX_PATH , m_pFilePath->c_str()))
					{
						list<STRING_T>::iterator where = find(TotalReferenceFile.begin() , TotalReferenceFile.end() , szBuf);
						if(where == TotalReferenceFile.end())
						{
							TotalReferenceFile.push_back(szBuf);
						}
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	check if dra file include volume

	@author	HumKyung.BAEK

	@date	2010.05.17

	@param	

	@return
*/
bool CaDraw_DraFile::DoesIncludeVolume(const CIsVolume& volume)
{
	for(list<CaDraw_View*>::iterator itr = m_pViewList->begin();itr != m_pViewList->end();++itr)
	{
		const bool res = (*itr)->DoesIncludeVolume(volume);
		if(true == res) return true;
	}

	return false;
}

/**	
	@brief	put a draw entity to drafile instance.

	@author	HumKyung.BAEK

	@date	2010.05.24

	@param	

	@return
*/
//int CaDraw_DraFile::Add(CaDraw_Entity* pEnt)
//{
//	assert(pEnt && "pEnt is NULL");
//
//	if(pEnt)
//	{
//		const CIsVolume volume = pEnt->GetVolume();
//
//		bool bAlreadyAdded = false;
//		for(list<CaDraw_LogicView*>::iterator itr = m_pLogicViewList->begin();itr != m_pLogicViewList->end();++itr)
//		{
//			list<CaDraw_View*> ViewList;
//			(*itr)->GetViewIncludeEntity(ViewList , pEnt);
//			for(list<CaDraw_View*>::iterator jtr = ViewList.begin();jtr != ViewList.end();++jtr)
//			{
//				if(!volume.CollideWith((*jtr)->GetVolume())) continue;
//
//				if(true == bAlreadyAdded)
//				{
//					CaDraw_Entity* pNewEnt = pEnt->Clone();
//					(*jtr)->Add(pNewEnt);
//				}
//				else
//				{
//					(*jtr)->Add(pEnt);
//					bAlreadyAdded = true;
//				}
//			}
//		}
//
//		return ERROR_SUCCESS;
//	}
//
//	return ERROR_BAD_ENVIRONMENT;
//}

/**	
	@brief	dra 파일을 분석한다.
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
int CaDraw_DraFile::Parse()
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	/// clear already created views
	Clear();

	CMarkup xml;
	if(true == xml.Load(*m_pFilePath))
	{
		MCD_STR sLoadResult = xml.GetResult();

		if(true == xml.FindElem(MCD_T("DRA")))
		{
			int iCount = 0;
			while ( 1 )
			{
				STRING_T sName = xml.GetAttribName(iCount++);
				if ( sName.empty() ) break;
				STRING_T sValue = xml.GetAttrib( sName );

				if(0 == STRICMP_T(sName.c_str() , _T("SETTING")))
				{
					(*m_pSettingFileName) = sValue;
				}
			}
			
			xml.IntoElem();
			while(true == xml.FindElem(MCD_T("VIEW")))
			{
				CaDraw_View* pView = (CaDraw_View*)CaDraw_View::CreateInstance();
				if(pView)
				{
					pView->Parse(xml);
					m_pViewList->push_back( pView );
				}
			}
			xml.OutOfElem();
		}
		
		return ERROR_SUCCESS;
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   Save
    @return     int
    @brief
******************************************************************************/
int CaDraw_DraFile::Save()
{
	try
	{
		CMarkup xml;
		xml.SetDoc( _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n") );

		xml.AddElem( _T("DRA") );
		{
			xml.AddAttrib( _T("Setting") , (*m_pSettingFileName));
		}

		for(list<CaDraw_View*>::iterator itr  = m_pViewList->begin();itr != m_pViewList->end();++itr)
		{
			xml.AddChildElem( _T("VIEW") );
			xml.IntoElem();
			(*itr)->Save(xml);
			xml.OutOfElem();
		}
		
		xml.Save(*m_pFilePath);
	}
	catch(const exception& ex)
	{
		::MessageBox(NULL , LPCTSTR(ex.what()) , _T("Message") , MB_OK);
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   CreateLogicView
    @return     int
    @brief
******************************************************************************/
int CaDraw_DraFile::CreateLogicView()
{
	const double nViewToler = 1.5f;
	for(list<CaDraw_View*>::iterator itr = m_pViewList->begin();itr != m_pViewList->end();++itr)
	{
		bool bAddView = false;
		for(list<CaDraw_LogicView*>::iterator jtr = m_pLogicViewList->begin();jtr != m_pLogicViewList->end();++jtr)
		{
			if((*jtr)->IsAdjacentView(*itr , nViewToler))
			{
				(*jtr)->AddView(*itr);
				bAddView = true;
				break;
			}
		}
		if(false == bAddView)
		{
			CaDraw_LogicView* pLogicView = (CaDraw_LogicView*)CaDraw_LogicView::CreateInstance();
			pLogicView->AddView(*itr);
			m_pLogicViewList->push_back(pLogicView);
		}
	}

	for(list<CaDraw_LogicView*>::iterator itr = m_pLogicViewList->begin();itr != m_pLogicViewList->end();++itr)
	{
		(*itr)->CreateViewBoundary(nViewToler , this->m_iExtTagSide , this->m_eViewCoords);
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	clear contents
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
int CaDraw_DraFile::Clear(void)
{
	/// clear contents of view list - 2014.06.03 added by humkyung
	if(NULL != m_pViewList)
	{
		for(list<CaDraw_View*>::iterator itr = m_pViewList->begin();itr != m_pViewList->end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_pViewList->clear();
	}
	/// up to here

	if(NULL != m_pLogicViewList)
	{
		for(list<CaDraw_LogicView*>::iterator itr = m_pLogicViewList->begin();itr != m_pLogicViewList->end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_pLogicViewList->clear();
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-11
    @function   GetViewList
    @return     int
    @param      list<CaDraw_View*>* pViewList
    @brief
******************************************************************************/
int CaDraw_DraFile::GetViewList(list<CaDraw_View*>* pViewList) const
{
	assert(pViewList && "pViewList is NULL");

	if(pViewList)
	{
		pViewList->clear();
		pViewList->insert(pViewList->begin() , m_pViewList->begin() , m_pViewList->end());

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.25

	@param	

	@return
*/
int CaDraw_DraFile::GetLogicViewList(list<CaDraw_LogicView*>* pLogicViewList) const
{
	assert(pLogicViewList && "pLogicViewList is NULL");

	if(pLogicViewList)
	{
		pLogicViewList->clear();
		pLogicViewList->insert(pLogicViewList->begin() , m_pLogicViewList->begin() , m_pLogicViewList->end());

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.27
	@param	
	@return
*/
STRING_T CaDraw_DraFile::GetFileNameWithoutExt() const
{
	STRING_T::size_type at = m_pFilePath->find_last_of( _T("\\") );
	STRING_T rFileName = m_pFilePath->substr(at + 1);
	at = rFileName.find_last_of(_T("."));
	rFileName = rFileName.substr(0 , at);

	return rFileName;
}


/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.05.27

	@param	

	@return
*/
const TCHAR* CaDraw_DraFile::GetSettingFileName() const
{
	return m_pSettingFileName->c_str();
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-13
    @class      CaDraw_DraFile
    @function   SetSettingFileName
    @return     int
    @param      LPCTSTR pSettingFileName
    @brief
******************************************************************************/
int CaDraw_DraFile::SetSettingFileName(LPCTSTR pSettingFileName)
{
	assert(pSettingFileName && "pSettingFileName is NULL");

	if(pSettingFileName)
	{
		m_pSettingFileName->assign(pSettingFileName);
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   AddView
    @return     CaDraw_View*
    @param      const       STRING_T&
    @param      sViewName
    @brief
******************************************************************************/
CaDraw_View* CaDraw_DraFile::AddView(const STRING_T& sViewName)
{
	for(list<CaDraw_View*>::iterator itr = m_pViewList->begin();itr != m_pViewList->end();++itr)
	{
		if(sViewName == (*itr)->name()) return NULL;
	}

	CaDraw_View* pDraView = (CaDraw_View*)(CaDraw_View::CreateInstance());
	pDraView->SetName( sViewName );
	m_pViewList->push_back( pDraView );

	return pDraView;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-12
    @function   GetView
    @return     CaDraw_View*
    @param      const       STRING_T&
    @param      sViewName
    @brief
******************************************************************************/
CaDraw_View* CaDraw_DraFile::GetView(const STRING_T& sViewName)
{
	for(list<CaDraw_View*>::iterator itr = m_pViewList->begin();itr != m_pViewList->end();++itr)
	{
		if(sViewName == (*itr)->name()) return (*itr);
	}

	return NULL;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-17
    @function   GetFilePath
    @return     STRING_T
    @brief
******************************************************************************/
STRING_T CaDraw_DraFile::GetFilePath() const
{
	return (*m_pFilePath);
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-18
    @function   SetExtTagSide
    @return     int
    @param      const       int&
    @param      iExtTagSide
    @brief
******************************************************************************/
int CaDraw_DraFile::SetExtTagSide(const int& iExtTagSide)
{
	m_iExtTagSide = iExtTagSide;

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-25
    @function   SetViewCoords
    @return     int
    @param      const CaDraw_View::COORD_OPTION_E& eCoords
    @brief
******************************************************************************/
int CaDraw_DraFile::SetViewCoords(const CaDraw_View::COORD_OPTION_E& eCoords)
{
	m_eViewCoords = eCoords;
	return ERROR_SUCCESS;
}

/******************************************************************************
	@brief
    @author     humkyung
    @date       2012-02-15
    @return     int
******************************************************************************/
int CaDraw_DraFile::Report(const STRING_T& sReportFilePath) const
{
	soci::session oSession(*soci::factory_sqlite3() , sReportFilePath.c_str());
	{
		const STRING_T sFileNameWithoutExt = this->GetFileNameWithoutExt();
		STRING_T sDocGuid = NewGuid();
		OSTRINGSTREAM_T oss;
		oss << _T("insert into Drawing(Guid,Name) values('") << sDocGuid << _T("','") << sFileNameWithoutExt << _T("')");
		oSession << oss.str();

		for(list<CaDraw_View*>::const_iterator itr = m_pViewList->begin();itr != m_pViewList->end();++itr)
		{
			(*itr)->Report(oSession,sDocGuid);
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/******************************************************************************
    @brief
	@author     humkyung
    @date       2016-02-11
    @function   GenerateReport
    @return     int
    @param      const STRING_T& sReportFilePath
******************************************************************************/
int CaDraw_DraFile::GenerateReport(const STRING_T& sReportFilePath)
{
	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		reutrn the status of dra file
	@author     humkyung
    @date       2014-06-16
    @class      CaDraw_DraFile
    @function   status
    @return     LPCTSTR
******************************************************************************/
LPCTSTR CaDraw_DraFile::status() const
{
	static STRING_T res;

	res = _T("");
	int iStatus=0;
	for(list<CaDraw_View*>::const_iterator itr = m_pViewList->begin();itr != m_pViewList->end();++itr)
	{
		iStatus |= (*itr)->status();
	}

	if(iStatus & CaDraw_View::DUPLICATED_MODEL_FILE)
	{
		res += _T("IDS_DUPLICATED_MODEL_FILE");
	}
	if(iStatus & CaDraw_View::NO_MODEL_FILE)
	{
		if(!res.empty()) res += _T(",");
		res += _T("IDS_NO_MODEL_FILE");
	}

	return res.c_str();
}

/******************************************************************************
    @brief		reset match line data
	@author     humkyung
    @date       2016.03.07
    @return     int
******************************************************************************/
int CaDraw_DraFile::ResetMatchLineData(const STRING_T& sNoMatchLineDwg)
{
	for(list<CaDraw_View*>::iterator itr = m_pViewList->begin();itr != m_pViewList->end();++itr)
	{
		(*itr)->SetMatchLineData(CaDraw_View::UP_SIDE_T,sNoMatchLineDwg.c_str());
		(*itr)->SetMatchLineData(CaDraw_View::DOWN_SIDE_T,sNoMatchLineDwg.c_str());
		(*itr)->SetMatchLineData(CaDraw_View::EAST_SIDE_T,sNoMatchLineDwg.c_str());
		(*itr)->SetMatchLineData(CaDraw_View::WEST_SIDE_T,sNoMatchLineDwg.c_str());
		(*itr)->SetMatchLineData(CaDraw_View::NORTH_SIDE_T,sNoMatchLineDwg.c_str());
		(*itr)->SetMatchLineData(CaDraw_View::SOUTH_SIDE_T,sNoMatchLineDwg.c_str());
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		fill match line data
	@author     humkyung
    @date       2016.03.07
    @return     int
******************************************************************************/
int CaDraw_DraFile::FillMatchLineData(const CaDraw_DraFile* pDraFile,const STRING_T& sMatchLineDwgPrefix,const STRING_T& sNoMatchLineDwg)
{
	assert(pDraFile && "pDraFile is null");
	if(NULL != pDraFile)
	{
		list<CaDraw_View*> oViewList;
		pDraFile->GetViewList(&oViewList);
		for(list<CaDraw_View*>::iterator itr = m_pViewList->begin();itr != m_pViewList->end();++itr)
		{
			for(list<CaDraw_View*>::const_iterator jtr = oViewList.begin();jtr != oViewList.end();++jtr)
			{
				const int res = (*itr)->IsAdjacent(*jtr,100.0);
				if(CaDraw_View::NONE_SIDE_T != res)
				{
					const STRING_T sDwgNo = pDraFile->GetDwgNo();
					const STRING_T sMatchLineData = sMatchLineDwgPrefix + sDwgNo;

					if(STRING_T((*itr)->GetMatchLineData(res)) == sNoMatchLineDwg) (*itr)->SetMatchLineData(res,sMatchLineData.c_str());
				}
			}
		}
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	return drawing no
	@author	humkyung
	@date	2016.03.26
*/
STRING_T CaDraw_DraFile::GetDwgNo() const
{
	TCHAR szBuf[MAX_PATH+1]={'\0',};

	const STRING_T sDefaultValue = GetFileNameWithoutExt();
	const STRING_T sDraIniFilePath = (*m_pFilePath) + _T(".ini");
	GetPrivateProfileString(_T("Title Block Data"),_T("DWG.NO 0"),sDefaultValue.c_str(),szBuf,MAX_PATH,sDraIniFilePath.c_str());
	return szBuf;
}