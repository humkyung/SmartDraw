#include "StdAfx.h"
#include <assert.h>
#include <aDraw_View.h>
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoNozzle.h"

#include <fstream>

///using namespace tag_entity;

SMARTDRAW_TAG_IMPLEMENT_FUNC(CaDraw_TagNozzle , CaDraw_TagEntity , _T("Nozzle"))

CaDraw_TagNozzle::CaDraw_TagNozzle(CaDraw_Entity* unnamed) : CaDraw_TagEntity(unnamed)
{
}

CaDraw_TagNozzle::~CaDraw_TagNozzle(void)
{
}

double fnNozzleDistanceTo(CCellMap::SearchRes* pSearchRes , const CCellMap::SIZE_T& sizItem , const CCellMap::CellIndex& tagOrigin)
{
	assert(pSearchRes && "pSearchRes is NULL");
	double res = DBL_MAX;

	if(pSearchRes)
	{
		double aryLength[9]={0,};
		
		CIsPoint2d at;	
		at.Set(pSearchRes->cIndex.x , pSearchRes->cIndex.y);
		aryLength[0] = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));
		
		at.Set(pSearchRes->cIndex.x + sizItem.width*0.5 , pSearchRes->cIndex.y);
		aryLength[1] = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));

		at.Set(pSearchRes->cIndex.x + sizItem.width , pSearchRes->cIndex.y);
		aryLength[2] = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));

		at.Set(pSearchRes->cIndex.x + sizItem.width , pSearchRes->cIndex.y + sizItem.height*0.5);
		aryLength[3] = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));

		at.Set(pSearchRes->cIndex.x + sizItem.width , pSearchRes->cIndex.y + sizItem.height);
		aryLength[4] = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));

		at.Set(pSearchRes->cIndex.x + sizItem.width*0.5 , pSearchRes->cIndex.y + sizItem.height);
		aryLength[5] = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));

		at.Set(pSearchRes->cIndex.x , pSearchRes->cIndex.y + sizItem.height);
		aryLength[6] = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));

		at.Set(pSearchRes->cIndex.x , pSearchRes->cIndex.y + sizItem.height*0.5);
		aryLength[7] = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));

		at.Set(pSearchRes->cIndex.x + sizItem.width*0.5 , pSearchRes->cIndex.y + sizItem.height*0.5);
		aryLength[8] = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));

		::stable_sort(aryLength , aryLength + 8);
		res = aryLength[0];
	}

	return res;
}

/**	
	@brief	annotate nozzle in inside of view.
	@author	humkyung
	@date	2010.06.12
	@param	
	@return	
*/
int CaDraw_TagNozzle::Annotate()
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	CaDraw_View* pDrawView = nozzle()->GetView();
	CIsPoint3d ptOrigin;
	ptOrigin = pDrawView->ModelCoord2ViewCoord(nozzle()->origin());
	ptOrigin.z() = 0.f;

	list<CCellMap::SearchRes> SearchResList;

	const double dItemWidth = env.m_nozzle.m_oTextStyle.GetTextLength(nozzle()->name());
	const double dItemHeight= env.m_nozzle.m_oTextStyle.height();
	const double nItemCellLength = dItemWidth * CSmartDrawAnnoModule::m_pCellMap->scale() + 0.5;
	const double nItemCellHeight = dItemHeight * CSmartDrawAnnoModule::m_pCellMap->scale();		
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//! SEARCH
	double nSearchCellLength = nItemCellLength * 5;	//! this value comes from experience.
	double nSearchCellHeight = nItemCellLength * 5;	//! this value comes from experience.

	CCellMap::SIZE_T sizSearch , sizItem;
	sizSearch.width = nSearchCellLength;
	sizSearch.height= nSearchCellHeight;
	sizItem.width = nItemCellLength;
	sizItem.height= nItemCellHeight;

	CCellMap::CellConversionAxis cAxis;
	cAxis.xAxis.Set(1 , 0);
	cAxis.yAxis.Set(0 , 1);

	CCellMap::CellIndex tagOrigin(CSmartDrawAnnoModule::m_pCellMap->PointToIndex(CIsPoint2d(ptOrigin.x() , ptOrigin.y())));
	CCellMap::CellIndex cOrigin(tagOrigin);
	cOrigin.x -= nSearchCellLength * 0.5;
	cOrigin.y -= nSearchCellHeight * 0.5;

	CCellMap::SearchRes res;
	STRNCPY_T(res.szString , nozzle()->name() , 255);
	bool bFound = CSmartDrawAnnoModule::m_pCellMap->FindBestLoc(res , cOrigin , tagOrigin , 
		sizSearch , sizItem , cAxis , 0xFF , fnNozzleDistanceTo);
	if(bFound)
	{
		res.ptTagOrigin = CIsPoint2d(ptOrigin.x() , ptOrigin.y());

		res.coords[0] = res.ptLoc;
		res.coords[1] = res.coords[0] + (res.cAxis.xAxis * dItemWidth);
		res.coords[2] = res.coords[1] + (res.cAxis.yAxis * dItemHeight);
		res.coords[3] = res.coords[2] - (res.cAxis.xAxis * dItemWidth);

		SearchResList.push_back(res);
	}

	if(!SearchResList.empty())
	{
		stable_sort(SearchResList.begin(), SearchResList.end(), CCellMap::fnResSort);  /// sorting

		CCellMap::SearchRes res = SearchResList.front();
		///const double nTextLength = env.m_equi.m_NameTextStyle.GetTextLength(res.szString);

		CSmartDrawAnnoModule::m_pCellMap->UpdateRectOf(res.cIndex , nItemCellLength , nItemCellHeight , res.cAxis , CCellMap::TAG);

		/*CCellMap::CellIndex tagOrigin = CSmartDrawAnnoModule::m_pCellMap->PointToIndex(res.coords[0] + ((res.cAxis.yAxis)*env.m_equi.m_NameTextStyle.m_nHeight*0.5));
		CCellMap::CellIndex cOrigin = CSmartDrawAnnoModule::m_pCellMap->PointToIndex(res.coords[0]);*/

		/*CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[0] , res.coords[1] , 3);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[1] , res.coords[2] , 3);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[2] , res.coords[3] , 3);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[3] , res.coords[0] , 3);

		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(cOrigin   , cOrigin   , 1);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(tagOrigin , tagOrigin , 2);*/

		if((res.radian > PI*0.5) && (res.radian < PI*1.5))
		{
			//! 글자가 뒤집혀 보이므로 보정해 준다.
			res.ptLoc += (res.cAxis.xAxis * dItemWidth) + (res.cAxis.yAxis * env.m_nozzle.m_oTextStyle.height());
			res.cAxis.xAxis = -res.cAxis.xAxis;
			res.cAxis.yAxis = -res.cAxis.yAxis;
			res.radian = res.cAxis.xAxis.angle();
		}

		m_bTagged = true;
		m_oEntTagShape.m_SearchRes = res;
		m_oEntTagShape.m_pTagStringList->push_back(res.szString);
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	write tag
	@author	HumKyung.BAEK
	@date	2010.06.12
	@param	
	@return	
*/
int CaDraw_TagNozzle::Write(OFSTREAM_T& ofile , const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	ofile << _T("<GROUP>|0,NOZZ,Pat 03") << std::endl;

	vector<CIsPoint2d> output;
	{
		vector<CIsPoint2d> input , clipPolygon;

		CIsPoint2d center;
		center = (m_oEntTagShape.m_SearchRes.coords[0] + m_oEntTagShape.m_SearchRes.coords[2])*0.5;	

		input.push_back(m_oEntTagShape.m_SearchRes.ptTagOrigin);
		input.push_back(center);
		clipPolygon.push_back(m_oEntTagShape.m_SearchRes.coords[0]);
		clipPolygon.push_back(m_oEntTagShape.m_SearchRes.coords[1]);
		clipPolygon.push_back(m_oEntTagShape.m_SearchRes.coords[2]);
		clipPolygon.push_back(m_oEntTagShape.m_SearchRes.coords[3]);
		clipPolygon.push_back(m_oEntTagShape.m_SearchRes.coords[0]);
		SutherlandHodgmanPolygonClip(input , clipPolygon , output);
	}

	ofile << _T("<LEADER>|");
	ofile << m_oEntTagShape.m_SearchRes.ptTagOrigin.x()*dDrawingScale << _T(",") << m_oEntTagShape.m_SearchRes.ptTagOrigin.y()*dDrawingScale << _T(",");
	if(!output.empty())
	{
		ofile << output[0].x()*dDrawingScale << _T(",") << output[0].y()*dDrawingScale;
	}
	ofile << _T("|");
	ofile << env.m_nozzle.m_oLeaderProp.level << _T(",") << env.m_nozzle.m_oLeaderProp.color << _T(",");
	ofile << env.m_nozzle.m_sArrowType << _T(",") << env.m_nozzle.m_dArrowSize << _T(",") << env.m_nozzle.m_dArrowSize/3.0 << std::endl;

	CIsPoint2d at = m_oEntTagShape.m_SearchRes.ptLoc;
	ofile << _T("<TEXT>|");
	ofile << at.x()*dDrawingScale << _T(",") << at.y()*dDrawingScale << _T(",0,");
	ofile << env.m_nozzle.m_oTextStyle.height()*dDrawingScale << _T(",") << (env.m_nozzle.m_oTextStyle.width())*dDrawingScale << _T(",");
	ofile << RAD2DEG(m_oEntTagShape.m_SearchRes.radian) << _T(",");
	ofile << (*m_oEntTagShape.m_pTagStringList)[0] << _T(",Left Bottom,0|");
	ofile << env.m_nozzle.m_oLabelProp.level << _T(",") << env.m_nozzle.m_oLabelProp.color << _T(",");
	ofile << env.m_nozzle.m_oTextStyle << std::endl;

	ofile << _T("</GROUP>") << std::endl;

	return ERROR_SUCCESS;
}

/**	
	@brief	create tag item of tag entity.
	@author	HumKyung.BAEK
	@date	2010.06.12
	@param	
	@return	
*/
CaDraw_TagItem* CaDraw_TagNozzle::CreateTagObjects(list<CaDraw_TagItem*>& TagItemList , CSmartDrawAnnoView *pAnnoView)
{
	TagItemList.clear();
	return NULL;
}

/**	
	@brief	
	@author	BHK
	@date	2010.06.12
	@param	
	@return	CaDraw_TagItem*	
*/
CaDraw_TagItem* CaDraw_TagNozzle::OnCreateTagObject(CSmartDrawAnnoView *pAnnoView , const CIsPoint3d& ptOrigin,const CIsVect3d& vec,const DITEM_E& enmType)
{
	CaDraw_TagItem* pTagItem=NULL;
	
	return pTagItem;
}