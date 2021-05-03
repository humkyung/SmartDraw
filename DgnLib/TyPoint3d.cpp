// DGNPoint.cpp: implementation of the CTyPoint3d class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TyPoint3d.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTyPoint3d::CTyPoint3d(const double& _x,const double& _y,const double& _z)
{
	x = _x;
	y = _y;
	z = _z;
}

CTyPoint3d::CTyPoint3d()
{
	x = y = z = 0.f;
}

CTyPoint3d::~CTyPoint3d()
{

}