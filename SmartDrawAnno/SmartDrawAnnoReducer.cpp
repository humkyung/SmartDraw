#include "StdAfx.h"
#include <tchar.h>
#include <aDraw_View.h>
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoReducer.h"

#include <sstream>

/******************************************************************************
    @author     humkyung
    @date       2012-03-18
    @function   ENV::Parse
    @return     int
    @param      const           STRING_T&
    @param      sIniFilePath
    @brief
******************************************************************************/
int CaDraw_TagReducer::ENV::Parse(const STRING_T& sIniFilePath)
{
	static const STRING_T sApp(_T("Reducer"));
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	if(GetPrivateProfileString(sApp.c_str() , _T("Name_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sNameStyle = szBuf;
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("ECC_Labeling") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sECCLabeling = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("CON_Labeling") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sCONLabeling = szBuf;
	}

	/// read size unit - 2016.02.11 added by humkyung
	m_iSizeUnit = CaDraw_TagEnv::INCH_FEET;
	if(GetPrivateProfileString(sApp.c_str() , _T("SizeUnit") , _T("Inch Feet") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_iSizeUnit = (STRING_T(_T("Inch Feet")) == STRING_T(szBuf)) ? CaDraw_TagEnv::INCH_FEET : CaDraw_TagEnv::METRIC;
	}
	/// up to here

	if(GetPrivateProfileString(sApp.c_str() , _T("TextStyle") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oTextStyle.Parse(szBuf , sIniFilePath);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Offset") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_dLabelOffset = ATOF_T(szBuf);
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLabelProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLabelProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLabelProp.style = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLabelProp.weight = szBuf;
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Type") , _T("CLOSE") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sArrowType = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Size") , _T("1.5") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_dArrowSize = ATOF_T(szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.style = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.weight = szBuf;
	}

	return ERROR_SUCCESS;
}

SMARTDRAW_TAG_IMPLEMENT_FUNC(CaDraw_TagReducer , CaDraw_TagEntity , _T("Reducer"))

CaDraw_TagReducer::CaDraw_TagReducer(CaDraw_Entity* unnamed) : CaDraw_TagEntity(unnamed)
{
}

CaDraw_TagReducer::~CaDraw_TagReducer(void)
{
}

/**	
	@brief	return reducer's name
	@author	humkyung
	@date	2010.06.12
	@param	
	@return	
*/
STRING_T CaDraw_TagReducer::name(const bool& csvf) const
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	const CaDraw_Reducer::ReducerType oReduType = this->reducer()->type();
	
	CSmartDrawAnnoModule& module = CSmartDrawAnnoModule::GetInstance();

	const double dSize1 = (CaDraw_TagEnv::INCH_FEET == env.m_reducer.m_iSizeUnit) ? reducer()->size1() : reducer()->GetSize1InMetric();
	const double dSize2 = (CaDraw_TagEnv::INCH_FEET == env.m_reducer.m_iSizeUnit) ? reducer()->size2() : reducer()->GetSize2InMetric();
	STRING_T sSize1 , sSize2;
	if(CaDraw_TagEnv::INCH_FEET == env.m_reducer.m_iSizeUnit)
	{
		sSize1 = module.INCH2STR(dSize1);
		sSize2 = module.INCH2STR(dSize2);
	}
	else
	{
		OSTRINGSTREAM_T oss;
		oss << SAFE_ROUND(dSize1,1);
		sSize1 = oss.str();

		oss.str(_T(""));
		oss << SAFE_ROUND(dSize2,1);
		sSize2 = oss.str();
	}
	
	OSTRINGSTREAM_T oss;
	if(_T("Type1") == env.m_reducer.m_sNameStyle)
	{
		if(CaDraw_Reducer::ECC == oReduType)
		{
			if(!env.m_reducer.m_sECCLabeling.empty())
			{
				oss << env.m_reducer.m_sECCLabeling << _T(" ");
			}
			oss << sSize1 << _T("x") << sSize2;
		}
		else if(CaDraw_Reducer::CON == oReduType)
		{
			if(!env.m_reducer.m_sCONLabeling.empty())
			{
				oss << env.m_reducer.m_sCONLabeling << _T(" ");
			}
			oss << sSize1 << _T("x") << sSize2;
		}
	}
	else
	{
		if(CaDraw_Reducer::ECC == oReduType)
		{
			oss << sSize1 << _T("x") << sSize2;
			if(!env.m_reducer.m_sECCLabeling.empty())
			{
				oss << _T(" ") << env.m_reducer.m_sECCLabeling;
			}
		}
		else if(CaDraw_Reducer::CON == oReduType)
		{
			oss << sSize1 << _T("x") << sSize2;
			if(!env.m_reducer.m_sCONLabeling.empty())
			{
				oss << _T(" ") << env.m_reducer.m_sCONLabeling;
			}
		}
	}
	
	STRING_T res(oss.str());
	if(true == csvf)
	{
		res = CSmartDrawAnnoModule::CSVString(res);
	}

	return res;
}

/**	
	@brief	annotate reducer item in inside of view.
	@author	humkyung
	@date	2010.06.12
	@param	
	@return	
*/
int CaDraw_TagReducer::Annotate()
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	CaDraw_View* pDrawView = reducer()->GetView();
	CIsPoint3d ptOrigin;
	ptOrigin = pDrawView->ModelCoord2ViewCoord(reducer()->origin());
	ptOrigin.z() = 0.f;

	list<CCellMap::SearchRes> SearchResList;

	const STRING_T sTagName(this->name());
	const double nTextLength = env.m_reducer.m_oTextStyle.GetTextLength(sTagName);
	const double dItemWidth = nTextLength;
	const double dItemHeight= (env.m_reducer.m_oTextStyle.height() + env.m_reducer.m_dLabelOffset);
	const double nItemCellHeight = dItemHeight * CSmartDrawAnnoModule::m_pCellMap->scale();
	const double nItemCellLength = dItemWidth * CSmartDrawAnnoModule::m_pCellMap->scale();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//! searching...
	double nSearchCellLength = nItemCellLength * 5;	//! this value comes from experience.

	CCellMap::CellConversionAxis cAxis;
	cAxis.xAxis.Set(1 , 0);
	cAxis.yAxis.Set(0 , 1);

	CCellMap::CellIndex tagOrigin(CSmartDrawAnnoModule::m_pCellMap->PointToIndex(CIsPoint2d(ptOrigin.x() , ptOrigin.y())));
	CCellMap::CellIndex cOrigin(tagOrigin);
	cOrigin.x -= nSearchCellLength * 0.5;
	cOrigin.y -= nSearchCellLength * 0.5;

	CCellMap::SIZE_T sizSearch , sizItem;
	sizSearch.width = nSearchCellLength;
	sizSearch.height= nSearchCellLength;
	sizItem.width = nItemCellLength;
	sizItem.height= nItemCellHeight;

	CCellMap::SearchRes res;
	STRNCPY_T(res.szString , this->name(true).c_str() , 255);
	bool bFound = CSmartDrawAnnoModule::m_pCellMap->FindBestLoc(res , cOrigin , tagOrigin , 
		sizSearch , sizItem , cAxis , 0xFF);
	if(bFound)
	{
		res.coords[0] = res.ptLoc;
		res.coords[1] = res.coords[0] + res.cAxis.xAxis*(dItemWidth);
		res.coords[2] = res.coords[1] + (res.cAxis.yAxis*dItemHeight);
		res.coords[3] = res.coords[2] - res.cAxis.xAxis*(dItemWidth);
		
		res.leaders[0] = res.coords[0];
		res.leaders[1] = res.coords[1];
		
		CIsPoint2d at(ptOrigin.x() , ptOrigin.y());
		if(at.DistanceTo(res.leaders[0]) > at.DistanceTo(res.leaders[1]))
		{
			swap(res.leaders[0] , res.leaders[1]);
		}

		res.ptLoc += res.cAxis.yAxis*env.m_reducer.m_dLabelOffset;

		SearchResList.push_back(res);
	}

	if(!SearchResList.empty())
	{
		stable_sort(SearchResList.begin(), SearchResList.end(), CCellMap::fnResSort);	/// sort

		CCellMap::SearchRes res = SearchResList.front();
		const double nTextLength = env.m_reducer.m_oTextStyle.GetTextLength(res.szString);

		CSmartDrawAnnoModule::m_pCellMap->UpdateRectOf(res.cIndex , nItemCellLength , nItemCellHeight , res.cAxis , 155);

		//CCellMap::CellIndex tagOrigin = CSmartDrawAnnoModule::m_pCellMap->PointToIndex(res.coords[0] + ((res.cAxis.yAxis)*env.m_equi.m_NameTextStyle.m_nHeight*0.5));
		//CCellMap::CellIndex cOrigin = CSmartDrawAnnoModule::m_pCellMap->PointToIndex(res.coords[0]);

		/*CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[0] , res.coords[1] , 3);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[1] , res.coords[2] , 3);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[2] , res.coords[3] , 3);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[3] , res.coords[0] , 3);

		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(cOrigin   , cOrigin   , 1);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(tagOrigin , tagOrigin , 2);*/

		if((res.radian > PI*0.5) && (res.radian < PI*1.5))
		{
			//! 글자가 뒤집혀 보이므로 보정해 준다.
			res.ptLoc += (res.cAxis.xAxis * nTextLength) + (res.cAxis.yAxis * env.m_reducer.m_oTextStyle.height());
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
	@brief	write reducer tag
	@author	HumKyung.BAEK
	@date	2010.06.12
	@param	
	@return	
*/
int CaDraw_TagReducer::Write(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	ofile << _T("<GROUP>|0,REDU,Pat 02") << std::endl;

	CaDraw_View* pDrawView = reducer()->GetView();
	const CIsPoint3d ptOrigin = pDrawView->ModelCoord2ViewCoord(reducer()->origin());
	ofile << _T("<LEADER>|") << (ptOrigin.x()*dDrawingScale) << _T(",") << (ptOrigin.y()*dDrawingScale) << _T(",");
	ofile << (m_oEntTagShape.m_SearchRes.leaders[0].x()*dDrawingScale) << _T(",") << (m_oEntTagShape.m_SearchRes.leaders[0].y()*dDrawingScale) << _T(",");
	ofile << (m_oEntTagShape.m_SearchRes.leaders[1].x()*dDrawingScale) << _T(",") << (m_oEntTagShape.m_SearchRes.leaders[1].y()*dDrawingScale) << _T("|");
	ofile << env.m_reducer.m_oLeaderProp.level << _T(",") << env.m_reducer.m_oLeaderProp.color << _T(",");
	ofile << env.m_reducer.m_sArrowType << _T(",") << (env.m_reducer.m_dArrowSize*dDrawingScale) << _T(",") << (env.m_reducer.m_dArrowSize/3.0*dDrawingScale) << _T(",");
	ofile << env.m_reducer.m_oLeaderProp.weight << std::endl;

	ofile << _T("<TEXT>|");
	double d = m_oEntTagShape.m_SearchRes.ptLoc.x(); ofile << (d*dDrawingScale) << _T(",");
	d = m_oEntTagShape.m_SearchRes.ptLoc.y(); ofile << (d*dDrawingScale) << _T(",0,");
	ofile << (env.m_reducer.m_oTextStyle.height()*dDrawingScale) << _T(",");
	ofile << (env.m_reducer.m_oTextStyle.height() * env.m_reducer.m_oTextStyle.GetWidthFactor()*dDrawingScale) << _T(",");
	ofile << RAD2DEG(m_oEntTagShape.m_SearchRes.radian) << _T(",");
	ofile << (*m_oEntTagShape.m_pTagStringList)[0] << _T(",Left Bottom,0|");
	ofile << env.m_reducer.m_oLabelProp.level << _T(",") << env.m_reducer.m_oLabelProp.color << _T(",");
	ofile << (env.m_reducer.m_oTextStyle) << std::endl;

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
CaDraw_TagItem* CaDraw_TagReducer::CreateTagObjects(list<CaDraw_TagItem*>& TagItemList , CSmartDrawAnnoView *pAnnoView)
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
CaDraw_TagItem* CaDraw_TagReducer::OnCreateTagObject(CSmartDrawAnnoView *pAnnoView , const CIsPoint3d& ptOrigin,const CIsVect3d& vec,const DITEM_E& enmType)
{
	CaDraw_TagItem* pTagItem=NULL;
	
	return pTagItem;
}