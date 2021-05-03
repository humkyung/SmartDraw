#include "StdAfx.h"
#include "StringTable.h"
#include <util/FileTools.h>

CStringTable::CStringTable(void)
{
}

/**
	@brief	destructor
*/
CStringTable::~CStringTable(void)
{
}

/**
	@brief	return the instance of string table
	@author	humkyung
	@date	2014.06.02
	@return	CStringTable&
*/
CStringTable& CStringTable::GetInstance()
{
	static CStringTable __instance__;

	return (__instance__);
}

/**
	@brief	get string value correspond to given name
	@author	humkyung
	@date	2014.06.02
	@return	string value
*/
STRING_T CStringTable::GetString(const STRING_T& sName)
{
	map<STRING_T,STRING_T>::const_iterator where = m_oStringValueMap.find(sName);
	if(m_oStringValueMap.end() != where) return (where->second);

	throw std::exception("can't found given name");
}

/**
	@brief	parse given file
	@author	humkyung
	@date	2014.06.02
	@return	int
*/
int CStringTable::Parse(const STRING_T& sFilePath)
{
	if(TRUE == CFileTools::DoesFileExist(sFilePath.c_str()))
	{
		CMarkup xml;
		if(true == xml.Load(sFilePath))
		{
			const LCID lcid = GetUserDefaultLCID();
			m_oStringValueMap.clear();

			MCD_STR sLoadResult = xml.GetResult();
			if(true == xml.FindElem(MCD_T("STRINGTABLE")))
			{
				xml.IntoElem();
				while(true == xml.FindElem(MCD_T("STRING")))
				{
					ParseString(xml , lcid);
				}
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_FILE_NOT_FOUND;
}

/**
	@brief	parse string value
	@author	humkyung
	@date	2014.06.02
	@return	int
*/
int CStringTable::ParseString(CMarkup& xml,const LCID& lcid)
{
	STRING_T sName , sValue;

	int iCount = 0;
	while(1)
	{
		STRING_T sAttrName = xml.GetAttribName(iCount++);
		if ( sAttrName.empty() ) break;
		STRING_T sAttrValue = xml.GetAttrib( sAttrName.c_str() );

		if(0 == STRICMP_T(_T("Name") , sAttrName.c_str()))
		{
			sName = sAttrValue;
		}
		else if((1042 == lcid) && (0 == STRICMP_T(_T("Korean") , sAttrName.c_str())))
		{
			sValue = sAttrValue;
		}
		else if((1041 == lcid) && (0 == STRICMP_T(_T("Japanese") , sAttrName.c_str())))
		{
			sValue = sAttrValue;
		}
		else if(sValue.empty() && (0 == STRICMP_T(_T("English") , sAttrName.c_str())))
		{
			sValue = sAttrValue;
		}
	}

	if(sValue.empty()) sValue = sName;
	m_oStringValueMap.insert(make_pair(sName , sValue));

	return ERROR_SUCCESS;
}