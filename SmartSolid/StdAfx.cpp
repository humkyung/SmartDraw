// stdafx.cpp : source file that includes just the standard includes
//	OpenSolid.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

/**
*/
RECT_T GetRectOf(const LINE_T& line)
{
	double minx = line.ptStart.x , miny = line.ptStart.y;
	double maxx = line.ptStart.x , maxy = line.ptStart.y;

	if(line.ptEnd.x < minx) minx = line.ptEnd.x;
	if(line.ptEnd.y < miny) miny = line.ptEnd.y;
	if(line.ptEnd.x > maxx) maxx = line.ptEnd.x;
	if(line.ptEnd.y > maxy) maxy = line.ptEnd.y;

	RECT_T rect;
	rect.x = minx;
	rect.y = miny;
	rect.width = maxx - minx;
	rect.height= maxy - miny;

	return rect;
}

/**
*/
RECT_T GetRectOf(const int& count , POINT_T* pts)
{
	RECT_T rect;
	rect.x = rect.y = 0;
	rect.width = rect.height = -1;

	if(count > 0)
	{
		double minx = pts[0].x , miny = pts[0].y;
		double maxx = pts[0].x , maxy = pts[0].y;

		for(int i = 0;i < count;++i)
		{
			if(pts[i].x < minx) minx = pts[0].x;
			if(pts[i].y < miny) miny = pts[0].y;
			if(pts[i].x > maxx) maxx = pts[0].x;
			if(pts[i].y > maxy) maxy = pts[0].y;
		}

		rect.x = minx;
		rect.y = miny;
		rect.width = maxx - minx;
		rect.height= maxy - miny;
	}

	return rect;
}

/**
	@brief	두 rect가 겹치는지 여부를 리턴한다.

	@author	백흠경
*/
bool CollideWith(const RECT_T& lhs , const RECT_T& rhs)
{
	double minx[2] , miny[2] , maxx[2] , maxy[2];

	minx[0] = lhs.x;
	miny[0] = lhs.y;
	maxx[0] = lhs.x + lhs.width;
	maxy[0] = lhs.y + lhs.height;

	minx[1] = rhs.x;
	miny[1] = rhs.y;
	maxx[1] = rhs.x + rhs.width;
	maxy[1] = rhs.y + rhs.height;

	if(minx[0] > maxx[1]) return false;
	if(maxx[0] < minx[1]) return false;
	if(miny[0] > maxy[1]) return false;
	if(maxy[0] < miny[1]) return false;

	return true;
}