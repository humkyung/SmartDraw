#include "StdAfx.h"
#include <assert.h>
#include <tchar.h>
#include <IsString.h>
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoEqui.h"
#include "SmartDrawAnnoNozzle.h"
#include "SmartDrawAnnoView.h"

#include <sstream>
using namespace std;

SMARTDRAW_TAG_IMPLEMENT_FUNC(CaDraw_TagEqui , CaDraw_TagEntity , _T("Equipment"))

CaDraw_TagEqui::HATCHING_ENV::HATCHING_ENV() : m_iHatchingOffset(5) , m_iHatchingScale(1)
{
}

CaDraw_TagEqui::HATCHING_ENV::HATCHING_ENV(const HATCHING_ENV& rhs) : m_iHatchingOffset(rhs.m_iHatchingOffset) , m_iHatchingScale(rhs.m_iHatchingScale)
{
}

CaDraw_TagEqui::HATCHING_ENV::~HATCHING_ENV()
{
}

CaDraw_TagEqui::HATCHING_ENV& CaDraw_TagEqui::HATCHING_ENV::GetInstance()
{
	static CaDraw_TagEqui::HATCHING_ENV __INSTANCE__;

	return (__INSTANCE__);
}

/**
	@brief	parse hatching setting
	@author	humkyung
	@date	2014.04.19
*/
int CaDraw_TagEqui::HATCHING_ENV::Parse(const STRING_T& sIniFilePath)
{
	const STRING_T sApp(_T("Hatching"));
	TCHAR szBuf[MAX_PATH + 1]={0,};

	GetPrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_CellLibrary") , _T("NULL") , szBuf , MAX_PATH  , sIniFilePath.c_str());
	m_sCellLibFilePath.assign(szBuf);
	GetPrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_CellName") , _T("NULL") , szBuf , MAX_PATH  , sIniFilePath.c_str());
	m_sCellName.assign(szBuf);

	m_iHatchingOffset = GetPrivateProfileInt(sApp.c_str() , _T("Eqp_Hatching_Offset") , 5 , sIniFilePath.c_str());
	m_iHatchingScale  = GetPrivateProfileInt(sApp.c_str() , _T("Eqp_Hatching_Scale") , 1 , sIniFilePath.c_str());

	GetPrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Level") , _T("1") , szBuf , MAX_PATH  , sIniFilePath.c_str());
	m_oHatchingProp.level.assign(szBuf);
	GetPrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Color") , _T("0") , szBuf , MAX_PATH  , sIniFilePath.c_str());
	m_oHatchingProp.color.assign(szBuf);
	GetPrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Style") , _T("0") , szBuf , MAX_PATH  , sIniFilePath.c_str());
	m_oHatchingProp.style.assign(szBuf);
	GetPrivateProfileString(sApp.c_str() , _T("Eqp_Hatching_Weight") , _T("0") , szBuf , MAX_PATH  , sIniFilePath.c_str());
	m_oHatchingProp.weight.assign(szBuf); 

	return ERROR_SUCCESS;
}

CaDraw_TagEqui::CaDraw_TagEqui(CaDraw_Entity* unnamed) : CaDraw_TagEntity(unnamed)
{
	m_pTagNozzleList = new list<CaDraw_TagNozzle*>;

	/// 2012.03.27 added by humkyung
	ZeroMemory(m_szGridNo[0] , sizeof(TCHAR)*16);
	ZeroMemory(m_szGridNo[1] , sizeof(TCHAR)*16);
	/// up to here
}

CaDraw_TagEqui::~CaDraw_TagEqui(void)
{
	try
	{
		SAFE_DELETE( m_pTagNozzleList );
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-26
    @function   AddTagNozzle
    @return     int
    @param      CaDraw_TagNozzle*  pNozzle
    @brief
******************************************************************************/
int CaDraw_TagEqui::AddTagNozzle( CaDraw_TagNozzle* pTagNozzle )
{
	assert(pTagNozzle && "pTagNozzle is NULL");

	if(pTagNozzle)
	{
		m_pTagNozzleList->push_back( pTagNozzle );
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-26
    @function   GetTagNozzleList
    @return     int
    @param      list<CaDraw_TagNozzle*>&   oNozzleList
    @brief
******************************************************************************/
int CaDraw_TagEqui::GetTagNozzleList( list<CaDraw_TagNozzle*>& oTagNozzleList )
{
	oTagNozzleList.clear();
	oTagNozzleList.insert(oTagNozzleList.begin() , m_pTagNozzleList->begin() , m_pTagNozzleList->end());
	return ERROR_SUCCESS;
}

/******************************************************************************
	@brief		return true if has nozzle,otherwise return false
    @author     humkyung
    @date       2016-03-14
    @return     bool
******************************************************************************/
bool CaDraw_TagEqui::HasNozzle() const
{
	return (!m_pTagNozzleList->empty());
}

/**	
	@brief	annotate equipment inside of view
	@author	HumKyung.BAEK
	@date	2010.06.12
	@param	
	@return	
*/
int CaDraw_TagEqui::Annotate()
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	CaDraw_View* pDrawView = equi()->GetView();
	CIsPoint3d ptOrigin;
	ptOrigin = pDrawView->ModelCoord2ViewCoord(equi()->origin());
	ptOrigin.z() = 0.f;
	const double dOffset = env.m_InsideEqp.m_dOffset;

	CIsVolume volume = pDrawView->ModelCoord2ViewCoord(equi()->GetVolume());

	list<CCellMap::SearchRes> SearchResList;

	/// get equipment coordinate - 2014.09.12 added by humkyung
	STRING_T sCoord;
	if(true == env.m_InsideEqp.m_bWriteCoordinate) this->GetCoordinate(&sCoord);
	/// up to here

	const double nTextLength = max(env.m_InsideEqp.m_oNameTextStyle.GetTextLength(equi()->name()) , env.m_InsideEqp.m_oCoordTextStyle.GetTextLength(sCoord));
	const double nItemCellHeight = ((env.m_InsideEqp.m_oNameTextStyle.height() + dOffset) + ((true == env.m_InsideEqp.m_bWriteCoordinate) ? (env.m_equi.m_oCoordTextStyle.height() + dOffset) : 0)) * CSmartDrawAnnoModule::m_pCellMap->scale();
	const double nItemCellLength = nTextLength * CSmartDrawAnnoModule::m_pCellMap->scale();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// HORIZONTAL SEARCH
	double nSearchCellLength = (volume.maxx() - volume.minx())*CSmartDrawAnnoModule::m_pCellMap->scale() + nItemCellLength * 3;	/// this value comes from experience.
	
	CCellMap::SIZE_T sizSearch , sizItem;
	sizSearch.width = ceil(nSearchCellLength);
	sizSearch.height= ceil(nItemCellHeight);
	sizItem.width = ceil(nItemCellLength);
	sizItem.height= ceil(nItemCellHeight);

	CCellMap::CellConversionAxis cAxis;
	cAxis.xAxis.Set(1 , 0);
	cAxis.yAxis.Set(0 , 1);

	CCellMap::CellIndex tagOrigin(CSmartDrawAnnoModule::m_pCellMap->PointToIndex(CIsPoint2d(ptOrigin.x() , ptOrigin.y())));
	CCellMap::CellIndex cOrigin(tagOrigin);
	cOrigin.x -= ceil(nSearchCellLength*0.5);
	cOrigin.y -= (true == env.m_InsideEqp.m_bWriteCoordinate) ? ceil((env.m_equi.m_oCoordTextStyle.height() + dOffset)*CSmartDrawAnnoModule::m_pCellMap->scale()) : 0.0;

	CCellMap::SearchRes res;
	STRNCPY_T(res.szString , equi()->name() , 255);
	bool bFound = CSmartDrawAnnoModule::m_pCellMap->FindBestLoc(res,cOrigin,tagOrigin,sizSearch,sizItem,cAxis,0xFF);
	if(bFound)
	{
		res.coords[0] = res.ptLoc;
		res.coords[1] = res.coords[0] + res.cAxis.xAxis*(nTextLength);
		res.coords[2] = CSmartDrawAnnoModule::m_pCellMap->IndexToPoint(cOrigin);
		res.coords[3] = res.coords[2] + cAxis.xAxis*nSearchCellLength;
		
		res.ptLoc.Set(res.ptLoc.x() , ptOrigin.y() + dOffset);
		SearchResList.push_back(res);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// VERTICAL SEARCH
	nSearchCellLength = (volume.maxy() - volume.miny())*CSmartDrawAnnoModule::m_pCellMap->scale() + nItemCellLength * 3;	/// this value comes from experience.

	sizSearch.width = ceil(nSearchCellLength);
	sizSearch.height= ceil(nItemCellHeight);
	sizItem.width = ceil(nItemCellLength);
	sizItem.height= ceil(nItemCellHeight);

	cAxis.xAxis.Set(0 , 1);
	cAxis.yAxis.Set(-1 , 0);

	cOrigin = tagOrigin;
	cOrigin.x += (true == env.m_InsideEqp.m_bWriteCoordinate) ? ceil((env.m_InsideEqp.m_oCoordTextStyle.height() + dOffset)*CSmartDrawAnnoModule::m_pCellMap->scale()) : 0.0;
	cOrigin.y -= ceil(nSearchCellLength * 0.5);
	bFound = CSmartDrawAnnoModule::m_pCellMap->FindBestLoc(res,cOrigin,tagOrigin,sizSearch,sizItem,cAxis,0xFF);
	if(bFound)
	{
		res.ptLoc.Set(ptOrigin.x() - dOffset , res.ptLoc.y());	/// set loc.x to x pos of equipment
		res.coords[0] = res.ptLoc - res.cAxis.yAxis*dOffset;
		res.coords[1] = res.coords[0] + res.cAxis.xAxis*(nTextLength);
		res.coords[2] = CSmartDrawAnnoModule::m_pCellMap->IndexToPoint(cOrigin);
		res.coords[3] = res.coords[2] + cAxis.xAxis*nSearchCellLength;
		SearchResList.push_back(res);
	}

	if(!SearchResList.empty())
	{
		stable_sort(SearchResList.begin(), SearchResList.end(), fn_Len_Sort);  /// LEN 으로 SORT
		stable_sort(SearchResList.begin(), SearchResList.end(), fn_Cnt_Sort);  /// TAG CNT 으로 SORT

		CCellMap::SearchRes res = SearchResList.front();
		const double nTextLength = env.m_equi.m_oNameTextStyle.GetTextLength(res.szString);

		CSmartDrawAnnoModule::m_pCellMap->UpdateRectOf(res.cIndex , ceil(nItemCellLength) , ceil(nItemCellHeight) , res.cAxis , 3);

		CCellMap::CellIndex tagOrigin = CSmartDrawAnnoModule::m_pCellMap->PointToIndex(res.coords[0] + ((res.cAxis.yAxis)*env.m_equi.m_oNameTextStyle.height()*0.5));
		CCellMap::CellIndex cOrigin = CSmartDrawAnnoModule::m_pCellMap->PointToIndex(res.coords[0]);

		m_bTagged = true;
		m_oEntTagShape.m_SearchRes = res;
		m_oEntTagShape.m_pTagStringList->push_back(res.szString);
	}

	return ERROR_SUCCESS;
}

/**
	@brief	fill section mark when equipment is vertical to view and overlap with view volume
	@author	humkyung
	@date	2014.04.19
*/
int CaDraw_TagEqui::FillSectionMark()
{
	const double TOLER = 0.001;
	for(list<CaDraw_Equipment::CylinderShape>::const_iterator itr = equi()->GetShapeList()->begin();itr != equi()->GetShapeList()->end();++itr)
	{
		CIsPoint3d pt[2];
		pt[0] = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(itr->top);
		pt[1] = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(itr->bottom);

		const bool isVertical = ((fabs(pt[0].x() - pt[1].x()) < TOLER) && (fabs(pt[0].y() - pt[1].y()) < TOLER) && (fabs(pt[0].z() - pt[1].z()) > TOLER));
		const bool isOverlap = (!m_pDrawEntity->GetView()->GetVolume().Contains(itr->top) || !m_pDrawEntity->GetView()->GetVolume().Contains(itr->bottom));
		if(isVertical && isOverlap)
		{
			const double dRadius = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(itr->radius);
			CIsPoint3d ptMid = (pt[0] + pt[1])*0.5;
			ptMid.z() = 0.0;

			const long lPipeRadius = long(dRadius * CSmartDrawAnnoModule::m_pCellMap->scale());
			CCellMap::CellIndex tagOrigin(CSmartDrawAnnoModule::m_pCellMap->PointToIndex(CIsPoint2d(ptMid.x() , ptMid.y())));
			CSmartDrawAnnoModule::m_pCellMap->FillCircleOf(tagOrigin , lPipeRadius , 4);
			CSmartDrawAnnoModule::m_pCellMap->FillCircleOf(tagOrigin , lPipeRadius*0.9 , CCellMap::EMPTY);	/// clear inside of donut
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	write section mark if this is vertical and overlap with view volume
	@author	humkyung
	@date	2014.04.18
*/
int CaDraw_TagEqui::WriteSectionMark(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	const double TOLER = 0.001;
	
	CaDraw_TagEqui::HATCHING_ENV& hatchingenv = CaDraw_TagEqui::HATCHING_ENV::GetInstance();
	for(list<CaDraw_Equipment::CylinderShape>::const_iterator itr = equi()->GetShapeList()->begin();itr != equi()->GetShapeList()->end();++itr)
	{
		CIsPoint3d pt[2];
		pt[0] = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(itr->top);
		pt[1] = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(itr->bottom);

		const bool isVertical = ((fabs(pt[0].x() - pt[1].x()) < TOLER) && (fabs(pt[0].y() - pt[1].y()) < TOLER) && (fabs(pt[0].z() - pt[1].z()) > TOLER));
		const bool isOverlap = (!m_pDrawEntity->GetView()->GetVolume().Contains(itr->top) || !m_pDrawEntity->GetView()->GetVolume().Contains(itr->bottom));
		if(isVertical && isOverlap)
		{
			const double dRadius = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(itr->radius);

			//ofile << _T("<GROUP>|0,HATCH,Pat 01") << std::endl;
			ofile << _T("<HATCHDONUT>|");
			ofile << pt[0].x()*dDrawingScale << _T(",") << pt[0].y()*dDrawingScale << _T(",") << (dRadius*dDrawingScale) << _T(",") << (dRadius - hatchingenv.m_iHatchingOffset)*dDrawingScale << _T("|");
			ofile << hatchingenv.m_oHatchingProp.level << _T(",") << hatchingenv.m_oHatchingProp.color << _T(",");
			if(!hatchingenv.m_sCellLibFilePath.empty() && !hatchingenv.m_sCellName.empty())
			{
				ofile << hatchingenv.m_sCellLibFilePath << _T(",") << hatchingenv.m_sCellName << _T(",");
				ofile << _T("45,") << hatchingenv.m_iHatchingScale*dDrawingScale << _T(",No") << std::endl;
			}
			else
			{
				ofile << _T("NULL,NULL,");
				ofile << _T("45,") << hatchingenv.m_iHatchingScale*dDrawingScale << _T(",No") << std::endl;
			}
			//ofile << _T("</GROUP>") << std::endl;
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	write equipment tag
	@author	HumKyung.BAEK
	@date	2010.06.12
	@param	
	@return	
*/
int CaDraw_TagEqui::Write(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	ofile << _T("# Equipment : ") << __LINE__ << _T(" at ") << __FILE__ << std::endl;
	ofile << _T("<GROUP>|0,EQUI,Pat 01") << std::endl;

	ofile << _T("<TEXT>|");
	/// (x,y,z)
	double d = m_oEntTagShape.m_SearchRes.ptLoc.x(); ofile << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
	d = m_oEntTagShape.m_SearchRes.ptLoc.y(); ofile << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",0,");
	/// height,width
	ofile << IsString::TrimedTrailingZero(env.m_equi.m_oNameTextStyle.height()*dDrawingScale) << _T(",");
	ofile << IsString::TrimedTrailingZero(env.m_equi.m_oNameTextStyle.height()*env.m_equi.m_oNameTextStyle.GetWidthFactor()*dDrawingScale) << _T(",");
	/// angle in degree
	const double deg = RAD2DEG(m_oEntTagShape.m_SearchRes.radian);
	ofile << IsString::TrimedTrailingZero(deg) << _T(",");
	ofile << (!m_oEntTagShape.m_pTagStringList->empty() ? (*(m_oEntTagShape.m_pTagStringList))[0] : _T("")) << _T(",Left,0|");
	ofile << env.m_InsideEqp.m_oNameProp.level << _T(",") << env.m_InsideEqp.m_oNameProp.color << _T(",");
	ofile << env.m_InsideEqp.m_oNameTextStyle << std::endl;

	CIsPoint2d pt;

	ofile << _T("<PLINE>|");
	pt = m_oEntTagShape.m_SearchRes.coords[0];
	ofile << IsString::TrimedTrailingZero(pt.x()*dDrawingScale) << _T(",");
	ofile << IsString::TrimedTrailingZero(pt.y()*dDrawingScale) << _T(",");
	pt = m_oEntTagShape.m_SearchRes.coords[1];
	ofile << IsString::TrimedTrailingZero(pt.x()*dDrawingScale) << _T(",");
	ofile << IsString::TrimedTrailingZero(pt.y()*dDrawingScale) << _T("|");
	/// color,layer,ltype,lscale,lweight
	ofile << env.m_InsideEqp.m_oLeaderProp << std::endl;

	if(true == env.m_InsideEqp.m_bWriteCoordinate)
	{
		const double dOffset = env.m_InsideEqp.m_dOffset;

		ofile << _T("<TEXT>|");
		/// (x,y,z)
		d = m_oEntTagShape.m_SearchRes.ptLoc.x() - (m_oEntTagShape.m_SearchRes.cAxis.yAxis.dx()*(env.m_InsideEqp.m_oCoordTextStyle.height()+dOffset*2)); ofile << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",");
		d = m_oEntTagShape.m_SearchRes.ptLoc.y() - (m_oEntTagShape.m_SearchRes.cAxis.yAxis.dy()*(env.m_InsideEqp.m_oCoordTextStyle.height()+dOffset*2)); ofile << IsString::TrimedTrailingZero(d*dDrawingScale) << _T(",0,");
		/// height,width
		ofile << IsString::TrimedTrailingZero(env.m_InsideEqp.m_oNameTextStyle.height()*dDrawingScale) << _T(",");
		ofile << IsString::TrimedTrailingZero(env.m_InsideEqp.m_oNameTextStyle.height()*env.m_InsideEqp.m_oNameTextStyle.GetWidthFactor()*dDrawingScale) << _T(",");
		/// angle in degree
		ofile << IsString::TrimedTrailingZero(deg) << _T(",");
		STRING_T sCoord;
		this->GetCoordinate(&sCoord);
		ofile << sCoord << _T(",Left,0|");
		ofile << env.m_InsideEqp.m_oNameProp.level << _T(",") << env.m_InsideEqp.m_oNameProp.color << _T(",");
		ofile << env.m_InsideEqp.m_oNameTextStyle << std::endl;
	}

	ofile << _T("</GROUP>") << std::endl;

	/// center line on
	if(env.m_equi.m_bCenterLine)
	{
		CaDraw_View* pDrawView = equi()->GetView();
		CIsPoint3d ptOrigin(pDrawView->ModelCoord2ViewCoord(equi()->origin()));
		CIsVolume volume = pDrawView->ModelCoord2ViewCoord(equi()->GetVolume());

		CIsPoint3d ptBox[4];
		ptBox[0].x() = volume.minx();
		ptBox[0].y() = ptOrigin.y();
		ptBox[1].x() = volume.maxx();
		ptBox[1].y() = ptOrigin.y();
		ptBox[2].x() = ptOrigin.x();
		ptBox[2].y() = volume.miny();
		ptBox[3].x() = ptOrigin.x();
		ptBox[3].y() = volume.maxy();

		ofile << _T("<LINE>|");
		ofile << ptBox[0]*dDrawingScale << _T(",");
		ofile << ptBox[1]*dDrawingScale << _T("|");
		/// color,layer,ltype,lscale,lweight
		ofile << env.m_equi.m_oCenterlineProp << std::endl;

		ofile << _T("<LINE>|");
		ofile << ptBox[2]*dDrawingScale << _T(",");
		ofile << ptBox[3]*dDrawingScale << _T("|");
		ofile << env.m_equi.m_oCenterlineProp << std::endl;
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	create tag item of tag entity.

	@author	HumKyung.BAEK

	@date	2010.06.03

	@param	

	@return	
*/
CaDraw_TagItem* CaDraw_TagEqui::CreateTagObjects(list<CaDraw_TagItem*>& TagItemList , CSmartDrawAnnoView *pAnnoView)
{
	TagItemList.clear();
	return NULL;
}

/**	
	@brief	

	@author	humkyung

	@date	2010.05.28

	@param	

	@return	CaDraw_TagItem*	
*/
CaDraw_TagItem* CaDraw_TagEqui::OnCreateTagObject(CSmartDrawAnnoView *pAnnoView , const CIsPoint3d& ptOrigin,const CIsVect3d& vec,const DITEM_E& enmType)
{
	CaDraw_TagItem* pTagItem=NULL;
	if(pTagItem = new CaDraw_EquiTagItem)
	{
		CIsPoint3d ptViewOrigin = pAnnoView->drawView()->center();
		const double nViewWidth = pAnnoView->drawView()->GetViewWidth();
		const double nViewHeight= pAnnoView->drawView()->GetViewHeight();

		pTagItem->m_ptOrigin= ptOrigin;
		pTagItem->vecDir    = vec;
		pTagItem->enmType   = enmType;
		pTagItem->m_pEnt    = this;

		if(pTagItem->m_ptOrigin.x() > ptViewOrigin.x() + nViewWidth*0.5)
			pTagItem->m_ptOrigin.x() = (ptViewOrigin.x() + nViewWidth*0.5);
		if(pTagItem->m_ptOrigin.x() < ptViewOrigin.x() - nViewWidth*0.5)
			pTagItem->m_ptOrigin.x() = (ptViewOrigin.x() - nViewWidth*0.5);
		if(pTagItem->m_ptOrigin.y() > ptViewOrigin.y() + nViewHeight*0.5)
			pTagItem->m_ptOrigin.y() = (ptViewOrigin.y() + nViewHeight*0.5);
		if(pTagItem->m_ptOrigin.y() < ptViewOrigin.y() - nViewHeight*0.5)
			pTagItem->m_ptOrigin.y() = ( ptViewOrigin.y() - nViewHeight*0.5);
	}

	return pTagItem;
}

/**	
	@brief	The CaDraw_TagEqui::GetTagNo function

	@author	humkyung

	@date	2010.06.02

	@param	pTagNo1	a parameter of type char*
	@param	pTagNo2	a parameter of type char*

	@return	void	
*/
void CaDraw_TagEqui::GetTagNo(STRING_T* pTagNo1 , STRING_T* pTagNo2)
{
	assert(pTagNo1 && "pTagNo1 is NULL");
	assert(pTagNo2 && "pTagNo2 is NULL");
	
	if(pTagNo1 && pTagNo2)
	{
		pTagNo1->assign( equi()->name());
		
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		if(env.m_equi.m_bWriteCoordinate)
		{
			GetCoordinate(pTagNo2);
		}
		else if(env.m_equi.m_bWriteDesc)
		{
			GetDesc(pTagNo2);
		}else	pTagNo2->assign( _T("") );
	}
}

/**	
	@brief	The CDraftEqui::GetCoordinate function
	@author	humkyung
	@param	pTagNo2	a parameter of type char *
	@return	void	
*/
void CaDraw_TagEqui::GetCoordinate(STRING_T *pTagNo2)
{
	assert(pTagNo2 && "pTagNo2 is NULL");

	if(pTagNo2)
	{	
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		TCHAR szXDir[3]= _T("E.") , szYDir[3]= _T("N.");	/// default value is "E."/"N."
		double x=0.,y=0.;
		CIsPoint3d OriginInModel = equi()->origin();
		if(CaDraw_TagEnv::XY_COORD == env.m_eAnnotateCoord)
		{
			strcpy(szXDir,"X");
			strcpy(szYDir,"Y");
		}
		else
		{
			if(OriginInModel.x() < 0.) strcpy(szXDir,"W.");
			if(OriginInModel.y() < 0.) strcpy(szYDir,"S.");
		}
		
		OSTRINGSTREAM_T oss;
		TCHAR szUnit[64]={0,};
		if(CaDraw_TagEnv::XY_COORD == env.m_eAnnotateCoord)
		{
			x = ((OriginInModel.x() / CSmartDrawAnnoModule::UNIT));
			y = ((OriginInModel.y() / CSmartDrawAnnoModule::UNIT));

			if(CaDraw_TagEnv::INCH_FEET == CaDraw_TagEnv::OUTPUT_UNIT)
			{	
				oss << szXDir << _T(" ") << CSmartDrawAnnoModule::UNIT2STR(x) << _T(" ");
				oss << szYDir << _T(" ") << CSmartDrawAnnoModule::UNIT2STR(y);
			}
			else
			{
				oss << szXDir << _T(" ") << CSmartDrawAnnoModule::UNIT2STR(x,M) << _T(" ");
				oss << szYDir << _T(" ") << CSmartDrawAnnoModule::UNIT2STR(y,M);
			}
		}
		else
		{
			x = fabs(((OriginInModel.x() / CSmartDrawAnnoModule::UNIT)));
			y = fabs(((OriginInModel.y() / CSmartDrawAnnoModule::UNIT)));

			if(CaDraw_TagEnv::INCH_FEET == CaDraw_TagEnv::OUTPUT_UNIT)
			{	
				oss << szXDir << _T(" ") << CSmartDrawAnnoModule::UNIT2STR(x) << _T(" ");
				oss << szYDir << _T(" ") << CSmartDrawAnnoModule::UNIT2STR(y);
			}
			else
			{
				oss << szXDir << _T(" ") << CSmartDrawAnnoModule::UNIT2STR(x,M) << _T(" ");
				oss << szYDir << _T(" ") << CSmartDrawAnnoModule::UNIT2STR(y,M);
			}
		}

		pTagNo2->assign( oss.str().c_str() );
	}
}

/**	
	@brief	The CaDraw_TagEqui::GetDesc function
	@author	HumKyung.BAEK
	@param	pTagNo2	a parameter of type char *
	@return	void	
*/
void CaDraw_TagEqui::GetDesc(STRING_T *pTagNo2)
{
	assert(pTagNo2 && "pTagNo2 is NULL");

	if(pTagNo2)
	{
		if(0 == STRCMP_T(equi()->desc() , _T("NONE")))
			pTagNo2->assign(_T("Nil"));
		else	pTagNo2->assign(equi()->desc());
	}
}