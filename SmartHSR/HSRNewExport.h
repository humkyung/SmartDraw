/********************************************************************
	created:	2002/01/01
	filename: 	HSRNewExport.h
	file base:	HSRNewExport
	file ext:	h
	author:		
	
	purpose	:
	update	:	[2002.12.17] - add IsSingleLine(const double nRadius)
*********************************************************************/

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQLib.h>
#include "HSRExport.h"

#include <list>
using namespace std;

class CHSRNewExport : public CHSRExport
{
public:
	CHSRNewExport();
	virtual ~CHSRNewExport();
public:
	bool WritePrimitiveAsAsciiFile(const char* pFileName);
};
