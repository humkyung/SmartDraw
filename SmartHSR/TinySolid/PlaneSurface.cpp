#include <graphics\geometry.h>
#include <graphics/plane3d.h>
#include <graphics/arc3d.h>
#include "libmesh/libmesh2d.h"
#include "PlaneSurface.h"

#include <algorithm>
#include <queue>

using namespace mesh;
using namespace std;

/**	\brief
*/
CPlaneSurface::CPlaneSurface(){
	m_sfcPlane.pBoundary = NULL;
	m_nPoints = 0;
	m_nEdge   = 0;
}

/**	\brief
*/
CPlaneSurface::CPlaneSurface(PLANESURF_T& PlaneSurface){
	m_sfcPlane = PlaneSurface;
	m_nPoints  = 0;
	m_nEdge    = 0;

	OnCreate();
}

/**	\brief
*/
CPlaneSurface::~CPlaneSurface(){
	//for(list<CFacet*>::iterator itrFacet=m_lstFacet.begin();itrFacet != m_lstFacet.end();){
	//	delete (*itrFacet);
	//	m_lstFacet.erase(itrFacet++);
	//}

	for(list<CPolyCurve*>::iterator itrBoundary=m_lstBoundaryCurve.begin();itrBoundary != m_lstBoundaryCurve.end();){
		delete (*itrBoundary);
		m_lstBoundaryCurve.erase(itrBoundary++);
	}
}

/**	\brief	The CPlaneSurface::OnCreate function


	\return	void	
*/
void CPlaneSurface::OnCreate(){
	CPlane3d plane(m_sfcPlane.vecNorm);
	double alpha=0.,beta=0;
	plane.GetRotatedAngleOfAxis(alpha,beta);

	long nStart=0,nEnd=0;
	POINT_T pt={0};
	for(PSQPRIMITIVE pGroup=m_sfcPlane.pBoundary;pGroup;pGroup = pGroup->next){
		nStart = m_nPoints;
		for(PSQPRIMITIVE ptr=pGroup->body.pPrimitive;ptr;ptr = ptr->next){
			switch(ptr->hdr.id){
				case ENM_LINE:
					if(!ptr->hdr.attr.reversed){
						pt = ptr->body.line.ptStart;
						pt.x -= m_sfcPlane.ptOrigin.x;
						pt.y -= m_sfcPlane.ptOrigin.y;
						pt.z -= m_sfcPlane.ptOrigin.z;
						pt = plane.RotateAboutXYAxis(pt,alpha,beta);
						pt.z = 0.;
						AppendPoint(pt);

						pt = ptr->body.line.ptEnd;
						pt.x -= m_sfcPlane.ptOrigin.x;
						pt.y -= m_sfcPlane.ptOrigin.y;
						pt.z -= m_sfcPlane.ptOrigin.z;
						pt = plane.RotateAboutXYAxis(pt,alpha,beta);
						pt.z = 0.;
						AppendPoint(pt);
					}else{
						pt = ptr->body.line.ptEnd;
						pt.x -= m_sfcPlane.ptOrigin.x;
						pt.y -= m_sfcPlane.ptOrigin.y;
						pt.z -= m_sfcPlane.ptOrigin.z;
						pt = plane.RotateAboutXYAxis(pt,alpha,beta);
						pt.z = 0.;
						AppendPoint(pt);

						pt = ptr->body.line.ptStart;
						pt.x -= m_sfcPlane.ptOrigin.x;
						pt.y -= m_sfcPlane.ptOrigin.y;
						pt.z -= m_sfcPlane.ptOrigin.z;
						pt = plane.RotateAboutXYAxis(pt,alpha,beta);
						pt.z = 0.;
						AppendPoint(pt);
					}
				break;
				case SQ_POINTS:
					if(!ptr->hdr.attr.reversed){
						for(vector<POINT_T>::iterator itr=ptr->body.pvtPoints->begin();itr != ptr->body.pvtPoints->end();++itr){
							pt = *itr;
							pt.x -= m_sfcPlane.ptOrigin.x;
							pt.y -= m_sfcPlane.ptOrigin.y;
							pt.z -= m_sfcPlane.ptOrigin.z;
							pt = plane.RotateAboutXYAxis(pt,alpha,beta);
							//pt.z = 0.;
							AppendPoint(pt);							
						}
					}else{
					}
					break;
				case ENM_ARC:{
					CArc3d arc(ptr->body.arc);
					arc.CreateSegments();
					for(int i=0;i < arc.GetNumOfPoints();i++){
						pt    = CCurve3d::pt(i);
						pt.x -= m_sfcPlane.ptOrigin.x;
						pt.y -= m_sfcPlane.ptOrigin.y;
						pt.z -= m_sfcPlane.ptOrigin.z;
						pt = CGeometry::RotateAboutXYAxis(pt,alpha,beta);
						pt.z = 0.;
						AppendPoint(pt);
					}
					    }
				break;
			}
		}
		nEnd = m_nPoints;
		if((nEnd > nStart) && pGroup->hdr.attr.reversed) reverse(&m_pt[nStart],&m_pt[nEnd]);

		if(nEnd > nStart){
			for(int i=nStart;i < nEnd - 1;i++){
				m_lnBoundary[m_nEdge].ptStart = m_pt[i];
				m_lnBoundary[m_nEdge].ptEnd   = m_pt[i+1];
				m_nEdge++;
			}
			
			CPolyCurve* pPolyCurve=new CPolyCurve(nEnd - nStart,&(m_pt[nStart]));
			if(pPolyCurve){
				m_lstBoundaryCurve.push_back(pPolyCurve);
			}
		}
	}
}

/**	\brief	The CPlaneSurface::IsBoundaryEdge function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	bool	
*/
bool CPlaneSurface::IsBoundaryEdge(const POINT_T& pt1,const POINT_T& pt2){
	bool bRet=false;

	for(int i=0;i < m_nEdge;i++){
		if(
			(0 == memcmp(&pt1,&(m_lnBoundary[i].ptStart),sizeof(POINT_T))) &&
			(0 == memcmp(&pt2,&(m_lnBoundary[i].ptEnd),sizeof(POINT_T)))){
			bRet = true;
			break;
		}
	}

	return bRet;
}

/**	\brief	The CPlaneSurface::IsOutboundTriangle function

	\param	nOutboundIndex	a parameter of type int&
	\param	pDT	a parameter of type CDT*

	\return	BOUND_T	
*/
BOUND_T CPlaneSurface::IsOutboundTriangle(int& nOutboundIndex,CDT* pDT){
	assert(pDT && "pDT is NULL");

	if(pDT){
		POINT_T pt[3]={0};
		pt[0] = (*pDT)[0];
		pt[1] = (*pDT)[1];
		pt[2] = (*pDT)[2];
		if(IsBoundaryEdge(pt[0],pt[1])){
			if(CDT::Area(pt[0],pt[1],pt[2]) < 0.){
				nOutboundIndex = 2;
				return OUTER_BOUND;
			}else	return INNER_BOUND;
		}else if(IsBoundaryEdge(pt[1],pt[0])){
			if(CDT::Area(pt[1],pt[0],pt[2]) < 0.){
				nOutboundIndex = 2;
				return OUTER_BOUND;
			}else	return INNER_BOUND;
		}else if(IsBoundaryEdge(pt[1],pt[2])){
			if(CDT::Area(pt[1],pt[2],pt[0]) < 0.){
				nOutboundIndex = 0;
				return OUTER_BOUND;
			}else	return INNER_BOUND;
		}else if(IsBoundaryEdge(pt[2],pt[1])){
			if(CDT::Area(pt[2],pt[1],pt[0]) < 0.){
				nOutboundIndex = 0;
				return OUTER_BOUND;
			}else	return INNER_BOUND;
		}else if(IsBoundaryEdge(pt[2],pt[0])){
			if(CDT::Area(pt[2],pt[0],pt[1]) < 0.){
				nOutboundIndex = 1;
				return OUTER_BOUND;
			}else	return INNER_BOUND;
		}else if(IsBoundaryEdge(pt[0],pt[2])){
			if(CDT::Area(pt[0],pt[2],pt[1]) < 0.){
				nOutboundIndex = 1;
				return OUTER_BOUND;
			}else	return INNER_BOUND;
		}
		/*
		for(list<KPolyCurve*>::iterator itr=m_lstBoundaryCurve.begin();itr != m_lstBoundaryCurve.end();++itr){
			if((*itr)->HasEdge(pt[0],pt[1])){
				if(pDT->Area(pt[0],pt[1],pt[2]) < 0.){
					nOutboundIndex = 2;
					return OUTER_BOUND;
				}else	return INNER_BOUND;
			}else if((*itr)->HasEdge(pt[1],pt[2])){
				if(pDT->Area(pt[1],pt[2],pt[0]) < 0.){
					nOutboundIndex = 0;
					return OUTER_BOUND;
				}else	return INNER_BOUND;
			}else if((*itr)->HasEdge(pt[2],pt[0])){
				if(pDT->Area(pt[2],pt[0],pt[1]) < 0.){
					nOutboundIndex = 1;
					return OUTER_BOUND;
				}else	return INNER_BOUND;
			}else if((*itr)->HasEdge(pt[2],pt[1])){
				if(pDT->Area(pt[2],pt[1],pt[0]) < 0.){
					nOutboundIndex = 0;
					return OUTER_BOUND;
				}else	return INNER_BOUND;
			}else if((*itr)->HasEdge(pt[1],pt[0])){
				if(pDT->Area(pt[1],pt[0],pt[2]) < 0.){
					nOutboundIndex = 2;
					return OUTER_BOUND;
				}else	return INNER_BOUND;
			}else if((*itr)->HasEdge(pt[0],pt[2])){
				if(pDT->Area(pt[0],pt[2],pt[1]) < 0.){
					nOutboundIndex = 1;
					return OUTER_BOUND;
				}else	return INNER_BOUND;
			}
		}
		*/
	}

	return ON_BOUND;
}

/**	\brief	The CPlaneSurface::RemoveOutboundTriangles function

	\param	plstDT	a parameter of type list<CDT*>*

	\return	void	
*/
void CPlaneSurface::RemoveOutboundTriangles(list<CDT*>* plstDT){
	assert(plstDT && "plstDT is NULL");
	if(plstDT){
		queue<EDGE_T> queEdge;

		int nOutboundIndex=0;
		for(list<CDT*>::iterator itr=plstDT->begin();itr != plstDT->end();){
			if(OUTER_BOUND == IsOutboundTriangle(nOutboundIndex,*itr)){
				int nIndex[2]={0};
				
				nIndex[0] = (nOutboundIndex + 1)%3;
				nIndex[1] = (nOutboundIndex + 2)%3;
				
				EDGE_T edge;
				edge.pos[0].SetX((**itr)[nOutboundIndex].x);
				edge.pos[0].SetY((**itr)[nOutboundIndex].y);
				edge.pos[1].SetX((**itr)[nIndex[0]].x);
				edge.pos[1].SetY((**itr)[nIndex[0]].y);
				
				POINT_T pt[2]={0,};
				pt[0] = (**itr)[nOutboundIndex];
				pt[1] = (**itr)[nIndex[0]];
				if(!IsBoundaryEdge(pt[0],pt[1]) && !IsBoundaryEdge(pt[1],pt[0]))
					queEdge.push(edge);
				
				edge.pos[0].SetX((**itr)[nOutboundIndex].x);
				edge.pos[0].SetY((**itr)[nOutboundIndex].y);
				edge.pos[1].SetX((**itr)[nIndex[1]].x);
				edge.pos[1].SetY((**itr)[nIndex[1]].y);
				
				pt[0] = (**itr)[nOutboundIndex];
				pt[1] = (**itr)[nIndex[1]];
				if(!IsBoundaryEdge(pt[0],pt[1]) && !IsBoundaryEdge(pt[1],pt[0])) queEdge.push(edge);
				
				plstDT->erase(itr++);
			}else	itr++;
		}

		EDGE_T edge;
		while(!queEdge.empty()){
			edge = queEdge.front();queEdge.pop();
			for(list<CDT*>::iterator itr = plstDT->begin();itr != plstDT->end();){
				POINT_T pt[2]={0};

				pt[0].x = edge.pos[0].x();
				pt[0].y = edge.pos[0].y();
				pt[0].z = 0;
				pt[1].x = edge.pos[1].x();
				pt[1].y = edge.pos[1].y();
				pt[1].z = 0;
				if((*itr)->HasEdge(pt[0],pt[1])){
					if(	(((**itr)[0].x != pt[0].x) || ((**itr)[0].y != pt[0].y)) &&
						(((**itr)[0].x != pt[1].x) || ((**itr)[0].y != pt[1].y))){
						edge.pos[0].SetX((**itr)[0].x);
						edge.pos[0].SetY((**itr)[0].y);
						edge.pos[1].SetX((**itr)[1].x);
						edge.pos[1].SetY((**itr)[1].y);
						queEdge.push(edge);
						edge.pos[0].SetX((**itr)[0].x);
						edge.pos[0].SetY((**itr)[0].y);
						edge.pos[1].SetX((**itr)[2].x);
						edge.pos[1].SetY((**itr)[2].y);
						queEdge.push(edge);
					}/*else if((((**itr)[1].x != pt[0].x) || ((**itr)[1].y != pt[0].y)) &&
						 (((**itr)[1].x != pt[1].x) || ((**itr)[1].y != pt[1].y))){
						edge.pos[0].x = (**itr)[1].x;
						edge.pos[0].y = (**itr)[1].y;
						edge.pos[1].x = (**itr)[0].x;
						edge.pos[1].y = (**itr)[0].y;
						queEdge.Push(edge);
						edge.pos[0].x = (**itr)[1].x;
						edge.pos[0].y = (**itr)[1].y;
						edge.pos[1].x = (**itr)[2].x;
						edge.pos[1].y = (**itr)[2].y;
						queEdge.Push(edge);
					}else if((((**itr)[2].x != pt[0].x) || ((**itr)[2].y != pt[0].y)) &&
						 (((**itr)[2].x != pt[1].x) || ((**itr)[2].y != pt[1].y))){
						edge.pos[0].x = (**itr)[2].x;
						edge.pos[0].y = (**itr)[2].y;
						edge.pos[1].x = (**itr)[0].x;
						edge.pos[1].y = (**itr)[0].y;
						queEdge.Push(edge);
						edge.pos[0].x = (**itr)[2].x;
						edge.pos[0].y = (**itr)[2].y;
						edge.pos[1].x = (**itr)[1].x;
						edge.pos[1].y = (**itr)[1].y;
						queEdge.Push(edge);
					}*/
					plstDT->erase(itr++);
					break;
				}else	itr++;
			}
		}
	}
}

/**	\brief	The CPlaneSurface::CreateFacets function


	\return	list<CFacet*>*	
*/
CTinyFacetContainer* CPlaneSurface::CreateFacets(){
	CTinyFacetContainer* pFacetContainer=new CTinyFacetContainer;
	if(NULL == pFacetContainer) return NULL;
	
	CLibMesh2d mesh;
	list<CDT*>* plstDT=mesh.Mesh(m_pt,m_nPoints,m_lnBoundary,m_nEdge);

	if(plstDT){
		RemoveOutboundTriangles(plstDT);

		CPlane3d plane(m_sfcPlane.vecNorm);
		double alpha=0.,beta=0;
		CGeometry::GetRotatedAngleOfAxis(m_sfcPlane.vecNorm,alpha,beta);
		for(list<CDT*>::iterator itr=plstDT->begin();itr != plstDT->end();++itr){
			POINT_T pt[3]={0};
			bool    bShow[3]={false};

			pt[0] = (**itr)[0];
			pt[1] = (**itr)[1];
			pt[2] = (**itr)[2];
			if(IsBoundaryEdge(pt[0],pt[1])) bShow[0] = true;
			if(IsBoundaryEdge(pt[1],pt[2])) bShow[1] = true;
			if(IsBoundaryEdge(pt[2],pt[0])) bShow[2] = true;

			pt[0] = plane.RotateAboutYXAxis(pt[0],-beta,-alpha);
			pt[0].x += m_sfcPlane.ptOrigin.x;
			pt[0].y += m_sfcPlane.ptOrigin.y;
			pt[0].z += m_sfcPlane.ptOrigin.z;
			pt[1] = plane.RotateAboutYXAxis(pt[1],-beta,-alpha);
			pt[1].x += m_sfcPlane.ptOrigin.x;
			pt[1].y += m_sfcPlane.ptOrigin.y;
			pt[1].z += m_sfcPlane.ptOrigin.z;
			pt[2] = plane.RotateAboutYXAxis(pt[2],-beta,-alpha);
			pt[2].x += m_sfcPlane.ptOrigin.x;
			pt[2].y += m_sfcPlane.ptOrigin.y;
			pt[2].z += m_sfcPlane.ptOrigin.z;

			CFacet* pFacet=NULL;
			if(pFacet = new CFacet(pt[0],pt[1],pt[2])){
				pFacet->m_bShow[0] = bShow[0];
				pFacet->m_bShow[1] = bShow[1];
				pFacet->m_bShow[2] = bShow[2];
				//m_lstFacet.push_back(pFacet);
				pFacetContainer->Add(pFacet);
			}
		}
	}

	return pFacetContainer;
}

/**	\brief	The CPlaneSurface::AppendPoint function

	\param	pt	a parameter of type POINT_T&

	\return	void	
*/
bool CPlaneSurface::AppendPoint(POINT_T& pt){
	if(m_nPoints > 0){
		POINT_T ptLast=m_pt[m_nPoints - 1];
		if(IsSamePoint(ptLast,pt)) return false;
	}
	m_pt[m_nPoints++] = pt;
	
	return false;
}