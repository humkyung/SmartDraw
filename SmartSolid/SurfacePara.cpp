// SurfacePara.cpp: implementation of the CSurfacePara class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SurfacePara.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSurfacePara::CSurfacePara()
{
	m_u = m_v = 0.;
}

CSurfacePara::CSurfacePara(const double& u,const double& v)
{
	m_u = _round(u,LIBMESH2D_PRECISION);
	m_v = _round(v,LIBMESH2D_PRECISION);
}

CSurfacePara::~CSurfacePara()
{

}

double CSurfacePara::u()
{
	return m_u;
}

double CSurfacePara::v()
{
	return m_v;
}

void CSurfacePara::Set(const double &u, const double &v)
{
	m_u = _round(u,LIBMESH2D_PRECISION);
	m_v = _round(v,LIBMESH2D_PRECISION);
}
