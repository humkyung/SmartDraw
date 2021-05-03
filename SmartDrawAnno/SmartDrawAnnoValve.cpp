#include "StdAfx.h"
#include <assert.h>
#include <IsString.h>
#include <aDraw_View.h>
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoValve.h"

#include <Tokenizer.h>

CSmartDrawAnnoValve::ENV::ENV(void)
{
	m_dLabelOffset = 0.0;
}

CSmartDrawAnnoValve::ENV::~ENV(void)
{
}

/**	
	@brief	read valve settings
	@author	humkyung
	@date	2010.05.27
	@param	
	@return
*/
int CSmartDrawAnnoValve::ENV::Parse(const STRING_T& sIniFilePath)
{
	static const STRING_T sApp(_T("Valve"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	/// 2014.12.31 added by humkyung
	GetPrivateProfileString(sApp.c_str() , _T("DisplayName") ,_T("piping_comp_no") ,szBuf,MAX_PATH , sIniFilePath.c_str());
	m_sDisplayName.assign(szBuf);
	/// up to here

	m_sDisplayType = _T("Name+Elevation");
	if(GetPrivateProfileString(sApp.c_str() , _T("DisplayType") ,NULL,szBuf,MAX_PATH , sIniFilePath.c_str()))
	{
		m_sDisplayType = szBuf;
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("EnclosureType") ,_T("Rectangle"),szBuf,MAX_PATH , sIniFilePath.c_str()))
	{
		if(0 == STRICMP_T(szBuf , _T("None")))
		{
			m_iEnclosureType = CaDraw_EntTagShape::NONE;
		}
		else if(0 == STRICMP_T(szBuf , _T("Rectangle")))
		{ 
			m_iEnclosureType = CaDraw_EntTagShape::RECTANGLE;
		}
	}else	m_iEnclosureType = CaDraw_EntTagShape::RECTANGLE;

	
	if(GetPrivateProfileString(sApp.c_str() , _T("TextStyle") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oTextStyle.Parse(szBuf , sIniFilePath);
	}	

	m_dLabelOffset = 0.5;
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Offset") ,NULL,szBuf,MAX_PATH,sIniFilePath.c_str()))
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

	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Type") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sArrowType = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Size") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
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

/**	
	@brief	return display name for valve
	@author	humkyung
	@date	2012.12.31
	@param	
	@return
*/
CSmartDrawAnnoValve::ENV::eDisplayName CSmartDrawAnnoValve::ENV::GetDisplayName() const
{
	if(_T("piping_comp_no") == m_sDisplayName) return CSmartDrawAnnoValve::ENV::PIPING_COMP_NO;
	if(_T("remarks") == m_sDisplayName) return CSmartDrawAnnoValve::ENV::REMARKS;
	return CSmartDrawAnnoValve::ENV::PIPING_COMP_NO;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SMARTDRAW_TAG_IMPLEMENT_FUNC(CSmartDrawAnnoValve , CaDraw_TagEntity , _T("Valve"))

CSmartDrawAnnoValve::CSmartDrawAnnoValve(CaDraw_Entity* unnamed) : CaDraw_TagEntity(unnamed)
{
}

CSmartDrawAnnoValve::~CSmartDrawAnnoValve(void)
{
}

static double fnValveDistanceTo(CCellMap::SearchRes* pSearchRes , const CCellMap::SIZE_T& sizItem , const CCellMap::CellIndex& tagOrigin)
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
	@brief	annotate valve tag in inside of view.
	@author	humkyung
	@date	2012.03.24
	@param	
	@return	
*/
int CSmartDrawAnnoValve::Annotate()
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	CaDraw_View* pDrawView = valve()->GetView();
	CIsPoint3d ptOrigin;
	ptOrigin = pDrawView->ModelCoord2ViewCoord(valve()->origin());
	ptOrigin.z() = 0.f;

	STRING_T sTagString;
	{
		OSTRINGSTREAM_T oss;
		const CSmartDrawAnnoValve::ENV::eDisplayName disp = env.m_valve.GetDisplayName();
		switch(disp)
		{
			case CSmartDrawAnnoValve::ENV::PIPING_COMP_NO:
				oss << valve()->name();
				break;
			case CSmartDrawAnnoValve::ENV::REMARKS:
				oss << valve()->remarks();
				break;
		}

		if(_T("Name+Elevation") == env.m_valve.m_sDisplayType)
		{
			oss << _T(" EL. ") << IsString::TrimedTrailingZero(SAFE_ROUND(valve()->origin().z() , 0));
		}
		sTagString = oss.str();
	}

	list<CCellMap::SearchRes> SearchResList;

	const double dTextLength = env.m_valve.m_oTextStyle.GetTextLength(sTagString);
	const double dItemWidth = dTextLength + env.m_valve.m_dLabelOffset*2;
	const double dItemHeight= env.m_valve.m_oTextStyle.height() + env.m_valve.m_dLabelOffset*2;
	const double dItemCellHeight = dItemHeight * CSmartDrawAnnoModule::m_pCellMap->scale();		
	const double dItemCellLength = dItemWidth * CSmartDrawAnnoModule::m_pCellMap->scale();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// HORIZONTAL SEARCH
	double dSearchCellLength = dItemCellLength * 4;	/// this value comes from experience.
	double dSearchCellHeight = dItemCellLength;		/// this value comes from experience.
	env.m_generation.GetSearchAreaOf(dSearchCellLength , dSearchCellHeight , CSmartDrawAnnoValve::TypeString());
	
	CCellMap::SIZE_T sizSearch , sizItem;
	sizSearch.width = ceil(dSearchCellLength);
	sizSearch.height= ceil(dSearchCellHeight);
	sizItem.width = ceil(dItemCellLength);
	sizItem.height= ceil(dItemCellHeight);

	const CGenerationOption::eSearchType eSearchType = env.m_generation.GetSearchTypeOf(CSmartDrawAnnoValve::TypeString());
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
		const bool bFound = CSmartDrawAnnoModule::m_pCellMap->FindBestLoc(res,cOrigin,tagOrigin,sizSearch,sizItem,cAxis,0xFF,fnValveDistanceTo);
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

		const bool bFound = CSmartDrawAnnoModule::m_pCellMap->FindBestLoc(res,cOrigin,tagOrigin,sizSearch,sizItem,cAxis,0xFF,fnValveDistanceTo);
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
	@brief	write valve tag
	@author	humkyung
	@date	2012.0.24
	@param	
	@return	
*/
int CSmartDrawAnnoValve::Write(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	if(CaDraw_EntTagShape::RECTANGLE == env.m_valve.m_iEnclosureType)
	{
		ofile << _T("<GROUP>|0,VALV,Pat 08") << std::endl;
		m_oEntTagShape.Write(ofile , CaDraw_EntTagShape::RECTANGLE , env.m_valve.m_oLeaderProp , dDrawingScale);
	}
	else
	{
		ofile << _T("<GROUP>|0,VALV,Pat 03") << std::endl;
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
	ofile << env.m_valve.m_oLeaderProp.level << _T(",") << env.m_valve.m_oLeaderProp.color << _T(",");
	ofile << env.m_valve.m_sArrowType << _T(",") << env.m_valve.m_dArrowSize*dDrawingScale << _T("," ) << env.m_valve.m_dArrowSize/3.0*dDrawingScale << std::endl;

	for(vector<STRING_T>::const_iterator itr = m_oEntTagShape.m_pTagStringList->begin();itr != m_oEntTagShape.m_pTagStringList->end();++itr)
	{
		ofile << _T("<TEXT>|");
		ofile << (center*dDrawingScale) << _T(",0,");
		ofile << env.m_valve.m_oTextStyle.height()*dDrawingScale << _T(",") << (env.m_valve.m_oTextStyle.height()*env.m_valve.m_oTextStyle.GetWidthFactor())*dDrawingScale << _T(",");
		ofile << RAD2DEG(m_oEntTagShape.m_SearchRes.radian) << _T(",");
		ofile << CSmartDrawAnnoModule::CSVString(*itr) << _T(",Center Center,0|");
		ofile << env.m_valve.m_oLabelProp.level << _T(",") << env.m_valve.m_oLabelProp.color << _T(",");
		ofile << env.m_valve.m_oTextStyle << std::endl;
	}

	ofile << _T("</GROUP>") << std::endl;

	return ERROR_SUCCESS;
}

/**	
	@brief	create tag item of tag entity.
	@author	humkyung
	@date	2012.03.24
	@param	
	@return	
*/
CaDraw_TagItem* CSmartDrawAnnoValve::CreateTagObjects(list<CaDraw_TagItem*>& TagItemList , CSmartDrawAnnoView *pAnnoView)
{
	TagItemList.clear();
	return NULL;
}

/**	
	@brief	
	@author	humkyung
	@date	2012.03.24
	@param	
	@return	CaDraw_TagItem*	
*/
CaDraw_TagItem* CSmartDrawAnnoValve::OnCreateTagObject(CSmartDrawAnnoView *pAnnoView , const CIsPoint3d& ptOrigin,const CIsVect3d& vec,const DITEM_E& enmType)
{
	CaDraw_TagItem* pTagItem=NULL;
	
	return pTagItem;
}