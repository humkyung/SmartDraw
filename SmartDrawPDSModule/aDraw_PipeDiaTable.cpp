#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include "aDraw_PipeDiaTable.h"
#include <Tokenizer.h>

#include <string>
#include <fstream>
using namespace std;

CaDraw_PipeDiaTable::CaDraw_PipeDiaTable(void)
{
	m_pRangeValueList = new list<RangeValue*>;

	/*CString rTblFilePath = GetExecPath() + _T("\\Setting\\PipeDiameter.TBL");
	
	IFSTREAM_T ifile(rTblFilePath.operator LPCTSTR());
	if(ifile.is_open())
	{
		RangeValue* pRangeValue = NULL;

		STRING_T aLine;
		while(!ifile.eof())
		{
			getline(ifile , aLine);
			if(ifile.eof()) break;
			if(aLine.empty() || ('#' == aLine[0])) continue;	/// skip empty or comment line.

			vector<STRING_T> oResult;
			CTokenizer<CIsFromString>::Tokenize(oResult , aLine , CIsFromString(_T("=")));
			if(2 == oResult.size())
			{
				if(NULL == pRangeValue) pRangeValue = new RangeValue;
				pRangeValue->value = ATOF_T(oResult[1].c_str());
				
				STRING_T aTemp(oResult[0]);
				CTokenizer<CIsFromString>::Tokenize(oResult , aTemp , CIsFromString(_T(":")));
				if(2 == oResult.size())
				{
					pRangeValue->more_equal_than = ATOF_T(oResult[0].c_str());
					pRangeValue->less_than = ATOF_T(oResult[1].c_str());

					m_pRangeValueList->push_back(pRangeValue);
					pRangeValue = NULL;
				}
			}
		}
		ifile.close();

		if(NULL != pRangeValue) SAFE_DELETE(pRangeValue);
	}*/
}

CaDraw_PipeDiaTable::~CaDraw_PipeDiaTable(void)
{
	try
	{
		Reset();
		SAFE_DELETE(m_pRangeValueList);
	}
	catch(...)
	{
	}
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2014.08.27
*/
CaDraw_PipeDiaTable& CaDraw_PipeDiaTable::GetInstance(void)
{
	static CaDraw_PipeDiaTable __INSTANCE__;

	return __INSTANCE__;
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2014.08.27
*/
CaDraw_PipeDiaTable::RangeValue* CaDraw_PipeDiaTable::Find(const double& nInch) const
{
	for(list<RangeValue*>::const_iterator itr = m_pRangeValueList->begin();itr != m_pRangeValueList->end();++itr)
	{
		if((nInch >= (*itr)->more_equal_than) && (nInch < (*itr)->less_than))
		{
			return (*itr);
		}
	}

	return NULL;
}

/**
	@brief	reset contents
	@author	humkyung
	@date	2015.07.17
*/
int CaDraw_PipeDiaTable::Reset()
{
	for(list<RangeValue*>::iterator itr = m_pRangeValueList->begin();itr != m_pRangeValueList->end();++itr)
	{
		SAFE_DELETE(*itr);
	}
	m_pRangeValueList->clear();

	return ERROR_SUCCESS;
}

/**
	@brief	add a pipe dia. data
	@author	humkyung
	@date	2015.07.17
*/
int CaDraw_PipeDiaTable::Add(const double& more_equal_than , const double& less_than , const double& value)
{
	assert((more_equal_than <= less_than) && "invalid parameter");

	if(more_equal_than <= less_than)
	{
		RangeValue* pRangeValue = new RangeValue;
		if(NULL != pRangeValue)
		{
			pRangeValue->more_equal_than = more_equal_than;
			pRangeValue->less_than = less_than;
			pRangeValue->value = value;
			m_pRangeValueList->push_back(pRangeValue);

			return ERROR_SUCCESS;
		}

		return ERROR_NOT_ENOUGH_MEMORY;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	save pipe dia. data to file
	@author	humkyung
	@date	2015.07.17
*/
int CaDraw_PipeDiaTable::Save()
{
	CString sTblFilePath = GetExecPath() + _T("\\Setting\\PipeDiameter.TBL");
	
	OFSTREAM_T ofile(sTblFilePath.operator LPCTSTR());
	if(ofile.is_open())
	{
		for(list<RangeValue*>::const_iterator itr = m_pRangeValueList->begin();itr != m_pRangeValueList->end();++itr)
		{
			ofile << (*itr)->more_equal_than << _T(":") << (*itr)->less_than << _T("=") << (*itr)->value << std::endl;
		}
		ofile.close();

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}