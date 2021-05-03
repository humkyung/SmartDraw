// MeshPos2d.cpp: implementation of the CMeshPos2d class.
//
//////////////////////////////////////////////////////////////////////

#include "libmesh2d.h"
#include "MeshPos2d.h"
using namespace mesh;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeshPos2d::CMeshPos2d()
{
	m_x = m_y = 0.;
}

CMeshPos2d::~CMeshPos2d()
{

}

CMeshPos2d::CMeshPos2d(const double &dx, const double &dy , const double& dz)
{
	m_x = _round(dx,LIBMESH2D_PRECISION);
	m_y = _round(dy,LIBMESH2D_PRECISION);
	m_z = _round(dz,LIBMESH2D_PRECISION);
}

CMeshPos2d::CMeshPos2d(POINT_T pos)
{
	m_x = _round(pos.x,LIBMESH2D_PRECISION);
	m_y = _round(pos.y,LIBMESH2D_PRECISION);
	m_z = _round(pos.z,LIBMESH2D_PRECISION);
}

CMeshPos2d::CMeshPos2d(const CMeshPos2d& pos)
{
	m_x = pos.m_x;
	m_y = pos.m_y;
	m_z = pos.m_z;
}

void CMeshPos2d::Set(const double &dx, const double &dy , const double& dz)
{
	m_x = _round(dx,LIBMESH2D_PRECISION);
	m_y = _round(dy,LIBMESH2D_PRECISION);
	m_z = _round(dz,LIBMESH2D_PRECISION);
}

double CMeshPos2d::x()
{
	return m_x;
}

double CMeshPos2d::y()
{
	return m_y;
}

void CMeshPos2d::SetX(const double &x)
{
	m_x = _round(x,LIBMESH2D_PRECISION);
}

void CMeshPos2d::SetY(const double &y)
{
	m_y = _round(y,LIBMESH2D_PRECISION);
}

bool CMeshPos2d::operator =(POINT_T pos)
{
	m_x = _round(pos.x,LIBMESH2D_PRECISION);
	m_y = _round(pos.y,LIBMESH2D_PRECISION);
	m_z = _round(pos.z,LIBMESH2D_PRECISION);

	return true;
}

POINT_T CMeshPos2d::point()
{
	POINT_T pos={0,};

	pos.x = m_x;
	pos.y = m_y;
	pos.z = m_z;

	return pos;
}

bool CMeshPos2d::operator ==(CMeshPos2d pos)
{
	if((m_x == pos.m_x) && (m_y == pos.m_y)) return true;

	return false;
}

double CMeshPos2d::z() const
{
	return m_z;
}

void CMeshPos2d::SetZ(const double &z)
{
	m_z = _round(z,LIBMESH2D_PRECISION);
}
