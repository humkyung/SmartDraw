#include "StdAfx.h"
#include <assert.h>
#include <fstream>
#include <IsString.h>
#include "aDraw_Pipe.h"
#include "aDraw_View.h"
#include <IsTools.h>

ADRAW_IMPLEMENT_FUNC(CaDraw_Pipe , CaDraw_Entity , _T("Pipe"))

CaDraw_Pipe::CaDraw_Pipe(void) : m_nSize(-1.f) , m_dInsThick(0.0) , m_dInsThickInMetric(0.0)
{
	m_dNominalPipeSizeInMetric = -1.f;
	m_pAttributeMap = new map<STRING_T,STRING_T>;
}

CaDraw_Pipe::~CaDraw_Pipe(void)
{
	try
	{
		if(m_pAttributeMap) SAFE_DELETE(m_pAttributeMap);
	}
	catch(...)
	{
	}
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
void CaDraw_Pipe::Copy(CaDraw_Pipe* unnamed)
{
	assert(unnamed && "unnamed is NULL");
	CaDraw_Entity::Copy(unnamed);

	if(unnamed)
	{
		m_ptStart = unnamed->m_ptStart;
		m_ptEnd = unnamed->m_ptEnd;
		m_rNames[LINE_LABEL] = unnamed->m_rNames[LINE_LABEL];
		m_rNames[LINE_ID]    = unnamed->m_rNames[LINE_ID];
		m_rSpec  = unnamed->m_rSpec;
		m_nTagFlag = unnamed->m_nTagFlag;
		m_nSize = unnamed->m_nSize;
		m_dNominalPipeSize = unnamed->m_dNominalPipeSize;
		m_dNominalPipeSizeInMetric = unnamed->m_dNominalPipeSizeInMetric;
		m_dInsThick = unnamed->m_dInsThick;
		m_dInsThickInMetric = unnamed->m_dInsThickInMetric;

		m_pAttributeMap->clear();
		m_pAttributeMap->insert(unnamed->m_pAttributeMap->begin() , unnamed->m_pAttributeMap->end());
	}
}

int CaDraw_Pipe::SetAttribute(const STRING_T& rAttr , const STRING_T& rValue)
{
	if(m_pAttributeMap)
	{
		map<STRING_T,STRING_T>::iterator where = m_pAttributeMap->find(rAttr);
		if(where != m_pAttributeMap->end())
		{
			where->second = rValue;
		}
		else
		{
			m_pAttributeMap->insert(make_pair(rAttr , rValue));
		}
	}

	return ERROR_SUCCESS;
}

STRING_T CaDraw_Pipe::GetAttribute(const STRING_T& rAttr) const 
{
	if(m_pAttributeMap)
	{
		map<STRING_T,STRING_T>::const_iterator where = m_pAttributeMap->find(rAttr);
		if(where != m_pAttributeMap->end())
		{
			return (where->second);
		}
	}

	return STRING_T(_T(""));
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
CIsPoint3d CaDraw_Pipe::GetStartPoint()
{
	return m_ptStart;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
CIsPoint3d CaDraw_Pipe::GetEndPoint()
{
	return m_ptEnd;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
void CaDraw_Pipe::SetStartPoint(const CIsPoint3d& ptStart)
{
	m_ptStart = ptStart;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
void CaDraw_Pipe::SetEndPoint(const CIsPoint3d& ptEnd)
{
	m_ptEnd = ptEnd;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
STRING_T CaDraw_Pipe::line_label() const
{
	return m_rNames[LINE_LABEL];
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
STRING_T CaDraw_Pipe::line_id() const
{
	return m_rNames[LINE_ID];
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
STRING_T CaDraw_Pipe::GetSpec() const
{
	return m_rSpec;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
void CaDraw_Pipe::SetLineLabel(const STRING_T& line_label)
{
	m_rNames[LINE_LABEL] = line_label;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
void CaDraw_Pipe::SetLineId(const STRING_T& line_id)
{
	m_rNames[LINE_ID] = line_id;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
void CaDraw_Pipe::SetSpec(const STRING_T& rSpec)
{
	m_rSpec = rSpec;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
double CaDraw_Pipe::GetSize() const
{
	return m_nSize;
}

/******************************************************************************
	@brief
    @author     humkyung
    @date       2012-04-15
    @function   GetSizeInMetric
    @return     double
******************************************************************************/
double CaDraw_Pipe::GetSizeInMetric() const
{
	return m_nSize*25.4;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.26
	@param	
	@return
*/
double CaDraw_Pipe::GetRadius() const
{
	return m_nSize * 0.5;
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-15
    @function   GetRadiusInMetric
    @return     double
    @brief
******************************************************************************/
double CaDraw_Pipe::GetRadiusInMetric() const
{
	return (this->GetSizeInMetric()) * 0.5;
}

/**	
	@brief	
	@author	humkyung
	@date	2010.05.24
	@param	
	@return
*/
void CaDraw_Pipe::SetSize(const double& nSize)
{
	m_nSize = nSize;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
double CaDraw_Pipe::GetNominalPipeSize() const
{
	return m_dNominalPipeSize;
}

/**	
	@brief	
	@author	humkyung
	@date	2016.03.09
	@param	
	@return
*/
double CaDraw_Pipe::GetNominalPipeSizeInMetric() const
{
	return m_dNominalPipeSizeInMetric;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
void CaDraw_Pipe::SetNominalPipeSize(const double& dNominalPipeSize)
{
	m_dNominalPipeSize = dNominalPipeSize;
}

/**	
	@brief	
	@author	humkyung
	@date	2016.03.09
	@param	
	@return
*/
void CaDraw_Pipe::SetNominalPipeSizeInMetric(const double& dNominalPipeSizeInMetric)
{
	m_dNominalPipeSizeInMetric = dNominalPipeSizeInMetric;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.06
	@param	
	@return
*/
double  CaDraw_Pipe::insThick() const
{
	return m_dInsThick;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.06
	@param	
	@return
*/
int CaDraw_Pipe::SetInsThick(const double& insul)
{
	m_dInsThick = insul;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   insThickInMetric
    @return     ouble
    @brief
******************************************************************************/
double  CaDraw_Pipe::insThickInMetric() const
{
	return m_dInsThickInMetric;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   SetInsThick
    @return     int
    @param      const   double&
    @param      insul
    @brief
******************************************************************************/
int CaDraw_Pipe::SetInsThickInMetric(const double& insul)
{
	m_dInsThickInMetric = insul;
	return ERROR_SUCCESS;
}

/******************************************************************************
	@brief		write pipe data to file
	@author     humkyung
    @date       2014-11-20
    @return     int
******************************************************************************/
int CaDraw_Pipe::Report(soci::session& oSession,const STRING_T& sViewGuid) const
{
	OSTRINGSTREAM_T oss;
	oss << _T("insert into ") << CaDraw_Pipe::TypeString() << _T(" values('") << sViewGuid << _T("','") << line_id() << _T("','");
	oss << m_rNames[0] << _T("','") << m_rSpec << _T("',") << m_nSize << _T(")");
	oSession << oss.str();

	return ERROR_SUCCESS;
}

/**
	@brief	determine this is enable by checking filters
	@author	humkyung
	@date	2016.03.19
*/
bool CaDraw_Pipe::IsEnable(AnnoFilters* pAnnoFilters) const
{
	assert((NULL != pAnnoFilters) && "pAnnoFilters is null");

	if(NULL != pAnnoFilters)
	{
		for(AnnoFilters::const_iterator itr = pAnnoFilters->begin();itr != pAnnoFilters->end();++itr)
		{
			const STRING_T sProp = (*itr)->GetProperty();
			if(_T("Name") == sProp)
			{
				const STRING_T sPropValue = line_label();
				if(false == (*itr)->Evaluate(sPropValue)) return false;
			}
			else if(_T("Spec") == sProp)
			{
				if(false == (*itr)->Evaluate(m_rSpec)) return false;
			}
			else if(_T("Size") == sProp)
			{
				if(false == (*itr)->Evaluate(GetNominalPipeSizeInMetric())) return false;
			}
			else if(_T("None") == sProp)
			{
				if(false == (*itr)->Evaluate(STRING_T())) return false;
			}
		}
	}

	return true;
}