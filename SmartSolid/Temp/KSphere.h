#ifndef __KSPHERE_H__
#define __KSPHERE_H__

#include <array.h>
#include "KBody.h"

class KSphere : public KBody{
public:
	KSphere();
	KSphere(SQPOINT ptOrigin,double nRadius);
	~KSphere();
public:
	static void GetSurfaceBoundaryPoint(int& nArray,KArray<SQPOINT>* pArray);
protected:
	void OnCreate();
private:
	SQPOINT m_ptOrigin;
	double  m_nRadius;
};

#endif
