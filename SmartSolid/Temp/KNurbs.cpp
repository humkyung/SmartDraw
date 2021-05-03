#include "KNURBS.h"

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KNURBS::KNURBS(){
	nDegree = 0;
	nPoints = 0;
	pKnots	= NULL;
	ppt	= NULL;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KNURBS::KNURBS(const NURBS& nurbs){
	nDegree = nurbs.nDegree;
	nPoints = nurbs.nPoints;
	if(pKnots = (double*)calloc(1,sizeof(double)*(nPoints + nDegree + 1))){
		memcpy(pKnots,nurbs.pKnots,sizeof(double)*(nPoints + nDegree + 1));
	}
	if(ppt = (WPOINT*)calloc(1,sizeof(WPOINT)*nPoints)){
		memcpy(ppt,nurbs.ppt,sizeof(WPOINT)*nPoints);
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KNURBS::KNURBS(const KNURBS& nurbs){
	nDegree = nurbs.nDegree;
	nPoints = nurbs.nPoints;
	if(pKnots = (double*)calloc(1,sizeof(double)*(nPoints + nDegree + 1))){
		memcpy(pKnots,nurbs.pKnots,sizeof(double)*(nPoints + nDegree + 1));
	}
	if(ppt = (WPOINT*)calloc(1,sizeof(WPOINT)*nPoints)){
		memcpy(ppt,nurbs.ppt,sizeof(WPOINT)*nPoints);
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KNURBS::~KNURBS(){
	if(pKnots) free(pKnots);
	if(ppt)	   free(ppt);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
int KNURBS::GetNumOfCtrlPoints(){
	return nPoints;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
int KNURBS::GetDegree(){
	return nDegree;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
NURBS::WPOINT* KNURBS::GetCtrlPoints(){
	return ppt;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
double* KNURBS::GetKnots(){
	return pKnots;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KNURBS::SetWeights(const double* pWeights){
	assert(pWeights && "pWeights is NULL");

	if(pWeights){
		for(unsigned int i=0;i < nPoints;i++){
			ppt[i].weight = pWeights[i];
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KNURBS::InsertKnot(const double u){
	int span=FindKnotSpan(u);

	if(	(ppt  = (NURBS::WPOINT*)realloc(ppt,sizeof(NURBS::WPOINT)*(nPoints + 1))) &&
		(pKnots= (double*)realloc(pKnots,sizeof(double)*(nPoints + nDegree + 2)))){
		memmove(&ppt[span + 1],&ppt[span],sizeof(NURBS::WPOINT)*(nPoints - span));
		
		for(unsigned int i=span;i >= span - nDegree + 1;i--){
			double a=(u - pKnots[i])/(pKnots[i + nDegree] - pKnots[i]);
			ppt[i].pt.x= (1 - a)*ppt[i - 1].pt.x + a*ppt[i].pt.x;
			ppt[i].pt.y= (1 - a)*ppt[i - 1].pt.y + a*ppt[i].pt.y;
			ppt[i].pt.z= (1 - a)*ppt[i - 1].pt.z + a*ppt[i].pt.z;
			ppt[i].weight=(1 - a)*ppt[i - 1].weight + a*ppt[i].weight;
		}
		memmove(&pKnots[span + 2],&pKnots[span + 1],sizeof(double)*(nPoints + nDegree - span));
		pKnots[span + 1] = u;

		nPoints++;
	}
}	

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
SQPOINT KNURBS::operator()(double u){
	int span=FindKnotSpan(u);
	int multiplicity=GetKnotMultiplicity(span,u);
	int times=nDegree - multiplicity;
	
	static double weights[256]={0};
	// copy
	for(int i=span - nDegree;i <= span;i++){
		weights[i - (span - nDegree)]      = ppt[i].weight;
		KCurve::pt[i - (span - nDegree)].x = (ppt[i].weight)*(ppt[i].pt.x);
		KCurve::pt[i - (span - nDegree)].y = (ppt[i].weight)*(ppt[i].pt.y);
		KCurve::pt[i - (span - nDegree)].z = (ppt[i].weight)*(ppt[i].pt.z);
	}

	for(int r=1;r <= times;r++){
		for(int i=span - nDegree + r;i <= span - multiplicity;i++){
			double a=(u-pKnots[i])/(pKnots[i + nDegree - r + 1] - pKnots[i]);
			KCurve::pt[i - (span - nDegree + r)].x = (1-a)*KCurve::pt[i - (span - nDegree + r)].x + a*KCurve::pt[i + 1 - (span - nDegree + r)].x;
			KCurve::pt[i - (span - nDegree + r)].y = (1-a)*KCurve::pt[i - (span - nDegree + r)].y + a*KCurve::pt[i + 1 - (span - nDegree + r)].y;
			KCurve::pt[i - (span - nDegree + r)].z = (1-a)*KCurve::pt[i - (span - nDegree + r)].z + a*KCurve::pt[i + 1 - (span - nDegree + r)].z;
			weights[i - (span - nDegree + r)]      = (1-a)*weights[i - (span - nDegree + r)] + a*weights[i + 1 - (span - nDegree + r)];
		}
	}	
	KCurve::pt[0].x/=weights[0];
	KCurve::pt[0].y/=weights[0];
	KCurve::pt[0].z/=weights[0];

	return KCurve::pt[0];
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
int KNURBS::FindKnotSpan(double u)
{
	int m=nDegree+nPoints+1;
	for(int i=nDegree;i < m;i++){
		double du[2]={0};

		du[0] = pKnots[i]-u;
		du[1] = pKnots[i+1]-u;
		if(du[0]*du[1] <= 0) return i;
	}

	return m;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
int KNURBS::GetKnotMultiplicity(int span, double u)
{
	int multiplicity=0;
	if(pKnots[span] == u){
		for(int i=span;(i >= 0) && (u == pKnots[i]);i--,multiplicity++);
	}

	return multiplicity;
}
