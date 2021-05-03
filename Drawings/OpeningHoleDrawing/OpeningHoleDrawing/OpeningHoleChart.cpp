#include "StdAfx.h"
#include "OpeningHoleChart.h"
#include "OpeningHoleDrawingOption.h"

COpeningHoleChart::COpeningHoleChart(void)
{
}

COpeningHoleChart::~COpeningHoleChart(void)
{
}

bool __SortByGroupNo(COpeningHoleGroupData* lhs , COpeningHoleGroupData* rhs)
{
	return (lhs->groupNo() < rhs->groupNo());
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   WriteChartFile
    @return     int
    @param      OFSTREAM_T& ofile
    @brief
******************************************************************************/
int COpeningHoleChart::Write(OFSTREAM_T& ofile)
{
	COpeningHoleDrawingOption& env = COpeningHoleDrawingOption::GetInstance();

	/// sort by group no
	stable_sort(m_oHoleChartItemGroupEntry.begin() , m_oHoleChartItemGroupEntry.end() , __SortByGroupNo);

	CLocation location = env.GetChartLocation();
	double locx = location.x();
	double locy = location.y();

	CLabelWidthContainer oLabelWidthContainer = env.GetLabelWidthContainer();

	if(_T("UpperRight") == location.corner())
	{
		locx -= oLabelWidthContainer.GetTotalWidth();
	}

	const double nTextHeight = env.m_oChartTextStyle.m_nHeight;
	const double nChartRowHeight = env.GetChartRowHeight();
	CompProp ChartLineProp = env.GetChartLineProp();

	if(!m_oHoleChartItemGroupEntry.empty())
	{
		double x = locx, y = locy;
		vector<STRING_T> oLabelEntry;
		env.GetLabelEntry( oLabelEntry );

		//! 맨 윗 가로줄을 그린다.
		ofile << _T("<LINE>|");
		ofile << locx << _T(",") << y << _T(",0,");
		ofile << locx + oLabelWidthContainer.GetTotalWidth() << _T(",") << y << _T(",0|");
		ofile << ChartLineProp.level << _T(",") << ChartLineProp.color << _T(",") << ChartLineProp.style << _T(",Default,") << ChartLineProp.weight << std::endl;
		this->WriteLabel( ofile , x , y - nChartRowHeight + (nChartRowHeight - nTextHeight) * 0.5 , oLabelEntry , oLabelWidthContainer );
		y -= nChartRowHeight;
		//! label 바로 밑 줄을 그린다.
		ofile << _T("<LINE>|");
		ofile << locx << _T(",") << y << _T(",0,");
		ofile << locx + oLabelWidthContainer.GetTotalWidth() << _T(",") << y << _T(",0|");
		ofile << ChartLineProp.level << _T(",") << ChartLineProp.color << _T(",") <<  ChartLineProp.style << _T(",Default,") << ChartLineProp.weight << std::endl;

		//! HoleChartItem을 그룹 별로 Write 한다.
		for(vector<COpeningHoleGroupData*>::iterator jtr = m_oHoleChartItemGroupEntry.begin(); jtr != m_oHoleChartItemGroupEntry.end(); ++jtr)
		{
			(*jtr)->WriteChart(ofile , x , y);
		}

		//! 맨 밑 가로줄을 그린다.
		ofile << _T("<LINE>|");
		ofile << locx << _T(",") << y << _T(",0,");
		ofile << locx + oLabelWidthContainer.GetTotalWidth() << _T(",") << y << _T(",0|");
		ofile << ChartLineProp.level << _T(",") << ChartLineProp.color << _T(",") << ChartLineProp.style << _T(",Default,") << ChartLineProp.weight << std::endl;

		//! 세로줄을 그린다.
		x = locx;
		size_t nCount = oLabelWidthContainer.GetWidthCount();
		for(int i = 0;i < int(nCount);++i)
		{
			ofile << _T("<LINE>|");
			ofile << x << _T(",") << locy << _T(",0,");
			ofile << x << _T(",") << y << _T(",0|");
			ofile << ChartLineProp.level << _T(",") << ChartLineProp.color << _T(",") << ChartLineProp.style << _T(",Default,") << ChartLineProp.weight << std::endl;

			x += oLabelWidthContainer.GetWidth(i);
		}
		ofile << _T("<LINE>|");
		ofile << x << _T(",") << locy << _T(",0,");
		ofile << x << _T(",") << y << _T(",0|");
		ofile << ChartLineProp.level << _T(",") << ChartLineProp.color << _T(",") << ChartLineProp.style << _T(",Default,") << ChartLineProp.weight << std::endl;
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-11
    @function   WriteLabel
    @return     int
    @param      OFSTREAM_T&             ofile
    @param      const                   double&
    @param      x                       const
    @param      double&                 y
    @param      vector<STRING_T>&       oLabelEntry
    @param      CLabelWidthContainer&   oLabelWidthContainer
    @brief
******************************************************************************/
int COpeningHoleChart::WriteLabel(OFSTREAM_T& ofile , const double& x, const double& y , vector<STRING_T>& oLabelEntry , CLabelWidthContainer& oLabelWidthContainer)
{
	COpeningHoleDrawingOption& env = COpeningHoleDrawingOption::GetInstance();

	const int nTextColor = env.GetChartTextColor();
	const int nTextFont = ATOI_T(env.m_oChartTextStyle.m_rFontName.c_str());
	const double nChartRowHeight = env.GetChartRowHeight();
	double TempX = x;
	int nIndex = 0;
	for(vector<STRING_T>::iterator itr = oLabelEntry.begin();itr != oLabelEntry.end();++itr,++nIndex)
	{
		ofile << "<TEXT>|";
		ofile << TempX + oLabelWidthContainer.GetWidth(nIndex) * 0.5 << _T(",") << y << _T(",0,");
		ofile << env.m_oChartTextStyle.m_nHeight << _T(",") << env.m_oChartTextStyle.m_nHeight*env.m_oChartTextStyle.m_nWidthFactor << _T(",0,") << (*itr) << _T(",Center Center,0|");
		ofile << _T("0,") << nTextColor << _T(",") << nTextFont <<  _T(",0") <<std::endl;

		TempX += oLabelWidthContainer.GetWidth(nIndex);
	}

	return ERROR_SUCCESS;
}