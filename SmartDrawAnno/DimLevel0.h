#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DimLevel.h"

class CExtDivision;
class CDimLevel0 : public CDimLevel  
{
public:
	CDimLevel0(CExtDivision* pParent);
	virtual ~CDimLevel0();
public:
	void Run(CExtDivision* pDivision);
private:
	CExtDivision* m_pParent;
};
