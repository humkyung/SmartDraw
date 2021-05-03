#include <assert.h>
#include <math.h>
#include <memory.h>
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include <graphics/rect.h>

using namespace itk;

const VECTOR_T CGeometry::AXIS={0,0,1};
const int CGeometry::NUM_OF_DIGITS = 13;

/**	\brief
*/
CGeometry::CGeometry(){
}

/**	\brief
*/
CGeometry::~CGeometry(){
}

/**	\brief	The CGeometry::IsSameValue function

	\param	val1	a parameter of type double
	\param	val2	a parameter of type double
	\param	tol	a parameter of type double

	\return	bool	
*/
bool CGeometry::IsSameValue(double val1,double val2,double tol){
	assert((tol > 0) && "tol is less than 0");

	if(tol > 0){
		if(fabs(val2 - val1) < tol) return true;
	}
	
	return false;
}

/**	\brief	The CGeometry::IsSamePoint function\n
	check whether pt1 and pt1 are same points or not.

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&
	\param	tol	a parameter of type double

	\remarks	check two points locate in tolerance.

	\return	true if two points are same points otherwise return flase.
*/
bool CGeometry::IsSamePoint(const POINT_T& pt1,const POINT_T& pt2,double tol){
	assert((tol > 0) && "tol is less than 0");

	if(tol){
		double dx=pt2.x - pt1.x;
		double dy=pt2.y - pt1.y;
		double dz=pt2.z - pt1.z;
		double l =sqrt(dx*dx + dy*dy + dz*dz);

		if(l < tol) return true;
	}
	return false;
}

/**	\brief	The CGeometry::GetRotatedAngleInXYPlane function\n
	calculate angle in 2D

	\param	pt	a parameter of type const POINT_T&

	\return	angle in radian.
*/
double CGeometry::GetRotatedAngleInXYPlane(const POINT_T& pt){
    	double l = sqrt(pt.x*pt.x + pt.y*pt.y);
	double theta=0.;

	if(l > 0){
	    	theta=acos(pt.x/l);

		if((pt.x > 0.) && (0. == pt.y)){
			theta = 0.;
		}else if((pt.x >= 0.) && (pt.y <= 0.)){
		    	theta = 2*PI - theta;
		}else if((pt.x <= 0.) && (pt.y <= 0.)){
		    	theta = 2*PI - theta;
		}
	}

	return theta;
}

/**	\brief	The CGeometry::GetRotatedAngleInXYPlane function\n
	calculate angle in 2D

	\param	vec	a parameter of type const VECTOR_T&

	\return	angle in radian.
*/
double CGeometry::GetRotatedAngleInXYPlane(const VECTOR_T& vec){
    	double l = sqrt(vec.dx*vec.dx + vec.dy*vec.dy);
	double theta=0.;

	if(l > 0){
	    	theta=acos(vec.dx/l);

		if((vec.dx >= 0.) && (vec.dy <= 0.)){
		    	theta = 2*PI - theta;
		}else if((vec.dx <= 0.) && (vec.dy <= 0.)){
		    	theta = 2*PI - theta;
		}
	}

	return theta;
}

/**	\brief	The CGeometry::GetRotatedAngleOfAxis function\n
	get rotated angle of x-axis,y-axis about z-axis(<0,0,1>).

	\param	vecAxis	a parameter of type const VECTOR_T&
	\param	alpha	a parameter of type double&
	\param	beta	a parameter of type double&

	\return	rotated angles(x,y) in radian.
*/
void CGeometry::GetRotatedAngleOfAxis(const VECTOR_T& vecAxis,double& alpha,double& beta)
{
	#define G_TOLERANCE 0.000001
	VECTOR_T axis={0};
       	
	axis.dx = vecAxis.dx;
	axis.dy = vecAxis.dy;
	axis.dz = vecAxis.dz;
	if(fabs(axis.dx) < G_TOLERANCE) axis.dx = 0.;
	if(fabs(axis.dy) < G_TOLERANCE) axis.dy = 0.;
	if(fabs(axis.dz) < G_TOLERANCE) axis.dz = 0.;
	if(false == Normalize(axis)) return;

	const double d=sqrt(axis.dy*axis.dy + axis.dz*axis.dz);
	if((axis.dy != 0.) || (axis.dz != 0.))
	{
		if((1. == axis.dy) && (0. == axis.dz))
		{
			alpha = PI*0.5;
			beta  = 0.;
		}
		else if((-1 == axis.dy) && (0. == axis.dz))
		{
			alpha = PI + PI*0.5;
			beta  = 0.;
		}
		else if((axis.dx >= 0.) && (axis.dy >= 0.) && (axis.dz < 0.))
		{
			if((0. == axis.dx) && (0. == axis.dy) && (axis.dz < 0.))
			{
				alpha = PI;
				beta = 0.;
			}
			else
			{
				alpha = acos(-axis.dz/d);
				alpha = PI - alpha;
				if(fabs(d - 1.) >= G_TOLERANCE)
					beta = -acos(d);
				else	beta = 0.;
			}
		}
		else if((axis.dx >= 0.) && (axis.dy < 0.) && (axis.dz >= 0.))
		{
			alpha = 2*PI -acos(axis.dz/d);
			beta  = (fabs(d - 1.0) < G_TOLERANCE) ? 0 : -acos(d);
		}
		else if((axis.dx >= 0.) && (axis.dy < 0.) && (axis.dz < 0.))
		{
			alpha = acos(-axis.dz/d);
			alpha+= PI;
			beta  = (0. == axis.dx) ? 0. : -acos(d);
		}
		else if((axis.dx < 0.) && (axis.dy >= 0.) && (axis.dz >= 0.))
		{
			alpha = acos(axis.dz/d);
			beta  = acos(d);
		}
		else if((axis.dx < 0.) && (axis.dy >= 0.) && (axis.dz < 0.))
		{
			alpha = acos(-axis.dz/d);
			alpha = PI - alpha;
			beta  = acos(d);
		}
		else if((axis.dx < 0.) && (axis.dy < 0.) && (axis.dz >= 0.))
		{
			alpha = acos(axis.dz/d);
			alpha = 2*PI - alpha;
			beta  = acos(d);
		}
		else if((axis.dx < 0.) && (axis.dy < 0.) && (axis.dz < 0.))
		{
			alpha = acos(-axis.dz/d);
			alpha+= PI;
			beta  = acos(d);
		}
		else if((axis.dx < 0.) && (axis.dy >= 0.) && (axis.dz < 0.))
		{
			alpha = acos(-axis.dz/d);
			alpha = PI - alpha;
			beta  = acos(d);
		}
		else if(0. == axis.dx )
		{
			alpha = acos(axis.dz/d);
			beta  = 0.;
		}
		else
		{
			alpha = acos(axis.dz/d);
			beta  = -acos(d);
		}//else if((axis.dx > 0.) && (axis.dy > 0.) && (axis.dz < 0.)){
		//	alpha = acos(-axis.dz/d);
		//	alpha = PI - alpha;
		//	beta = -acos(d);
		//}else if((axis.dx <= 0.) && (axis.dy <= ) && (axis.dz < 0.)){
		//}
	}
	else if(axis.dx > 0.)
	{
		alpha = 0.;
		beta = PI + PI*0.5;
	}
	else if(axis.dx < 0.)
	{
		alpha = 0.;
		beta = PI*0.5;
	}
}

/**	\brief	The CGeometry::RotateOnXYPlane function

	\param	vec	a parameter of type const VECTOR_T
	\param	nDeg	a parameter of type double

	\return	VECTOR_T	
*/
VECTOR_T CGeometry::RotateOnXYPlane(const VECTOR_T vec,double nDeg){
	VECTOR_T vecRet={0};

	double nRad=DEG2RAD(nDeg);
	vecRet.dx = vec.dx*cos(nRad) - vec.dy*sin(nRad);
	vecRet.dy = vec.dx*sin(nRad) + vec.dy*cos(nRad);
	vecRet.dz = vec.dz;

	return vecRet;
}

/**	\brief	The CGeometry::RotateAboutXYAxis function\n
	first, rotate about x axis by alpha angle,\n
	then rotate about y axis by beta angle.\n
	rotating matrix=\n
		| 1      0           0    | | cos(beta) 0 -sin(beta)|\n
	|x y z|*| 0 cos(alpha)  sin(alpha)|*|     0     1     0     |\n
		| 0 -sin(alpha) cos(alpha)| | sin(beta) 0 cos(beta) |

	\param	pt	a parameter of type POINT_T&
	\param	alpha	a parameter of type double
	\param	beta	a parameter of type double

	\return	POINT_T	
*/
POINT_T CGeometry::RotateAboutXYAxis(POINT_T& pt,const double& alpha,const double& beta){
    	POINT_T ptRet={0,};
	double cos_alpha=cos(alpha),cos_beta=cos(beta),sin_alpha=sin(alpha),sin_beta=sin(beta);
	if((fabs(cos_alpha) < 0.0000000001)) cos_alpha = 0.;
	if((fabs(cos_beta) < 0.0000000001)) cos_beta = 0.;
	if((fabs(sin_alpha) < 0.0000000001)) sin_alpha = 0.;
	if((fabs(sin_beta) < 0.0000000001)) sin_beta = 0.;
	
	ptRet.x = pt.x*cos_beta + pt.y*sin_alpha*sin_beta + pt.z*cos_alpha*sin_beta;
	ptRet.y = pt.y*cos_alpha- pt.z*sin_alpha;
	ptRet.z = -pt.x*sin_beta + pt.y*sin_alpha*cos_beta + pt.z*cos_alpha*cos_beta;

	return ptRet;
}

/**	\brief	The CGeometry::RotateAboutXYAxis function\n
	first, rotate about x axis by alpha angle,\n
	then rotate about y axis by beta angle.\n
	rotating matrix=\n
		| 1      0         0      | | cos(beta) 0 -sin(beta)|\n
	|x y z|*| 0  cos(alpha) sin(alpha)|*|     0     1     0     |\n
		| 0 -sin(alpha) cos(alpha)| | sin(beta) 0 cos(beta) |

	\param	vec	a parameter of type const VECTOR_T&
	\param	alpha	a parameter of type double
	\param	beta	a parameter of type double

	\return	VECTOR_T	
*/
VECTOR_T CGeometry::RotateAboutXYAxis(const VECTOR_T& vec,double alpha,double beta){
	VECTOR_T vecRet={0};

	vecRet.dx = vec.dx*cos(beta) + vec.dy*sin(beta)*sin(alpha) + vec.dz*sin(beta)*cos(alpha);
	vecRet.dy = vec.dy*cos(alpha)- vec.dz*sin(alpha);
	vecRet.dz = -vec.dx*sin(beta)+ vec.dy*sin(alpha)*cos(beta) + vec.dz*cos(alpha)*cos(beta);

	return vecRet;
}

/**	\brief	The CGeometry::RotateAboutYXAxis function\n
	first rotate about y axis by beta angle,\n
	rotate about x axis by alpha angle.\n
		| cos(beta) 0 -sin(beta)| | 1      0          0     |\n
	|x y z|*|    0      1     0     |*| 0 cos(alpha)  sin(alpha)|\n
		| sin(beta) 0 cos(beta) | | 0 -sin(alpha) cos(alpah)|  

	\param	pt	a parameter of type POINT_T&
	\param	beta	a parameter of type double
	\param	alpha	a parameter of type double

	\return	POINT_T	
*/
POINT_T CGeometry::RotateAboutYXAxis(POINT_T& pt,double beta,double alpha){
    	POINT_T ptRet={0};

	ptRet.x = pt.x*cos(beta) + pt.z*sin(beta);
	ptRet.y = pt.x*sin(alpha)*sin(beta) + pt.y*cos(alpha) - pt.z*sin(alpha)*cos(beta);
	ptRet.z = -pt.x*cos(alpha)*sin(beta)+ pt.y*sin(alpha) + pt.z*cos(alpha)*cos(beta);

	return ptRet;
}

/**	\brief	The CGeometry::RotateAboutYXAxis function\n
	first rotate about y axis by beta angle,\n
	rotate about x axis by alpha angle.\n
		| cos(beta)  0 sin(beta)| | 1     0           0     |\n
	|x y z|*|    0       1     0    |*| 0 cos(alpha)  sin(alpha)|\n
		| -sin(beta) 0 cos(beta)| | 0 -sin(alpha) cos(alpah)|

	\param	vec	a parameter of type VECTOR_T&
	\param	beta	a parameter of type double
	\param	alpha	a parameter of type double

	\return	VECTOR_T	
*/
VECTOR_T CGeometry::RotateAboutYXAxis(VECTOR_T& vec,double beta,double alpha){
	VECTOR_T vecRet={0};

	vecRet.dx = vec.dx*cos(beta) + vec.dz*sin(beta);
	vecRet.dy = vec.dx*sin(alpha)*sin(beta) + vec.dy*cos(alpha) - vec.dz*sin(alpha)*cos(beta);
	vecRet.dz = -vec.dx*cos(alpha)*sin(beta)+ vec.dy*sin(alpha) + vec.dz*cos(alpha)*cos(beta);

	return vecRet;
}

/**	\brief	The CGeometry::RotateAboutZAxis function\n
	rotate about z axis by theta.\n
	rotating matrix=\n
	| cos(theta) -sin(theta)  0 | |x|\n
	| sin(theta)  cos(theta)  0 |*|y|\n
	|      0          0       1 | |z|

	\param	vec	a parameter of type const VECTOR_T&
	\param	theta	a parameter of type const double

	\return	VECTOR_T	
*/
VECTOR_T CGeometry::RotateAboutZAxis(const VECTOR_T& vec,const double theta){
	VECTOR_T vecRet={0};

	double s=sin(theta),c=cos(theta);
	vecRet.dx = vec.dx*c  - vec.dy*s;
	vecRet.dy = vec.dx*s + vec.dy*c;
	vecRet.dz = vec.dz;

	return vecRet;
}

/**	\brief	The CGeometry::RotateAboutZAxis function\n
	rotating matrix=\n
	| cos(theta) -sin(theta)  0 | |x|\n
	| sin(theta)  cos(theta)  0 |*|y|\n
	|      0          0       1 | |z|

	\param	pt	a parameter of type const POINT_T&
	\param	theta	angle in radian

	\return	POINT_T	
*/
POINT_T CGeometry::RotateAboutZAxis(const POINT_T& pt,const double theta){
	POINT_T ptRet={0};

	double s=sin(theta),c=cos(theta);
	ptRet.x = pt.x*c - pt.y*s;
	ptRet.y = pt.x*s + pt.y*c;
	ptRet.z = pt.z;

	return ptRet;
}

/**	\brief	The CGeometry::Rotate function

	\param	pt	a parameter of type POINT_T
	\param	quat	a parameter of type QUAT_T

	\return	POINT_T	
*/
POINT_T CGeometry::Rotate(POINT_T pt,QUAT_T quat){
	POINT_T ret={0.};

	QUAT_T tmp={0.};
	tmp.w = 0;tmp.x = pt.x;tmp.y = pt.y;tmp.z = pt.z;

	QUAT_T inverse=Inverse(quat);
	tmp = (quat*tmp)*inverse;

	ret.x = tmp.x;ret.y = tmp.y;ret.z = tmp.z;

	return ret;
}

/**	\brief	The CGeometry::Rotate function

	\param	vec	a parameter of type VECTOR_T
	\param	quat	a parameter of type QUAT_T

	\return	VECTOR_T	
*/
VECTOR_T CGeometry::Rotate(VECTOR_T vec,QUAT_T quat){
	VECTOR_T ret={0.};

	QUAT_T tmp={0.};
	tmp.w = 0;tmp.x = vec.dx;tmp.y = vec.dy;tmp.z = vec.dz;

	QUAT_T inverse=Inverse(quat);
	tmp = (quat*tmp)*inverse;

	ret.dx = tmp.x;ret.dy = tmp.y;ret.dz = tmp.z;

	return ret;
}

/**	\brief	The CGeometry::IsPointOnLine function

	\param	x	a parameter of type double
	\param	y	a parameter of type double
	\param	px	a parameter of type double
	\param	py	a parameter of type double
	\param	qx	a parameter of type double
	\param	qy	a parameter of type double

	\return	bool	
*/
bool CGeometry::IsPointOnLine(double x,double y,double px,double py,double qx,double qy){
	bool bRet = false;

	if((px != qx) || (py != qy)){
		double tx[2]={0},ty[2]={0};
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
		if((x < tx[0]) || (x > tx[1]) || (y < ty[0]) || (y > ty[1])) return false;

		/*
		double ux=x - px;
		double uy=y - py;
		double vx=x - qx;
		double vy=y - qy;
		double det=ux*vy - uy*vx;
		if(0. == det){
			double t=-1;
			if(vx)	t = (x - px)/vx;
			else	t = (y - py)/vy;

			if((t >= 0.) && (t <= 1)) bRet = true;
		}
		*/

		double len[2]={0},dot=0.;
		len[0] = sqrt((px - x)*(px - x) + (py - y)*(py - y));
		len[1] = sqrt((qx - x)*(qx - x) + (qy - y)*(qy - y));
		dot    = (px - x)*(qx - x) + (py - y)*(qy - y);
		if((0. != len[0]) && (0. != len[1])){
			double cosval = (dot)/(len[0]*len[1]);
			if((cosval <= (-1 + 0.000001)) || (cosval >= (1 - 0.000001))) bRet = true;
			//if((cosval > 1. - 0.001) || (cosval < -1. + 0.001)) bRet = true;
		}
	}

	return bRet;
}

/**	\brief	The CGeometry::IntersectLine2D function\n
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
INTERSECTION_E CGeometry::IntersectLine2D(double* x,double* y,double x1,double y1,double x2,double y2,double x3,double y3,double x4,double y4,const double tol){
	double Ax=0.,Bx=0.,Cx=0.,Ay=0.,By=0.,Cy=0.,d=0.,e=0.,f=0.;
	double x1lo=0.,x1hi=0.,y1lo=0.,y1hi=0.;

	Ax = x2 - x1;
	Bx = x3 - x4;
	// X bound box test
	if(Ax < 0.){
		x1lo=x2; x1hi=x1;
	}else{
		x1hi=x2; x1lo=x1;
	}
	if(Bx > 0.){
		if((x1hi < x4) || (x3 < x1lo)) return DONT_INTERSECTION;
	} else {
		if((x1hi < x3) || (x4 < x1lo)) return DONT_INTERSECTION;
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
		if((y1hi < y4) || (y3 < y1lo)) return DONT_INTERSECTION;
	} else {
		if((y1hi < y3) || (y4 < y1lo)) return DONT_INTERSECTION;
	}

	Cx = x1-x3;
	Cy = y1-y3;
	f = Ay*Bx - Ax*By;	/* both denominator*/
	/// lines are colinear.
	if(0. == f) return COLLINEAR_LINE;

	d = By*Cx - Bx*Cy;	/* alpha numerator*/
	if(f > 0.) {		/* alpha tests*/
		if((d < (0 + tol)) || (d > (f - tol))) return DONT_INTERSECTION;
	}else{
		if((d > (0 - tol)) || (d < (f + tol))) return DONT_INTERSECTION;
	}

	e = Ax*Cy - Ay*Cx;	/* beta numerator*/
	if(f > 0.) {		/* beta tests*/
		if((e < (0 + tol)) || (e > (f - tol))) return DONT_INTERSECTION;
	}else{
		if((e > (0 - tol)) || (e < (f + tol))) return DONT_INTERSECTION;
	}

	/*compute intersection coordinates*/
	double alpha = d/f;
	
	if(x1 == x2)
		*x = x1;
	else	*x = x1 + alpha*Ax;
	if(y1 == y2)
		*y = y1;
	else	*y = y1 + alpha*Ay;

	return DO_INTERSECTION;
}

/**	\brief	The CGeometry::IntersectLine2D function

	\param	pt	a parameter of type POINT_T&
	\param	line1	a parameter of type const LINE_T&
	\param	line2	a parameter of type const LINE_T&

	\return	INTERSECTION_E	
*/
INTERSECTION_E CGeometry::IntersectLine2D(POINT_T& pt,const LINE_T& line1,const LINE_T& line2){
	double x[4]={0.},y[4]={0.};
	
	x[0] = line1.ptStart.x;
	y[0] = line1.ptStart.y;
	x[1] = line1.ptEnd.x;
	y[1] = line1.ptEnd.y;
	x[2] = line2.ptStart.x;
	y[2] = line2.ptStart.y;
	x[3] = line2.ptEnd.x;
	y[3] = line2.ptEnd.y;
	
	INTERSECTION_E ret = IntersectLine2D(&(pt.x),&(pt.y),x[0],y[0],x[1],y[1],x[2],y[2],x[3],y[3],0.);
	
	return ret;
}

/**	\brief	The CGeometry::IntersectLineWithPlane function

	\param	pt	a parameter of type POINT_T*
	\param	line	a parameter of type LINE_T
	\param	plane	a parameter of type PLANE_T

	\return	bool	
*/
bool CGeometry::IntersectLineWithPlane(POINT_T* pt,LINE_T line,PLANE_T plane){
	int sign1=0, sign2=0; // must be int since gonna do a bitwise ^
	double aa= plane.a; double bb= plane.b; double cc= plane.c;
	double dd= plane.d;

	// get signs
	double temp1 = (aa*line.ptStart.x) + (bb*line.ptStart.y) + (cc*line.ptStart.z) + dd;
	if (temp1 < -0.001)	sign1= -1;
	else if (temp1 > 0.001)	sign1= 1;
	else{
		pt->x = line.ptStart.x;
		pt->y = line.ptStart.y;
		pt->z = line.ptStart.z;
		return true;
	}

	double temp2= (aa*line.ptEnd.x) + (bb*line.ptEnd.y) + (cc*line.ptEnd.z) + dd;
	if (temp2 < -0.001)     sign2= -1;
	else if (temp2 > 0.001) sign2= 1;
	else{
		pt->x = line.ptEnd.x;
		pt->y = line.ptEnd.y;
		pt->z = line.ptEnd.z;
		return false;
	}

	// signs different? 
	// recall: -1^1 == 1^-1 ==> 1    case 4 & 5, see Gems III
	//         -1^-1 == 1^1 ==> 0    case 2 & 3, see Gems III
	if (sign1 ^ sign2) {
		// compute intersection point 
		double dx= line.ptEnd.x - line.ptStart.x;
		double dy= line.ptEnd.y - line.ptStart.y;
		double dz= line.ptEnd.z - line.ptStart.z;

		double denom= (aa*dx) + (bb*dy) + (cc*dz);
		double tt   = -((aa*line.ptStart.x) + (bb*line.ptStart.y) + (cc*line.ptStart.z) + dd)/denom;

		pt->x= line.ptStart.x + (tt * dx);
		pt->y= line.ptStart.y + (tt * dy);
		pt->z= line.ptStart.z + (tt * dz);
		
		return true;
	}
	
	return false;
}

/**	\brief	The CGeometry::IntersectLineToVolume function

	\param	pLine	a parameter of type PLINE_T
	\param	pVolume	a parameter of type PSQVOLUME
	\param	pRet	a parameter of type PPOINT_T

	\return	bool	
*/
bool CGeometry::IntersectLineToVolume(PLINE_T pLine,PSQVOLUME pVolume,PPOINT_T pRet){
	assert(pVolume && "pVolume is invalid-[IntersectToVolume]");
	bool ret=false;

	if(pVolume){
		// get volume of line.
		SQVOLUME vol={0};
		vol.x[LOW] = vol.x[HIGH] = pLine->ptStart.x;
		vol.y[LOW] = vol.y[HIGH] = pLine->ptStart.y;
		vol.z[LOW] = vol.z[HIGH] = pLine->ptStart.z;
		if(vol.x[LOW] > pLine->ptEnd.x) vol.x[LOW] = pLine->ptEnd.x;
		if(vol.x[HIGH]< pLine->ptEnd.x) vol.x[HIGH]= pLine->ptEnd.x;
		if(vol.y[LOW] > pLine->ptEnd.y) vol.y[LOW] = pLine->ptEnd.y;
		if(vol.y[HIGH]< pLine->ptEnd.y) vol.y[HIGH]= pLine->ptEnd.y;
		if(vol.z[LOW] > pLine->ptEnd.z) vol.z[LOW] = pLine->ptEnd.z;
		if(vol.z[HIGH]< pLine->ptEnd.z) vol.z[HIGH]= pLine->ptEnd.z;
		
		if((vol.x[LOW] > pVolume->x[HIGH]) || (vol.x[HIGH] < pVolume->x[LOW]) ||
			(vol.y[LOW] > pVolume->y[HIGH]) || (vol.y[HIGH] < pVolume->y[LOW]) ||
			(vol.z[LOW] > pVolume->z[HIGH]) || (vol.z[HIGH] < pVolume->z[LOW])){
			// line exists out of volume.
			return false;
		}

		// get edge of volume boundary.
		int nBoundary=4;
		LINE_T lnBoundary[4]={0};
		lnBoundary[0].ptStart.x = pVolume->x[LOW];
		lnBoundary[0].ptStart.y = pVolume->y[LOW];
		lnBoundary[0].ptStart.z = 0.;
		lnBoundary[0].ptEnd.x = pVolume->x[HIGH];
		lnBoundary[0].ptEnd.y = pVolume->y[LOW];
		lnBoundary[0].ptEnd.z = 0.;
		lnBoundary[1].ptStart.x = pVolume->x[HIGH];
		lnBoundary[1].ptStart.y = pVolume->y[LOW];
		lnBoundary[1].ptStart.z = 0.;
		lnBoundary[1].ptEnd.x = pVolume->x[HIGH];
		lnBoundary[1].ptEnd.y = pVolume->y[HIGH];
		lnBoundary[1].ptEnd.z = 0.;
		lnBoundary[2].ptStart.x = pVolume->x[HIGH];
		lnBoundary[2].ptStart.y = pVolume->y[HIGH];
		lnBoundary[2].ptStart.z = 0.;
		lnBoundary[2].ptEnd.x = pVolume->x[LOW];
		lnBoundary[2].ptEnd.y = pVolume->y[HIGH];
		lnBoundary[2].ptEnd.z = 0.;
		lnBoundary[3].ptStart.x = pVolume->x[LOW];
		lnBoundary[3].ptStart.y = pVolume->y[HIGH];
		lnBoundary[3].ptStart.z = 0.;
		lnBoundary[3].ptEnd.x = pVolume->x[LOW];
		lnBoundary[3].ptEnd.y = pVolume->y[LOW];
		lnBoundary[3].ptEnd.z = 0.;

		int nCount=0;
		LINE_T ln[4]={0};
		memcpy(&ln[nCount++],pLine,sizeof(LINE_T));
		
		POINT_T pt={0};
		bool bIntersection=false;
		int i = 0;
		int j = 0;
		for(i=0;i < nCount;bIntersection=false){
			for(j=0;j < nBoundary;j++){
				if(DO_INTERSECTION == IntersectLine2D(pt,lnBoundary[j],ln[i])){
					if(!(IsEqualPoint2D(pt,ln[i].ptStart) || IsEqualPoint2D(pt,ln[i].ptEnd))){
						// occur line intersect!
						bIntersection=true;
						double z = CMath::GetInterpolationZ(ln[i],pt);
						ln[nCount].ptStart.x = pt.x;
						ln[nCount].ptStart.y = pt.y;
						ln[nCount].ptStart.z = z;
						ln[nCount].ptEnd.x = ln[i].ptEnd.x;
						ln[nCount].ptEnd.y = ln[i].ptEnd.y;
						ln[nCount++].ptEnd.z = z;

						ln[i].ptEnd.x = pt.x;
						ln[i].ptEnd.y = pt.y;
						ln[i].ptEnd.z = z;
						ret=true;
					}
				}
			}
			if(bIntersection)
				i=0;
			else	i++;
		}

		CRect2d rect;
		rect.x = pVolume->x[LOW];
		rect.y = pVolume->y[LOW];
		rect.width = pVolume->x[HIGH] - pVolume->x[LOW];
		rect.height= pVolume->y[HIGH] - pVolume->y[LOW];
		for(i=0;i < nCount;i++){
			pt.x = (ln[i].ptStart.x + ln[i].ptEnd.x)*0.5;
			pt.y = (ln[i].ptStart.y + ln[i].ptEnd.y)*0.5;
			if(rect.IsInnerPoint(pt)){
				pLine->ptStart.x = ln[i].ptStart.x;
				pLine->ptStart.y = ln[i].ptStart.y;
				pLine->ptStart.z = ln[i].ptStart.z;
				pLine->ptEnd.x = ln[i].ptEnd.x;
				pLine->ptEnd.y = ln[i].ptEnd.y;
				pLine->ptEnd.z = ln[i].ptEnd.z;
				break;
			}
		}
	}

	return ret;
}

/**	\brief	The CGeometry::IsLeftSidePoint function\n
	test which pt is located on left side of line or not.

	\param	pt	a parameter of type const POINT_T&
	\param	line	a parameter of type const LINE_T&

	\return	true - located on left side, false - 
*/
bool CGeometry::IsLeftSidePoint(const POINT_T& pt,const LINE_T& line){
	double ux=line.ptEnd.x - line.ptStart.x;
	double uy=line.ptEnd.y - line.ptStart.y;
	double vx=pt.x - line.ptStart.x;
	double vy=pt.y - line.ptStart.y;
		
	return (ux*vy - uy*vx) > 0.;
}

/**	\brief	The CGeometry::IsRightSidePoint function\n
	test which the point is located on right side of line or not.

	\param	pt	a parameter of type const POINT_T&
	\param	line	a parameter of type const LINE_T&

	\return	true - located on right side, false - 
*/
bool CGeometry::IsRightSidePoint(const POINT_T& pt,const LINE_T& line){
	double ux=line.ptEnd.x - line.ptStart.x;
	double uy=line.ptEnd.y - line.ptStart.y;
	double vx=pt.x - line.ptStart.x;
	double vy=pt.y - line.ptStart.y;
	
	return (ux*vy - uy*vx) < 0.;
}

/**	\brief	The CGeometry::IsEqualPoint2D function

	\param	x	a parameter of type POINT_T&
	\param	y	a parameter of type POINT_T&
	\param	tol	a parameter of type double

	\return	bool	
*/
bool CGeometry::IsEqualPoint2D(POINT_T& x,POINT_T& y,double tol){
	bool ret=false;

	if((fabs(x.x - y.x) <= tol) && (fabs(x.y - y.y) <= tol)){
		ret=true;
	}

	return ret;
}
