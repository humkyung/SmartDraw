#ifndef __BSPNODE_H__
#define __BSPNODE_H__

#include "HSRFace.h"

class CBSPNode{
public:
	CBSPNode();
	~CBSPNode();
public:
	CBSPNode *GetNegNode();
	CBSPNode *GetPosNode();

	void SetParent(CBSPNode *parent);
	void SetPartitionFaceList(CHSRFace* SameDir,CHSRFace* OppDir);
	void SetNegNode(CBSPNode *neg);
	void SetPosNode(CBSPNode *po);

	int  GetNumOfElement();
public:
	CHSRFace *m_pSameDir,*m_pOppDir;
	CBSPNode *m_pParent,*front,*back,*m_pNegSide,*m_pPosSide;
protected:
};

#endif