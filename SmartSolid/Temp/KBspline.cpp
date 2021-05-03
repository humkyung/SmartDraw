// KBspline.cpp: implementation of the KBspline class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KBspline.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KBspline::KBspline()
{
	nDegree = 0;
	nPoints = 0;
	m_pN    = NULL;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KBspline::KBspline(const int degree,const int points,SQPOINT* ptCtrl){
	nDegree  = degree;
	nPoints  = points;
	
	if(knots= (double*)calloc(1,sizeof(double)*(nDegree + nPoints + 1))){
		double divide=nPoints - nDegree;
		for(int i=nDegree+1;i < nPoints;i++){
			knots[i] = (i - nDegree)/divide;
		}
		for(i = nPoints;i <= nPoints + nDegree+1;i++){
			knots[i] = 1.;
		}
	}
	
	if(ppt = (SQPOINT*)calloc(1,sizeof(SQPOINT)*nPoints)){
		memcpy(ppt,ptCtrl,sizeof(SQPOINT)*nPoints);
	}
	m_pN = (double*)calloc(1,sizeof(double)*(nPoints+1));
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KBspline::KBspline(const SQBSPLINE& bspline){
	nDegree  = bspline.nDegree;
	nPoints  = bspline.nPoints;
	if(knots = (double*)calloc(1,sizeof(double)*(nDegree+nPoints+1))){
		memcpy(knots,bspline.knots,sizeof(double)*(nDegree+nPoints+1));
	}
	if(ppt = (SQPOINT*)calloc(1,sizeof(SQPOINT)*nPoints)){
		memcpy(ppt,bspline.ppt,sizeof(SQPOINT)*nPoints);
	}
	m_pN = (double*)calloc(1,sizeof(double)*(nPoints+1));
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KBspline::KBspline(const KBspline& bspline){
	nDegree  = bspline.nDegree;
	nPoints  = bspline.nPoints;
	if(knots = (double*)calloc(1,sizeof(double)*(nDegree+nPoints+1))){
		memcpy(knots,bspline.knots,sizeof(double)*(nDegree+nPoints+1));
	}
	if(ppt = (SQPOINT*)calloc(1,sizeof(SQPOINT)*nPoints)){
		memcpy(ppt,bspline.ppt,sizeof(SQPOINT)*nPoints);
	}
	m_pN = (double*)calloc(1,sizeof(double)*(nPoints+1));
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KBspline::~KBspline()
{
	if(m_pN) free((void*)m_pN);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
int KBspline::FindKnotSpan(double u){
	int m=nDegree+nPoints+1;

	for(int i=nDegree;i < m;i++){
		double du[2]={0};

		du[0] = knots[i]-u;
		du[1] = knots[i+1]-u;
		if(du[0]*du[1] <= 0) return i;
	}

	return m;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KBspline::ComputateBasisFuncs(int index,double u){
	assert(m_pN && "m_pN is NULL");

	if(m_pN){
		int m=nDegree+nPoints+1;

		memset(m_pN,0,sizeof(double)*(nPoints+1));

		if(u == knots[0]){
			m_pN[0]=1.;
			return;
		}else if(u == knots[m]){
			m_pN[m]=1.0;
			return;
		}

		m_pN[index]=1.0;
		for(int i=1;i <= nDegree;i++){
			double factor=m_pN[index - i + 1]/(knots[index+1]-knots[(index-i)+1]);
			m_pN[index - i]=factor*(knots[index+1] - u);
			for(int j=index - i + 1;j <= index - 1;j++){
				double newfactor=m_pN[j+1]/(knots[j+i+1]-knots[j+1]);
				m_pN[j]=factor*(u-knots[j]) + newfactor*(knots[j+i+1]-u);
				factor = newfactor;
			}
			m_pN[index]=factor*(u-knots[index]);
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
SQPOINT KBspline::operator()(double u){
	SQPOINT ret={0};

	int index=FindKnotSpan(u);
	ComputateBasisFuncs(index,u);
	for(int i=index-nDegree;i<=index;i++){
		ret.x = ret.x + m_pN[i]*ppt[i].x;
		ret.y = ret.y + m_pN[i]*ppt[i].y;
		ret.z = ret.z + m_pN[i]*ppt[i].z;
	}

	return ret;
}