// CGridMethod.h: interface for the CGridMethod class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CGridMETHOD_H__0241DFD6_3105_425A_A603_6E0EB82ED4A7__INCLUDED_)
#define AFX_CGridMETHOD_H__0241DFD6_3105_425A_A603_6E0EB82ED4A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQLib.h>
#include "grid.h"

#include <list>
using namespace std;

class CGridMethod  
{
public:
	CGridMethod();
	virtual ~CGridMethod();
public:
	void Initialize();

	void InsertGrid(CGrid* pGrid);
	void InsertPoint(const POINT_T& pt);
	snode<LINE_T>* InsertEdge(snode<LINE_T>* pEdge);
public:
	list<CGrid*> m_lstGrid;
};

#endif // !defined(AFX_CGridMETHOD_H__0241DFD6_3105_425A_A603_6E0EB82ED4A7__INCLUDED_)
