// ViewBoundary.h: interface for the CViewBoundary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWBOUNDARY_H__BF2A691B_B12D_4CF4_BBE3_D21F73CA4890__INCLUDED_)
#define AFX_VIEWBOUNDARY_H__BF2A691B_B12D_4CF4_BBE3_D21F73CA4890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <IsPoint3d.h>
#include <IsLine3d.h>
#include <IsVect3d.h>
#include "aDraw_Entity.h"

#define	SHOW	1
#define	HIDE	2

#define	LINE_VBS	0x01
#define	ARC_VBS		0x02

#define	BOTTOM_SIDE	0
#define	LEFT_SIDE	1
#define	TOP_SIDE	2
#define	RIGHT_SIDE	3

#define	VIEW_MATCH_BUF_SIZ	128

class CaDraw_View;
class ON_MODULE_EXT_DLL CViewBoundaySegment
{
public:
	CViewBoundaySegment();
	~CViewBoundaySegment();

	bool IsEnable(AnnoFilters* ) const;

	CIsPoint3d	pt;
	CIsPoint3d	ptCenterOfQuad;
	int		render;

	CIsLine3d	edge;
	double		nHeight[2];	/// 0 = Line No , 1 = Other
	CIsVect3d	vecTagDir;
	TCHAR		szViewMatchLine[VIEW_MATCH_BUF_SIZ + 1];	/// match line data
	TCHAR		szDim[2][64];				/// view coordinate text
	bool		bOpen[2];
	int		nType;							/// LINE_VBS or ARC_VBS
	int	    nDivisionFlag;					/// DIVISION_ON or DIVISION_OFF
	CaDraw_View*	pView;

	CViewBoundaySegment* next;
};

class ON_MODULE_EXT_DLL CViewBoundary : public CaDraw_Entity
{
public:
	CViewBoundary();
	virtual ~CViewBoundary();

	ADRAW_DECLARE_FUNC()
public:
	bool IsEnable(AnnoFilters* ) const;	/// 2016.03.19 added by humkyung

	bool operator +=(CViewBoundaySegment* pVBS);
	bool Split(CViewBoundary* pFace);
	bool IsHiddenLine(CViewBoundaySegment* vt1,CViewBoundaySegment* vt2 , const double& nToler);
private:
	bool IntersectLine(double* x,double* y,double x1,double y1,double x2,double y2,double x3,double y3,double x4,double y4,double tol=0.0);
	bool IsInnerPoint(double x,double y,CViewBoundaySegment* pvt1,CViewBoundaySegment* pvt2);
	CViewBoundaySegment* SplitViewSegment(CViewBoundaySegment* vt1,CViewBoundaySegment* vt2);
	bool IsInnerVertex(CIsPoint3d* pt);
public:
	CViewBoundaySegment* m_pVBS;
};

#endif // !defined(AFX_VIEWBOUNDARY_H__BF2A691B_B12D_4CF4_BBE3_D21F73CA4890__INCLUDED_)
