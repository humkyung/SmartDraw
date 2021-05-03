// HSRPoint.cpp: implementation of the CHSRPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HSRPoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHSRPoint::CHSRPoint()
{
	this->x = this->y = this->z = 0.0;
	m_code = CHSRPoint::PRIMARY;
}

CHSRPoint::CHSRPoint(const CHSRPoint& rhs)
{
	(*this) = rhs;
}

CHSRPoint::CHSRPoint(const POINT_T& rhs)
{
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;

	m_code = 0;
}

CHSRPoint& CHSRPoint::operator =(const CHSRPoint& rhs)
{
	if(this != &rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;
		this->m_code = rhs.m_code;
	}

	return (*this);
}

CHSRPoint::~CHSRPoint()
{

}
