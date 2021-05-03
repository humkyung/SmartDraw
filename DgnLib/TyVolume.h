// TyVolume.h: interface for the CTyVolume class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TYVOLUME_H__68B8FEB0_93B8_4503_8299_2267D7A5ECB1__INCLUDED_)
#define AFX_TYVOLUME_H__68B8FEB0_93B8_4503_8299_2267D7A5ECB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TyPoint3d.h"

#ifndef __DLL_EXPORT__
#define __DLL_EXPORT__	__declspec(dllexport)
#endif

class __DLL_EXPORT__ CTyVolume  
{
public:
	void SetMaxPoint(const CTyPoint3d& point);
	void SetMinPoint(const CTyPoint3d& point);
	CTyPoint3d GetMaxPoint();
	CTyPoint3d GetMinPoint();
	CTyVolume(const CTyPoint3d& ptMin,const CTyPoint3d& ptMax);
	CTyVolume();
	virtual ~CTyVolume();
private:
	CTyPoint3d m_minPoint;
	CTyPoint3d m_maxPoint;
};

#endif // !defined(AFX_TYVOLUME_H__68B8FEB0_93B8_4503_8299_2267D7A5ECB1__INCLUDED_)
