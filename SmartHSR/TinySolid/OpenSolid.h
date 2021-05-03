/********************************************************************
	created:	2004/01/16
	created:	16:1:2004   13:46
	filename: 	D:\Project\PlantWAVE\OpenSolid\OpenSolid.h
	file path:	D:\Project\PlantWAVE\OpenSolid
	file base:	OpenSolid
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __OPENSOLID_H__
#define __OPENSOLID_H__

#include <SQLib.h>

#include <queue>
using namespace std;

#ifndef	DLL_EXPORT
#define	DLL_EXPORT __declspec(dllexport)
#endif

class DLL_EXPORT COpenSolid{
public:
	COpenSolid();
	virtual ~COpenSolid();
public:
	const int GetNumOfCurveSegments();
	void SetNumOfCurveSegments(int nSegments);


	PSQPRIMITIVE CreatePlaneSurface(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateConeSurface(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateTorusSurface(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateSphereSurface(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateSolid(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateSurface(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateArc(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateFacets(PPOINT_T pptPoints,int nParts,int nPoints,const SQHEADER& hdr);

	void ComputeVolume(PSQPRIMITIVE pPrimitive);

	void Pure();
protected:
	bool PossibleBeCone(PSQPRIMITIVE pPrimitive);
	bool PossibleBeDome(PSQPRIMITIVE pPrimitive);
	bool PossibleBeSphere(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE NewPrimitive();
	bool DeletePrimitive(PSQPRIMITIVE pPrimitive);
private:
	int m_nCurveSegments;

	queue<PSQPRIMITIVE> m_quePrimitivePool;
	list<PSQPRIMITIVE>*  m_plstPrimitive;
};

extern COpenSolid theOpenSolid;
#endif