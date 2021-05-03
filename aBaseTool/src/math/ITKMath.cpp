#include <math.h>
#include <assert.h>
#include <ITKDef.h>
#include <math\ITKMath.h>

/**	\brief	The CMath::IsGreaterEqualThan function

	\param	val1	a parameter of type const double
	\param	val2	a parameter of type const double

	\return	true if val1 is greater equal than val2.
*/
bool CMath::IsGreaterEqualThan(const double val1,const double val2){
	return (val1 - val2) > -0.0000000001; 
}

/**	\brief	The CMath::round function

	\param	a	a 반올림 할 수
	\param	b	a 반올림할 소수점 자리

	\return	double	
*/
double CMath::round(double a, int at)
{
	double ret = a;

	ret*= pow(10.f , at - 1);
	ret = (ret > 0) ?  floor(ret + 0.5) : ceil(ret - 0.5);
	ret*= pow(10.f , -(at - 1));
	
	return ret;
}

/**	\brief	The CMath::GetInterpolationZ function

	\param	line	a parameter of type LINE_T&
	\param	pt	a parameter of type POINT_T&

	\return	double	
*/
double CMath::GetInterpolationZ(LINE_T& line,POINT_T& pt){
	double z=0.;
	double nDeltaX = line.ptEnd.x - line.ptStart.x;
	double nDeltaY = line.ptEnd.y - line.ptStart.y;

	if(fabs(nDeltaX) > fabs(nDeltaY)){
		double x = pt.x - line.ptStart.x;
		double nDeltaZ = line.ptEnd.z - line.ptStart.z;
		
		z = x*(nDeltaZ/nDeltaX) + line.ptStart.z;
	}else{
		double y = pt.y - line.ptStart.y;
		double nDeltaZ = line.ptEnd.z - line.ptStart.z;

		z = y*(nDeltaZ/nDeltaY) + line.ptStart.z;
	}

	return z;
}

/**	\brief	The - function

	\param	pt1	a parameter of type const POINT_T
	\param	pt2	a parameter of type const POINT_T

	\return	VECTOR_T CMath::operator	
*/
/*
VECTOR_T CMath::operator -(const POINT_T pt1,const POINT_T pt2){
	VECTOR_T vecRet={0.};

	vecRet.dx = pt2.x - pt1.x;
	vecRet.dy = pt2.y - pt1.y;
	vecRet.dz = pt2.z - pt1.z;

	return vecRet;
}
*/
/**	\brief	The + function

	\param	vec1	a parameter of type const VECTOR_T
	\param	vec2	a parameter of type const VECTOR_T

	\return	VECTOR_T CMath::operator	
*/
/*
VECTOR_T CMath::operator +(const VECTOR_T vec1,const VECTOR_T vec2){
	VECTOR_T vecRet={0.};

	vecRet.dx = vec1.dx + vec2.dx;
	vecRet.dy = vec1.dy + vec2.dy;
	vecRet.dz = vec1.dz + vec2.dz;

	return vecRet;
}
*/
/**	\brief	The - function

	\param	vec1	a parameter of type const VECTOR_T
	\param	vec2	a parameter of type const VECTOR_T

	\return	VECTOR_T CMath::operator	
*/
/*
VECTOR_T CMath::operator -(const VECTOR_T vec1,const VECTOR_T vec2){
	VECTOR_T vecRet={0.};

	vecRet.dx = vec1.dx - vec2.dx;
	vecRet.dy = vec1.dy - vec2.dy;
	vecRet.dz = vec1.dz - vec2.dz;

	return vecRet;
}
*/
/**	\brief	The CMath::operator* function

	\param	vec	a parameter of type const VECTOR_T
	\param	nValue	a parameter of type const double

	\return	VECTOR_T	
*/
/*
VECTOR_T CMath::operator*(const VECTOR_T vec,const double nValue){
	VECTOR_T vecRet={0};

	vecRet.dx = vec.dx*nValue;
	vecRet.dy = vec.dy*nValue;
	vecRet.dz = vec.dz*nValue;

	return vecRet;
}
*/
/**	\brief	The CMath::operator* function

	\param	nValue	a parameter of type const double
	\param	vec	a parameter of type const VECTOR_T

	\return	VECTOR_T	
*/
/*
VECTOR_T CMath::operator*(const double nValue,const VECTOR_T vec){
	VECTOR_T vecRet={0};

	vecRet.dx = vec.dx*nValue;
	vecRet.dy = vec.dy*nValue;
	vecRet.dz = vec.dz*nValue;

	return vecRet;
}
*/
/**	\brief	The CMath::NormalizeVector function\n
	normalize vector.

	\param	ii	a parameter of type double
	\param	jj	a parameter of type double
	\param	kk	a parameter of type double
	\param	ii2	a parameter of type double *
	\param	jj2	a parameter of type double *
	\param	kk2	a parameter of type double *

	\return	bool	
*/
bool CMath::NormalizeVector(double ii,double jj,double kk, double *ii2,double *jj2,double *kk2){
    	double magnitude= sqrt((double)ii*ii + (double)jj*jj + (double)kk*kk);
	
	if(magnitude > 0.){
		double dfactor = 1.0/magnitude;

		*ii2 = (double)(ii*dfactor);
		*jj2 = (double)(jj*dfactor);
		*kk2 = (double)(kk*dfactor);

		return true;
	}else{
		//assert(magnitude > 0. && "magnitude is less than or equal zero.");
	}
	
	return false;
}

/**	\brief	The CMath::NormalizeVector function\n
	Normalize Vector.

	\param	vec	a parameter of type VECTOR_T&

	\return	bool	
*/
bool CMath::NormalizeVector(VECTOR_T& vec){
	double x=0.,y=0.,z=0.;
	bool ret = false;
	
	if(NormalizeVector(vec.dx,vec.dy,vec.dz,&x,&y,&z)){
		vec.dx = x;
		vec.dy = y;
		vec.dz = z;

		ret = true;
	}

	return ret;
}

/**	\brief	The CMath::DotProduct function

	\param	vtx1	a parameter of type double
	\param	vty1	a parameter of type double
	\param	vtz1	a parameter of type double
	\param	vtx2	a parameter of type double
	\param	vty2	a parameter of type double
	\param	vtz2	a parameter of type double

	\return	double	
*/
double CMath::DotProduct(double vtx1,double vty1,double vtz1,double vtx2,double vty2,double vtz2){
	return (vtx1*vtx2 + vty1*vty2 + vtz1*vtz2);
}

/**	\brief	The CMath::DotProduct function

	\param	vec1	a parameter of type const VECTOR_T&
	\param	vec2	a parameter of type const VECTOR_T&

	\return	double	
*/
double CMath::DotProduct(const VECTOR_T& vec1,const VECTOR_T& vec2){
	return (vec1.dx*vec2.dx + vec1.dy*vec2.dy + vec1.dz*vec2.dz);
}

/**	\brief	Performs cross product.
	
	\param	ii1,jj1,kk1, ii2,j2,kk2 - two vectors
	\param	iicp,jjcp,kkcp          - cross product
*/
void CMath::GetCrossProduct(double ii1,double jj1,double kk1, double ii2,double jj2,double kk2,
	double *iicp,double *jjcp,double *kkcp) {
    	*iicp= jj1*kk2 - jj2*kk1; *jjcp= ii2*kk1 - ii1*kk2; *kkcp= ii1*jj2 - ii2*jj1;
}

/**	\brief	The CMath::GetCrossProduct function

	\param	vec	a parameter of type VECTOR_T&
	\param	vec1	a parameter of type const VECTOR_T
	\param	vec2	a parameter of type const VECTOR_T

	\return	void	
*/
void CMath::GetCrossProduct(VECTOR_T& vec,const VECTOR_T vec1,const VECTOR_T vec2){
	vec.dx = vec1.dy*vec2.dz - vec2.dy*vec1.dz;
	vec.dy = vec2.dx*vec1.dz - vec1.dx*vec2.dz;
	vec.dz = vec1.dx*vec2.dy - vec2.dx*vec1.dy;
}

/**	\brief	The CMath::CrossProduct function

	\param	vec1	a parameter of type const VECTOR_T
	\param	vec2	a parameter of type const VECTOR_T

	\return	VECTOR_T	
*/
VECTOR_T CMath::CrossProduct(const VECTOR_T vec1,const VECTOR_T vec2){
	VECTOR_T vecRet={0};

	vecRet.dx = vec1.dy*vec2.dz - vec2.dy*vec1.dz;
	vecRet.dy = vec2.dx*vec1.dz - vec1.dx*vec2.dz;
	vecRet.dz = vec1.dx*vec2.dy - vec2.dx*vec1.dy;

	return vecRet;
}

/**	\brief	The CMath::GetMatrixFromQuaternion function

	\param	ret	a parameter of type PSQMATRIX
	\param	quat	a parameter of type QUAT_T

	\return	bool	
*/
bool CMath::GetMatrixFromQuaternion(PSQMATRIX ret,QUAT_T quat){
	assert(ret && "ret is NULL");

	if(ret){
		double norm = quat.x*quat.x + quat.y*quat.y + quat.z*quat.z + quat.w*quat.w;
		double s = (norm > 0.0) ? 2.0/norm : 0.0;
		double xs = quat.x*s,ys = quat.y*s,zs = quat.z*s;
		double wx = quat.w*xs, wy = quat.w*ys,wz = quat.w*zs,
		       xx = quat.x*xs, xy = quat.x*ys,xz = quat.x*zs,
		       yy = quat.y*ys, yz = quat.y*zs,zz = quat.z*zs;
		double (*mat)[3] = ret->element;

		mat[0][0] = 1.0 - (yy + zz); 
		mat[1][0] = xy + wz; 
		mat[2][0] = xz - wy;
		mat[0][1] = xy - wz; 
		mat[1][1] = 1.0 - (xx + zz); 
		mat[2][1] = yz + wx;
		mat[0][2] = xz + wy; 
		mat[1][2] = yz - wx; 
		mat[2][2] = 1.0 - (xx + yy);

		return true;
	}

	return false;
}

/**	\brief	The CMath::GetAngle function\n
	get angle between vec1 and vec2.

	\param	vec1	a parameter of type VECTOR_T&
	\param	vec2	a parameter of type VECTOR_T&

	\remarks	return value's type is degree.

	\return	double	
*/
double CMath::GetAngle(VECTOR_T& vec1,VECTOR_T& vec2){
	double theta = 0.;

	double dot = vec1.dx*vec2.dx + vec1.dy*vec2.dy + vec1.dz*vec2.dz;	
	double len[2]={0.};
	len[0] = sqrt(vec1.dx*vec1.dx + vec1.dy*vec1.dy + vec1.dz*vec1.dz);
	len[1] = sqrt(vec2.dx*vec2.dx + vec2.dy*vec2.dy + vec2.dz*vec2.dz);

	if((0. != len[0]) && (0. != len[1])){
		theta = acos(dot/(len[0]*len[1]));
		theta = RAD2DEG(theta);
	}

	return theta;
}
