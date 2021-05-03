// GenerationOption.cpp: implementation of the CGenerationOption class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <tchar.h>
#include <Tokenizer.h>
#include <IsString.h>
#include <aDraw_Entity.h>
#include "AnnoFilter.h"
#include "SmartDrawAnnoPos.h"
#include "GenerationOption.h"

#include "soci.h"
#include "soci-sqlite3.h"

using namespace tag_option;

CGenerationOption::CGenerationItem::CGenerationItem()
{
	m_pAnnoFilters = new AnnoFilters();
}

CGenerationOption::CGenerationItem::~CGenerationItem()
{
	try
	{
		for(AnnoFilters::iterator itr = m_pAnnoFilters->begin();itr != m_pAnnoFilters->end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_pAnnoFilters->clear();
		SAFE_DELETE(m_pAnnoFilters);
	}
	catch(...)
	{
	}
}

/**
	@brief	check if be able to generate given item
	@author	humkyung
	@date	2016.03.22
*/
bool CGenerationOption::CGenerationItem::IsEnable(CaDraw_Entity* pItem) const
{
	if(NULL != pItem)
	{
		return pItem->IsEnable(m_pAnnoFilters);
	}
	else
	{
		for(AnnoFilters::const_iterator itr = m_pAnnoFilters->begin();itr != m_pAnnoFilters->end();++itr)
		{
			if((STRING_T(_T("Const")) == (*itr)->GetOperator()) && (STRING_T(_T("False")) == (*itr)->GetValue())) return false; 
		}
	}

	return true;
}

/**
	@brief	parse generation item
	@author	humkyung
	@date	2016.03.21
*/
int CGenerationOption::CGenerationItem::Parse(vector<STRING_T>& oTokens,const STRING_T& sSettingName,const STRING_T& sRefFilePath)
{
	STRING_T sName(oTokens[0]);
	if(_T("ByFilter") == oTokens[1])
	{
		soci::session oSession(*soci::factory_sqlite3() , sRefFilePath.c_str());
		{
			OSTRINGSTREAM_T oss;
			oss << _T("select Seq,Prop,Oper,Value from AnnoFilters where Setting='") << sSettingName << _T("' and Item='") << oTokens[0] << _T("' order by Seq");
			soci::rowset<soci::row> rs(oSession.prepare << oss.str());
			for(soci::rowset<soci::row>::const_iterator itr = rs.begin();itr != rs.end();++itr)
			{
				if((soci::indicator::i_ok != itr->get_indicator(0)) || (soci::indicator::i_ok != itr->get_indicator(1)) || (soci::indicator::i_ok != itr->get_indicator(2))) continue;
				CAnnoFilter* pAnnoFilter = new CAnnoFilter(itr->get<int>(0),itr->get<STRING_T>(1),itr->get<STRING_T>(2),
				(soci::indicator::i_ok == itr->get_indicator(3) ? itr->get<STRING_T>(3) : _T("")));
				m_pAnnoFilters->push_back(pAnnoFilter);
			}
		}
	}
	else
	{
		CAnnoFilter* pAnnoFilter = new CAnnoFilter(1,_T("None"),_T("Const"), oTokens[1]);
		m_pAnnoFilters->push_back(pAnnoFilter);
	}

	IsString::ToUpper(sName);
	{
		this->eType = CGenerationOption::SEARCH_AROUND;
		this->eDist = CGenerationOption::SEARCH_NEAR;
		if(oTokens.size() >= 3)
		{
			if(_T("Horizontal") == oTokens[2])
				this->eType = CGenerationOption::SEARCH_HORIZONTAL;
			else if(_T("Vertical") == oTokens[2])
				this->eType = CGenerationOption::SEARCH_VERTICAL;
			else if(_T("Orthogonal") == oTokens[2])
				this->eType = CGenerationOption::SEARCH_ORGHOGONAL;
			else if(_T("Around") == oTokens[2])
				this->eType = CGenerationOption::SEARCH_AROUND;
		}
		if(4 == oTokens.size()) this->eDist = (_T("Near") == oTokens[3]) ? CGenerationOption::SEARCH_NEAR : ((_T("Middle") == oTokens[3]) ? CGenerationOption::SEARCH_MIDDLE : CGenerationOption::SEARCH_FAR);
	}
	
	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGenerationOption::CGenerationOption() : m_eOutputFormat(AUTOCAD)
{
	m_pLevelOnItemEntry[0] = new vector< STRING_T >;
	m_pLevelOnItemEntry[1] = new vector< STRING_T >;
	m_pLevelOnItemEntry[2] = new vector< STRING_T >;
	
	m_pLocationOrderEntry[0] = new vector<STRING_T>;
	m_pLocationOrderEntry[1] = new vector<STRING_T>;
	m_pLocationOrderEntry[2] = new vector<STRING_T>;

	m_pGenerationItemMap = new map<STRING_T,CGenerationItem*>;	/// 2014.03.30 added by humkyung
}

CGenerationOption::~CGenerationOption()
{
	try
	{
		SAFE_DELETE(m_pLevelOnItemEntry[0]);
		SAFE_DELETE(m_pLevelOnItemEntry[1]);
		SAFE_DELETE(m_pLevelOnItemEntry[2]);

		SAFE_DELETE(m_pLocationOrderEntry[0]);
		SAFE_DELETE(m_pLocationOrderEntry[1]);
		SAFE_DELETE(m_pLocationOrderEntry[2]);

		for(map<STRING_T,CGenerationItem*>::iterator itr = m_pGenerationItemMap->begin();itr != m_pGenerationItemMap->end();++itr)
		{
			SAFE_DELETE(itr->second);
		}
		SAFE_DELETE(m_pGenerationItemMap);			/// 2014.03.30 added by humkyung
	}
	catch(...)
	{
	}
}

/**	
	@brief	The CGenerationOption::Parse function
	@param	rIniFilePath.c_str()	a parameter of type const char *
	@return	void	
*/
void CGenerationOption::Parse(const STRING_T& rIniFilePath,const STRING_T& sRefFilePath)
{
	static const STRING_T sApp( _T("GENERATION_OPTIONS") );
	///get setting name from ini file name
	STRING_T sSettingName(rIniFilePath);
	STRING_T::size_type at = sSettingName.find_last_of(_T("\\"));
	sSettingName = sSettingName.substr(at+1);
	at = sSettingName.find_last_of(_T("."));
	sSettingName = sSettingName.substr(0,at);
	///up to here

	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	//////////////////////////////////////////////////////////////////////////
	//"[GENERATION_OPTION]"
	if(GetPrivateProfileString(sApp.c_str() , _T("Output Format") , NULL , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		if(0 == STRICMP_T(szBuf , _T("MSTN")))
			m_eOutputFormat = MSTN;
		else	m_eOutputFormat = AUTOCAD;
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Outside_Level") , _T("") ,szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		if(0 == STRCMP_T(szBuf,_T("All")))
			m_eTagItems = TAG_ALL;
		else if(0 == STRCMP_T(szBuf,_T("OnlyPipe")))
			m_eTagItems = TAG_ONLY_PIPE;
		else if(0 == STRCMP_T(szBuf,_T("PipeEqp")))
			m_eTagItems = TAG_PIPE_EQP;
		else if(0 == STRCMP_T(szBuf,_T("PipeStr")))
			m_eTagItems = TAG_PIPE_STR;
		else if(0 == STRCMP_T(szBuf,_T("No")))
			m_eTagItems = TAG_NO;
	}

	vector<STRING_T> oResult;
	const int iGenerationItemCount = GetPrivateProfileInt(sApp.c_str() , _T("Generation Item Count") , 0 , rIniFilePath.c_str());
	for(int i = 0;i < iGenerationItemCount;++i)
	{
		OSTRINGSTREAM_T oss;
		oss << _T("Generation Item") << i;
		GetPrivateProfileString(sApp.c_str() , oss.str().c_str() , _T("") , szBuf , MAX_PATH , rIniFilePath.c_str());
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		if(oResult.size() >= 2)
		{
			STRING_T sName(oResult[0]);
			CGenerationItem* pItem = new CGenerationItem();
			if(NULL != pItem)
			{
				pItem->Parse(oResult,sSettingName,sRefFilePath);
				if(m_pGenerationItemMap->end() == m_pGenerationItemMap->find(sName))
				{
					m_pGenerationItemMap->insert(m_pGenerationItemMap->end() , make_pair(sName , pItem));
				}
				else
				{
					(*m_pGenerationItemMap)[sName] = pItem;
				}
			}
		}
	}

	if(GetPrivateProfileString(sApp.c_str(),_T("Global_Dimension"),_T(""),szBuf,MAX_PATH,rIniFilePath.c_str()))
	{
		if(0 == STRICMP_T(szBuf,_T("Top_Left")))
			m_global_dim = TOP_LEFT_GLOBAL_DIMENSION;
		else	m_global_dim = ALL_GLOBAL_DIMENSION;
	}else	m_global_dim = ALL_GLOBAL_DIMENSION;

	/// dimension on/off items
	if( GetPrivateProfileString( sApp.c_str() , _T("Level1 Ele") , NULL , szBuf , MAX_PATH , rIniFilePath.c_str() ))
	{
		CTokenizer<CIsComma>::Tokenize( *(m_pLevelOnItemEntry[0]) , szBuf , CIsComma() );
	}
	else
	{
		m_pLevelOnItemEntry[0]->clear();
	}

	if( GetPrivateProfileString( sApp.c_str() , _T("Level2 Ele") , NULL , szBuf , MAX_PATH , rIniFilePath.c_str() ))
	{
		CTokenizer<CIsComma>::Tokenize( *(m_pLevelOnItemEntry[1]) , szBuf , CIsComma() );
	}
	else
	{
		m_pLevelOnItemEntry[1]->clear();
	}

	if( GetPrivateProfileString( sApp.c_str() , _T("Level3 Ele") , NULL , szBuf , MAX_PATH , rIniFilePath.c_str() ))
	{
		CTokenizer<CIsComma>::Tokenize( *(m_pLevelOnItemEntry[2]) , szBuf , CIsComma() );
	}
	else
	{
		m_pLevelOnItemEntry[2]->clear();
	}

	/// location order entry
	if( GetPrivateProfileString( sApp.c_str() , _T("Location Order1") , _T("Structure") , szBuf , MAX_PATH , rIniFilePath.c_str() ))
	{
		CTokenizer<CIsComma>::Tokenize( *(m_pLocationOrderEntry[0]) , szBuf , CIsComma() );
	}
	
	if( GetPrivateProfileString( sApp.c_str() , _T("Location Order2") ,  _T("Equipment") , szBuf , MAX_PATH , rIniFilePath.c_str() ))
	{
		CTokenizer<CIsComma>::Tokenize( *(m_pLocationOrderEntry[1]) , szBuf , CIsComma() );
	}
	
	if( GetPrivateProfileString( sApp.c_str() , _T("Location Order3") ,  _T("Pipe") , szBuf , MAX_PATH , rIniFilePath.c_str() ))
	{
		CTokenizer<CIsComma>::Tokenize( *(m_pLocationOrderEntry[2]) , szBuf , CIsComma() );
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-20
    @class      DIM_ENV_T
    @function   GetDimLevelOf
    @return     int
    @param      const STRING_T& sItemType
    @brief
******************************************************************************/
int CGenerationOption::GetDimLevelOf(const STRING_T& sItemType)
{
	int res = DIM_LEVEL_X;
	vector<STRING_T>::iterator where = find(m_pLevelOnItemEntry[0]->begin() , m_pLevelOnItemEntry[0]->end() , sItemType);
	if(where != m_pLevelOnItemEntry[0]->end())
	{
		res |= DIM_LEVEL_0;
	}
	
	where = find(m_pLevelOnItemEntry[1]->begin() , m_pLevelOnItemEntry[1]->end() , sItemType);
	if(where != m_pLevelOnItemEntry[1]->end())
	{
		res |= DIM_LEVEL_1;
	}

	where = find(m_pLevelOnItemEntry[2]->begin() , m_pLevelOnItemEntry[2]->end() , sItemType);
	if(where != m_pLevelOnItemEntry[2]->end())
	{
		res |= DIM_LEVEL_2;
	}

	return res;
}

/**
	@brief	check item can be generated
	@author	humkyung
	@date	2014.03.30
	@return	bool
*/
bool CGenerationOption::IsEnable(const STRING_T& sItemType,CaDraw_Entity* pEnt) const
{
	STRING_T str(sItemType);
	if(m_pGenerationItemMap->end() != m_pGenerationItemMap->find(str))
	{
		return (*m_pGenerationItemMap)[str]->IsEnable(pEnt);
	}

	return true;
}

/**
	@brief	return search area for given item
	@author	humkyung
	@date	2014.12.31
	@param	dLength default search length
	@param	dHeight default search height
	@return	int
*/
int CGenerationOption::GetSearchAreaOf(double& dLength , double& dHeight , const STRING_T& sItemType) const
{
	CGenerationOption::eSearchDist res = CGenerationOption::SEARCH_NEAR;
	if(m_pGenerationItemMap->end() != m_pGenerationItemMap->find(sItemType))
	{
		res = (*m_pGenerationItemMap)[sItemType]->eDist;
	}

	if(CGenerationOption::SEARCH_MIDDLE == res)
	{
		dLength *= 2;
		dHeight *= 2;
	}
	else if(CGenerationOption::SEARCH_FAR == res)
	{
		dLength *= 4;
		dHeight *= 4;
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		return the location order of given item
	@author     humkyung
    @date       2015-01-01
    @return     eLocationOrder
    @param      const STRING_T& sItemType
******************************************************************************/
CGenerationOption::eLocationOrder CGenerationOption::GetLocationOrderOf(const STRING_T& sItemType) const
{
	CGenerationOption::eLocationOrder res = CGenerationOption::LOCATION_ORDER1;
	vector<STRING_T>::iterator where = find(m_pLocationOrderEntry[0]->begin() , m_pLocationOrderEntry[0]->end() , sItemType);
	if(where != m_pLocationOrderEntry[0]->end())
	{
		res = CGenerationOption::LOCATION_ORDER1;
	}
	
	where = find(m_pLocationOrderEntry[1]->begin() , m_pLocationOrderEntry[1]->end() , sItemType);
	if(where != m_pLocationOrderEntry[1]->end())
	{
		res = CGenerationOption::LOCATION_ORDER2;
	}

	where = find(m_pLocationOrderEntry[2]->begin() , m_pLocationOrderEntry[2]->end() , sItemType);
	if(where != m_pLocationOrderEntry[2]->end())
	{
		res = CGenerationOption::LOCATION_ORDER3;
	}

	return res;
}

/******************************************************************************
    @brief		return item list of given location order
	@author     humkyung
    @date       2015-01-01
    @return     int
    @param      const STRING_T& sItemType
******************************************************************************/
int CGenerationOption::GetItemListOfLLocationOrder(vector<STRING_T>& aItemList , const CGenerationOption::eLocationOrder& order) const
{
	aItemList.clear();
	switch(order)
	{
		case CGenerationOption::LOCATION_ORDER1:
			aItemList.insert(aItemList.end() , m_pLocationOrderEntry[0]->begin() , m_pLocationOrderEntry[0]->end());
		break;
		case CGenerationOption::LOCATION_ORDER2:
			aItemList.insert(aItemList.end() , m_pLocationOrderEntry[1]->begin() , m_pLocationOrderEntry[1]->end());
		break;
		case CGenerationOption::LOCATION_ORDER3:
			aItemList.insert(aItemList.end() , m_pLocationOrderEntry[2]->begin() , m_pLocationOrderEntry[2]->end());
		break;
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		return search type of given item type
	@author     humkyung
    @date       2015-01-06
    @return     CGenerationOption::eSearchType
    @param      const STRING_T& sItemType
******************************************************************************/
CGenerationOption::eSearchType CGenerationOption::GetSearchTypeOf(const STRING_T& sItemType) const
{
	CGenerationOption::eSearchType res = CGenerationOption::SEARCH_AROUND;
	if(m_pGenerationItemMap->end() != m_pGenerationItemMap->find(sItemType))
	{
		res = (*m_pGenerationItemMap)[sItemType]->eType;
	}

	return res;
}