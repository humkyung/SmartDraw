#pragma once

#include <IsTools.h>
#include "OpeningHoleGroupData.h"
#include "OpeningHoleDrawingOption.h"

class COpeningHoleChart
{
public:
	COpeningHoleChart(void);
	~COpeningHoleChart(void);

	int Write(OFSTREAM_T& ofile);
public:
	vector<COpeningHoleGroupData*> m_oHoleChartItemGroupEntry;
private:
	int WriteLabel(OFSTREAM_T& ofile , const double& x, const double& y , vector<STRING_T>& oLabelEntry , CLabelWidthContainer& oLabelWidthContainer);
};
