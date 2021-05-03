// CGrid.h: interface for the CGrid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CGrid_H__DF81CCB9_3050_4DF9_9DD8_1061F893C7B9__INCLUDED_)
#define AFX_CGrid_H__DF81CCB9_3050_4DF9_9DD8_1061F893C7B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQlib.h>

using namespace node;

class CGrid  
{
	typedef snode<POINT_T> _node;
	CGrid(const CGrid& rhs){}
	CGrid& operator=(const CGrid& rhs){ return (*this);}
public:
	CGrid();
	CGrid(const RECT_T& rect);
	CGrid(const POINT_T pt[]);
	
	virtual ~CGrid();
public:
	bool IsOnApex(const POINT_T& pt);
	int  IsOnBoundary(const POINT_T& pt);
	bool IsInnerPoint(const POINT_T& pt);
	bool IsDiagonalLine(const LINE_T& line);
	bool IsLeftSide(const int& nIndex,const LINE_T& line);
	bool IsValid();
	int  HasEdge(const LINE_T& line);
	bool InsertPoint(list<CGrid*>& newGridList , const POINT_T& pt);
	
	CGrid* Split(const LINE_T& line);
	int DoIncluding(const LINE_T& line);
	bool DoIntersection(POINT_T& pt,const LINE_T& line);
private:
	void CalcRect();
	RECT_T m_rect;
public:
	RECT_T rect() const;
	snode<POINT_T>* m_pNode;
	bool m_bDeleted;

	unsigned long m_nId;
	static unsigned long ID;
};

#endif // !defined(AFX_CGrid_H__DF81CCB9_3050_4DF9_9DD8_1061F893C7B9__INCLUDED_)
