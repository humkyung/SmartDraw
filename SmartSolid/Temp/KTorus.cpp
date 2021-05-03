#include "KSurface.h"
#include "KCircle.h"
#include "KTorus.h"

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KTorus::KTorus(){
    	m_ptOrigin.x = 0.;
	m_ptOrigin.y = 0.;
	m_ptOrigin.z = 0.;

	m_vecNorm.dx = 0.;
	m_vecNorm.dy = 0.;
	m_vecNorm.dz = 1.;

	m_nTorusRadius = 100.;
	m_nTubeRadius  = 10.;

	OnCreate();
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KTorus::KTorus(SQPOINT ptOrigin,SQVECTOR vecNorm,double nTorusRadius,double nTubeRadius){
	m_ptOrigin.x = ptOrigin.x;
	m_ptOrigin.y = ptOrigin.y;
	m_ptOrigin.z = ptOrigin.z;

	m_vecNorm.dx = vecNorm.dx;
	m_vecNorm.dy = vecNorm.dy;
	m_vecNorm.dz = vecNorm.dz;

	m_nTorusRadius = nTorusRadius;
	m_nTubeRadius  = nTubeRadius;

	OnCreate();
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KTorus::~KTorus(){
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KTorus::OnCreate(){
    	KSurface* pSurface=NULL;

	SQCIRCLE circle={0};
	circle.ptOrigin.x = m_ptOrigin.x + m_nTorusRadius;
	circle.ptOrigin.y = m_ptOrigin.y;
	circle.ptOrigin.z = m_ptOrigin.z;
	circle.vecNorm.dx = 0;
	circle.vecNorm.dy = 1;
	circle.vecNorm.dz = 0;
	circle.nRadius    = m_nTubeRadius;

	KCircle* pCircle = new KCircle(circle);
	if(pCircle){
		PSQPOINT pptPoints = pCircle->Sweep(m_vecNorm,2*PI,32);
		if(pSurface = new KSurface(32,pCircle->GetNumOfPoints(),pptPoints,this)){
	    		(*this) += pSurface;
		}
		delete pCircle;
	}
}
