/********************************************************************
	created:	2004/01/14
	created:	14:1:2004   9:39
	filename: 	D:\Project\ITK\src\Geo\point.cpp
	file path:	D:\Project\ITK\src\Geo
	file base:	point
	file ext:	cpp
	author:		
	
	purpose:	
*********************************************************************/

#include <math.h>
#include <math\vect.h>
#include <graphics\point.h>
#include <graphics/enum.h>


/**	\brief
*/
CPoint3d::CPoint3d(){
	m_nId = ENM_ITK_POINT;
}

/**	\brief
*/
CPoint3d::~CPoint3d(){}

/**	\brief	The Distance function\n
	calculate distance between pt1 and pt2.

	\param	pt1	the first point
	\param	pt2	the second point

	\return	the distance between pt1 and pt2.
*/
const double CPoint3d::Distance(const POINT_T& pt1,const POINT_T& pt2){
	double dx = pt1.x - pt2.x;
	double dy = pt1.y - pt2.y;
	double dz = pt1.z - pt2.z;
	
	return sqrt(dx*dx + dy*dy + dz*dz);
}

/**	\brief	The + function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	POINT_T operator	
*/
POINT_T operator +(const POINT_T& pt1,const POINT_T& pt2){
	POINT_T ret={0,};

	ret.x = pt1.x + pt2.x;
	ret.y = pt1.y + pt2.y;
	ret.z = pt1.z + pt2.z;

	return ret;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
POINT_T operator *(const double nVal,const POINT_T& pt){
	POINT_T ret={0,};

	ret.x = nVal*pt.x;
	ret.y = nVal*pt.y;
	ret.z = nVal*pt.z;

	return ret;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool IsEqual(const POINT_T& x,const POINT_T& y,double nTol){
	bool bRet=false;

	if((fabs(x.x - y.x) <= nTol) && (fabs(x.y - y.y) <= nTol)){
		bRet=true;
	}

	return bRet;
}
