/********************************************************************
	created:	2003/08/09
	created:	9:8:2003   13:03
	filename: 	D:\Project\ITK\include\Math\quat.h
	file path:	D:\Project\ITK\include\Math
	file base:	quat
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __QUAT_H__
#define __QUAT_H__

namespace itk{
	// q = w + xi + yj + zk
	typedef struct tagQuat{
		double w;	// imaginary part
		double x;	// real part
		double y;	
		double z;
	}QUAT_T,* PQUAT_T;
};

extern itk::QUAT_T __declspec(dllexport) operator *(const itk::QUAT_T quat1,const itk::QUAT_T quat2);
extern itk::QUAT_T __declspec(dllexport) Inverse(const itk::QUAT_T quat);

#endif
