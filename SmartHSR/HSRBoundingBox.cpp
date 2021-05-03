// HSRBoundingBox.cpp: implementation of the CHSRBoundingBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HSRBoundingBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHSRBoundingBox::CHSRBoundingBox()
{
	m_bEmpty = true;
}

CHSRBoundingBox::CHSRBoundingBox(const double minx, const double miny, const double minz, const double maxx, const double maxy, const double maxz)
{
	m_nMins[X] = minx;m_nMins[Y] = miny;m_nMins[Z] = minz;
	m_nMaxs[X] = maxx;m_nMaxs[Y] = maxy;m_nMaxs[Z] = maxz;
	m_bEmpty = false;
}

CHSRBoundingBox::~CHSRBoundingBox()
{

}

bool CHSRBoundingBox::operator +=(CHSRBoundingBox &bbox)
{
	if(true == m_bEmpty){
		memcpy(m_nMins,bbox.m_nMins,sizeof(double)*3);
		memcpy(m_nMaxs,bbox.m_nMaxs,sizeof(double)*3);
		
		m_bEmpty = false;
	}else{
		if(bbox.m_nMins[X] < m_nMins[X]) m_nMins[X] = bbox.m_nMins[X];
		if(bbox.m_nMaxs[X] > m_nMaxs[X]) m_nMaxs[X] = bbox.m_nMaxs[X];
		if(bbox.m_nMins[Y] < m_nMins[Y]) m_nMins[Y] = bbox.m_nMins[Y];
		if(bbox.m_nMaxs[Y] > m_nMaxs[Y]) m_nMaxs[Y] = bbox.m_nMaxs[Y];
		if(bbox.m_nMins[Z] < m_nMins[Z]) m_nMins[Z] = bbox.m_nMins[Z];
		if(bbox.m_nMaxs[Z] > m_nMaxs[Z]) m_nMaxs[Z] = bbox.m_nMaxs[Z];
	}

	return true;
}

inline const double CHSRBoundingBox::GetMax(const int nIndex)
{
	return m_nMaxs[nIndex];
}

inline const double CHSRBoundingBox::GetMin(const int nIndex)
{
	return m_nMins[nIndex];
}

bool CHSRBoundingBox::IsEmpty()
{
	return m_bEmpty;
}

void CHSRBoundingBox::SetEmpty()
{
	m_bEmpty = true;
}

CHSRBoundingBox::CHSRBoundingBox(const CHSRBoundingBox &bbox)
{
	m_bEmpty = bbox.m_bEmpty;
	memcpy(m_nMins,bbox.m_nMins,sizeof(double)*3);
	memcpy(m_nMaxs,bbox.m_nMaxs,sizeof(double)*3);
}

bool CHSRBoundingBox::Collide(const CHSRBoundingBox &bbox)
{
	if(m_nMins[0] > bbox.m_nMaxs[0]) return false;
	if(m_nMins[1] > bbox.m_nMaxs[1]) return false;
	if(m_nMins[2] > bbox.m_nMaxs[2]) return false;
	if(m_nMaxs[0] < bbox.m_nMins[0]) return false;
	if(m_nMaxs[1] < bbox.m_nMins[1]) return false;
	if(m_nMaxs[2] < bbox.m_nMins[2]) return false;

	return true;
}
