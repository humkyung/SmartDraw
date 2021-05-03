#ifndef	__DGNGROUP_H__
#define	__DGNGROUP_H__

#include <SQLib.h>
#include "DGNLib.h"
#include <DGNElement.h>

class CDGNFile;
class DLL_EXPORT CDGNGroup : public CDGNElement{
public:
	CDGNGroup();
	~CDGNGroup();
public:
	long GetElemSize(const int nDimension);
};

#endif
