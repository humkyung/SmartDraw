#ifndef __SQCURVE_H__
#define __SQCURVE_H__

#include <graphics\line.h>

typedef struct tagSQPolygon{
	LINE_T edge;

	struct tagSQPolygon *next;
}SQPOLYGON,* PSQPOLYGON;

typedef struct tagSQLineX{
	char    aryArrow[2];
	POINT_T ptStart;
	POINT_T ptEnd;
}LINE_TX,* PLINE_TX;

//	interpolate curve
typedef struct tagIntCurve{
    	SQVECTOR vecNorm;
	int      nPoints;
	POINT_T* ppt;
}SQINTCURVE,* PSQINTCURVE;

//	parametric curve
typedef struct tagCurve{
	int      nDegree;
	int      nPoints;
	PPOINT_T ppt;
}SQCURVE,* PSQCURVE;

typedef struct tagBSPLINE{
	int      nDegree;
	int      nPoints;
	double*  knots;
	PPOINT_T ppt;
}SQBSPLINE,* PSQBSPLINE;

typedef struct tagNurbs{
    	unsigned int nDegree;
	unsigned int nPoints;
	double* pKnots;
	typedef struct tagWPOINT{
		double  weight;
		POINT_T pt;
	}WPOINT;
	WPOINT* ppt;
}NURBS,* PSQNURBS;

#endif
