/********************************************************************
	created:	2004/01/16
	created:	16:1:2004   12:21
	filename: 	D:\Project\PlantWAVE\OpenSolid\mesh.h
	file path:	D:\Project\PlantWAVE\OpenSolid
	file base:	mesh
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/

#ifndef __OPEN3D_MESH_H__
#define __OPEN3D_MESH_H__

#include <boost/pool/object_pool.hpp>
#include "../3ds.h"
#include "DT.h"
#include "GridMethod.h"
#include "MeshPos2d.h"
#include "TinyEdge.h"

#include <list>
using namespace std;

#define LIBMESH2D_PRECISION	9

namespace mesh{
	#define POS_BUF_SIZ		256
	#define BOUNDARY_BUF_SIZ	524

	typedef struct tagBoundary
	{
		VECTOR_T    vecNorm;
		list<EDGE_T>* plstEdge;
	}BOUNDARY_T,* PBOUNDARY_T;

	class CLibMesh2d{
	protected:
		CLibMesh2d();
	public:
		static CLibMesh2d* GetInstance();
		virtual ~CLibMesh2d();
	public:
		void Initialize();

		list<CGrid*>* GetListOfGrid();
		void InsertGrids(list<CGrid*>& lstGrid);
		void InsertGridPoints(POINT_T aryPoint[],int nPoints);
		void InsertGridEdges(snode<LINE_T>* pEdgeNode);

		list<CDT*>* Mesh(POINT_T aryPoint[],int nPoint,LINE_T aryBoundary[],int nBoundary);
		list<CDT*>* Mesh(list<CDT*>& lstDT,POINT_T aryBoundaryPoint[],int nPoints,LINE_T aryBoundary[],int nBoundary);
	public:
		void DestroyDT(CDT* pDT);
		CGrid* new_grid();
		CDT* NewDT(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3);
		static long EDGEID;
		static CMeshPos2d pos[POS_BUF_SIZ];
		static LINE_T lnBoundary[BOUNDARY_BUF_SIZ];
		const static double TOLERANCE;
	private:
		boost::object_pool<CDT>* m_pDTPool;

		CGridMethod m_GridMethod;
		list<CDT*> m_lstDT;
	};
}

extern double _round(double doValue, int nPrecision);
extern bool IsSameValue(const double& val1,const double& val2);
extern bool IsSamePoint(const POINT_T& pt1,const POINT_T& pt2);
#endif

