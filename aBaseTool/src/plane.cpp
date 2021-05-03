/********************************************************************
	created:	2003/01/08
	created:	8:1:2003   11:45
	filename: 	D:\Project\Sqlib\CPlane.cpp
	file path:	D:\Project\Sqlib
	file base:	CPlane
	file ext:	cpp
	author:		
	
	purpose:	
*********************************************************************/
#include "math\itkmath.h"
#include "..\include\Plane.h"

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CPlane::CPlane(const SQVECTOR vecNorm,const POINT_T pt){
	a = vecNorm.dx;b = vecNorm.dy;c = vecNorm.dz;
	d = -a*pt.x - b*pt.y - c*pt.z;
}

CPlane::~CPlane(){}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
SQPOINT CPlane::Project(const POINT_T pt){
	POINT_T ptOn={0};
	double t=d/(a*a + b*b + c*c);
	ptOn.x = a*t;ptOn.y = b*t;ptOn.z = c*t;

	SQVECTOR vec={0},vecNorm={a,b,c};
	vec.dx = ptOn.x - pt.x;vec.dy = ptOn.y - pt.y;vec.dz = ptOn.z - pt.z;
	CMath::NormalizeVector(vecNorm);
	double nLen=CMath::DotProduct(vec,vecNorm);

	ptOn.x = pt.x + vecNorm.dx*nLen;
	ptOn.y = pt.y + vecNorm.dy*nLen;
	ptOn.z = pt.z + vecNorm.dz*nLen;

	return ptOn;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
SQVECTOR CPlane::Project(const SQVECTOR vec){
	SQVECTOR vecRet={0};

	SQVECTOR vecNorm={a,b,c};
	CMath::NormalizeVector(vecNorm);
	double nLen=CMath::DotProduct(vec,vecNorm);

	vecRet.dx = vec.dx - vecNorm.dx*nLen;
	vecRet.dy = vec.dy - vecNorm.dy*nLen;
	vecRet.dz = vec.dz - vecNorm.dz*nLen;

	return vecRet;
}
