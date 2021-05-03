#ifndef __KTORUS_H__
#define __KTORUS_H__

#include "KBody.h"

class KTorus : public KBody{
public:
    	KTorus();
	KTorus(SQPOINT ptOrigin,SQVECTOR vecNorm,double nTorusRadius,double nTubeRadius);
	~KTorus();
protected:
	void OnCreate();
private:
	SQPOINT  m_ptOrigin;
	SQVECTOR m_vecNorm;
	double   m_nTorusRadius;
	double   m_nTubeRadius;
};

#endif
