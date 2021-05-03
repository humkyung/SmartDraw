#include "KArc.h"
#include "KSurface.h"
#include "KFace.h"
#include "KSphere.h"

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KSphere::KSphere(){
	m_ptOrigin.x = 0.;
	m_ptOrigin.y = 0.;
	m_ptOrigin.z = 0.;

	m_nRadius    = 10;

	OnCreate();
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KSphere::KSphere(SQPOINT ptOrigin,double nRadius){
    	m_ptOrigin.x = ptOrigin.x;
	m_ptOrigin.y = ptOrigin.y;
	m_ptOrigin.z = ptOrigin.z;

	m_nRadius    = nRadius;

	OnCreate();
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KSphere::~KSphere(){
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KSphere::OnCreate(){
	KSurface* pSurface=NULL;
	
	SQPOINT ptStart ={m_ptOrigin.x,m_ptOrigin.y,m_ptOrigin.z - m_nRadius};
	SQARC   arc={0};
	arc.ptOrigin  = m_ptOrigin;
	arc.ptStart   = ptStart;
	arc.vecNorm.dx= 0;
	arc.vecNorm.dy= 1;
	arc.vecNorm.dz= 0;
	arc.nSweep    = 180.;
	if(KArc* pArc = new KArc(arc)){		
		SQVECTOR vecAxis={0,0,1};
		PSQPOINT pptPoints = pArc->Revolve(vecAxis,2*PI,16);
		if(pSurface = new KSurface(16,pArc->GetNumOfPoints(),pptPoints,this)){
			(*this) += pSurface;
		}
		
		delete pArc;
	}
}
