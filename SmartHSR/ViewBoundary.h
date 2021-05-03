// ViewBoundary.h: interface for the CViewBoundary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWBOUNDARY_H__1547AEEC_1481_46FB_B39E_511ADE7DD001__INCLUDED_)
#define AFX_VIEWBOUNDARY_H__1547AEEC_1481_46FB_B39E_511ADE7DD001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "HSRFace.h"
#include "HSRVIEWFormat.h"
#include <list>
using namespace std;

class CViewBoundary  
{
public:
	CViewBoundary();
	virtual ~CViewBoundary();
public:
	PLINE_T GetViewBoundary(){return m_aryViewBoundary;}
	int  GetNumOfViewBoundaryEdge(){return m_nViewBoundaryEdge;}
	bool operator+=(CHSRFace* pViewFace);
	bool operator+=(CHSRViewFormat* pView);
	void ComputateViewBoundary();
private:
	bool IsInnerPoint(double x,double y,PHSR_VERTEX pvt1,PHSR_VERTEX pvt2);
	bool IntersectLine(double* x,double* y,double x1,double y1,double x2,double y2,double x3,double y3,double x4,double y4,double tol=0.);
	bool    Split(CHSRFace* pViewBoundary1,CHSRFace* pViewBoundary2);
	PHSR_VERTEX SplitViewSegment(CHSRFace* pViewBoundary,PHSR_VERTEX vt1,PHSR_VERTEX vt2);
	bool IsHiddenLine(PHSR_VERTEX vt1,PHSR_VERTEX vt2,CHSRFace* pFace);
	bool IsInnerVertex(PPOINT_T pt,CHSRFace* pFace);
	void RemoveSameViewBoundary();
	void JoinSeparateViewLine();
	bool IsSamePoint(const POINT_T& pt1,const POINT_T& pt2);
	bool IsEqualLine(const LINE_T& line1,const LINE_T& line2);
public:
	int    m_nViewBoundaryEdge;
	LINE_T m_aryViewBoundary[100];
private:
	list<CHSRFace*> m_lstViewBoundary;
	list<CHSRViewFormat*> m_lstView;
};

#endif // !defined(AFX_VIEWBOUNDARY_H__1547AEEC_1481_46FB_B39E_511ADE7DD001__INCLUDED_)
