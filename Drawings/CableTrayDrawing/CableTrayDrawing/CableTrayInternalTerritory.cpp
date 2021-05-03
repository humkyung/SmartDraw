#include "StdAfx.h"
#include <IsTools.h>
#include <AnnoTerritory.h>
#include <SmartDrawAnnoEnv.h>
#include <SmartDrawAnnoStru.h>
#include "CableTrayInternalTerritory.h"
#include "CableRoute.h"

#include <set>
using namespace std;

CCellMap* CCableTrayInternalTerritory::m_pCellMapExp = NULL;

CCableTrayInternalTerritory::CCableTrayInternalTerritory()
{
}

CCableTrayInternalTerritory::~CCableTrayInternalTerritory(void)
{
	try
	{
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-16
    @class      CCableTrayInternalTerritory
    @function   Annotate
    @return     int
    @param      list<CIsLine2d>&    oHsrLineList
    @brief
******************************************************************************/
int CCableTrayInternalTerritory::Annotate(list<CIsLine2d>& oHsrLineList)
{
	const double TOLER = 0.001;
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	CCableTrayInternalTerritory::m_pCellMapExp = this->m_pCellMap;

	this->PrepareCellMap(oHsrLineList);

	TCHAR szTemp[MAX_PATH + 1]={'\0',};
	GetTempPath(_MAX_PATH, szTemp);
	m_pCellMap->DumpToBMPFile(szTemp + STRING_T(_T("hsr.bmp")));

	/// collect cable tray entites
	vector<CaDraw_TagCableTray*> oCableTrayEntList;
	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if((*itr)->IsKindOf(CaDraw_TagCableTray::TypeString()))
		{
			CaDraw_TagCableTray* pCableTrayEnt = static_cast<CaDraw_TagCableTray*>(*itr);
			oCableTrayEntList.push_back( pCableTrayEnt );
		}
	}
	CableTray::CreateCableRoute(oCableTrayEntList);
	/// up to here

	list<CaDraw_TagElbow*> TagElbowList;
	list<CaDraw_TagEqui*>  TagEquiList;
	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if((*itr)->IsKindOf(CaDraw_TagPipe::TypeString()))
		{
			CaDraw_TagPipe* pTagPipe = static_cast<CaDraw_TagPipe*>(*itr);
			const string line_id = pTagPipe->pipe()->line_id();
			
			list<CLineId>::iterator jtr;
			for(jtr = m_pLineIdList->begin();jtr != m_pLineIdList->end();++jtr)
			{
				if(line_id == jtr->m_line_id)
				{
					if(jtr->m_nMaxSize < pTagPipe->pipe()->GetSize())
					{
						jtr->m_nMaxSize = pTagPipe->pipe()->GetSize();
					}
					jtr->m_pSegmentList->push_back(pTagPipe);
					break;
				}
			}
			if(jtr == m_pLineIdList->end())
			{
				CLineId lineId;
				lineId.m_line_id  = pTagPipe->pipe()->line_id();
				lineId.m_nMaxSize = pTagPipe->pipe()->GetSize();
				lineId.m_pSegmentList->push_back(pTagPipe);
				m_pLineIdList->push_back(lineId);
			}
		}
		else if((*itr)->IsKindOf(CaDraw_TagElbow::TypeString()))
		{
			CaDraw_TagElbow* pTagElbow = static_cast<CaDraw_TagElbow*>(*itr);
			TagElbowList.push_back(pTagElbow);
		}
		else if((*itr)->IsKindOf(CaDraw_TagEqui::TypeString()))
		{
			CaDraw_TagEqui* pTagEqui = static_cast<CaDraw_TagEqui*>(*itr);
			TagEquiList.push_back(pTagEqui);
		}
	}
	
	///1. annotate platform hatching
	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if((*itr)->IsKindOf(CSmartDrawAnnoPlatform::TypeString()))
		{
			(*itr)->Annotate();
		}
	}
	/// up to here

	///2. annotate equipment
	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if((false == (*itr)->m_bTagged) && (*itr)->IsKindOf(CaDraw_TagEqui::TypeString()))
		{
			(*itr)->Annotate();
		}
	}
	///3. annotate pipe line
	this->AnnotateLineId(TagElbowList);
	///4. annotate rest of entities except equipment and pipe line
	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if(false == (*itr)->m_bTagged)
		{
			if((*itr)->IsKindOf(CaDraw_TagStru::TypeString()))
			{
			}
			else
			{
				(*itr)->Annotate();
			}
		}
	}

	/// run cable tray annotation
	CableTray::Annotate();
	
	m_pCellMap->DumpToBMPFile(szTemp + STRING_T(_T("anno.bmp")));

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   Write
    @return     void
    @param      OFSTREAM_T& ofile
    @brief
******************************************************************************/
void CCableTrayInternalTerritory::Write(OFSTREAM_T& ofile)
{
	CInternalTerritory::Write(ofile);
}