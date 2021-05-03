#pragma once

#include <SmartDrawAnnoPlatform.h>
#include <InternalTerritory.h>

#include <list>
using namespace std;

class CPipingInternalTerritory : public CInternalTerritory
{
public:
	CPipingInternalTerritory(/*CAnnoTerritory* pTerritory*/);
	~CPipingInternalTerritory(void);

	int Annotate(list<CIsLine2d>& oHsrLineList);
	void Write(OFSTREAM_T& ofile,const double& dDrawingScale);

private:
};
