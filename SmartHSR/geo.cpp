#include "stdafx.h"
#include <math.h>
#include "Geo.h"

KGeometry1::KGeometry1(){
}

KGeometry1::~KGeometry1(){
}

bool KGeometry1::IsPointOnLine(double px,double py,double qx,double qy,double x,double y){
	//<-- 2001.10.30
	double tx[2],ty[2],len[3],dot;
	double cosval;
	bool   ret = false;

	if(px < qx){
		tx[0] = px;
		tx[1] = qx;
	}else{
		tx[0] = qx;
		tx[1] = px;
	}
	
	if(py < qy){
		ty[0] = py;
		ty[1] = qy;
	}else{
		ty[0] = qy;
		ty[1] = py;
	}
	if((x < tx[0]) || (x > tx[1]) || (y < ty[0]) || (y > ty[1])) return ret;

	len[0] = sqrt((px - x)*(px - x) + (py - y)*(py - y));
	len[1] = sqrt((qx - x)*(qx - x) + (qy - y)*(qy - y));
	dot    = (px - x)*(qx - x) + (py - y)*(qy - y);
	if((0. != len[0]) && (0. != len[1])){
		cosval = (dot)/(len[0]*len[1]);
		if(cosval <= (-1 + 0.01)) ret = true;
	}

	return ret;
	//<--
/*
	if(fabs((qy - py)*(x-px) - (y - py)*(qx-px)) >= max(fabs(qx-px),fabs(qy-py))) return false;

	if(((qx < px) && (px < x)) || ((qy < py) && (py < y))) return false;
	if(((x < px) && (px < qx)) || ((y < py) && (py < qy))) return false;
	if(((px < qx) && (qx < x)) || ((py < qy) && (qy < y))) return false;
	if(((x < qx) && (qx < px)) || ((y < qy) && (qy < py))) return false;

	return true;
*/
}

//	parameter	:
//	description	: check line is intersect. if so get cross point.
//			: line is colinear. 
//	remarks		:
//	returns		:
int KGeometry1::IntersectLine(double x1,double y1,double x2,double y2,double x3,double y3,double x4,double y4,double* x,double* y,double tol){
	double Ax,Bx,Cx,Ay,By,Cy,d,e,f;
	double x1lo,x1hi,y1lo,y1hi;

	//tol = 0.000000001;

	Ax = x2 - x1;
	Bx = x3 - x4;
	// X bound box test
	if(Ax < 0){
		x1lo=x2; x1hi=x1;
	} else {
		x1hi=x2; x1lo=x1;
	}
	if(Bx > 0){
		if(x1hi < x4 || x3 < x1lo) return DONT_INTERSECT;
	} else {
		if(x1hi < x3 || x4 < x1lo) return DONT_INTERSECT;
	}

	Ay = y2 - y1;
	By = y3 - y4;
	/* Y bound box test*/
	if(Ay < 0) {
		y1lo=y2; y1hi=y1;
	} else {
		y1hi=y2; y1lo=y1;
	}

	if(By > 0) {
		if(y1hi < y4 || y3 < y1lo) return DONT_INTERSECT;
	} else {
		if(y1hi < y3 || y4 < y1lo) return DONT_INTERSECT;
	}

	Cx = x1-x3;
	Cy = y1-y3;
	d = By*Cx - Bx*Cy;					/* alpha numerator*/
	f = Ay*Bx - Ax*By;					/* both denominator*/
	
	// lines are colinear.
	if(f == 0.) return COLLINEAR;

	if(f > 0.) {						/* alpha tests*/
		if((d < (0 + tol)) || (d > (f - tol))) return DONT_INTERSECT;
	} else {
		if((d > (0 - tol)) || (d < (f + tol))) return DONT_INTERSECT;
	}

	e = Ax*Cy - Ay*Cx;					/* beta numerator*/
	if(f > 0.) {						/* beta tests*/
		if((e < (0 + tol)) || (e > (f - tol))) return DONT_INTERSECT;
	} else {
		if((e > (0 - tol)) || (e < (f + tol))) return DONT_INTERSECT;
	}

	/*compute intersection coordinates*/
	double alpha = d/f;
	
	if(x1 == x2)
		*x = x1;
	else	*x = x1 + alpha*Ax;
	if(y1 == y2)
		*y = y1;
	else	*y = y1 + alpha*Ay;

	/*
	/// check bounds
	Ax = x2 - x1;
	if(Ax < 0){
		x1lo=x2; x1hi=x1;
	}else{	x1hi=x2; x1lo=x1;}
	if(*x < x1lo) *x = x1lo;
	if(*x > x1hi) *x = x1hi;

	Ay = y2 - y1;
	if(Ay < 0){
		y1lo=y2; y1hi=y1;
	}else{	y1hi=y2; y1lo=y1;}
	if(*y < y1lo) *y = y1lo;
	if(*y > y1hi) *y = y1hi;

	Ax = x4 - x3;
	if(Ax < 0){
		x1lo=x4; x1hi=x3;
	}else{	x1hi=x4; x1lo=x3;}
	if(*x < x1lo) *x = x1lo;
	if(*x > x1hi) *x = x1hi;

	Ay = y4 - y3;
	if(Ay < 0){
		y1lo=y4; y1hi=y3;
	}else{	y1hi=y4; y1lo=y3;}
	if(*y < y1lo) *y = y1lo;
	if(*y > y1hi) *y = y1hi;
	*/

	return DO_INTERSECT;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
double KGeometry1::DotProduct(double vtx1,double vty1,double vtz1,double vtx2,double vty2,double vtz2){
	return (vtx1*vtx2 + vty1*vty2 + vtz1*vtz2);
}