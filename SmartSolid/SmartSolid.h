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
#include <boost/pool/object_pool.hpp>

#include <queue>
using namespace std;

#ifdef	OPENSOLID_EXPORTS
	#define	OPENSOLID_EXT_CLASS	__declspec(dllexport)
#else
	#define	OPENSOLID_EXT_CLASS	__declspec(dllimport)
#endif

class OPENSOLID_EXT_CLASS COpenSolid
{
public:
	COpenSolid();
	virtual ~COpenSolid();
public:
	void delete_vertex_list(list<SQVERTEX>* plst);
	list<SQVERTEX>* new_vertex_list();
	void delete_point_vector(vector<POINT_T>* pvt);
	vector<POINT_T>* new_point_vector();
	void delete_primitive(PSQPRIMITIVE ptr);
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
	PSQPRIMITIVE new_primitive();
private:
	int m_nCurveSegments;

	list<PSQPRIMITIVE>*  m_plstPrimitive;

	boost::pool<>* m_pPrimitivePool;
	boost::object_pool< vector<POINT_T> >* m_pPointVectorPool;
	boost::object_pool< list<SQVERTEX> >* m_pVertexListPool;
};

extern COpenSolid theOpenSolid;
#endif