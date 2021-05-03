#ifndef __OPENSOLID_H__
#define __OPENSOLID_H__

#include <queue>
#include <list>
using namespace std;

#ifndef	DLL_EXPORT
#define	DLL_EXPORT __declspec(dllexport)
#endif

class DLL_EXPORT KOpenSolid{
public:
	KOpenSolid();
	virtual ~KOpenSolid();
public:
	int GetNumOfCurveSegments();
	void SetNumOfCurveSegments(int nSegments);

	PSQPRIMITIVE CreateCone(PSQPRIMITIVE pPrimitive);

	PSQPRIMITIVE CreatePlaneSurface(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateConeSurface(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateTorusSurface(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateSphereSurface(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateSolid(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateSurface(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateArc(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateEllipse(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateCubicSpline(PSQPRIMITIVE pPrimitive);
	PSQPRIMITIVE CreateFacets(PSQPOINT pptPoints,int nParts,int nPoints,const SQHEADER& hdr);

	void ComputeVolume(PSQPRIMITIVE pPrimitive);

	void OnFreePrimitive();
protected:
	PSQPRIMITIVE NewPrimitive();
	bool DeletePrimitive(PSQPRIMITIVE pPrimitive);
private:
	int m_nCurveSegments;

	queue<PSQPRIMITIVE> m_quePrimitivePool;
	list<PSQPRIMITIVE>  m_lstPrimitive;
};

extern KOpenSolid theOpenSolid;
#endif