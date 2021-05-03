#pragma once

#include <SmartDrawAnnoPlatform.h>
#include <InternalTerritory.h>

#include <list>
using namespace std;

class CCableTrayInternalTerritory : public CInternalTerritory
{
public:
	CCableTrayInternalTerritory();
	~CCableTrayInternalTerritory(void);

	int Annotate(list<CIsLine2d>& oHsrLineList);
	void Write(OFSTREAM_T& ofile);

public:
	static CCellMap* m_pCellMapExp;
};
