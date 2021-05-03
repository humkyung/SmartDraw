// HSRElementFactory.h: interface for the CHSRElementFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSRELEMENTFACTORY_H__2809E71E_D276_42FD_B998_78EA9B4ED97B__INCLUDED_)
#define AFX_HSRELEMENTFACTORY_H__2809E71E_D276_42FD_B998_78EA9B4ED97B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __DLL_EXPORT__
#define __DLL_EXPORT__
#endif

#include "StdAfx.h"
#include "HsrScanner.h"
#include "Element.h"

#include <map>
#include <queue>
using namespace std;

class __DLL_EXPORT__ CHSRElementFactory  
{
	CHSRElementFactory();
	CHSRElementFactory(const CHSRElementFactory& rhs){}
public:
	static CHSRElementFactory& GetInstance();
	virtual LPCTSTR GetKeyword(){return NULL;}
	virtual CHSRElement* ParseLine(CHSRScanner* pHSRScanner){return NULL;}
	virtual ~CHSRElementFactory();

	CHSRElement* CreateElement(const STRING_T& sTypeStr);
	int ReturnElement( CHSRElement* pElm );
private:
	map<STRING_T , queue<CHSRElement*>* >* m_pElementMap;
};

#endif // !defined(AFX_HSRELEMENTFACTORY_H__2809E71E_D276_42FD_B998_78EA9B4ED97B__INCLUDED_)
