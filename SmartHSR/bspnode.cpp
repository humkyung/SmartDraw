#include "stdafx.h"
#include <stdio.h>
#include "bspnode.h"

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CBSPNode::CBSPNode(){
	m_pParent = front = back = NULL;
	m_pSameDir= m_pOppDir  = NULL;
	m_pNegSide= m_pPosSide = NULL;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CBSPNode::~CBSPNode(){
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CBSPNode *CBSPNode::GetNegNode(){
	return m_pNegSide;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CBSPNode *CBSPNode::GetPosNode(){
	return m_pPosSide;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CBSPNode::SetParent(CBSPNode *parent){
	m_pParent = parent;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CBSPNode::SetNegNode(CBSPNode *neg){
	m_pNegSide = neg;
	if(neg) neg->SetParent(this);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CBSPNode::SetPosNode(CBSPNode *pos){
	m_pPosSide = pos;
	if(pos) pos->SetParent(this);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CBSPNode::SetPartitionFaceList(CHSRFace* SameDir,CHSRFace* OppDir){
	m_pSameDir = SameDir;
	m_pOppDir  = OppDir;
}

