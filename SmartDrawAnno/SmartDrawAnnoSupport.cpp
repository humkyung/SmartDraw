#include "StdAfx.h"
#include <assert.h>
#include <IsString.h>
#include <aDraw_View.h>
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoSupport.h"

///using namespace tag_entity;

SMARTDRAW_TAG_IMPLEMENT_FUNC(CaDraw_TagSupport , CaDraw_TagEntity , _T("Support"))

CaDraw_TagSupport::CaDraw_TagSupport(CaDraw_Entity* unnamed) : CaDraw_TagEntity(unnamed)
{
}

CaDraw_TagSupport::~CaDraw_TagSupport(void)
{
}

double fnSupportDistanceTo(CCellMap::SearchRes* pSearchRes , const CCellMap::SIZE_T& sizItem , const CCellMap::CellIndex& tagOrigin)
{
	assert(pSearchRes && "pSearchRes is NULL");
	double res = DBL_MAX;

	if(pSearchRes)
	{
		double aryLength[4]={0,};
		
		CIsPoint2d at;	
		at.Set(pSearchRes->cIndex.x , pSearchRes->cIndex.y);
		aryLength[0] = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));
		
		at.Set(pSearchRes->cIndex.x + sizItem.width , pSearchRes->cIndex.y);
		aryLength[1] = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));

		at.Set(pSearchRes->cIndex.x + sizItem.width , pSearchRes->cIndex.y + sizItem.height);
		aryLength[2] = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));

		at.Set(pSearchRes->cIndex.x , pSearchRes->cIndex.y + sizItem.height);
		aryLength[3] = at.DistanceTo(CIsPoint2d(tagOrigin.x , tagOrigin.y));

		::stable_sort(aryLength , aryLength + 4);
		res = aryLength[0];
	}

	return res;
}

/**	
	@brief	annotate support in inside of view.
	@author	humkyung
	@date	2010.06.12
	@param	
	@return	
*/
int CaDraw_TagSupport::Annotate()
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	CaDraw_View* pDrawView = support()->GetView();
	CIsPoint3d ptOrigin;
	ptOrigin = pDrawView->ModelCoord2ViewCoord(support()->origin());
	ptOrigin.z() = 0.f;

	STRING_T sTagString;
	{
		OSTRINGSTREAM_T oss;
		CSupportOption::eDisplayName disp = env.m_support.GetDisplayName();
		switch(disp)
		{
			case CSupportOption::SUPPORT_NO:
				oss << support()->name();
				break;
			case CSupportOption::SHOP_DETAIL:
				oss << support()->shop_name();
				break;
			case CSupportOption::FIELD_DETAIL:
				oss << support()->field_name();
				break;
			case CSupportOption::COMMODITY_CODE:
				oss << support()->commodity_code();
				break;
		}

		if(_T("Name+Elevation") == env.m_support.m_sDisplayType)
		{
			oss << _T(" EL. ") << IsString::TrimedTrailingZero(SAFE_ROUND(support()->origin().z() , 0));
		}
		sTagString = oss.str();
	}

	list<CCellMap::SearchRes> SearchResList;

	const double dTextLength = env.m_support.m_oTextStyle.GetTextLength(sTagString);
	const double dItemWidth = dTextLength + env.m_support.m_dLabelOffset*2;
	const double dItemHeight= env.m_support.m_oTextStyle.height() + env.m_support.m_dLabelOffset*2;
	const double dItemCellHeight = dItemHeight * CSmartDrawAnnoModule::m_pCellMap->scale();		
	const double dItemCellLength = dItemWidth * CSmartDrawAnnoModule::m_pCellMap->scale();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// HORIZONTAL SEARCH
	double dSearchCellLength = dItemCellLength * 4;	/// this value comes from experience.
	double dSearchCellHeight = dItemCellLength;		/// this value comes from experience.
	env.m_generation.GetSearchAreaOf(dSearchCellLength , dSearchCellHeight , CaDraw_TagSupport::TypeString());
	
	CCellMap::SIZE_T sizSearch , sizItem;
	sizSearch.width = ceil(dSearchCellLength);
	sizSearch.height= ceil(dSearchCellHeight);
	sizItem.width = ceil(dItemCellLength);
	sizItem.height= ceil(dItemCellHeight);

	const CGenerationOption::eSearchType eSearchType = env.m_generation.GetSearchTypeOf(CaDraw_TagSupport::TypeString());
	if((CGenerationOption::SEARCH_ORGHOGONAL == eSearchType) || (CGenerationOption::SEARCH_HORIZONTAL == eSearchType))
	{
		sizSearch.height  = sizItem.height;
		dSearchCellHeight = sizItem.height;
	}

	CCellMap::CellConversionAxis cAxis;
	
	CCellMap::CellIndex tagOrigin(CSmartDrawAnnoModule::m_pCellMap->PointToIndex(CIsPoint2d(ptOrigin.x() , ptOrigin.y())));
	CCellMap::CellIndex cOrigin(tagOrigin);
	if((CGenerationOption::SEARCH_ORGHOGONAL == eSearchType) || (CGenerationOption::SEARCH_HORIZONTAL == eSearchType) || (CGenerationOption::SEARCH_AROUND == eSearchType))
	{
		cAxis.xAxis.Set(1 , 0);
		cAxis.yAxis.Set(0 , 1);

		cOrigin.x -= ceil(dSearchCellLength*0.5);
		cOrigin.y -= ((CGenerationOption::SEARCH_ORGHOGONAL == eSearchType) || (CGenerationOption::SEARCH_HORIZONTAL == eSearchType)) ? 0 : ceil(dSearchCellHeight*0.5);

		CCellMap::SearchRes res;
		STRNCPY_T(res.szString , sTagString.c_str() , 255);
		const bool bFound = CSmartDrawAnnoModule::m_pCellMap->FindBestLoc(res,cOrigin,tagOrigin,sizSearch,sizItem,cAxis,0xFF,fnSupportDistanceTo);
		if(bFound)
		{
			res.ptTagOrigin = CIsPoint2d(ptOrigin.x() , ptOrigin.y());

			/// adjust y position
			if((CGenerationOption::SEARCH_ORGHOGONAL == eSearchType) || (CGenerationOption::SEARCH_HORIZONTAL == eSearchType))
			{
				res.ptLoc.Set(res.ptLoc.x() , ptOrigin.y());	/// set loc.y to y pos of support
			}
			res.coords[0] = res.ptLoc;
			res.coords[1] = res.coords[0] + res.cAxis.xAxis*(dItemWidth);
			res.coords[2] = res.coords[1] + res.cAxis.yAxis*(dItemHeight);
			res.coords[3] = res.coords[2] - cAxis.xAxis*(dItemWidth);
			
			SearchResList.push_back(res);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// VERTICAL SEARCH
	if((CGenerationOption::SEARCH_ORGHOGONAL == eSearchType) || (CGenerationOption::SEARCH_VERTICAL == eSearchType) || (CGenerationOption::SEARCH_AROUND == eSearchType))
	{
		cAxis.xAxis.Set(0 , 1);
		cAxis.yAxis.Set(1, 0);

		cOrigin = tagOrigin;
		cOrigin.x -= ((CGenerationOption::SEARCH_ORGHOGONAL == eSearchType) || (CGenerationOption::SEARCH_VERTICAL == eSearchType)) ? 0 : ceil(dSearchCellHeight * 0.5);
		cOrigin.y -= ceil(dSearchCellLength * 0.5);

		CCellMap::SearchRes res;
		STRNCPY_T(res.szString , sTagString.c_str() , 255);

		const bool bFound = CSmartDrawAnnoModule::m_pCellMap->FindBestLoc(res,cOrigin,tagOrigin,sizSearch,sizItem,cAxis,0xFF,fnSupportDistanceTo);
		if(bFound)
		{
			res.ptTagOrigin = CIsPoint2d(ptOrigin.x() , ptOrigin.y());

			/// adjust x position
			if((CGenerationOption::SEARCH_ORGHOGONAL == eSearchType) || (CGenerationOption::SEARCH_VERTICAL == eSearchType))
			{
				res.ptLoc.Set(ptOrigin.x() , res.ptLoc.y());	/// set loc.x to x pos of support
			}
			res.coords[0] = res.ptLoc;
			res.coords[1] = res.coords[0] + res.cAxis.xAxis*(dItemWidth);
			res.coords[2] = res.coords[1] + res.cAxis.yAxis*(dItemHeight);
			res.coords[3] = res.coords[2] - res.cAxis.xAxis*(dItemWidth);
			SearchResList.push_back(res);
		}
	}
	/// up to here

	if(!SearchResList.empty())
	{
		stable_sort(SearchResList.begin(), SearchResList.end(), CCellMap::fnResSort);  /// sorting
		CCellMap::SearchRes res = SearchResList.front();

		CSmartDrawAnnoModule::m_pCellMap->UpdateRectOf(res.cIndex , dItemCellLength , dItemCellHeight , res.cAxis , 155);

		if((res.radian > PI*0.5) && (res.radian < PI*1.5))
		{
			/// 글자가 뒤집혀 보이므로 보정해 준다.
			res.ptLoc += (res.cAxis.xAxis * dTextLength) + (res.cAxis.yAxis * env.m_equi.m_oNameTextStyle.height());
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
	@brief	write support tag
	@author	HumKyung.BAEK
	@date	2010.06.12
	@param	
	@return	
*/
int CaDraw_TagSupport::Write(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	if(CaDraw_EntTagShape::RECTANGLE == env.m_support.m_iEnclosureType)
	{
		ofile << _T("<GROUP>|0,SUPP,Pat 08") << std::endl;
		m_oEntTagShape.Write(ofile , CaDraw_EntTagShape::RECTANGLE , env.m_support.m_oLeaderProp , dDrawingScale);
	}
	else
	{
		ofile << _T("<GROUP>|0,SUPP,Pat 03") << std::endl;
	}

	const CIsPoint2d center((m_oEntTagShape.m_SearchRes.coords[0] + m_oEntTagShape.m_SearchRes.coords[2]) * 0.5);
	ofile << _T("<LEADER>|");
	ofile << (m_oEntTagShape.m_SearchRes.ptTagOrigin*dDrawingScale) << _T(",");
	
	CIsPoint2d ptMin(m_oEntTagShape.m_SearchRes.coords[0]);
	double dMinDistance = ptMin.DistanceTo(m_oEntTagShape.m_SearchRes.ptTagOrigin);
	for(int i = 1;i < 4;++i)
	{
		const double d = m_oEntTagShape.m_SearchRes.coords[i].DistanceTo(m_oEntTagShape.m_SearchRes.ptTagOrigin);
		if(d < dMinDistance)
		{
			dMinDistance = d;
			ptMin = m_oEntTagShape.m_SearchRes.coords[i];
		}
	}
	ofile << ptMin*dDrawingScale << _T("|");
	ofile << env.m_support.m_oLeaderProp.level << _T(",") << env.m_support.m_oLeaderProp.color << _T(",");
	ofile << env.m_support.m_sArrowType << _T(",") << env.m_support.m_dArrowSize*dDrawingScale << _T("," ) << env.m_support.m_dArrowSize/3.0*dDrawingScale << std::endl;

	for(vector<STRING_T>::iterator itr = m_oEntTagShape.m_pTagStringList->begin();itr != m_oEntTagShape.m_pTagStringList->end();++itr)
	{
		ofile << _T("<TEXT>|");
		ofile << (center*dDrawingScale) << _T(",0,");
		ofile << env.m_support.m_oTextStyle.height()*dDrawingScale << _T(",") << (env.m_support.m_oTextStyle.height()*env.m_support.m_oTextStyle.GetWidthFactor())*dDrawingScale << _T(",");
		ofile << RAD2DEG(m_oEntTagShape.m_SearchRes.radian) << _T(",");
		ofile << CSmartDrawAnnoModule::CSVString(*itr) << _T(",Center Center,0|");
		ofile << env.m_support.m_oLabelProp.level << _T(",") << env.m_support.m_oLabelProp.color << _T(",");
		ofile << env.m_support.m_oTextStyle << std::endl;
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
CaDraw_TagItem* CaDraw_TagSupport::CreateTagObjects(list<CaDraw_TagItem*>& TagItemList , CSmartDrawAnnoView *pAnnoView)
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
CaDraw_TagItem* CaDraw_TagSupport::OnCreateTagObject(CSmartDrawAnnoView *pAnnoView , const CIsPoint3d& ptOrigin,const CIsVect3d& vec,const DITEM_E& enmType)
{
	CaDraw_TagItem* pTagItem=NULL;
	
	return pTagItem;
}