// DelaunayEdge.h: interface for the CDelaunayEdge class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DELAUNAYEDGE_H__14876E34_3C52_4ED0_9523_077AF4A49A85__INCLUDED_)
#define AFX_DELAUNAYEDGE_H__14876E34_3C52_4ED0_9523_077AF4A49A85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MeshPos2d.h"
using namespace mesh;

class CDelaunayEdge  
{
public:
	CDelaunayEdge();
	virtual ~CDelaunayEdge();
public:
	int ref;
	CMeshPos2d pt[2];
};

#endif // !defined(AFX_DELAUNAYEDGE_H__14876E34_3C52_4ED0_9523_077AF4A49A85__INCLUDED_)
