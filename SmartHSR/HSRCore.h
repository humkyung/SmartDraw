// KHiddenCore.h: interface for the KHiddenCore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KHIDDENCORE_H__2C78A968_6D4F_431A_9C3C_C8F51095A511__INCLUDED_)
#define AFX_KHIDDENCORE_H__2C78A968_6D4F_431A_9C3C_C8F51095A511__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SQLib.h"

#include "bspnode.h"
#include "BSPTree.h"

#include <list>
#include "HSRFace.h"	// Added by ClassView
using namespace std;

class CHSRCore{
public:
	CHSRCore();
	~CHSRCore();
public:
	void CreateRenderList(CBSPNode *pNode);
	void ExtractHiddenLines();
	void RenderList2FaceList();
	void UnionFaces();


	void Clear();
private:
	void CalculateRegion(CHSRFace* pFace,const RECT_T& rectView);
	void CreatePositiveOrderRenderList(CBSPNode* pNode);
	void CreateNegativeOrderRenderList(CBSPNode* pNode);
	void HideFace(CHSRFace* pFace,PRENDERNODE pRenderList);
public:
	PRENDERNODE m_pRenderList;
	CHSRFace*   m_pMergeFace;

	CBSPTree    m_BSPTree;
private:
	list<PRENDERNODE> m_renderNodePool;
protected:
	PRENDERNODE NewRenderNode();
};

#endif // !defined(AFX_KHIDDENCORE_H__2C78A968_6D4F_431A_9C3C_C8F51095A511__INCLUDED_)
