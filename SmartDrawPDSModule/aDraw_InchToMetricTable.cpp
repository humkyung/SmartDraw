#include "StdAfx.h"
#include <IsTools.h>
#include "aDraw_InchToMetricTable.h"
#include <Tokenizer.h>

#include <string>
#include <fstream>
using namespace std;

CaDraw_InchToMetricTable::CaDraw_InchToMetricTable(void)
{
	CString rTblFilePath = GetExecPath() + _T("\\Setting\\Inch_To_Metric.TBL");
	
	IFSTREAM_T ifile(rTblFilePath.operator LPCTSTR());
	if(ifile.is_open())
	{
		RangeValue* pRangeValue = NULL;

		STRING_T aLine;
		while(!ifile.eof())
		{
			getline(ifile , aLine);
			if(ifile.eof()) break;
			if(aLine.empty() || ('#' == aLine[0])) continue;	//! skip empty or comment line.

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

					m_RangeValueList.push_back(pRangeValue);
					pRangeValue = NULL;
				}
			}
		}
		ifile.close();

		if(NULL != pRangeValue) SAFE_DELETE(pRangeValue);
	}
}

CaDraw_InchToMetricTable::~CaDraw_InchToMetricTable(void)
{
	try
	{
		for(list<RangeValue*>::iterator itr = m_RangeValueList.begin();itr != m_RangeValueList.end();++itr)
		{
			SAFE_DELETE(*itr);
		}
	}
	catch(...)
	{
	}
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
CaDraw_InchToMetricTable& CaDraw_InchToMetricTable::GetInstance(void)
{
	static CaDraw_InchToMetricTable __INSTANCE__;

	return __INSTANCE__;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05
*/
CaDraw_InchToMetricTable::RangeValue* CaDraw_InchToMetricTable::Find(const double& nInch)
{
	for(list<RangeValue*>::iterator itr = m_RangeValueList.begin();itr != m_RangeValueList.end();++itr)
	{
		if((nInch >= (*itr)->more_equal_than) && (nInch < (*itr)->less_than))
		{
			return (*itr);
		}
	}

	return NULL;
}
