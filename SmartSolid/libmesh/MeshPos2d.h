// MeshPos2d.h: interface for the CMeshPos2d class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHPOS2D_H__46175B99_DA2C_451F_BA1E_56B7B123AA0A__INCLUDED_)
#define AFX_MESHPOS2D_H__46175B99_DA2C_451F_BA1E_56B7B123AA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQLib.h>

namespace mesh{
	
class CMeshPos2d  
{
public:
	void SetZ(const double& z);
	double z() const;
	bool operator ==(CMeshPos2d pos);
	POINT_T point();
	bool operator =(POINT_T pos);
	void SetY(const double& y);
	void SetX(const double& x);
	double y();
	double x();
	void Set(const double& dx,const double& dy , const double& dz);
	CMeshPos2d(const CMeshPos2d& pos);
	CMeshPos2d(POINT_T pos);
	CMeshPos2d(const double& x,const double& y , const double& dz);
	CMeshPos2d();
	virtual ~CMeshPos2d();
private:
	double m_x,m_y;
	double m_z;
};
//typedef CMeshPos2d POS_T;
//typedef CMeshPos2d *PPOS_T;

}

#endif // !defined(AFX_MESHPOS2D_H__46175B99_DA2C_451F_BA1E_56B7B123AA0A__INCLUDED_)
