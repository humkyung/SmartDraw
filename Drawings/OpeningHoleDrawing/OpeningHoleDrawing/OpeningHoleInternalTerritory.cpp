#include "StdAfx.h"
#include <IsTools.h>
#include <AnnoTerritory.h>
#include <SmartDrawAnnoEnv.h>
#include <SmartDrawAnnoStru.h>

#include "OpeningHoleInternalTerritory.h"

#include <set>
using namespace std;

COpeningHoleInternalTerritory::COpeningHoleInternalTerritory(/*CAnnoTerritory* pTerritory*/)/// : CInternalTerritory(pTerritory)
{
}

COpeningHoleInternalTerritory::~COpeningHoleInternalTerritory(void)
{
	try
	{
		for(list<COpeningHoleGroupData*>::iterator itr = m_oOpeningHoleGroupDataList.begin();itr != m_oOpeningHoleGroupDataList.end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_oOpeningHoleGroupDataList.clear();
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   CreateStruGridLine
    @return     int
    @brief
******************************************************************************/
int COpeningHoleInternalTerritory::CreateStruGridLine(CCellMap* pCellMap)
{
	m_oStruGridLineList.clear();

	set<double> oRowGridLine , oColGridLine;
	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if((*itr)->IsKindOf(CaDraw_TagStru::TypeString()))
		{
			CaDraw_TagStru* pTagStru = static_cast<CaDraw_TagStru*>(*itr);
			const CIsPoint3d origin = pTagStru->GetView()->ModelCoord2ViewCoord(pTagStru->stru()->origin());
			oRowGridLine.insert(origin.y());
			oColGridLine.insert(origin.x());
		}
	}

	for(set<double>::iterator itr = oColGridLine.begin();itr != oColGridLine.end();++itr)
	{
		CIsLine2d line;
		line.Set(CIsPoint2d((*itr) , m_pTerritory->m_ExtentRect.y) , CIsPoint2d((*itr) , m_pTerritory->m_ExtentRect.y + m_pTerritory->m_ExtentRect.height));
		m_oStruGridLineList.push_back(line);
	}

	for(set<double>::iterator itr = oRowGridLine.begin();itr != oRowGridLine.end();++itr)
	{
		CIsLine2d line;
		line.Set(CIsPoint2d(m_pTerritory->m_ExtentRect.x , (*itr)) , CIsPoint2d(m_pTerritory->m_ExtentRect.x + m_pTerritory->m_ExtentRect.width , (*itr)));
		m_oStruGridLineList.push_back(line);
	}

	if(NULL != pCellMap)
	{
		for(list<CIsLine2d>::iterator itr = m_oStruGridLineList.begin();itr != m_oStruGridLineList.end();++itr)
		{
			pCellMap->UpdateLineOf(itr->start() , itr->end() , CCellMap::WHITE);
		}
	}

	return ERROR_SUCCESS;
}

static bool __SortBySize(COpeningHoleData* lhs , COpeningHoleData* rhs)
{
	return (lhs->radius() > rhs->radius());
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-09
    @class      COpeningHoleInternalTerritory
    @function   CreateHoleData
    @return     int
    @brief		TODO: multi view일때는 어떻게 되는가? 고려해 볼것...
******************************************************************************/
int COpeningHoleInternalTerritory::CreateHoleData()
{
	m_oPlatformList.clear();
	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if((*itr)->IsKindOf(CSmartDrawAnnoPlatform::TypeString()))
		{
			m_oPlatformList.push_back(static_cast<CSmartDrawAnnoPlatform*>(*itr));
		}
	}

	list<COpeningHoleData*> oOpeningHoleDataList;
	bool isInPlatForm = false;
	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if((*itr)->IsKindOf(CaDraw_TagPipe::TypeString()))
		{
			CaDraw_TagPipe* pPipe = static_cast<CaDraw_TagPipe*>(*itr);

			isInPlatForm = false;
			for (list<CSmartDrawAnnoPlatform*>::iterator jtr = m_oPlatformList.begin(); jtr != m_oPlatformList.end(); ++jtr)
			{
				CIsPoint3d start = pPipe->pipe()->GetStartPoint();
				CIsPoint3d end = pPipe->pipe()->GetEndPoint();
				if ((*jtr)->PassedThrough(start , end))
				{
					CSmartDrawAnnoPlatform* pPlatform = static_cast<CSmartDrawAnnoPlatform*>(*jtr);
					const double dElevation = pPlatform->elevation();

					CIsPoint3d tmp1 = pPipe->GetView()->ModelCoord2ViewCoord(start);
					CIsPoint3d tmp2 = pPipe->GetView()->ModelCoord2ViewCoord(end);

					CIsVect3d dir = end - start;
					dir.Normalize();
					const double t = (dElevation - start.z()) / dir.dz();
					const CIsPoint3d ptModel = start + dir*t;
					CIsPoint3d ptView  = pPipe->GetView()->ModelCoord2ViewCoord(ptModel);
					if ((*jtr)->IsInnerPoint(CIsPoint2d(ptView.x() , ptView.y())))
					{
						isInPlatForm = true;
						COpeningHoleData* pOpeningHoleData = new COpeningHoleData(CIsPoint2d(ptView.x() , ptView.y()) , pPipe);
						if(NULL != pOpeningHoleData)
						{
							pOpeningHoleData->m_ptModelOrigin = ptModel;
							oOpeningHoleDataList.push_back(pOpeningHoleData);
							break;
						}
					}
				}
			}
		}
	}

	stable_sort(oOpeningHoleDataList.begin() , oOpeningHoleDataList.end() , __SortBySize);
	/// create hole group
	while(!oOpeningHoleDataList.empty())
	{
		COpeningHoleGroupData* pOpeningHoleGroupData = new COpeningHoleGroupData(COpeningHoleGroupData::m_iGroupCount++);
		m_oOpeningHoleGroupDataList.push_back(pOpeningHoleGroupData);

		for(list<COpeningHoleData*>::iterator itr = oOpeningHoleDataList.begin();itr != oOpeningHoleDataList.end();)
		{
			if(pOpeningHoleGroupData->IsPossibleToBeMember(*itr))
			{
				pOpeningHoleGroupData->Add(*itr);
				itr = oOpeningHoleDataList.erase(itr);
				continue;
			}
			++itr;
		}
	}
	/// up to here

	return ERROR_SUCCESS;
}

int COpeningHoleInternalTerritory::Annotate(list<CIsLine2d>& oHsrLineList)
{
	const double TOLER = 0.001;
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	/// prepare cell and collect entites within views
	this->PrepareCellMap(oHsrLineList);

	TCHAR szTemp[MAX_PATH + 1]={'\0',};
	GetTempPath(_MAX_PATH, szTemp);
	m_pCellMap->DumpToBMPFile(szTemp + STRING_T(_T("hsr.bmp")));

	this->CreateStruGridLine(m_pCellMap);
	this->CreateHoleData();	/// create hole group data passing through plate
	for(list<COpeningHoleGroupData*>::iterator itr = m_oOpeningHoleGroupDataList.begin();itr != m_oOpeningHoleGroupDataList.end();++itr)
	{
		(*itr)->Annotate(m_pCellMap);
	}

	list<CaDraw_TagElbow*> oTagElbowList;
	list<CaDraw_TagEqui*>  oTagEquiList;
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
			oTagElbowList.push_back(pTagElbow);
		}
		else if((*itr)->IsKindOf(CaDraw_TagEqui::TypeString()))
		{
			CaDraw_TagEqui* pTagEqui = static_cast<CaDraw_TagEqui*>(*itr);
			oTagEquiList.push_back(pTagEqui);
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
	this->AnnotateLineId(oTagElbowList);
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
void COpeningHoleInternalTerritory::Write(OFSTREAM_T& ofile)
{
	for(list<CIsLine2d>::iterator itr = m_oStruGridLineList.begin();itr != m_oStruGridLineList.end();++itr)
	{
		ofile << _T("<LINE>|");
		ofile << itr->start().x() << _T(",") << itr->start().y() << _T(",0,");
		ofile << itr->end().x() << _T(",") << itr->end().y() << _T(",0|");
		ofile << _T("2,2,4,4,0") << std::endl;
	}

	CInternalTerritory::Write(ofile);
	for(list<COpeningHoleGroupData*>::iterator itr = m_oOpeningHoleGroupDataList.begin();itr != m_oOpeningHoleGroupDataList.end();++itr)
	{
		(*itr)->Write(ofile);
	}
}