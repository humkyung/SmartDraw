#include "StdAfx.h"
#include <IsPoint2d.h>

int ccw(const CIsPoint2d* A , const CIsPoint2d* B , const CIsPoint2d* C)
{
	return ((B->x()-A->x())*(C->y() - B->y()) - (B->y() - A->y())*(C->x() - B->x()));
}

bool inside(CIsPoint2d* testVertex , CIsPoint2d clipEdge[2])
{
	return ((ccw(testVertex , &clipEdge[0] , &clipEdge[1])) >= 0);
}

/**
	@brief	두 직선의 교점을 구한다.

	@author	

	@date	2010.??.??

	@return
*/
int intersect(CIsPoint2d* A , CIsPoint2d* B,CIsPoint2d clipEdge[2] , CIsPoint2d* intPt)
{
	CIsPoint2d* C = &clipEdge[0];
	CIsPoint2d* D = &clipEdge[1];
	double det = (B->x() - A->x())*(D->y() - C->y()) - (B->y() - A->y())*(D->x() - C->x());

	if(0.f != det)
	{
		const double r = ((A->y() - C->y())*(D->x() - C->x()) - (A->x() - C->x())*(D->y() - C->y())) / det;
		intPt->Set(A->x() + r*(B->x() - A->x()) , A->y() + r*(B->y() - A->y()));
		if((r >= 0.0) && (r <= 1.0))
		{
			return ERROR_SUCCESS;
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

void SutherlandHodgmanPolyClip(CIsPoint2d* P, const int& n , CIsPoint2d clipEdge[2] , vector<CIsPoint2d>& outPut)
{
	outPut.clear();

	CIsPoint2d Q;
	CIsPoint2d* s = &(P[n-1]);
	for(int j = 0;j < n;++j)
	{
		CIsPoint2d* p = &(P[j]);
		if(inside(p , clipEdge))
		{
			if(inside(s , clipEdge))
				outPut.push_back(*p);
			else
			{
				intersect(s , p , clipEdge, &Q);
				outPut.push_back(Q);
				outPut.push_back(*p);
			}
		}
		else
		{
			if(inside(s , clipEdge))
			{
				intersect(s , p , clipEdge , &Q);
				outPut.push_back(Q);
			}
		}
		s = p;
	}
}

extern "C" __declspec(dllexport)
void SutherlandHodgmanPolygonClip(vector<CIsPoint2d>& polygon , vector<CIsPoint2d>& clipPolygon , vector<CIsPoint2d>& Q)
{
	if(polygon.size() < 2) return;

	Q.clear();

	CIsPoint2d E[2];
	vector<CIsPoint2d> P , T;
	P.insert(P.begin() , polygon.begin() , polygon.end());
	for(vector<CIsPoint2d>::iterator itr = clipPolygon.begin();(itr+1) != clipPolygon.end();++itr)
	{
		E[0] = *(itr);
		E[1] = *(itr+1);

		SutherlandHodgmanPolyClip(&(P[0]) , P.size() , E, Q);
		if(Q.empty()) break;

		P.clear();
		P.insert(P.begin() , Q.begin() , Q.end());
	}
}