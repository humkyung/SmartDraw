/********************************************************************
	created:	2004/01/14
	created:	14:1:2004   12:03
	filename: 	D:\Project\PlantWAVE\OpenSolid\CCubicSpline.h
	file path:	D:\Project\PlantWAVE\OpenSolid
	file base:	CCubicSpline
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __OPEN3D_CUBICSPLINE_H__
#define __OPEN3D_CUBICSPLINE_H__

#include <SQLib.h>
#include <graphics/curve3d.h>

class CCubicSpline : public CCurve3d{
public:
	CCubicSpline();
	CCubicSpline(int nCount,POINT_T ptControl[]);
	CCubicSpline(const SQCURVE& CubicSpline);
	~CCubicSpline();
public:
	POINT_T operator()(int k,double u);
protected:
	void CreateSegments();
	void OnDestroy();
private:
	int m_nCount;
	
	double* m_pnCoord;
	double* m_pnDelta;

	POINT_T m_ptControl[256];
	POINT_T m_ptDerivative[256];

	double  m_nXu[256][4];
	double  m_nYu[256][4];
	double  m_nZu[256][4];
};

#endif