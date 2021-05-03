// MeshEdge.cpp: implementation of the CTinyEdge class.
//
//////////////////////////////////////////////////////////////////////

#include "MeshEdge.h"
using namespace mesh;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTinyEdge::CTinyEdge()
{

}

CTinyEdge::~CTinyEdge()
{

}

bool CTinyEdge::IsValid()
{
	const static double nTol=0.000000001;
	
	double du=pos[1].x() - pos[0].x();
	double dv=pos[1].y() - pos[0].y();
	if((0. == du) && (0. == dv)) return false;
	
	return true;	
}

bool CTinyEdge::operator ==(CTinyEdge edge){
	return true;
}