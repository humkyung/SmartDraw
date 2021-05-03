/********************************************************************
	created:	
	created:	
	filename: 	d:\Project\ITK\Math\KMath.h
	file path:	d:\Project\ITK\Math
	file base:	KMath
	file ext:	h
	author:		
	
	purpose:	- mathematics operation
	2002.09.12	- add quaternion operation
	2002.12.3	- times of two quaternions and inverse of quaternion.
*********************************************************************/

#ifndef __MATH_H__
#define __MATH_H__

#ifndef	DLL_EXPORT
#define	DLL_EXPORT __declspec(dllexport)
#endif

#include <graphics\line.h>
#include <math\quat.h>
#include "..\..\SQStruct.h"
using namespace itk;

class DLL_EXPORT CMath{
public:
	static bool IsGreaterEqualThan(const double val1,const double val2);
	static double round(double a,int b);
	static double GetInterpolationZ(LINE_T& line,POINT_T& pt);
	
	/*
	VECTOR_T operator -(const POINT_T pt1,const POINT_T pt2);
	VECTOR_T operator +(const VECTOR_T vec1,const VECTOR_T vec2);
	static VECTOR_T operator -(const VECTOR_T vec1,const VECTOR_T vec2);
	static VECTOR_T operator *(const VECTOR_T vec,const double nValue);
	static VECTOR_T operator *(const double nValue,const VECTOR_T vec);
	*/
	static bool NormalizeVector(double ii,double jj,double kk, double *ii2,double *jj2,double *kk2);
	static bool NormalizeVector(VECTOR_T& vec);
	static double DotProduct(double vtx1,double vty1,double vtz1,double vtx2,double vty2,double vtz2);
	static double DotProduct(const VECTOR_T& vec1,const VECTOR_T& vec2);
	static void   GetCrossProduct(double ii1,double jj1,double kk1, double ii2,double jj2,double kk2,
		double *iicp,double *jjcp,double *kkcp);
	static void GetCrossProduct(VECTOR_T& vec,const VECTOR_T vec1,const VECTOR_T vec2);
	static VECTOR_T CrossProduct(const VECTOR_T vec1,const VECTOR_T vec2);
	
	static double GetAngle(VECTOR_T& vec1,VECTOR_T& vec2);
	
	static bool GetMatrixFromQuaternion(PSQMATRIX ret,QUAT_T quat);
};

#endif
