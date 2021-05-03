// DGNPoint.h: interface for the CTyPoint3d class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TYPOINT3D_H__359ECD24_4439_401E_839D_8ADCD166C167__INCLUDED_)
#define AFX_TYPOINT3D_H__359ECD24_4439_401E_839D_8ADCD166C167__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __DLL_EXPORT__
#define __DLL_EXPORT__	__declspec(dllexport)
#endif

class __DLL_EXPORT__ CTyPoint3d  
{
public:
	CTyPoint3d(const double& _x,const double& _y,const double& _z);
	CTyPoint3d();
	virtual ~CTyPoint3d();
public:
	double x;
	double y;
	double z;
};

#endif // !defined(AFX_TYPOINT3D_H__359ECD24_4439_401E_839D_8ADCD166C167__INCLUDED_)
