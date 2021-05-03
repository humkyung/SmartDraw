// KBSPTree.h: interface for the KBSPTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KBSPTREE_H__7C46B27C_7122_4B2A_8175_DA257DFE2842__INCLUDED_)
#define AFX_KBSPTREE_H__7C46B27C_7122_4B2A_8175_DA257DFE2842__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(push)
#include <math.h>
#pragma warning(disable: 4514)
#pragma warning(pop)

#include <boost/pool/object_pool.hpp>
#include <graphics\plane3d.h>
#include "hidden.h"
#include "bspnode.h"

#include <fstream>
#include <queue>
using namespace std;

class CBSPTree
{
public:
	CBSPTree();
	virtual ~CBSPTree();
public:
	CBSPNode* Construct(CHSRFace* *pFaceList);
	bool      IsViewerInPositiveSideOfPlane(const PLANE_T *PLANE_T,const PPOINT_T position);

private:
	HSR_VERTEX* AllocVertex(double xx,double yy,double zz);
	CHSRFace* AllocFace(HSR_VERTEX *vlist,const CHSRFace* face);

	SIGN_T AnyEdgeIntersectWithPlane(double x1,double y1,double z1,double x2,double y2,double z2,
			       const PLANE_T *PLANE_T,double *ixx, double *iyy, double *izz);

	void PartitionFaceListWithPlane(const PLANE_T *PLANE_T,CHSRFace**faceList,CHSRFace**faceNeg, CHSRFace **facePos,CHSRFace **faceSameDir, CHSRFace **faceOppDir);
	HSR_VERTEX* FindNextIntersection(HSR_VERTEX *vstart,const PLANE_T* plane,double *ixx,double *iyy,double *izz,SIGN_T *sign);
	SIGN_T WhichSideIsFaceWRTPlane(CHSRFace *face, const PLANE_T* plane);
	bool ChoosePartitionPlane(CHSRFace *faceList,PLANE_T* plane);
	bool DoesFaceStraddlePlane(const CHSRFace *face, const PLANE_T* plane);

	CHSRFace* CreateOtherFace(CHSRFace *face, 
			     HSR_VERTEX *v1, double ixx1, double iyy1, double izz1,
			     HSR_VERTEX *v2, double ixx2, double iyy2, double izz2
			     );

	void FreeVertexList(HSR_VERTEX **vlist);
public:
	CBSPNode *m_pNode;
private:
	boost::object_pool<CBSPNode>* m_pBSPNodePool;
	list<CBSPNode*> m_lstBSPNode;
protected:
	void Clear();
	CBSPNode* NewBSPNode();
};

#endif // !defined(AFX_KBSPTREE_H__7C46B27C_7122_4B2A_8175_DA257DFE2842__INCLUDED_)
