#include <math.h>
#include <math\vect.h>
#include <graphics\point.h>
#include <graphics\line.h>

CVect::CVect(){dx=0;dy=0;dz=1.;}
CVect::CVect(double x,double y,double z){dx=x;dy=y;dz=z;}
CVect::CVect(const SQVECTOR& vec){dx=vec.dx;dy=vec.dy;dz=vec.dz;}
CVect::CVect(const CVect& vec){dx=vec.dx;dy=vec.dy;dz=vec.dz;}

/**	\brief	The CVect::Valid function


	\return	bool	
*/
bool CVect::Valid(){
	return ((dx*dx + dy*dy + dz*dz) > 0.);
}

/**	\brief	The CVect::Length function


	\return	double	
*/
double CVect::Length(){
    	return sqrt(dx*dx + dy*dy + dz*dz);
}

/**	\brief	The CVect::Flatten function


	\return	void	
*/
void CVect::Flatten(){
    	dz = 0.;
}

/**	\brief	The CVect::Perpendicular function


	\return	CVect	
*/
CVect CVect::Perpendicular(){
    	CVect ret;
	
	CVect flat(dx,dy,0),z(0,0,1);
	ret = (z * flat);

	return ret;
}

/**	\brief	The CVect::Normalize function


	\return	bool	
*/
bool CVect::Normalize(){
	bool bRet=false;

    	if(Valid()){
	    	double nFactor=1./Length();
		dx *= nFactor;dy *= nFactor;dz *= nFactor;

		bRet = true;
	}

	return bRet;
}

/**	\brief	The CVect::Dot function

	\param	vec	a parameter of type const CVect&

	\return	double	
*/
double CVect::Dot(const CVect& vec){
	return (dx*vec.dx + dy*vec.dy + dz*vec.dz);
}

/**	\brief	The = function

	\param	vec	a parameter of type const SQVECTOR&

	\return	void CVect::operator	
*/
void CVect::operator =(const SQVECTOR& vec){
	dx = vec.dx;dy = vec.dy;dz = vec.dz;
}

/**	\brief	The = function

	\param	vec	a parameter of type const CVect&

	\return	void CVect::operator	
*/
void CVect::operator =(const CVect& vec){
	dx = vec.dx;dy = vec.dy;dz = vec.dz;
}

/**	\brief	The += function

	\param	vec	a parameter of type const CVect&

	\return	void CVect::operator	
*/
void CVect::operator +=(const CVect& vec){
    	dx += vec.dx;dy += vec.dy;dz += vec.dz;
}

/**	\brief	The -= function

	\param	vec	a parameter of type const CVect&

	\return	void CVect::operator	
*/
void CVect::operator -=(const CVect& vec){
    	dx -= vec.dx;dy -= vec.dy;dz -= vec.dz;
}

/**	\brief	The *= function

	\param	vec	a parameter of type const CVect&

	\return	void CVect::operator	
*/
void CVect::operator *=(const CVect& vec){
    	CVect tmp=*this;
	
	dx = tmp.dy*vec.dz - vec.dy*tmp.dz;
	dy = vec.dx*tmp.dz - tmp.dx*vec.dz;
	dz = tmp.dx*vec.dy - vec.dx*tmp.dy;
}

/**	\brief	The + function

	\param	vec1	a parameter of type CVect&
	\param	vec2	a parameter of type CVect&

	\return	CVect CVect::operator	
*/
CVect CVect::operator +(CVect& rhs){
	CVect ret;
	
	ret.dx = dx + rhs.dx;
	ret.dy = dy + rhs.dy;
	ret.dz = dz + rhs.dz;

	return ret;
}

/**	\brief	The - function

	\param	vec1	a parameter of type CVect&
	\param	vec2	a parameter of type CVect&

	\return	CVect CVect::operator	
*/
CVect CVect::operator -(CVect& rhs){
    	CVect ret(dx - rhs.dx,dy - rhs.dy,dz - rhs.dz);

	return ret;
}

/**	\brief	The CVect::Dot function

	\param	vec1	a parameter of type CVect&
	\param	vec2	a parameter of type CVect&

	\return	double	
*/
double CVect::Dot(CVect& rhs){
    	return (dx*rhs.dx + dy*rhs.dy + dz*rhs.dz);
}

/**	\brief	The * function

	\param	vec1	a parameter of type CVect&
	\param	vec2	a parameter of type CVect&

	\return	CVect CVect::operator	
*/
CVect CVect::operator *(CVect& rhs){
	CVect ret;
	
    	ret.dx = dy*rhs.dz - rhs.dy*dz;
	ret.dy = rhs.dx*dz - dx*rhs.dz;
	ret.dz = dx*rhs.dy - rhs.dx*dy;

	return ret;
} 

/**	\brief	The - function

	\param	vec	a parameter of type const VECTOR_T&

	\return	VECTOR_T operator	
*/
VECTOR_T operator -(const VECTOR_T& vec){
	VECTOR_T ret;

	ret.dx = -vec.dx;
	ret.dy = -vec.dy;
	ret.dz = -vec.dz;

	return ret;
}

/**	\brief	The - function

	\param	pt1	a parameter of type const POINT_T
	\param	pt2	a parameter of type const POINT_T

	\return	VECTOR_T operator	
*/
VECTOR_T operator -(const POINT_T pt1,const POINT_T pt2){
	VECTOR_T ret;

	ret.dx = pt1.x - pt2.x;
	ret.dy = pt1.y - pt2.y;
	ret.dz = pt1.z - pt2.z;

	return ret;
}

/**	\brief	The + function

	\param	vec1	a parameter of type const VECTOR_T&
	\param	vec2	a parameter of type const VECTOR_T&

	\return	VECTOR_T operator
*/
VECTOR_T operator +(const VECTOR_T& vec1,const VECTOR_T& vec2){
	VECTOR_T ret;
	
	ret.dx = vec1.dx + vec2.dx;
	ret.dy = vec1.dy + vec2.dy;
	ret.dz = vec1.dz + vec2.dz;

	return ret;
}

/**	\brief	The - function

	\param	vec1	a parameter of type const VECTOR_T&
	\param	vec2	a parameter of type const VECTOR_T&

	\return	VECTOR_T operator	
*/
VECTOR_T operator -(const VECTOR_T& vec1,const VECTOR_T& vec2){
	VECTOR_T ret;
	
	ret.dx = vec1.dx - vec2.dx;
	ret.dy = vec1.dy - vec2.dy;
	ret.dz = vec1.dz - vec2.dz;

	return ret;
}

/**	\brief	The * function

	\param	nVal	a parameter of type const double
	\param	vec	a parameter of type const VECTOR_T&

	\return	VECTOR_T operator	
*/
VECTOR_T operator *(const double nVal,const VECTOR_T& vec){
	VECTOR_T ret;
	
	ret.dx = nVal*vec.dx;
	ret.dy = nVal*vec.dy;
	ret.dz = nVal*vec.dz;

	return ret;
}

/**	\brief	The * function\n
	cross product of two CVectors

	\param	vec1	a parameter of type const VECTOR_T&
	\param	vec2	a parameter of type const VECTOR_T&

	\return	VECTOR_T operator	
*/
VECTOR_T operator *(const VECTOR_T& vec1,const VECTOR_T& vec2){
	VECTOR_T ret;
	
	ret.dx = vec1.dy*vec2.dz - vec2.dy*vec1.dz;
	ret.dy = vec2.dx*vec1.dz - vec1.dx*vec2.dz;
	ret.dz = vec1.dx*vec2.dy - vec2.dx*vec1.dy;

	return ret;
}

/**	\brief	The Length function

	\param	vec	a parameter of type const VECTOR_T&

	\return	double	
*/
double Length(const VECTOR_T& vec){
	return sqrt(vec.dx*vec.dx + vec.dy*vec.dy + vec.dz*vec.dz);
}

/**	\brief	The Normalize function

	\param	vec	a parameter of type VECTOR_T&

	\return	bool	
*/
bool Normalize(VECTOR_T& vec){
	double nMagnitude=sqrt(vec.dx*vec.dx + vec.dy*vec.dy + vec.dz*vec.dz);
	bool bRet;

	if(nMagnitude > 0.){
		double nFactor=1./nMagnitude;

		vec.dx *= nFactor;
		vec.dy *= nFactor;
		vec.dz *= nFactor;

		bRet = true;
	}

	return bRet;
}

/**	\brief	The Perpendicular function\n
	for get perpendicular vector to vec, rotate 90 degree about z-axis.\n
	and normalize.

	\param	vec	a parameter of type VECTOR_T&

	\return	VECTOR_T	
*/
VECTOR_T Perpendicular(VECTOR_T& vec){
	VECTOR_T ret={0};

	ret.dx = -vec.dy;
	ret.dy = vec.dx;
	ret.dz = vec.dz;
	if((0. != ret.dx) || (0. != ret.dy) || (0. != ret.dz)) Normalize(ret);

	return ret;
}

/**	\brief	The IsPerpendicular function\n
	check two vector are perpendicular.

	\param	vec1	a parameter of type const VECTOR_T&
	\param	vec2	a parameter of type const VECTOR_T&

	\return	bool	
*/
bool IsPerpendicular(const VECTOR_T& vec1,const VECTOR_T& vec2){
	double dot=vec1.dx*vec2.dx + vec1.dy*vec2.dy + vec1.dz*vec2.dz;

	return (0. == dot) ? true : false;
}

/**	\brief	The DotProduct function

	\param	vec1	a parameter of type const VECTOR_T
	\param	vec2	a parameter of type const VECTOR_T

	\return	double	
*/
double DotProduct(const VECTOR_T vec1,const VECTOR_T vec2){
	double nRet=vec1.dx*vec2.dx + vec1.dy*vec2.dy + vec1.dz*vec2.dz;
	
	return nRet;
}

/**	\brief	The CrossProduct function

	\param	vec1	a parameter of type const VECTOR_T
	\param	vec2	a parameter of type const VECTOR_T

	\return	VECTOR_T	
*/
VECTOR_T CrossProduct(const VECTOR_T vec1,const VECTOR_T vec2){
	VECTOR_T vecRet={0};

	vecRet.dx = vec1.dy*vec2.dz - vec2.dy*vec1.dz;
	vecRet.dy = vec2.dx*vec1.dz - vec1.dx*vec2.dz;
	vecRet.dz = vec1.dx*vec2.dy - vec2.dx*vec1.dy;

	return vecRet;
}
