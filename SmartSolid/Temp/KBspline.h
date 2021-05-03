// KBspline.h: interface for the KBspline class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KBSPLINE_H__2CC52268_FFAD_405F_8813_FBAA4AF9ADAC__INCLUDED_)
#define AFX_KBSPLINE_H__2CC52268_FFAD_405F_8813_FBAA4AF9ADAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQLib.h>
#include "KCurve.h"

class KBspline : public SQBSPLINE,public KCurve
{
public:
	KBspline();
	KBspline(const int nDegree,const int nPoints,SQPOINT* ppt);
	KBspline(const SQBSPLINE&  bspline);
	KBspline(const KBspline& bspline);
	virtual ~KBspline();
public:
	int  GetNumOfCtrlPoints(){return nPoints;}
	int  GetNumOfDegree(){return nDegree;}
	void SetKnotVector(){}
	void CreateSegments(){};
	SQPOINT operator()(double u);
protected:
	int  FindKnotSpan(double u);
	void ComputateBasisFuncs(int index,double u);
private:
	double* m_pN;
};

#endif // !defined(AFX_KBSPLINE_H__2CC52268_FFAD_405F_8813_FBAA4AF9ADAC__INCLUDED_)
