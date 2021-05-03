#include <math.h>
#include <math/vect.h>
#include <graphics/line.h>
#include <graphics/enum.h>

/**	\brief	The Length function

	\param	line	a parameter of type const LINE_T&

	\return	double	
*/
double Length(const LINE_T& line){
	double nRet=0.;

	double dx=line.ptEnd.x - line.ptStart.x;
	double dy=line.ptEnd.y - line.ptStart.y;
	double dz=line.ptEnd.z - line.ptStart.z;
	nRet = sqrt(dx*dx + dy*dy + dz*dz);

	return nRet;
}

/**	\brief	The IntersectLine2D function\n
	check line is intersect. if so get cross point.\n
	line is colinear.

	\param	x	a parameter of type double*
	\param	y	a parameter of type double*
	\param	x1	a parameter of type double
	\param	y1	a parameter of type double
	\param	x2	a parameter of type double
	\param	y2	a parameter of type double
	\param	x3	a parameter of type double
	\param	y3	a parameter of type double
	\param	x4	a parameter of type double
	\param	y4	a parameter of type double
	\param	tol	a parameter of type double

	\return	INTERSECTION_E	
*/
INTERSECTION_E IntersectLine2D(double* x,double* y,double x1,double y1,double x2,double y2,double x3,double y3,double x4,double y4,double tol){
	double Ax=0.,Bx=0.,Cx=0.,Ay=0.,By=0.,Cy=0.,d=0.,e=0.,f=0.;
	double x1lo=0.,x1hi=0.,y1lo=0.,y1hi=0.;

	Ax = x2 - x1;
	Bx = x3 - x4;
	/// X bound box test
	if(Ax < 0){
		x1lo=x2; x1hi=x1;
	}else{
		x1hi=x2; x1lo=x1;
	}
	if(Bx > 0){
		if(x1hi < x4 || x3 < x1lo) return DONT_INTERSECTION;
	} else {
		if(x1hi < x3 || x4 < x1lo) return DONT_INTERSECTION;
	}

	Ay = y2 - y1;
	By = y3 - y4;
	/// Y bound box test
	if(Ay < 0) {
		y1lo=y2; y1hi=y1;
	} else {
		y1hi=y2; y1lo=y1;
	}

	if(By > 0) {
		if(y1hi < y4 || y3 < y1lo) return DONT_INTERSECTION;
	} else {
		if(y1hi < y3 || y4 < y1lo) return DONT_INTERSECTION;
	}

	Cx = x1-x3;
	Cy = y1-y3;
	d = By*Cx - Bx*Cy;	/// alpha numerator
	f = Ay*Bx - Ax*By;	/// both denominator
	
	if(f > 0.) {		/// alpha tests
		if((d < (0 + tol)) || (d > (f - tol))) return DONT_INTERSECTION;
	} else {
		if((d > (0 - tol)) || (d < (f + tol))) return DONT_INTERSECTION;
	}

	e = Ax*Cy - Ay*Cx;	/// beta numerator
	if(f > 0.) {		/// beta tests
		if((e < (0 + tol)) || (e > (f - tol))) return DONT_INTERSECTION;
	} else {
		if((e > (0 - tol)) || (e < (f + tol))) return DONT_INTERSECTION;
	}

	/// lines are colinear.
	if(f == 0.) return COLLINEAR_LINE;

	///compute intersection coordinates
	double alpha = d/f;
	
	if(x1 == x2)
		*x = x1;
	else
		*x = x1 + alpha*Ax;
	if(y1 == y2)
		*y = y1;
	else
		*y = y1 + alpha*Ay;

	return DO_INTERSECTION;
}

/**	\brief	The IntersectLine2D function

	\param	pt	a parameter of type POINT_T&
	\param	line1	a parameter of type const LINE_T&
	\param	line2	a parameter of type const LINE_T&

	\return	INTERSECTION_E	
*/
INTERSECTION_E IntersectLine2D(POINT_T& pt,const LINE_T& line1,const LINE_T& line2){
	double x[4]={0.},y[4]={0.};
	
	x[0] = line1.ptStart.x;
	y[0] = line1.ptStart.y;
	x[1] = line1.ptEnd.x;
	y[1] = line1.ptEnd.y;
	x[2] = line2.ptStart.x;
	y[2] = line2.ptStart.y;
	x[3] = line2.ptEnd.x;
	y[3] = line2.ptEnd.y;
	
	INTERSECTION_E ret = IntersectLine2D(&(pt.x),&(pt.y),x[0],y[0],x[1],y[1],x[2],y[2],x[3],y[3]);
	
	return ret;
}

/**	\brief	The IntersectLine3D function

	\param	pt	a parameter of type POINT_T&
	\param	line1	a parameter of type const LINE_T&
	\param	line2	a parameter of type const LINE_T&

	\return	INTERSECTION_E	
*/
INTERSECTION_E IntersectLine3D(POINT_T& pt,const LINE_T& line1,const LINE_T& line2){
	VECTOR_T vec1={line1.ptEnd.x - line1.ptStart.x,line1.ptEnd.y - line1.ptStart.y,line1.ptEnd.z - line1.ptStart.z};
	VECTOR_T vec2={line2.ptEnd.x - line2.ptStart.x,line2.ptEnd.y - line2.ptStart.y,line2.ptEnd.z - line2.ptStart.z};
	VECTOR_T vec ={line2.ptStart.x - line1.ptStart.x,line2.ptStart.y - line1.ptStart.y,line2.ptStart.z - line1.ptStart.z};

	double nLen=Length(CrossProduct(vec1,vec2));
	if(fabs(nLen) < 0.001) return COLLINEAR_LINE;
	double t=DotProduct(CrossProduct(vec,vec2),CrossProduct(vec1,vec2))/(nLen*nLen);

	pt.x = line1.ptStart.x + vec1.dx*t;
	pt.y = line1.ptStart.y + vec1.dy*t;
	pt.z = line1.ptStart.z + vec1.dz*t;

	return DO_INTERSECTION;
}
