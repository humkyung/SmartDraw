#include <math.h>
#include "KTriangle.h"

#define TOLERANCE 0.00000001

KTriangle::KTriangle(){
    	m_pt[0].x = 0.;
	m_pt[0].y = 0.;
	m_pt[0].z = 0.;

	m_pt[1].x = 1.;
	m_pt[1].y = 0.;
	m_pt[1].z = 0.;
	
	m_pt[2].x = 1.;
	m_pt[2].y = 1.;
	m_pt[2].z = 0.;
}

KTriangle::KTriangle(SQPOINT& pt1,SQPOINT& pt2,SQPOINT& pt3){
	m_pt[0] = pt1;
	m_pt[1] = pt2;
	m_pt[2] = pt3;
	
	SQPOINT pt[3]={0.};
	pt[0].x = m_pt[0].x;
	pt[0].y = m_pt[0].y;
	pt[0].z = m_pt[0].z;
	pt[1].x = m_pt[1].x;
	pt[1].y = m_pt[1].y;
	pt[1].z = m_pt[1].z;
	pt[2].x = m_pt[2].x;
	pt[2].y = m_pt[2].y;
	pt[2].z = m_pt[2].z;
	//ComputePlaneEquation(pt[0],pt[1],pt[2]);
}

KTriangle::KTriangle(KTriangle& triangle){
	m_pt[0] = triangle.m_pt[0];
	m_pt[1] = triangle.m_pt[1];
	m_pt[2] = triangle.m_pt[2];

	m_vecNorm = triangle.m_vecNorm;
}

KTriangle::~KTriangle(){
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool KTriangle::HasEdge(SQPOINT& pt1,SQPOINT& pt2){
	if(0 != memcmp(&pt1,&pt2,sizeof(SQPOINT))){
		bool bFirst=false;
		if(	((m_pt[0].x == pt1.x) && (m_pt[0].y == pt1.y) && (m_pt[0].z == pt1.z)) || 
			((m_pt[1].x == pt1.x) && (m_pt[1].y == pt1.y) && (m_pt[1].z == pt1.z)) ||
			((m_pt[2].x == pt1.x) && (m_pt[2].y == pt1.y) && (m_pt[2].z == pt1.z))){
			bFirst = true;
		}

		bool bSecond=false;
		if(	((m_pt[0].x == pt2.x) && (m_pt[0].y == pt2.y) && (m_pt[0].z == pt2.z)) || 
			((m_pt[1].x == pt2.x) && (m_pt[1].y == pt2.y) && (m_pt[1].z == pt2.z)) ||
			((m_pt[2].x == pt2.x) && (m_pt[2].y == pt2.y) && (m_pt[2].z == pt2.z))){
			bSecond = true;
		}
		return (bFirst && bSecond);
	}else	return false;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool KTriangle::DoOccurIntersection(SQPOINT& pt1,SQPOINT& pt2){
	double x=0.,y=0.;

	INTERSECTION_T t=IntersectLine2D(&x,&y,
		m_pt[0].x,m_pt[0].y,m_pt[1].x,m_pt[1].y,
		pt1.x,pt1.y,pt2.x,pt2.y,TOLERANCE);
	if(DO_INTERSECTION == t) return true;

	t=IntersectLine2D(&x,&y,
		m_pt[1].x,m_pt[1].y,m_pt[2].x,m_pt[2].y,
		pt1.x,pt1.y,pt2.x,pt2.y,TOLERANCE);
	if(DO_INTERSECTION == t) return true;

	t=IntersectLine2D(&x,&y,
		m_pt[2].x,m_pt[2].y,m_pt[0].x,m_pt[0].y,
		pt1.x,pt1.y,pt2.x,pt2.y,TOLERANCE);
	if(DO_INTERSECTION == t) return true;

	return false;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KTriangle::SetCCW(){
	if(Area(m_pt[0],m_pt[1],m_pt[2]) < 0.){
		SQPOINT pt={0.};

		pt = m_pt[0];
		m_pt[0] = m_pt[1];
		m_pt[1] = pt;
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
double KTriangle::Area(const SQPOINT& pt1,const SQPOINT& pt2,const SQPOINT& pt3){
	double ux=pt2.x - pt1.x;
	double uy=pt2.y - pt1.y;
	double vx=pt3.x - pt1.x;
	double vy=pt3.y - pt1.y;
	
	return (ux*vy - uy*vx)*0.5;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
SQPOINT KTriangle::FindCircumCenter(const SQPOINT& pt1,const SQPOINT& pt2,const SQPOINT& pt3){
	double p1=pt1.x*pt1.x + pt1.y*pt1.y;
	double p2=pt2.x*pt2.x + pt2.y*pt2.y;
	double p3=pt3.x*pt3.x + pt3.y*pt3.y;
	double cx=(p1*(pt2.y-pt3.y) + p2*(pt3.y-pt1.y) + p3*(pt1.y-pt2.y))/
		(2*Area(pt1,pt2,pt3)*2);
	double cy=(p1*(pt3.x-pt2.x) + p2*(pt1.x-pt3.x) + p3*(pt2.x-pt1.x))/
		(2*Area(pt1,pt2,pt3)*2);

	SQPOINT pt={cx,cy,0.};

	return pt;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool KTriangle::IsOnCircle(const SQPOINT& pt1,const SQPOINT& pt2,const SQPOINT& pt3,const SQPOINT& pt){
	bool bRet=false;

	SQPOINT ptCenter=FindCircumCenter(pt1,pt2,pt3);
	double dx=ptCenter.x - pt.x;
	double dy=ptCenter.y - pt.y;
	double dz=ptCenter.z - pt.z;
	double dist1=sqrt(dx*dx + dy*dy + dz*dz);
	dx=ptCenter.x - pt1.x;
	dy=ptCenter.y - pt1.y;
	dz=ptCenter.z - pt1.z;
	double dist2=sqrt(dx*dx + dy*dy + dz*dz);
	if(fabs(dist1 - dist2) < TOLERANCE) bRet = true;

	return bRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool KTriangle::IsInsideCircle(const SQPOINT& pt1,const SQPOINT& pt2,const SQPOINT& pt3,const SQPOINT& pt){
	bool bRet=false;

	SQPOINT ptCenter=FindCircumCenter(pt1,pt2,pt3);
	double dx=ptCenter.x - pt.x;
	double dy=ptCenter.y - pt.y;
	double dz=ptCenter.z - pt.z;
	double dist1=sqrt(dx*dx + dy*dy + dz*dz);
	dx=ptCenter.x - pt1.x;
	dy=ptCenter.y - pt1.y;
	dz=ptCenter.z - pt1.z;
	double dist2=sqrt(dx*dx + dy*dy + dz*dz);
	if((dist1 - dist2) < -TOLERANCE) bRet = true;

	return bRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KTriangle::operator()(SQPOINT& pt1,SQPOINT& pt2,SQPOINT& pt3){
    	m_pt[0] = pt1;
	m_pt[1] = pt2;
	m_pt[2] = pt3;
	
	SQPOINT pt[3]={0.};
	pt[0].x = m_pt[0].x;
	pt[0].y = m_pt[0].y;
	pt[0].z = m_pt[0].z;
	pt[1].x = m_pt[1].x;
	pt[1].y = m_pt[1].y;
	pt[1].z = m_pt[1].z;
	pt[2].x = m_pt[2].x;
	pt[2].y = m_pt[2].y;
	pt[2].z = m_pt[2].z;
	ComputePlaneEquation(pt[0],pt[1],pt[2]);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
SQPOINT& KTriangle::operator[](int nIndex){
    	assert((nIndex >= 0) && "nIndex is less 0.");
	assert((nIndex < 3)  && "nIndex is greather than 3.");
       
	if((nIndex >= 0) && (nIndex < 3)){
	    	return m_pt[nIndex];
	}	    
	
	return m_pt[0];
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool operator==(KTriangle& tri1,KTriangle& tri2){
	bool bRet=false;

	bool bMatchEdge[3]={false};
	SQPOINT vt[2][3]={0};
	vt[0][0] = tri1[0];
	vt[0][1] = tri1[1];
	vt[0][2] = tri1[2];
	vt[1][0] = tri2[0];
	vt[1][1] = tri2[1];
	vt[1][2] = tri2[2];

	if(	((vt[0][0].x == vt[1][0].x) && (vt[0][0].y == vt[1][0].y) && (vt[0][0].z == vt[1][0].z)) || 
		((vt[0][0].x == vt[1][1].x) && (vt[0][0].y == vt[1][1].y) && (vt[0][0].z == vt[1][1].z)) ||
		((vt[0][0].x == vt[1][2].x) && (vt[0][0].y == vt[1][2].y) && (vt[0][0].z == vt[1][2].z))){
			bMatchEdge[0] = true;
	}

	if(	((vt[0][1].x == vt[1][0].x) && (vt[0][1].y == vt[1][0].y) && (vt[0][1].z == vt[1][0].z)) || 
		((vt[0][1].x == vt[1][1].x) && (vt[0][1].y == vt[1][1].y) && (vt[0][1].z == vt[1][1].z)) ||
		((vt[0][1].x == vt[1][2].x) && (vt[0][1].y == vt[1][2].y) && (vt[0][1].z == vt[1][2].z))){
			bMatchEdge[1] = true;
	}

	if(	((vt[0][2].x == vt[1][0].x) && (vt[0][2].y == vt[1][0].y) && (vt[0][2].z == vt[1][0].z)) || 
		((vt[0][2].x == vt[1][1].x) && (vt[0][2].y == vt[1][1].y) && (vt[0][2].z == vt[1][1].z)) ||
		((vt[0][2].x == vt[1][2].x) && (vt[0][2].y == vt[1][2].y) && (vt[0][2].z == vt[1][2].z))){
			bMatchEdge[2] = true;
	}

	bRet = (bMatchEdge[0] && bMatchEdge[1] && bMatchEdge[2]);

	return bRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
int KTriangle::FindIndexOfSmallestAngle(){
	double e[3]={0};

	for(int i=0;i < 3;i++)
		e[i] = GetLength(m_pt[(i + 1)%3],m_pt[(i + 2)%3]);
	for(i = 0;i < 3;i++){
		if((e[i] <= e[(i + 1)%3]) && (e[i] <= e[(i + 2)%3])) break;
	}

	return i;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
double KTriangle::Angle(const SQPOINT& pt1,const SQPOINT& pt2,const SQPOINT& pt3){
	double ux=pt2.x - pt1.x;
	double uy=pt2.y - pt1.y;
	double vx=pt3.x - pt1.x;
	double vy=pt3.y - pt1.y;

	double dot=DotProduct(ux,uy,0.,vx,vy,0.);
	return acos(sqrt((dot*dot)/((ux*ux + uy*uy)*(vx*vx + vy*vy))))*(180./PI);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool KTriangle::HasBadAngle(){
	bool bRet=false;

	int nIndex=FindIndexOfSmallestAngle();
	double angle=Angle(m_pt[nIndex],m_pt[(nIndex + 1)%3],m_pt[(nIndex + 2)%3]);
	if(angle < 10.){
		bRet = true;
	}

	return bRet;
}