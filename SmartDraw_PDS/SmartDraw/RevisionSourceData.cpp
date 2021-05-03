#include "StdAfx.h"
#include <assert.h>
#include <util/Markup/Markup.h>
#include <RevisionChart.h>
#include "RevisionSourceData.h"

CRevisionData::CRevisionData()
{
	m_pValueMap = new std::tr1::unordered_map<STRING_T , STRING_T>();
	if(NULL != m_pValueMap)
	{
		const int iSize = sizeof(CRevisionChart::COLUMNS)/sizeof(STRING_T);
		for(int i = 0;i < iSize;++i)
		{
			m_pValueMap->insert(make_pair(CRevisionChart::COLUMNS[i] , _T("")));
		}
	}
}

CRevisionData::~CRevisionData()
{
	try
	{
		SAFE_DELETE(m_pValueMap);
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @brief		return revision data string
	@author     humkyung
    @date       2014-11-17
    @function   ToString()
    @return     STRING_T
******************************************************************************/
STRING_T CRevisionData::ToString() const
{
	STRINGSTREAM_T oss;
	
	const int iSize = sizeof(CRevisionChart::COLUMNS)/sizeof(STRING_T);
	for(int i = 0;i < iSize;++i)
	{
		oss << this->Value(CRevisionChart::COLUMNS[i].c_str());
		if((i + 1) < iSize) oss << _T(",");
	}

	return oss.str();
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   Value
    @return     STRING_T
    @param      const STRING_T& sKey
    @brief
******************************************************************************/
STRING_T CRevisionData::Value(const STRING_T& sKey) const
{
	std::tr1::unordered_map<STRING_T,STRING_T>::const_iterator where = m_pValueMap->find(sKey);
	if(where != m_pValueMap->end())
	{
		return where->second;
	}

	return STRING_T(_T(""));
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   Value
    @return     STRING_T&
    @param      const   STRING_T&
    @param      sKey
    @brief
******************************************************************************/
STRING_T& CRevisionData::Value(const STRING_T& sKey)
{
	std::tr1::unordered_map<STRING_T,STRING_T>::iterator where = m_pValueMap->find(sKey);
	if(where != m_pValueMap->end())
	{
		return (where->second);
	}

	throw exception("out of index");
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-28
    @function   Parse
    @return     int
    @param      CMarkup &xml
    @brief
******************************************************************************/
int CRevisionData::Parse(CMarkup &xml)
{
	int iCount = 0;
	while ( 1 )
	{
		STRING_T sName = xml.GetAttribName(iCount++);
		if ( sName.empty() ) break;
		STRING_T sValue = xml.GetAttrib( sName.c_str() );

		std::tr1::unordered_map<STRING_T,STRING_T>::iterator where = m_pValueMap->find(sName);
		if(where != m_pValueMap->end())
		{
			where->second = sValue;
		}
		else
		{
			m_pValueMap->insert(make_pair(sName , sValue));
		}
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   Save
    @return     int
    @param      CMarkup&    xml
    @brief
******************************************************************************/
int CRevisionData::Save(CMarkup& xml)
{
	for(std::tr1::unordered_map<STRING_T,STRING_T>::iterator itr = m_pValueMap->begin();itr != m_pValueMap->end();++itr)
	{
		xml.AddAttrib( itr->first , itr->second);
	}

	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRevisionSourceData::CRevisionSourceData(void)
{
}

CRevisionSourceData::~CRevisionSourceData(void)
{
	try
	{
		this->Clear();
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   GetRevisionDataList
    @return     vector<CRevisionData*>*
    @brief
******************************************************************************/
vector<CRevisionData*>* CRevisionSourceData::GetRevisionDataList()
{
	return &m_oRevisionDataList;
}

/******************************************************************************
    @brief		return next rev. data corresponding to given rev. no
	@author     humkyung
    @date       2014-11-16
    @function   GetNextRevDataOf
    @return     CRevisionData*
******************************************************************************/
CRevisionData* CRevisionSourceData::GetNextRevDataOf(const STRING_T& sRevNo)
{
	for(vector<CRevisionData*>::iterator itr = m_oRevisionDataList.begin();itr != m_oRevisionDataList.end();++itr)
	{
		if(((*itr)->Value(_T("REV_NO")) == sRevNo) && ((itr+1) != m_oRevisionDataList.end()))
		{
			return (*(itr+1));
		}
	}

	return NULL;
}

/******************************************************************************
    @brief		return rev. data corresponding to given rev. no
	@author     humkyung
    @date       2014-11-16
    @function   GetRevDataOf
    @return     CRevisionData*
******************************************************************************/
CRevisionData* CRevisionSourceData::GetRevDataOf(const STRING_T& sRevNo)
{
	for(vector<CRevisionData*>::iterator itr = m_oRevisionDataList.begin();itr != m_oRevisionDataList.end();++itr)
	{
		if((*itr)->Value(_T("REV_NO")) == sRevNo)
		{
			return (*itr);
		}
	}

	return NULL;
}

/******************************************************************************
    @brief		return rev. data corresponding to given index
	@author     humkyung
    @date       2014-11-16
    @function   GetRevDataOf
    @return     CRevisionData*
******************************************************************************/
CRevisionData* CRevisionSourceData::GetRevDataOf(const int& at)
{
	if((at >= 0) && (at < int(m_oRevisionDataList.size()))) return m_oRevisionDataList[at];

	return NULL;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   Clear
    @return     int
    @brief
******************************************************************************/
int CRevisionSourceData::Clear()
{
	for(vector<CRevisionData*>::iterator itr = m_oRevisionDataList.begin();itr != m_oRevisionDataList.end();++itr)
	{
		SAFE_DELETE( *itr );
	}
	m_oRevisionDataList.clear();
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-28
    @class      CRevisionSourceData
    @function   Load
    @return     int
    @param      LPCTSTR pFilePath
    @brief
******************************************************************************/
int CRevisionSourceData::Load(LPCTSTR pFilePath)
{
	assert(pFilePath && "pFilePath is NULL");

	if(pFilePath)
	{
		m_sFilePath = pFilePath;

		CMarkup xml;
		if(true == xml.Load(pFilePath))
		{
			MCD_STR sLoadResult = xml.GetResult();

			if(true == xml.FindElem(MCD_T("RevisionData")))
			{
				xml.IntoElem();
				while(true == xml.FindElem(MCD_T("Data")))
				{
					CRevisionData* pData = new CRevisionData;
					if(NULL != pData)
					{
						pData->Parse(xml);
						m_oRevisionDataList.push_back( pData );
					}
				}
				xml.OutOfElem();
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-31
    @function   Save
    @return     int
    @brief
******************************************************************************/
int CRevisionSourceData::Save()
{
	try
	{
		CMarkup xml;
		xml.SetDoc( _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n") );

		xml.AddElem( _T("RevisionData") );
		for(vector<CRevisionData*>::iterator itr  = m_oRevisionDataList.begin();itr != m_oRevisionDataList.end();++itr)
		{
			xml.AddChildElem( _T("Data") );
			xml.IntoElem();
			(*itr)->Save(xml);
			xml.OutOfElem();
		}

		xml.Save(m_sFilePath);
	}
	catch(const exception& ex)
	{
		::MessageBox(NULL , LPCTSTR(ex.what()) , _T("MSG") , MB_OK);
	}

	return ERROR_SUCCESS;
}