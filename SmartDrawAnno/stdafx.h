// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include "CellMap.h"
#include <string>
using namespace std;

#define _CRT_SECURE_NO_WARNINGS	1

#define TAG_BUF_SIZE		64
#define	MAX_GROUP_ITEM_COUNT	10

extern bool fn_Len_Sort(const CCellMap::SearchRes& lhs, const CCellMap::SearchRes& rhs);
extern bool fn_Cnt_Sort(const CCellMap::SearchRes& lhs, const CCellMap::SearchRes& rhs);

extern STRING_T GetExecPath(void);
extern int ccw(const  CIsPoint2d* A , const CIsPoint2d* B , const CIsPoint2d* C);
#ifdef ADRAW_TAG_EXPORTS
extern "C" __declspec(dllexport)
void SutherlandHodgmanPolygonClip(vector<CIsPoint2d>& input , vector<CIsPoint2d>& clipPolygon , vector<CIsPoint2d>& output);
extern "C" __declspec(dllexport)
int intersect(CIsPoint2d* A , CIsPoint2d* B,CIsPoint2d clipEdge[2] , CIsPoint2d* intPt);
#else
extern "C" __declspec(dllimport)
void SutherlandHodgmanPolygonClip(vector<CIsPoint2d>& input , vector<CIsPoint2d>& clipPolygon , vector<CIsPoint2d>& output);
extern "C" __declspec(dllimport)
int intersect(CIsPoint2d* A , CIsPoint2d* B,CIsPoint2d clipEdge[2] , CIsPoint2d* intPt);
#endif
