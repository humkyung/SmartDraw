#pragma once

#include <SmartDrawAnnoPlatform.h>
#include <InternalTerritory.h>

#include <list>
using namespace std;

class CCableTrayInternalTerritory : public CInternalTerritory
{
public:
	CCableTrayInternalTerritory(CAnnoTerritory* pTerritory);
	~CCableTrayInternalTerritory(void);

	int Annotate(list<CIsLine2d>& oHsrLineList);
	void Write(OFSTREAM_T& ofile);

private:
};
