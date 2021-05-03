#pragma once

#include <SmartDrawAnnoPlatform.h>
#include <InternalTerritory.h>

#include "OpeningHoleGroupData.h"

#include <list>
using namespace std;

class COpeningHoleInternalTerritory : public CInternalTerritory
{
public:
	COpeningHoleInternalTerritory(/*CAnnoTerritory* pTerritory*/);
	~COpeningHoleInternalTerritory(void);

	int Annotate(list<CIsLine2d>& oHsrLineList);
	void Write(OFSTREAM_T& ofile);

	list<COpeningHoleGroupData*> m_oOpeningHoleGroupDataList;
private:
	int CreateStruGridLine(CCellMap* pCellMap);
	int CreateHoleData();

	list<CIsLine2d> m_oStruGridLineList;
	list<CSmartDrawAnnoPlatform*> m_oPlatformList;
};
