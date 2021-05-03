//#include "d:\\project\\spoon_0_0_1\\utility\\profile.h"

#include <math.h>
#include <SQLib.h>
#include <graphics\geometry.h>
#include "libmesh2d.h"
#include "DT.h"
#include "delaunay.h"

using namespace mesh;

CDelaunayEdge CDelaunay::_Edge[10240];
long CDelaunay::_nEdge=0;

/**	\brief
*/
CDelaunay::CDelaunay()
{
	m_nCC = 0;
	_nEdge= 0;
}

/**	\brief
*/
CDelaunay::~CDelaunay()
{
	//OnDestroy();
}

/**	\brief	The CDelaunay::OnDestroy function


	\return	void	
*/
void CDelaunay::OnDestroy()
{
	for(list<CDT*>::iterator itr=m_lstDT.begin();itr != m_lstDT.end();)
	{
		delete (*itr);
		m_lstDT.erase(itr++);
	}
}

/**	\brief	The CDelaunay::Initialize function\n
	erase Delaunay Triangles.


	\return	void	
*/
void CDelaunay::Initialize()
{
	for(list<CDT*>::iterator itr=m_lstDT.begin();itr != m_lstDT.end();)
	{
		delete (*itr);
		m_lstDT.erase(itr++);
	}
	m_lstDT.resize(0);

	m_nCC  = 0;
	_nEdge = 0;
}

/**	\brief	The CDelaunay::Triangulate function\n
	int aryPoint[], create delaunay triangles.

	\param	aryPoint[]	a parameter of type POINT_T
	\param	nPoints	a parameter of type long

	\return	void	
*/
void CDelaunay::Triangulate(POINT_T aryPoint[],long nPoints)
{
	assert(aryPoint && "pVertices is NULL");
	CLibMesh2d* pInst=CLibMesh2d::GetInstance();

	if(aryPoint)
	{
		//PROFILE("Triangulate");

		POINT_T ptSeg[2]={0,};
		ptSeg[0] = aryPoint[nPoints - 2];ptSeg[1] = aryPoint[nPoints - 1];
		RemoveDuplicatedPoints(aryPoint,nPoints);

		POINT_T pt[3]={0.};
		CDelaunayEdge edge;
		int nIndex[2]={0,};
		queue<CDelaunayEdge> que;
		FindClosetPoints(edge,aryPoint,nPoints);
		//pt[0] = aryPoint[nIndex[0]];pt[1] = aryPoint[nIndex[1]];
		pt[0] = edge.pt[0].point();pt[1] = edge.pt[1].point();
		int index=0;
		double min=0.,area=0.;
		for(int i=0;i < nPoints;i++)
		{
			double area=CDT::Area(pt[0],pt[1],aryPoint[i]);
			CDelaunayEdge e[2];
			e[0].pt[0] = edge.pt[0];
			e[0].pt[1] = aryPoint[i];
			e[1].pt[0] = edge.pt[1];
			e[1].pt[1] = aryPoint[i];
			if((0. != area) && (!DoOccurIntersection(e[0],e[1])))
			{
				double ux=edge.pt[0].x() - aryPoint[i].x;
				double uy=edge.pt[0].y() - aryPoint[i].y;
				double vx=edge.pt[1].x() - aryPoint[i].x;
				double vy=edge.pt[1].y() - aryPoint[i].y;
				double len=ux*ux + uy*uy + vx*vx + vy*vy;
				if(0. == min)
				{
					min   = len;
					index = i;
				}
				else if(len < min)
				{
					min  = len;
					index= i;
				}
			}
		}
		CDT* pDT=pInst->NewDT(edge.pt[0].point(),edge.pt[1].point(),aryPoint[index]);
		if(pDT)
		{
			int n=pDT->HasSegment(ptSeg[0],ptSeg[1]);
			if(n >= 0) pDT->m_nEdgeId[n] = 1;

			pDT->SetCCW();
			m_lstDT.push_back(pDT);
			edge.pt[0] = (*pDT)[2];
			edge.pt[1] = (*pDT)[1];
			que.push(edge);
			AddEdge(edge.pt[0].point(),edge.pt[1].point());
			
			edge.pt[0] = (*pDT)[1];
			edge.pt[1] = (*pDT)[0];
			que.push(edge);
			AddEdge(edge.pt[0].point(),edge.pt[1].point());
			
			edge.pt[0] = (*pDT)[0];
			edge.pt[1] = (*pDT)[2];
			que.push(edge);
			AddEdge(edge.pt[0].point(),edge.pt[1].point());
		}
		
		while(!que.empty())
		{
			edge = que.front();que.pop();

			long nStart=FindIndexOfLeftPoint(aryPoint,nPoints,edge.pt[0].point(),edge.pt[1].point());
			if(-1 != nStart)
			{
				m_nCC = 0;
				long nIndex=FindIndexOfDelaunayPoint(aryPoint,nStart,nPoints,edge.pt[0].point(),edge.pt[1].point());
				if(-1 != nIndex)
				{
					if(0 == m_nCC)
					{
						CDT* pDT=pInst->NewDT(edge.pt[0].point(),edge.pt[1].point(),aryPoint[nIndex]);
						if(pDT)
						{
							int n=pDT->HasSegment(ptSeg[0],ptSeg[1]);
							if(n >= 0) pDT->m_nEdgeId[n] = 1;

							pDT->SetCCW();
							if(AddDT(pDT))
							{
								edge.pt[0] = (*pDT)[0];
								edge.pt[1] = (*pDT)[2];
								que.push(edge);
								edge.pt[0] = (*pDT)[2];
								edge.pt[1] = (*pDT)[1];
								que.push(edge);
							}
						}
					}
					else
					{
						m_ptCC[m_nCC++] = aryPoint[nIndex];
						m_ptCC[m_nCC++] = edge.pt[1].point();
						m_ptCC[m_nCC++] = edge.pt[0].point();
						CCW();
						for(int i=0;i < m_nCC - 2;i++)
						{
							CDT* pDT=pInst->NewDT(m_ptCC[m_nCC - 1],m_ptCC[m_nCC - (i+2)],m_ptCC[m_nCC - (i+3)]);
							if(pDT)
							{
								int n=pDT->HasSegment(ptSeg[0],ptSeg[1]);
								if(n >= 0) pDT->m_nEdgeId[n] = 1;

								if(AddDT(pDT))
								{
									edge.pt[0] = (*pDT)[0];
									edge.pt[1] = (*pDT)[2];
									que.push(edge);
									edge.pt[0] = (*pDT)[2];
									edge.pt[1] = (*pDT)[1];
								}
							}
						}
						CDelaunayEdge edge;
						edge.pt[0] = m_ptCC[m_nCC - 1];
						edge.pt[1] = m_ptCC[0];
						AddEdge(edge.pt[0].point(),edge.pt[1].point());
					}
				}
			}
		}
	}
}

/**	\brief	The CDelaunay::RemoveDuplicatedPoints function

	\param	aryPoint[]	a parameter of type POINT_T
	\param	nPoints	a parameter of type long&

	\return	void	
*/
void CDelaunay::RemoveDuplicatedPoints(POINT_T aryPoint[],long& nPoints)
{
	assert((nPoints > 0) && "nPoints is less than 0.");

	if(nPoints > 0)
	{
		for(long i=0;i < nPoints;i++)
		{
			aryPoint[i].x = _round(aryPoint[i].x,9);
			aryPoint[i].y = _round(aryPoint[i].y,9);
			aryPoint[i].z = _round(aryPoint[i].z,9);
		}
		for(long i=0;i < nPoints;i++)
		{
			for(long j= i + 1;j < nPoints;)
			{
				if(IsSamePoint(aryPoint[i],aryPoint[j]))
				{
					memmove(&(aryPoint[j]),&(aryPoint[j+1]),sizeof(POINT_T)*(nPoints - j));
					nPoints--;
				}else	j++;
			}
		}
	}
}

/**	\brief	The CDelaunay::InsertPoints function

	\param	aryBoundaryPoint[]	a parameter of type POINT_T
	\param	nPoints	a parameter of type long

	\return	void	
*/
void CDelaunay::InsertPoints(POINT_T aryBoundaryPoint[],long nPoints)
{
	assert(aryBoundaryPoint && "aryBoundaryPoint is NULL");
	assert((nPoints > 0)    && "nPoints is less than 0.");
	CLibMesh2d* pInst=CLibMesh2d::GetInstance();

	if(aryBoundaryPoint && (nPoints > 0))
	{
		RemoveDuplicatedPoints(aryBoundaryPoint,nPoints);
		for(int i=0;i < nPoints;i++)
		{
			for(list<CDT*>::iterator itr=m_lstDT.begin();itr != m_lstDT.end();)
			{
				CDT* pDT=*itr;
				LOCATE_T l=pDT->WhereLocatedPoint(aryBoundaryPoint[i]);
				if(LOC_INNER_POINT == l)
				{
					POINT_T pt[3]={0};
					pt[0] = (*pDT)[0];
					pt[1] = (*pDT)[1];
					pt[2] = (*pDT)[2];

					DeleteEdge(pt[0],pt[1]);
					DeleteEdge(pt[1],pt[2]);
					DeleteEdge(pt[2],pt[0]);
					
					//////////////////////////////////////////////////////////////////////////
					pInst->DestroyDT(pDT);//delete pDT;
					m_lstDT.erase(itr++);
					//////////////////////////////////////////////////////////////////////////

					CDT* pNewDT=pInst->NewDT(aryBoundaryPoint[i],pt[0],pt[1]);
					if(pNewDT) AddDT(pNewDT);

					pNewDT = pInst->NewDT(aryBoundaryPoint[i],pt[1],pt[2]);
					if(pNewDT) AddDT(pNewDT);

					pNewDT = pInst->NewDT(aryBoundaryPoint[i],pt[2],pt[0]);
					if(pNewDT) AddDT(pNewDT);
					break;
				}
				else if(LOC_ON_EDGE == l)
				{
					POINT_T pt[3]={0};
					pt[0] = (**itr)[0];
					pt[1] = (**itr)[1];
					pt[2] = (**itr)[2];

					if(CGeometry::IsPointOnLine(aryBoundaryPoint[i].x,aryBoundaryPoint[i].y,
						pt[0].x,pt[0].y,pt[1].x,pt[1].y))
					{
						
						DeleteEdge(pt[0],pt[1]);
						DeleteEdge(pt[1],pt[2]);
						DeleteEdge(pt[2],pt[0]);
						
						pInst->DestroyDT(pDT);
						m_lstDT.erase(itr++);

						CDT* pNewDT=pInst->NewDT(pt[0],aryBoundaryPoint[i],pt[2]);
						if(pNewDT) AddDT(pNewDT);

						pNewDT=pInst->NewDT(aryBoundaryPoint[i],pt[1],pt[2]);
						if(pNewDT) AddDT(pNewDT);
					}
					else if(CGeometry::IsPointOnLine(aryBoundaryPoint[i].x,aryBoundaryPoint[i].y,
						pt[1].x,pt[1].y,pt[2].x,pt[2].y))
					{

						DeleteEdge(pt[0],pt[1]);
						DeleteEdge(pt[1],pt[2]);
						DeleteEdge(pt[2],pt[0]);
						
						pInst->DestroyDT(pDT);
						m_lstDT.erase(itr++);
						
						CDT* pNewDT=pInst->NewDT(pt[0],pt[1],aryBoundaryPoint[i]);
						if(pNewDT) AddDT(pNewDT);
						
						pNewDT=pInst->NewDT(aryBoundaryPoint[i],pt[2],pt[0]);
						if(pNewDT) AddDT(pNewDT);
					}
					else if(CGeometry::IsPointOnLine(aryBoundaryPoint[i].x,aryBoundaryPoint[i].y,
						pt[2].x,pt[2].y,pt[0].x,pt[0].y))
					{

						DeleteEdge(pt[0],pt[1]);
						DeleteEdge(pt[1],pt[2]);
						DeleteEdge(pt[2],pt[0]);
						
						pInst->DestroyDT(pDT);
						m_lstDT.erase(itr++);

						CDT* pNewDT=pInst->NewDT(pt[0],pt[1],aryBoundaryPoint[i]);
						if(pNewDT) AddDT(pNewDT);
						
						pNewDT=pInst->NewDT(aryBoundaryPoint[i],pt[1],pt[2]);
						if(pNewDT) AddDT(pNewDT);
					}
				}
				else if(LOC_ON_POINT == l)
				{
					itr++;
					break;
				}else	itr++;
			}
		}
	}
}

/**	\brief	The CDelaunay::InsertBoundary function\n
	delete triangle intersected with boundary, and re-triangulate.

	\param	aryBoundary[]	a parameter of type LINE_T
	\param	nBoundary	a parameter of type long

	\return	void	
*/
void CDelaunay::InsertBoundary(LINE_T aryBoundary[],long nBoundary)
{
	assert(aryBoundary && "aryBoundary is NULL");
	if(aryBoundary)
	{
		POINT_T ptLeft[512]={0},ptRight[512]={0};
		long nLeft=0,nRight=0;
		for(int i = 0;i < nBoundary;i++,nLeft = 0,nRight = 0)
		{
			aryBoundary[i].ptStart.x = _round(aryBoundary[i].ptStart.x,9);
			aryBoundary[i].ptStart.y = _round(aryBoundary[i].ptStart.y,9);
			aryBoundary[i].ptStart.z = _round(aryBoundary[i].ptStart.z,9);
			aryBoundary[i].ptEnd.x = _round(aryBoundary[i].ptEnd.x,9);
			aryBoundary[i].ptEnd.y = _round(aryBoundary[i].ptEnd.y,9);
			aryBoundary[i].ptEnd.z = _round(aryBoundary[i].ptEnd.z,9);

			///if((i < 18) || (i > 18)) continue;
			for(list<CDT*>::iterator itr=m_lstDT.begin();itr != m_lstDT.end();)
			{
				CDT* pDT=*itr;
				if((*itr)->DoOccurIntersection(aryBoundary[i].ptStart,aryBoundary[i].ptEnd))
				{
					POINT_T pt[3]={0};
					pt[0] = (**itr)[0];
					pt[1] = (**itr)[1];
					pt[2] = (**itr)[2];

					if(Left(aryBoundary[i].ptStart,aryBoundary[i].ptEnd,pt[0]))
					{
						ptLeft[nLeft++]=pt[0];
					}
					else if(Right(aryBoundary[i].ptStart,aryBoundary[i].ptEnd,pt[0]))
					{
						ptRight[nRight++]=pt[0];
					}
					
					if(Left(aryBoundary[i].ptStart,aryBoundary[i].ptEnd,pt[1]))
					{
						ptLeft[nLeft++]=pt[1];
					}
					else if(Right(aryBoundary[i].ptStart,aryBoundary[i].ptEnd,pt[1]))
					{
						ptRight[nRight++]=pt[1];
					}
					
					if(Left(aryBoundary[i].ptStart,aryBoundary[i].ptEnd,pt[2]))
					{
						ptLeft[nLeft++]=pt[2];
					}
					else if(Right(aryBoundary[i].ptStart,aryBoundary[i].ptEnd,pt[2]))
					{
						ptRight[nRight++]=pt[2];
					}
					
					DeleteEdge(pt[0],pt[1]);
					DeleteEdge(pt[1],pt[2]);
					DeleteEdge(pt[2],pt[0]);
					m_lstDT.erase(itr++);
				}else	itr++;
			}
			ptLeft[nLeft++] = aryBoundary[i].ptStart;
			ptLeft[nLeft++] = aryBoundary[i].ptEnd;
			if(nLeft >= 3) Triangulate(ptLeft,nLeft);
			ptRight[nRight++] = aryBoundary[i].ptStart;
			ptRight[nRight++] = aryBoundary[i].ptEnd;
			if(nRight >= 3) Triangulate(ptRight,nRight);
			//}
		}
	}
}

/**	\brief	The CDelaunay::GetListOfDT function


	\return	list<CDT*>*	
*/
list<CDT*>* CDelaunay::GetListOfDT()
{
	return &(m_lstDT);
}

/**	\brief	The CDelaunay::FindClosetPoints function

	\param	edge	a parameter of type CDelaunayEdge&
	\param	aryPoint[]	a parameter of type POINT_T
	\param	nPoints	a parameter of type long

	\return	void	
*/
void CDelaunay::FindClosetPoints(CDelaunayEdge& edge,POINT_T aryPoint[],long nPoints)
{
	assert((nPoints > 1));
	double min=0.,dist=0.;

	int first=0,second=1;
	min = Distance(aryPoint[0],aryPoint[1]);
	for(int i=0;i < nPoints - 1;i++)
	{
		for(int j=i+1;j < nPoints;j++)
		{
			dist = Distance(aryPoint[i],aryPoint[j]);
			if(dist < min)
			{
				min   = dist;
				first = i;
				second= j;
			}
		}
	}

	edge.pt[0] = aryPoint[first];
	edge.pt[1] = aryPoint[second];
}

/**	\brief	The CDelaunay::Distance function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	double	
*/
double CDelaunay::Distance(const POINT_T& pt1,const POINT_T& pt2)
{
	double dx=pt2.x - pt1.x;
	double dy=pt2.y - pt1.y;
	
	double nDist=sqrt(dx*dx + dy*dy);
	return nDist;
}

/**	\brief	The CDelaunay::FindIndexOfLeftPoint function\n
	get index of point located left side of edge.

	\param	aryPoint[]	a parameter of type POINT_T
	\param	nPoints	a parameter of type long
	\param	pt1	a parameter of type POINT_T&
	\param	pt2	a parameter of type POINT_T&

	\return	long	
*/
long CDelaunay::FindIndexOfLeftPoint(POINT_T aryPoint[],long nPoints,POINT_T& pt1,POINT_T& pt2)
{
	long nIndex=-1;

	LINE_T line={0};
	line.ptStart = pt1;
	line.ptEnd   = pt2;
	for(int i=0;i < nPoints;i++)
	{
		if(Left(pt1,pt2,aryPoint[i]))
		{
			nIndex = i;
			break;
		}
	}

	return nIndex;
}

/**	\brief	The CDelaunay::FindIndexOfDelaunayPoint function

	\param	aryPoint[]	a parameter of type POINT_T
	\param	nStart	a parameter of type long
	\param	nPoints	a parameter of type long
	\param	pt1	a parameter of type POINT_T&
	\param	pt2	a parameter of type POINT_T&

	\return	long	
*/
long CDelaunay::FindIndexOfDelaunayPoint(POINT_T aryPoint[],long nStart,long nPoints,POINT_T& pt1,POINT_T& pt2)
{
	long nIndex=nStart;

	LINE_T line={0};
	line.ptStart = pt1;
	line.ptEnd   = pt2;
	for(long i=nStart+1;i < nPoints;i++)
	{
		if(Left(pt1,pt2,aryPoint[i]) && CDT::IsInsideCircle(pt1,pt2,aryPoint[nIndex],aryPoint[i]))
		{
			CDelaunayEdge edge[2];

			edge[0].pt[0] = aryPoint[i];
			edge[0].pt[1] = pt1;
			edge[1].pt[0] = aryPoint[i];
			edge[1].pt[1] = pt2;
			if(!DoOccurIntersection(edge[0],edge[1]))
			{
				if(CDT::IsOnCircle(pt1,pt2,aryPoint[nIndex],aryPoint[i]))
				{
					m_ptCC[m_nCC++] = aryPoint[nIndex];
				}
				nIndex = i;
			}
		}
	}
	CDelaunayEdge edge[2];

	edge[0].pt[0] = aryPoint[nIndex];
	edge[0].pt[1] = pt1;
	edge[1].pt[0] = aryPoint[nIndex];
	edge[1].pt[1] = pt2;
	if(DoOccurIntersection(edge[0],edge[1])) nIndex = -1;

	return nIndex;
}

/**	\brief	The CDelaunay::DoOccurIntersection function

	\param	edge1	a parameter of type CDelaunayEdge&
	\param	edge2	a parameter of type CDelaunayEdge&

	\return	bool	
*/
bool CDelaunay::DoOccurIntersection(CDelaunayEdge& edge1,CDelaunayEdge& edge2)
{
	/*for(int i=0;(i < 10240) && i < g_nEdge;i++){
		if(DoIntersect(edge1.pt[0],edge1.pt[1],g_Edge[i].pt[0],g_Edge[i].pt[1]) || 
			DoIntersect(edge2.pt[0],edge2.pt[1],g_Edge[i].pt[0],g_Edge[i].pt[1])){
			return true;
		}
	}
	*/
	for(int i=0;(i < 10240) && i < _nEdge;i++)
	{
		if(DoIntersect(edge1.pt[0].point(),edge1.pt[1].point(),_Edge[i].pt[0].point(),_Edge[i].pt[1].point()) || 
			DoIntersect(edge2.pt[0].point(),edge2.pt[1].point(),_Edge[i].pt[0].point(),_Edge[i].pt[1].point()))
		{
			return true;
		}
	}

	return false;
}

/**	\brief	The CDelaunay::DoIntersect function

	\param	pt1	a parameter of type POINT_T&
	\param	pt2	a parameter of type POINT_T&
	\param	pt3	a parameter of type POINT_T&
	\param	pt4	a parameter of type POINT_T&

	\return	bool	
*/
bool CDelaunay::DoIntersect(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3,POINT_T& pt4)
{
	double dx=pt3.x - pt1.x;
	double dy=pt3.y - pt1.y;
	double dz=pt3.z - pt1.z;
	if(IsSamePoint(pt1,pt3)) return false;
	//if((fabs(dx) < CLibMesh2d::TOLERANCE) && (fabs(dy) < CLibMesh2d::TOLERANCE) && (fabs(dz) < CLibMesh2d::TOLERANCE)) return false;
	dx = pt4.x - pt1.x;
	dy = pt4.y - pt1.y;
	dz = pt4.z - pt1.z;
	if(IsSamePoint(pt4,pt1)) return false;
	//if((fabs(dx) < CLibMesh2d::TOLERANCE) && (fabs(dy) < CLibMesh2d::TOLERANCE) && (fabs(dz) < CLibMesh2d::TOLERANCE)) return false;
	dx = pt3.x - pt2.x;
	dy = pt3.y - pt2.y;
	dz = pt3.z - pt2.z;
	if(IsSamePoint(pt3,pt2)) return false;
	//if((fabs(dx) < CLibMesh2d::TOLERANCE) && (fabs(dy) < CLibMesh2d::TOLERANCE) && (fabs(dz) < CLibMesh2d::TOLERANCE)) return false;
	dx = pt4.x - pt2.x;
	dy = pt4.y - pt2.y;
	dz = pt4.z - pt2.z;
	if(IsSamePoint(pt4,pt2)) return false;
	//if((fabs(dx) < CLibMesh2d::TOLERANCE) && (fabs(dy) < CLibMesh2d::TOLERANCE) && (fabs(dz) < CLibMesh2d::TOLERANCE)) return false;

	if(Collinear(pt1,pt2,pt3) || Collinear(pt1,pt2,pt4) || 
		Collinear(pt3,pt4,pt1) || Collinear(pt3,pt4,pt2))
		return false;

	return Xor(Left(pt1,pt2,pt3),Left(pt1,pt2,pt4)) && Xor(Left(pt3,pt4,pt1),Left(pt3,pt4,pt2));
}

/**	\brief	The CDelaunay::Collinear function

	\param	pt1	a parameter of type POINT_T&
	\param	pt2	a parameter of type POINT_T&
	\param	pt3	a parameter of type POINT_T&

	\return	bool	
*/
bool CDelaunay::Collinear(POINT_T& pt1,POINT_T& pt2,POINT_T& pt3)
{
	return (CDT::Area(pt1,pt2,pt3) == 0.);
}

/**	\brief	The CDelaunay::Left function

	\param	pt1	a parameter of type POINT_T&
	\param	pt2	a parameter of type POINT_T&
	\param	check	a parameter of type POINT_T&

	\return	bool	
*/
bool CDelaunay::Left(POINT_T& pt1,POINT_T& pt2,POINT_T& check)
{
	return CDT::Area(pt1,pt2,check) > 0.;
}

/**	\brief	The CDelaunay::Right function

	\param	pt1	a parameter of type POINT_T&
	\param	pt2	a parameter of type POINT_T&
	\param	check	a parameter of type POINT_T&

	\return	bool	
*/
bool CDelaunay::Right(POINT_T& pt1,POINT_T& pt2,POINT_T& check)
{
	return CDT::Area(pt1,pt2,check) < 0.;
}

/**	\brief	The CDelaunay::Xor function

	\param	x	a parameter of type bool
	\param	y	a parameter of type bool

	\return	bool	
*/
bool CDelaunay::Xor(bool x,bool y)
{
	return !x ^ !y;
}

/**	\brief	The CDelaunay::CCW function


	\return	void	
*/
void CDelaunay::CCW()
{
	for(int i=0;i < m_nCC - 2;i++)
	{
		int j=i;
		while(CDT::Area(m_ptCC[m_nCC - 1],m_ptCC[j],m_ptCC[j + 1]) > 0.)
		{
			POINT_T temp=m_ptCC[j];
			m_ptCC[j] = m_ptCC[j+1];
			m_ptCC[j+1] = temp;
			if(j > 0) j--;
		}
	}
}

/**	\brief	The CDelaunay::IsLeftSidePoint function

	\param	pt	a parameter of type const POINT_T&
	\param	line	a parameter of type const LINE_T&

	\return	bool	
*/
bool CDelaunay::IsLeftSidePoint(const POINT_T& pt,const LINE_T& line)
{
	double ux=pt.x - line.ptStart.x;
	double uy=pt.y - line.ptStart.y;
	double vx=line.ptEnd.x - line.ptStart.x;
	double vy=line.ptEnd.y - line.ptStart.y;
	
	return (ux*vy - uy*vx) > 0.;
}

/**	\brief	The CDelaunay::PushEdge function

	\param	queEdge	a parameter of type queue<CDelaunayEdge>&
	\param	edge	a parameter of type CDelaunayEdge&

	\return	void	
*/
void CDelaunay::PushEdge(queue<CDelaunayEdge>& queEdge,CDelaunayEdge& edge)
{
	queEdge.push(edge);
	
	if(_nEdge < 10240)
	{
		_Edge[_nEdge].pt[0] = edge.pt[0];
		_Edge[_nEdge].pt[1] = edge.pt[1];
		_nEdge++;
	}
}

/**	\brief	The CDelaunay::AddDT function\n
	if a triangle not exits in list of triangle , \n
	append a triangle and edges of triangle.

	\param	pDT	a parameter of type CDT*

	\return	true - append triangle and edges of triangle , false - not append.
*/
bool CDelaunay::AddDT(CDT* pDT)
{
	assert(pDT && "pTriangle is NULL");
	bool bRet=false;

	if(pDT)
	{
		bool bExist=false;
		for(list<CDT*>::iterator itr=m_lstDT.begin();itr != m_lstDT.end();++itr)
		{
			CDT* p=*itr;
			//if((**itr) == (*pDT)){
			if(pDT->operator ==(**itr))
			{
				bExist = true;
				break;
			}
		}
		if(!bExist)
		{			
			int nCount=0;
			CDelaunayEdge edge;
			edge.pt[0] = (*pDT)[0];
			edge.pt[1] = (*pDT)[1];
			if(AddEdge(edge.pt[0].point(),edge.pt[1].point())) nCount++;

			edge.pt[0] = (*pDT)[2];
			edge.pt[1] = (*pDT)[1];
			if(AddEdge(edge.pt[0].point(),edge.pt[1].point())) nCount++;

			edge.pt[0] = (*pDT)[0];
			edge.pt[1] = (*pDT)[2];
			if(AddEdge(edge.pt[0].point(),edge.pt[1].point())) nCount++;

			m_lstDT.push_back(pDT);
			bRet = true;
		}
	}

	return bRet;
}

/**	\brief	The CDelaunay::DeleteEdge function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	void	
*/
void CDelaunay::DeleteEdge(const POINT_T& pt1,const POINT_T& pt2)
{
	for(int i=0;i < _nEdge;i++)
	{
		if(	((IsSamePoint(_Edge[i].pt[0].point(),pt1)) && (IsSamePoint(_Edge[i].pt[1].point(),pt2))) ||
			((IsSamePoint(_Edge[i].pt[0].point(),pt2)) && (IsSamePoint(_Edge[i].pt[1].point(),pt1))))
		{
		//if(	((0 == memcmp(&(_Edge[i].pt[0]),&pt1,sizeof(POINT_T))) && (0 == memcmp(&(_Edge[i].pt[1]),&pt2,sizeof(POINT_T)))) ||
		//	((0 == memcmp(&(_Edge[i].pt[0]),&pt2,sizeof(POINT_T))) && (0 == memcmp(&(_Edge[i].pt[1]),&pt1,sizeof(POINT_T))))){
			(_Edge[i].ref)--;
			if(_Edge[i].ref <= 0)
			{
				memmove(&(_Edge[i]),&(_Edge[i + 1]),sizeof(CDelaunayEdge)*(_nEdge - i));
				_nEdge--;
			}
		}
	}
}

/**	\brief	The CDelaunay::AddEdge function\n
	append a edge consisted of pt1 and pt2 to array of edge.\n
	if edge already exits in array, increase reference of edge.

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	void	
*/
bool CDelaunay::AddEdge(const POINT_T& pt1,const POINT_T& pt2)
{
	for(int i=0;i < _nEdge;i++)
	{
		if(	((IsSamePoint(_Edge[i].pt[0].point(),pt1)) && (IsSamePoint(_Edge[i].pt[1].point(),pt2))) ||
			((IsSamePoint(_Edge[i].pt[0].point(),pt2)) && (IsSamePoint(_Edge[i].pt[1].point(),pt1))))
		{
		//if(	((0 == memcmp(&(_Edge[i].pt[0]),&pt1,sizeof(POINT_T))) && (0 == memcmp(&(_Edge[i].pt[1]),&pt2,sizeof(POINT_T)))) ||
		//	((0 == memcmp(&(_Edge[i].pt[0]),&pt2,sizeof(POINT_T))) && (0 == memcmp(&(_Edge[i].pt[1]),&pt1,sizeof(POINT_T))))){
			_Edge[i].ref++;
			return false;
		}
	}
	_Edge[_nEdge].pt[0] = pt1;
	_Edge[_nEdge].pt[1] = pt2;
	_Edge[_nEdge].ref   = 1;
	_nEdge++;

	return true;
}