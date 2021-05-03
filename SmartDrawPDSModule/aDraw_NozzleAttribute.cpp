#include "StdAfx.h"
#include "aDraw_NozzleAttribute.h"
#include "aDraw_InchToMetricTable.h"

CaDraw_NozzleAttribute::CaDraw_NozzleAttribute(void) : m_nominal_piping_dia(0)
{
}

CaDraw_NozzleAttribute::~CaDraw_NozzleAttribute(void)
{
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.04
*/
int CaDraw_NozzleAttribute::GetAttribute(CADODB& adoDB , const int& nPartitionNo , const int& nozzle_occur , const int& equi_occur)
{
	CString rSql;
	rSql.Format(_T("SELECT nominal_piping_dia,rating,preparation,table_suffix FROM PDTABLE_22_%d WHERE nozzle_indx_no='%d'") , nPartitionNo , nozzle_occur);
	adoDB.OpenQuery(rSql.operator LPCTSTR());
	LONG lRecordCount = 0L;
	adoDB.GetRecordCount(&lRecordCount);
	if(1 == lRecordCount)
	{
		STRING_T strValue , rRatingValue , rPreparation , rTableSuffix;
		adoDB.GetFieldValue(0 , _T("nominal_piping_dia") , &strValue);
		m_nominal_piping_dia = ATOF_T(strValue.c_str());
		adoDB.GetFieldValue(0 , _T("rating") , &rRatingValue);
		adoDB.GetFieldValue(0 , _T("preparation") , &rPreparation);
		m_rating = GetRatingString(rRatingValue , rPreparation);
		adoDB.GetFieldValue(0 , _T("table_suffix") , &rTableSuffix);
		m_nozzle_code = GetNozzleCode(rTableSuffix);
			
		if(m_nominal_piping_dia > 5000)
		{
			m_nominal_piping_dia = ((m_nominal_piping_dia - 5000) / 32);
			
			CaDraw_InchToMetricTable& InchToMetricTable = CaDraw_InchToMetricTable::GetInstance();
			CaDraw_InchToMetricTable::RangeValue* pRangeValue = InchToMetricTable.Find(m_nominal_piping_dia);
			if(pRangeValue)
			{
				m_nominal_piping_dia = pRangeValue->value;
			}
			else
			{
				m_nominal_piping_dia *= 25.4;
			}
		}
	}

	rSql.Format(_T("SELECT nozzle_type,orientation_1,dimension_1,dimension_2,dimension_3 FROM PDTABLE_23_%d WHERE nozzle_indx_no='%d' AND equip_indx_no='%d'") , nPartitionNo , nozzle_occur , equi_occur);
	adoDB.OpenQuery(rSql.operator LPCTSTR());
	adoDB.GetRecordCount(&lRecordCount);
	if(1 == lRecordCount)
	{
		STRING_T strValue , orientation_1 , dimension_1 , dimension_2 , dimension_3;
		adoDB.GetFieldValue(0 , _T("nozzle_type") , &strValue);
		m_nozzle_type = strValue;
		adoDB.GetFieldValue(0 , _T("orientation_1") , &orientation_1);
		adoDB.GetFieldValue(0 , _T("dimension_1") , &dimension_1);
		adoDB.GetFieldValue(0 , _T("dimension_2") , &dimension_2);
		adoDB.GetFieldValue(0 , _T("dimension_3") , &dimension_3);
    
		if(_T("-") != m_nozzle_type)
		{
			const int nNozzleType = ATOI_T(m_nozzle_type.c_str());
			switch(nNozzleType)
			{
				case 1:
					m_orientation = orientation_1;
					IsString::TrimWhiteSpace(m_orientation);
					m_projection = dimension_2;
					IsString::TrimWhiteSpace(m_projection);
					break;
				case 2: case 3:
					m_orientation = orientation_1;
					IsString::TrimWhiteSpace(m_orientation);
					m_projection = _T("-");
					break;
				case 4: case 5: case 8:
					m_orientation = orientation_1;
					IsString::TrimWhiteSpace(m_orientation);
					m_projection = dimension_2;
					IsString::TrimWhiteSpace(m_projection);
					break;
				case 6: case 7:
					m_orientation = orientation_1;
					IsString::TrimWhiteSpace(m_orientation);
					m_projection = dimension_3;
					IsString::TrimWhiteSpace(m_projection);
					break;
				default:
					m_orientation = _T("-");
					m_projection = _T("-");
					break;
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.04
*/
STRING_T CaDraw_NozzleAttribute::GetRatingString(const STRING_T& rRating, const STRING_T& rPreparation)
{
	STRING_T res;
	CString rIniFilePath = GetExecPath() + _T("\\Setting\\NozzleChart.ini");
	
	STRING_T rRatingPrefix , rRatingSuffix;
	TCHAR szBuf[MAX_PATH] = {'\0',};
	///PREFIX & SUFFIX
	if(GetPrivateProfileString(_T("RATING_DISPLAY") , _T("Prefix") , NULL , szBuf , MAX_PATH , rIniFilePath))
	{
		rRatingPrefix.assign(szBuf);
	}
	if(GetPrivateProfileString(_T("RATING_DISPLAY") , _T("Suffix") , NULL , szBuf , MAX_PATH , rIniFilePath))
	{
		rRatingSuffix.assign(szBuf);
	}
    
	STRING_T rRatingStr;
	///PREPARATION_TABLE
	if(GetPrivateProfileString(_T("PREPARATION_TABLE") , rPreparation.c_str() , NULL , szBuf , MAX_PATH , rIniFilePath))
	{
		rRatingStr.assign(szBuf);
	}
    
	if(rRating.empty())
	{
		res = _T("-");
	}
	else
	{
		if(rRatingPrefix.empty())
		{
			res = rRating + rRatingSuffix;
		}
		else
		{
			res = rRatingPrefix + _T(" ") + rRating + rRatingSuffix;
		}

		if(!rRatingStr.empty())
		{
			res = rRatingPrefix + _T(" ") + rRating + rRatingSuffix + _T(" ") + rRatingStr;
		}
	}
    
	return res;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.04
*/
STRING_T CaDraw_NozzleAttribute::GetNozzleCode(const STRING_T& rTableSuffix)
{
	STRING_T res;
	CString rIniFilePath = GetExecPath() + _T("\\Setting\\NozzleChart.ini");
    
	TCHAR szBuf[MAX_PATH] = {'\0',};
	///Code
	if(GetPrivateProfileString(_T("TABLE_SUFFIX_TABLE") , rTableSuffix.c_str() , NULL , szBuf , MAX_PATH , rIniFilePath))
	{
		res.assign(szBuf);
	}else	res = _T("-");

	return res;
}
