// TinyConeSurfTester.h: interface for the CTinyConeSurfTester class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TINYCONESURFTESTER_H__48252B93_D951_448E_BAFB_7EE2390E2E53__INCLUDED_)
#define AFX_TINYCONESURFTESTER_H__48252B93_D951_448E_BAFB_7EE2390E2E53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "libmesh\grid.h"
#include <list>
using namespace std;

class CTinyConeSurfTester  
{
public:
	void OutputTestString(list<CGrid*>& lstGrid , snode<LINE_T>* pEdge);
	int m_nRows;
	int m_nBoundaries;
	int m_nSegments;
	CTinyConeSurfTester(bool bOutput , const int& surf_num);
	virtual ~CTinyConeSurfTester();
private:
	bool m_bOutput;
	int m_surf_num;
};

#endif // !defined(AFX_TINYCONESURFTESTER_H__48252B93_D951_448E_BAFB_7EE2390E2E53__INCLUDED_)
