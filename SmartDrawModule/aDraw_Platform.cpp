#include "StdAfx.h"
#include <assert.h>
#include <tchar.h>
#include <IsTools.h>
#include <IsString.h>

#include "aDraw_View.h"
#include "aDraw_Platform.h"

#include <fstream>
using namespace std;

ADRAW_IMPLEMENT_FUNC(CaDraw_Platform , CaDraw_Entity , _T("Platform"))

CaDraw_Platform::CaDraw_Platform(void)
{
	m_nHeight = 0.0;
	m_pPointList = new vector<CIsPoint3d>;
}

CaDraw_Platform::~CaDraw_Platform(void)
{
	try
	{
		if(m_pPointList) SAFE_DELETE(m_pPointList);
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
void CaDraw_Platform::Copy(CaDraw_Platform* unnamed)
{
	assert(unnamed && "unnamed is NULL");
	CaDraw_Entity::Copy(unnamed);

	if(unnamed)
	{
		m_pPointList->clear();
		m_pPointList->insert(m_pPointList->begin() , unnamed->m_pPointList->begin() , unnamed->m_pPointList->end());
		m_vecDir  = unnamed->m_vecDir;
		m_nHeight = unnamed->m_nHeight;
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-25
    @function   GetPointCount
    @return     int
    @brief
******************************************************************************/
int CaDraw_Platform::GetPointCount() const
{
	return m_pPointList->size();
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.06
*/
CIsPoint3d CaDraw_Platform::point(const int& at) const
{
	assert((at < int(m_pPointList->size())) && "range error");

	if((at >= 0) && at < int(m_pPointList->size())) return (*m_pPointList)[at];

	throw exception("range error");
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.06
*/
int CaDraw_Platform::Add(const CIsPoint3d& pt)
{
	m_pPointList->push_back(pt);
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.06
*/
CIsVect3d CaDraw_Platform::dir() const
{
	return m_vecDir;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.06
*/
double CaDraw_Platform::height() const
{
	return m_nHeight;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.06
*/
int CaDraw_Platform::SetDir(const CIsVect3d& dir)
{
	m_vecDir = dir;
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.06
*/
int CaDraw_Platform::SetHeight(const double& height)
{
	m_nHeight = height;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-15
    @function   Dump
    @return     int
    @param      OFSTREAM_T& ofile
    @brief
******************************************************************************/
int CaDraw_Platform::Dump(OFSTREAM_T& ofile)
{
	static long index = 1L;

	for(vector<CIsPoint3d>::iterator itr = m_pPointList->begin();itr != m_pPointList->end();++itr)
	{
		ofile << (index) << _T(",PLATFORM,") << m_pView->name() << _T(",") << (index) << _T(",");
		CIsPoint3d pt = m_pView->ModelCoord2ViewCoord(*itr);
		ofile << IsString::TrimedTrailingZero(pt.x()) << _T(",") << IsString::TrimedTrailingZero(pt.y()) << _T(",") << IsString::TrimedTrailingZero(itr->z()) << std::endl;
	}
	++index;

	return ERROR_SUCCESS;
}

/**
	@brief	determine this is enable by checking filters
	@author	humkyung
	@date	2016.03.19
*/
bool CaDraw_Platform::IsEnable(AnnoFilters* pAnnoFilters) const
{
	assert((NULL != pAnnoFilters) && "pAnnoFilters is null");

	if(NULL != pAnnoFilters)
	{
		for(AnnoFilters::const_iterator itr = pAnnoFilters->begin();itr != pAnnoFilters->end();++itr)
		{
			const STRING_T sProp = (*itr)->GetProperty();
			if(_T("None") == sProp)
			{
				if(false == (*itr)->Evaluate(STRING_T())) return false;
			}
		}
	}

	return true;
}