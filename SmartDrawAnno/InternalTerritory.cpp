#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include <Tokenizer.h>

#include "CellMap.h"
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoEnv.h"
#include "AnnoTerritory.h"
#include "SmartDrawAnnoView.h"
#include "InternalTerritory.h"
#include "SmartDrawAnnoNozzle.h"
#include "SmartDrawAnnoPlatform.h"

#include "CableRoute.h"

#define	INTERNAL_TOLER	0.001

static bool fn_Len_Sort(const CCellMap::SearchRes& lhs, const CCellMap::SearchRes& rhs)
{
	return (lhs.distance < rhs.distance);
}
static bool fn_Cnt_Sort(const CCellMap::SearchRes& lhs, const CCellMap::SearchRes& rhs)
{
	return (lhs.occupied < rhs.occupied);
}


/******************************************************************************
    @author     humkyung
    @date       2012-03-09
    @class      CInternalTerritory
    @function   CInternalTerritory
    @return
    @param      CAnnoTerritory* pTerritory
    @brief
******************************************************************************/
CInternalTerritory::CInternalTerritory(/*CAnnoTerritory* pTerritory*/)/// : m_pTerritory(pTerritory)
{
	m_pCellMap = NULL;

	m_pLineIdList = new list<CLineId>;
	m_plstTagItem = new list<CaDraw_TagItem*>;
	m_pTagEntList = new list<CaDraw_TagEntity*>;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.06
	@param	
	@return	
*/
CInternalTerritory::~CInternalTerritory(void)
{
	try
	{
		SAFE_DELETE(m_pLineIdList);

		for(list<CaDraw_TagItem*>::iterator itr = m_plstTagItem->begin();itr != m_plstTagItem->end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_plstTagItem->clear();
		SAFE_DELETE(m_plstTagItem);

		SAFE_DELETE(m_pTagEntList);
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-16
    @class      CInternalTerritory
    @function   SetAnnoTerritory
    @return     int
    @param      CAnnoTerritory* pTerritory
    @brief
******************************************************************************/
int CInternalTerritory::SetAnnoTerritory(CAnnoTerritory* pTerritory)
{
	m_pTerritory = pTerritory;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-02
    @class      CInternalTerritory
    @function   PrepareCellMap
    @return     int
    @brief
******************************************************************************/
int CInternalTerritory::PrepareCellMap(list<CIsLine2d>& oHiddenLineList)
{
	if(NULL == CSmartDrawAnnoModule::m_pCellMap)
	{
		CIsPoint2d origin(
			m_pTerritory->m_ExtentRect.x + m_pTerritory->m_ExtentRect.width * 0.5 , 
			m_pTerritory->m_ExtentRect.y + m_pTerritory->m_ExtentRect.height* 0.5);

		CSmartDrawAnnoModule::m_pCellMap = new CCellMap(origin , m_pTerritory->m_ExtentRect.width , m_pTerritory->m_ExtentRect.height , 1.f);
		CSmartDrawAnnoModule::m_pCellMap->Reset(true);
	}
	else
	{
		CIsPoint2d origin(
			m_pTerritory->m_ExtentRect.x + m_pTerritory->m_ExtentRect.width * 0.5 , 
			m_pTerritory->m_ExtentRect.y + m_pTerritory->m_ExtentRect.height* 0.5);

		CSmartDrawAnnoModule::m_pCellMap->Resize(origin , m_pTerritory->m_ExtentRect.width , m_pTerritory->m_ExtentRect.height , 1.f);
		CSmartDrawAnnoModule::m_pCellMap->Reset(CCellMap::WHITE);
	}
	m_pCellMap = CSmartDrawAnnoModule::m_pCellMap;

	/// View 부분의 CELL 점유를 clear한다.
	m_pTagEntList->clear();
	list<CaDraw_View*> ViewList;
	m_pTerritory->m_pLogicView->GetViewList(&ViewList);
	for(list<CSmartDrawAnnoView*>::iterator itr = m_pTerritory->m_pAnnoViewList->begin();itr != m_pTerritory->m_pAnnoViewList->end();++itr)
	{
		/// clear cell map of view area
		CIsPoint2d ptMin(
			(*itr)->drawView()->center().x() - (*itr)->drawView()->GetViewWidth() * 0.5 ,
			(*itr)->drawView()->center().y() - (*itr)->drawView()->GetViewHeight() * 0.5);
		CIsPoint2d ptMax(
			(*itr)->drawView()->center().x() + (*itr)->drawView()->GetViewWidth() * 0.5 , 
			(*itr)->drawView()->center().y() + (*itr)->drawView()->GetViewHeight() * 0.5);
		CSmartDrawAnnoModule::m_pCellMap->UpdateRectOf(ptMin , ptMax , CCellMap::EMPTY);

		list<CaDraw_TagEntity*> _TagEntList;
		(*itr)->GetTagEntList(&_TagEntList);
		m_pTagEntList->insert(m_pTagEntList->end() , _TagEntList.begin() , _TagEntList.end());
	}

	/// HIDDEN DATA로 CELL을 채운다.
	this->FillUpCellWithHiddenLine(oHiddenLineList);

	return ERROR_SUCCESS;
}

/**	
	@brief	annotate inside of view

	@author	HumKyung.BAEK

	@date	2010.06.06

	@param	

	@return	
*/
int CInternalTerritory::Annotate(list<CIsLine2d>& oHsrLineList)
{
	const double TOLER = 0.001;
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	this->PrepareCellMap(oHsrLineList);

	TCHAR szTemp[MAX_PATH + 1]={'\0',};
	GetTempPath(_MAX_PATH, szTemp);
	CSmartDrawAnnoModule::m_pCellMap->DumpToBMPFile(szTemp + STRING_T(_T("hsr.bmp")));

	/// collect cable tray entites
	/*vector<CaDraw_TagCableTray*> oCableTrayEntList;
	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if((*itr)->IsKindOf(CaDraw_TagCableTray::TypeString()))
		{
			CaDraw_TagCableTray* pCableTrayEnt = static_cast<CaDraw_TagCableTray*>(*itr);
			oCableTrayEntList.push_back( pCableTrayEnt );
		}
	}
	CableTray::CreateCableRoute(oCableTrayEntList);*/
	/// up to here

	list<CaDraw_TagElbow*> oTagElbowList;
	list<CaDraw_TagEqui*>  TagEquiList;
	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if((*itr)->IsKindOf(CaDraw_TagPipe::TypeString()))
		{
			CaDraw_TagPipe* pTagPipe = static_cast<CaDraw_TagPipe*>(*itr);
			pTagPipe->FillSectionMark();
			const STRING_T line_id = pTagPipe->pipe()->line_id();
			
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
			CaDraw_TagEqui* pEqui = static_cast<CaDraw_TagEqui*>(*itr);
			pEqui->FillSectionMark();
			TagEquiList.push_back(pEqui);
		}
	}
	
	/// 1. annotate platform hatching
	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if((*itr)->IsKindOf(CSmartDrawAnnoPlatform::TypeString()))
		{
			(*itr)->Annotate();
		}
	}
	/// up to here

	/// 2. annotate equipment
	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if((false == (*itr)->m_bTagged) && (*itr)->IsKindOf(CaDraw_TagEqui::TypeString()))
		{
			(*itr)->Annotate();
		}
	}
	/// 3. annotate pipe line
	this->AnnotateLineId(oTagElbowList);
	/// 4. annotate rest of entities except equipment and pipe line
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

	CSmartDrawAnnoModule::m_pCellMap->DumpToBMPFile(szTemp + STRING_T(_T("anno.bmp")));

	return ERROR_SUCCESS;
}

/**	
	@brief	annotate line id
	@author	humkyung
	@date	2010.06.12
	@param	
	@return	
*/
int CInternalTerritory::AnnotateLineId(list<CaDraw_TagElbow*>& oTagElbowList)
{
	::stable_sort(m_pLineIdList->begin() , m_pLineIdList->end() , CLineId::fn_line_id_size_sort);
	for(list<CLineId>::iterator itr = m_pLineIdList->begin();itr != m_pLineIdList->end();++itr)
	{
		if(true == itr->HasTaggedMember()) continue;

		itr->SortBySize();

		bool bAlreadyTagged = false;
		/// 1. PIPE 안에 TAG 하도록 시도한다. 태그 했으면 종료.
		/// 2. 꺽어지는 부분(ELBOW , END POINT)에 태그하도록 시도한다. 태그 했으면 종료.
		/// 3. PIPE LINE 위에 태그를 하도록 시도한다. 태그를 했으면 종료.
		if(itr->TryToPlaceTagInsideOfPipe(10)) continue;
		if(itr->TryToPlaceTagAtCornerOfPipe(oTagElbowList)) continue;
		if(itr->TryToPlaceTagOverPipe(0xFF)) continue;
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.08
	@param	
	@return	
*/
void CInternalTerritory::Write(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	for(list<CLineId>::iterator itr = m_pLineIdList->begin();itr != m_pLineIdList->end();++itr)
	{
		if(itr->m_bTagged)
		{
			itr->Write(ofile,dDrawingScale);
		}
	}

	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if((*itr)->m_bTagged)
		{
			(*itr)->Write(ofile,dDrawingScale);
		}
	}

	/// write vertical section mark - 2014.04.13 added by humkyung
	for(list<CaDraw_TagEntity*>::iterator itr = m_pTagEntList->begin();itr != m_pTagEntList->end();++itr)
	{
		if((*itr)->IsKindOf(CaDraw_TagPipe::TypeString()))
		{
			(static_cast<CaDraw_TagPipe*>(*itr))->WriteSectionMark(ofile,dDrawingScale);
		}
		else if((*itr)->IsKindOf(CaDraw_TagEqui::TypeString()))
		{
			(static_cast<CaDraw_TagEqui*>(*itr))->WriteSectionMark(ofile,dDrawingScale);
		}
	}
	/// up to here
}

/**	
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.08

	@param	

	@return	
*/
int CInternalTerritory::FillUpCellWithHiddenLine(list<CIsLine2d>& oHiddenLineList)
{
	for(list<CIsLine2d>::iterator itr = oHiddenLineList.begin();itr != oHiddenLineList.end();++itr)
	{
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(itr->start() , itr->end() , CCellMap::WHITE);
	}

	return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
CLineId::CLineId() : m_nMaxSize(0.f) ,m_bTagged(false)
{
	m_pSegmentList = new list<CaDraw_TagPipe*>;
}

CLineId::~CLineId()
{
	try
	{
		SAFE_DELETE(m_pSegmentList);
	}
	catch(...)
	{
	}
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.11
	@param	
	@return	
*/
bool CLineId::TryToPlaceTagInsideOfPipe(const int& nMinOccupied)
{
	bool ret = false;

	STRING_T sLabelNo , sElev;
	for (list<CaDraw_TagPipe*>::iterator jtr = m_pSegmentList->begin(); jtr != m_pSegmentList->end(); ++jtr)
	{
		CCellMap::SearchRes res;
		if((*jtr)->TryToPlaceTagAlongPipe(res , sLabelNo , sElev , CaDraw_TagPipe::INSIDE_OF_PIPE , nMinOccupied))
		{
			m_bTagged = true;
			m_oEntTagShape.m_pTagStringList->push_back((*jtr)->GetLineLabel());
			m_oEntTagShape.m_SearchRes = res;
			
			m_oEntTagShape.m_pTagStringList->push_back( sLabelNo );
			m_oEntTagShape.m_pTagStringList->push_back( sElev );

			ret = true;
			break;
		}
	}

	return ret;
}

int CLineId::CheckConnectionOfPipe(CaDraw_TagEntity** pStartEnt , CaDraw_TagEntity** pEndEnt , 
								   CaDraw_TagPipe* pPipe , 
								   list<CaDraw_TagElbow*>& oElbowList , list<CaDraw_TagPipe*>* pPipeList)
{
	assert(pPipe && "pPipe is NULL");
	assert(pPipeList && "pPipeList is NULL");

	const double TOLER = 0.001;
	if(pPipe && pPipeList)
	{
		const CIsPoint3d ptStart = pPipe->pipe()->GetStartPoint();
		const CIsPoint3d ptEnd = pPipe->pipe()->GetEndPoint();

		list<CCellMap::SearchRes> oSearchNominateList;
		for(list<CaDraw_TagElbow*>::iterator itr = oElbowList.begin();itr != oElbowList.end();++itr)
		{
			if((NULL == (*pStartEnt)) && (ptStart.DistanceTo((*itr)->elbow()->origin()) < TOLER))
			{
				(*pStartEnt) = (*itr);
			}

			if((NULL == (*pEndEnt)) && (ptEnd.DistanceTo((*itr)->elbow()->origin()) < TOLER))
			{
				(*pEndEnt) = (*itr);
			}

			if((NULL != (*pStartEnt)) && (NULL != (*pEndEnt))) break;
		}

		if((NULL == (*pStartEnt)) || (NULL == (*pEndEnt)))
		{
			for(list<CaDraw_TagPipe*>::iterator itr = pPipeList->begin();itr != pPipeList->end();++itr)
			{
				if(pPipe == (*itr)) continue;

				if((NULL == (*pStartEnt)) && ((ptStart.DistanceTo((*itr)->pipe()->GetStartPoint()) < TOLER) || (ptStart.DistanceTo((*itr)->pipe()->GetEndPoint()) < TOLER)))
				{
					(*pStartEnt) = (*itr);
				}

				if((NULL == (*pEndEnt)) && ((ptEnd.DistanceTo((*itr)->pipe()->GetStartPoint()) < TOLER) || (ptEnd.DistanceTo((*itr)->pipe()->GetEndPoint()) < TOLER)))
				{
					(*pEndEnt) = (*itr);
				}

				if((NULL != (*pStartEnt)) && (NULL != (*pEndEnt))) break;
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**	
	@brief	annotate pipe tag at corner of pipe
	@author	humkyung
	@date	2010.06.11
	@param	
	@return	
*/
bool CLineId::TryToPlaceTagAtCornerOfPipe(list<CaDraw_TagElbow*>& TagElbowList)
{
	const double TOLER = 0.001;
	bool ret = false;

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	STRING_T sLabelNo , sElev;

	int nIndex = 0;
	map<CaDraw_TagPipe* , list<CCellMap::SearchRes> > SearchNominateMap;
	for (list<CaDraw_TagPipe*>::iterator jtr = m_pSegmentList->begin(); jtr != m_pSegmentList->end(); ++jtr,++nIndex)
	{
		CIsPoint3d ptStart , ptEnd , pt[2];
		pt[0] = ptStart = ((*jtr)->pipe()->GetStartPoint());
		pt[0] = (*jtr)->ModelCoord2ViewCoord(pt[0]);
		pt[0].z() = 0.f;
		pt[1] = ptEnd = ((*jtr)->pipe()->GetEndPoint());
		pt[1] = (*jtr)->ModelCoord2ViewCoord(pt[1]);
		pt[1].z() = 0.f;
		if((fabs(pt[0].x() - pt[1].x()) < TOLER) && (fabs(pt[0].y() - pt[1].y()) < TOLER)) continue;
		
		list<CCellMap::SearchRes> oSearchNominateList;
		CaDraw_TagEntity *pStartEnt=NULL , *pEndEnt=NULL;
		this->CheckConnectionOfPipe(&pStartEnt , &pEndEnt , (*jtr) , TagElbowList , m_pSegmentList);
		if((NULL != pStartEnt) && pStartEnt->IsKindOf(CaDraw_TagElbow::TypeString()))
		{
			CaDraw_TagElbow* pTagElbow = static_cast<CaDraw_TagElbow*>(pStartEnt);

			CCellMap::SearchRes sRes;

			CIsPoint3d ptElbow = (*jtr)->ModelCoord2ViewCoord(pTagElbow->elbow()->origin());

			STRNCPY_T(sRes.szString , (*jtr)->GetLineLabel().c_str() , 255);
			sRes.ptLoc = sRes.ptTagOrigin = CIsPoint2d(ptElbow.x() , ptElbow.y());
			sRes.cAxis.xAxis = CIsVect2d(pt[0].x() - pt[1].x() , pt[0].y() - pt[1].y());
			sRes.cAxis.xAxis.Normalize();
			CIsVect3d delta = CIsVect3d(0.f , 0.f , 1.f) * CIsVect3d(sRes.cAxis.xAxis.dx() , sRes.cAxis.xAxis.dy() , 0.f);
			sRes.cAxis.yAxis = CIsVect2d(delta.dx() , delta.dy());
			sRes.cAxis.yAxis.Normalize();
			oSearchNominateList.push_back(sRes);
		}
		if((NULL != pEndEnt) && pEndEnt->IsKindOf(CaDraw_TagElbow::TypeString()))
		{
			CaDraw_TagElbow* pTagElbow = static_cast<CaDraw_TagElbow*>(pEndEnt);

			CCellMap::SearchRes sRes;

			CIsPoint3d ptElbow = (*jtr)->ModelCoord2ViewCoord(pTagElbow->elbow()->origin());

			STRNCPY_T(sRes.szString , (*jtr)->GetLineLabel().c_str() , 255);
			sRes.ptLoc = sRes.ptTagOrigin = CIsPoint2d(ptElbow.x() , ptElbow.y());
			sRes.cAxis.xAxis = CIsVect2d(pt[1].x() - pt[0].x() , pt[1].y() - pt[0].y());
			sRes.cAxis.xAxis.Normalize();
			CIsVect3d delta = CIsVect3d(0.f , 0.f , 1.f) * CIsVect3d(sRes.cAxis.xAxis.dx() , sRes.cAxis.xAxis.dy() , 0.f);
			sRes.cAxis.yAxis = CIsVect2d(delta.dx() , delta.dy());
			sRes.cAxis.yAxis.Normalize();
			oSearchNominateList.push_back(sRes);
		}

		(*jtr)->ClipByViewBoundary(pt);
		if((fabs(pt[0].x() - pt[1].x()) < TOLER) && (fabs(pt[0].y() - pt[1].y()) < TOLER)) continue;	/// 2015.01.06 added by humkyung
		if(NULL == pStartEnt)
		{
			CCellMap::SearchRes sRes;

			CIsPoint3d ptElbow = pt[0];

			STRNCPY_T(sRes.szString , (*jtr)->GetLineLabel().c_str() , 255);
			sRes.ptLoc = sRes.ptTagOrigin = CIsPoint2d(ptElbow.x() , ptElbow.y());
			sRes.cAxis.xAxis = CIsVect2d(pt[0].x() - pt[1].x() , pt[0].y() - pt[1].y());
			sRes.cAxis.xAxis.Normalize();
			CIsVect3d delta = CIsVect3d(0.f , 0.f , 1.f) * CIsVect3d(sRes.cAxis.xAxis.dx() , sRes.cAxis.xAxis.dy() , 0.f);
			sRes.cAxis.yAxis = CIsVect2d(delta.dx() , delta.dy());
			sRes.cAxis.yAxis.Normalize();
			oSearchNominateList.push_back(sRes);
		}
		if(NULL == pEndEnt)
		{
			CCellMap::SearchRes sRes;

			CIsPoint3d ptElbow = pt[1];

			STRNCPY_T(sRes.szString , (*jtr)->GetLineLabel().c_str() , 255);
			sRes.ptLoc = sRes.ptTagOrigin = CIsPoint2d(ptElbow.x() , ptElbow.y());
			sRes.cAxis.xAxis = CIsVect2d(pt[1].x() - pt[0].x() , pt[1].y() - pt[0].y());
			sRes.cAxis.xAxis.Normalize();
			CIsVect3d delta = CIsVect3d(0.f , 0.f , 1.f) * CIsVect3d(sRes.cAxis.xAxis.dx() , sRes.cAxis.xAxis.dy() , 0.f);
			sRes.cAxis.yAxis = CIsVect2d(delta.dx() , delta.dy());
			sRes.cAxis.yAxis.Normalize();
			oSearchNominateList.push_back(sRes);
		}

		if(!oSearchNominateList.empty())
		{
			SearchNominateMap.insert(make_pair(*jtr , oSearchNominateList));
		}
	}

	list<CCellMap::SearchRes> SearchResList;
	for(map<CaDraw_TagPipe* , list<CCellMap::SearchRes> >::iterator jtr = SearchNominateMap.begin();jtr != SearchNominateMap.end();++jtr)
	{
		double dItemWidth = 0.0 , dItemHeight = 0.0;
		jtr->first->CalculateTagItemRect(sLabelNo , sElev , dItemWidth , dItemHeight);
		for(list<CCellMap::SearchRes>::iterator ktr = jtr->second.begin();ktr != jtr->second.end();++ktr)
		{
			const double nTextLength = env.m_pipe.m_int.m_oLineNoTextStyle.GetTextLength(ktr->szString);
			
			CCellMap::SIZE_T sizItem;
			sizItem.width = int(SAFE_ROUND(dItemWidth * CSmartDrawAnnoModule::m_pCellMap->scale(),0));
			sizItem.height= int(SAFE_ROUND(dItemHeight * CSmartDrawAnnoModule::m_pCellMap->scale(),0));

			const double dSearchLength = dItemWidth * 2;	/// TODO: 옵션에 따라 변경된다.
			CCellMap::SIZE_T sizSearch;
			sizSearch.width = int(SAFE_ROUND(dSearchLength * CSmartDrawAnnoModule::m_pCellMap->scale(),0));
			sizSearch.height= int(SAFE_ROUND(dItemHeight * CSmartDrawAnnoModule::m_pCellMap->scale(),0));

			CCellMap::CellIndex tagOrigin = CSmartDrawAnnoModule::m_pCellMap->PointToIndex(ktr->ptLoc);
			CIsPoint2d origin(ktr->ptLoc);
			if((_T("Only LineNo") == env.m_pipe.m_int.m_sDisplayStyle) || (_T("LineNo+Elev") == env.m_pipe.m_int.m_sDisplayStyle))
			{
				origin = ktr->ptLoc;
			}
			else if(_T("LineNo/Elev") == env.m_pipe.m_int.m_sDisplayStyle)
			{
				origin = ktr->ptLoc - ((ktr->cAxis.yAxis)*(env.m_pipe.m_int.m_oElevTextStyle.height() + env.m_pipe.m_int.m_dOffset));
			}
			else
			{
				origin = ktr->ptLoc + (-(ktr->cAxis.yAxis)*env.m_pipe.m_int.m_oLineNoTextStyle.height()*0.5);
			}
			CCellMap::CellIndex cOrigin = CSmartDrawAnnoModule::m_pCellMap->PointToIndex(origin);
			
			ktr->radian = ktr->cAxis.xAxis.angle();
			(ktr->radian < 0.0) ? (ktr->radian += 2*PI) : (ktr->radian);
			if((ktr->radian > PI*0.5) && (ktr->radian < PI*1.5))
			{
				/// 글자가 뒤집혀 보이게 되므로 보정해 준다.(relocate start point of search)
				CIsPoint2d tmp = origin + ktr->cAxis.xAxis*(dSearchLength);
				cOrigin = CSmartDrawAnnoModule::m_pCellMap->PointToIndex(tmp);
				
				ktr->cAxis.xAxis = -ktr->cAxis.xAxis;
				ktr->cAxis.yAxis = -ktr->cAxis.yAxis;
				ktr->radian = ktr->cAxis.xAxis.angle();
			}

			CCellMap::SearchRes res(*ktr);
			if(true == CSmartDrawAnnoModule::m_pCellMap->FindBestLoc(res , cOrigin , tagOrigin , 
				sizSearch , sizItem , ktr->cAxis , 30))
			{
				SPRINTF_T(res.szString , _T("%s\\n%s") , sLabelNo.c_str() , sElev.c_str());

				res.coords[0] = res.ptLoc;
				res.coords[1] = res.coords[0] + (res.cAxis.xAxis * dItemWidth);
				res.coords[2] = res.coords[1] + (res.cAxis.yAxis * dItemHeight);
				res.coords[3] = res.coords[2] - (res.cAxis.xAxis * dItemWidth);

				if(((_T("Only LineNo") == env.m_pipe.m_int.m_sDisplayStyle) || _T("Only LineNo") == env.m_pipe.m_int.m_sDisplayStyle))
				{					
					res.leaders[0] = res.coords[0];
					res.leaders[1] = res.coords[1];
					if(res.leaders[0].DistanceTo(ktr->ptLoc) > res.leaders[1].DistanceTo(ktr->ptLoc))
					{
						swap(res.leaders[0] , res.leaders[1]);
					}
				}
				else
				{
					res.leaders[0] = res.coords[0] + (res.cAxis.yAxis * (env.m_pipe.m_int.m_oElevTextStyle.height() + env.m_pipe.m_int.m_dOffset));
					res.leaders[1] = res.coords[1] + (res.cAxis.yAxis * (env.m_pipe.m_int.m_oElevTextStyle.height() + env.m_pipe.m_int.m_dOffset));
					if(res.leaders[0].DistanceTo(ktr->ptLoc) > res.leaders[1].DistanceTo(ktr->ptLoc))
					{
						swap(res.leaders[0] , res.leaders[1]);
					}
				}
				SearchResList.push_back(res);
			}
		}
	}

	if(!SearchResList.empty())
	{
		stable_sort(SearchResList.begin(), SearchResList.end(), CCellMap::fnResSort);

		CCellMap::SearchRes res = SearchResList.front();
		
		/// cell 좌표와 실제 좌표와의 오차를 보정해 준다. - 2012.03.08 added by humkyung
		{
			const double dx = res.leaders[0].x() - res.ptTagOrigin.x();
			const double dy = res.leaders[0].y() - res.ptTagOrigin.y();
			const double l = sqrt(dx*dx + dy*dy);
			const double dot= res.cAxis.xAxis.dx()*dx + res.cAxis.xAxis.dy()*dy;
			const double h = sqrt(l*l - dot*dot);

			CIsPoint2d at = res.ptTagOrigin + res.cAxis.xAxis*dot;
			CIsVect2d dir = at - res.leaders[0];
			dir.Normalize();

			res.ptLoc     = res.ptLoc + dir*h;
			res.coords[0] = res.coords[0] + dir*h;
			res.coords[1] = res.coords[1] + dir*h;
			res.coords[2] = res.coords[2] + dir*h;
			res.coords[3] = res.coords[3] + dir*h;
			res.leaders[0] = res.leaders[0] + dir*h;
			res.leaders[1] = res.leaders[1] + dir*h;
		}
		/// up to here

		CIsVect2d ldir = res.coords[1] - res.coords[0];
		const long lItemCellLength = long(SAFE_ROUND(ldir.Length()*CSmartDrawAnnoModule::m_pCellMap->scale() , 1));
		CIsVect2d hdir = res.coords[2] - res.coords[1];
		const long lItemCellHeight = long(SAFE_ROUND(hdir.Length()*CSmartDrawAnnoModule::m_pCellMap->scale() , 1));
		CSmartDrawAnnoModule::m_pCellMap->UpdateRectOf(CSmartDrawAnnoModule::m_pCellMap->PointToIndex(res.coords[0]) , lItemCellLength , lItemCellHeight , res.cAxis , 155);

		m_bTagged = true;
		m_oEntTagShape.m_SearchRes = res;
		
		TCHAR* pNewLine = STRSTR_T(res.szString , _T("\\n"));
		if(NULL != pNewLine)
		{
			m_oEntTagShape.m_pTagStringList->push_back((const TCHAR*)(pNewLine + 2));
			(*pNewLine) = '\0';
			m_oEntTagShape.m_pTagStringList->push_back(res.szString);
			reverse(m_oEntTagShape.m_pTagStringList->begin() , m_oEntTagShape.m_pTagStringList->end());
		}
		else
		{
			m_oEntTagShape.m_pTagStringList->push_back(res.szString);
		}

		///TODO: TEST해 볼것
		///CTokenizer<CIsFromRegExp>::Tokenize((*m_oEntTagShape.m_pTagStringList) , res.szString , CIsFromRegExp(_T("\\n")));
		///m_oEntTagShape.m_pTagStringList->push_back( sLabelNo );
		///m_oEntTagShape.m_pTagStringList->push_back( sElev );

		///m_oEntTagShape.m_pTagStringList->push_back(res.szString);

		ret = true;
	}

	return ret;
}

/**	
	@brief	

	@author	humkyung

	@date	2010.06.11

	@param	

	@return	
*/
bool CLineId::TryToPlaceTagOverPipe(const int& nMinOccupied)
{
	bool ret = false;

	STRING_T sLabelNo , sElev;
	for (list<CaDraw_TagPipe*>::iterator jtr = m_pSegmentList->begin(); jtr != m_pSegmentList->end(); ++jtr)
	{
		CCellMap::SearchRes res;
		if((*jtr)->TryToPlaceTagAlongPipe(res , sLabelNo , sElev , CaDraw_TagPipe::OVER_PIPE , nMinOccupied))
		{
			m_bTagged = true;
			m_oEntTagShape.m_pTagStringList->push_back((*jtr)->GetLineLabel());
			m_oEntTagShape.m_SearchRes = res;

			m_oEntTagShape.m_pTagStringList->push_back( sLabelNo );
			m_oEntTagShape.m_pTagStringList->push_back( sElev );

			ret = true;
			break;
		}
	}

	return ret;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-25
    @function   HasTaggedMember
    @return     bool
    @brief
******************************************************************************/
bool CLineId::HasTaggedMember() const
{
	for (list<CaDraw_TagPipe*>::const_iterator itr = m_pSegmentList->begin(); itr != m_pSegmentList->end(); ++itr)
	{
		if(CaDraw_Pipe::ALREADY_TAGGED == (*itr)->pipe()->m_nTagFlag) return true;
	}

	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-19
    @function   Write
    @return     bool
    @param      OFSTREAM_T& ofile
    @brief
******************************************************************************/
int CLineId::Write(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	if(0 == STRCMP_T(this->m_oEntTagShape.m_SearchRes.szString , _T("Inside Pipe")))
	{
		ofile << _T("<GROUP>|0,PIPE,Pat 01") << std::endl;

		ofile << _T("<LINE>|");
		ofile << this->m_oEntTagShape.m_SearchRes.leaders[0].x()*dDrawingScale << _T(",") << this->m_oEntTagShape.m_SearchRes.leaders[0].y()*dDrawingScale << _T(",0,");
		ofile << this->m_oEntTagShape.m_SearchRes.leaders[1].x()*dDrawingScale << _T(",") << this->m_oEntTagShape.m_SearchRes.leaders[1].y()*dDrawingScale << _T(",0|");
		ofile << env.m_pipe.m_int.m_oLeaderProp.level << _T(",") << env.m_pipe.m_int.m_oLeaderProp.color << _T(",");
		ofile << env.m_pipe.m_int.m_oLeaderProp.style << _T(",0,") << env.m_pipe.m_int.m_oLeaderProp.weight << std::endl;
	}
	else
	{
		ofile << _T("<GROUP>|0,PIPE,Pat 02") << std::endl;

		ofile << _T("<LEADER>|");
		ofile << this->m_oEntTagShape.m_SearchRes.ptTagOrigin.x()*dDrawingScale << _T(",") << this->m_oEntTagShape.m_SearchRes.ptTagOrigin.y()*dDrawingScale << _T(",");
		ofile << this->m_oEntTagShape.m_SearchRes.leaders[0].x()*dDrawingScale << _T(",") << this->m_oEntTagShape.m_SearchRes.leaders[0].y()*dDrawingScale << _T(",");
		ofile << this->m_oEntTagShape.m_SearchRes.leaders[1].x()*dDrawingScale << _T(",") << this->m_oEntTagShape.m_SearchRes.leaders[1].y()*dDrawingScale << _T("|");
		ofile << env.m_pipe.m_int.m_oLeaderProp.level << _T(",") << env.m_pipe.m_int.m_oLeaderProp.color << _T(",");
		ofile << env.m_pipe.m_int.m_sArrowType << _T(",") << env.m_pipe.m_int.m_dArrowSize*dDrawingScale << _T(",") << env.m_pipe.m_int.m_dArrowSize/3.0*dDrawingScale << std::endl;
	}
	
	if((_T("Only LineNo") == env.m_pipe.m_int.m_sDisplayStyle) || (_T("LineNo+Elev") == env.m_pipe.m_int.m_sDisplayStyle))
	{
		CIsPoint2d pt = this->m_oEntTagShape.m_SearchRes.leaders[0] + m_oEntTagShape.m_SearchRes.cAxis.yAxis*env.m_pipe.m_int.m_dOffset;
		
		ofile << _T("<TEXT>|");
		ofile << pt.x()*dDrawingScale << _T(",") << pt.y()*dDrawingScale << _T(",0,");
		ofile << env.m_pipe.m_int.m_oLineNoTextStyle.height()*dDrawingScale << _T(",") << env.m_pipe.m_int.m_oLineNoTextStyle.width()*dDrawingScale << _T(",");
		ofile << RAD2DEG(this->m_oEntTagShape.m_SearchRes.radian) << _T(",");
		ofile << (*(this->m_oEntTagShape.m_pTagStringList))[0] << _T(",Left Bottom,0|");
		ofile << env.m_pipe.m_int.m_oLineNoProp.level << _T(",") << env.m_pipe.m_int.m_oLineNoProp.color << _T(",");
		ofile << env.m_pipe.m_int.m_oLineNoTextStyle << std::endl;
	}
	else if(_T("LineNo/Elev") == env.m_pipe.m_int.m_sDisplayStyle)
	{
		CIsPoint2d pt = this->m_oEntTagShape.m_SearchRes.coords[0] + m_oEntTagShape.m_SearchRes.cAxis.yAxis*(env.m_pipe.m_int.m_oElevTextStyle.height() + env.m_pipe.m_int.m_dOffset*2.0);

		ofile << _T("<TEXT>|");
		ofile << pt.x()*dDrawingScale << _T(",") << pt.y()*dDrawingScale << _T(",0,");
		ofile << env.m_pipe.m_int.m_oLineNoTextStyle.height()*dDrawingScale << _T(",") << env.m_pipe.m_int.m_oLineNoTextStyle.width()*dDrawingScale << _T(",");
		ofile << RAD2DEG(this->m_oEntTagShape.m_SearchRes.radian) << _T(",");
		ofile << (*(this->m_oEntTagShape.m_pTagStringList))[0] << _T(",Left Bottom,0|");
		ofile << env.m_pipe.m_int.m_oLineNoProp.level << _T(",") << env.m_pipe.m_int.m_oLineNoProp.color << _T(",");
		ofile << env.m_pipe.m_int.m_oLineNoTextStyle << std::endl;

		pt = this->m_oEntTagShape.m_SearchRes.coords[0];

		if(this->m_oEntTagShape.m_pTagStringList->size() > 1)
		{
			ofile << _T("<TEXT>|");
			ofile << pt.x()*dDrawingScale << _T(",") << pt.y()*dDrawingScale << _T(",0,");
			ofile << env.m_pipe.m_int.m_oElevTextStyle.height()*dDrawingScale << _T(",") << env.m_pipe.m_int.m_oElevTextStyle.width()*dDrawingScale << _T(",");
			ofile << RAD2DEG(this->m_oEntTagShape.m_SearchRes.radian) << _T(",");
			ofile << (*(this->m_oEntTagShape.m_pTagStringList))[1] << _T(",Left Bottom,0|");
			ofile << env.m_pipe.m_int.m_oElevProp.level << _T(",") << env.m_pipe.m_int.m_oElevProp.color << _T(",");
			ofile << env.m_pipe.m_int.m_oElevTextStyle << std::endl;
		}
	}

	ofile << _T("</GROUP>") << std::endl;

	return ERROR_SUCCESS;
}