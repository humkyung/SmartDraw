/********************************************************************
	created:	2003/08/09
	created:	9:8:2003   13:03
	filename: 	d:\Project\ITK\src\Math\quat.cpp
	file path:	d:\Project\ITK\src\Math
	file base:	quat
	file ext:	cpp
	author:		
	
	purpose:	
*********************************************************************/

#include <math\quat.h>
using namespace itk;

/**	\brief	The * function

	\param	quat1	a parameter of type const QUAT_T
	\param	quat2	a parameter of type const QUAT_T

	\return	QUAT_T operator	
*/
QUAT_T operator *(const QUAT_T quat1,const QUAT_T quat2){
	QUAT_T ret={0.};

	ret.w = quat1.w*quat2.w - (quat1.x*quat2.x+ quat1.y*quat2.y + quat1.z*quat2.z);
	ret.x = quat1.w*quat2.x + quat2.w*quat1.x + (quat1.y*quat2.z - quat1.z*quat2.y);
	ret.y = quat1.w*quat2.y + quat2.w*quat1.y + (-quat1.x*quat2.z+ quat1.z*quat2.x);
	ret.z = quat1.w*quat2.z + quat2.w*quat1.z + (quat1.x*quat2.y - quat1.y*quat2.x);

	return ret;
}

/**	\brief	The Inverse function

	\param	quat	a parameter of type const QUAT_T

	\return	QUAT_T	
*/
QUAT_T Inverse(const QUAT_T quat){
	QUAT_T ret={0,};

	ret.w = quat.w;
	ret.x = -quat.x;
	ret.y = -quat.y;
	ret.z = -quat.z;
	double nMagnitude=quat.w*quat.w + quat.x*quat.x + quat.y*quat.y + quat.z*quat.z;
	if(nMagnitude > 0.){
		ret.w /= nMagnitude;
		ret.x /= nMagnitude;
		ret.y /= nMagnitude;
		ret.z /= nMagnitude;
	}

	return ret;
}
