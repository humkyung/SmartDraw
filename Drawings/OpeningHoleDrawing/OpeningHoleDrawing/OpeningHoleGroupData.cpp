#include "StdAfx.h"
#include <assert.h>
#include <IsCircle2d.h>
#include <IsPoly2d.h>
#include <IsConvexHull.h>
#include "OpeningHoleGroupData.h"
#include "OpeningHoleDrawingOption.h"

int COpeningHoleGroupData::m_iGroupCount = 1;

COpeningHoleGroupData::COpeningHoleGroupData(const int& iGroupNo) : m_iGroupNo(iGroupNo)
{
}

COpeningHoleGroupData::~COpeningHoleGroupData(void)
{
	try
	{
		for (vector<COpeningHoleData*>::iterator itr = m_oOpeningHoleDataList.begin(); itr != m_oOpeningHoleDataList.end(); ++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_oOpeningHoleDataList.clear();
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   Annotate
    @return     int
    @param      CCellMap*   pCellMap
    @brief
******************************************************************************/
int COpeningHoleGroupData::Annotate(CCellMap* pCellMap)
{
	COpeningHoleDrawingOption& env = COpeningHoleDrawingOption::GetInstance();

	for (vector<COpeningHoleData*>::iterator itr = m_oOpeningHoleDataList.begin(); itr != m_oOpeningHoleDataList.end(); ++itr)
	{
		(*itr)->Annotate(pCellMap);
	}

	if(m_oOpeningHoleDataList.size() > 1)
	{
		if(_T("Box") == env.GetGroupType())
		{
			m_vol.Reset();
			for (vector<COpeningHoleData*>::iterator itr = m_oOpeningHoleDataList.begin(); itr != m_oOpeningHoleDataList.end(); ++itr)
			{
				CIsPoint3d pt((*itr)->origin().x() , (*itr)->origin().y() , 0.0);
				const double dRadius = (*itr)->radius();
				m_vol.Add(CIsPoint3d(pt.x() - dRadius , pt.y() - dRadius , 0.0));
				m_vol.Add(CIsPoint3d(pt.x() + dRadius , pt.y() - dRadius , 0.0));
				m_vol.Add(CIsPoint3d(pt.x() + dRadius , pt.y() + dRadius , 0.0));
				m_vol.Add(CIsPoint3d(pt.x() - dRadius , pt.y() + dRadius , 0.0));
			}
			m_origin = CIsPoint2d(m_vol.center().x() , m_vol.center().y());

			const double dViewScale = m_oOpeningHoleDataList[0]->tagPipe()->GetView()->GetScale();
			const int iWidth = int(ceil((m_vol.maxx() - m_vol.minx())*dViewScale));
			const int iHeight= int(ceil((m_vol.maxy() - m_vol.miny())*dViewScale));
			OSTRINGSTREAM_T oss;
			oss << _T("#") << m_iGroupNo << _T(" ") << iWidth << _T("x") << iHeight;
			m_sTag = oss.str();

			const double nTextLength = env.m_oTextStyle.GetTextLength(m_sTag);
			const long lItemCellHeight   = ceil((env.m_oTextStyle.m_nHeight*1.2) * pCellMap->scale());
			const long lItemCellLength   = ceil((nTextLength) * pCellMap->scale());
			long lSearchCellLength = lItemCellHeight * 20;	//! this value comes from experience.

			CCellMap::CellConversionAxis cAxis;
			cAxis.xAxis.Set(1 , 0);
			cAxis.yAxis.Set(0 , 1);

			CCellMap::CellIndex tagOrigin(pCellMap->PointToIndex(CIsPoint2d(m_vol.center().x() , m_vol.center().y())));
			CCellMap::CellIndex cOrigin(tagOrigin);
			cOrigin.x -= long(SAFE_ROUND(lSearchCellLength * 0.5 , 0));
			cOrigin.y -= long(SAFE_ROUND(lSearchCellLength * 0.5 , 0));

			CCellMap::SIZE_T sizSearch , sizItem;
			sizSearch.width = lSearchCellLength; sizSearch.height = lSearchCellLength;
			sizItem.width = lItemCellLength; sizItem.height = lItemCellHeight;
			CCellMap::SearchRes res;
			bool bFound = pCellMap->FindBestLoc(res , cOrigin , tagOrigin , sizSearch , sizItem , cAxis , 0xFF);
			if(bFound)
			{
				m_ptTagOrigin = m_ptLeader[1] = res.ptLoc;
				m_ptTagOrigin.x() += res.cAxis.yAxis.dx()*0.6;
				m_ptTagOrigin.y() += res.cAxis.yAxis.dy()*0.6;

				res.coords[0] = CIsPoint2d(m_origin.x() , m_origin.y());
				res.coords[1] = res.ptLoc;
				res.coords[2] = res.ptLoc + CIsPoint2d(nTextLength , 0.0);
				m_ptLeader[2].x() = m_ptLeader[1].x() + res.cAxis.xAxis.dx()*nTextLength;
				m_ptLeader[2].y() = m_ptLeader[1].y() + res.cAxis.xAxis.dy()*nTextLength;
				if(m_origin.DistanceTo(m_ptLeader[2]) < m_origin.DistanceTo(m_ptLeader[1]))
				{
					swap(m_ptLeader[1] , m_ptLeader[2]);
				}
				/// make leader indicate box 
				m_ptLeader[0] = CIsPoint2d(m_vol.center().x() , m_vol.center().y());
				/// up to here

				pCellMap->UpdateRectOf(res.cIndex , lItemCellLength , lItemCellHeight , res.cAxis , CCellMap::TAG);

				vector<CIsPoint2d> input , oClipPolygon , output;
				input.push_back(m_ptLeader[0]);
				input.push_back(m_ptLeader[1]);
				oClipPolygon.push_back(CIsPoint2d(m_vol.minx() , m_vol.miny()));
				oClipPolygon.push_back(CIsPoint2d(m_vol.maxx() , m_vol.miny()));
				oClipPolygon.push_back(CIsPoint2d(m_vol.maxx() , m_vol.maxy()));
				oClipPolygon.push_back(CIsPoint2d(m_vol.minx() , m_vol.maxy()));
				oClipPolygon.push_back(CIsPoint2d(m_vol.minx() , m_vol.miny()));
				SutherlandHodgmanPolygonClip(input , oClipPolygon , output);
				m_ptLeader[0] = output[0];
			}
		}
		else
		{
			CIsPoly2d oPoly2d;
			CIsPoint2d pts[24];
			for (vector<COpeningHoleData*>::iterator itr = m_oOpeningHoleDataList.begin(); itr != m_oOpeningHoleDataList.end(); ++itr)
			{
				CIsCircle2d oCircle2d((*itr)->origin() , (*itr)->radius());
				oCircle2d.digitize(pts , 24);
				for(int i = 0;i < 24;++i)
				{
					oPoly2d.AddVertex(pts[i]);
				}
			}

			CIsConvexHull oConvexHull;									
			m_oGroupOutline = oConvexHull.Create( oPoly2d );
		}
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   Write
    @return     void
    @param      OFSTREAM_T& ofile
    @brief
******************************************************************************/
void COpeningHoleGroupData::Write(OFSTREAM_T& ofile)
{
	COpeningHoleDrawingOption& env = COpeningHoleDrawingOption::GetInstance();

	for (vector<COpeningHoleData*>::iterator itr = m_oOpeningHoleDataList.begin(); itr != m_oOpeningHoleDataList.end(); ++itr)
	{
		(*itr)->Write(ofile);
	}
	
	if(m_oOpeningHoleDataList.size() > 1)
	{
		if(_T("Box") == env.GetGroupType())
		{
			ofile << _T("<PLINE>|");
			ofile << m_vol.minx() << _T(",") << m_vol.miny() << _T(",");
			ofile << m_vol.maxx() << _T(",") << m_vol.miny() << _T(",");
			ofile << m_vol.maxx() << _T(",") << m_vol.maxy() << _T(",");
			ofile << m_vol.minx() << _T(",") << m_vol.maxy() << _T(",");
			ofile << m_vol.minx() << _T(",") << m_vol.miny() << _T("|");
			ofile << _T("0,0,0,0,0") << std::endl;

			ofile << _T("<GROUP>|0,Hole,Pat 01") << std::endl;

			ofile << _T("<TEXT>|") << m_ptTagOrigin.x() << _T(",") << m_ptTagOrigin.y() << _T(",0,");		
			ofile << env.m_oTextStyle.m_nHeight << _T(",") << env.m_oTextStyle.m_nHeight*env.m_oTextStyle.m_nWidthFactor << _T(",");
			ofile << ("0,") << m_sTag << _T(",Left Bottom,0|");
			ofile << _T("0,4,") << env.m_oTextStyle.m_rFontName << _T(",0") << std::endl;

			ofile << _T("<LEADER>|") << m_ptLeader[0].x() << _T(",") << m_ptLeader[0].y() << _T(",");		
			ofile << m_ptLeader[1].x() << _T(",") << m_ptLeader[1].y() << _T(",");
			ofile << m_ptLeader[2].x() << _T(",") << m_ptLeader[2].y() << _T("|");
			ofile << _T("0,4,CLOSE,3,1") << std::endl;

			ofile << _T("</GROUP>") << std::endl;
		}
		else
		{
			ofile << _T("<PLINE>|");

			const int iCount = m_oGroupOutline.GetVertexCount();
			if(iCount > 2)
			{
				for(int i = 0;i < iCount;++i)
				{
					ofile << m_oGroupOutline[i].x() << _T(",") << m_oGroupOutline[i].y() << _T(",");
				}
				ofile << m_oGroupOutline[0].x() << _T(",") << m_oGroupOutline[0].y() << _T("|");
			}

			ofile << _T("0,0,0,0,0") << std::endl;
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   WriteChart
    @return     void
    @param      OFSTREAM_T& ofile
    @param      const       double&
    @param      x           const
    @param      double&     y
    @brief
******************************************************************************/
int COpeningHoleGroupData::WriteChart(OFSTREAM_T& ofile , double& locx , double& locy)
{
	COpeningHoleDrawingOption& env = COpeningHoleDrawingOption::GetInstance();
	CLabelWidthContainer oLabelWidthContainer = env.GetLabelWidthContainer();
	const double nTextHeight = env.m_oChartTextStyle.m_nHeight;
	const double nChartRowHeight = env.GetChartRowHeight();
	const int nTextFont = ATOI_T(env.m_oChartTextStyle.m_rFontName.c_str());
	CompProp ChartLineProp = env.GetChartLineProp();
	CompProp ChartLabelProp = env.GetChartLabelProp();

	const double nTopY = locy;
	for(vector<COpeningHoleData*>::iterator itr = m_oOpeningHoleDataList.begin();itr != m_oOpeningHoleDataList.end();++itr)
	{
		(*itr)->WriteChart( ofile , locx + oLabelWidthContainer.GetWidth(0) , locy - nChartRowHeight + (nChartRowHeight - nTextHeight) * 0.5);
		
		//! 아이템의 밑 가로줄을 그린다.
		if((itr + 1) != m_oOpeningHoleDataList.end())
		{
			ofile << _T("<LINE>|");
			ofile << locx + oLabelWidthContainer.GetWidth(0) << _T(",") << (locy - nChartRowHeight) << _T(",0,"); 
			ofile << locx + oLabelWidthContainer.GetTotalWidth() << _T(",") << (locy - nChartRowHeight) << _T(",0|");
			ofile << ChartLineProp.level << _T(",") << ChartLineProp.color <<  _T(",Default,Default,") << ChartLineProp.weight << std::endl;
		}

		locy -= nChartRowHeight;
	}
	//! Hole Number를 쓴다.
	ofile << _T("<TEXT>|");
	ofile << locx + oLabelWidthContainer.GetWidth(0) * 0.5 << _T(",") << (nTopY + locy - nTextHeight) * 0.5 << _T(",0,");
	ofile << nTextHeight << _T(",") <<  nTextHeight * env.m_oChartTextStyle.m_nWidthFactor << _T(",0,") << m_iGroupNo << _T(",Center Center,0|");
	ofile << ChartLabelProp.level << _T(",") << ChartLabelProp.color << _T(",") << nTextFont << _T(",0") <<std::endl;

	//! 제일 밑 가로줄을 그린다.
	ofile << _T("<LINE>|");
	ofile << locx << _T(",") << locy << _T(",0,");
	ofile << locx + oLabelWidthContainer.GetTotalWidth() << _T(",") << locy << _T(",0|");
	ofile << ChartLineProp.level << _T(",") << ChartLineProp.color <<  _T(",Default,Default,") << ChartLineProp.weight << std::endl;

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   Add
    @return     int
    @param      COpeningHoleData*   pOpeningHoleData
    @brief
******************************************************************************/
int COpeningHoleGroupData::Add(COpeningHoleData* pOpeningHoleData)
{
	assert(pOpeningHoleData && "pOpeningHoleData is NULL");
	if(pOpeningHoleData)
	{
		m_oOpeningHoleDataList.push_back(pOpeningHoleData);
		pOpeningHoleData->SetParent(this);

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   IsPossibleToBeMember
    @return     bool
    @param      COpeningHoleData*   pData
    @brief
******************************************************************************/
bool COpeningHoleGroupData::IsPossibleToBeMember(COpeningHoleData* pData)
{
	assert(pData && "pData is NULL");
	COpeningHoleDrawingOption& env = COpeningHoleDrawingOption::GetInstance();

	if(pData)
	{
		if(m_oOpeningHoleDataList.empty())
		{
			return true;
		}
		else
		{
			for (vector<COpeningHoleData*>::iterator itr = m_oOpeningHoleDataList.begin(); itr != m_oOpeningHoleDataList.end(); ++itr)
			{
				//! 두 파이프 센트 거리를 구한다.
				const double hdist = (*itr)->origin().DistanceTo(pData->origin());
				if (hdist > 0.1)
				{
					const double cdist = pData->radius() + (*itr)->radius();
					if (hdist < cdist) return true;;
				}
			}

			return false;
		}

		return true;
	}

	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   groupNo
    @return     int
    @brief
******************************************************************************/
int COpeningHoleGroupData::groupNo() const
{
	return m_iGroupNo;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   _SortBySize
    @return     bool
    @param      COpeningHoleData*   lhs
    @param      COpeningHoleData*   rhs
    @brief
******************************************************************************/
static bool _SortBySize(COpeningHoleData* lhs , COpeningHoleData* rhs)
{
	return (lhs->tagPipe()->pipe()->GetSize() < rhs->tagPipe()->pipe()->GetSize());
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   SortBySize
    @return     int
    @brief		PIPE SIZE기준으로 소팅한다.
******************************************************************************/
int COpeningHoleGroupData::SortBySize()
{
	::stable_sort(m_oOpeningHoleDataList.begin() , m_oOpeningHoleDataList.end() , _SortBySize);
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   GetOpeningHoleCount
    @return     int
    @brief
******************************************************************************/
int COpeningHoleGroupData::GetOpeningHoleCount() const
{
	return m_oOpeningHoleDataList.size();
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   GetOpeningHoleDataAt
    @return     COpeningHoleData*
    @param      const   int&
    @param      at
    @brief
******************************************************************************/
COpeningHoleData* COpeningHoleGroupData::GetOpeningHoleDataAt(const size_t& at)
{
	assert((at < m_oOpeningHoleDataList.size()) && "out of index");

	if(at < m_oOpeningHoleDataList.size())
	{
		return m_oOpeningHoleDataList[at];
	}

	return NULL;
}