#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include <IsString.h>
#include "OpeningHoleData.h"
#include "OpeningHoleDrawingOption.h"
#include "OpeningHoleGroupData.h"

#include <fstream>
using namespace std;

COpeningHoleData::COpeningHoleData(const CIsPoint2d& origin , CaDraw_TagPipe* pTagPipe) : m_origin(origin) , m_pTagPipe(pTagPipe)
{
	m_dCircleRadius = 0.0;

	COpeningHoleDrawingOption& env = COpeningHoleDrawingOption::GetInstance();

	CaDraw_Pipe* pPipe = m_pTagPipe->pipe();
	const double dPipeSize = pPipe->GetSize();	/// in Inch

	const double dClearance = env.GetClearanceOf(dPipeSize);
	m_iHoleSize = int(ceil(dPipeSize*25.4 + pPipe->insThick()*25.4*2 + dClearance*2));
	m_iHoleSize = (m_iHoleSize/5)*5 + (((m_iHoleSize % 5) > 0) ? 5 : 0);

	m_dCircleRadius = m_iHoleSize*0.5;
	m_dCircleRadius = pPipe->GetView()->ModelCoord2ViewCoord(m_dCircleRadius);

	m_pParent = NULL;
}

COpeningHoleData::~COpeningHoleData(void)
{
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   pipe
    @return     CaDraw_TagPipe*
    @brief
******************************************************************************/
CaDraw_TagPipe* COpeningHoleData::tagPipe() const
{
	return m_pTagPipe;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   origin
    @return     CIsPoint2d
    @brief
******************************************************************************/
CIsPoint2d COpeningHoleData::origin() const
{
	return m_origin;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   radius
    @return     double
    @brief
******************************************************************************/
double COpeningHoleData::radius() const
{
	return m_dCircleRadius;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   parent
    @return     COpeningHoleGroupData*
    @brief
******************************************************************************/
COpeningHoleGroupData* COpeningHoleData::parent() const
{
	return m_pParent;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   SetParent
    @return     int
    @param      COpeningHoleGroupData*  pParent
    @brief
******************************************************************************/
int COpeningHoleData::SetParent(COpeningHoleGroupData* pParent)
{
	m_pParent = pParent;
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   Annotate
    @return     int
    @param      CCellMap*   pCellMap
    @brief
******************************************************************************/
int COpeningHoleData::Annotate(CCellMap* pCellMap)
{
	assert(pCellMap && "pCellMap is NULL");
	if(pCellMap)
	{
		COpeningHoleDrawingOption& env = COpeningHoleDrawingOption::GetInstance();

		m_sTag.clear();
		OSTRINGSTREAM_T oss;
		oss << _T("#") << m_pParent->groupNo() << _T(" DIA ") << m_iHoleSize;
		m_sTag = oss.str();

		const double nTextLength = env.m_oTextStyle.GetTextLength(m_sTag);
		const long lItemCellHeight   = ceil((env.m_oTextStyle.m_nHeight*1.2) * pCellMap->scale());
		const long lItemCellLength   = ceil((nTextLength) * pCellMap->scale());
		long lSearchCellLength = lItemCellHeight * 20;	//! this value comes from experience.

		CCellMap::CellConversionAxis cAxis;
		cAxis.xAxis.Set(1 , 0);
		cAxis.yAxis.Set(0 , 1);

		CCellMap::CellIndex tagOrigin(pCellMap->PointToIndex(CIsPoint2d(m_origin.x() , m_origin.y())));
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
			/// make leader indicate circle 
			CIsVect2d dir(m_ptLeader[1] - m_origin);
			dir.Normalize();
			m_ptLeader[0] = m_origin + dir*m_dCircleRadius;
			/// up to here

			pCellMap->UpdateRectOf(res.cIndex , lItemCellLength , lItemCellHeight , res.cAxis , CCellMap::TAG);
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-10
    @function   Write
    @return     void
    @param      OFSTREAM_T& ofile
    @brief
******************************************************************************/
void COpeningHoleData::Write(OFSTREAM_T& ofile)
{
	COpeningHoleDrawingOption& env = COpeningHoleDrawingOption::GetInstance();

	ofile << _T("<CIRCLE>|");
	ofile << m_origin.x() << _T(",") << m_origin.y() << _T(",0,") << m_dCircleRadius << _T("|");
	ofile << _T("0,3,0,0,0") << std::endl;

	//! draw hole cross line
	ofile << _T("<LINE>|");
	ofile << (m_origin.x() - m_dCircleRadius - 2) << _T(",") << m_origin.y() << _T(",0,");
	ofile << (m_origin.x() + m_dCircleRadius + 2) << _T(",") << m_origin.y() << _T(",0|");
	ofile << _T("0,15,0,1,0") << std::endl;

	ofile << _T("<LINE>|");
	ofile << m_origin.x() << _T(",") << (m_origin.y() - m_dCircleRadius - 2) << _T(",0,");
	ofile << m_origin.x() << _T(",") << (m_origin.y() + m_dCircleRadius + 2) << _T(",0|");
	ofile << _T("0,15,0,1,0") << std::endl;

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

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   WriteChart
    @return     int
    @param      OFSTREAM_T& ofile
    @param      double      x
    @param      double      y
    @brief
******************************************************************************/
int COpeningHoleData::WriteChart(OFSTREAM_T& ofile , double x , double y)
{
	COpeningHoleDrawingOption& env = COpeningHoleDrawingOption::GetInstance();
	const double nTextHeight = env.m_oChartTextStyle.m_nHeight;
	const int nTextFont = ATOI_T(env.m_oChartTextStyle.m_rFontName.c_str());
	const int nTextColor= env.GetChartTextColor();
	const double nChartRowHeight = env.GetChartRowHeight();
	CLabelWidthContainer oLabelWidthContainer = env.GetLabelWidthContainer();
	CompProp ChartLabelProp = env.GetChartLabelProp();
	
	ofile << _T("<TEXT>|");
	ofile << x + oLabelWidthContainer.GetWidth(1) * 0.5 << _T(",") << y << _T(",0,");
	ofile << nTextHeight << _T(",") << nTextHeight * env.m_oChartTextStyle.m_nWidthFactor << _T(",0,") << m_pTagPipe->pipe()->line_label() << _T(",Center Center,0|");
	ofile << ChartLabelProp.level << _T(",") << nTextColor << _T(",") << nTextFont << _T(",0") <<std::endl;

	x += oLabelWidthContainer.GetWidth(1);
	ofile << _T("<TEXT>|");
	ofile << x + oLabelWidthContainer.GetWidth(2) * 0.5 << _T(",") << y << _T(",0,");
	ofile << nTextHeight << _T(",") << nTextHeight * env.m_oChartTextStyle.m_nWidthFactor << _T(",0,R ") << int(m_iHoleSize*0.5) << _T(",Center Center,0|");
	ofile << ChartLabelProp.level << nTextColor << _T(",") << _T(",") << nTextFont << _T(",0") <<std::endl;

	x += oLabelWidthContainer.GetWidth(2);
	ofile << _T("<TEXT>|");
	ofile << x + oLabelWidthContainer.GetWidth(3) * 0.5 << _T(",") << y << _T(",0,");
	ofile << nTextHeight << _T(",") << nTextHeight * env.m_oChartTextStyle.m_nWidthFactor << _T(",0,");
	ofile << IsString::TrimedTrailingZero(m_pTagPipe->pipe()->GetSize()) << _T(",Center Center,0|");
	ofile << ChartLabelProp.level << nTextColor << _T(",") << _T(",") << nTextFont << _T(",0") <<std::endl;

	x += oLabelWidthContainer.GetWidth(3);
	ofile << _T("<TEXT>|");
	ofile << x + oLabelWidthContainer.GetWidth(4) * 0.5 << _T(",") << y << _T(",0,");
	ofile << nTextHeight << _T(",") << nTextHeight * env.m_oChartTextStyle.m_nWidthFactor << _T(",0,");
	ofile << IsString::TrimedTrailingZero(m_pTagPipe->pipe()->insThickInMetric()) << _T(",Center Center,0|");
	ofile << ChartLabelProp.level << nTextColor << _T(",") << _T(",") << nTextFont << _T(",0") <<std::endl;

	x += oLabelWidthContainer.GetWidth(4);
	ofile << _T("<TEXT>|");
	ofile << x + oLabelWidthContainer.GetWidth(5) * 0.5 << _T(",") << y << _T(",0,");
	ofile << nTextHeight << _T(",") << nTextHeight * env.m_oChartTextStyle.m_nWidthFactor << _T(",0,");
	ofile << _T("E. ") << int(SAFE_ROUND(m_ptModelOrigin.x(),0)) << _T(" N. ") << int(SAFE_ROUND(m_ptModelOrigin.y(),0)) << _T(",Center Center,0|");
	ofile << ChartLabelProp.level << _T(",") << nTextColor << _T(",") << nTextFont << _T(",0") <<std::endl;
	
	x += oLabelWidthContainer.GetWidth(5);
	ofile << _T("<TEXT>|");
	ofile << x + oLabelWidthContainer.GetWidth(6) * 0.5 << _T(",") << y << _T(",0,");
	ofile << nTextHeight << _T(",") << nTextHeight * env.m_oChartTextStyle.m_nWidthFactor << _T(",0,");
	ofile << int(SAFE_ROUND(m_ptModelOrigin.z(),0)) << _T(",Center Center,0|");
	ofile << ChartLabelProp.level << _T(",") << nTextColor << _T(",") << nTextFont << _T(",0") <<std::endl;

	return ERROR_SUCCESS;
}