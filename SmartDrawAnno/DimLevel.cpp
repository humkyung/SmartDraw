// DimLevel.cpp: implementation of the CDimLevel class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <assert.h>
#include <math.h>
#include "SmartDrawAnnoEnv.h"
#include "DimLevel.h"
#include "ExtDivision.h"
#include "SmartDrawAnnoPos.H"
#include "SmartDrawAnnoModule.h"

#include <vector>
#include <algorithm>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CDimLevel::CDimLevel(const DIM_LEVEL_E& nLevel)
{
	m_nLevel = nLevel;
	m_pvtTagPos = new vector<CaDraw_TagPos*>;
}

/**	\brief
*/
CDimLevel::~CDimLevel()
{
	try
	{
		if(NULL != m_pvtTagPos) delete m_pvtTagPos;
	}
	catch(...)
	{
	}
	/// don't delete tag item pointer;
}

/**	
	@brief	The _SortByX function

	@param	p1	a parameter of type CaDraw_TagPos*
	@param	p2	a parameter of type CaDraw_TagPos*

	@return	static bool	
*/
static bool _SortByX(CaDraw_TagPos* p1,CaDraw_TagPos* p2)
{
	assert(p1 && "p1 is NULL");
	assert(p2 && "p2 is NULL");

	if(p1 && p2)
	{
		if(p1->m_ptStart.x() < p2->m_ptStart.x()) return true;
	}

	return false;
}

/**	\brief	The _SortByY function

	\param	p1	a parameter of type CaDraw_TagPos*
	\param	p2	a parameter of type CaDraw_TagPos*

	\return	static bool	
*/
static bool _SortByY(CaDraw_TagPos* p1,CaDraw_TagPos* p2)
{
	assert(p1 && "p1 is NULL");
	assert(p2 && "p2 is NULL");

	if(p1 && p2)
	{
		if(p1->m_ptStart.y() < p2->m_ptStart.y()) return true;
	}

	return false;
}

/**	\brief	The CDimLevel::SetBaseLine function

	\param	lnBase	a parameter of type const LINE_T

	\return	void	
*/
void CDimLevel::SetBaseLine(const CIsLine3d& lnBase)
{
	m_lnBase = lnBase;
}

/**	
	@brief	The CDimLevel::SetLowerLine function

	@param	lnLower	a parameter of type const LINE_T

	@return	void	
*/
void CDimLevel::SetLowerLine(const CIsLine3d& lnLower)
{
	m_lnLower = lnLower;
}

/**	\brief	The += function

	\param	pTagPos	a parameter of type CaDraw_TagPos*

	\return	bool CDimLevel::operator	
*/
bool CDimLevel::operator +=(CaDraw_TagPos* pTagPos)
{
	assert(pTagPos && "pTagPos is NULL");

	if(pTagPos)
	{
		m_pvtTagPos->push_back(pTagPos);
		return true;
	}

	return false;
}

/**	\brief	The CDimLevel::GetItemCount function


	\return	const long	
*/
const long CDimLevel::GetItemCount()
{
	return m_pvtTagPos->size();
}

/**	
	@brief	The CDimLevel::Run function
	@author	humkyung
	@date	?
	@param	pDivision	a parameter of type CExtDivision*
	@return	void	
*/
void CDimLevel::Run(CExtDivision* pDivision)
{
	assert(pDivision && "pDivision is NULL");

	if(pDivision)
	{
		const double nTolerance = CaDraw_TagEnv::TOLERANCE;
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		if(!m_pvtTagPos->empty())
		{
			if(pDivision->IsHorizontal())
			{
				//! sort tag pos itmes accoring to it's x coordinate.
				stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByX);

				vector<CaDraw_TagPos*>::iterator itrTagPos;
				for(vector<CaDraw_TagPos*>::iterator itrTagPos=m_pvtTagPos->begin();(itrTagPos+1) != m_pvtTagPos->end();/*++itrTagPos*/)
				{
					//! remove tag item if distance to next item is less tolerance.
					double dx=(*itrTagPos)->m_ptStart.x() - (*(itrTagPos + 1))->m_ptStart.x();
					if(fabs(dx) < nTolerance)
					{
						itrTagPos = m_pvtTagPos->erase(itrTagPos);
					}
					else
					{
						++itrTagPos;
					}
				}

				int nPair=1;
				CIsPoint3d pt[2];
				if(false == env.m_extDimStyle.m_bViewLimitOn)
				{
					nPair = 0;
				}
				else
				{
					pt[0] = m_lnBase.start();
				}
				for(itrTagPos=m_pvtTagPos->begin();itrTagPos != m_pvtTagPos->end();++itrTagPos)
				{
					pt[nPair] = (*itrTagPos)->m_ptStart;
					nPair++;
					
					if(2 == nPair)
					{
						CaDraw_TagPos* pTagPos=CSmartDrawAnnoModule::GetInstance().new_tag_pos();
						if(pTagPos)
						{
							pTagPos->type      = DIM_TAG;
							pTagPos->m_ptStart.x() = pt[0].x();
							pTagPos->m_ptStart.y() = m_lnBase.start().y();
							pTagPos->m_ptEnd.x() = pt[1].x();
							pTagPos->m_ptEnd.y() = m_lnBase.start().y();
							pTagPos->pTITEM = CSmartDrawAnnoModule::GetInstance().new_titem_t();
							pTagPos->pTITEM->nLevel  = m_nLevel;

							pDivision->m_plstDimension->push_back(pTagPos);
						}
						nPair   = 1;
						pt[0] = pt[1];
					}
				}
				if((true == env.m_extDimStyle.m_bViewLimitOn) && (fabs(pt[0].x() - m_lnBase.end().x()) > nTolerance))
				{
					CaDraw_TagPos* pTagPos=CSmartDrawAnnoModule::GetInstance().new_tag_pos();
					if(pTagPos)
					{
						pTagPos->type      = DIM_TAG;
						pTagPos->m_ptStart.x() = pt[0].x();
						pTagPos->m_ptStart.y() = m_lnBase.start().y();
						pTagPos->m_ptEnd.x() = m_lnBase.end().x();
						pTagPos->m_ptEnd.y() = m_lnBase.start().y();
						pTagPos->pTITEM = CSmartDrawAnnoModule::GetInstance().new_titem_t();
						pTagPos->pTITEM->nLevel  = m_nLevel;
						
						pDivision->m_plstDimension->push_back(pTagPos);
					}
				}
			}
			else if(pDivision->IsVertical())
			{
				//! sort tag pos itmes accoring to it's y coordinate.
				stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByY);
				vector<CaDraw_TagPos*>::iterator itrTagPos;
				for(itrTagPos=m_pvtTagPos->begin();(itrTagPos+1) != m_pvtTagPos->end();/*++itrTagPos*/)
				{
					//! remove tag item if distance to next item is less tolerance.
					const double dy=(*itrTagPos)->m_ptStart.y() - (*(itrTagPos+1))->m_ptStart.y();
					if(fabs(dy) < nTolerance) 
					{
						itrTagPos = m_pvtTagPos->erase(itrTagPos);
					}
					else
					{
						++itrTagPos;
					}
				}
				
				int nPair=1;
				CIsPoint3d pt[2];
				if(false == env.m_extDimStyle.m_bViewLimitOn)
				{
					nPair = 0;
				}
				else
				{
					pt[0] = m_lnBase.start();
				}
				for(itrTagPos=m_pvtTagPos->begin();itrTagPos != m_pvtTagPos->end();++itrTagPos)
				{
					pt[nPair] = (*itrTagPos)->m_ptStart;
					nPair++;
					if(2 == nPair)
					{
						CaDraw_TagPos* pTagPos=CSmartDrawAnnoModule::GetInstance().new_tag_pos();
						if(pTagPos)
						{
							pTagPos->type      = DIM_TAG;
							pTagPos->m_ptStart.x() = m_lnBase.start().x();
							pTagPos->m_ptStart.y() = pt[0].y();
							pTagPos->m_ptEnd.x() = m_lnBase.start().x();
							pTagPos->m_ptEnd.y() = pt[1].y();
							pTagPos->pTITEM = CSmartDrawAnnoModule::GetInstance().new_titem_t();
							pTagPos->pTITEM->nLevel  = m_nLevel;

							pDivision->m_plstDimension->push_back(pTagPos);
						}

						nPair   = 1;
						pt[0] = pt[1];
					}
				}
				if((true == env.m_extDimStyle.m_bViewLimitOn) && (fabs(pt[0].y() - m_lnBase.end().y()) > nTolerance))
				{
					CaDraw_TagPos* pTagPos=CSmartDrawAnnoModule::GetInstance().new_tag_pos();
					if(pTagPos)
					{
						pTagPos->type      = DIM_TAG;
						pTagPos->m_ptStart.x() = m_lnBase.start().x();
						pTagPos->m_ptStart.y() = pt[0].y();
						pTagPos->m_ptEnd.x() = m_lnBase.end().x();
						pTagPos->m_ptEnd.y() = m_lnBase.end().y();
						pTagPos->pTITEM = CSmartDrawAnnoModule::GetInstance().new_titem_t();
						pTagPos->pTITEM->nLevel  = m_nLevel;
						
						pDivision->m_plstDimension->push_back(pTagPos);
					}
				}
			}
		}
	}
}

/**	
	@brief	The CDimLevel::StoreDITEM function
	@author	HumKyung.BAEK
	@date	?
	@param	pDivision	a parameter of type CExtDivision*
	@return	void	
*/
void CDimLevel::StoreDITEM(CExtDivision* pDivision)
{
	assert(pDivision && "pDivision is NULL");

	if(pDivision)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		switch(m_nLevel)
		{
			case DIM_LEVEL_0:	
			{
				m_lnBase = pDivision->m_lnBaseLine;
				m_lnBase[0].x() += pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA);
				m_lnBase[0].y() += pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA);
				m_lnBase[1].x()   += pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA);
				m_lnBase[1].y()   += pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA);
			}
				break;

			case DIM_LEVEL_1:
			{
				m_lnBase = pDivision->m_lnBaseLine;
				m_lnBase[0].x() += pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB);
				m_lnBase[0].y() += pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB);
				m_lnBase[1].x() += pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB);
				m_lnBase[1].y() += pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB);
			}
				break;

			case DIM_LEVEL_2:
			{
				m_lnBase = pDivision->m_lnBaseLine;
				m_lnBase[0].x() += pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC);
				m_lnBase[0].y() += pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC);
				m_lnBase[1].x() += pDivision->m_vecTagDir.dx()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC);
				m_lnBase[1].y() += pDivision->m_vecTagDir.dy()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC);
			}
				break;
		}

		for(vector<CaDraw_TagPos*>::iterator itr = pDivision->m_pvtTagPos->begin();itr != pDivision->m_pvtTagPos->end();++itr)
		{
			if(m_nLevel & (*itr)->nLevel) m_pvtTagPos->push_back(*itr);
		}
	}
}

/**	
	@brief	set dimension level
	@author	humkyung
	@date	2016.03.29
	@return	int
*/
int CDimLevel::SetLevel(const DIM_LEVEL_E& eLevel)
{
	m_nLevel = eLevel;
	return ERROR_SUCCESS;
}

DIM_LEVEL_E CDimLevel::GetLevel() const
{
	return m_nLevel;
}