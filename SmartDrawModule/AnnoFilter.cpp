#include "stdafx.h"
#include <Tokenizer.h>
#include "AnnoFilter.h"
#include "aDraw_Entity.h"

CAnnoFilter::CAnnoFilter(const int& iSeq,const STRING_T& sProp,const STRING_T& sOper,const STRING_T& sValue)
{
	m_iSeq = iSeq;
	m_psProperty = new STRING_T(sProp);
	m_psOperator = new STRING_T(sOper);
	m_psValue = new STRING_T(sValue);
}

CAnnoFilter::~CAnnoFilter(void)
{
	try
	{
		SAFE_DELETE(m_psProperty);
		SAFE_DELETE(m_psOperator);
		SAFE_DELETE(m_psValue);
	}
	catch(...)
	{
	}
}

CAnnoFilter::CAnnoFilter(const CAnnoFilter& rhs)
{
	m_iSeq = -1;
	m_psProperty = new STRING_T;
	m_psOperator = new STRING_T;
	m_psValue = new STRING_T;

	(*this) = rhs;
}

CAnnoFilter& CAnnoFilter::operator =(const CAnnoFilter& rhs)
{
	if(this != &rhs)
	{
		m_iSeq = rhs.m_iSeq;
		m_psProperty->assign(rhs.m_psProperty->c_str());
		m_psOperator->assign(rhs.m_psOperator->c_str());
		m_psValue->assign(rhs.m_psValue->c_str());
	}

	return (*this);
}

int CAnnoFilter::GetSeq() const
{
	return m_iSeq;
}

int CAnnoFilter::SetSeq(const int& iSeq)
{
	m_iSeq = iSeq;
	return ERROR_SUCCESS;
}

LPCTSTR CAnnoFilter::GetProperty() const
{
	return m_psProperty->c_str();
}

int CAnnoFilter::SetProperty(const STRING_T& sProperty)
{
	m_psProperty->assign(sProperty);
	return ERROR_SUCCESS;
}

LPCTSTR CAnnoFilter::GetOperator() const
{
	return m_psOperator->c_str();
}

int CAnnoFilter::SetOperator(const STRING_T& sOperator)
{
	m_psOperator->assign(sOperator);
	return ERROR_SUCCESS;
}

LPCTSTR CAnnoFilter::GetValue() const
{
	return m_psValue->c_str();
}

int CAnnoFilter::SetValue(const STRING_T& sValue)
{
	m_psValue->assign(sValue);
	return ERROR_SUCCESS;
}

/**
	@brief	evaluate given value
	@author	humkyung
	@date	2016.03.19
*/
bool CAnnoFilter::Evaluate(const STRING_T& sPropValue) const
{
	if(_T("Const") == *m_psOperator)
	{
		return (_T("True") == *m_psValue);
	}
	else if(_T("Equals") == *m_psOperator)
	{
		return (sPropValue == *m_psValue);
	}
	else if(_T("Not Equals") == *m_psOperator)
	{
		return !(sPropValue == *m_psValue);
	}
	else if(_T("Contains") == *m_psOperator)
	{
		return (STRING_T::npos != sPropValue.find(*m_psValue));
	}
	else if(_T("Not Contains") == *m_psOperator)
	{
		return (STRING_T::npos == sPropValue.find(*m_psValue));
	}
	else if(_T("Begins with") == *m_psOperator)
	{
		return (0 == sPropValue.find(*m_psValue));
	}
	else if(_T("Ends with") == *m_psOperator)
	{
		STRING_T::size_type at = sPropValue.find(*m_psValue);
		return (at == (sPropValue.length() - m_psValue->length()));
	}

	return false;
}

/**
	@brief	evaluate given value
	@author	humkyung
	@date	2016.03.19
*/
bool CAnnoFilter::Evaluate(const double& dPropValue) const
{
	vector<STRING_T> oResult;
	CTokenizer<CIsFromString>::Tokenize(oResult , *m_psValue , CIsFromString(_T("-")));
	if(oResult.size() >= 2)
	{
		const double dCriticalSize = ATOF_T(oResult[oResult.size() - 1].c_str());
		if(_T("Equals") == *m_psOperator)
		{
			return (dCriticalSize == dPropValue);
		}
		else if(_T("Over") == *m_psOperator)
		{
			return (dPropValue > dCriticalSize); 
		}
		else if(_T("Less") == *m_psOperator)
		{
			return (dPropValue < dCriticalSize); 
		}
	}

	return false;
}

/**
	@brief	evaluate given value
	@author	humkyung
	@date	2016.03.19
*/
bool CAnnoFilter::Evaluate(const bool& bPropValue) const
{
	const bool bValue = (_T("True") == *m_psValue);
	return (bPropValue == bValue);
}

/**
	@brief	return clone of this
	@author	humkyung
	@date	2016.03.19
*/
CAnnoFilter* CAnnoFilter::Clone() const
{
	CAnnoFilter* res = new CAnnoFilter(*this);
	return res;
}

/**
	@brief	return string of filter
	@author	humkyung
	@date	2016.03.17
*/
STRING_T CAnnoFilter::ToString() const
{
	OSTRINGSTREAM_T oss;
	oss << (*m_psProperty) << _T(" ") << (*m_psOperator) << _T(" ") << (*m_psValue);
	return oss.str();
}

/**
	@brief	return sql for filter
	@author	humkyung
	@date	2016.03.17
*/
LPCTSTR CAnnoFilter::ToSql(LPCTSTR psSetting,LPCTSTR psItem) const
{
	OSTRINGSTREAM_T oss;
	oss << _T("insert into AnnoFilters(Seq,Setting,Item,Property,Operator,Value) values(") << m_iSeq << _T("'") << psSetting << _T("','") <<
		psItem << _T("','") << (*m_psProperty) << _T("','") << (*m_psOperator) << _T("','") << (*m_psValue) << _T("')");
	return oss.str().c_str();
}
