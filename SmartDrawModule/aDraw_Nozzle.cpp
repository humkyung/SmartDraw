#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include "aDraw_Nozzle.h"

ADRAW_IMPLEMENT_FUNC(CaDraw_Nozzle , CaDraw_Entity , _T("Nozzle"))

CaDraw_Nozzle::CaDraw_Nozzle(void) : /*m_pEqui(NULL) , */m_nNominalPipeDia(0.)
{
	m_pName = new STRING_T;
	m_pEquiName = new STRING_T;

	m_pRating = new STRING_T;
	m_pNozzleCode = new STRING_T;
	m_pNozzleType = new STRING_T;
	m_pOrientation = new STRING_T;
	m_pProjection  = new STRING_T;
}

CaDraw_Nozzle::~CaDraw_Nozzle(void)
{
	try
	{
		if(m_pName) SAFE_DELETE(m_pName);
		if(m_pEquiName) SAFE_DELETE(m_pEquiName);
		if(m_pRating) SAFE_DELETE(m_pRating);
		if(m_pNozzleCode) SAFE_DELETE(m_pNozzleCode);
		if(m_pNozzleType) SAFE_DELETE(m_pNozzleType);
		if(m_pOrientation) SAFE_DELETE(m_pOrientation);
		if(m_pProjection) SAFE_DELETE(m_pProjection);
	}
	catch(...)
	{
	}
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.05

	@param	

	@return
*/
void CaDraw_Nozzle::Copy(CaDraw_Nozzle* unnamed)
{
	assert(unnamed && "unnamed is NULL");
	CaDraw_Entity::Copy(unnamed);

	if(unnamed)
	{
		m_ptOrigin = unnamed->m_ptOrigin;
		*(m_pEquiName) = *(unnamed->m_pEquiName);
		m_nNominalPipeDia = unnamed->m_nNominalPipeDia;
		*(m_pRating) = *(unnamed->m_pRating);
		*(m_pNozzleCode) = *(unnamed->m_pNozzleCode);
		*(m_pNozzleType) = *(unnamed->m_pNozzleType);
		*(m_pOrientation) = *(unnamed->m_pOrientation);
		*(m_pProjection) = *(unnamed->m_pProjection);

		//m_pEqui = unnamed->m_pEqui;
	}
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return
*/	
const TCHAR* CaDraw_Nozzle::equi_name() const
{
	return m_pEquiName->c_str();
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.03

	@param	

	@return
*/
int CaDraw_Nozzle::SetEquiName(const STRING_T& rName)
{
	*m_pEquiName = rName;
	return ERROR_SUCCESS;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return
*/
double CaDraw_Nozzle::nominal_pipe_dia() const
{
	return m_nNominalPipeDia;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return
*/
int CaDraw_Nozzle::SetNominalPipeDia(const double& nominal_pipe_dia)
{
	m_nNominalPipeDia = nominal_pipe_dia;
	return ERROR_SUCCESS;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return
*/
const TCHAR* CaDraw_Nozzle::rating() const
{
	return m_pRating->c_str();
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.03

	@param	

	@return
*/
int CaDraw_Nozzle::SetRating(const STRING_T& rating)
{
	*(m_pRating) = rating;
	return ERROR_SUCCESS;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.03

	@param	

	@return
*/
const TCHAR* CaDraw_Nozzle::projection() const
{
	return m_pProjection->c_str();
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return
*/
int CaDraw_Nozzle::SetProjection(const STRING_T& projection)
{
	///#110:convert to double and then round
	const double dProj = SAFE_ROUND(ATOF_T(projection.c_str()),1);
	OSTRINGSTREAM_T oss;
	oss << dProj;
	/// up to here

	(*m_pProjection) = oss.str();

	return ERROR_SUCCESS;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return
*/
const TCHAR* CaDraw_Nozzle::orientation() const
{
	return m_pOrientation->c_str();
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return
*/
int CaDraw_Nozzle::SetOrientation(const STRING_T& orientation)
{
	///#110:convert to double and then round
	const double dOri = SAFE_ROUND(ATOF_T(orientation.c_str()),1);
	OSTRINGSTREAM_T oss;
	oss << dOri;
	/// up to here

	*(m_pOrientation) = oss.str();
	return ERROR_SUCCESS;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return
*/
const TCHAR* CaDraw_Nozzle::nozzle_type() const
{
	return m_pNozzleType->c_str();
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.03

	@param	

	@return
*/
int CaDraw_Nozzle::SetNozzleType(const STRING_T& nozzle_type)
{
	(*m_pNozzleType) = nozzle_type;
	return ERROR_SUCCESS;
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.03

	@param	

	@return
*/
const TCHAR* CaDraw_Nozzle::nozzle_code() const
{
	return m_pNozzleCode->c_str();
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.03

	@param	

	@return
*/
int CaDraw_Nozzle::SetNozzleCode(const STRING_T& nozzle_code)
{
	(*m_pNozzleCode) = nozzle_code;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @brief		report nozzle data
	@author     humkyung
    @date       2016-02-24
    @return     int
******************************************************************************/
int CaDraw_Nozzle::Report(soci::session& oSession,const STRING_T& sViewGuid) const
{
	OSTRINGSTREAM_T oss;
	oss << _T("insert into ") << CaDraw_Nozzle::TypeString() << _T(" values('") << sViewGuid << _T("','");
	oss << (*m_pName) << _T("','") << (*m_pEquiName) << _T("','") << (*m_pRating) << _T("','");
	oss << (*m_pNozzleCode) << _T("','") << (*m_pNozzleType) << _T("','") << (*m_pOrientation) << _T("','");
	oss << (*m_pProjection) << _T("','") << m_nNominalPipeDia << _T("')");
	oSession << oss.str();

	return ERROR_SUCCESS;
}

/**
	@brief	determine this is enable by checking filters
	@author	humkyung
	@date	2016.03.19
*/
bool CaDraw_Nozzle::IsEnable(AnnoFilters* pAnnoFilters) const
{
	assert((NULL != pAnnoFilters) && "pAnnoFilters is null");

	if(NULL != pAnnoFilters)
	{
		for(AnnoFilters::const_iterator itr = pAnnoFilters->begin();itr != pAnnoFilters->end();++itr)
		{
			const STRING_T sProp = (*itr)->GetProperty();
			if(_T("Name") == sProp)
			{
				return (*itr)->Evaluate(STRING_T(name()));
			}
			else if(_T("None") == sProp)
			{
				return (*itr)->Evaluate(STRING_T());
			}
		}
	}

	return true;
}