// DimLevel0.cpp: implementation of the CDimLevel0 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <math.h>
#include <algorithm>
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoPos.H"
#include "DimLevel0.h"
#include "ExtDivision.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CDimLevel0::CDimLevel0(CExtDivision* pParent) : CDimLevel(DIM_LEVEL_0)
{
	m_pParent = pParent;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CDimLevel0::~CDimLevel0()
{

}

/**	
	@brief	The CDimLevel0::Run function

	@author	BHK

	@date	?

	@param	pDivision	a parameter of type CExtDivision*

	@return	void
*/
void CDimLevel0::Run(CExtDivision* pDivision)
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
				for(itrTagPos=m_pvtTagPos->begin();(itrTagPos+1) != m_pvtTagPos->end();)
				{
					//! remove tag item if distance to next item is less tolerance.
					const double dx=(*itrTagPos)->m_ptStart.x() - (*(itrTagPos + 1))->m_ptStart.x();
					if(fabs(dx) < nTolerance)
						itrTagPos = m_pvtTagPos->erase(itrTagPos);
					else	++itrTagPos;
				}
				
				int nPair=1;
				CIsPoint3d pt[2];
				DITEM_E enmType[2]={NONE,};
				char    nLevelOn[2]={0,};
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
					enmType[nPair] = (*itrTagPos)->pTITEM->enmType;
					nLevelOn[nPair]= (*itrTagPos)->nLevelOn;
					nPair++;
					
					if(2 == nPair)
					{
						//if((DRAFT_DIM == enmType[0]) || (DRAFT_DIM == enmType[1]) || (DRAFT_PIPE == enmType[0]) || (DRAFT_PIPE == enmType[1])){
						if((nLevelOn[0] & DIM_LEVEL_0_ON) || (nLevelOn[1] & DIM_LEVEL_0_ON))
						{
							CaDraw_TagPos* pTagPos=CSmartDrawAnnoModule::GetInstance().new_tag_pos();
							if(pTagPos){
								pTagPos->type      = DIM_TAG;
								pTagPos->pTITEM = CSmartDrawAnnoModule::GetInstance().new_titem_t();
								pTagPos->pTITEM->nLevel = m_nLevel;
								pTagPos->m_ptStart.x() = pt[0].x();
								pTagPos->m_ptStart.y() = m_lnBase.start().y();
								pTagPos->m_ptEnd.x() = pt[1].x();
								pTagPos->m_ptEnd.y() = m_lnBase.start().y();

								pDivision->m_plstDimension->push_back(pTagPos);
							}
						}
						nPair   = 1;
						pt[0] = pt[1];
						enmType[0] = enmType[1];
						nLevelOn[0]= nLevelOn[1];
					}
				}
				if((true == env.m_extDimStyle.m_bViewLimitOn) && (fabs(pt[0].x() - m_lnBase.end().x()) > nTolerance))
				{
					if((nLevelOn[0] & DIM_LEVEL_0_ON))
					{
						CaDraw_TagPos* pTagPos=CSmartDrawAnnoModule::GetInstance().new_tag_pos();
						if(pTagPos)
						{
							pTagPos->type = DIM_TAG;
							pTagPos->pTITEM = CSmartDrawAnnoModule::GetInstance().new_titem_t();
							pTagPos->pTITEM->nLevel = m_nLevel;
							pTagPos->m_ptStart.x() = pt[0].x();
							pTagPos->m_ptStart.y() = m_lnBase.start().y();
							pTagPos->m_ptEnd.x() = m_lnBase.end().x();
							pTagPos->m_ptEnd.y() = m_lnBase.start().y();
							
							pDivision->m_plstDimension->push_back(pTagPos);
						}
					}
				}
			}
			else if(pDivision->IsVertical())
			{
				//! sort tag pos itmes accoring to it's y coordinate.
				stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByY);
				vector<CaDraw_TagPos*>::iterator itrTagPos;
				for(itrTagPos=m_pvtTagPos->begin();(itrTagPos+1) != m_pvtTagPos->end();)
				{
					//! remove tag item if distance to next item is less tolerance.
					const double dy=(*itrTagPos)->m_ptStart.y() - (*(itrTagPos+1))->m_ptStart.y();
					if(fabs(dy) < nTolerance)
						itrTagPos = m_pvtTagPos->erase(itrTagPos);
					else	++itrTagPos;
				}
				
				int nPair=1;
				CIsPoint3d pt[2];
				DITEM_E enmType[2]={NONE,};
				char    nLevelOn[2]={0,};
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
					enmType[nPair] = (*itrTagPos)->pTITEM->enmType;
					nLevelOn[nPair]= (*itrTagPos)->nLevelOn;
					nPair++;
					if(2 == nPair)
					{
						if((nLevelOn[0] & DIM_LEVEL_0_ON) || (nLevelOn[1] & DIM_LEVEL_0_ON))
						{
							CaDraw_TagPos* pTagPos=CSmartDrawAnnoModule::GetInstance().new_tag_pos();
							if(pTagPos)
							{
								pTagPos->type = DIM_TAG;
								pTagPos->pTITEM = CSmartDrawAnnoModule::GetInstance().new_titem_t();
								pTagPos->pTITEM->nLevel = m_nLevel;
								pTagPos->m_ptStart.x() = m_lnBase.start().x();
								pTagPos->m_ptStart.y() = pt[0].y();
								pTagPos->m_ptEnd.x() = m_lnBase.start().x();
								pTagPos->m_ptEnd.y() = pt[1].y();

								pDivision->m_plstDimension->push_back(pTagPos);
							}
						}
						nPair   = 1;
						pt[0] = pt[1];
						enmType[0] = enmType[1];
						nLevelOn[0]= nLevelOn[1];
					}
				}
				if((true == env.m_extDimStyle.m_bViewLimitOn) && (fabs(pt[0].y() - m_lnBase.end().y()) > nTolerance))
				{
					if((nLevelOn[0] & DIM_LEVEL_0_ON))
					{
						CaDraw_TagPos* pTagPos=CSmartDrawAnnoModule::GetInstance().new_tag_pos();
						if(pTagPos)
						{
							pTagPos->type = DIM_TAG;
							pTagPos->pTITEM = CSmartDrawAnnoModule::GetInstance().new_titem_t();
							pTagPos->pTITEM->nLevel = m_nLevel;
							pTagPos->m_ptStart.x() = m_lnBase.start().x();
							pTagPos->m_ptStart.y() = pt[0].y();
							pTagPos->m_ptEnd.x() = m_lnBase.end().x();
							pTagPos->m_ptEnd.y() = m_lnBase.end().y();
							
							pDivision->m_plstDimension->push_back(pTagPos);
						}
					}
				}
			}
		}
	}
}
