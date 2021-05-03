#include "StdAfx.h"
#include <aDraw_View.h>
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoInstrument.h"

#include <Tokenizer.h>
///using namespace tag_entity;

SMARTDRAW_TAG_IMPLEMENT_FUNC(CaDraw_TagInstrument , CaDraw_TagEntity , _T("Instrument"))

CaDraw_TagInstrument::CaDraw_TagInstrument(CaDraw_Entity* unnamed) : CaDraw_TagEntity(unnamed)
{
}

CaDraw_TagInstrument::~CaDraw_TagInstrument(void)
{
}

/**	
	@brief	annotate instrument tag in inside of view.

	@author	humkyung

	@date	2010.06.12

	@param	

	@return	
*/
int CaDraw_TagInstrument::Annotate()
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	CaDraw_View* pDrawView = inst()->GetView();
	CIsPoint3d ptOrigin;
	ptOrigin = pDrawView->ModelCoord2ViewCoord(inst()->origin());
	ptOrigin.z() = 0.f;

	list<CCellMap::SearchRes> SearchResList;

	const double dTextLength= env.m_inst.m_oTextStyle.GetTextLength(inst()->name());
	const double dItemWidth = env.m_inst.m_dEnclosureSizeB;
	const double dItemHeight= env.m_inst.m_dEnclosureSizeA;
	const double dItemCellHeight   = dItemHeight * CSmartDrawAnnoModule::m_pCellMap->scale();		
	const double dItemCellLength   = dItemWidth  * CSmartDrawAnnoModule::m_pCellMap->scale();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//! SEARCH
	const double nSearchCellLength = dItemCellLength * 6;	//! this value comes from experience.

	CCellMap::SIZE_T sizSearch , sizItem;
	sizSearch.width = nSearchCellLength;
	sizSearch.height= nSearchCellLength;
	sizItem.width = dItemCellLength;
	sizItem.height= dItemCellHeight;

	CCellMap::CellConversionAxis cAxis;
	cAxis.xAxis.Set(1 , 0);
	cAxis.yAxis.Set(0 , 1);

	CCellMap::CellIndex tagOrigin(CSmartDrawAnnoModule::m_pCellMap->PointToIndex(CIsPoint2d(ptOrigin.x() , ptOrigin.y())));
	CCellMap::CellIndex cOrigin(tagOrigin);
	cOrigin.x -= nSearchCellLength * 0.5;
	cOrigin.y -= nSearchCellLength * 0.5;

	CCellMap::SearchRes res;
	STRNCPY_T(res.szString , inst()->name() , 255);
	bool bFound = CSmartDrawAnnoModule::m_pCellMap->FindBestLoc(res , cOrigin , tagOrigin , 
		sizSearch , sizItem , cAxis , 0xFF);
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

		CSmartDrawAnnoModule::m_pCellMap->UpdateRectOf(res.cIndex , dItemCellLength , dItemCellHeight , res.cAxis , 155);

		///CCellMap::CellIndex tagOrigin = CSmartDrawAnnoModule::m_pCellMap->PointToIndex(res.coords[0] + ((res.cAxis.yAxis)*env.m_inst.m_NameTextStyle.m_nHeight*0.5));
		///CCellMap::CellIndex cOrigin = CSmartDrawAnnoModule::m_pCellMap->PointToIndex(res.coords[0]);

		/*CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[0] , res.coords[1] , 3);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[1] , res.coords[2] , 3);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[2] , res.coords[3] , 3);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[3] , res.coords[0] , 3);

		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(cOrigin   , cOrigin   , 1);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(tagOrigin , tagOrigin , 2);*/

		if((res.radian > PI*0.5) && (res.radian < PI*1.5))
		{
			//! 글자가 뒤집혀 보이므로 보정해 준다.
			res.ptLoc += (res.cAxis.xAxis * dTextLength) + (res.cAxis.yAxis * env.m_inst.m_oTextStyle.height());
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
	@brief	write instrument tag
	@author	HumKyung.BAEK
	@date	2010.06.12
	@param	
	@return	
*/
int CaDraw_TagInstrument::Write(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	vector<STRING_T> oResult;
	if(!env.m_inst.m_sDelimiter.empty())
	{
		CTokenizer<CIsFromString>::Tokenize(oResult , (*m_oEntTagShape.m_pTagStringList)[0] , CIsFromString(env.m_inst.m_sDelimiter));
	}
	else
	{
		oResult.push_back((*m_oEntTagShape.m_pTagStringList)[0]);
	}
	
	ofile << _T("<GROUP>|0,INST,Pat 04") << std::endl;

	m_oEntTagShape.Write(ofile , env.m_inst.m_iEnclosureType , env.m_inst.m_oLeaderProp , dDrawingScale);
	CIsPoint2d center((m_oEntTagShape.m_SearchRes.coords[0] + m_oEntTagShape.m_SearchRes.coords[2]) * 0.5);
	if(env.m_inst.m_bEnclosureCenterLine && (oResult.size() > 1))
	{
		ofile << _T("<LINE>|");
		ofile << (m_oEntTagShape.m_SearchRes.coords[0]*dDrawingScale) << _T(",0,");
		ofile << (m_oEntTagShape.m_SearchRes.coords[1]*dDrawingScale) << _T(",0|");
		ofile << env.m_inst.m_oLeaderProp << std::endl;
	}

	ofile << _T("<LEADER>|");
	ofile << (m_oEntTagShape.m_SearchRes.ptTagOrigin*dDrawingScale) << _T(",");
	if(CaDraw_EntTagShape::CIRCLE == env.m_inst.m_iEnclosureType)
	{
		CIsVect2d dir(m_oEntTagShape.m_SearchRes.ptTagOrigin - center);
		dir.Normalize();
		CIsPoint2d tmp(center + dir*(env.m_inst.m_dEnclosureSizeA*0.5));
		ofile << (tmp*dDrawingScale) << _T("|");
	}
	else
	{
		ofile << (center*dDrawingScale) << _T("|");
	}
	ofile << env.m_inst.m_oLeaderProp.level << _T(",") << env.m_inst.m_oLeaderProp.color << _T(",");
	ofile << env.m_inst.m_sArrowType << _T(",") << env.m_inst.m_dArrowSize*dDrawingScale << _T("," ) << env.m_inst.m_dArrowSize/3.0*dDrawingScale << std::endl;

	const double nStep = env.m_inst.m_dEnclosureSizeA / oResult.size();
	int nIndex = 0;
	for(vector<STRING_T>::iterator itr = oResult.begin();itr != oResult.end();++itr,++nIndex)
	{
		ofile << _T("<TEXT>|");
		ofile << center.x()*dDrawingScale << _T(",");
		ofile << (m_oEntTagShape.m_SearchRes.coords[0].y() + env.m_inst.m_dEnclosureSizeA - nIndex*nStep - nStep*0.5)*dDrawingScale << _T(",0,");
		ofile << env.m_inst.m_oTextStyle.height()*dDrawingScale << _T(",") << (env.m_inst.m_oTextStyle.height()*env.m_inst.m_oTextStyle.GetWidthFactor())*dDrawingScale << _T(",");
		ofile << RAD2DEG(m_oEntTagShape.m_SearchRes.radian) << _T(",");
		ofile << (*itr) << _T(",Center Center,0|");
		ofile << env.m_inst.m_oLabelProp.level << _T(",") << env.m_inst.m_oLabelProp.color << _T(",");
		ofile << env.m_inst.m_oTextStyle << std::endl;
	}

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
CaDraw_TagItem* CaDraw_TagInstrument::CreateTagObjects(list<CaDraw_TagItem*>& TagItemList , CSmartDrawAnnoView *pAnnoView)
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
CaDraw_TagItem* CaDraw_TagInstrument::OnCreateTagObject(CSmartDrawAnnoView *pAnnoView , const CIsPoint3d& ptOrigin,const CIsVect3d& vec,const DITEM_E& enmType)
{
	CaDraw_TagItem* pTagItem=NULL;
	
	return pTagItem;
}