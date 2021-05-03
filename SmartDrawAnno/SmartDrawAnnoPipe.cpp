#include "StdAfx.h"
#include <assert.h>
#include <Tokenizer.h>
#include <IsString.h>
#include <IsVect2d.h>
#include <aDraw_View.h>
#include "SmartDrawAnnoPipe.h"
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoItem.h"
#include "SmartDrawAnnoView.h"

#include <vector>

SMARTDRAW_TAG_IMPLEMENT_FUNC(CaDraw_TagPipe , CaDraw_TagEntity , _T("Pipe"))

CaDraw_TagPipe::HATCHING_ENV::HATCHING_ENV() : m_iHatchingTypeA(1) , m_iHatchingTypeB(1)
{
}

CaDraw_TagPipe::HATCHING_ENV::HATCHING_ENV(const HATCHING_ENV& rhs) : m_iHatchingTypeA(rhs.m_iHatchingTypeA) , m_iHatchingTypeB(rhs.m_iHatchingTypeB)
{
}

CaDraw_TagPipe::HATCHING_ENV::~HATCHING_ENV()
{
}

CaDraw_TagPipe::HATCHING_ENV& CaDraw_TagPipe::HATCHING_ENV::GetInstance()
{
	static CaDraw_TagPipe::HATCHING_ENV __INSTANCE__;

	return (__INSTANCE__);
}

/**
	@brief	parse hatching setting
	@author	humkyung
	@date	2014.04.13
*/
int CaDraw_TagPipe::HATCHING_ENV::Parse(const STRING_T& sIniFilePath)
{
	const STRING_T sApp(_T("Hatching"));
	TCHAR szBuf[MAX_PATH + 1]={0,};

	GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Type") , _T("Depend on Size") , szBuf , MAX_PATH  , sIniFilePath.c_str());
	m_eHatchingType = (STRING_T(_T("Depend on Size")) == szBuf) ? DEPEND_ON_SIZE : ALL;
	
	GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Cirital_Size") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str());
	m_oSize.Parse(szBuf);

	m_iHatchingTypeA = GetPrivateProfileInt(sApp.c_str() , _T("Vertical_Pipe_Hatching_MarkA") , 1 , sIniFilePath.c_str());
	m_iHatchingTypeB = GetPrivateProfileInt(sApp.c_str() , _T("Vertical_Pipe_Hatching_MarkB") , 1 , sIniFilePath.c_str());

	GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Level") , _T("1") , szBuf , MAX_PATH  , sIniFilePath.c_str());
	m_oHatchingProp.level.assign(szBuf);
	GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Color") , _T("0") , szBuf , MAX_PATH  , sIniFilePath.c_str());
	m_oHatchingProp.color.assign(szBuf);
	GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Style") , _T("0") , szBuf , MAX_PATH  , sIniFilePath.c_str());
	m_oHatchingProp.style.assign(szBuf);
	GetPrivateProfileString(sApp.c_str() , _T("Vertical_Pipe_Hatching_Weight") , _T("0") , szBuf , MAX_PATH  , sIniFilePath.c_str());
	m_oHatchingProp.weight.assign(szBuf); 

	return ERROR_SUCCESS;
}

CaDraw_TagPipe::CaDraw_TagPipe(CaDraw_Entity* unnamed) : CaDraw_TagEntity(unnamed)
{
}

CaDraw_TagPipe::~CaDraw_TagPipe(void)
{
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-19
    @class      CaDraw_TagPipe
    @function   GetElevString
    @return     STRING_T
    @param      const   double&
    @param      pt
    @brief
******************************************************************************/
STRING_T CaDraw_TagPipe::GetElevString( const CIsPoint3d& pt )
{
	STRING_T res;

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	const long x = long(SAFE_ROUND(pt.x() , 0));
	const long y = long(SAFE_ROUND(pt.y() , 0));

	STRING_T strX, strY;
	//if (IsMetric) 
	{
		OSTRINGSTREAM_T oss;
		(x >= 0) ? (oss << _T("E. ") << x) : (oss << _T("W. ") << abs(x));
		strX = oss.str();

		oss.str(_T(""));
		(y >= 0) ? (oss << _T("N. ") << y) : (oss << _T("S. ") << abs(y));
		strY = oss.str();
	}
	//else
	//{
	//	double xd = Pt.m_dx / ModelUint;
	//	if (xd >= 0) 
	//		strX.Format("E. %s", sg_MetricToInch(xd));
	//	else
	//		strX.Format("W. %s", sg_MetricToInch(xd*-1));
	//	double yd = Pt.m_dy / ModelUint;
	//	if (yd >= 0) 
	//		strY.Format("N. %s", sg_MetricToInch(yd));
	//	else
	//		strY.Format("S. %s", sg_MetricToInch(yd*-1));
	//}
	//// ReType 1 : X value , 2 : Y value , 3 : X Y value
	//if (ReType == 1) 
	//	res = strX;
	//else if (ReType == 2) 
	//	res = strY;
	//else
	//	res.Format("%s %s", strX, strY);
	
	OSTRINGSTREAM_T oss;
	oss << _T("CL. ") << IsString::TrimedTrailingZero(SAFE_ROUND(pt.z() , 1));
	res = oss.str();

	return res;
}

/**	
	@brief	get pipe tag no
	@param	pTagNo	buffer which tag no is stored to
	@param	pt	a parameter of type const POINT_T
	@param	pView	a parameter of type CDraftView*
	@remarks
	옵션이 BOP일때 SLOP진 PIPE는 CL를 표기하기 때문에 TagNo에서 'BOP'는 빼도록 한다.
	@return	void	
*/
void CaDraw_TagPipe::GetTagNo(STRING_T* pTagNo,const CIsPoint3d& pt,CaDraw_View* pView)
{
	static const double TOLER = 0.001;

	assert(pTagNo && "pTagNo is NULL");
	assert(pView  && "pView is NULL");
	
	UINT_TYPE_E sub_unit=M;

	if(pTagNo && pView)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		CaDraw_Pipe* pDrawPipe = static_cast<CaDraw_Pipe*>(m_pDrawEntity);

		const double nCL  = (pt.z() / CSmartDrawAnnoModule::UNIT) * env.m_nUnitConvFactor;
		const double nBOP = (nCL - pDrawPipe->GetRadiusInMetric() * env.m_nUnitConvFactor);
		const double nTOP = (nCL + pDrawPipe->GetRadiusInMetric() * env.m_nUnitConvFactor);
		
		CIsPoint3d pt[2];
		pt[0] = pView->ModelCoord2ViewCoord(pDrawPipe->GetStartPoint());
		pt[1] = pView->ModelCoord2ViewCoord(pDrawPipe->GetEndPoint());
		CIsVect3d vec(pt[0] - pt[1]);

		int aaa = 0;
		TCHAR aTmp[16] = {'\0',};
		
		if(false == vec.IsValid())
		{
			pTagNo->assign(_T("ERROR"));
			return;
		}
		vec.Normalize();
		
		if(DOUBLELINE == env.m_pipe.m_ext.eTag)
		{
			if((fabs(vec.dx()) < TOLER) && (fabs(vec.dy()) < TOLER) && (fabs(vec.dz()) > TOLER))
			{
				pTagNo->assign(pDrawPipe->line_label().c_str()); /// vertical pipe
				if(true == env.m_pipe.GetTrimEndDash())
				{
					if('-' == (*pTagNo)[pTagNo->length() - 1]) (*pTagNo) = pTagNo->substr(0,pTagNo->length()-1);
				}
			}
			else if(fabs(vec.dz()) < TOLER && ((fabs(vec.dx()) > TOLER) || (fabs(vec.dy()) > TOLER)))
			{
				PIPE_ELEV_T eElevType = env.m_pipe.m_ext.eElev;
				if(ELEV_BYSPEC == env.m_pipe.m_ext.eElev)
				{
					const STRING_T sSpec = this->pipe()->GetAttribute(_T("PIPING_MATER_CLASS"));
					eElevType = env.m_pipe.GetElevationTypeBySpec(sSpec);
				}

				if(ELEV_MIXIN == eElevType)
				{
					pTagNo->assign(pDrawPipe->line_label().c_str() );
					if(true == env.m_pipe.GetTrimEndDash())
					{
						if('-' == (*pTagNo)[pTagNo->length() - 1]) (*pTagNo) = pTagNo->substr(0,pTagNo->length()-1);
					}
					(*pTagNo) += _T("\\n");
					(*pTagNo) += env.m_pipe.m_ext.szPrefix[ELEV_CL-1];
					(*pTagNo) += _T(" ");

					(*pTagNo) += CSmartDrawAnnoModule::UNIT2STR(nCL , sub_unit);
					(*pTagNo) += _T(" ");
					
					(*pTagNo) += env.m_pipe.m_ext.szPrefix[ELEV_BOP-1];
					(*pTagNo) += _T(" ");
					(*pTagNo) += CSmartDrawAnnoModule::UNIT2STR(nBOP,sub_unit);
				}
				else if(ELEV_CL == eElevType)  
				{
					pTagNo->assign(pDrawPipe->line_label().c_str());
					if(true == env.m_pipe.GetTrimEndDash())
					{
						if('-' == (*pTagNo)[pTagNo->length() - 1]) (*pTagNo) = pTagNo->substr(0,pTagNo->length()-1);
					}
					(*pTagNo) += _T("\\n");
					(*pTagNo) += env.m_pipe.m_ext.szPrefix[eElevType-1];
					(*pTagNo) += _T(" ");
					(*pTagNo) += CSmartDrawAnnoModule::UNIT2STR(nCL,sub_unit);
				}
				else if(ELEV_BOP == eElevType)  
				{
					pTagNo->assign(pDrawPipe->line_label().c_str());
					if(true == env.m_pipe.GetTrimEndDash())
					{
						if('-' == (*pTagNo)[pTagNo->length() - 1]) (*pTagNo) = pTagNo->substr(0,pTagNo->length()-1);
					}
					(*pTagNo) += _T("\\n");
					(*pTagNo) += env.m_pipe.m_ext.szPrefix[eElevType-1];
					(*pTagNo) += _T(" ");
					(*pTagNo) += CSmartDrawAnnoModule::UNIT2STR(nBOP,sub_unit);
				}
				else if(ELEV_TOP == eElevType) 
				{
					pTagNo->assign(pDrawPipe->line_label().c_str());
					if(true == env.m_pipe.GetTrimEndDash())
					{
						if('-' == (*pTagNo)[pTagNo->length() - 1]) (*pTagNo) = pTagNo->substr(0,pTagNo->length()-1);
					}
					(*pTagNo) += _T("\\n");
					(*pTagNo) += env.m_pipe.m_ext.szPrefix[eElevType-1];
					(*pTagNo) += _T(" ");
					(*pTagNo) += CSmartDrawAnnoModule::UNIT2STR(nTOP,sub_unit);
				}
			}
			else
			{	/// only 'WPCL.'
				if((ELEV_MIXIN == env.m_pipe.m_ext.eElev) || (ELEV_CL == env.m_pipe.m_ext.eElev) || (ELEV_BOP == env.m_pipe.m_ext.eElev) || 
				   (ELEV_TOP == env.m_pipe.m_ext.eElev) || (ELEV_BYSPEC == env.m_pipe.m_ext.eElev))
				{
					pTagNo->assign(pDrawPipe->line_label().c_str());
					if(true == env.m_pipe.GetTrimEndDash())
					{
						if('-' == (*pTagNo)[pTagNo->length() - 1]) (*pTagNo) = pTagNo->substr(0,pTagNo->length()-1);
					}
					(*pTagNo) += _T("\\n");
					(*pTagNo) += env.m_pipe.m_ext.szWP;
					(*pTagNo) += _T(" ");
					(*pTagNo) += CSmartDrawAnnoModule::UNIT2STR(nCL,sub_unit);
				}
			}
		}
		else if(SINGLE_LINENO_ELEV == env.m_pipe.m_ext.eElev)
		{
			if((fabs(vec.dx()) < 0.001) && (fabs(vec.dy()) < 0.001))
			{
				pTagNo->assign( pDrawPipe->line_label().c_str() );	/// vertical pipe
				if(true == env.m_pipe.GetTrimEndDash())
				{
					if('-' == (*pTagNo)[pTagNo->length() - 1]) (*pTagNo) = pTagNo->substr(0,pTagNo->length()-1);
				}
			}
			else
			{
				PIPE_ELEV_T eElevType = env.m_pipe.m_ext.eElev;
				if(ELEV_BYSPEC == env.m_pipe.m_ext.eElev)
				{
					const STRING_T sSpec = this->pipe()->GetAttribute(_T("PIPING_MATER_CLASS"));
					eElevType = env.m_pipe.GetElevationTypeBySpec(sSpec);
				}

				if(ELEV_MIXIN == eElevType)
				{
					pTagNo->assign(pDrawPipe->line_label().c_str());
					if(true == env.m_pipe.GetTrimEndDash())
					{
						if('-' == (*pTagNo)[pTagNo->length() - 1]) (*pTagNo) = pTagNo->substr(0,pTagNo->length()-1);
					}
					(*pTagNo) += _T("(");
					(*pTagNo) += env.m_pipe.m_ext.szPrefix[ELEV_CL-1];
					(*pTagNo) += _T(" ");
					(*pTagNo) += CSmartDrawAnnoModule::UNIT2STR(nCL,sub_unit);
					(*pTagNo) += _T(" ");
					(*pTagNo) += env.m_pipe.m_ext.szPrefix[ELEV_BOP-1];
					(*pTagNo) += _T(" ");		
					(*pTagNo) += CSmartDrawAnnoModule::UNIT2STR(nBOP,sub_unit);
					(*pTagNo) += _T(")");
				}
				else if(ELEV_CL == eElevType)
				{
					pTagNo->assign(pDrawPipe->line_label().c_str());
					if(true == env.m_pipe.GetTrimEndDash())
					{
						if('-' == (*pTagNo)[pTagNo->length() - 1]) (*pTagNo) = pTagNo->substr(0,pTagNo->length()-1);
					}
					(*pTagNo) += _T("(");
					(*pTagNo) += env.m_pipe.m_ext.szPrefix[eElevType-1];
					(*pTagNo) += _T(" ");
					(*pTagNo) += CSmartDrawAnnoModule::UNIT2STR(nCL,sub_unit);
					(*pTagNo) += _T(")");
				}					
				else if(ELEV_BOP == eElevType)
				{
					pTagNo->assign( pDrawPipe->line_label().c_str() );
					if(true == env.m_pipe.GetTrimEndDash())
					{
						if('-' == (*pTagNo)[pTagNo->length() - 1]) (*pTagNo) = pTagNo->substr(0,pTagNo->length()-1);
					}
					(*pTagNo) += _T("(");
					(*pTagNo) += env.m_pipe.m_ext.szPrefix[eElevType-1];
					(*pTagNo) += _T(" ");
					(*pTagNo) += CSmartDrawAnnoModule::UNIT2STR(nBOP,sub_unit);
					(*pTagNo) += _T(")");
				}
				else if(ELEV_TOP == eElevType) 
				{
					pTagNo->assign( pDrawPipe->line_label().c_str() );
					if(true == env.m_pipe.GetTrimEndDash())
					{
						if('-' == (*pTagNo)[pTagNo->length() - 1]) (*pTagNo) = pTagNo->substr(0,pTagNo->length()-1);
					}
					(*pTagNo) += _T("(");
					(*pTagNo) += env.m_pipe.m_ext.szPrefix[eElevType-1];
					(*pTagNo) += _T(" ");
					(*pTagNo) += CSmartDrawAnnoModule::UNIT2STR(nTOP,sub_unit);
					(*pTagNo) += _T(")");
				}	
			}
		}
		else if(SINGLE_LINENO == env.m_pipe.m_ext.eTag)
		{
			pTagNo->assign( pDrawPipe->line_label().c_str() );
			if(true == env.m_pipe.GetTrimEndDash())
			{
				if('-' == (*pTagNo)[pTagNo->length() - 1]) (*pTagNo) = pTagNo->substr(0,pTagNo->length()-1);
			}
		}
	}
}

/**	
	@brief	The CDraftPipe::GetLineLabel function
	@author	humkyung
	@date	????.??.??
	@param	
	@return	string	
*/
STRING_T CaDraw_TagPipe::GetLineLabel() const
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	STRING_T sLabelNo = pipe()->line_label();
	if(true == env.m_pipe.GetTrimEndDash())
	{
		if('-' == sLabelNo[sLabelNo.length() - 1]) sLabelNo = sLabelNo.substr(0,sLabelNo.length()-1);
	}

	return sLabelNo;
}

/**	
	@brief	create tag item of tag entity.
	@author	HumKyung.BAEK
	@date	2010.06.03
	@param	
	@return	
*/
CaDraw_TagItem* CaDraw_TagPipe::CreateTagObjects(list<CaDraw_TagItem*>& TagItemList , CSmartDrawAnnoView *pAnnoView)
{
	TagItemList.clear();
	return NULL;
}

/**	
	@brief	create a tag item for pipe

	@author	humkyung

	@date	2010.05.28

	@param	

	@return	CaDraw_TagItem*	
*/
CaDraw_TagItem* CaDraw_TagPipe::OnCreateTagObject(CSmartDrawAnnoView *pAnnoView , const CIsPoint3d& ptOrigin,const CIsVect3d& vec,const DITEM_E& enmType)
{
	CaDraw_TagItem* pTagItem=NULL;
	if(pTagItem = new CaDraw_PipeTagItem)
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
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.08

	@param	

	@return	
*/
bool CaDraw_TagPipe::IsHorizontal() const
{
	const double TOLER = 0.001;

	CIsPoint3d pt[2];
	pt[0] = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pipe()->GetStartPoint());
	pt[1] = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pipe()->GetEndPoint());
	if(pt[0].x() != pt[1].x())
	{
		if(pt[1].x() < pt[0].x()) swap(pt[0] , pt[1]);
	}
	else
	{
		if(pt[1].y() < pt[0].y()) swap(pt[0] , pt[1]);
	}

	return ((fabs(pt[0].x() - pt[1].x()) > TOLER) && (fabs(pt[0].y() - pt[1].y()) < TOLER));
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.08
	@param	
	@return	
*/
bool CaDraw_TagPipe::IsVertical() const
{
	const double TOLER = 0.001;

	CIsPoint3d pt[2];
	pt[0] = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pipe()->GetStartPoint());
	pt[1] = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pipe()->GetEndPoint());
	/*if(pt[0].x() != pt[1].x())
	{
		if(pt[1].x() < pt[0].x()) swap(pt[0] , pt[1]);
	}
	else
	{
		if(pt[1].y() < pt[0].y()) swap(pt[0] , pt[1]);
	}*/

	return ((fabs(pt[0].x() - pt[1].x()) < TOLER) && (fabs(pt[0].y() - pt[1].y()) < TOLER) && (fabs(pt[0].z() - pt[1].z()) > TOLER));
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-19
    @function   CalculateTagItemRect
    @return     int
    @param      double& dItemWidth
    @param      double& dItemHeight
    @brief
******************************************************************************/
int CaDraw_TagPipe::CalculateTagItemRect(STRING_T& sLabelNo , STRING_T& sElev , double& dItemWidth , double& dItemHeight)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	sLabelNo = this->GetLineLabel();

	if((_T("Only LineNo") == env.m_pipe.m_int.m_sDisplayStyle) || (_T("LineNo+Elev") == env.m_pipe.m_int.m_sDisplayStyle))
	{
		dItemHeight = env.m_pipe.m_int.m_oLineNoTextStyle.height() + env.m_pipe.m_int.m_dOffset;
	}
	else if(_T("LineNo/Elev") == env.m_pipe.m_int.m_sDisplayStyle)
	{
		dItemHeight = env.m_pipe.m_int.m_oLineNoTextStyle.height() + env.m_pipe.m_int.m_oElevTextStyle.height() + env.m_pipe.m_int.m_dOffset*2.0;
	}

	if(_T("Only LineNo") == env.m_pipe.m_int.m_sDisplayStyle)
	{
		dItemWidth = env.m_pipe.m_int.m_oLineNoTextStyle.GetTextLength(sLabelNo);
	}
	else if(_T("LineNo+Elev") == env.m_pipe.m_int.m_sDisplayStyle)
	{
		sElev = this->GetElevString(pipe()->GetStartPoint());
		dItemWidth = env.m_pipe.m_int.m_oLineNoTextStyle.GetTextLength(sLabelNo + _T("(") + sElev + _T(")"));
	}
	else if(_T("LineNo/Elev") == env.m_pipe.m_int.m_sDisplayStyle)
	{
		sElev = this->GetElevString(pipe()->GetStartPoint());
		dItemWidth = max(env.m_pipe.m_int.m_oLineNoTextStyle.GetTextLength(sLabelNo) , env.m_pipe.m_int.m_oElevTextStyle.GetTextLength(sElev));
	}

	return ERROR_SUCCESS;
}


/**	
	@brief	try to place tag along pipe
	@author	humkyung
	@param	TagArea		INSIDE_OF_PIPE , OVER_PIPE
	@date	2010.06.07
	@return	bool
*/
bool CaDraw_TagPipe::TryToPlaceTagAlongPipe(CCellMap::SearchRes& res , STRING_T& sLabelNo , STRING_T& sElev , const TAG_AREA& eTagArea , const int& nMinOccupied)
{
	const double TOLER = 0.001;
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	sLabelNo = this->GetLineLabel();
	CaDraw_View* pDrawView = m_pDrawEntity->GetView();
	CIsPoint3d pt[2] , ptMid;
	pt[0] = pDrawView->ModelCoord2ViewCoord(pipe()->GetStartPoint());
	pt[0].z() = 0.f;
	pt[1] = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pipe()->GetEndPoint());
	pt[1].z() = 0.f;
	this->ClipByViewBoundary(pt);
	if((fabs(pt[0].x() - pt[1].x()) < TOLER) && (fabs(pt[0].y() - pt[1].y()) < TOLER)) return false;
	if(fabs(pt[1].x() - pt[0].x()) > TOLER)
	{
		if(pt[1].x() < pt[0].x()) swap(pt[0] , pt[1]);
	}
	else
	{
		if(pt[1].y() < pt[0].y()) swap(pt[0] , pt[1]);
	}

	ptMid = (pt[0] + pt[1]) * 0.5;	/// center of pipe

	list<CCellMap::SearchRes> oSearchResList;
	double nSearchCellLength = SAFE_ROUND(pt[0].DistanceTo(pt[1]) * CSmartDrawAnnoModule::m_pCellMap->scale() , 1);
	
	double dItemHeight = 0.0 , dItemWidth = 0.0;
	this->CalculateTagItemRect(sLabelNo , sElev , dItemWidth , dItemHeight);
	const double dItemCellHeight = dItemHeight*CSmartDrawAnnoModule::m_pCellMap->scale();
	const double dItemCellLength = dItemWidth*CSmartDrawAnnoModule::m_pCellMap->scale();
	/// 
	nSearchCellLength = (nSearchCellLength < dItemCellLength) ? dItemCellLength : nSearchCellLength;

	CIsPoint2d ptOrigin(ptMid.x() , ptMid.y());

	CCellMap::CellConversionAxis cAxis;
	cAxis.xAxis.Set(pt[1].x() - pt[0].x() , pt[1].y() - pt[0].y());
	CIsVect3d dir = (CIsVect3d(0.f,0.f,1.f)*CIsVect3d(cAxis.xAxis.dx() , cAxis.xAxis.dy() , 0.f));
	cAxis.yAxis.Set(dir.dx() , dir.dy());
	cAxis.xAxis.Normalize();
	cAxis.yAxis.Normalize();
	if(CaDraw_TagPipe::INSIDE_OF_PIPE == eTagArea)
	{
		ptOrigin = ptOrigin - (cAxis.xAxis*(nSearchCellLength*0.5)) - cAxis.yAxis*(dItemHeight * 0.5);
	}
	else if(CaDraw_TagPipe::OVER_PIPE == eTagArea)
	{
		const long nPipeSize = pipe()->GetView()->ModelCoord2ViewCoord(pipe()->GetSize());	///TODO: CHECK THIS CODE!!!
		ptOrigin = ptOrigin - (cAxis.xAxis*(nSearchCellLength*0.5)) + cAxis.yAxis * (nPipeSize * 0.5 + (1.f/CSmartDrawAnnoModule::m_pCellMap->scale()));
	}

	CCellMap::CellIndex tagOrigin(CSmartDrawAnnoModule::m_pCellMap->PointToIndex(CIsPoint2d(ptMid.x() , ptMid.y())));
	CCellMap::CellIndex cOrigin(CSmartDrawAnnoModule::m_pCellMap->PointToIndex(CIsPoint2d(ptOrigin.x() , ptOrigin.y())));
	
	CCellMap::SIZE_T sizSearch , sizItem;
	sizSearch.width = nSearchCellLength;
	sizSearch.height= dItemCellHeight;
	sizItem.width = dItemCellLength;
	sizItem.height= dItemCellHeight;
	const bool bFound = CSmartDrawAnnoModule::m_pCellMap->FindBestLoc(res , cOrigin , tagOrigin , 
		sizSearch , sizItem , cAxis , nMinOccupied);
	if(bFound)
	{
		res.ptTagOrigin = CIsPoint2d(ptMid.x() , ptMid.y());
		res.coords[0] = res.ptLoc;
		res.coords[1] = res.coords[0] + (cAxis.xAxis * dItemWidth);
		res.coords[2] = res.coords[1] + (cAxis.yAxis * dItemHeight);
		res.coords[3] = res.coords[2] - (cAxis.xAxis * dItemWidth);

		if((_T("Only LineNo") == env.m_pipe.m_int.m_sDisplayStyle) || (_T("LineNo+Elev") == env.m_pipe.m_int.m_sDisplayStyle))
		{
			CIsPoint2d pt(res.coords[0] + res.cAxis.yAxis*env.m_pipe.m_int.m_dOffset);
			res.leaders[0] = pt;
			res.leaders[1] = pt + res.cAxis.xAxis*dItemWidth;
		}
		else if(_T("LineNo/Elev") == env.m_pipe.m_int.m_sDisplayStyle)
		{
			CIsPoint2d pt(res.coords[0] + res.cAxis.yAxis*(env.m_pipe.m_int.m_oElevTextStyle.height() + env.m_pipe.m_int.m_dOffset));
			res.leaders[0] = pt;
			res.leaders[1] = pt + res.cAxis.xAxis*dItemWidth;
		}

		if(CaDraw_TagPipe::INSIDE_OF_PIPE == eTagArea)
		{
			STRCPY_T(res.szString , _T("Inside Pipe"));
		}
		else
		{
			STRCPY_T(res.szString , _T("Over Pipe"));
		}

		/*CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[0] , res.coords[1] , 1);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[1] , res.coords[2] , 1);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[2] , res.coords[3] , 1);
		CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(res.coords[3] , res.coords[0] , 1);*/

		///CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(cOrigin   , cOrigin   , 2);
		///CSmartDrawAnnoModule::m_pCellMap->UpdateLineOf(tagOrigin , tagOrigin , 2);

		CSmartDrawAnnoModule::m_pCellMap->UpdateRectOf(res.cIndex , dItemCellLength , dItemCellHeight , res.cAxis , 155);
	}
	
	return bFound;
}

/**	
	@brief	clipping a line consist of two points by view boundary

	@author	HumKyung.BAEK

	@date	2010.06.10

	@return	int
*/
int CaDraw_TagPipe::ClipByViewBoundary(CIsPoint3d pt[2])
{
	CaDraw_View* pDrawView = m_pDrawEntity->GetView();
	if(pDrawView)
	{
		const CIsPoint3d ptOrigin = pDrawView->center();
		const double dViewWidth = pDrawView->GetViewWidth();
		const double dViewHeight= pDrawView->GetViewHeight();

		vector<CIsPoint2d> input , clipPolygon , output;
		input.push_back( CIsPoint2d(pt[0].x() , pt[0].y()) );
		input.push_back( CIsPoint2d(pt[1].x() , pt[1].y()) );

		clipPolygon.push_back(CIsPoint2d(ptOrigin.x() - dViewWidth*0.5 , ptOrigin.y() - dViewHeight*0.5));
		clipPolygon.push_back(CIsPoint2d(ptOrigin.x() + dViewWidth*0.5 , ptOrigin.y() - dViewHeight*0.5));
		clipPolygon.push_back(CIsPoint2d(ptOrigin.x() + dViewWidth*0.5 , ptOrigin.y() + dViewHeight*0.5));
		clipPolygon.push_back(CIsPoint2d(ptOrigin.x() - dViewWidth*0.5 , ptOrigin.y() + dViewHeight*0.5));

		SutherlandHodgmanPolygonClip(input , clipPolygon , output);
		if(3 == output.size())
		{
			pt[0] = CIsPoint3d(output[1].x() , output[1].y() , 0.0);
			pt[1] = CIsPoint3d(output[2].x() , output[2].y() , 0.0);
		}

		/*if(pt[0].x() < (ptOrigin.x() - nViewWidth*0.5))
		{
			pt[0].x() = (ptOrigin.x() - nViewWidth*0.5);
		}
		if(pt[0].y() < (ptOrigin.y() - nViewHeight*0.5))
		{
			pt[0].y() = (ptOrigin.y() - nViewHeight*0.5);
		}
		if(pt[0].x() > (ptOrigin.x() + nViewWidth*0.5))
		{
			pt[0].x() = (ptOrigin.x() + nViewWidth*0.5);
		}
		if(pt[0].y() > (ptOrigin.y() + nViewHeight*0.5))
		{
			CIsVect3d dir(pt[0] - pt[1]);
			dir.Normalize();
			const double t = (ptOrigin.y() + nViewHeight*0.5 - pt[1].y()) / dir.dy();
			pt[0] = pt[1] + dir*t;
			///pt[0].y() = (ptOrigin.y() + nViewHeight*0.5);
			///pt[0].x() = pt[1].x() + dir
		}

		if(pt[1].x() < (ptOrigin.x() - nViewWidth*0.5))
		{
			pt[1].x() = (ptOrigin.x() - nViewWidth*0.5);
		}
		if(pt[1].y() < (ptOrigin.y() - nViewHeight*0.5))
		{
			pt[1].y() = (ptOrigin.y() - nViewHeight*0.5);
		}
		if(pt[1].x() > (ptOrigin.x() + nViewWidth*0.5))
		{
			pt[1].x() = (ptOrigin.x() + nViewWidth*0.5);
		}
		if(pt[1].y() > (ptOrigin.y() + nViewHeight*0.5))
		{
			pt[1].y() = (ptOrigin.y() + nViewHeight*0.5);
		}*/

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	determine to display section mark or not
	@author	humkyung
	@date	2014.04.13
*/
bool CaDraw_TagPipe::ExamineToDisplaySectionMark() const
{
	if(this->IsVertical())
	{
		CIsPoint3d pt[2];
		pt[0] = pipe()->GetStartPoint();
		pt[1] = pipe()->GetEndPoint();

		return (!m_pDrawEntity->GetView()->GetVolume().Contains(pt[0]) || !m_pDrawEntity->GetView()->GetVolume().Contains(pt[1]));
	}

	return false;
}

/**
	@brief	fill section mark when pipe is vertical and overlap with view volume
	@author	humkyung
	@date	2014.04.13
*/
int CaDraw_TagPipe::FillSectionMark()
{
	if(ExamineToDisplaySectionMark())
	{
		CIsPoint3d pt[2];
		pt[0] = pipe()->GetStartPoint();
		pt[1] = pipe()->GetEndPoint();

		pt[0] = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pt[0]);
		pt[1] = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pt[1]);
		CIsPoint3d ptMid = (pt[0] + pt[1])*0.5;
		ptMid.z() = 0.0;

		const double dPipeSize = pipe()->GetView()->ModelCoord2ViewCoord(pipe()->GetSizeInMetric());
		const long lPipeRadius = long((dPipeSize/2) * CSmartDrawAnnoModule::m_pCellMap->scale());
		CCellMap::CellIndex tagOrigin(CSmartDrawAnnoModule::m_pCellMap->PointToIndex(CIsPoint2d(ptMid.x() , ptMid.y())));
		CSmartDrawAnnoModule::m_pCellMap->FillCircleOf(tagOrigin , lPipeRadius , 4);
	}

	return ERROR_SUCCESS;
}

CIsPoint3d MidPointOnCircle(const CIsPoint3d& origin , const CIsPoint3d& lhs , const CIsPoint3d& rhs , const double& dRadius)
{
	CIsPoint3d res;

	res.x() = (lhs.x() + rhs.x())*0.5 - origin.x(); res.y() = (lhs.y() + rhs.y())*0.5 - origin.y();
	const double dLen = sqrt(res.x()*res.x() + res.y()*res.y());
	if(dLen > 0.0)
	{
		res.x() /= dLen;res.y() /= dLen;
		res.x() = origin.x() + res.x()*(dRadius);res.y() = origin.y() + res.y()*(dRadius);
	}

	return res;
}

/**
	@brief	write section mark if this is vertical and overlap with view volume
	@author	humkyung
	@date	2014.04.13
*/
int CaDraw_TagPipe::WriteSectionMark(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	if(ExamineToDisplaySectionMark())
	{
		CIsPoint3d pt[2];
		pt[0] = pipe()->GetStartPoint();
		pt[1] = pipe()->GetEndPoint();

		{
			pt[0] = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pt[0]);
			pt[1] = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pt[1]);
			CIsPoint3d ptMid = (pt[0] + pt[1])*0.5;
			ptMid.z() = 0.0;

			int iHatchingType = CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_iHatchingTypeA;
			if(CaDraw_TagPipe::HATCHING_ENV::DEPEND_ON_SIZE == CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_eHatchingType)
			{
				iHatchingType = (CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oSize.GreaterThanInMetric(pipe()->GetSizeInMetric())) ? 
					CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_iHatchingTypeA : CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_iHatchingTypeB;
			}
			const double dPipeSize = pipe()->GetView()->ModelCoord2ViewCoord(pipe()->GetNominalPipeSize()*25.4);
			
			ofile << _T("<GROUP>|0,HATCH,Pat 01") << std::endl;
			ofile << _T("<ELLIPSE>|");
			ofile << ptMid.x()*dDrawingScale << _T(",") << ptMid.y()*dDrawingScale << _T(",") << (dPipeSize*0.5)*dDrawingScale << _T(",") << (dPipeSize*0.5)*dDrawingScale << _T(",0|");
			ofile << CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oHatchingProp << std::endl;
			
			switch(iHatchingType)
			{
				case 0:
					break;
				case 1:
					ofile << _T("<HATCHCIRCLE>|");
					ofile << ptMid.x()*dDrawingScale << _T(",") << ptMid.y()*dDrawingScale << _T(",") << (dPipeSize*0.5)*dDrawingScale << _T("|");
					ofile << CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oHatchingProp.level << _T(",") << CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oHatchingProp.color << _T(",");
					ofile << _T("NULL,NULL,");
					ofile << _T("45,") << (dPipeSize/3.5) << _T(",No") << std::endl;
					break;
				case 2: case 3:
				{
					CIsPoint3d pts[5];
					{
						pts[0].x() = ptMid.x() + (dPipeSize*0.5); pts[0].y() = ptMid.y();
						pts[4].x() = ptMid.x(); pts[4].y() = ptMid.y() + (dPipeSize*0.5);
						pts[2].x() = (pts[0].x() + pts[4].x())*0.5 - ptMid.x(); pts[2].y() = (pts[0].y() + pts[4].y())*0.5 - ptMid.y();
						double dLen = sqrt(pts[2].x()*pts[2].x() + pts[2].y()*pts[2].y());
						pts[2].x() /= dLen;pts[2].y() /= dLen;
						pts[2].x() = ptMid.x() + pts[2].x()*(dPipeSize*0.5);pts[2].y() = ptMid.y() + pts[2].y()*(dPipeSize*0.5);
						
						pts[1].x() = (pts[0].x() + pts[2].x())*0.5 - ptMid.x(); pts[1].y() = (pts[0].y() + pts[2].y())*0.5 - ptMid.y();
						dLen = sqrt(pts[1].x()*pts[1].x() + pts[1].y()*pts[1].y());
						pts[1].x() /= dLen;pts[1].y() /= dLen;
						pts[1].x() = ptMid.x() + pts[1].x()*(dPipeSize*0.5);pts[1].y() = ptMid.y() + pts[1].y()*(dPipeSize*0.5);

						pts[3].x() = (pts[2].x() + pts[4].x())*0.5 - ptMid.x(); pts[3].y() = (pts[2].y() + pts[4].y())*0.5 - ptMid.y();
						dLen = sqrt(pts[3].x()*pts[3].x() + pts[3].y()*pts[3].y());
						pts[3].x() /= dLen;pts[3].y() /= dLen;
						pts[3].x() = ptMid.x() + pts[3].x()*(dPipeSize*0.5);pts[3].y() = ptMid.y() + pts[3].y()*(dPipeSize*0.5);
					}
					ofile << _T("<HATCHPLINE>|");
					ofile << ptMid.x()*dDrawingScale << _T(",") << ptMid.y()*dDrawingScale << _T(",");
					ofile << pts[0].x()*dDrawingScale << _T(",") << pts[0].y()*dDrawingScale << _T(",");
					ofile << pts[1].x()*dDrawingScale << _T(",") << pts[1].y()*dDrawingScale << _T(",");
					ofile << pts[2].x()*dDrawingScale << _T(",") << pts[2].y()*dDrawingScale << _T(",");
					ofile << pts[3].x()*dDrawingScale << _T(",") << pts[3].y()*dDrawingScale << _T(",");
					ofile << pts[4].x()*dDrawingScale << _T(",") << pts[4].y()*dDrawingScale << _T("|");
					ofile << CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oHatchingProp.level << _T(",") << CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oHatchingProp.color;
					ofile << ((2 == iHatchingType) ? _T(",NULL,NULL,45,") : _T(",NULL,SOLID,45,")) << (dPipeSize/3.5)*dDrawingScale << _T(",No") << std::endl;

					{
						pts[0].x() = ptMid.x() - (dPipeSize*0.5); pts[0].y() = ptMid.y();
						pts[4].x() = ptMid.x(); pts[4].y() = ptMid.y() - (dPipeSize*0.5);
						pts[2].x() = (pts[0].x() + pts[4].x())*0.5 - ptMid.x(); pts[2].y() = (pts[0].y() + pts[4].y())*0.5 - ptMid.y();
						double dLen = sqrt(pts[2].x()*pts[2].x() + pts[2].y()*pts[2].y());
						pts[2].x() /= dLen;pts[2].y() /= dLen;
						pts[2].x() = ptMid.x() + pts[2].x()*(dPipeSize*0.5);pts[2].y() = ptMid.y() + pts[2].y()*(dPipeSize*0.5);
						
						pts[1].x() = (pts[0].x() + pts[2].x())*0.5 - ptMid.x(); pts[1].y() = (pts[0].y() + pts[2].y())*0.5 - ptMid.y();
						dLen = sqrt(pts[1].x()*pts[1].x() + pts[1].y()*pts[1].y());
						pts[1].x() /= dLen;pts[1].y() /= dLen;
						pts[1].x() = ptMid.x() + pts[1].x()*(dPipeSize*0.5);pts[1].y() = ptMid.y() + pts[1].y()*(dPipeSize*0.5);

						pts[3].x() = (pts[2].x() + pts[4].x())*0.5 - ptMid.x(); pts[3].y() = (pts[2].y() + pts[4].y())*0.5 - ptMid.y();
						dLen = sqrt(pts[3].x()*pts[3].x() + pts[3].y()*pts[3].y());
						pts[3].x() /= dLen;pts[3].y() /= dLen;
						pts[3].x() = ptMid.x() + pts[3].x()*(dPipeSize*0.5);pts[3].y() = ptMid.y() + pts[3].y()*(dPipeSize*0.5);
					}
					ofile << _T("<HATCHPLINE>|");
					ofile << ptMid.x()*dDrawingScale << _T(",") << ptMid.y()*dDrawingScale << _T(",");
					ofile << pts[0].x()*dDrawingScale << _T(",") << pts[0].y()*dDrawingScale << _T(",");
					ofile << pts[1].x()*dDrawingScale << _T(",") << pts[1].y()*dDrawingScale << _T(",");
					ofile << pts[2].x()*dDrawingScale << _T(",") << pts[2].y()*dDrawingScale << _T(",");
					ofile << pts[3].x()*dDrawingScale << _T(",") << pts[3].y()*dDrawingScale << _T(",");
					ofile << pts[4].x()*dDrawingScale << _T(",") << pts[4].y()*dDrawingScale << _T("|");
					ofile << CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oHatchingProp.level << _T(",") << CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oHatchingProp.color;
					ofile << ((2 == iHatchingType) ? _T(",NULL,NULL,45,") : _T(",NULL,SOLID,45,")) << (dPipeSize/3.5)*dDrawingScale << _T(",No") << std::endl;
				}
				break;
				case 4: case 5:
				{
					CIsPoint3d pts[16];
					{
						pts[0].x() = ptMid.x() + (dPipeSize*0.5); pts[0].y() = ptMid.y();
						pts[4].x() = ptMid.x(); pts[4].y() = ptMid.y() + (dPipeSize*0.5);
						pts[2] = MidPointOnCircle(ptMid , pts[0] ,  pts[4] , dPipeSize*0.5);
						pts[1] = MidPointOnCircle(ptMid , pts[0] ,  pts[2] , dPipeSize*0.5);
						pts[3] = MidPointOnCircle(ptMid , pts[2] ,  pts[4] , dPipeSize*0.5);

						pts[6].x() = ptMid.x() - (dPipeSize*0.25); pts[6].y() = ptMid.y() + (dPipeSize*0.25);
						pts[5] = MidPointOnCircle(CIsPoint3d(ptMid.x() , ptMid.y() + dPipeSize*0.25,0) , pts[4] ,  pts[6] , dPipeSize*0.25);
						pts[8] = ptMid;
						pts[7] = MidPointOnCircle(CIsPoint3d(ptMid.x() , ptMid.y() + dPipeSize*0.25,0) , pts[6] ,  pts[8] , dPipeSize*0.25);

						pts[10].x() = ptMid.x() + (dPipeSize*0.25); pts[10].y() = ptMid.y() - (dPipeSize*0.25);
						pts[9] = MidPointOnCircle(CIsPoint3d(ptMid.x() , ptMid.y() - dPipeSize*0.25,0) , pts[8] ,  pts[10] , dPipeSize*0.25);
						pts[12].x() = ptMid.x(); pts[12].y() = ptMid.y() - (dPipeSize*0.5);
						pts[11] = MidPointOnCircle(CIsPoint3d(ptMid.x() , ptMid.y() - dPipeSize*0.25,0) , pts[10] ,  pts[12] , dPipeSize*0.25);

						pts[14] = MidPointOnCircle(ptMid , pts[12] ,  pts[0] , dPipeSize*0.5);
						pts[13] = MidPointOnCircle(ptMid , pts[12] ,  pts[14] , dPipeSize*0.5);
						pts[15] = MidPointOnCircle(ptMid , pts[14] ,  pts[0] , dPipeSize*0.5);
					}
					ofile << _T("<HATCHPLINE>|");
					const int iSize = SIZE_OF_ARRAY(pts);
					for(int i = 0;i < iSize;++i)
					{
						ofile << pts[i].x()*dDrawingScale << _T(",") << pts[i].y()*dDrawingScale;
						(i < (iSize - 1)) ? (ofile  << _T(",")) : (ofile << _T("|"));
					}
					ofile << CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oHatchingProp.level << _T(",") << CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oHatchingProp.color;
					ofile << ((4 == iHatchingType) ? _T(",NULL,NULL,45,") : _T(",NULL,SOLID,45,")) << (dPipeSize/3.5)*dDrawingScale << _T(",No") << std::endl;
				}
				break;
			};

			/// draw cross lines
			ofile << _T("<LINE>|");
			ofile << (ptMid.x() - ((dPipeSize*0.5) + (dPipeSize/3.5)))*dDrawingScale << _T(",") << ptMid.y()*dDrawingScale << _T(",0,");
			ofile << (ptMid.x() + ((dPipeSize*0.5) + (dPipeSize/3.5)))*dDrawingScale << _T(",") << ptMid.y()*dDrawingScale << _T(",0|");
			ofile << CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oHatchingProp << std::endl;
			ofile << _T("<LINE>|");
			ofile << ptMid.x()*dDrawingScale << _T(",") << (ptMid.y() - ((dPipeSize*0.5) + (dPipeSize/3.5)))*dDrawingScale << _T(",0,");
			ofile << ptMid.x()*dDrawingScale << _T(",") << (ptMid.y() + ((dPipeSize*0.5) + (dPipeSize/3.5)))*dDrawingScale << _T(",0|");
			ofile << CaDraw_TagPipe::HATCHING_ENV::GetInstance().m_oHatchingProp << std::endl;
			/// up to here
			ofile << _T("</GROUP>") << std::endl;
		}	
	}

	return ERROR_SUCCESS;
}