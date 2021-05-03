#ifndef __KCURVE_H__
#define __KCURVE_H__

#include "KPlane.h"
#include "KArray.h"

class KCurve : public KPlane{
public:
	KCurve();
	virtual ~KCurve();
public:
	virtual KArray<SQPOINT>* GetFacets();
public:
	int m_nFacets;
};

#endif
