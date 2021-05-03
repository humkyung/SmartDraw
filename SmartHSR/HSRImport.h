// KImport.h: interface for the KImport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KIMPORT_H__698B56FD_6EBC_4854_BCAE_7887A79B36A0__INCLUDED_)
#define AFX_KIMPORT_H__698B56FD_6EBC_4854_BCAE_7887A79B36A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SQLib.h"
#include "HiddenDef.h"
#include "HSRFace.h"

class CHSRImport
{
public:
	CHSRImport();
	~CHSRImport();
public:
	CHSRFace* ExtractPrimitives(DList* dlist,CHSRFace* flist);
	long  GetNumOfFace();
private:

	CHSRFace* ExtractCylinder(CHSRFace* pFaceList,DList* pList,HSR_PRIMITIVE_E id);
	CHSRFace* ExtractElbow(DList* pList,CHSRFace* flist);
	CHSRFace* ExtractCone(CHSRFace* pFaceList,DList* pList,HSR_PRIMITIVE_E id);
	CHSRFace* ExtractTorus(DList* pList,CHSRFace* flist);
	CHSRFace* ExtractPipe(CHSRFace* pFaceList,DList* pList);
	CHSRFace* ExtractTee(CHSRFace* pFaceList,DList* pList);
	CHSRFace* ExtractFlange(DList* pList,CHSRFace* flist);
	CHSRFace* ExtractBox(DList* pList,CHSRFace* flist);
	CHSRFace* ExtractDome(DList* pList,CHSRFace* flist);
	CHSRFace* ExtractSWElbow(DList* pList,CHSRFace* flist);

	CHSRFace* ExtractSideFace(DList* pList,bool bCenterline = false);
	CHSRFace* ExtractElbowEdgeFace(DList* pList,PPOINT_T pt,double* radius);

	CHSRFace* ExtractFace(const int nCount,DList* pFaceList);
	CHSRFace* ExtractHFace(const int nCount,DList* pFaceList);
	CHSRFace* CHSRImport::ExtractFace(CHSRFace* pFaceList, DList *pDList);
	CHSRFace* ExtractHFace(CHSRFace* pFaceList, DList *pDList);
	PHSR_VERTEX ExtractVertexListFromDList(CHSRFace* pFace,DList* pVertexList,int *nCount,PPOINT_T pt,bool box = true);
	
	PHSR_VERTEX AppendVertex(PHSR_VERTEX pVertex,PHSR_VERTEX pValue);
private:
	PHSR_VERTEX CreateTeeSymbol(const LINE_T& line,const double nRadius);
	CHSRFace* ExtractPipeBend(CHSRFace* pFaceList,DList* pList);
	CHSRFace* ExtractExtendTee(CHSRFace* pFaceList,DList* pList);
	CHSRFace* ExtractExtendPipe(CHSRFace* pFaceList,DList* pList);
	bool ExtractFaceInfo(PPOINT_T pt,double* pRadius,int* pCount,DList** pList);
	long m_nID;

	queue<DList*> m_queDList;
	POINT_T m_ptBuf[2][100];
};

#endif // !defined(AFX_KIMPORT_H__698B56FD_6EBC_4854_BCAE_7887A79B36A0__INCLUDED_)
