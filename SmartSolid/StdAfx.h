// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__66C94F98_0093_4234_A4DC_26FBB840CD0D__INCLUDED_)
#define AFX_STDAFX_H__66C94F98_0093_4234_A4DC_26FBB840CD0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <graphics\geometry.h>
#include "./libmesh/libmesh2d.h"
using namespace mesh;

// TODO: reference additional headers your program requires here
extern bool IsValidEdge(EDGE_T edge);
extern bool IsSameEdge(EDGE_T edge1,EDGE_T edge2);

extern RECT_T GetRectOf(const LINE_T& line);
extern RECT_T GetRectOf(const int& count , POINT_T* pts);
extern bool CollideWith(const RECT_T& lhs , const RECT_T& rhs);

#define	MAX_OPENSOLID_PRECISION	9

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__66C94F98_0093_4234_A4DC_26FBB840CD0D__INCLUDED_)
