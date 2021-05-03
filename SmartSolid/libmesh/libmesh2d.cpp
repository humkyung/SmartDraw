#include <math.h>
#include <SQLib.h>
#include "delaunay.h"
#include "libmesh2d.h"
using namespace mesh;

long CLibMesh2d::EDGEID=0L;
CMeshPos2d CLibMesh2d::pos[POS_BUF_SIZ];
LINE_T CLibMesh2d::lnBoundary[BOUNDARY_BUF_SIZ]={0,};
const double CLibMesh2d::TOLERANCE=0.0000000000001;

double _round(double doValue, int nPrecision)
{
	static const double doBase = 10.0;
	double doComplete5, doComplete5i;
	
	doComplete5 = doValue * pow(doBase, (double) (nPrecision + 1));
	
	if(doValue < 0.0)
		doComplete5 -= 5.0;
	else
		doComplete5 += 5.0;
	
	doComplete5 /= doBase;
	modf(doComplete5, &doComplete5i);
	
	return doComplete5i / pow(doBase, (double) nPrecision);
}

bool IsSameValue(const double& val1,const double& val2)
{
	const static double nTol=0.000000001;
	
	double d=_round(val1 - val2,9);
	return (fabs(d) < nTol);
}

bool IsSamePoint(const POINT_T& pt1,const POINT_T& pt2)
{
	const static double nTol=0.000000001;
	double dx=_round(pt1.x - pt2.x,9);
	double dy=_round(pt1.y - pt2.y,9);

	if((0 == dx) && (0 == dy)) return true;
	//if((fabs(dx) < nTol) && (fabs(dy) < nTol)) return true;

	return false;
}

CLibMesh2d::CLibMesh2d()
{
	m_pDTPool = NULL;
}

CLibMesh2d::~CLibMesh2d()
{
}

/**	\brief	The CLibMesh2d::GetInstance function


	\return	CLibMesh2d*	
*/
CLibMesh2d* CLibMesh2d::GetInstance()
{
	static CLibMesh2d theLibMesh2d;

	return (&theLibMesh2d);
}

/**	\brief	The CLibMesh2d::Initialize function


	\return	void	
*/
void CLibMesh2d::Initialize()
{
	if(NULL != m_pDTPool) delete m_pDTPool;
	m_pDTPool = NULL;

	m_GridMethod.Initialize();
}

/**	\brief	The CLibMesh2d::Mesh function

	\param	aryPoint[]	a parameter of type POINT_T
	\param	nPoints	a parameter of type int
	\param	aryBoundary[]	a parameter of type LINE_T
	\param	nBoundary	a parameter of type int

	\return	list<CDT*>*	
*/
list<CDT*>* CLibMesh2d::Mesh(POINT_T aryPoint[],int nPoints,LINE_T aryBoundary[],int nBoundary)
{
    	assert(aryPoint && "pVertices is NULL");

	if(aryPoint)
	{
		CDelaunay delaunay;
		delaunay.Initialize();

		/// triangulate all vertices.
		delaunay.Triangulate(aryPoint,nPoints);
		if(aryBoundary && nBoundary)
			delaunay.InsertBoundary(aryBoundary,nBoundary);

		 
		list<CDT*>* plstDT=delaunay.GetListOfDT();
		if(plstDT)
		{
			for(list<CDT*>::iterator itr=m_lstDT.begin();itr != m_lstDT.end();)
			{
				m_lstDT.erase(itr++);
			}

			for(list<CDT*>::iterator itr = plstDT->begin();itr != plstDT->end();++itr)
			{
				m_lstDT.push_back(*itr);
			}
		}
	}

	return &m_lstDT;
}

/**	\brief	The CLibMesh2d::Mesh function

	\param	lstDT	a parameter of type list<CDT*>&
	\param	aryBoundaryPoint[]	a parameter of type POINT_T
	\param	nPoints	a parameter of type int
	\param	aryBoundary[]	a parameter of type LINE_T
	\param	nBoundary	a parameter of type int

	\return	list<CDT*>*	
*/
list<CDT*>* CLibMesh2d::Mesh(list<CDT*>& lstDT,POINT_T aryBoundaryPoint[],int nPoints,LINE_T aryBoundary[],int nBoundary)
{
	CDelaunay delaunay;
	delaunay.Initialize();

	for(list<CDT*>::iterator itr=lstDT.begin();itr != lstDT.end();itr++)
	{
		delaunay.AddDT(*itr);
	}

	if(aryBoundaryPoint && nPoints) delaunay.InsertPoints(aryBoundaryPoint,nPoints);
	if(aryBoundary && nBoundary)    delaunay.InsertBoundary(aryBoundary,nBoundary);

	list<CDT*>* plstDT=delaunay.GetListOfDT();
	if(plstDT)
	{
		for(list<CDT*>::iterator itr=m_lstDT.begin();itr != m_lstDT.end();)
		{
			m_lstDT.erase(itr++);
		}

		for(list<CDT*>::iterator itr = plstDT->begin();itr != plstDT->end();++itr)
		{
			m_lstDT.push_back(*itr);
		}
	}

	return &m_lstDT;
}

/**	\brief	The CLibMesh2d::GetListOfGrid function


	\return	list<CGrid*>*	
*/
list<CGrid*>* CLibMesh2d::GetListOfGrid()
{
	return &(m_GridMethod.m_lstGrid);
}

/**	\brief	The CLibMesh2d::InsertGrids function

	\param	lstGrid	a parameter of type list<CGrid*>&

	\return	void	
*/
void CLibMesh2d::InsertGrids(list<CGrid*>& lstGrid)
{
	for(list<CGrid*>::iterator itr=lstGrid.begin();itr != lstGrid.end();++itr)
	{
		m_GridMethod.InsertGrid((*itr));
	}
}

/**	
	@brief	The CLibMesh2d::InsertGridPoints function

	@param	aryPoint[]	a parameter of type POINT_T
	@param	nPoints	a parameter of type int

	@return	void	
*/
void CLibMesh2d::InsertGridPoints(POINT_T aryPoint[] , int nPoints)
{
	assert(aryPoint && "aryPoint is NULL");

	if(aryPoint && (nPoints > 0))
	{
		int i = 0;
		for(i = 0;i < nPoints;++i)
		{
			aryPoint[i].x = _round(aryPoint[i].x,9);
			aryPoint[i].y = _round(aryPoint[i].y,9);
			aryPoint[i].z = _round(aryPoint[i].z,9);
		}

		//! remove duplicated points.
		for(i = 0;i < nPoints;++i)
		{
			for(int j= i + 1;j < nPoints;)
			{
				if(IsSamePoint(aryPoint[i],aryPoint[j]))
				{
					memmove(&(aryPoint[j]),&(aryPoint[j+1]),sizeof(POINT_T)*(nPoints - j));
					nPoints--;
				}else	j++;
			}
		}

		for(i=0;i < nPoints;++i)
		{
			m_GridMethod.InsertPoint(aryPoint[i]);
		}
	}
}

/**	\brief	The CLibMesh2d::InsertGridEdges function

	\param	pEdgeNode	a parameter of type snode<LINE_T>*

	\return	void	
*/
void CLibMesh2d::InsertGridEdges(snode<LINE_T>* pEdgeNode)
{
	assert(pEdgeNode && "pEdgeNode is NULL");

	if(pEdgeNode)
	{
		for(snode<LINE_T>* ptr=pEdgeNode;ptr;)
		{
			ptr->element.ptStart.x = _round(ptr->element.ptStart.x,9);
			ptr->element.ptStart.y = _round(ptr->element.ptStart.y,9);
			//if(	(ptr->element.ptStart.x == ptr->element.ptEnd.x) && 
			//	(ptr->element.ptStart.y == ptr->element.ptEnd.y)) 
			if(IsSamePoint(ptr->element.ptStart,ptr->element.ptEnd))
				ptr = ptr->next;
			else	ptr = m_GridMethod.InsertEdge(ptr);
		}
	}
}

/**	\brief	The CLibMesh2d::NewDT function


	\return	CDT*	
*/
CDT* CLibMesh2d::NewDT(const POINT_T& pt1,const POINT_T& pt2,const POINT_T& pt3)
{
	CDT* pDT=NULL;

	if(NULL == m_pDTPool) m_pDTPool = new boost::object_pool<CDT>;
	if(NULL != m_pDTPool)
	{
		pDT=m_pDTPool->construct();
		pDT->Set(pt1,pt2,pt3);
	}

	return pDT;
}

void CLibMesh2d::DestroyDT(CDT *pDT)
{
	assert(pDT && "pDT is NULL");

	if(pDT && (NULL != m_pDTPool)) m_pDTPool->destroy(pDT);
}
