#pragma once

#include <AnnoTerritory.h>

class COpeningHoleAnnoTerritory : public CAnnoTerritory
{
public:
	COpeningHoleAnnoTerritory(CaDraw_LogicView* pLogicView , CInternalTerritory* pIntTerritory = NULL , CExternalTerritory* pExtTerritory = NULL);
	~COpeningHoleAnnoTerritory(void);

	int Annotate(list<CIsLine2d>& oHsrLineList);
	int Write(OFSTREAM_T& ofile);
};
