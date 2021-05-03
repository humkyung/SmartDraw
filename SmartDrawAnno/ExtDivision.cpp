// Division.cpp: implementation of the CExtDivision class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <assert.h>
#include <math.h>
#include <algorithm>
#include <tchar.h>
#include <IsLine2d.h>
#include <IsTools.h>
#include "SmartDrawAnnoPipe.h"
#include "ExtDivision.h"
#include "DimLevel0.h"
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoModule.h"
#include "ExtWriter.h"

#include "SmartDrawAnnoEqui.h"
#include "SmartDrawAnnoStru.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**	\brief	The _SortByX function

	\param	p1	a parameter of type CaDraw_TagPos*
	\param	p2	a parameter of type CaDraw_TagPos*

	\return	static bool	
*/
static bool _SortByX(CaDraw_TagPos* p1,CaDraw_TagPos* p2)
{
	assert(p1 && "p1 is NULL");
	assert(p2 && "p2 is NULL");

	if(p1 && p2)
	{
		CIsPoint3d pt[2];
		pt[0] = p1->m_ptEnd;
		pt[1] = p2->m_ptEnd;

		if(SINGLE_TAG == p1->type)
		{
			pt[0].x() = (pt[0].x() + p1->rect.height*0.5);
		}

		if(SINGLE_TAG == p2->type)
		{
			pt[1].x() = ( pt[1].x() + p2->rect.height*0.5);
		}
		if(pt[0].x() < pt[1].x()) return true;
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
		CIsPoint3d pt[2];
		pt[0] = p1->m_ptEnd;
		pt[1] = p2->m_ptEnd;

		if(SINGLE_TAG == p1->type)
		{
			pt[0].y() = ( pt[0].y() - p1->rect.height*0.5);
		}

		if(SINGLE_TAG == p2->type)
		{
			pt[1].y() = ( pt[1].y() - p2->rect.height*0.5);
		}

		if(pt[0].y() > pt[1].y()) return true;
	}

	return false;
}

/**	
	@brief	The _SortByName function
	@param	p1	a parameter of type CaDraw_TagPos*
	@param	p2	a parameter of type CaDraw_TagPos*
	@return	static bool	
*/
static bool _SortByName(CaDraw_TagPos* p1,CaDraw_TagPos* p2)
{
	assert(p1 && "p1 is NULL");
	assert(p2 && "p2 is NULL");

	if(p1 && p2)
	{
		static TCHAR szName[2][256 + 1]={'\0' ,};

		STRNCPY_T(szName[0],p1->pTITEM->name[0]->c_str() , 256);
		TCHAR* pFirstName=STRSTR_T(szName[0], _T("\\n"));
		if(pFirstName) (*pFirstName) = '\0';
		
		STRNCPY_T(szName[1],p2->pTITEM->name[0]->c_str() , 256);
		pFirstName=STRSTR_T(szName[1], _T("\\n"));
		if(pFirstName) (*pFirstName) = '\0';
	
		if(STRCMP_T(szName[0],szName[1]) < 0) return true;
	}

	return false;
}

/**	
	@brief	The _SortByLineId function

	@author	HumKyung.BAEK

	@param	p1	a parameter of type CaDraw_TagPos*
	@param	p2	a parameter of type CaDraw_TagPos*

	@return	bool	
*/
static bool _SortByLineId(CaDraw_TagPos* p1,CaDraw_TagPos* p2)
{
	assert(p1 && "p1 is NULL");
	assert(p2 && "p2 is NULL");

	if(p1 && p2)
	{
		if(STRCMP_T(p1->pTITEM->name[1]->c_str() , p2->pTITEM->name[1]->c_str()) < 0) return true;
	}

	return false;
}

/**	\brief	The _SortByType function

	\param	p1	a parameter of type CaDraw_TagPos*
	\param	p2	a parameter of type CaDraw_TagPos*

	\return	static bool	
*/
static bool _SortByType(CaDraw_TagPos* p1,CaDraw_TagPos* p2)
{
	assert(p1 && "p1 is NULL");
	assert(p2 && "p2 is NULL");
	bool bRet = true;

	if(p1 && p2)
	{
		if(VERTICAL == p1->pTITEM->enmSubType) bRet = false;
	}

	return bRet;
}

CExtDivision::CExtDivision(const CIsLine3d& lnBaseLine,bool bOpen[],const CIsVect3d& vecTagDir,CaDraw_View* pView)
{
	{
		try
		{
			m_bOpen[0] = bOpen[0];
			m_bOpen[1] = bOpen[1];
			m_bOver[0] = m_bOver[1] = false;
			memcpy(&m_vecTagDir,&vecTagDir,sizeof(CIsVect3d));
			memcpy(&m_lnBaseLine,&lnBaseLine,sizeof(CIsLine3d));
			
			m_lnUpperLine = m_lnBaseLine;
			
			m_pDrawView = pView;
			m_nInGroup    = 0;
			m_nGroupCount = 0;
			
			m_pvtTagPos    = new vector<CaDraw_TagPos*>;
			m_pvtBOP = new vector<CaDraw_TagPos*>;

			m_pvtPipeTagPos = new vector<CaDraw_TagPos*>;
			m_pvtEquiTagPos = new vector<CaDraw_TagPos*>;
			m_pvtStruTagPos = new vector<CaDraw_TagPos*>;
			m_pvtOtherTagPos= new vector<CaDraw_TagPos*>;
			m_pvtDimTagPos  = new vector<CaDraw_TagPos*>;

			m_plstDimension = new list<CaDraw_TagPos*>;
			m_plstLeader = new list<CDraftLeader*>;	/// 2013.06.16 added by humkyung
						
			m_pDimLevel0= NULL;
			m_pDimLevel1= NULL;
			m_pDimLevel2= NULL;

			m_plstGroupCount = new list<int>;
			
			m_szBuf[0]  = '\0';
			memset(m_szViewMatchLine,'\0',VIEW_MATCH_BUF_SIZ);
		}
		catch(...)
		{
		}
	}
}

/**	
	@brief

*/
CExtDivision::~CExtDivision()
{
	try
	{
		if(m_pvtTagPos) SAFE_DELETE(m_pvtTagPos);

		if(NULL != m_pvtPipeTagPos) SAFE_DELETE(m_pvtPipeTagPos);
		if(NULL != m_pvtEquiTagPos) SAFE_DELETE(m_pvtEquiTagPos);
		if(NULL != m_pvtStruTagPos) SAFE_DELETE(m_pvtStruTagPos);
		if(NULL != m_pvtOtherTagPos) SAFE_DELETE(m_pvtOtherTagPos);

		if(NULL != m_plstDimension) delete m_plstDimension;
		m_plstDimension = NULL;
				
		///if(m_plstTagDimension) delete m_plstTagDimension;
		///m_plstTagDimension = NULL;
		if(m_plstGroupCount) delete m_plstGroupCount;
		m_plstGroupCount = NULL;
		
		if(m_pDimLevel0) delete m_pDimLevel0;
		if(m_pDimLevel1) delete m_pDimLevel1;
		if(m_pDimLevel2) delete m_pDimLevel2;
	}
	catch(...)
	{
	}
}

/**	
	@brief	The CExtDivision::SetDivisionHeight function
	@param	nHeight	a parameter of type const double
	@return	void	
*/
void CExtDivision::SetDivisionHeight(const double& nLineNoHeight , const double& dOtherHeight)
{
	m_nHeight[0] = nLineNoHeight;
	m_nHeight[1] = dOtherHeight;
}

/**	
	@brief	The CExtDivision::IsMathLinePoint function
	@author	humkyung
	@date	?
	@param	pt	a parameter of type const CIsPoint3d
	\remarks\n
	;2004.12.17 - check division area
	@return	bool	
*/
bool CExtDivision::IsMathLinePoint(const CIsPoint3d& pt)
{
	{
		double dx = m_lnBaseLine.end().x() - m_lnBaseLine.start().x();
		double dy = m_lnBaseLine.end().y() - m_lnBaseLine.start().y();
		
		if(fabs(dx) < 0.001)
		{
			const double dy1 = m_lnBaseLine.start().y() - pt.y();
			const double dy2 = m_lnBaseLine.end().y() - pt.y();
			if((dy1*dy2 <= 0.f) && (fabs(m_lnBaseLine.start().x() - pt.x()) < 0.001)) return true;
		}
		else if(fabs(dy) < 0.001)
		{
			const double dx1 = m_lnBaseLine.start().x() - pt.x();
			const double dx2 = m_lnBaseLine.end().x() - pt.x();
			if((dx1*dx2 <= 0.f) && (fabs(m_lnBaseLine.start().y() - pt.y()) < 0.001)) return true;
		}
	}

	return false;
}

/**	
	@brief	The CExtDivision::IsInDivision function
	@author	humkyung
	@date	?
	@param	pt	a parameter of type const CIsPoint3d
	@param	iType	range type(0 = line no , 1 = others)
	@return	bool	
*/
bool CExtDivision::IsInDivision(const CIsPoint3d& pt , const CIsVect3d& dir , const int& iType)
{
	bool bRet = false;

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	{
		const double cross = dir.dx()*m_vecTagDir.dy() - dir.dy()*m_vecTagDir.dx();
		if(fabs(cross) < 0.001)
		{ // test is same dir?		
			CIsLine2d lnLowerLine(
				CIsPoint2d(m_lnBaseLine.start().x() , m_lnBaseLine.start().y()) , 
				CIsPoint2d(m_lnBaseLine.end().x() , m_lnBaseLine.end().y()));
			lnLowerLine[0] -= CIsPoint2d(
				m_vecTagDir.dx()*(env.m_nBaseLineOffset + m_nHeight[iType]) , 
				m_vecTagDir.dy()*(env.m_nBaseLineOffset + m_nHeight[iType]));
			lnLowerLine[1] -= CIsPoint2d(
				m_vecTagDir.dx()*(env.m_nBaseLineOffset + m_nHeight[iType]) ,
				m_vecTagDir.dy()*(env.m_nBaseLineOffset + m_nHeight[iType]));
			
			CIsPoint2d _pt(pt.x(),pt.y());
			double dx[2]={0,},dy[2]={0,};
			
			dx[0] = lnLowerLine.start().x() - pt.x();
			dx[1] = lnLowerLine.end().x() - pt.x();
			dy[0] = lnLowerLine.start().y() - pt.y();
			dy[1] = lnLowerLine.end().y() - pt.y();
			if(fabs(m_vecTagDir.dy()) > 0.001)
			{
				if(dx[0]*dx[1] > 0.) return false;
			}
			if(fabs(m_vecTagDir.dx()) > 0.001)
			{
				if(dy[0]*dy[1] > 0.) return false;
			}

			if(lnLowerLine.IsRightSide(_pt)) bRet = true;
		}
	}

	return bRet;
}

/**	
	@brief	add tag object.
	@author	humkyung
	@param	pTITEM	a parameter of type CaDraw_TagItem*
	@return	bool
*/
bool CExtDivision::Register(CaDraw_TagItem* pTagItem)
{
	assert(pTagItem && "pTagItem is NULL");
	bool bRet=false;

	if(pTagItem)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		vector<CaDraw_TagPos*> oTagPosList;
		const int res = pTagItem->OnCreateTagPos(oTagPosList , this);
		for(vector<CaDraw_TagPos*>::iterator itr = oTagPosList.begin();itr != oTagPosList.end();++itr)
		{
			CaDraw_TagPos *pTagPos = (*itr);

			if(DIM_TAG == (*itr)->type)
			{
				m_pvtDimTagPos->push_back(*itr);
				continue;
			}

			if(pTagItem->m_pEnt->IsKindOf(CaDraw_TagPipe::TypeString()))
			{
				m_pvtPipeTagPos->push_back(*itr);
			}
			else if(pTagItem->m_pEnt->IsKindOf(CaDraw_TagEqui::TypeString()))
			{
				m_pvtEquiTagPos->push_back(*itr);
			}
			else if(pTagItem->m_pEnt->IsKindOf(CaDraw_TagStru::TypeString()))
			{
				m_pvtStruTagPos->push_back(*itr);
			}
			else
			{
				m_pvtOtherTagPos->push_back(*itr);
			}
		}
	}

	return bRet;
}

/**	
	@brief	erase all pipes having same pipe name except one.

	@author	humkyung

	@date	????.??.??

	@remarks\n
	;2003.05.12 - push back pipe which erased from pipe list to dim list.
	;2004.12.09 - run tag when it is mathch line.

	@return	void	
*/
void CExtDivision::EraseSamePipeTagItem()
{
	if(m_pvtPipeTagPos->empty()) return;

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	stable_sort(m_pvtPipeTagPos->begin(),m_pvtPipeTagPos->end(),_SortByLineId);
	//// for AP , PDSM , PSDS ect ... stable_sort(m_pvtPipeTagPos->begin(),m_pvtPipeTagPos->end(),_SortByName);
		
	bool bErased=false,bIsSamePipeLine=false;
	do
	{
		bErased = false;
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtPipeTagPos->begin();(itr+1) != m_pvtPipeTagPos->end();++itr)
		{			
			if(0 == STRCMP_T((*itr)->pTITEM->name[1]->c_str() , (*(itr+1))->pTITEM->name[1]->c_str()))
			{
				bIsSamePipeLine = true;
			}else	bIsSamePipeLine = false;
						
			if(bIsSamePipeLine)
			{
				if(IsHorizontal())
				{
					double dy[2]={0.,},nRadius[2]={0,};
					double dx=0.;
					
					if((NULL != (*itr)->pTITEM->m_pEnt) && (NULL != (*(itr+1))->pTITEM->m_pEnt))
					{
						if((*itr)->pTITEM->m_pEnt->typeString() != (*(itr+1))->pTITEM->m_pEnt->typeString()) continue;

						if((*itr)->pTITEM->m_pEnt->IsKindOf(CaDraw_TagPipe::TypeString()))
						{
							CaDraw_TagPipe* pTagPipe = static_cast<CaDraw_TagPipe*>((*itr)->pTITEM->m_pEnt);
							nRadius[0] = pTagPipe->pipe()->GetRadius();
							pTagPipe = static_cast<CaDraw_TagPipe*>((*(itr+1))->pTITEM->m_pEnt);
							nRadius[1] = pTagPipe->pipe()->GetRadius();
						}
						else	continue;
						
						dx=fabs((*itr)->pTITEM->m_ptOrigin.x() - (*(itr+1))->pTITEM->m_ptOrigin.x());
						dy[0] = fabs((*itr)->pTITEM->m_ptOrigin.y() - m_lnBaseLine.start().y());
						dy[1] = fabs((*(itr+1))->pTITEM->m_ptOrigin.y() - m_lnBaseLine.start().y());
						
						if((nRadius[1] > nRadius[0]))
						{
							CaDraw_TagPipe* pTagPipe = static_cast<CaDraw_TagPipe*>((*itr)->pTITEM->m_pEnt);
							const double dNominalSizeInMetric = pTagPipe->pipe()->GetNominalPipeSizeInMetric();
							const double dDimDisplaySize = env.m_pipe.m_ext.GetDimDisplaySize();
							if(dNominalSizeInMetric > dDimDisplaySize)
							{
								(*itr)->pTITEM->nLevel = DIM_LEVEL_0;
								(*itr)->m_ptStart = (*itr)->pTITEM->m_ptOrigin;
								m_pvtDimTagPos->push_back(*itr);
							}
							if(env.m_pipe.m_ext.m_bLeaderLine)
							{
								if(dx >= CaDraw_TagEnv::TOLERANCE)
								{
									if(fabs((*itr)->pTITEM->m_ptOrigin.x() - m_lnBaseLine.end().x()) > CaDraw_TagEnv::TOLERANCE)
									{
										m_plstLeader->push_back(new CDraftLeader((*itr)->pTITEM));
									}
								}
							}
							m_pvtPipeTagPos->erase(itr);
							bErased = true;
							break;
						}
						else if((nRadius[0] > nRadius[1]))
						{
							CaDraw_TagPipe* pTagPipe = static_cast<CaDraw_TagPipe*>((*(itr+1))->pTITEM->m_pEnt);
							const double dNominalSizeInMetric = pTagPipe->pipe()->GetNominalPipeSizeInMetric();
							const double dDimDisplaySize = env.m_pipe.m_ext.GetDimDisplaySize();
							if(dNominalSizeInMetric > dDimDisplaySize)
							{
								(*(itr+1))->pTITEM->nLevel = DIM_LEVEL_0;
								(*(itr+1))->m_ptStart = (*(itr+1))->pTITEM->m_ptOrigin;
								m_pvtDimTagPos->push_back(*(itr+1));
							}
							if(env.m_pipe.m_ext.m_bLeaderLine)
							{
								if(dx >= CaDraw_TagEnv::TOLERANCE) m_plstLeader->push_back(new CDraftLeader((*(itr+1))->pTITEM));
							}
							m_pvtPipeTagPos->erase(itr+1);
							
							bErased = true;
							break;
						}
					}
					
					/// remove item which is long distance away from baseline.
					if((dy[0] > dy[1]))
					{
						CaDraw_TagPipe* pTagPipe = static_cast<CaDraw_TagPipe*>((*(itr))->pTITEM->m_pEnt);
						const double dNominalSizeInMetric = pTagPipe->pipe()->GetNominalPipeSizeInMetric();
						const double dDimDisplaySize = env.m_pipe.m_ext.GetDimDisplaySize();
						if(dNominalSizeInMetric > dDimDisplaySize)
						{
							(*itr)->pTITEM->nLevel = DIM_LEVEL_0;
							(*itr)->m_ptStart = (*itr)->pTITEM->m_ptOrigin;
							m_pvtDimTagPos->push_back(*itr);
						}
						if(env.m_pipe.m_ext.m_bLeaderLine)
						{
							if(dx >= CaDraw_TagEnv::TOLERANCE)
							{
								if(fabs((*itr)->pTITEM->m_ptOrigin.x() - m_lnBaseLine.end().x()) > CaDraw_TagEnv::TOLERANCE)
								{
									m_plstLeader->push_back(new CDraftLeader((*itr)->pTITEM));
								}
							}
						}
						m_pvtPipeTagPos->erase(itr);
						
						bErased = true;
						break;
					}
					else
					{
						CaDraw_TagPipe* pTagPipe = static_cast<CaDraw_TagPipe*>((*(itr+1))->pTITEM->m_pEnt);
						const double dNominalSizeInMetric = pTagPipe->pipe()->GetNominalPipeSizeInMetric();
						const double dDimDisplaySize = env.m_pipe.m_ext.GetDimDisplaySize();
						if(dNominalSizeInMetric > dDimDisplaySize)
						{
							(*(itr+1))->pTITEM->nLevel = DIM_LEVEL_0;
							(*(itr+1))->m_ptStart = (*(itr+1))->pTITEM->m_ptOrigin;
							m_pvtDimTagPos->push_back(*(itr+1));
						}
						if(env.m_pipe.m_ext.m_bLeaderLine)
						{
							if(dx >= CaDraw_TagEnv::TOLERANCE) m_plstLeader->push_back(new CDraftLeader((*(itr+1))->pTITEM));
						}
						m_pvtPipeTagPos->erase(itr+1);
						
						bErased = true;
						break;
					}
				}
				else
				{
					double dx[2]={0.,},nRadius[2]={0,};
					double dy=0.;
					
					if((NULL != (*itr)->pTITEM->m_pEnt) && (NULL != (*(itr+1))->pTITEM->m_pEnt))
					{
						if((*itr)->pTITEM->m_pEnt->IsKindOf(CaDraw_TagPipe::TypeString()))
						{
							CaDraw_TagPipe* pTagPipe = static_cast<CaDraw_TagPipe*>((*itr)->pTITEM->m_pEnt);
							nRadius[0] = pTagPipe->pipe()->GetRadius();
							pTagPipe = static_cast<CaDraw_TagPipe*>((*(itr+1))->pTITEM->m_pEnt);
							nRadius[1] = pTagPipe->pipe()->GetRadius();
						}
						else	continue;

						dy=fabs((*itr)->pTITEM->m_ptOrigin.y() - (*(itr+1))->pTITEM->m_ptOrigin.y());
						dx[0] = fabs((*itr)->pTITEM->m_ptOrigin.x() - m_lnBaseLine.start().x());
						dx[1] = fabs((*(itr+1))->pTITEM->m_ptOrigin.x() - m_lnBaseLine.start().x());
						
						if((nRadius[1] > nRadius[0]))
						{
							CaDraw_TagPipe* pTagPipe = static_cast<CaDraw_TagPipe*>((*(itr))->pTITEM->m_pEnt);
							const double dNominalSizeInMetric = pTagPipe->pipe()->GetNominalPipeSizeInMetric();
							const double dDimDisplaySize = env.m_pipe.m_ext.GetDimDisplaySize();
							if(dNominalSizeInMetric > dDimDisplaySize)
							{
								(*itr)->pTITEM->nLevel = DIM_LEVEL_0;
								(*itr)->m_ptStart = (*itr)->pTITEM->m_ptOrigin;
								m_pvtDimTagPos->push_back(*itr);
							}
							if(env.m_pipe.m_ext.m_bLeaderLine)
							{
								if(dy >= CaDraw_TagEnv::TOLERANCE) m_plstLeader->push_back(new CDraftLeader((*itr)->pTITEM));
							}
							m_pvtPipeTagPos->erase(itr);
							
							bErased = true;
							break;
						}
						else if((nRadius[0] > nRadius[1]))
						{
							CaDraw_TagPipe* pTagPipe = static_cast<CaDraw_TagPipe*>((*(itr+1))->pTITEM->m_pEnt);
							const double dNominalSizeInMetric = pTagPipe->pipe()->GetNominalPipeSizeInMetric();
							const double dDimDisplaySize = env.m_pipe.m_ext.GetDimDisplaySize();
							if(dNominalSizeInMetric > dDimDisplaySize)
							{
								(*(itr+1))->pTITEM->nLevel = DIM_LEVEL_0;
								(*(itr+1))->m_ptStart = (*(itr+1))->pTITEM->m_ptOrigin;
								m_pvtDimTagPos->push_back(*(itr+1));
							}
							if(env.m_pipe.m_ext.m_bLeaderLine)
							{
								if(dy >= CaDraw_TagEnv::TOLERANCE) m_plstLeader->push_back(new CDraftLeader((*(itr+1))->pTITEM));
							}
							m_pvtPipeTagPos->erase(itr+1);
							
							bErased = true;
							break;
						}
					}
					
					/// remove item which is long distance away from baseline.
					if((dx[0] > dx[1]))
					{
						CaDraw_TagPipe* pTagPipe = static_cast<CaDraw_TagPipe*>((*(itr))->pTITEM->m_pEnt);
						const double dNominalSizeInMetric = pTagPipe->pipe()->GetNominalPipeSizeInMetric();
						const double dDimDisplaySize = env.m_pipe.m_ext.GetDimDisplaySize();
						if(dNominalSizeInMetric > dDimDisplaySize)
						{
							(*itr)->pTITEM->nLevel = DIM_LEVEL_0;
							(*itr)->m_ptStart = (*itr)->pTITEM->m_ptOrigin;
							m_pvtDimTagPos->push_back(*itr);
						}
						if(env.m_pipe.m_ext.m_bLeaderLine)
						{
							if(dy >= CaDraw_TagEnv::TOLERANCE) m_plstLeader->push_back(new CDraftLeader((*itr)->pTITEM));
						}
						m_pvtPipeTagPos->erase(itr);
						
						bErased = true;
						break;
					}
					else
					{
						CaDraw_TagPipe* pTagPipe = static_cast<CaDraw_TagPipe*>((*(itr+1))->pTITEM->m_pEnt);
						const double dNominalSizeInMetric = pTagPipe->pipe()->GetNominalPipeSizeInMetric();
						const double dDimDisplaySize = env.m_pipe.m_ext.GetDimDisplaySize();
						if(dNominalSizeInMetric > dDimDisplaySize)
						{
							(*(itr+1))->pTITEM->nLevel = DIM_LEVEL_0;
							(*(itr+1))->m_ptStart = (*(itr+1))->pTITEM->m_ptOrigin;
							m_pvtDimTagPos->push_back(*(itr+1));
						}
						if(env.m_pipe.m_ext.m_bLeaderLine)
						{
							if(dy >= CaDraw_TagEnv::TOLERANCE) m_plstLeader->push_back(new CDraftLeader((*(itr+1))->pTITEM));
						}
						m_pvtPipeTagPos->erase(itr+1);
						
						bErased = true;
						break;
					}
				}
			}
		}
	}while(bErased);
	EraseSamePipeTagItem_Pt();
}

/**	
	@brief	erase all pipes having same pipe name except one.\n
	created by sinkab.\n
	           ~~~~~~

	@remarks\n
	;2003.05.14 - push back pipe which erased from pipe list to dim list.

	@return	void	
*/
void CExtDivision::EraseSamePipeTagItem_Pt()
{
	TCHAR szName[2][64]={0,};

	if(!m_pvtPipeTagPos->empty())
	{
		stable_sort(m_pvtPipeTagPos->begin(),m_pvtPipeTagPos->end(),_SortByName);
		if(IsHorizontal())
		{
			for(vector<CaDraw_TagPos*>::iterator itr=m_pvtPipeTagPos->begin();itr != m_pvtPipeTagPos->end();itr++)
			{
				if(VERTICAL != (*itr)->pTITEM->enmSubType) continue;
				for(vector<CaDraw_TagPos*>::iterator itrNext=itr+1;itrNext != m_pvtPipeTagPos->end();)
				{
					if(VERTICAL == (*itrNext)->pTITEM->enmSubType)
					{
						double dx=fabs((*itr)->m_ptStart.x() - (*itrNext)->m_ptStart.x());
						
						if(fabs(dx) < 0.1)
						{
							double dy[2]={0.,};						
							itrNext = m_pvtPipeTagPos->erase(itrNext);
							continue;
							
							//bErased = true;
							//break;
						}
					}
					++itrNext;
				}
			}
		}
		else if(IsVertical())
		{
			for(vector<CaDraw_TagPos*>::iterator itr=m_pvtPipeTagPos->begin();itr != m_pvtPipeTagPos->end();itr++)
			{
				if(VERTICAL != (*itr)->pTITEM->enmSubType) continue;
				for(vector<CaDraw_TagPos*>::iterator itrNext=itr+1;itrNext != m_pvtPipeTagPos->end();/*;itrNext++*/)
				{
					if(VERTICAL == (*itrNext)->pTITEM->enmSubType)
					{
						double dy=fabs((*itr)->m_ptStart.y() - (*itrNext)->m_ptStart.y());
						
						if(fabs(dy) < 0.1)
						{
							double dy[2]={0.,};
							itrNext = m_pvtPipeTagPos->erase(itrNext);
							continue;
						}
					}
					++itrNext;
				}
			}
		}
	}
}

/**	
	@brief	erase all equipments having same equipment name except one.


	@return	void	
*/
void CExtDivision::EraseSameEquiTagItem()
{
	TCHAR szName[2][64]={0,};

	if(!m_pvtEquiTagPos->empty())
	{
		stable_sort(m_pvtEquiTagPos->begin(),m_pvtEquiTagPos->end(),_SortByName);
		bool bErased=false;
		do
		{
			bErased = false;
			for(vector<CaDraw_TagPos*>::iterator itr=m_pvtEquiTagPos->begin();(itr+1) != m_pvtEquiTagPos->end();itr++)
			{
				STRCPY_T(szName[0],(*itr)->pTITEM->name[0]->c_str());
				STRCPY_T(szName[1],(*(itr + 1))->pTITEM->name[0]->c_str());
				
				if(0 == STRCMP_T(szName[0],szName[1]))
				{
					if(IsHorizontal())
					{
						double dy[2]={0.,};
						
						dy[0] = fabs((*itr)->pTITEM->m_ptOrigin.y() - m_lnBaseLine.start().y());
						dy[1] = fabs((*(itr+1))->pTITEM->m_ptOrigin.y() - m_lnBaseLine.start().y());
						if(dy[0] > dy[1])
						{
							m_pvtEquiTagPos->erase(itr);
							bErased = true;
							break;
						}
						else
						{
							m_pvtEquiTagPos->erase(itr+1);
							bErased = true;
							break;
						}
					}
					else
					{
						double dx[2]={0.,};
						
						dx[0] = fabs((*itr)->pTITEM->m_ptOrigin.x() - m_lnBaseLine.start().x());
						dx[1] = fabs((*(itr+1))->pTITEM->m_ptOrigin.x() - m_lnBaseLine.start().x());
						if(dx[0] > dx[1])
						{
							m_pvtEquiTagPos->erase(itr);
							bErased = true;
							break;
						}
						else
						{
							m_pvtEquiTagPos->erase(itr+1);
							bErased = true;
							break;
						}
					}
				}
			}
		}while(bErased);
	}
}

/**	\brief	The CExtDivision::EraseSameStruTagItem function\n
	erase all structure having same equipment name except one.


	\return	void	
*/
void CExtDivision::EraseSameStruTagItem()
{
	TCHAR szName[4][64]={0,};

	if(!m_pvtStruTagPos->empty())
	{
		stable_sort(m_pvtStruTagPos->begin(),m_pvtStruTagPos->end(),_SortByName);
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtStruTagPos->begin();itr != m_pvtStruTagPos->end();itr++)
		{
			for(vector<CaDraw_TagPos*>::iterator itrNext=itr+1;itrNext != m_pvtStruTagPos->end();itrNext++)
			{
				STRCPY_T(szName[0],(*itr)->pTITEM->name[0]->c_str());
				STRCPY_T(szName[1],(*itrNext)->pTITEM->name[0]->c_str());
				STRCPY_T(szName[2],(*itr)->pTITEM->name[1]->c_str());
				STRCPY_T(szName[3],(*itrNext)->pTITEM->name[1]->c_str());
				
				if(	(0 == STRCMP_T((*itr)->pTITEM->name[0]->c_str(),(*(itrNext))->pTITEM->name[0]->c_str())) && 
					(0 == STRCMP_T((*itr)->pTITEM->name[1]->c_str(),(*(itrNext))->pTITEM->name[1]->c_str())))
				{ /// have same name
					if(IsHorizontal())
					{
						double dy[2]={0.,};
						
						dy[0] = fabs((*itr)->pTITEM->m_ptOrigin.y() - m_lnBaseLine.start().y());
						dy[1] = fabs((*(itrNext))->pTITEM->m_ptOrigin.y() - m_lnBaseLine.start().y());
						
						double dx=fabs((*itr)->pTITEM->m_ptOrigin.x() - (*(itrNext))->pTITEM->m_ptOrigin.x());
//-----------------------------------------------------------------------------------------
						if(dx < 1)
						{	// tempary code for PW ENM_PDS.
//-----------------------------------------------------------------------------------------
							if(dy[0] > dy[1])
							{
								swap(*itr,*itrNext);
								m_pvtStruTagPos->erase(itrNext--);
							}else	m_pvtStruTagPos->erase(itrNext--);
						}
					}
					else
					{
						double dx[2]={0.,};
						
						dx[0] = fabs((*itr)->pTITEM->m_ptOrigin.x() - m_lnBaseLine.start().x());
						dx[1] = fabs((*(itrNext))->pTITEM->m_ptOrigin.x() - m_lnBaseLine.start().x());
						
						double dy=fabs((*itr)->pTITEM->m_ptOrigin.y() - (*(itrNext))->pTITEM->m_ptOrigin.y());
//-----------------------------------------------------------------------------------------
						if(dy < 1)
						{	// tempary code for PW ENM_PDS.
//-----------------------------------------------------------------------------------------
							if(dx[0] > dx[1])
							{
								swap(*itr,*itrNext);
								m_pvtStruTagPos->erase(itrNext--);
							}else	m_pvtStruTagPos->erase(itrNext--);
						}
					}
				}
			}
		}
	}
}

/**	
	@brief	annotate
	@author	humkyung
	@date	????.??.??
	2004.12.01 - write only structure dimension when name is '-'.
	2007.11.09 - pipe grouping은 second tag type이 ONLY_ELEV일 때만 한다.
	@return	void	
*/
void CExtDivision::Annotate()
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	{
		CIsLine3d lnBaseLine=m_lnBaseLine;
		if((fabs(m_vecTagDir.dy()) > 0.001) && (lnBaseLine.start().x() > lnBaseLine.end().x()))
		{
			/// swap line
			CIsPoint3d ptStart(m_lnBaseLine.start()) , ptEnd(m_lnBaseLine.end());
			m_lnBaseLine.Set(ptEnd , ptStart);
			
			TCHAR sz[64]={0,};
			STRNCPY_T(sz,m_szDim[0],64);
			sz[63] = '\0';
			STRCPY_T(m_szDim[0],m_szDim[1]);
			STRCPY_T(m_szDim[1],sz);
		}
		
		/// swap base line points.
		if((fabs(m_vecTagDir.dx()) > 0.001) && (lnBaseLine.start().y() > lnBaseLine.end().y()))
		{
			/// swap line
			CIsPoint3d ptStart(m_lnBaseLine.start()) , ptEnd(m_lnBaseLine.end());
			m_lnBaseLine.Set(ptEnd , ptStart);
			
			TCHAR sz[64]={0,};
			STRNCPY_T(sz,m_szDim[0],64);
			sz[63] = '\0';
			STRCPY_T(m_szDim[0],m_szDim[1]);
			STRCPY_T(m_szDim[1],sz);
		}
		
		/// erase same pipe tag item
		//////////////////////////////////////////////////////////////////////////
		EraseSamePipeTagItem();
		int nSiz=m_pvtPipeTagPos->size();
		//////////////////////////////////////////////////////////////////////////
		EraseSamePipeTagItem_Pt();
		nSiz=m_pvtPipeTagPos->size();
		EraseSameEquiTagItem();
		EraseSameStruTagItem();
		
		//////////////////////////////////////////////////////////////////////////
		/// STR
		/*if(false == env.m_stru.bTaggingOverlap)
		{
			for(vector<CaDraw_TagPos*>::iterator itrStrTagPos=m_pvtStruTagPos->begin();itrStrTagPos != m_pvtStruTagPos->end();++itrStrTagPos)
			{
				CaDraw_TagPos* pTagPos=(*itrStrTagPos);
				if('-' == (*(*itrStrTagPos)->pTITEM->name[0])[0]) continue;
				(*itrStrTagPos)->bFixed = false;
				m_pvtTagPos->push_back(*itrStrTagPos);
			}
			RunStrTag();
		}
		else
		{
			for(vector<CaDraw_TagPos*>::iterator itrStrTagPos=m_pvtStruTagPos->begin();itrStrTagPos != m_pvtStruTagPos->end();++itrStrTagPos)
			{
				CaDraw_TagPos* pTagPos=(*itrStrTagPos);
				if('-' == (*(*itrStrTagPos)->pTITEM->name[0])[0]) continue;
				(*itrStrTagPos)->bFixed = true;
				m_pvtTagPos->push_back(*itrStrTagPos);
			}
		}*/
		
		////////////////////////////////////////////////////////////////////////////
		///// EQUIPMENT
		//if(env.m_equi.bTag)	/// check if equipment tag output is on
		//{
		//	for(vector<CaDraw_TagPos*>::iterator itrEqpTagPos=m_pvtEquiTagPos->begin();itrEqpTagPos != m_pvtEquiTagPos->end();++itrEqpTagPos)
		//	{
		//		(*itrEqpTagPos)->bFixed = false;
		//		m_pvtTagPos->push_back(*itrEqpTagPos);
		//	}
		//}
		//if(IsHorizontal())
		//	stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByX);
		//else	stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByY);
		//RunPipeAndEquiTag();
		////////////////////////////////////////////////////////////////////////////
		//
		//for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		//{
		//	(*itr)->bFixed = true;
		//}
		//for(vector<CaDraw_TagPos*>::iterator itrPipeTagPos=m_pvtPipeTagPos->begin();itrPipeTagPos != m_pvtPipeTagPos->end();++itrPipeTagPos)
		//{
		//	m_pvtTagPos->push_back(*itrPipeTagPos);
		//}
		//for(vector<CaDraw_TagPos*>::iterator itrTITEMTagPos=m_pvtOtherTagPos->begin();itrTITEMTagPos != m_pvtOtherTagPos->end();++itrTITEMTagPos)
		//{
		//	m_pvtTagPos->push_back(*itrTITEMTagPos);
		//}
		//if(IsHorizontal())
		//	stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByX);
		//else	stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByY);
		//
		//if((env.m_pipe.m_ext.nGroupLevel > 0) && (DOUBLELINE == env.m_pipe.m_ext.eTag))
		//{
		//	if( ONLY_ELEV == env.m_pipe.m_ext.eSecondTagType )	/// grouping은 second tag type이 ONLY_ELEV일 때만 한다.
		//	{
		//		if((ELEV_MIXIN == env.m_pipe.m_ext.eElev) || (ELEV_BOP == env.m_pipe.m_ext.eElev))
		//			GroupByBOP();
		//		else if((ELEV_TOP == env.m_pipe.m_ext.eElev))
		//			GroupByTOP();
		//		else if((ELEV_CL == env.m_pipe.m_ext.eElev))
		//			GroupByCL();
		//	}
		//}
		//ComputateSpace((*m_pvtTagPos));	
		
		RunTag();
		RunDimension();
	}
}

/**	
	@brief	The CExtDivision::InsertBOP function
	@return	void	
*/
void CExtDivision::InsertBOP()
{
	CountOverlap((*m_pvtTagPos));
	if(IsHorizontal())
	{
		return;
		
		double nPrevSpace=0,nNextSpace=0;
		vector<CaDraw_TagPos*>::iterator itr;
		CaDraw_TagPos* ptr;
		for(itr=m_pvtBOP->begin();itr != m_pvtBOP->end();++itr)
		{
			nPrevSpace = (*itr)->next->nPrevSpace;
			for(ptr=(*itr)->next;ptr && ptr->next;ptr = ptr->next);
			nNextSpace = ptr->nNextSpace;

			if(-1 == nPrevSpace)
			{
				(*itr)->m_ptStart.SetX(m_lnBaseLine.start().x());
				(*itr)->m_ptStart.SetY((*itr)->next->m_ptStart.y());
				(*itr)->m_ptEnd.SetX(m_lnBaseLine.start().x());
				(*itr)->m_ptEnd.SetY((*itr)->next->m_ptEnd.y() + ((*itr)->next->rect.height)*0.5);
			}
			else if(-1 == nNextSpace)
			{
				(*itr)->m_ptStart.SetX(m_lnBaseLine.start().x());
				(*itr)->m_ptStart.SetY(ptr->next->m_ptStart.y());
				(*itr)->m_ptEnd.SetX(m_lnBaseLine.start().x());
				(*itr)->m_ptEnd.SetY(ptr->m_ptEnd.y() - (ptr->rect.height)*0.5);
			}
			else if(nPrevSpace > nNextSpace)
			{
				(*itr)->m_ptStart.SetX(m_lnBaseLine.start().x());
				(*itr)->m_ptStart.SetY((*itr)->next->m_ptStart.y());
				(*itr)->m_ptEnd.SetX(m_lnBaseLine.start().x());
				(*itr)->m_ptEnd.SetY((*itr)->next->m_ptEnd.y() + ((*itr)->next->rect.height)*0.5);
			}
			else
			{
				(*itr)->m_ptStart.SetX(m_lnBaseLine.start().x());
				(*itr)->m_ptStart.SetY(ptr->next->m_ptStart.y());
				(*itr)->m_ptEnd.SetX(m_lnBaseLine.start().x());
				(*itr)->m_ptEnd.SetY(ptr->m_ptEnd.y() - (ptr->rect.height)*0.5);
			}
			m_pvtTagPos->push_back(*itr);
		}
		stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByX);
	}
	else
	{
		double nPrevSpace=0,nNextSpace=0;
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtBOP->begin();itr != m_pvtBOP->end();++itr)
		{
			nPrevSpace = (*itr)->next->nPrevSpace;
			CaDraw_TagPos* ptr;
			for(ptr=(*itr)->next;ptr && ptr->next;ptr = ptr->next);
			nNextSpace = ptr->nNextSpace;

			if(-1 == nPrevSpace)
			{
				(*itr)->m_ptStart.SetX(m_lnBaseLine.start().x());
				(*itr)->m_ptStart.SetY((*itr)->next->m_ptEnd.y() + ((*itr)->next->rect.height)*0.5);
				(*itr)->m_ptEnd = (*itr)->m_ptStart;
			}
			else if(-1 == nNextSpace)
			{
				(*itr)->m_ptStart.SetX(m_lnBaseLine.start().x());
				(*itr)->m_ptStart.SetY(ptr->m_ptEnd.y() - (ptr->rect.height)*0.5);
				(*itr)->m_ptEnd = (*itr)->m_ptStart;
			}
			else if(nPrevSpace > nNextSpace)
			{
				(*itr)->m_ptStart.SetX(m_lnBaseLine.start().x());
				(*itr)->m_ptStart.SetY((*itr)->next->m_ptEnd.y() + ((*itr)->next->rect.height)*0.5);
				(*itr)->m_ptEnd = (*itr)->m_ptStart;
			}
			else
			{
				(*itr)->m_ptStart.SetX(m_lnBaseLine.start().x());
				(*itr)->m_ptStart.SetY(ptr->m_ptEnd.y() - (ptr->rect.height)*0.5);
				(*itr)->m_ptEnd = (*itr)->m_ptStart;
			}
			m_pvtTagPos->push_back(*itr);
		}
		stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByY);
	}
}

/**	
	@brief	write external division data
	@author	BHK
	@date	?
	@param	file	a parameter of type tstream&
	@remarks\n
	;2003.07.03 - check the g_varMatch.bWriteMatchLine variable.
	@return	void	
*/
void CExtDivision::Write(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CExtWriter writer(this);
	writer.Write(ofile,dDrawingScale);
}

/**	
	@brief	computate the longest dimension line.

	@author	HumKyung.BAEK

	@remarks\n
	;2003.04.07 - adapt eqp text height and width factor

	@return	void	
*/
void CExtDivision::CalculateUpperLine()
{
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		double nMaxLen=0.;
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			if(NULL == (*itr)->pTITEM) continue;
			if((*itr)->pTITEM->m_pEnt->IsKindOf(CaDraw_TagPipe::TypeString()))
			{
				TCHAR* pName=GetFirstTagNo((*itr)->pTITEM);
				if(pName)
				{
					CSmartDrawAnnoTextStyle TextStyle(_T("3") , env.m_anno.m_nLineTextHeight , env.m_nWidthFactor);
					const double nLen = TextStyle.GetTextLength(pName);
					nMaxLen = (nMaxLen < nLen) ? nLen : nMaxLen;
				}
			}
			/*
			else if(DRAFT_EQP == (*itr)->pTITEM->enmType)
			{
				if((*itr)->pTITEM->name[0])
				{
					double nLen=CSmartDrawAnnoModule::GetTextLength((*itr)->pTITEM->name[0]->c_str(), 0 , g_nWidthFactor,env.m_equi.nTextHeight);
					nMaxLen = (nMaxLen < nLen) ? nLen : nMaxLen;
				}
				if((*itr)->pTITEM->name[1])
				{
					double nLen=CSmartDrawAnnoModule::GetTextLength((*itr)->pTITEM->name[1]->c_str(),0,g_nWidthFactor,env.m_equi.nCoordHeight);
					nMaxLen = (nMaxLen < nLen) ? nLen : nMaxLen;
				}
			}
			else if(DRAFT_ETIM == (*itr)->pTITEM->enmType)
			{
				if((*itr)->pTITEM->name[0])
				{
					double nLen=CSmartDrawAnnoModule::GetTextLength((*itr)->pTITEM->name[0]->c_str(),0,g_nWidthFactor,env.m_anno.m_nLineTextHeight);
					nMaxLen = (nMaxLen < nLen) ? nLen : nMaxLen;
				}
				if((*itr)->pTITEM->name[1])
				{
					double nLen=CSmartDrawAnnoModule::GetTextLength((*itr)->pTITEM->name[1]->c_str(),0,g_nWidthFactor,env.m_anno.m_nLineTextHeight);
					nMaxLen = (nMaxLen < nLen) ? nLen : nMaxLen;
				}
			}
			*/
		}

		double nDistToUpperline=0.0;
		if((ALL_GLOBAL_DIMENSION == env.m_generation.m_global_dim) || 
		   ((TOP_LEFT_GLOBAL_DIMENSION == env.m_generation.m_global_dim) && 
			(IsLeftSide() || IsTopSide())))
		{
			nDistToUpperline = env.m_nDistText + nMaxLen + env.m_outside.m_nDistD + env.m_outside.m_nDistE;
		}
		else
		{
			nDistToUpperline = env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_anno.m_nLineTextHeight + nMaxLen + env.m_outside.m_nDistD + env.m_outside.m_nDistE;
		}
		
		if(env.m_outside.PipeEqpDist() > nDistToUpperline) nDistToUpperline =  env.m_outside.PipeEqpDist();
		nDistToUpperline = env.m_outside.PipeEqpDist();
		m_lnUpperLine[0]  += CIsPoint3d(
			m_vecTagDir.dx()*nDistToUpperline , 
			m_vecTagDir.dy()*nDistToUpperline ,
			0.f);
		m_lnUpperLine[1] += CIsPoint3d(
			m_vecTagDir.dx()*nDistToUpperline , 
			m_vecTagDir.dy()*nDistToUpperline , 
			0.f);
	}
}

/**	\brief	The CExtDivision::DoesExistEqualDim function

	\param	pTagPos	a parameter of type CaDraw_TagPos*

	\return	bool	
*/
bool CExtDivision::DoesExistEqualDim(const DIM_LEVEL_E nLevel,CaDraw_TagPos* pTagPos)
{
	assert(pTagPos && "pTagPos is NULL");
	static const double nTol = 0.001;
	bool bRet=false;

	if(pTagPos)
	{
		if(IsHorizontal())
		{
			for(list<CaDraw_TagPos*>::iterator itr = m_plstDimension->begin();itr != m_plstDimension->end();itr++)
			{
				if((*itr)->pTITEM->nLevel <= nLevel) continue;
				if((fabs((*itr)->m_ptStart.x() - pTagPos->m_ptStart.x()) < nTol) && 
				   (fabs((*itr)->m_ptEnd.x() - pTagPos->m_ptEnd.x()) < nTol))
				{
					bRet = true;
					break;
				}
			}
		}
		else
		{
			for(list<CaDraw_TagPos*>::iterator itr = m_plstDimension->begin();itr != m_plstDimension->end();itr++)
			{
				if((*itr)->pTITEM->nLevel <= nLevel) continue;
				if((fabs((*itr)->m_ptStart.y() - pTagPos->m_ptStart.y()) < nTol) && 
				   (fabs((*itr)->m_ptEnd.y() - pTagPos->m_ptEnd.y()) < nTol))
				{
					bRet = true;
					break;
				}
			}
		}
	}

	return bRet;
}

/**	
	@brief	create dimension levels.
	@author	humkyung
	@remarks
	;2003.10.22 - create dim level3 on top or left side only.
	@return	void	
*/
void CExtDivision::RunDimension()
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	if(NULL == m_pDimLevel0) m_pDimLevel0 = new CDimLevel0(this);
	if(NULL == m_pDimLevel1)
	{
		if(!env.m_outside.DistBIsZero()) m_pDimLevel1 = new CDimLevel(DIM_LEVEL_1);
	}
	if(NULL == m_pDimLevel2)
	{
		GLOBAL_DIMENSION_E varGlobalDimOpt=env.m_generation.m_global_dim;
		if(	(ALL_GLOBAL_DIMENSION == varGlobalDimOpt) || 
			((TOP_LEFT_GLOBAL_DIMENSION == varGlobalDimOpt) && (IsLeftSide() || IsTopSide())))
		{
			if(!env.m_outside.DistCIsZero()) m_pDimLevel2 = new CDimLevel(DIM_LEVEL_2);
		}
	}

	long nDimLevel0Count=0L;
	if(m_pDimLevel0)
	{ /// dim level0
		CIsLine3d lnBase;

		lnBase = m_lnBaseLine;
		lnBase[0] += CIsPoint3d(
			m_vecTagDir.dx()*(env.m_outside.m_nDistA) , 
			m_vecTagDir.dy()*(env.m_outside.m_nDistA) ,
			0.f);
		lnBase[1] += CIsPoint3d(
			m_vecTagDir.dx()*(env.m_outside.m_nDistA) ,
			m_vecTagDir.dy()*(env.m_outside.m_nDistA) ,
			0.f);
		m_pDimLevel0->SetBaseLine(lnBase);

		for(vector<CaDraw_TagPos*>::const_iterator itrPipeTagPos=m_pvtPipeTagPos->begin();itrPipeTagPos != m_pvtPipeTagPos->end();++itrPipeTagPos)
		{
			if((*itrPipeTagPos)->OnRegisterToDimension(m_pDimLevel0)) ++nDimLevel0Count;
		}
		for(vector<CaDraw_TagPos*>::const_iterator itrEqpTagPos=m_pvtEquiTagPos->begin();itrEqpTagPos != m_pvtEquiTagPos->end();++itrEqpTagPos)
		{
			if((*itrEqpTagPos)->OnRegisterToDimension(m_pDimLevel0)) ++nDimLevel0Count;
		}
		for(vector<CaDraw_TagPos*>::const_iterator itrStrTagPos=m_pvtStruTagPos->begin();itrStrTagPos != m_pvtStruTagPos->end();++itrStrTagPos)
		{
			if((*itrStrTagPos)->OnRegisterToDimension(m_pDimLevel0)) ++nDimLevel0Count;
		}
		
		for(vector<CaDraw_TagPos*>::const_iterator itr=m_pvtOtherTagPos->begin();itr != m_pvtOtherTagPos->end();++itr)
		{
			if((*itr)->OnRegisterToDimension(m_pDimLevel0)) ++nDimLevel0Count;
			
		}

		for(vector<CaDraw_TagPos*>::const_iterator itrDIMTagPos=m_pvtDimTagPos->begin();itrDIMTagPos != m_pvtDimTagPos->end();++itrDIMTagPos)
		{
			if(((*itrDIMTagPos)->nLevelOn) & DIM_LEVEL_0)
			{
				(*m_pDimLevel0)+=*itrDIMTagPos;
				nDimLevel0Count++;
			}
		}
	}

	long nDimLevel1Count=0L;
	if(m_pDimLevel1)
	{ /// dim level1
		CIsLine3d lnBase;
		lnBase = m_lnBaseLine;
		lnBase[0] += CIsPoint3d(
			m_vecTagDir.dx()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB) ,
			m_vecTagDir.dy()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB) ,
			0.f);
		lnBase[1] += CIsPoint3d(
			m_vecTagDir.dx()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB) ,
			m_vecTagDir.dy()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB) ,
			0.f);
		m_pDimLevel1->SetBaseLine(lnBase);

		for(vector<CaDraw_TagPos*>::const_iterator itrPipeTagPos=m_pvtPipeTagPos->begin();itrPipeTagPos != m_pvtPipeTagPos->end();++itrPipeTagPos)
		{
			if((*itrPipeTagPos)->OnRegisterToDimension(m_pDimLevel1)) ++nDimLevel1Count;
		}
		for(vector<CaDraw_TagPos*>::const_iterator itrEqpTagPos=m_pvtEquiTagPos->begin();itrEqpTagPos != m_pvtEquiTagPos->end();++itrEqpTagPos)
		{
			if((*itrEqpTagPos)->OnRegisterToDimension(m_pDimLevel1)) nDimLevel1Count++;
		}
		for(vector<CaDraw_TagPos*>::const_iterator itrStrTagPos=m_pvtStruTagPos->begin();itrStrTagPos != m_pvtStruTagPos->end();++itrStrTagPos)
		{
			if((*itrStrTagPos)->OnRegisterToDimension(m_pDimLevel1)) nDimLevel1Count++;
		}
		
		for(vector<CaDraw_TagPos*>::const_iterator itr=m_pvtOtherTagPos->begin();itr != m_pvtOtherTagPos->end();++itr)
		{
			if(((*itr)->nLevelOn & DIM_LEVEL_1) && (*itr)->OnRegisterToDimension(m_pDimLevel1)) nDimLevel1Count++;
		}

		for(vector<CaDraw_TagPos*>::const_iterator itrDIMTagPos=m_pvtDimTagPos->begin();itrDIMTagPos != m_pvtDimTagPos->end();++itrDIMTagPos)
		{
			if(((*itrDIMTagPos)->nLevelOn) & DIM_LEVEL_1)
			{
				(*m_pDimLevel1)+=*itrDIMTagPos;
				nDimLevel1Count++;
			}
		}
	}

	long nDimLevel2Count=0L;
	if(m_pDimLevel2)
	{ /// dim level2
		CIsLine3d lnBase;

		lnBase = m_lnBaseLine;
		lnBase[0] += CIsPoint3d(
			m_vecTagDir.dx()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC) ,
			m_vecTagDir.dy()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC) ,
			0.f);
		lnBase[1] += CIsPoint3d(
			m_vecTagDir.dx()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC) ,
			m_vecTagDir.dy()*(env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC) ,
			0.f);
		m_pDimLevel2->SetBaseLine(lnBase);

		for(vector<CaDraw_TagPos*>::const_iterator itrPipeTagPos=m_pvtPipeTagPos->begin();itrPipeTagPos != m_pvtPipeTagPos->end();++itrPipeTagPos)
		{
			if((*itrPipeTagPos)->OnRegisterToDimension(m_pDimLevel2)) ++nDimLevel2Count;
		}
		for(vector<CaDraw_TagPos*>::const_iterator itrEqpTagPos=m_pvtEquiTagPos->begin();itrEqpTagPos != m_pvtEquiTagPos->end();++itrEqpTagPos)
		{
			if((*itrEqpTagPos)->OnRegisterToDimension(m_pDimLevel2)) nDimLevel2Count++;
		}
		for(vector<CaDraw_TagPos*>::const_iterator itrStrTagPos=m_pvtStruTagPos->begin();itrStrTagPos != m_pvtStruTagPos->end();++itrStrTagPos)
		{
			if((*itrStrTagPos)->OnRegisterToDimension(m_pDimLevel2)) nDimLevel2Count++;
		}

		for(vector<CaDraw_TagPos*>::const_iterator itr=m_pvtOtherTagPos->begin();itr != m_pvtOtherTagPos->end();++itr)
		{
			if(((*itr)->nLevelOn & DIM_LEVEL_2) && (*itr)->OnRegisterToDimension(m_pDimLevel2)) nDimLevel2Count++;
		}

		for(vector<CaDraw_TagPos*>::const_iterator itrDIMTagPos=m_pvtDimTagPos->begin();itrDIMTagPos != m_pvtDimTagPos->end();++itrDIMTagPos)
		{
			if(((*itrDIMTagPos)->nLevelOn) & DIM_LEVEL_2)
			{
				(*m_pDimLevel2)+=*itrDIMTagPos;
				nDimLevel2Count++;
			}
		}
	}

	if(m_pDimLevel0)
	{
		vector<CaDraw_TagPos*>* pvtDITEM=m_pDimLevel0->GetDITEMVector();
		if(!pvtDITEM->empty())
		{
			if(IsHorizontal())
				stable_sort(pvtDITEM->begin(),pvtDITEM->end(),_SortByX);
			else	stable_sort(pvtDITEM->begin(),pvtDITEM->end(),_SortByY);

			m_pDimLevel0->Run(this);
		}
		else if(DIM_LEVEL_0 & env.m_generation.GetDimLevelOf( _T("Global")) )
		{
			CIsLine3d lnBase = m_pDimLevel0->GetBaseLine();
			
			CaDraw_TagPos* pTagPos = CSmartDrawAnnoModule::GetInstance().new_tag_pos();
			if(pTagPos)
			{
				pTagPos->type   = DIM_TAG;
				pTagPos->pTITEM = CSmartDrawAnnoModule::GetInstance().new_titem_t();
				pTagPos->pTITEM->nLevel = DIM_LEVEL_0;
				pTagPos->m_ptStart = lnBase.start();
				pTagPos->m_ptEnd   = lnBase.end();
				
				m_plstDimension->push_back(pTagPos);
			}
		}
		else if(((NULL != m_pDimLevel1) && !m_pDimLevel1->GetDITEMVector()->empty()) || (DIM_LEVEL_1 & env.m_generation.GetDimLevelOf(_T("Global"))))
		{
			m_pDimLevel1->SetBaseLine(m_pDimLevel0->GetBaseLine());
			m_pDimLevel1->SetLevel(DIM_LEVEL_0);
		}
		else if((NULL != m_pDimLevel2) && !m_pDimLevel2->GetDITEMVector()->empty())
		{
			m_pDimLevel2->SetBaseLine(m_pDimLevel0->GetBaseLine());
			m_pDimLevel2->SetLevel(DIM_LEVEL_0);
		}
	}

	if(m_pDimLevel1)
	{
		vector<CaDraw_TagPos*>* pvtDITEM=m_pDimLevel1->GetDITEMVector();
		if(!pvtDITEM->empty())
		{
			m_pDimLevel1->Run(this);
		}
		else if(DIM_LEVEL_1 & env.m_generation.GetDimLevelOf( _T("Global")) )
		{
			CIsLine3d lnBase=m_pDimLevel1->GetBaseLine();
			CIsLine3d lnDim = lnBase;
			if(m_pDimLevel0 && (false == env.m_extDimStyle.m_bViewLimitOn))
			{
				vector<CaDraw_TagPos*>* pvtDITEM=m_pDimLevel0->GetDITEMVector();
				if(!pvtDITEM->empty())
				{
					lnDim.Set( pvtDITEM->front()->m_ptStart , pvtDITEM->back()->m_ptStart);
					if(IsHorizontal())
					{
						lnDim.Set(CIsPoint3d(lnDim.start().x() , lnBase.start().y() , 0.0) , CIsPoint3d(lnDim.end().x() , lnBase.end().y() , 0.0));
					}
					else
					{
						lnDim.Set(CIsPoint3d(lnBase.start().x() , lnDim.start().y() , 0.0) , CIsPoint3d(lnBase.end().x() , lnDim.end().y() , 0.0));
					}
				}
			}
			
			CaDraw_TagPos* pTagPos=CSmartDrawAnnoModule::GetInstance().new_tag_pos();
			if(pTagPos)
			{
				pTagPos->type   = DIM_TAG;
				pTagPos->pTITEM = CSmartDrawAnnoModule::GetInstance().new_titem_t();
				pTagPos->pTITEM->nLevel = DIM_LEVEL_1;
				pTagPos->m_ptStart = lnDim.start();
				pTagPos->m_ptEnd   = lnDim.end();

				m_plstDimension->push_back(pTagPos);
			}
		}
		else if(NULL != m_pDimLevel2)
		{
			m_pDimLevel2->SetBaseLine(m_pDimLevel1->GetBaseLine());
			m_pDimLevel2->SetLevel(m_pDimLevel1->GetLevel());
		}
	}

	if(m_pDimLevel2)
	{
		vector<CaDraw_TagPos*>* pvtDITEM = m_pDimLevel2->GetDITEMVector();
		if(!pvtDITEM->empty())
		{
			m_pDimLevel2->Run(this);
		}
		else
		{
			const CIsLine3d lnBase = m_pDimLevel2->GetBaseLine();
			CIsLine3d lnDim = lnBase;
			if(m_pDimLevel1 && (false == env.m_extDimStyle.m_bViewLimitOn))
			{
				vector<CaDraw_TagPos*>* pvtDITEM=m_pDimLevel1->GetDITEMVector();
				if(!pvtDITEM->empty())
				{
					lnDim.Set( pvtDITEM->front()->m_ptStart , pvtDITEM->back()->m_ptStart);
					if(IsHorizontal())
					{
						lnDim.Set(CIsPoint3d(lnDim.start().x() , lnBase.start().y() , 0.0) , CIsPoint3d(lnDim.end().x() , lnBase.end().y() , 0.0));
					}
					else
					{
						lnDim.Set(CIsPoint3d(lnBase.start().x() , lnDim.start().y() , 0.0) , CIsPoint3d(lnBase.end().x() , lnDim.end().y() , 0.0));
					}
				}
			}
			
			CaDraw_TagPos* pTagPos=CSmartDrawAnnoModule::GetInstance().new_tag_pos();
			if(pTagPos)
			{
				pTagPos->type   = DIM_TAG;
				pTagPos->pTITEM = CSmartDrawAnnoModule::GetInstance().new_titem_t();
				pTagPos->pTITEM->nLevel = m_pDimLevel2->GetLevel();
				pTagPos->m_ptStart = lnDim.start();
				pTagPos->m_ptEnd   = lnDim.end();
				
				m_plstDimension->push_back(pTagPos);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	/// remove sample dimension
	if(IsHorizontal())
	{
		for(list<CaDraw_TagPos*>::iterator itr=m_plstDimension->begin();itr != m_plstDimension->end();)
		{
			if((DIM_LEVEL_0 == (*itr)->pTITEM->nLevel) &&
				DoesExistEqualDim(DIM_LEVEL_0,*itr))
			{
				itr = m_plstDimension->erase(itr);
			}
			else if((DIM_LEVEL_1 == (*itr)->pTITEM->nLevel) &&
				DoesExistEqualDim(DIM_LEVEL_1,*itr))
			{
				itr = m_plstDimension->erase(itr);
			}else	itr++;
		}
	}
	else
	{
		for(list<CaDraw_TagPos*>::iterator itr=m_plstDimension->begin();itr != m_plstDimension->end();)
		{
			if((DIM_LEVEL_0 == (*itr)->pTITEM->nLevel) &&
				DoesExistEqualDim(DIM_LEVEL_0,*itr))
			{
				itr = m_plstDimension->erase(itr);
			}
			else if((DIM_LEVEL_1 == (*itr)->pTITEM->nLevel) &&
				DoesExistEqualDim(DIM_LEVEL_1,*itr))
			{
				itr = m_plstDimension->erase(itr);
			}else	itr++;
		}
	}
}

/**	
	@brief	The CExtDivision::RunTag function
	@author	humkyung
	@date	?
	@param
	@return	void	
*/
void CExtDivision::RunTag()
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	CGenerationOption::eLocationOrder orders[3]={CGenerationOption::LOCATION_ORDER1,CGenerationOption::LOCATION_ORDER2,CGenerationOption::LOCATION_ORDER3};
	
	vector<STRING_T> aItemList;
	for(int i = 0;i < SIZE_OF_ARRAY(orders);++i)
	{
		env.m_generation.GetItemListOfLLocationOrder(aItemList , orders[i]);
		if(aItemList.empty()) continue;

		for(vector<STRING_T>::const_iterator itr = aItemList.begin();itr != aItemList.end();++itr)
		{
			if(_T("Structure") == *itr)
			{
				for(vector<CaDraw_TagPos*>::iterator itrStrTagPos=m_pvtStruTagPos->begin();itrStrTagPos != m_pvtStruTagPos->end();++itrStrTagPos)
				{
					CaDraw_TagPos* pTagPos=(*itrStrTagPos);
					if('-' == (*(*itrStrTagPos)->pTITEM->name[0])[0]) continue;
					(*itrStrTagPos)->bFixed = false;
					m_pvtTagPos->push_back(*itrStrTagPos);
				}

				if(IsHorizontal())
					stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByX);
				else	stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByY);
			}
			else if((_T("Equipment") == *itr) && env.m_equi.bTag)
			{
				for(vector<CaDraw_TagPos*>::iterator itrEqpTagPos=m_pvtEquiTagPos->begin();itrEqpTagPos != m_pvtEquiTagPos->end();++itrEqpTagPos)
				{
					(*itrEqpTagPos)->bFixed = false;
					m_pvtTagPos->push_back(*itrEqpTagPos);
				}
				if(IsHorizontal())
					stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByX);
				else	stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByY);
			}
			else if(_T("Pipe") == *itr)
			{
				for(vector<CaDraw_TagPos*>::iterator itrPipeTagPos=m_pvtPipeTagPos->begin();itrPipeTagPos != m_pvtPipeTagPos->end();++itrPipeTagPos)
				{
					m_pvtTagPos->push_back(*itrPipeTagPos);
				}
				for(vector<CaDraw_TagPos*>::iterator itrTITEMTagPos=m_pvtOtherTagPos->begin();itrTITEMTagPos != m_pvtOtherTagPos->end();++itrTITEMTagPos)
				{
					m_pvtTagPos->push_back(*itrTITEMTagPos);
				}
				
				if(IsHorizontal())
					stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByX);
				else	stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByY);

				if((env.m_pipe.m_ext.nGroupLevel > 0) && (DOUBLELINE == env.m_pipe.m_ext.eTag))
				{
					if( ONLY_ELEV == env.m_pipe.m_ext.eSecondTagType )	/// grouping은 second tag type이 ONLY_ELEV일 때만 한다.
					{
						if((ELEV_MIXIN == env.m_pipe.m_ext.eElev) || (ELEV_BOP == env.m_pipe.m_ext.eElev))
							GroupByBOP();
						else if((ELEV_TOP == env.m_pipe.m_ext.eElev))
							GroupByTOP();
						else if((ELEV_CL == env.m_pipe.m_ext.eElev))
							GroupByCL();
					}
				}
			}
		}
		ComputateSpace((*m_pvtTagPos));
		RunPipeAndEquiTag();
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			(*itr)->bFixed = true;
		}
	}
}

/**	
	@brief	search the space for tagging pipe and equipment
	@authr	BHK
	@date	?
	@param
	@return	void	
*/
void CExtDivision::RunPipeAndEquiTag()
{
	if(IsHorizontal())
	{		
		int nLoop=0,nOverlap=0;
		CountOverlap(*m_pvtTagPos);
		do
		{
			vector<CaDraw_TagPos*>::iterator itrMax;
			if(!PickupMaxOverlapObject(*m_pvtTagPos,itrMax)) break;
			
			RunLeftSideTag(*m_pvtTagPos,itrMax);
			RunRightSideTag(*m_pvtTagPos,itrMax);
					
			nOverlap=CountOverlap(*m_pvtTagPos);
			nLoop++;
		}while((nLoop < MAXLOOP) && nOverlap);
	}
	else if(IsVertical())
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		
		int nLoop=0,nOverlap=0;
		CountOverlap(*m_pvtTagPos);
		do
		{
			vector<CaDraw_TagPos*>::iterator itrMax;
			if(!PickupMaxOverlapObject(*m_pvtTagPos,itrMax)) break;
			
			RunLeftSideTag(*m_pvtTagPos,itrMax);
			RunRightSideTag(*m_pvtTagPos,itrMax);
	
			nOverlap = CountOverlap(*m_pvtTagPos);
			nLoop++;
		}while((nLoop < MAXLOOP) && nOverlap);
	}
}

/**	\brief	The CExtDivision::RunStrTag function


	\return	void	
*/
void CExtDivision::RunStrTag()
{
	if(IsHorizontal())
	{
		stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByX);
		
		int nLoop=0,nOverlap=0;
		CountOverlap(*m_pvtTagPos);
		do
		{
			int nMaxOverlap=0,nMaxCount=0;
			for(vector<CaDraw_TagPos*>::iterator itr = m_pvtTagPos->begin();itr != m_pvtTagPos->end();itr++)
			{
				if((*itr)->nOverlap > nMaxOverlap)
				{
					nMaxOverlap = (*itr)->nOverlap;
					nMaxCount   = 1;
				}
				else if((*itr)->nOverlap == nMaxOverlap)
				{
					nMaxCount++;
				}
			}
			if(!nMaxOverlap) break;

			int nMeanCount = (int)(nMaxCount*0.5);
			vector<CaDraw_TagPos*>::iterator itrMax; 
			for(itrMax = m_pvtTagPos->begin();(itrMax != m_pvtTagPos->end()) && nMeanCount;itrMax++)
			{
				if((*itrMax)->nOverlap == nMaxOverlap) nMeanCount--;
			}
			
			if((itrMax) != m_pvtTagPos->begin())
			{
				vector<CaDraw_TagPos*>::iterator itrMaxTmp=itrMax;
				while(itrMaxTmp != m_pvtTagPos->begin())
				{
					double x1=((*(itrMaxTmp-1))->m_ptEnd.x() + (*(itrMaxTmp-1))->rect.height*0.5);
					double x2=((*itrMaxTmp)->m_ptEnd.x() - (*itrMaxTmp)->rect.height*0.5);
					if(x1 > x2)
					{
						double nOverlapWidth = x1 - x2;
						(*(itrMaxTmp - 1))->m_ptEnd.SetX((*(itrMaxTmp - 1))->m_ptEnd.x() - nOverlapWidth);
					}
					itrMaxTmp--;
				}
			}
			if((itrMax+1) != m_pvtTagPos->end())
			{
				vector<CaDraw_TagPos*>::iterator itrMaxTmp=itrMax;
				while((itrMaxTmp+1) != m_pvtTagPos->end())
				{
					double y1=((*itrMaxTmp)->m_ptEnd.x() + (*itrMaxTmp)->rect.height*0.5);
					double y2=((*(itrMaxTmp+1))->m_ptEnd.x() - (*(itrMaxTmp+1))->rect.height*0.5);
					if(y1 > y2)
					{
						double nOverlapWidth = y1 - y2;
						(*(itrMaxTmp+1))->m_ptEnd.SetX((*(itrMaxTmp+1))->m_ptEnd.x() + nOverlapWidth);
					}
					itrMaxTmp++;
				}
			}
					
			nOverlap=CountOverlap(*m_pvtTagPos);
			nLoop++;
		}while((nLoop < MAXLOOP) && nOverlap);
	}
	else if(IsVertical())
	{
		stable_sort(m_pvtTagPos->begin(),m_pvtTagPos->end(),_SortByY);

		int nLoop=0,nOverlap=0;
		CountOverlap(*m_pvtTagPos);
		do
		{
			int nMaxOverlap=0,nMaxCount=0;
			for(vector<CaDraw_TagPos*>::iterator itr = m_pvtTagPos->begin();itr != m_pvtTagPos->end();itr++)
			{
				if((*itr)->nOverlap > nMaxOverlap)
				{
					nMaxOverlap = (*itr)->nOverlap;
					nMaxCount   = 1;
				}
				else if((*itr)->nOverlap == nMaxOverlap)
				{
					nMaxCount++;
				}
			}
			if(!nMaxOverlap) break;
			
			int nMeanCount = (int)(nMaxCount*0.5);
			vector<CaDraw_TagPos*>::iterator itrMax;
			for(itrMax = m_pvtTagPos->begin();(itrMax != m_pvtTagPos->end()) && nMeanCount;itrMax++)
			{
				if((*itrMax)->nOverlap == nMaxOverlap) nMeanCount--;
			}
			if((itrMax) != m_pvtTagPos->begin())
			{
				vector<CaDraw_TagPos*>::iterator itrMaxTmp=itrMax;
				while(itrMaxTmp != m_pvtTagPos->begin())
				{
					double y1=(*(itrMaxTmp-1))->m_ptEnd.y() - (*(itrMaxTmp-1))->rect.height*0.5;
					double y2=(*(itrMaxTmp))->m_ptEnd.y()   + (*(itrMaxTmp))->rect.height*0.5;
					if(y1 <= y2)
					{
						double nOverlapHeight = y2 - y1;
						(*(itrMaxTmp-1))->m_ptEnd += CIsPoint3d(0.f , nOverlapHeight , 0.f);
					}
					itrMaxTmp--;
				}
			}
			
			if((itrMax+1) != m_pvtTagPos->end())
			{
				vector<CaDraw_TagPos*>::iterator itrMaxTmp=itrMax;
				while((itrMaxTmp+1) != m_pvtTagPos->end())
				{
					double y1=(*itrMaxTmp)->m_ptEnd.y()     - (*itrMaxTmp)->rect.height*0.5;
					double y2=(*(itrMaxTmp+1))->m_ptEnd.y() + (*(itrMaxTmp+1))->rect.height*0.5;
					if(y1 <= y2)
					{
						double nOverlapHeight = y2 - y1;
						(*(itrMaxTmp+1))->m_ptEnd -= CIsPoint3d(0.f , nOverlapHeight , 0.f);
					}
					itrMaxTmp++;
				}
			}
	
			nOverlap = CountOverlap(*m_pvtTagPos);
			nLoop++;
		}while((nLoop < MAXLOOP) && nOverlap);
	}
}

/**	@brief	The CExtDivision::ArrangeTagPosWithAttr function
	@author	백흠경

	@return	void	
*/
void CExtDivision::ArrangeTagPosWithAttr()
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	if(IsHorizontal())
	{
		if((false == m_bOpen[0]) && (true == m_bOver[0]))
		{
			if(m_lnBaseLine.start().x() < m_lnBaseLine.end().x())
			{
				if(m_pvtTagPos->front()->m_ptEnd.x() < m_lnBaseLine.start().x())
				{
					m_pvtTagPos->front()->m_ptEnd.SetY(m_lnBaseLine.start().x());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();(itr+1) != m_pvtTagPos->end();itr++)
					{
						double x1=(*(itr))->m_ptEnd.x() + (*(itr))->rect.height*0.5 + (env.m_nGap)*2;
						double x2=(*(itr + 1))->m_ptEnd.x() - (*(itr + 1))->rect.height*0.5 - (env.m_nGap)*2;
						if(x2 < x1)
						{
							if((*(itr + 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetX((*(itr + 1))->m_ptEnd.x() + (*(itr + 1))->rect.height*0.5 + (*itr)->rect.height*0.5 + env.m_nGap*3);
								swap(*itr,*(itr + 1));
							}else	(*(itr + 1))->m_ptEnd += CIsPoint3d(x1 - x2 , 0.f , 0.f);
						}else	break;
					}
				}
				if((*(m_pvtTagPos->end() - 1))->m_ptEnd.x() < m_lnBaseLine.start().x())
				{
					(*(m_pvtTagPos->end() - 1))->m_ptEnd.SetX(m_lnBaseLine.start().x());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->end() - 1;itr != m_pvtTagPos->begin();itr--)
					{
						double x1=(*(itr))->m_ptEnd.x() + (*(itr))->rect.height*0.5 + env.m_nGap*2;
						double x2=(*(itr - 1))->m_ptEnd.x() - (*(itr - 1))->rect.height*0.5 - env.m_nGap*2;
						if(x2 < x1)
						{
							if((*(itr - 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetX((*(itr - 1))->m_ptEnd.x() + (*(itr - 1))->rect.height*0.5 + (*itr)->rect.height*0.5 + (env.m_nGap)*3);
								swap(*itr,*(itr - 1));
							}else	(*(itr - 1))->m_ptEnd += CIsPoint3d(x1 - x2 , 0.f , 0.f);
						}
					}
				}
			}
			else
			{
				if(m_pvtTagPos->front()->m_ptStart.x() > m_lnBaseLine.start().x())
				{
					m_pvtTagPos->front()->m_ptEnd.SetX(m_lnBaseLine.start().x());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();(itr+1) != m_pvtTagPos->end();itr++)
					{
						double x1=(*(itr))->m_ptEnd.x() - (*(itr))->rect.height*0.5 - (env.m_nGap)*2;
						double x2=(*(itr + 1))->m_ptEnd.x() + (*(itr + 1))->rect.height*0.5 + (env.m_nGap)*2;
						if(x2 > x1)
						{
							if((*(itr + 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetX((*(itr + 1))->m_ptEnd.x() - (*(itr + 1))->rect.height*0.5 - (*itr)->rect.height*0.5 - env.m_nGap*3);
								swap(*itr,*(itr + 1));
							}else	(*(itr + 1))->m_ptEnd += CIsPoint3d(x1 - x2 , 0.f , 0.f);
						}else	break;
					}
				}
				if((*(m_pvtTagPos->end() - 1))->m_ptEnd.x() > m_lnBaseLine.start().x())
				{
					(*(m_pvtTagPos->end() - 1))->m_ptEnd.SetX(m_lnBaseLine.start().x());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->end() - 1;itr != m_pvtTagPos->begin();itr--)
					{
						double x1=(*(itr))->m_ptEnd.x() - (*(itr))->rect.height*0.5 - env.m_nGap*2;
						double x2=(*(itr - 1))->m_ptEnd.x() + (*(itr - 1))->rect.height*0.5 + env.m_nGap*2;
						if(x2 > x1)
						{
							if((*(itr - 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetX((*(itr - 1))->m_ptEnd.x() - (*(itr - 1))->rect.height*0.5 - (*itr)->rect.height*0.5 - (env.m_nGap)*3);
								swap(*itr,*(itr - 1));
							}else	(*(itr - 1))->m_ptEnd += CIsPoint3d(x1 - x2 , 0.f , 0.f);
						}
					}
				}
			}
		}

		if((false == m_bOpen[1]) && (true == m_bOver[1]))
		{
			if(m_lnBaseLine.end().x() < m_lnBaseLine.start().x())
			{
				if(m_pvtTagPos->front()->m_ptEnd.x() < m_lnBaseLine.end().x())
				{
					m_pvtTagPos->front()->m_ptEnd.SetX(m_lnBaseLine.end().x());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();(itr+1) != m_pvtTagPos->end();itr++)
					{
						double x1=(*(itr))->m_ptEnd.x() + (*(itr))->rect.height*0.5 + (env.m_nGap)*2;
						double x2=(*(itr + 1))->m_ptEnd.x() - (*(itr + 1))->rect.height*0.5 - (env.m_nGap)*2;
						if(x2 < x1)
						{
							if((*(itr + 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetX((*(itr + 1))->m_ptEnd.x() + (*(itr + 1))->rect.height*0.5 + (*itr)->rect.height*0.5 + env.m_nGap*3);
								swap(*itr,*(itr + 1));
							}else	(*(itr + 1))->m_ptEnd += CIsPoint3d(x1 - x2 , 0.f , 0.f);
						}else	break;
					}
				}
				if((*(m_pvtTagPos->end() - 1))->m_ptEnd.x() < m_lnBaseLine.end().x())
				{
					(*(m_pvtTagPos->end() - 1))->m_ptEnd.SetX(m_lnBaseLine.end().x());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->end() - 1;itr != m_pvtTagPos->begin();itr--)
					{
						double x1=(*(itr))->m_ptEnd.x() + (*(itr))->rect.height*0.5 + env.m_nGap*2;
						double x2=(*(itr - 1))->m_ptEnd.x() - (*(itr - 1))->rect.height*0.5 - env.m_nGap*2;
						if(x2 < x1)
						{
							if((*(itr - 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetX((*(itr - 1))->m_ptEnd.x() + (*(itr - 1))->rect.height*0.5 + (*itr)->rect.height*0.5 + (env.m_nGap)*3);
								swap(*itr,*(itr - 1));
							}else	(*(itr - 1))->m_ptEnd += CIsPoint3d(x1 - x2 , 0.f , 0.f);
						}
					}
				}
			}
			else
			{
				if(m_pvtTagPos->front()->m_ptEnd.x() > m_lnBaseLine.end().x())
				{
					m_pvtTagPos->front()->m_ptEnd.SetX(m_lnBaseLine.end().x());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();(itr+1) != m_pvtTagPos->end();itr++)
					{
						double x1=(*(itr))->m_ptEnd.x() - (*(itr))->rect.height*0.5 - (env.m_nGap)*2;
						double x2=(*(itr + 1))->m_ptEnd.x() + (*(itr + 1))->rect.height*0.5 + (env.m_nGap)*2;
						if(x2 > x1)
						{
							if((*(itr + 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetX((*(itr + 1))->m_ptEnd.x() - (*(itr + 1))->rect.height*0.5 - (*itr)->rect.height*0.5 - env.m_nGap*3);
								swap(*itr,*(itr + 1));
							}else	(*(itr + 1))->m_ptEnd += CIsPoint3d(x1 - x2 , 0.f , 0.f);
						}else	break;
					}
				}
				if((*(m_pvtTagPos->end() - 1))->m_ptEnd.x() > m_lnBaseLine.end().x())
				{
					(*(m_pvtTagPos->end() - 1))->m_ptEnd.SetX(m_lnBaseLine.end().x());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->end() - 1;itr != m_pvtTagPos->begin();itr--)
					{
						double x1=(*(itr))->m_ptEnd.x() - (*(itr))->rect.height*0.5 - env.m_nGap*2;
						double x2=(*(itr - 1))->m_ptEnd.x() + (*(itr - 1))->rect.height*0.5 + env.m_nGap*2;
						if(x2 > x1)
						{
							if((*(itr - 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetX((*(itr - 1))->m_ptEnd.x() - (*(itr - 1))->rect.height*0.5 - (*itr)->rect.height*0.5 - (env.m_nGap)*3);
								swap(*itr,*(itr - 1));
							}else	(*(itr - 1))->m_ptEnd += CIsPoint3d(x1 - x2 , 0.f , 0.f);
						}
					}
				}
			}
		}
	}
	else if(IsVertical())
	{
		if((false == m_bOpen[0]) && (true == m_bOver[0]))
		{
			if(m_lnBaseLine.start().y() < m_lnBaseLine.end().y())
			{
				if(m_pvtTagPos->front()->m_ptEnd.y() < m_lnBaseLine.start().y())
				{
					m_pvtTagPos->front()->m_ptEnd.SetY(m_lnBaseLine.start().y());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();(itr+1) != m_pvtTagPos->end();itr++)
					{
						double y1=(*(itr))->m_ptEnd.y() + (*(itr))->rect.height*0.5 + (env.m_nGap)*2;
						double y2=(*(itr + 1))->m_ptEnd.y() - (*(itr + 1))->rect.height*0.5 - (env.m_nGap)*2;
						if(y2 < y1)
						{
							if((*(itr + 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetY((*(itr + 1))->m_ptEnd.y() + (*(itr + 1))->rect.height*0.5 + (*itr)->rect.height*0.5 + env.m_nGap*3);
								swap(*itr,*(itr + 1));
							}else	(*(itr + 1))->m_ptEnd += CIsPoint3d(0.f , y1 - y2 , 0.f);
						}else	break;
					}
				}
				if((*(m_pvtTagPos->end() - 1))->m_ptEnd.y() < m_lnBaseLine.start().y())
				{
					(*(m_pvtTagPos->end() - 1))->m_ptEnd.SetY(m_lnBaseLine.start().y());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->end() - 1;itr != m_pvtTagPos->begin();itr--)
					{
						double y1=(*(itr))->m_ptEnd.y() + (*(itr))->rect.height*0.5 + env.m_nGap*2;
						double y2=(*(itr - 1))->m_ptEnd.y() - (*(itr - 1))->rect.height*0.5 - env.m_nGap*2;
						if(y2 < y1)
						{
							if((*(itr - 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetY((*(itr - 1))->m_ptEnd.y() + (*(itr - 1))->rect.height*0.5 + (*itr)->rect.height*0.5 + (env.m_nGap)*3);
								swap(*itr,*(itr - 1));
							}else	(*(itr - 1))->m_ptEnd += CIsPoint3d(0.f , y1 - y2 , 0.f);
						}
					}
				}
			}
			else
			{
				if(m_pvtTagPos->front()->m_ptStart.y() > m_lnBaseLine.start().y())
				{
					m_pvtTagPos->front()->m_ptEnd.SetY(m_lnBaseLine.start().y());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();(itr+1) != m_pvtTagPos->end();itr++)
					{
						double y1=(*(itr))->m_ptEnd.y() - (*(itr))->rect.height*0.5 - (env.m_nGap)*2;
						double y2=(*(itr + 1))->m_ptEnd.y() + (*(itr + 1))->rect.height*0.5 + (env.m_nGap)*2;
						if(y2 > y1)
						{
							if((*(itr + 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetY((*(itr + 1))->m_ptEnd.y() - (*(itr + 1))->rect.height*0.5 - (*itr)->rect.height*0.5 - env.m_nGap*3);
								swap(*itr,*(itr + 1));
							}else	(*(itr + 1))->m_ptEnd += CIsPoint3d(0.f , y1 - y2 , 0.f);
						}else	break;
					}
				}
				if((*(m_pvtTagPos->end() - 1))->m_ptEnd.y() > m_lnBaseLine.start().y())
				{
					(*(m_pvtTagPos->end() - 1))->m_ptEnd.SetY(m_lnBaseLine.start().y());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->end() - 1;itr != m_pvtTagPos->begin();itr--)
					{
						double y1=(*(itr))->m_ptEnd.y() - (*(itr))->rect.height*0.5 - env.m_nGap*2;
						double y2=(*(itr - 1))->m_ptEnd.y() + (*(itr - 1))->rect.height*0.5 + env.m_nGap*2;
						if(y2 > y1)
						{
							if((*(itr - 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetY((*(itr - 1))->m_ptEnd.y() - (*(itr - 1))->rect.height*0.5 - (*itr)->rect.height*0.5 - (env.m_nGap)*3);
								swap(*itr,*(itr - 1));
							}else	(*(itr - 1))->m_ptEnd += CIsPoint3d(0.f , y1 - y2 , 0.f);
						}
					}
				}
			}
		}

		if((false == m_bOpen[1]) && (true == m_bOver[1]))
		{
			if(m_lnBaseLine.end().y() < m_lnBaseLine.start().y())
			{
				if(m_pvtTagPos->front()->m_ptEnd.y() < m_lnBaseLine.end().y())
				{
					m_pvtTagPos->front()->m_ptEnd.SetY(m_lnBaseLine.end().y());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();(itr+1) != m_pvtTagPos->end();itr++)
					{
						double y1=(*(itr))->m_ptEnd.y() + (*(itr))->rect.height*0.5 + (env.m_nGap)*2;
						double y2=(*(itr + 1))->m_ptEnd.y() - (*(itr + 1))->rect.height*0.5 - (env.m_nGap)*2;
						if(y2 < y1)
						{
							if((*(itr + 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetY((*(itr + 1))->m_ptEnd.y() + (*(itr + 1))->rect.height*0.5 + (*itr)->rect.height*0.5 + env.m_nGap*3);
								swap(*itr,*(itr + 1));
							}else	(*(itr + 1))->m_ptEnd += CIsPoint3d(0.f , y1 - y2 , 0.f);
						}else	break;
					}
				}
				if((*(m_pvtTagPos->end() - 1))->m_ptEnd.y() < m_lnBaseLine.end().y())
				{
					(*(m_pvtTagPos->end() - 1))->m_ptEnd.SetY(m_lnBaseLine.end().y());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->end() - 1;itr != m_pvtTagPos->begin();itr--)
					{
						double y1=(*(itr))->m_ptEnd.y() + (*(itr))->rect.height*0.5 + env.m_nGap*2;
						double y2=(*(itr - 1))->m_ptEnd.y() - (*(itr - 1))->rect.height*0.5 - env.m_nGap*2;
						if(y2 < y1)
						{
							if((*(itr - 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetY((*(itr - 1))->m_ptEnd.y() + (*(itr - 1))->rect.height*0.5 + (*itr)->rect.height*0.5 + (env.m_nGap)*3);
								swap(*itr,*(itr - 1));
							}else	(*(itr - 1))->m_ptEnd += CIsPoint3d(0.f , y1 - y2 , 0.f);
						}
					}
				}
			}
			else
			{
				if(m_pvtTagPos->front()->m_ptEnd.y() > m_lnBaseLine.end().y())
				{
					m_pvtTagPos->front()->m_ptEnd.SetY(m_lnBaseLine.end().y());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();(itr+1) != m_pvtTagPos->end();itr++)
					{
						double y1=(*(itr))->m_ptEnd.y() - (*(itr))->rect.height*0.5 - (env.m_nGap)*2;
						double y2=(*(itr + 1))->m_ptEnd.y() + (*(itr + 1))->rect.height*0.5 + (env.m_nGap)*2;
						if(y2 > y1)
						{
							if((*(itr + 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetY((*(itr + 1))->m_ptEnd.y() - (*(itr + 1))->rect.height*0.5 - (*itr)->rect.height*0.5 - env.m_nGap*3);
								swap(*itr,*(itr + 1));
							}else	(*(itr + 1))->m_ptEnd += CIsPoint3d(0.f , y1 - y2 , 0.f);
						}else	break;
					}
				}
				if((*(m_pvtTagPos->end() - 1))->m_ptEnd.y() > m_lnBaseLine.end().y())
				{
					(*(m_pvtTagPos->end() - 1))->m_ptEnd.SetY(m_lnBaseLine.end().y());

					for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->end() - 1;itr != m_pvtTagPos->begin();itr--)
					{
						double y1=(*(itr))->m_ptEnd.y() - (*(itr))->rect.height*0.5 - env.m_nGap*2;
						double y2=(*(itr - 1))->m_ptEnd.y() + (*(itr - 1))->rect.height*0.5 + env.m_nGap*2;
						if(y2 > y1)
						{
							if((*(itr - 1))->bFixed)
							{
								(*itr)->m_ptEnd.SetY((*(itr - 1))->m_ptEnd.y() - (*(itr - 1))->rect.height*0.5 - (*itr)->rect.height*0.5 - (env.m_nGap)*3);
								swap(*itr,*(itr - 1));
							}else	(*(itr - 1))->m_ptEnd += CIsPoint3d(0.f , y1 - y2 , 0.f);
						}
					}
				}
			}
		}
	}
}

/**	
	@brief	The CExtDivision::RunLeftSideTag function

	@author	BHK

	@date	?

	@param	vtTagPos	a parameter of type vector<CaDraw_TagPos*>&
	@param	itrFlag	a parameter of type vector<CaDraw_TagPos*>::iterator

	@return	void	
*/
void CExtDivision::RunLeftSideTag(vector<CaDraw_TagPos*>& vtTagPos,vector<CaDraw_TagPos*>::iterator itrFlag)
{
	vector<CaDraw_TagPos*>::iterator itr=itrFlag;
	if(IsHorizontal())
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		if((itr) != vtTagPos.begin())
		{
			vector<CaDraw_TagPos*>::iterator itrMaxTmp=itr;
			while(itrMaxTmp != vtTagPos.begin())
			{
				const double x1=((*(itrMaxTmp-1))->m_ptEnd.x() + (*(itrMaxTmp-1))->rect.height*0.5 + env.m_nGap*2);
				const double x2=((*itrMaxTmp)->m_ptEnd.x()     - (*itrMaxTmp)->rect.height*0.5     - env.m_nGap*2);
				if(x1 >= x2)
				{	/// overlap
					if((*(itrMaxTmp-1))->bFixed && !(*itrMaxTmp)->bFixed) /// occurs cross
					{
						if((*itrMaxTmp)->nNextSpace >= fabs(x1 - x2)*2)
						{
							(*itrMaxTmp)->m_ptEnd.SetX((*(itrMaxTmp-1))->m_ptEnd.x() + (*(itrMaxTmp))->rect.height*0.5 + (*(itrMaxTmp-1))->rect.height*0.5 + env.m_nGap*4);
						}
						else
						{
							(*itrMaxTmp)->m_ptEnd.SetX((*(itrMaxTmp-1))->m_ptEnd.x() - (*(itrMaxTmp))->rect.height*0.5 - (*(itrMaxTmp-1))->rect.height*0.5 - env.m_nGap*4);
							swap(*itrMaxTmp,*(itrMaxTmp - 1));
						}
					}
					else if(!(*(itrMaxTmp-1))->bFixed)
					{
						const double nOverlapWidth = x1 - x2;
						(*(itrMaxTmp-1))->m_ptEnd -= CIsPoint3d(nOverlapWidth , 0.f , 0.f);
					}
					
					if(m_lnBaseLine.start().x() < m_lnBaseLine.end().x())
					{
						if((*(itrMaxTmp-1))->m_ptEnd.x() < m_lnBaseLine.start().x()) m_bOver[0] = true;
						if((*(itrMaxTmp-1))->m_ptEnd.x() > m_lnBaseLine.end().x())   m_bOver[1] = true;
					}
					else
					{
						if((*(itrMaxTmp-1))->m_ptEnd.x() < m_lnBaseLine.end().x())   m_bOver[1] = true;
						if((*(itrMaxTmp-1))->m_ptEnd.x() > m_lnBaseLine.start().x()) m_bOver[0] = true;
					}
				}
				itrMaxTmp--;
			}
		}
	}
	else
	{	// vertical division
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		if((itr) != m_pvtTagPos->begin())
		{
			vector<CaDraw_TagPos*>::iterator itrMaxTmp=itr;
			while(itrMaxTmp != vtTagPos.begin())
			{
				const double y1=(*(itrMaxTmp-1))->m_ptEnd.y() - (*(itrMaxTmp-1))->rect.height*0.5 - env.m_nGap*2;
				const double y2=(*(itrMaxTmp))->m_ptEnd.y()   + (*(itrMaxTmp))->rect.height*0.5   + env.m_nGap*2;
				if(y1 <= y2)
				{	// overlap
					if((*(itrMaxTmp-1))->bFixed && !(*itrMaxTmp)->bFixed)
					{ /// occrus cross
						if((*itrMaxTmp)->nNextSpace >= fabs(y2 - y1)*2)
						{
							(*itrMaxTmp)->m_ptEnd.SetY((*(itrMaxTmp-1))->m_ptEnd.y() - (*(itrMaxTmp))->rect.height*0.5 - (*(itrMaxTmp-1))->rect.height*0.5 - env.m_nGap*4);
						}
						else
						{
							(*itrMaxTmp)->m_ptEnd.SetY((*(itrMaxTmp-1))->m_ptEnd.y() + (*(itrMaxTmp))->rect.height*0.5 + (*(itrMaxTmp-1))->rect.height*0.5 + env.m_nGap*4);
							swap(*itrMaxTmp,*(itrMaxTmp - 1));
						}
					}
					else if(!(*(itrMaxTmp-1))->bFixed)
					{
						double nOverlapHeight = y2 - y1;
						(*(itrMaxTmp-1))->m_ptEnd.SetY((*itrMaxTmp)->m_ptEnd.y() + (*(itrMaxTmp))->rect.height*0.5 + (*(itrMaxTmp-1))->rect.height*0.5 + env.m_nGap*4);
					}
					
					if(m_lnBaseLine.start().y() < m_lnBaseLine.end().y())
					{
						if((*(itrMaxTmp-1))->m_ptEnd.y() < m_lnBaseLine.start().y()) m_bOver[0] = true;
						if((*(itrMaxTmp-1))->m_ptEnd.y() > m_lnBaseLine.end().y())   m_bOver[1] = true;
					}
					else
					{
						if((*(itrMaxTmp-1))->m_ptEnd.y() < m_lnBaseLine.end().y())   m_bOver[1] = true;
						if((*(itrMaxTmp-1))->m_ptEnd.y() > m_lnBaseLine.start().y()) m_bOver[0] = true;
					}
				}//else	break;
				itrMaxTmp--;
			}
		}
	}
}

/**	
	@brief	The CExtDivision::RunRightSideTag function

	@author	BHK

	@date	?

	@param	vtTagPos	a parameter of type vector<CaDraw_TagPos*>&
	@param	itrFlag	a parameter of type vector<CaDraw_TagPos*>::iterator

	@return	void	
*/
void CExtDivision::RunRightSideTag(vector<CaDraw_TagPos*>& vtTagPos,vector<CaDraw_TagPos*>::iterator itrFlag)
{
	vector<CaDraw_TagPos*>::iterator itr=itrFlag;
	if(IsHorizontal())
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		if((itr+1) != vtTagPos.end())
		{
			vector<CaDraw_TagPos*>::iterator itrMaxTmp=itr;
			while((itrMaxTmp+1) != vtTagPos.end())
			{
				const double x1=((*itrMaxTmp)->m_ptEnd.x()     + (*itrMaxTmp)->rect.height*0.5     + env.m_nGap*2);
				const double x2=((*(itrMaxTmp+1))->m_ptEnd.x() - (*(itrMaxTmp+1))->rect.height*0.5 - env.m_nGap*2);
				if(x1 >= x2)
				{	/// overlap
					if((*(itrMaxTmp+1))->bFixed && !(*itrMaxTmp)->bFixed)  // occurs cross
					{
						if((itrMaxTmp == vtTagPos.begin()) || ((*itrMaxTmp)->nPrevSpace >= fabs(x2 - x1)*2))
						{
							(*(itrMaxTmp))->m_ptEnd.SetX((*(itrMaxTmp+1))->m_ptEnd.x() - (*(itrMaxTmp+1))->rect.height*0.5 - (*(itrMaxTmp))->rect.height*0.5 - env.m_nGap*4);
						}
						else
						{
							(*(itrMaxTmp))->m_ptEnd.SetX((*(itrMaxTmp+1))->m_ptEnd.x() + (*(itrMaxTmp))->rect.height*0.5 + (*(itrMaxTmp+1))->rect.height*0.5 + env.m_nGap*4);
							swap(*itrMaxTmp,*(itrMaxTmp + 1));
						}
					}
					else if(!(*(itrMaxTmp+1))->bFixed)
					{
						double nOverlapWidth = x1 - x2;
						(*(itrMaxTmp+1))->m_ptEnd += CIsPoint3d(nOverlapWidth , 0.f , 0.f);
					}
					
					if(m_lnBaseLine.start().x() < m_lnBaseLine.end().x())
					{
						if((*(itrMaxTmp+1))->m_ptEnd.x() < m_lnBaseLine.start().x()) m_bOver[0] = true;
						if((*(itrMaxTmp+1))->m_ptEnd.x() > m_lnBaseLine.end().x())   m_bOver[1] = true;
					}
					else
					{
						if((*(itrMaxTmp+1))->m_ptEnd.x() < m_lnBaseLine.end().x())   m_bOver[1] = true;
						if((*(itrMaxTmp+1))->m_ptEnd.x() > m_lnBaseLine.start().x()) m_bOver[0] = true;
					}
				}//else	break;
				itrMaxTmp++;
			}
		}
	}
	else
	{	// vertical division
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		if((itr+1) != vtTagPos.end())
		{
			vector<CaDraw_TagPos*>::iterator itrMaxTmp=itr;
			while((itrMaxTmp+1) != vtTagPos.end())
			{
				const double y1=(*itrMaxTmp)->m_ptEnd.y()     - (*itrMaxTmp)->rect.height*0.5 - (env.m_nGap)*2;
				const double y2=(*(itrMaxTmp+1))->m_ptEnd.y() + (*(itrMaxTmp+1))->rect.height*0.5 + (env.m_nGap)*2;
				if(y1 <= y2)
				{ /// overlap
					if((*(itrMaxTmp+1))->bFixed && !(*itrMaxTmp)->bFixed)
					{ /// occurs cross
						if((itrMaxTmp == vtTagPos.begin()) || ((*itrMaxTmp)->nPrevSpace >= fabs(y2 - y1)*2))
						{
							(*(itrMaxTmp))->m_ptEnd.SetY((*(itrMaxTmp + 1))->m_ptEnd.y() + (*(itrMaxTmp))->rect.height*0.5 +
								(*(itrMaxTmp+1))->rect.height*0.5 + (env.m_nGap)*4);
							
						}
						else if((y2 - y1) > env.m_nGap*2)
						{
							(*itrMaxTmp)->m_ptEnd.SetY((*(itrMaxTmp+1))->m_ptEnd.y() - (*(itrMaxTmp))->rect.height*0.5 - (*(itrMaxTmp+1))->rect.height*0.5 - (env.m_nGap)*4);
							swap(*itrMaxTmp,*(itrMaxTmp + 1));
						}
						else
						{
							(*itrMaxTmp)->m_ptEnd += CIsPoint3d(0.f , (y2 - y1) , 0.f);
						}
					}
					else if(!(*(itrMaxTmp+1))->bFixed)
					{
						double nOverlapHeight = y2 - y1;
						(*(itrMaxTmp+1))->m_ptEnd.SetY((*itrMaxTmp)->m_ptEnd.y() - (*(itrMaxTmp))->rect.height*0.5 - (*(itrMaxTmp+1))->rect.height*0.5 - (env.m_nGap)*4);
					}
					
					if(m_lnBaseLine.start().y() < m_lnBaseLine.end().y())
					{
						if((*(itrMaxTmp+1))->m_ptEnd.y() < m_lnBaseLine.start().y()) m_bOver[0] = true;
						if((*(itrMaxTmp+1))->m_ptEnd.y() > m_lnBaseLine.end().y())   m_bOver[1] = true;
					}
					else
					{
						if((*(itrMaxTmp+1))->m_ptEnd.y() < m_lnBaseLine.end().y())   m_bOver[1] = true;
						if((*(itrMaxTmp+1))->m_ptEnd.y() > m_lnBaseLine.start().y()) m_bOver[0] = true;
					}
				}//else	break;
				itrMaxTmp++;
			}
		}
	}
}

/**	
	@brief	pick up the tag item which is most overlapped.

	@author	BHK

	@date	?

	@param	vtTagPos	a parameter of type vector<CaDraw_TagPos*>&

	@return	int	
*/
int  CExtDivision::CountOverlap(vector<CaDraw_TagPos*>& vtTagPos)
{
	vector<CaDraw_TagPos*>::iterator itr;
	for(itr=vtTagPos.begin();itr != vtTagPos.end();++itr)
	{
		(*itr)->nOverlap   = 0;
		(*itr)->nPrevSpace = 0;
		(*itr)->nNextSpace = 0;
	}
	
	int nOverlap=0;
	if(IsHorizontal())
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		for(itr = vtTagPos.begin();itr != vtTagPos.end();itr++)
		{
			const double x1=(*itr)->m_ptEnd.x() + ((*itr)->rect.height)*0.5 + env.m_nGap*2;
			for(vector<CaDraw_TagPos*>::iterator itrNext = itr+1;itrNext != vtTagPos.end();itrNext++)
			{
				const double x2=(*itrNext)->m_ptEnd.x() - ((*itrNext)->rect.height)*0.5 - env.m_nGap*2;
				if(x1 > x2)
				{
					((*itr)->nOverlap)++;
					((*itrNext)->nOverlap)++;
					((*itr)->nNextSpace)    = -1;
					((*itrNext)->nPrevSpace)= -1;
					nOverlap++;
				}
				else
				{
					if(-1 != ((*itr)->nNextSpace)) ((*itr)->nNextSpace)    = x2 - x1;
					if(-1 != ((*itrNext)->nPrevSpace)) ((*itrNext)->nPrevSpace)= x2 - x1;
					break;
				}
			}
		}
	}
	else if(IsVertical())
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		for(itr = vtTagPos.begin();itr != vtTagPos.end();itr++)
		{
			const double y1=(*itr)->m_ptEnd.y() - ((*itr)->rect.height)*0.5 - env.m_nGap*2;
			for(vector<CaDraw_TagPos*>::iterator itrNext = itr+1;itrNext != vtTagPos.end();itrNext++)
			{
				const double y2=(*itrNext)->m_ptEnd.y() + ((*itrNext)->rect.height)*0.5 + env.m_nGap*2;
				if(y1 < y2)
				{
					((*itr)->nOverlap)++;
					((*itrNext)->nOverlap)++;
					((*itr)->nNextSpace)    = -1;//y1 - y2;
					((*itrNext)->nPrevSpace)= -1;//y1 - y2;
					nOverlap++;
				}else{
					if(-1 != ((*itr)->nNextSpace)) ((*itr)->nNextSpace)    = y1 - y2;
					if(-1 != ((*itrNext)->nPrevSpace)) ((*itrNext)->nPrevSpace)= y1 - y2;
					break;
				}
			}
		}
	}

	return nOverlap;
}

/**	
	@brief	The CExtDivision::ComputateSpace function
	@author	BHK
	@date	?
	@param	vtTagPos	a parameter of type vector<CaDraw_TagPos*>&
	@return	void	
*/
void CExtDivision::ComputateSpace(vector<CaDraw_TagPos*>& vtTagPos)
{
	for(vector<CaDraw_TagPos*>::iterator itr =vtTagPos.begin();itr != vtTagPos.end();++itr)
	{
		(*itr)->nOverlap = 0;
		(*itr)->nPrevSpace = DBL_MAX;
		(*itr)->nNextSpace = DBL_MAX;
	}
	
	int nOverlap=0;
	if(IsHorizontal() && !vtTagPos.empty())
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		for(vector<CaDraw_TagPos*>::iterator itr = vtTagPos.begin();itr != vtTagPos.end();itr++)
		{
			double x1=(*itr)->m_ptEnd.x() + (*itr)->rect.height*0.5 + env.m_nGap*2;
			for(vector<CaDraw_TagPos*>::iterator itrNext = itr+1;itrNext != vtTagPos.end();itrNext++)
			{
				double x2=(*itrNext)->m_ptEnd.x() - (*itrNext)->rect.height*0.5 - env.m_nGap*2;
				if(x1 > x2)
				{
					/// find overlap, and increase overlap count.
					((*itr)->nOverlap)++;
					((*itrNext)->nOverlap)++;
					((*itr)->nNextSpace)    = 0;
					((*itrNext)->nPrevSpace)= 0;
					nOverlap++;
				}
				else
				{
					((*itr)->nNextSpace)    = x2 - x1;
					((*itrNext)->nPrevSpace)= x2 - x1;
					break;
				}
			}
		}
	}
	else if(IsVertical() && !vtTagPos.empty())
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		for(vector<CaDraw_TagPos*>::iterator itr = vtTagPos.begin();(itr+1) != vtTagPos.end();itr++)
		{
			double y1=(*itr)->m_ptEnd.y() - (*itr)->rect.height*0.5 - env.m_nGap*2;
			double y2=(*(itr+1))->m_ptEnd.y() + (*(itr+1))->rect.height*0.5 + env.m_nGap*2;
			if(y1 < y2)
			{
				((*itr)->nNextSpace)    = y1 - y2;
				((*(itr+1))->nPrevSpace)= y1 - y2;
			}else{
				((*itr)->nNextSpace)    = y1 - y2;
				((*(itr+1))->nPrevSpace)= y1 - y2;
			}
		}
	}
}

/**	
	@brief	pick the most overlapped object

	@author	BHK

	@date	?

	@param	vtTagPos	a parameter of type vector<CaDraw_TagPos*>&
	@param	itr	a parameter of type vector<CaDraw_TagPos*>::iterator&

	@return	bool	
*/
bool CExtDivision::PickupMaxOverlapObject(vector<CaDraw_TagPos*>& vtTagPos,vector<CaDraw_TagPos*>::iterator& itr)
{
	bool bRet=false;

	int nMaxOverlap[3]={-1,-1,-1},nMaxCount[3]={0,0,0};
	vector<CaDraw_TagPos*>::iterator itrMax[3];
	itrMax[0] = itrMax[1] = itrMax[2] = vtTagPos.begin();
	for(itr = vtTagPos.begin();itr != vtTagPos.end();itr++)
	{
		if((*itr)->nOverlap <= 0) continue;
		if((NULL == (*itr)->pTITEM) || ((DRAFT_PIPE == (*itr)->pTITEM->enmType) && (DOUBLE_TAG == (*itr)->type)))
		{
			if((*itr)->nOverlap > nMaxOverlap[0])
			{
				nMaxOverlap[0] = (*itr)->nOverlap;
				nMaxCount[0]   = 1;
				itrMax[0]      = itr;
			}
			else if((*itr)->nOverlap == nMaxOverlap[0])
			{
				nMaxCount[0]++;
			}
		}
		else if((DRAFT_PIPE == (*itr)->pTITEM->enmType) && (SINGLE_TAG == (*itr)->type))
		{
			if((*itr)->nOverlap > nMaxOverlap[1])
			{
				nMaxOverlap[1] = (*itr)->nOverlap;
				nMaxCount[1]   = 1;
				itrMax[1]      = itr;
			}
			else if((*itr)->nOverlap == nMaxOverlap[1])
			{
				nMaxCount[1]++;
			}
		}
		else if((DRAFT_EQP == (*itr)->pTITEM->enmType) || (DRAFT_STR == (*itr)->pTITEM->enmType))
		{
			if((*itr)->nOverlap > nMaxOverlap[2])
			{
				nMaxOverlap[2] = (*itr)->nOverlap;
				nMaxCount[2]   = 1;
				itrMax[2]      = itr;
			}
			else if((*itr)->nOverlap == nMaxOverlap[2])
			{
				nMaxCount[2]++;
			}
		}
	}
	if(!nMaxOverlap[0] && !nMaxOverlap[1] && !nMaxOverlap[2]) return bRet;
	
	if(nMaxOverlap[2] > 0)
	{
		int nMeanCount = (int)(nMaxCount[2]*0.5);
		for(itr = itrMax[2];(itr != vtTagPos.end()) && nMeanCount;itr++)
		{
			if(((DRAFT_EQP == (*itr)->pTITEM->enmType) || (DRAFT_STR == (*itr)->pTITEM->enmType)) && ((*itr)->nOverlap == nMaxOverlap[2])) nMeanCount--;
		}
		bRet = true;
	}
	else if(nMaxOverlap[1] > 0)
	{
		int nMeanCount = (int)(nMaxCount[1]*0.5);
		for(itr = itrMax[1];(itr != vtTagPos.end()) && nMeanCount;itr++)
		{
			if(	(DRAFT_PIPE == (*itr)->pTITEM->enmType) && (SINGLE_TAG == (*itr)->type) &&
				((*itr)->nOverlap == nMaxOverlap[1]))
				nMeanCount--;
		}
		bRet = true;
	}
	else if(nMaxOverlap[0] > 0)
	{
		int nMeanCount = (int)(nMaxCount[0]*0.5);
		for(itr = itrMax[0];(itr != vtTagPos.end()) && nMeanCount;itr++)
		{
			if((NULL == (*itr)->pTITEM) || ((DRAFT_PIPE == (*itr)->pTITEM->enmType) && ((*itr)->nOverlap == nMaxOverlap[0]))) nMeanCount--;
		}
		bRet = true;
	}

	return bRet;
}

/**	
	@brief	The CExtDivision::IsHorizontal function
	@author	humkyung
	@date	?
	@param	
	@return	bool	
*/
bool CExtDivision::IsHorizontal()
{
	double dy=m_lnBaseLine.start().y() - m_lnBaseLine.end().y();
	
	return (fabs(dy) < 0.001);
}

/**	
	@brief	The CExtDivision::IsVertical function
	@author	humkyung
	@date	?
	@param
	@return	bool	
*/
bool CExtDivision::IsVertical()
{
	double dx=m_lnBaseLine.start().x() - m_lnBaseLine.end().x();
	
	return (fabs(dx) < 0.001);
}

/**	\brief	The CExtDivision::IsCrossingXRay function

	\param	pt	a parameter of type const CIsPoint3d&

	\remarks	;2004.06.10 - check division direction
	\return	bool	
*/
bool CExtDivision::IsCrossingXRay(const CIsPoint3d& pt)
{
	double dy=m_lnBaseLine.end().y() - m_lnBaseLine.start().y();
	if(fabs(dy) < 0.001) return false;
	return ((m_lnBaseLine.start().y() - pt.y())*(m_lnBaseLine.end().y() - pt.y()) <= 0.);
}

/**	\brief	The CExtDivision::IsCrossingYRay function

	\param	pt	a parameter of type const CIsPoint3d&
	
	\remarks	;2004.06.10 - check division direction
	
	\return	bool	
*/
bool CExtDivision::IsCrossingYRay(const CIsPoint3d& pt)
{
	double dx=m_lnBaseLine.end().x() - m_lnBaseLine.start().x();
	if(fabs(dx) < 0.001) return false;
	return ((m_lnBaseLine.start().x() - pt.x())*(m_lnBaseLine.end().x() - pt.x()) <= 0.);
}

/**	\brief	The CExtDivision::GetXDistance function

	\param	pt	a parameter of type const CIsPoint3d&
	\param	bSign	a parameter of type const bool&

	\return	double	
*/
double CExtDivision::GetXDistance(const CIsPoint3d& pt,const bool& bSign)
{
	return (bSign) ? m_lnBaseLine.start().x() - pt.x() : fabs(m_lnBaseLine.start().x() - pt.x());
}

/**	\brief	The CExtDivision::GetYDistance function

	\param	pt	a parameter of type const CIsPoint3d&
	\param	bSign	a parameter of type const bool&

	\return	double	
*/
double CExtDivision::GetYDistance(const CIsPoint3d& pt,const bool& bSign)
{
	return (bSign) ? m_lnBaseLine.start().y() - pt.y() : fabs(m_lnBaseLine.start().y() - pt.y());
}

#define	GROUP_ITEM(item)\
	(!((VERTICAL == (item)->pTITEM->enmSubType) || (SLOPE == (item)->pTITEM->enmSubType)))

//	parameter	:
//	description	: unique each of contents
//	remarks		: end of pSrc must be NULL.
//	returns		:
template<class T>
T* ListPush(T* pDest,T* pSrc)
{
	assert(pSrc && "pSrc is NULL");

	if(pSrc)
	{
		if(pDest)
		{
			T* ptr = pDest;
			for(ptr = pDest;ptr && ptr->next;ptr = ptr->next)
			{
				assert(ptr != pSrc);
			}
			ptr->next = pSrc;
			//pSrc->prev= ptr;
			//pSrc->next= NULL;

			return pDest;
		}
		else
		{
			pDest = pSrc;
			//pDest->prev = pDest->next = NULL;

			return pSrc;
		}
	}

	return pDest;
}

/**	@brief	group pipes which have same BOP in conditionally.
	and increase group count.

	@remarks
	;2003.03.14 - set MAX_GROUP_ITEM_COUNT.
	which limits count of item group can get in

	@return	void	
*/
void CExtDivision::GroupByBOP()
{
	static bool _bIncreaseGroup=true;

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	int nGroupItemCount=0;
	if(1 == env.m_pipe.m_ext.nGroupLevel)
	{
		vector<CaDraw_TagPos*>::iterator itrNext;
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			if((DRAFT_PIPE == (*itr)->pTITEM->enmType) && GROUP_ITEM(*itr))
			{
				for(itrNext=itr+1;itrNext != m_pvtTagPos->end();++itrNext)
				{
					if((DRAFT_PIPE == (*itrNext)->pTITEM->enmType) && GROUP_ITEM(*itrNext))
					{
						if(OverlapCount(*itr,*itrNext) > 0.)
						{
							/// GROUP
							if((*itr)->pTITEM->nBOP == (*itrNext)->pTITEM->nBOP)
							{
								if(nGroupItemCount < MAX_GROUP_ITEM_COUNT)
								{
									(*itr)->rect.height *= 0.5;
									(*itr)->type         = SINGLE_TAG;
									if(IsHorizontal())
									{
										(*itr)->m_ptEnd -= CIsPoint3d((*itr)->rect.height*0.5 , 0.f , 0.f);
									}
									else
									{
										(*itr)->m_ptEnd += CIsPoint3d(0.f , (*itr)->rect.height*0.5 , 0.f);
									}
									
									if((*itr)->next)
									{
										(*itr)->gcode = GROUP_IN;
										(*itrNext)->next = ListPush((*itrNext)->next,(*itr)->next);
									}
									else
									{
										(*itr)->gcode = GROUP_START;
										m_nGroupCount++;
									}
									(*itr)->next = NULL;
									(*itrNext)->next = ListPush((*itrNext)->next,*itr);
									(*itrNext)->gcode = GROUP_END;
									(*itrNext)->nGroup= m_nGroupCount;
									for(CaDraw_TagPos* ptr=(*itrNext);ptr;ptr = ptr->next) ptr->nGroup = m_nGroupCount;
									nGroupItemCount++;
									break;
								}
								else
								{
									nGroupItemCount = 0;
									break;
								}
							}else	break;
						}else	break;
					}
				}
			}
		}
	}
	else if(2 == env.m_pipe.m_ext.nGroupLevel)
	{
		vector<CaDraw_TagPos*>::iterator itrNext;
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			if((DRAFT_PIPE == (*itr)->pTITEM->enmType) && GROUP_ITEM(*itr))
			{
				for(itrNext=itr+1;itrNext != m_pvtTagPos->end();++itrNext)
				{
					if((DRAFT_PIPE == (*itrNext)->pTITEM->enmType) && GROUP_ITEM(*itrNext))
					{
						if(OverlapCount(*itr,*itrNext) > -(*itrNext)->rect.height)
						{
							// GROUP
							double nBop1=(*itr)->pTITEM->nBOP,nBop2=(*itrNext)->pTITEM->nBOP;
							if((*itr)->pTITEM->nBOP == (*itrNext)->pTITEM->nBOP)
							{
								if(nGroupItemCount < MAX_GROUP_ITEM_COUNT)
								{
									(*itr)->rect.height *= 0.5;
									(*itr)->type         = SINGLE_TAG;
									if(IsHorizontal())
									{
										(*itr)->m_ptEnd -= CIsPoint3d((*itr)->rect.height*0.5 , 0.f , 0.f);
									}
									else
									{
										(*itr)->m_ptEnd += CIsPoint3d(0.f , (*itr)->rect.height*0.5 , 0.f);
									}
									
									if((*itr)->next)
									{
										(*itr)->gcode = GROUP_IN;
										(*itrNext)->next = ListPush((*itrNext)->next,(*itr)->next);
									}
									else
									{
										(*itr)->gcode = GROUP_START;
										m_nGroupCount++;
									}
									(*itr)->next = NULL;
									(*itrNext)->next = ListPush((*itrNext)->next,*itr);
									(*itrNext)->gcode = GROUP_END;
									(*itrNext)->nGroup= m_nGroupCount;
									for(CaDraw_TagPos* ptr=(*itrNext);ptr;ptr = ptr->next) ptr->nGroup = m_nGroupCount;
									nGroupItemCount++;
									break;
								}
								else
								{
									nGroupItemCount = 0;
									break;
								}
							}else	break;
						}else	break;
					}
				}
			}
		}
	}
	else if(3 == env.m_pipe.m_ext.nGroupLevel)
	{
		vector<CaDraw_TagPos*>::iterator itrNext;
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			if((DRAFT_PIPE == (*itr)->pTITEM->enmType) && GROUP_ITEM(*itr))
			{
				for(itrNext=itr+1;itrNext != m_pvtTagPos->end();++itrNext)
				{
					if((DRAFT_PIPE == (*itrNext)->pTITEM->enmType) && GROUP_ITEM(*itrNext))
					{
						if(OverlapCount(*itr,*itrNext) > -(*itrNext)->rect.height*2)
						{
							// GROUP
							if((*itr)->pTITEM->nBOP == (*itrNext)->pTITEM->nBOP)
							{
								if(nGroupItemCount < MAX_GROUP_ITEM_COUNT)
								{
									(*itr)->rect.height *= 0.5;
									(*itr)->type         = SINGLE_TAG;
									if(IsHorizontal())
									{
										(*itr)->m_ptEnd -= CIsPoint3d((*itr)->rect.height*0.5 , 0.f , 0.f);
									}
									else
									{
										(*itr)->m_ptEnd += CIsPoint3d(0.f , (*itr)->rect.height*0.5 , 0.f);
									}
									
									if((*itr)->next)
									{
										(*itr)->gcode = GROUP_IN;
										(*itrNext)->next = ListPush((*itrNext)->next,(*itr)->next);
									}
									else
									{
										(*itr)->gcode = GROUP_START;
										m_nGroupCount++;
									}
									(*itr)->next = NULL;
									(*itrNext)->next = ListPush((*itrNext)->next,*itr);
									(*itrNext)->gcode = GROUP_END;
									(*itrNext)->nGroup= m_nGroupCount;
									for(CaDraw_TagPos* ptr=(*itrNext);ptr;ptr = ptr->next) ptr->nGroup = m_nGroupCount;
									nGroupItemCount++;
									break;
								}
								else
								{
									nGroupItemCount = 0;
									break;
								}
							}else	break;
						}else	break;
					}
				}
			}
		}
	}

	// arrange
	if(IsHorizontal())
	{
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			for(vector<CaDraw_TagPos*>::iterator itrNext = itr+1;itrNext != m_pvtTagPos->end();itrNext++)
			{
				if((*itr)->m_ptEnd.x() > (*itrNext)->m_ptEnd.x())
				{
					if((*itrNext)->bFixed)
					{
						(*itr)->m_ptEnd.SetX((*(itrNext))->m_ptEnd.x() + (*(itrNext))->rect.height);
						
						CaDraw_TagPos* ptr =*(itr);
						*(itr)=(*itrNext);
						*itrNext    =ptr;
					}
					else
					{
						(*(itrNext))->m_ptEnd += CIsPoint3d((*itr)->m_ptEnd.x() - (*itrNext)->m_ptEnd.x() + (*itr)->rect.height*0.5 , 0.f , 0.f);
					}
				}else	break;
			}
		}
	}
	else if(IsVertical())
	{
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			for(vector<CaDraw_TagPos*>::iterator itrNext = itr+1;itrNext != m_pvtTagPos->end();itrNext++)
			{
				if((*itr)->m_ptEnd.y() < (*itrNext)->m_ptEnd.y())
				{
					if((*itrNext)->bFixed)
					{
						(*itr)->m_ptEnd.SetY((*(itrNext))->m_ptEnd.y() - (*(itrNext))->rect.height*0.5);
						
						CaDraw_TagPos* ptr =*(itr);
						*(itr)=(*itrNext);
						*itrNext    =ptr;
					}
					else
					{
						(*(itrNext))->m_ptEnd += CIsPoint3d(0.f , (*itr)->m_ptEnd.y() - (*itrNext)->m_ptEnd.y() - (*itr)->rect.height*0.5 , 0.f);
					}
				}else	break;
			}
		}
	}
}

/**	@brief	group pipes which have same BOP in conditionally.\n
	and increase group count.

	@remarks
	;2003.03.14 - set MAX_GROUP_ITEM_COUNT.
	which limits count of item group can get in

	@return	void	
*/
void CExtDivision::GroupByTOP()
{
	static bool _bIncreaseGroup=true;

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	
	int nGroupItemCount=0;
	if(1 == env.m_pipe.m_ext.nGroupLevel)
	{
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			if(((*itr)->pTITEM->m_pEnt->IsKindOf(CaDraw_TagPipe::TypeString())) && GROUP_ITEM(*itr))
			{
				for(vector<CaDraw_TagPos*>::iterator itrNext=itr+1;itrNext != m_pvtTagPos->end();++itrNext)
				{
					if(((*itrNext)->pTITEM->m_pEnt->IsKindOf(CaDraw_TagPipe::TypeString())) && GROUP_ITEM(*itrNext))
					{
						if(OverlapCount(*itr,*itrNext) > 0.)
						{
							if((*itr)->pTITEM->nTOP == (*itrNext)->pTITEM->nTOP)
							{
								if(nGroupItemCount < MAX_GROUP_ITEM_COUNT)
								{
									(*itr)->rect.height *= 0.5;
									(*itr)->type         = SINGLE_TAG;
									if(IsHorizontal())
									{
										(*itr)->m_ptEnd -= CIsPoint3d((*itr)->rect.height*0.5 , 0.f , 0.f);
									}
									else
									{
										(*itr)->m_ptEnd += CIsPoint3d(0.f , (*itr)->rect.height*0.5 , 0.f);
									}
									
									if((*itr)->next)
									{
										(*itr)->gcode = GROUP_IN;
										(*itrNext)->next = ListPush((*itrNext)->next,(*itr)->next);
									}
									else
									{
										(*itr)->gcode = GROUP_START;
										m_nGroupCount++;
									}
									(*itr)->next = NULL;
									(*itrNext)->next = ListPush((*itrNext)->next,*itr);
									(*itrNext)->gcode = GROUP_END;
									(*itrNext)->nGroup= m_nGroupCount;
									for(CaDraw_TagPos* ptr=(*itrNext);ptr;ptr = ptr->next) ptr->nGroup = m_nGroupCount;
									nGroupItemCount++;
									break;
								}
								else
								{
									nGroupItemCount = 0;
									break;
								}
							}else	break;
						}else	break;
					}
				}
			}
		}
	}
	else if(2 == env.m_pipe.m_ext.nGroupLevel)
	{
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			if((DRAFT_PIPE == (*itr)->pTITEM->enmType) && GROUP_ITEM(*itr))
			{
				for(vector<CaDraw_TagPos*>::iterator itrNext=itr+1;itrNext != m_pvtTagPos->end();++itrNext)
				{
					if((DRAFT_PIPE == (*itrNext)->pTITEM->enmType) && GROUP_ITEM(*itrNext))
					{
						if(OverlapCount(*itr,*itrNext) > -(*itrNext)->rect.height)
						{
							if((*itr)->pTITEM->nTOP == (*itrNext)->pTITEM->nTOP)
							{
								if(nGroupItemCount < MAX_GROUP_ITEM_COUNT)
								{
									(*itr)->rect.height *= 0.5;
									(*itr)->type         = SINGLE_TAG;
									if(IsHorizontal())
									{
										(*itr)->m_ptEnd -= CIsPoint3d((*itr)->rect.height*0.5 , 0.f , 0.f);
									}
									else
									{
										(*itr)->m_ptEnd += CIsPoint3d(0.f , (*itr)->rect.height*0.5 , 0.f);
									}
									
									if((*itr)->next)
									{
										(*itr)->gcode = GROUP_IN;
										(*itrNext)->next = ListPush((*itrNext)->next,(*itr)->next);
									}
									else
									{
										(*itr)->gcode = GROUP_START;
										m_nGroupCount++;
									}
									(*itr)->next = NULL;
									(*itrNext)->next = ListPush((*itrNext)->next,*itr);
									(*itrNext)->gcode = GROUP_END;
									(*itrNext)->nGroup= m_nGroupCount;
									for(CaDraw_TagPos* ptr=(*itrNext);ptr;ptr = ptr->next) ptr->nGroup = m_nGroupCount;
									nGroupItemCount++;
									break;
								}
								else
								{
									nGroupItemCount = 0;
									break;
								}
							}else	break;
						}else	break;
					}
				}
			}
		}
	}
	else if(3 == env.m_pipe.m_ext.nGroupLevel)
	{
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			if((DRAFT_PIPE == (*itr)->pTITEM->enmType) && GROUP_ITEM(*itr))
			{
				for(vector<CaDraw_TagPos*>::iterator itrNext=itr+1;itrNext != m_pvtTagPos->end();++itrNext)
				{
					if((DRAFT_PIPE == (*itrNext)->pTITEM->enmType) && GROUP_ITEM(*itrNext))
					{
						if(OverlapCount(*itr,*itrNext) > -(*itrNext)->rect.height*2)
						{
							if((*itr)->pTITEM->nTOP == (*itrNext)->pTITEM->nTOP)
							{
								if(nGroupItemCount < MAX_GROUP_ITEM_COUNT)
								{
									(*itr)->rect.height *= 0.5;
									(*itr)->type         = SINGLE_TAG;
									(*itr)->nGroup = m_nGroupCount;

									if(IsHorizontal())
									{
										(*itr)->m_ptEnd.x()     -= (*itr)->rect.height*0.5;
									}
									else
									{
										(*itr)->m_ptEnd.y() += (*itr)->rect.height*0.5;
									}
									
									if((*itr)->next)
									{
										(*itr)->gcode = GROUP_IN;
										(*itrNext)->next = ListPush((*itrNext)->next,(*itr)->next);
									}
									else
									{
										(*itr)->gcode = GROUP_START;
										m_nGroupCount++;
									}
									(*itr)->next = NULL;
									(*itrNext)->next  = ListPush((*itrNext)->next,*itr);
									(*itrNext)->gcode = GROUP_END;
									(*itrNext)->nGroup= m_nGroupCount;
									for(CaDraw_TagPos* ptr=(*itrNext);ptr;ptr = ptr->next) ptr->nGroup = m_nGroupCount;
									nGroupItemCount++;
									break;
								}
								else
								{
									nGroupItemCount = 0;
									break;
								}
							}else	break;
						}else	break;
					}
				}
			}
		}
	}

	// arrange
	if(IsHorizontal())
	{
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			for(vector<CaDraw_TagPos*>::iterator itrNext = itr+1;itrNext != m_pvtTagPos->end();itrNext++)
			{
				if((*itr)->m_ptEnd.x() > (*itrNext)->m_ptEnd.x())
				{
					if((*itrNext)->bFixed)
					{
						(*itr)->m_ptEnd.x() = (*(itrNext))->m_ptEnd.x() + (*(itrNext))->rect.height;
						
						CaDraw_TagPos* ptr =*(itr);
						*(itr)=(*itrNext);
						*itrNext    =ptr;
					}
					else
					{
						(*(itrNext))->m_ptEnd.x() += (*itr)->m_ptEnd.x() - (*itrNext)->m_ptEnd.x() + (*itr)->rect.height*0.5;
					}
				}else	break;
			}
		}
	}
	else if(IsVertical())
	{
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			for(vector<CaDraw_TagPos*>::iterator itrNext = itr+1;itrNext != m_pvtTagPos->end();itrNext++)
			{
				if((*itr)->m_ptEnd.y() < (*itrNext)->m_ptEnd.y())
				{
					if((*itrNext)->bFixed)
					{
						(*itr)->m_ptEnd.y() = (*(itrNext))->m_ptEnd.y() - (*(itrNext))->rect.height*0.5;
						
						CaDraw_TagPos* ptr =*(itr);
						*(itr)=(*itrNext);
						*itrNext    =ptr;
					}
					else
					{
						(*(itrNext))->m_ptEnd.y() += (*itr)->m_ptEnd.y() - (*itrNext)->m_ptEnd.y() - (*itr)->rect.height*0.5;
					}
				}else	break;
			}
		}
	}
}

/**	@brief	group pipes which have same CL in conditionally.
	and increase group count.

	@remarks
	;2003.03.14 - set MAX_GROUP_ITEM_COUNT.\n
	which limits count of item group can get in

	@return	void	
*/
void CExtDivision::GroupByCL()
{
	static bool _bIncreaseGroup=true;

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	
	int nGroupItemCount=0;
	if(1 == env.m_pipe.m_ext.nGroupLevel)
	{
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			if((DRAFT_PIPE == (*itr)->pTITEM->enmType) && GROUP_ITEM(*itr))
			{
				for(vector<CaDraw_TagPos*>::iterator itrNext=itr+1;itrNext != m_pvtTagPos->end();++itrNext)
				{
					if((DRAFT_PIPE == (*itrNext)->pTITEM->enmType) && GROUP_ITEM(*itrNext))
					{
						if(OverlapCount(*itr,*itrNext) > 0.)
						{
							if((*itr)->pTITEM->nCL == (*itrNext)->pTITEM->nCL)
							{
								if(nGroupItemCount < MAX_GROUP_ITEM_COUNT)
								{
									(*itr)->rect.height *= 0.5;
									(*itr)->type         = SINGLE_TAG;
									if(IsHorizontal())
									{
										(*itr)->m_ptEnd.x() -= (*itr)->rect.height*0.5;
									}
									else
									{
										(*itr)->m_ptEnd.y() += (*itr)->rect.height*0.5;
									}
									
									if((*itr)->next)
									{
										(*itr)->gcode = GROUP_IN;
										(*itrNext)->next = ListPush((*itrNext)->next,(*itr)->next);
									}
									else
									{
										(*itr)->gcode = GROUP_START;
										m_nGroupCount++;
									}
									(*itr)->next = NULL;
									(*itrNext)->next = ListPush((*itrNext)->next,*itr);
									(*itrNext)->gcode = GROUP_END;
									(*itrNext)->nGroup= m_nGroupCount;
									for(CaDraw_TagPos* ptr=(*itrNext);ptr;ptr = ptr->next) ptr->nGroup = m_nGroupCount;
									nGroupItemCount++;
									break;
								}
								else
								{
									nGroupItemCount = 0;
									break;
								}
							}else	break;
						}else	break;
					}
				}
			}
		}
	}
	else if(2 == env.m_pipe.m_ext.nGroupLevel)
	{
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			if((DRAFT_PIPE == (*itr)->pTITEM->enmType) && GROUP_ITEM(*itr))
			{
				for(vector<CaDraw_TagPos*>::iterator itrNext=itr+1;itrNext != m_pvtTagPos->end();++itrNext)
				{
					if((DRAFT_PIPE == (*itrNext)->pTITEM->enmType) && GROUP_ITEM(*itrNext))
					{
						if(OverlapCount(*itr,*itrNext) > -(*itrNext)->rect.height)
						{
							if((*itr)->pTITEM->nCL == (*itrNext)->pTITEM->nCL)
							{
								if(nGroupItemCount < MAX_GROUP_ITEM_COUNT)
								{
									(*itr)->rect.height *= 0.5;
									(*itr)->type         = SINGLE_TAG;
									if(IsHorizontal())
									{
										(*itr)->m_ptEnd.x() -= (*itr)->rect.height*0.5;
									}
									else
									{
										(*itr)->m_ptEnd.y() += (*itr)->rect.height*0.5;
									}
									
									if((*itr)->next)
									{
										(*itr)->gcode = GROUP_IN;
										(*itrNext)->next = ListPush((*itrNext)->next,(*itr)->next);
									}
									else
									{
										(*itr)->gcode = GROUP_START;
										m_nGroupCount++;
									}
									(*itr)->next = NULL;
									(*itrNext)->next = ListPush((*itrNext)->next,*itr);
									(*itrNext)->gcode = GROUP_END;
									(*itrNext)->nGroup= m_nGroupCount;
									for(CaDraw_TagPos* ptr=(*itrNext);ptr;ptr = ptr->next) ptr->nGroup = m_nGroupCount;
									nGroupItemCount++;
									break;
								}
								else
								{
									nGroupItemCount = 0;
									break;
								}
							}else	break;
						}else	break;
					}
				}
			}
		}
	}
	else if(3 == env.m_pipe.m_ext.nGroupLevel)
	{
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			if((DRAFT_PIPE == (*itr)->pTITEM->enmType) && GROUP_ITEM(*itr))
			{
				for(vector<CaDraw_TagPos*>::iterator itrNext=itr+1;itrNext != m_pvtTagPos->end();++itrNext)
				{
					if((DRAFT_PIPE == (*itrNext)->pTITEM->enmType) && GROUP_ITEM(*itrNext))
					{
						if(OverlapCount(*itr,*itrNext) > -(*itrNext)->rect.height*2)
						{
							if((*itr)->pTITEM->nCL == (*itrNext)->pTITEM->nCL)
							{
								if(nGroupItemCount < MAX_GROUP_ITEM_COUNT)
								{
									(*itr)->rect.height *= 0.5;
									(*itr)->type         = SINGLE_TAG;
									(*itr)->nGroup = m_nGroupCount;

									if(IsHorizontal())
									{
										(*itr)->m_ptEnd.x()     -= (*itr)->rect.height*0.5;
									}
									else
									{
										(*itr)->m_ptEnd.y()     += (*itr)->rect.height*0.5;
									}
									
									if((*itr)->next)
									{
										(*itr)->gcode = GROUP_IN;
										(*itrNext)->next = ListPush((*itrNext)->next,(*itr)->next);
									}
									else
									{
										(*itr)->gcode = GROUP_START;
										m_nGroupCount++;
									}
									(*itr)->next = NULL;
									(*itrNext)->next  = ListPush((*itrNext)->next,*itr);
									(*itrNext)->gcode = GROUP_END;
									(*itrNext)->nGroup= m_nGroupCount;
									for(CaDraw_TagPos* ptr=(*itrNext);ptr;ptr = ptr->next) ptr->nGroup = m_nGroupCount;
									nGroupItemCount++;
									break;
								}
								else
								{
									nGroupItemCount = 0;
									break;
								}
							}else	break;
						}else	break;
					}
				}
			}
		}
	}

	// arrange
	if(IsHorizontal())
	{
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			for(vector<CaDraw_TagPos*>::iterator itrNext = itr+1;itrNext != m_pvtTagPos->end();itrNext++)
			{
				if((*itr)->m_ptEnd.x() > (*itrNext)->m_ptEnd.x())
				{
					if((*itrNext)->bFixed)
					{
						(*itr)->m_ptEnd.x() = (*(itrNext))->m_ptEnd.x() + (*(itrNext))->rect.height;
						
						CaDraw_TagPos* ptr =*(itr);
						*(itr)=(*itrNext);
						*itrNext    =ptr;
					}
					else
					{
						(*(itrNext))->m_ptEnd.x() += (*itr)->m_ptEnd.x() - (*itrNext)->m_ptEnd.x() + (*itr)->rect.height*0.5;
					}
				}else	break;
			}
		}
	}
	else if(IsVertical())
	{
		for(vector<CaDraw_TagPos*>::iterator itr=m_pvtTagPos->begin();itr != m_pvtTagPos->end();++itr)
		{
			for(vector<CaDraw_TagPos*>::iterator itrNext = itr+1;itrNext != m_pvtTagPos->end();itrNext++)
			{
				if((*itr)->m_ptEnd.y() < (*itrNext)->m_ptEnd.y())
				{
					if((*itrNext)->bFixed)
					{
						(*itr)->m_ptEnd.y() = (*(itrNext))->m_ptEnd.y() - (*(itrNext))->rect.height*0.5;
						
						CaDraw_TagPos* ptr =*(itr);
						*(itr)=(*itrNext);
						*itrNext    =ptr;
					}
					else
					{
						(*(itrNext))->m_ptEnd.y() += (*itr)->m_ptEnd.y() - (*itrNext)->m_ptEnd.y() - (*itr)->rect.height*0.5;
					}
				}else	break;
			}
		}
	}
}

/**	
	@brief	check either two tag point is overlapped or not.
	@param	pTagPos1	a parameter of type CaDraw_TagPos*
	@param	pTagPos2	a parameter of type CaDraw_TagPos*
	@return	double	
*/
double CExtDivision::OverlapCount(CaDraw_TagPos* pTagPos1,CaDraw_TagPos* pTagPos2)
{
	assert(pTagPos1 && "pTagPos1 is NULL");
	assert(pTagPos2 && "pTagPos2 is NULL");
	double nRet=0.;
	
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	if(pTagPos1 && pTagPos2)
	{
		if(IsHorizontal())
		{
			double nWidth[2]={0.,};
			nWidth[0] = (SINGLE_TAG == pTagPos1->type) ? (pTagPos1->rect.height)*0.5 : (pTagPos1->rect.height)*0.5 + (env.m_nGap)*2;
			nWidth[1] = (SINGLE_TAG == pTagPos2->type) ? (pTagPos1->rect.height)*0.5 : (pTagPos2->rect.height)*0.5 + (env.m_nGap)*2;
			nWidth[0] += nWidth[1];
			double nLen  = fabs(pTagPos1->m_ptEnd.x() - pTagPos2->m_ptEnd.x());
			
			return (nWidth[0] - nLen);
		}
		else if(IsVertical())
		{
			double nWidth[2]={0.,};
			nWidth[0] = (SINGLE_TAG == pTagPos1->type) ? (pTagPos1->rect.height)*0.5 : (pTagPos1->rect.height)*0.5 + (env.m_nGap)*2;
			nWidth[1] = (SINGLE_TAG == pTagPos2->type) ? (pTagPos1->rect.height)*0.5 : (pTagPos2->rect.height)*0.5 + (env.m_nGap)*2;
			nWidth[0]+= nWidth[1];
			double nLen  = fabs(pTagPos1->m_ptEnd.y() - pTagPos2->m_ptEnd.y());
			
			double yMax=pTagPos1->m_ptEnd.y() - pTagPos1->rect.height*0.5 - env.m_nGap;
			double yMin=pTagPos2->m_ptEnd.y() + pTagPos2->rect.height*0.5 + env.m_nGap;
			
			return (yMin - yMax);
		}
	}
	
	return nRet;
}

/**	
	@brief	get pipe BOP(BOP).
	@param	pTITEM	a parameter of type CaDraw_TagItem*
	@return	int	
*/
int CExtDivision::GetPipeBOP(CaDraw_TagItem* pTITEM)
{
	assert(pTITEM && "pTITEM is NULL");
	int nElev=0;

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	if(pTITEM)
	{
		nElev = (int)SAFE_ROUND(pTITEM->m_ptOrigin.z(),1);
	}

	return nElev;
}

/**	
	@brief	The CExtDivision::GetFirstTagNo function
	@author	HumKyung.BAEK
	@param	pTITEM	a parameter of type CaDraw_TagItem*
	@return	TCHAR*	
*/
TCHAR* CExtDivision::GetFirstTagNo(CaDraw_TagItem* pTITEM)
{
	assert(pTITEM && "pTITEM is NULL");
	static TCHAR sz[256]={0,};

	sz[0] = '\0';
	if(pTITEM && (*pTITEM->name[0])[0])
	{
		ZeroMemory(sz , sizeof(TCHAR) * 256);
		
		STRCPY_T(sz,pTITEM->name[0]->c_str());
		TCHAR* pNewLine=STRSTR_T(sz, _T("\\n"));
		if(pNewLine) (*pNewLine) = '\0';
	}

	return sz;
}

/**	
	@brief	The CExtDivision::GetSecondTagNo function
	@author	humkyung
	@param	pTITEM	a parameter of type CaDraw_TagItem*
	@return	TCHAR*	
*/
TCHAR* CExtDivision::GetSecondTagNo(CaDraw_TagItem* pTITEM)
{
	assert(pTITEM && "pTITEM is NULL");
	static TCHAR sz[256]={0,};

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	sz[0] = '\0';
	if(pTITEM && (*pTITEM->name[0])[0])
	{
		ZeroMemory(sz , sizeof(TCHAR) * 256);
		const TCHAR* pNewLine=STRSTR_T(pTITEM->name[0]->c_str(), _T("\\n")) ;
		if(pNewLine) STRCPY_T(sz,(const TCHAR*)(pNewLine + 2));

		/// 파이프의 옵션에서 tag type이 DOUBLELINE이고 , second tag type이 ELEV_COORD일 경우.
		if( (DRAFT_PIPE == pTITEM->enmType) && (DOUBLELINE == env.m_pipe.m_ext.eTag) && (ELEV_COORD == env.m_pipe.m_ext.eSecondTagType))
		{
			if( pTITEM->enmSubType == HORIZONTAL )
			{
				UINT_TYPE_E SubUnit = M;

				CaDraw_View::ROTATE_T vRotate = m_pDrawView->GetViewAngle();
				if((CaDraw_View::R0_T == vRotate) || (CaDraw_View::R180_T == vRotate))
				{
					if( IsLeftSide() || IsRightSide() )
					{
						(pTITEM->ptModelOrigin.y() < 0.f) ? STRCAT_T( sz , _T(" S ")) : STRCAT_T( sz , _T(" N "));
						STRCAT_T( sz ,CSmartDrawAnnoModule::UNIT2STR(fabs(pTITEM->ptModelOrigin.y()) , SubUnit));
					}
					else
					{
						(pTITEM->ptModelOrigin.x() < 0.f) ? STRCAT_T( sz , _T(" W ")) : STRCAT_T( sz , _T(" E "));
						STRCAT_T( sz ,CSmartDrawAnnoModule::UNIT2STR(fabs(pTITEM->ptModelOrigin.x()) , SubUnit));
					}
				}
				else	/// 90도 혹은 270도 회전.
				{
					if( IsLeftSide() || IsRightSide() )
					{
						(pTITEM->ptModelOrigin.x() < 0.f) ? STRCAT_T( sz , _T(" W ")) : STRCAT_T( sz , _T(" E "));
						STRCAT_T( sz ,CSmartDrawAnnoModule::UNIT2STR(fabs(pTITEM->ptModelOrigin.x()) , SubUnit));
					}
					else
					{
						(pTITEM->ptModelOrigin.y() < 0.f) ? STRCAT_T( sz , _T(" S ")) : STRCAT_T( sz , _T(" N "));
						STRCAT_T( sz ,CSmartDrawAnnoModule::UNIT2STR(fabs(pTITEM->ptModelOrigin.y()) , SubUnit));
					}
				}
			}
		}
	}

	return sz;
}

/**	
	@brief	return max distance to dim. line
	@author	humkyung
	@date	2016.03.11
	@return	double
*/
double CExtDivision::GetMaxDistToDimLine() const
{
	double res = 0.f;

	int iMaxLevel = -1;
	for(list<CaDraw_TagPos*>::const_iterator itr = m_plstDimension->begin();itr != m_plstDimension->end();++itr)
	{
		if((*itr)->pTITEM->nLevel > iMaxLevel) iMaxLevel = (*itr)->pTITEM->nLevel;
		if(DIM_LEVEL_2 == iMaxLevel) break;
	}

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	if(DIM_LEVEL_0 == iMaxLevel)
	{
		res = env.m_outside.m_nDistA;
	}
	else if(DIM_LEVEL_1 == iMaxLevel)
	{
		res = env.m_outside.m_nDistA + env.m_outside.m_nDistB;
	}
	else if(DIM_LEVEL_2 == iMaxLevel)
	{
		res = env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC;
	}

	return res;
}

bool CExtDivision::IsBottomSide() const
{
	const static double _nTol=0.001;

	if((fabs(m_vecTagDir.dx()) < _nTol) && (fabs(m_vecTagDir.dy() + 1) < _nTol)) return true;

	return false;
}

bool CExtDivision::IsRightSide() const
{
	const static double _nTol=0.001;
	
	if((fabs(m_vecTagDir.dx() - 1) < _nTol) && (fabs(m_vecTagDir.dy()) < _nTol)) return true;
	
	return false;
}

bool CExtDivision::IsTopSide() const
{
	const static double _nTol=0.001;
	
	if((fabs(m_vecTagDir.dx()) < _nTol) && (fabs(m_vecTagDir.dy() - 1) < _nTol)) return true;
	
	return false;
}

bool CExtDivision::IsLeftSide() const
{
	const static double _nTol=0.001;
	
	if((fabs(m_vecTagDir.dx() + 1) < _nTol) && (fabs(m_vecTagDir.dy()) < _nTol)) return true;
	
	return false;
}

CIsLine3d CExtDivision::GetBaseLine()
{
	return m_lnBaseLine;
}
