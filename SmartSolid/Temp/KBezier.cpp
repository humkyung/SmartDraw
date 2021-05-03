#include "KBezier.h"

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KBezier::KBezier(){
	m_crvBezier.nDegree = 0;
	m_crvBezier.nPoints = 0;
	m_crvBezier.ppt     = NULL;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KBezier::KBezier(int nPoints,SQPOINT ptControl[]){
	m_crvBezier.nPoints = nPoints;
	m_crvBezier.nDegree = nPoints - 1;
	if(m_crvBezier.ppt  = (SQPOINT*)calloc(1,sizeof(SQPOINT)*nPoints)){
		memcpy(m_crvBezier.ppt,ptControl,sizeof(SQPOINT)*nPoints);
	}else{
		m_crvBezier.nDegree = 0;
		m_crvBezier.nPoints = 0;
		m_crvBezier.ppt     = NULL;
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KBezier::KBezier(const SQCURVE& bezier){
	m_crvBezier.nPoints = bezier.nPoints;
	m_crvBezier.nDegree = bezier.nPoints - 1;
	if(m_crvBezier.ppt  = (SQPOINT*)calloc(1,sizeof(SQPOINT)*bezier.nPoints)){
		memcpy(m_crvBezier.ppt,bezier.ppt,sizeof(SQPOINT)*bezier.nPoints);
	}else{
		m_crvBezier.nDegree = 0;
		m_crvBezier.nPoints = 0;
		m_crvBezier.ppt     = NULL;
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KBezier::KBezier(const KBezier& bezier){
	m_crvBezier.nPoints = bezier.m_crvBezier.nPoints;
	m_crvBezier.nDegree = bezier.m_crvBezier.nPoints - 1;
	if(m_crvBezier.ppt  = (SQPOINT*)calloc(1,sizeof(SQPOINT)*bezier.m_crvBezier.nPoints)){
		memcpy(m_crvBezier.ppt,bezier.m_crvBezier.ppt,sizeof(SQPOINT)*bezier.m_crvBezier.nPoints);
	}else{
		m_crvBezier.nDegree = 0;
		m_crvBezier.nPoints = 0;
		m_crvBezier.ppt     = NULL;
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KBezier::~KBezier(){
	if(m_crvBezier.ppt) free((void*)m_crvBezier.ppt);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
SQPOINT KBezier::operator()(double t){
	memcpy(KCurve::pt,m_crvBezier.ppt,sizeof(SQPOINT)*m_crvBezier.nPoints);
	for(int i = 1;i < m_crvBezier.nPoints;i++){
		for(int j=0;j <= (m_crvBezier.nPoints - i);j++){
			SQVECTOR vec={0};

			vec.dx = KCurve::pt[j+1].x - KCurve::pt[j].x;
			vec.dy = KCurve::pt[j+1].y - KCurve::pt[j].y;
			vec.dz = KCurve::pt[j+1].z - KCurve::pt[j].z;
			KCurve::pt[j].x = KCurve::pt[j].x + t*vec.dx;
			KCurve::pt[j].y = KCurve::pt[j].y + t*vec.dy;
			KCurve::pt[j].z = KCurve::pt[j].z + t*vec.dz;
			/*
			KCurve::pt[j].x = (1-t)*KCurve::pt[j].x + t*KCurve::pt[j+1].x;
			KCurve::pt[j].y = (1-t)*KCurve::pt[j].y + t*KCurve::pt[j+1].y;
			KCurve::pt[j].z = (1-t)*KCurve::pt[j].z + t*KCurve::pt[j+1].z;
			*/
		}
	}

	return KCurve::pt[0];
}

//	parameter	:
//	description	: elevate degree
//	remarks		:
//	returns		: NONE
void KBezier::ElevateDegree(){
	if(m_crvBezier.ppt = (SQPOINT*)::realloc(m_crvBezier.ppt,sizeof(SQPOINT)*(m_crvBezier.nPoints+1))){
		KCurve::pt[0] = m_crvBezier.ppt[0];
		for(int i=1;i < m_crvBezier.nPoints;i++){
			double factor=(double)i/((double)(m_crvBezier.nDegree + 1));

			KCurve::pt[i].x = factor*m_crvBezier.ppt[i - 1].x + 
					(1-factor)*m_crvBezier.ppt[i].x;
			KCurve::pt[i].y = factor*m_crvBezier.ppt[i - 1].y + 
					(1-factor)*m_crvBezier.ppt[i].y;
			KCurve::pt[i].z = factor*m_crvBezier.ppt[i - 1].z + 
					(1-factor)*m_crvBezier.ppt[i].z;
		}
		KCurve::pt[i] = m_crvBezier.ppt[m_crvBezier.nPoints - 1];
		memcpy(m_crvBezier.ppt,KCurve::pt,sizeof(SQPOINT)*(i+1));
		
		m_crvBezier.nPoints++;
		m_crvBezier.nDegree++;
	}
}

//	parameter	:
//	description	: subdivide the bezier curve at t
//	remarks		:
//	returns		:
KBezier* KBezier::SubDivideAt(const double t){
	assert((t > 0) && (t < 1) && "t is invalid");

	if((t > 0) && (t < 1)){
		KBezier* pNewBezier=new KBezier(*this);
		if(pNewBezier){
			memcpy(&(KCurve::pt),m_crvBezier.ppt,sizeof(SQPOINT)*m_crvBezier.nPoints);
			pNewBezier->m_crvBezier.ppt[0] = m_crvBezier.ppt[m_crvBezier.nPoints - 1];
			for(int i=1;i < m_crvBezier.nPoints;i++){
				for(int j=0;j < m_crvBezier.nPoints - i;j++){
					KCurve::pt[j].x = KCurve::pt[j].x*(1-t) + KCurve::pt[j+1].x*t;
					KCurve::pt[j].y = KCurve::pt[j].y*(1-t) + KCurve::pt[j+1].y*t;
					KCurve::pt[j].z = KCurve::pt[j].z*(1-t) + KCurve::pt[j+1].z*t;
				}
				m_crvBezier.ppt[i] = KCurve::pt[0];
				pNewBezier->m_crvBezier.ppt[i] = KCurve::pt[j-1];
			}
			return pNewBezier;
		}
	}

	return NULL;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KBezier::operator =(const KBezier& bezier){
	m_crvBezier.nDegree = bezier.m_crvBezier.nDegree;
	m_crvBezier.nPoints = bezier.m_crvBezier.nPoints;
	if(m_crvBezier.ppt)   free((void*)m_crvBezier.ppt);
	if(m_crvBezier.ppt = (SQPOINT*)calloc(1,sizeof(SQPOINT)*m_crvBezier.nPoints)){
		memcpy(m_crvBezier.ppt,bezier.m_crvBezier.ppt,sizeof(SQPOINT)*m_crvBezier.nPoints);
	}
}