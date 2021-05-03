#pragma once

#include <AnnoTerritory.h>

class CCableTrayAnnoTerritory : public CAnnoTerritory
{
public:
	CCableTrayAnnoTerritory(CaDraw_LogicView* pLogicView);
	~CCableTrayAnnoTerritory(void);

	int Annotate(list<CIsLine2d>& oHsrLineList);
	int Write(OFSTREAM_T& ofile,const double& dDrawingScale);
};
