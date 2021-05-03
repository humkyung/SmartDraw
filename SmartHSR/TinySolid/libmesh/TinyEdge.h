// MeshEdge.h: interface for the CTinyEdge class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHEDGE_H__91B27C96_E71B_49A3_80A5_36690DDF0985__INCLUDED_)
#define AFX_MESHEDGE_H__91B27C96_E71B_49A3_80A5_36690DDF0985__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MeshPos2d.h"
namespace mesh{

typedef class CTinyEdge  
{
public:
	CTinyEdge();
	virtual ~CTinyEdge();
public:
	bool operator ==(CTinyEdge edge);
	bool IsValid();
	CMeshPos2d pos[2];

}EDGE_T,*PEDGE_T;

};
#endif // !defined(AFX_MESHEDGE_H__91B27C96_E71B_49A3_80A5_36690DDF0985__INCLUDED_)
