// OpenSolid.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <math.h>

//#include "d:\\project\\spoon_0_0_1\\utility\\profile.h"

#include <SQLib.h>
#include <graphics/curve3d.h>
#include <graphics/ellipse3d.h>
#include <graphics/arc3d.h>
#include "cubic_spline.h"
#include "Cone.h"
#include "PlaneSurface.h"
#include "ConeSurface.h"
#include "TorusSurface.h"
#include "SphereSurface.h"
#include "SmartSolid.h"

using namespace mesh;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

bool IsValidEdge(EDGE_T edge)
{
	const static double nTol=0.000000001;

	double du=edge.pos[1].x() - edge.pos[0].x();
	double dv=edge.pos[1].y() - edge.pos[0].y();
	if((0. == du) && (0. == dv)) return false;
	//if((fabs(du) < nTol) && (fabs(dv) < nTol)) return false;
	
	return true;
}

bool IsSameEdge(EDGE_T edge1,EDGE_T edge2)
{
	const static double nTol=0.000000001;

	double du[2]={0};
	double dv[2]={0};
	du[0] = edge1.pos[0].x() - edge2.pos[0].x();
	dv[0] = edge1.pos[0].y() - edge2.pos[0].y();
	du[1] = edge1.pos[1].x() - edge2.pos[1].x();
	dv[1] = edge1.pos[1].y() - edge2.pos[1].y();
	if((fabs(du[0]) < nTol) && (fabs(du[1]) < nTol) && (fabs(dv[0]) < nTol) && (fabs(dv[1]) < nTol)) return true;
	
	du[0] = edge1.pos[0].x() - edge2.pos[1].x();
	dv[0] = edge1.pos[0].y() - edge2.pos[1].y();
	du[1] = edge1.pos[1].x() - edge2.pos[0].x();
	dv[1] = edge1.pos[1].y() - edge2.pos[0].y();
	if((fabs(du[0]) < nTol) && (fabs(du[1]) < nTol) && (fabs(dv[0]) < nTol) && (fabs(dv[1]) < nTol)) return true;

	return false;
}

/**	\brief
*/
COpenSolid::COpenSolid()
{
	m_plstPrimitive    = NULL;
	m_pPrimitivePool   = NULL;
	m_pPointVectorPool = NULL;
	m_pVertexListPool  = NULL;
}

/**	\brief
*/
COpenSolid::~COpenSolid()
{
}

/**	\brief	The COpenSolid::new_primitive function


	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE COpenSolid::new_primitive()
{
	PSQPRIMITIVE pRet=NULL;

	if(NULL == m_pPrimitivePool) m_pPrimitivePool= new boost::pool<>(sizeof(SQPRIMITIVE));
	if(NULL != m_pPrimitivePool)
	{
		pRet=(PSQPRIMITIVE)(m_pPrimitivePool->malloc());
		memset(pRet,'\0',sizeof(SQPRIMITIVE));
	}
	
	return pRet;
}

/**	\brief	The COpenSolid::Pure function


	\return	void	
*/
void COpenSolid::Pure()
{
	try
	{
		if(NULL != m_pPrimitivePool)
		{
			delete m_pPrimitivePool;
			m_pPrimitivePool = NULL;
		}

		if(NULL != m_pPointVectorPool)
		{
			delete m_pPointVectorPool;
			m_pPointVectorPool = NULL;
		}

		if(NULL != m_pVertexListPool)
		{
			delete m_pVertexListPool;
			m_pVertexListPool = NULL;
		}
	}
	catch(...)
	{
		OutputDebugString("Error in Pure()");
	}

#ifdef _DEBUG
	OutputDebugString("COpenSolid::Pure()");
#endif
}

/**	\brief	The COpenSolid::GetNumOfCurveSegments function


	\return	int	
*/
const int COpenSolid::GetNumOfCurveSegments()
{
	return 10;//m_nCurveSegments;
}

/**	\brief	The COpenSolid::SetNumOfCurveSegments function

	\param	nCurveSegments	a parameter of type int

	\return	void	
*/
void COpenSolid::SetNumOfCurveSegments(int nCurveSegments)
{
	assert((nCurveSegments > 0) && "nCurveSegments is less than 0");

	if(nCurveSegments)
	{
		theOpenSolid.m_nCurveSegments = nCurveSegments;
	}
}

/**	\brief	The COpenSolid::ComputeVolume function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	void	
*/
void COpenSolid::ComputeVolume(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		VOLUME_T volume={0};

		if(ENM_GROUP == pPrimitive->hdr.id)
		{
			if(pPrimitive->body.pPrimitive)
			{
				volume = pPrimitive->body.pPrimitive->hdr.volume;
				for(PSQPRIMITIVE ptr=pPrimitive->body.pPrimitive->next;ptr;ptr=ptr->next)
				{
					if(volume.x[0] > ptr->hdr.volume.x[0]) volume.x[0] = ptr->hdr.volume.x[0];
					if(volume.x[1] < ptr->hdr.volume.x[1]) volume.x[1] = ptr->hdr.volume.x[1];
					if(volume.y[0] > ptr->hdr.volume.y[0]) volume.y[0] = ptr->hdr.volume.y[0];
					if(volume.y[1] < ptr->hdr.volume.y[1]) volume.y[1] = ptr->hdr.volume.y[1];
					if(volume.z[0] > ptr->hdr.volume.z[0]) volume.z[0] = ptr->hdr.volume.z[0];
					if(volume.z[1] < ptr->hdr.volume.z[1]) volume.z[1] = ptr->hdr.volume.z[1];
				}
				pPrimitive->hdr.volume = volume;
			}
		}
	}
}

/**	\brief	The COpenSolid::CreateFacets function

	\param	pptPoints	a parameter of type PPOINT_T
	\param	nParts	a parameter of type int
	\param	nPoints	a parameter of type int
	\param	hdr	a parameter of type const SQHEADER&

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE COpenSolid::CreateFacets(PPOINT_T pptPoints,int nParts,int nPoints,const SQHEADER& hdr)
{
	PSQPRIMITIVE pRet=NULL;

	for(int j = 0;j < nParts;j++)
	{
		for(int i = 0;i < nPoints - 1;i++)
		{
			PSQPRIMITIVE pPolyline=new_primitive();
			if(pPolyline)
			{
				memcpy(&(pPolyline->hdr),&hdr,sizeof(SQHEADER));
				pPolyline->hdr.id     = ENM_POLYLINE;
				pPolyline->hdr.attr.d = 3;
				pPolyline->hdr.attr.f = 1;
				pPolyline->hdr.attr.closed = 1;
			
				double xMin=pptPoints[j*(nPoints) + i].x;
				double xMax=xMin;
				double yMin=pptPoints[j*(nPoints) + i].y;
				double yMax=yMin;
				double zMin=pptPoints[j*(nPoints) + i].z;
				double zMax=zMin;

				PPOINT_TNODE ptr=(PPOINT_TNODE)calloc(1,sizeof(SQPOINTNODE));
				if(ptr)
				{
					ptr->pt = pptPoints[j*(nPoints) + i];
					if(0 == j) ptr->visible = true;

					pPolyline->body.polyline = ListPush(pPolyline->body.polyline,ptr);
				}
				ptr=(PPOINT_TNODE)calloc(1,sizeof(SQPOINTNODE));
				if(ptr)
				{
					ptr->pt = pptPoints[j*(nPoints) + i + 1];
					if(xMin > pptPoints[j*(nPoints) + i + 1].x) xMin = pptPoints[j*(nPoints) + i + 1].x;
					if(xMax < pptPoints[j*(nPoints) + i + 1].x) xMax = pptPoints[j*(nPoints) + i + 1].x;
					if(yMin > pptPoints[j*(nPoints) + i + 1].y) yMin = pptPoints[j*(nPoints) + i + 1].y;
					if(yMax < pptPoints[j*(nPoints) + i + 1].y) yMax = pptPoints[j*(nPoints) + i + 1].y;
					if(zMin > pptPoints[j*(nPoints) + i + 1].z) zMin = pptPoints[j*(nPoints) + i + 1].z;
					if(zMax < pptPoints[j*(nPoints) + i + 1].z) zMax = pptPoints[j*(nPoints) + i + 1].z;
					ptr->visible = true;

					pPolyline->body.polyline = ListPush(pPolyline->body.polyline,ptr);
				}
				ptr=(PPOINT_TNODE)calloc(1,sizeof(SQPOINTNODE));
				if(ptr)
				{
					ptr->pt = pptPoints[j*(nPoints) + nPoints + i + 1];
					if(xMin > pptPoints[j*(nPoints) + nPoints + i + 1].x) xMin = pptPoints[j*(nPoints) + nPoints + i + 1].x;
					if(xMax < pptPoints[j*(nPoints) + nPoints + i + 1].x) xMax = pptPoints[j*(nPoints) + nPoints + i + 1].x;
					if(yMin > pptPoints[j*(nPoints) + nPoints + i + 1].y) yMin = pptPoints[j*(nPoints) + nPoints + i + 1].y;
					if(yMax < pptPoints[j*(nPoints) + nPoints + i + 1].y) yMax = pptPoints[j*(nPoints) + nPoints + i + 1].y;
					if(zMin > pptPoints[j*(nPoints) + nPoints + i + 1].z) zMin = pptPoints[j*(nPoints) + nPoints + i + 1].z;
					if(zMax < pptPoints[j*(nPoints) + nPoints + i + 1].z) zMax = pptPoints[j*(nPoints) + nPoints + i + 1].z;
					if(nParts - 1 == j) ptr->visible = true;

					pPolyline->body.polyline = ListPush(pPolyline->body.polyline,ptr);
				}
				ptr=(PPOINT_TNODE)calloc(1,sizeof(SQPOINTNODE));
				if(ptr)
				{
					ptr->pt = pptPoints[j*(nPoints) + nPoints + i];
					if(xMin > pptPoints[j*(nPoints) + nPoints + i].x) xMin = pptPoints[j*(nPoints) + nPoints + i].x;
					if(xMax < pptPoints[j*(nPoints) + nPoints + i].x) xMax = pptPoints[j*(nPoints) + nPoints + i].x;
					if(yMin > pptPoints[j*(nPoints) + nPoints + i].y) yMin = pptPoints[j*(nPoints) + nPoints + i].y;
					if(yMax < pptPoints[j*(nPoints) + nPoints + i].y) yMax = pptPoints[j*(nPoints) + nPoints + i].y;
					if(zMin > pptPoints[j*(nPoints) + nPoints + i].z) zMin = pptPoints[j*(nPoints) + nPoints + i].z;
					if(zMax < pptPoints[j*(nPoints) + nPoints + i].z) zMax = pptPoints[j*(nPoints) + nPoints + i].z;
					ptr->visible = true;

					pPolyline->body.polyline = ListPush(pPolyline->body.polyline,ptr);
				}
				ptr=(PPOINT_TNODE)calloc(1,sizeof(SQPOINTNODE));
				if(ptr)
				{
					ptr->pt = pptPoints[j*(nPoints) + i];
					ptr->visible = true;

					pPolyline->body.polyline = ListPush(pPolyline->body.polyline,ptr);
				}
				pPolyline->hdr.volume.x[LOW] = xMin;
				pPolyline->hdr.volume.x[HIGH]= xMax;
				pPolyline->hdr.volume.y[LOW] = yMin;
				pPolyline->hdr.volume.y[HIGH]= yMax;
				pPolyline->hdr.volume.z[LOW] = zMin;
				pPolyline->hdr.volume.z[HIGH]= zMax;

				pRet = ListPushFront(pRet,pPolyline);
			}
		}
	}

	return pRet;
}


/**	
	@brief	if plane-surface has only one loop, create a points list.\n
		if plane-surface has more than two loops, crate a instance of CPlaneSurface.

	@author	HumKyung.BAEK

	@date	?

	@param	pPrimitive	a parameter of type PSQPRIMITIVE

	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE COpenSolid::CreatePlaneSurface(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	assert(pPrimitive->body.pPlaneSurf && "pPrimitive->pPlaneSurf is NULL");
	PSQPRIMITIVE pRet=NULL,pTail=NULL;

	try
	{
		if(pPrimitive && pPrimitive->body.pPlaneSurf)
		{
			PSQPRIMITIVE pGroup=pPrimitive->body.pPlaneSurf->pBoundary;
			if(pGroup && pGroup->next)
			{
				CPlaneSurface PlaneSurface(*(pPrimitive->body.pPlaneSurf));
				CTinyFacetContainer* pFacetContainer = PlaneSurface.CreateFacets();
				if(pFacetContainer)
				{
					const long nSize=pFacetContainer->Size();
					for(long i=0;i < nSize;i++)
					{
						CFacet* pFacet=pFacetContainer->GetAt(i);
						
						PSQPRIMITIVE pPlane = theOpenSolid.new_primitive();
						if(pPlane)
						{
							if(pPlane->body.plstVertices = new_vertex_list())
							{
								pPlane->hdr.id = SQ_VERTICES;
								pPlane->hdr.attr.f = 1;
								
								for(int i=0;i < pFacet->m_nPoints;++i)
								{
									SQVERTEX vertex={0};
									
									vertex.code.moveto = pFacet->m_bShow[i] ? 0 : 1;
									vertex.x = pFacet->m_pt[i].x;
									vertex.y = pFacet->m_pt[i].y;
									vertex.z = pFacet->m_pt[i].z;
									pPlane->body.plstVertices->push_back(vertex);
									
								}
								if(	(pFacet->m_pt[0].x != pFacet->m_pt[pFacet->m_nPoints - 1].x) || 
									(pFacet->m_pt[0].y != pFacet->m_pt[pFacet->m_nPoints - 1].y) ||
									(pFacet->m_pt[0].z != pFacet->m_pt[pFacet->m_nPoints - 1].z))
								{
									SQVERTEX vertex={0};
									
									vertex.code.moveto = 0;
									vertex.x = pFacet->m_pt[0].x;
									vertex.y = pFacet->m_pt[0].y;
									vertex.z = pFacet->m_pt[0].z;
									pPlane->body.plstVertices->push_back(vertex);
								}
								
								if(NULL == pRet)
									pRet = pTail = ListPush(pRet,pPlane);
								else	pTail = ListPush(pTail,pPlane);
								pTail = pPlane;
							}
						}
					}
					delete pFacetContainer;
				}
			}
			else if(pGroup) /// only one boundary curve
			{
				pRet = theOpenSolid.new_primitive();
				if(pRet)
				{
					pRet->hdr.id     = ENM_GROUP;
					pRet->hdr.attr.f = 1;
					pRet->hdr.attr.d = 3;
					if(pGroup->body.pPrimitive && (NULL == pGroup->body.pPrimitive->next))
					{
						pRet->body.pPrimitive = pGroup->body.pPrimitive;
					}
					else
					{
						CPlaneSurface PlaneSurface(*(pPrimitive->body.pPlaneSurf));
						CPolyCurve* pBoundary = PlaneSurface.GetBoundaryCurve(0);
						if(NULL != pBoundary)
						{
							pRet->body.pPrimitive = theOpenSolid.new_primitive();
							pRet->body.pPrimitive->hdr.id = SQ_POINTS;
							pRet->body.pPrimitive->hdr.attr.f = 1;
							pRet->body.pPrimitive->hdr.attr.d = 3;
							pRet->body.pPrimitive->hdr.attr.continuous = 1;
							pRet->body.pPrimitive->body.pvtPoints = theOpenSolid.new_point_vector();
							
							double alpha=0.,beta=0;
							CGeometry::GetRotatedAngleOfAxis(pPrimitive->body.pPlaneSurf->vecNorm,alpha,beta);
			
							long count = pBoundary->GetNumOfPoints();
							for(int i = 0;i < count; i++)
							{
								POINT_T pt = (*pBoundary)[i];
								pt = CGeometry::RotateAboutYXAxis(pt,-beta,-alpha);
								pt.x += pPrimitive->body.pPlaneSurf->ptOrigin.x;
								pt.y += pPrimitive->body.pPlaneSurf->ptOrigin.y;
								pt.z += pPrimitive->body.pPlaneSurf->ptOrigin.z;
								pRet->body.pPrimitive->body.pvtPoints->push_back(pt);
							}
						}
					}
				}
			}
		}
		
		return pRet;
	}
	catch(const exception ex)
	{
		throw ex;
	}
	catch(...)
	{
		throw exception("throw exception in COpenSolid::CreatePlaneSurface");
	}
}

/**	
	@brief	check if this primitive can be cone.

	@author	HumKyung.BAEK

	@param	pPrimitive	a parameter of type PSQPRIMITIVE

	@return	bool	
*/
bool COpenSolid::PossibleBeCone(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	bool bRet=false;

	if(pPrimitive)
	{
		if((SQ_CONESURFACE == pPrimitive->hdr.id) && (2 == pPrimitive->body.pConeSurf->nLoop))
		{
			if((ENM_ARC == pPrimitive->body.pConeSurf->pBoundary[0]->hdr.id) && (360. == pPrimitive->body.pConeSurf->pBoundary[0]->body.arc.nSweep) &&
			   (ENM_ARC == pPrimitive->body.pConeSurf->pBoundary[1]->hdr.id) && (360. == pPrimitive->body.pConeSurf->pBoundary[1]->body.arc.nSweep))
			{
				bRet = true;
			}
			else if((ENM_ARC == pPrimitive->body.pConeSurf->pBoundary[0]->hdr.id) && (360. == pPrimitive->body.pConeSurf->pBoundary[0]->body.arc.nSweep) &&
			        (ENM_POINT == pPrimitive->body.pConeSurf->pBoundary[1]->hdr.id))
			{
				bRet = true;
			}
			else if((ENM_POINT == pPrimitive->body.pConeSurf->pBoundary[0]->hdr.id) &&
				(ENM_ARC == pPrimitive->body.pConeSurf->pBoundary[1]->hdr.id) && (360. == pPrimitive->body.pConeSurf->pBoundary[1]->body.arc.nSweep))
			        
			{
				bRet = true;
			}
		}
	}

	return bRet;
}

/**	\brief	The COpenSolid::PossibleBeDome function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	bool	
*/
bool COpenSolid::PossibleBeDome(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	static const double _nTol = 0.00001;

	if(pPrimitive)
	{
		double nSweepAngle=0.;
		for(SQSURFACE::PSURFELM_T ptrElm=pPrimitive->body.pSurface->pElement;ptrElm;ptrElm=ptrElm->next)
		{
			for(PSQPRIMITIVE ptr = ptrElm->pBoundaryCurve;ptr;ptr=ptr->next)
			{
				if(ENM_ELLIPSE == ptr->hdr.id)
				{
					const double d = 90.f - ptr->body.ellipse.nSweepAngle;
					if(d > _nTol) return false;
				}
				else if(ENM_ARC == ptr->hdr.id)
				{
					const double d = fabs(90.f - ptr->body.arc.nSweep);
					if(d > _nTol) return false;
				}
			}

			for(PSQPRIMITIVE ptr = ptrElm->pRuleCurve;ptr;ptr=ptr->next)
			{
				if(ENM_ELLIPSE == ptr->hdr.id)
				{
					nSweepAngle += ptr->body.ellipse.nSweepAngle;
					if(!(fabs(1-fabs(ptr->body.ellipse.vecNorm.dx)) < _nTol) && 
					   !(fabs(1-fabs(ptr->body.ellipse.vecNorm.dy)) < _nTol) &&
					   !(fabs(1-fabs(ptr->body.ellipse.vecNorm.dz)) < _nTol)) return false;
				}
				else if(ENM_ARC == ptr->hdr.id)
				{
					nSweepAngle += ptr->body.arc.nSweep;
					if(!(fabs(1-fabs(ptr->body.arc.vecNorm.dx)) < _nTol) && 
					   !(fabs(1-fabs(ptr->body.arc.vecNorm.dy)) < _nTol) &&
					   !(fabs(1-fabs(ptr->body.arc.vecNorm.dz)) < _nTol)) return false;
				}else	return false;
			}
		}

		if(360. == nSweepAngle) return true;
	}

	return false;
}

/**	\brief	The COpenSolid::PossibleBeSphere function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	bool	
*/
bool COpenSolid::PossibleBeSphere(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		double nSweepAngle=0.;
		for(SQSURFACE::PSURFELM_T ptrElm=pPrimitive->body.pSurface->pElement;ptrElm;ptrElm=ptrElm->next)
		{
			for(PSQPRIMITIVE ptr = ptrElm->pBoundaryCurve;ptr;ptr=ptr->next)
			{
				if(!((ENM_ELLIPSE == ptr->hdr.id) && (180. == fabs(ptr->body.ellipse.nSweepAngle))))
					return false;
			}

			for(PSQPRIMITIVE ptr = ptrElm->pRuleCurve;ptr;ptr=ptr->next)
			{
				if(ENM_ELLIPSE == ptr->hdr.id) nSweepAngle += ptr->body.ellipse.nSweepAngle;
			}
		}

		if(360. == nSweepAngle) return true;
	}

	return false;
}

/**	
	@brief	The COpenSolid::CreateConeSurface function

	@author	HumKyung.BAEK

	@param	pPrimitive	a parameter of type PSQPRIMITIVE

	@remarks\n
	;2003.07.10 - copy "LOOP" to primitive's szDesc.

	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE COpenSolid::CreateConeSurface(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	PSQPRIMITIVE pRet=NULL;

	try
	{
		if(pPrimitive && PossibleBeCone(pPrimitive))
		{
			strcpy(pPrimitive->hdr.szDesc,"CONE");
			pRet = pPrimitive;
		}
		else if(pPrimitive)
		{
			CONE_T cone={0,};
			if(CTinyCone::IsKindOf(cone,pPrimitive))
			{
				CTinyCone tinyCone(cone);
				CTinyFacetContainer* pFacetContainer=tinyCone.CreateFacets();
				if(pFacetContainer)
				{
					const long nSize=pFacetContainer->Size();
					for(long i=0;i < nSize;i++)
					{
						CFacet* pFacet=pFacetContainer->GetAt(i);
						
						PSQPRIMITIVE pPrimitive = new_primitive();
						if(pPrimitive)
						{
							if(pPrimitive->body.pvtPoints = new_point_vector())
							{
								pPrimitive->hdr.id = SQ_POINTS;
								pPrimitive->hdr.attr.f = 1;
								pPrimitive->hdr.attr.d = 3;
								pPrimitive->hdr.attr.continuous = 1;
								strcpy(pPrimitive->hdr.szDesc,"LOOP");
								
								int j=0;
								for(j=0;j < pFacet->m_nPoints;++j)
								{
									POINT_T pt={0};
									
									pt = pFacet->m_pt[j];
									pPrimitive->body.pvtPoints->push_back(pFacet->m_pt[j]);
								}
								if(0 != memcmp(&(pFacet->m_pt[0]),&(pFacet->m_pt[j - 1]),sizeof(POINT_T)))
								{
									pPrimitive->body.pvtPoints->push_back(pFacet->m_pt[0]);
								}
								
								pRet = ListPush(pRet,pPrimitive);
							}else	delete_primitive(pPrimitive);
						}
					}
					delete pFacetContainer;
				}
			}
			else
			{
				CConeSurface ConeSurface(*(pPrimitive->body.pConeSurf),pPrimitive->hdr.attr.reversed ? true : false);
				CTinyFacetContainer* pFacetContainer=ConeSurface.CreateFacets();
				if(pFacetContainer)
				{
					const long nSize=pFacetContainer->Size();
					for(long i=0;i < nSize;i++)
					{
						CFacet* pFacet=pFacetContainer->GetAt(i);

						PSQPRIMITIVE pPrimitive = new_primitive();
						if(pPrimitive)
						{
							memset(pPrimitive,0,sizeof(SQPRIMITIVE));
							if(pPrimitive->body.pvtPoints = new vector<POINT_T>)
							{
								pPrimitive->hdr.id = SQ_POINTS;
								pPrimitive->hdr.attr.f = 1;
								pPrimitive->hdr.attr.d = 3;
								pPrimitive->hdr.attr.continuous = 1;
								strcpy(pPrimitive->hdr.szDesc,"LOOP");
								
								int j=0;
								for(j=0;j < pFacet->m_nPoints;++j)
								{
									POINT_T pt={0};
									
									pt = pFacet->m_pt[j];
									pPrimitive->body.pvtPoints->push_back(pFacet->m_pt[j]);
								}
								if(0 != memcmp(&(pFacet->m_pt[0]),&(pFacet->m_pt[j - 1]),sizeof(POINT_T)))
								{
									pPrimitive->body.pvtPoints->push_back(pFacet->m_pt[0]);
								}
								
								pRet = ListPush(pRet,pPrimitive);
							}else	delete_primitive(pPrimitive);
						}
					}
					delete pFacetContainer;
				}
			}
		}

		return pRet;
	}
	catch(const exception ex)
	{
		throw ex;
	}
	catch(...)
	{
		throw exception("throw exception in COpenSolid::CreateConeSurface");
	}
}

/**	\brief	The COpenSolid::CreateTorusSurface function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\remarks\n
	;2003.07.10 - copy "LOOP" to primitive's szDesc.

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE COpenSolid::CreateTorusSurface(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	PSQPRIMITIVE pRet=NULL;

	if(pPrimitive)
	{
		CTorusSurface torussurface(*(pPrimitive->body.pTorusSurf),pPrimitive->hdr.attr.reversed ? true : false);
		CTinyFacetContainer* pFacetContainer=torussurface.CreateFacets();
		if(pFacetContainer)
		{
			const long nSize=pFacetContainer->Size();
			for(long i=0;i < nSize;i++)
			{
				CFacet* pFacet=pFacetContainer->GetAt(i);

				PSQPRIMITIVE pPrimitive = (PSQPRIMITIVE)new_primitive();
				if(pPrimitive)
				{
					if(pPrimitive->body.pvtPoints = new_point_vector())
					{
						pPrimitive->hdr.id = SQ_POINTS;
						pPrimitive->hdr.attr.f = 1;
						pPrimitive->hdr.attr.d = 3;
						pPrimitive->hdr.attr.continuous = 1;
						strcpy(pPrimitive->hdr.szDesc,"LOOP");
						
						for(int i=0;i < pFacet->m_nPoints;++i)
						{
							pPrimitive->body.pvtPoints->push_back(pFacet->m_pt[i]);
						}
						if(	(pFacet->m_pt[0].x != pFacet->m_pt[pFacet->m_nPoints - 1].x) || 
							(pFacet->m_pt[0].y != pFacet->m_pt[pFacet->m_nPoints - 1].y) ||
							(pFacet->m_pt[0].z != pFacet->m_pt[pFacet->m_nPoints - 1].z))
						{
							pPrimitive->body.pvtPoints->push_back(pFacet->m_pt[0]);
						}
						
						pRet = ListPush(pRet,pPrimitive);
					}else	delete_primitive(pPrimitive);
				}
			}
			delete pFacetContainer;
		}
	}

	return pRet;
}

/**	\brief	The COpenSolid::CreateSphereSurface function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\remarks	[2003.07.10] - copy "LOOP" to primitive's szDesc.

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE COpenSolid::CreateSphereSurface(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	PSQPRIMITIVE pRet=NULL;

	if(pPrimitive)
	{
		CSphereSurface spheresurface(*(pPrimitive->body.pSphereSurf),pPrimitive->hdr.attr.reversed ? true : false);
		CTinyFacetContainer* pFacetContainer=spheresurface.CreateFacets();
		if(pFacetContainer)
		{
			const long nSize=pFacetContainer->Size();
			for(long i=0;i < nSize;i++)
			{
				CFacet* pFacet=pFacetContainer->GetAt(i);

				PSQPRIMITIVE pPrimitive = (PSQPRIMITIVE)new_primitive();
				if(pPrimitive)
				{
					if(pPrimitive->body.pvtPoints = new_point_vector())
					{
						pPrimitive->hdr.id = SQ_POINTS;
						pPrimitive->hdr.attr.f = 1;
						pPrimitive->hdr.attr.d = 3;
						pPrimitive->hdr.attr.continuous = 1;
						strcpy(pPrimitive->hdr.szDesc,"LOOP");
						
						for(int i=0;i < pFacet->m_nPoints;++i)
						{
							pPrimitive->body.pvtPoints->push_back(pFacet->m_pt[i]);
						}
						if(	(pFacet->m_pt[0].x != pFacet->m_pt[pFacet->m_nPoints - 1].x) || 
							(pFacet->m_pt[0].y != pFacet->m_pt[pFacet->m_nPoints - 1].y) ||
							(pFacet->m_pt[0].z != pFacet->m_pt[pFacet->m_nPoints - 1].z))
						{
							pPrimitive->body.pvtPoints->push_back(pFacet->m_pt[0]);
						}
						
						pRet = ListPush(pRet,pPrimitive);
					}else	delete_primitive(pPrimitive);
				}
			}
			delete pFacetContainer;
		}
	}

	return pRet;
}

/**	\brief	The COpenSolid::CreateSolid function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE COpenSolid::CreateSolid(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	PSQPRIMITIVE pRet=NULL;

	if(pPrimitive)
	{
		/*
		if(SURFACE_OF_REVOLUTION == pPrimitive->body.pSurface->nType){ // surface of revolution
			int nElements=0;
			for(PSQPRIMITIVE ptr=pPrimitive->body.solid.pPrimitive;ptr;ptr = ptr->next){
				if((ENM_ELLIPSE == ptr->hdr.id) && (SQ_PRIMARY == ptr->hdr.attr.type)){
					CEllipse3D ellipse(ptr->body.ellipse);
					nElements = ellipse.GetNumOfPoints();
				}else if((SQ_POINTS == ptr->hdr.id) && (SQ_PRIMARY == ptr->hdr.attr.type)){
					nElements = 0;
					for(vector<POINT_T>::iterator itr=ptr->body.pvtPoints->begin();itr != ptr->body.pvtPoints->end();++itr){
						CCurve3d::m_pt(nElements++) = (*itr);
					}
				}else if((ENM_ELLIPSE == ptr->hdr.id) && (SQ_RULE == ptr->hdr.attr.type)){
					for(int i=0;i < nElements;i++){
						CCurve3d::pt(i).x -= ptr->body.ellipse.ptOrigin.x;
						CCurve3d::pt(i).y -= ptr->body.ellipse.ptOrigin.y;
						CCurve3d::pt(i).z -= ptr->body.ellipse.ptOrigin.z;
					}
					CCurve3d::Revolve(ptr->body.ellipse.vecNorm,nElements,CCurve3d::m_pt,ptr->body.ellipse.nSweepAngle,8);
					for(i=0;i < nElements*9;i++){
						CCurve3d::pt(i).x += ptr->body.ellipse.ptOrigin.x;
						CCurve3d::pt(i).y += ptr->body.ellipse.ptOrigin.y;
						CCurve3d::pt(i).z += ptr->body.ellipse.ptOrigin.z;
					}

					PSQPRIMITIVE pFacets=CreateFacets(CCurve3d::m_pt,8,nElements,pPrimitive->hdr);
					if(pFacets) pRet = ListPush(pRet,pFacets);

					PSQPRIMITIVE ptrPrev=NULL;
					do{
						ptrPrev = ptr;
						ptr = ptr->next;
					}while(ptr && (SQ_RULE == ptr->hdr.attr.type));
					ptr = ptrPrev;
				}
			}
		}
		*/
	}
	return pRet;
}

POINT_T pt[2][3072]={0, };

/**	
	@brief	create surface facets for sweep surface and surface of revolution.

	@author	humkyung

	@param	pPrimitive	a parameter of type PSQPRIMITIVE

	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE COpenSolid::CreateSurface(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	PSQPRIMITIVE pRet=NULL;

	if(pPrimitive)
	{
		// Sweep Surface
		if(SWEEP_SURF == pPrimitive->body.pSurface->nType)
		{	
			int nCount[2]={0,0};

			int nIndex=0;
			for(SQSURFACE::PSURFELM_T ptrElement=pPrimitive->body.pSurface->pElement;ptrElement;ptrElement=ptrElement->next)
			{
				nCount[0] = nCount[1] = 0;
				for(PSQPRIMITIVE ptr=ptrElement->pBoundaryCurve;(nIndex < 2) && ptr;nIndex++)
				{
					for(int i=0;(i < pPrimitive->body.pSurface->nBoundary) && ptr;ptr = ptr->next,i++)
					{
						if(ptr && (ENM_LINE == ptr->hdr.id))
						{
							pt[nIndex][nCount[nIndex]] = ptr->body.line.ptStart;
							(nCount[nIndex])++;
							pt[nIndex][nCount[nIndex]] = ptr->body.line.ptEnd;
							(nCount[nIndex])++;
						}
						else if(ptr && (SQ_POINTS == ptr->hdr.id))
						{
							for(vector<POINT_T>::iterator itr=ptr->body.pvtPoints->begin();itr != ptr->body.pvtPoints->end();++itr)
							{
								pt[nIndex][nCount[nIndex]]=*itr;
								(nCount[nIndex])++;
							}
						}
						else if(ptr && (ENM_ELLIPSE == ptr->hdr.id))
						{
							CEllipse3d ellipse(ptr->body.ellipse);
							ellipse.CreateSegments();
							int nPoints = ellipse.GetNumOfPoints();
							
							for(int i=0;i < nPoints;i++)
							{
								pt[nIndex][nCount[nIndex]] = CCurve3d::pt(i);
								(nCount[nIndex])++;
							}
						}
					}
				}
				
				if(nCount[0] == nCount[1])
				{
					for(int i=0;i < nCount[0] - 1;i++)
					{
						PSQPRIMITIVE pElement=new_primitive();
						if(pElement && (pElement->body.pvtPoints = new_point_vector()))
						{
							memcpy(&(pElement->hdr),&(pPrimitive->hdr),sizeof(SQHEADER));
							pElement->hdr.id = SQ_POINTS;
							pElement->hdr.attr.continuous  = 1;
							pElement->hdr.display.lcolor = ptrElement->pBoundaryCurve->hdr.display.lcolor;
							pElement->hdr.display.rgb[0]= ptrElement->pBoundaryCurve->hdr.display.rgb[0];
							pElement->hdr.display.rgb[1]= ptrElement->pBoundaryCurve->hdr.display.rgb[1];
							pElement->hdr.display.rgb[2]= ptrElement->pBoundaryCurve->hdr.display.rgb[2];
							pElement->hdr.attr.closed = 1;
							strcpy(pElement->hdr.szDesc,"LOOP");

							double xMin=pt[0][i].x;
							double xMax=xMin;
							double yMin=pt[0][i].y;
							double yMax=yMin;
							double zMin=pt[0][i].z;
							double zMax=zMin;
							if(xMin > pt[0][i+1].x) xMin = pt[0][i+1].x;
							if(xMax < pt[0][i+1].x) xMax = pt[0][i+1].x;
							if(yMin > pt[0][i+1].y) yMin = pt[0][i+1].y;
							if(yMax < pt[0][i+1].y) yMax = pt[0][i+1].y;
							if(zMin > pt[0][i+1].z) zMin = pt[0][i+1].z;
							if(zMax < pt[0][i+1].z) zMax = pt[0][i+1].z;
							if(xMin > pt[1][i+1].x) xMin = pt[1][i+1].x;
							if(xMax < pt[1][i+1].x) xMax = pt[1][i+1].x;
							if(yMin > pt[1][i+1].y) yMin = pt[1][i+1].y;
							if(yMax < pt[1][i+1].y) yMax = pt[1][i+1].y;
							if(zMin > pt[1][i+1].z) zMin = pt[1][i+1].z;
							if(zMax < pt[1][i+1].z) zMax = pt[1][i+1].z;
							if(xMin > pt[1][i].x) xMin = pt[1][i].x;
							if(xMax < pt[1][i].x) xMax = pt[1][i].x;
							if(yMin > pt[1][i].y) yMin = pt[1][i].y;
							if(yMax < pt[1][i].y) yMax = pt[1][i].y;
							if(zMin > pt[1][i].z) zMin = pt[1][i].z;
							if(zMax < pt[1][i].z) zMax = pt[1][i].z;
							pElement->hdr.volume.x[LOW] = xMin;
							pElement->hdr.volume.x[HIGH]= xMax;
							pElement->hdr.volume.y[LOW] = yMin;
							pElement->hdr.volume.y[HIGH]= yMax;
							pElement->hdr.volume.z[LOW] = zMin;
							pElement->hdr.volume.z[HIGH]= zMax;
							
							pElement->body.pvtPoints->push_back(pt[0][i]);
							pElement->body.pvtPoints->push_back(pt[0][i+1]);
							pElement->body.pvtPoints->push_back(pt[1][i+1]);
							pElement->body.pvtPoints->push_back(pt[1][i]);
							pElement->body.pvtPoints->push_back(pt[0][i]);
							
							pRet = ListPush(pRet,pElement);
						}
					}
				}
			}
		}
		else if(SURF_OF_REVOLUTION == pPrimitive->body.pSurface->nType)
		{
			if(PossibleBeDome(pPrimitive))
			{
				pRet = pPrimitive;
				strcpy(pRet->hdr.szDesc,"DOME");
				return pRet;
			}
			else if(PossibleBeSphere(pPrimitive))
			{
				pRet = pPrimitive;
				strcpy(pRet->hdr.szDesc,"SPHERE");
				return pRet;
			}
			int nCount[2]={0,0};

			int nIndex=0;
			for(SQSURFACE::PSURFELM_T ptrElement=pPrimitive->body.pSurface->pElement;ptrElement;ptrElement=ptrElement->next)
			{
				nCount[0] = nCount[1] = 0;
				for(PSQPRIMITIVE ptr=ptrElement->pBoundaryCurve;(nIndex < 2) && ptr && (nCount[nIndex] < 3072);nIndex++)
				{
					for(int i=0;(i < pPrimitive->body.pSurface->nBoundary) && ptr;ptr = ptr->next,i++)
					{
						if(ptr && (ENM_LINE == ptr->hdr.id))
						{
							pt[nIndex][nCount[nIndex]] = ptr->body.line.ptStart;
							(nCount[nIndex])++;
							pt[nIndex][nCount[nIndex]] = ptr->body.line.ptEnd;
							(nCount[nIndex])++;
						}
						else if(ptr && (SQ_POINTS == ptr->hdr.id))
						{
							for(vector<POINT_T>::iterator itr=ptr->body.pvtPoints->begin();itr != ptr->body.pvtPoints->end();++itr)
							{
								pt[nIndex][nCount[nIndex]]=*itr;
								(nCount[nIndex])++;
							}
						}
						else if(ptr && (ENM_ELLIPSE == ptr->hdr.id))
						{
							CEllipse3d ellipse(ptr->body.ellipse);
							ellipse.CreateSegments();
							const int nPoints = ellipse.GetNumOfPoints();
							
							for(int j=0;j < nPoints;++j)
							{
								pt[nIndex][nCount[nIndex]] = CCurve3d::pt(j);
								(nCount[nIndex])++;
							}
						}
						else if(ptr && (ENM_ARC == ptr->hdr.id))
						{
							CArc3d arc(ptr->body.arc);
							arc.CreateSegments();
							int nPoints = arc.GetNumOfPoints();
							
							for(int j=0;j < nPoints;j++)
							{
								pt[nIndex][nCount[nIndex]] = CCurve3d::pt(j);
								(nCount[nIndex])++;
							}
						}
					}
				}
				if((3072 == nCount[0]) || (3072 == nCount[1])) continue;

				for(PSQPRIMITIVE ptr = ptrElement->pRuleCurve;ptr;ptr=ptr->next)
				{
					if((ENM_ELLIPSE == ptr->hdr.id) && ((ptr->body.ellipse.nAxis[0] > 0.) && (ptr->body.ellipse.nAxis[1] > 0.)) && (RULE_CRV_T == ptr->hdr.attr.type))
					{
						for(int i=0;i < nCount[0];i++)
						{
							pt[0][i].x -= ptr->body.ellipse.ptOrigin.x;
							pt[0][i].y -= ptr->body.ellipse.ptOrigin.y;
							pt[0][i].z -= ptr->body.ellipse.ptOrigin.z;
						}
						CCurve3d::Revolve(ptr->body.ellipse.vecNorm,nCount[0],pt[0],ptr->body.ellipse.nSweepAngle,8);
						for(int i=0;i < nCount[0]*9;i++)
						{
							pt[0][i].x = CCurve3d::pt(i).x + ptr->body.ellipse.ptOrigin.x;
							pt[0][i].y = CCurve3d::pt(i).y + ptr->body.ellipse.ptOrigin.y;
							pt[0][i].z = CCurve3d::pt(i).z + ptr->body.ellipse.ptOrigin.z;
						}
						
						PSQPRIMITIVE pFacets=CreateFacets(pt[0],8,nCount[0],pPrimitive->hdr);
						if(pFacets) pRet = ListPush(pRet,pFacets);
						
						break;
					}
					else if((ENM_ARC == ptr->hdr.id) && (RULE_CRV_T == ptr->hdr.attr.type))
					{
						for(int i=0;i < nCount[0];i++)
						{
							pt[0][i].x -= ptr->body.arc.ptOrigin.x;
							pt[0][i].y -= ptr->body.arc.ptOrigin.y;
							pt[0][i].z -= ptr->body.arc.ptOrigin.z;
						}
						CCurve3d::Revolve(ptr->body.arc.vecNorm,nCount[0],pt[0],ptr->body.arc.nSweep,8);
						for(int i=0;i < nCount[0]*9;i++)
						{
							pt[0][i].x = CCurve3d::pt(i).x + ptr->body.arc.ptOrigin.x;
							pt[0][i].y = CCurve3d::pt(i).y + ptr->body.arc.ptOrigin.y;
							pt[0][i].z = CCurve3d::pt(i).z + ptr->body.arc.ptOrigin.z;
						}
						
						PSQPRIMITIVE pFacets=CreateFacets(pt[0],8,nCount[0],pPrimitive->hdr);
						if(pFacets) pRet = ListPush(pRet,pFacets);

						break;
					}
				}

				if(2 == nIndex)
				{
					for(int i=0;i < nCount[1];i++) pt[0][i] = pt[1][i];
					nIndex = 1;
					nCount[0] = nCount[1];
				}
			}
		}
	}

	return pRet;
}

/**	\brief	The COpenSolid::CreateArc function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE COpenSolid::CreateArc(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	PSQPRIMITIVE pRet=NULL;

	if(pPrimitive && (pRet = new_primitive()))
	{
		memcpy(&(pRet->hdr),&(pPrimitive->hdr),sizeof(SQHEADER));
		pRet->hdr.id              = SQ_POINTS;
		pRet->hdr.attr.f	  = 1;
		pRet->hdr.attr.continuous = 1;
		strcpy(pRet->hdr.szDesc,"LOOP");
		if(pRet->body.pvtPoints = new_point_vector())
		{
			CArc3d arc(pPrimitive->body.arc);
			arc.CreateSegments();
			int nPoints=arc.GetNumOfPoints();
			if(pRet->hdr.attr.reversed)
			{
				for(int i=nPoints-1;i >= 0;--i)
				{
					pRet->body.pvtPoints->push_back(CCurve3d::pt(i));
				}
			}
			else
			{
				for(int i=0;i < nPoints;i++)
				{
					pRet->body.pvtPoints->push_back(CCurve3d::pt(i));
				}
			}
		}
		else
		{
			delete_primitive(pRet);
			pRet = NULL;
		}
	}

	return pRet;
}

COpenSolid theOpenSolid;
//--------------------------------------------------------------------

/**	\brief	The SetNumOfCurveSegments function

	\param	nCurveSegments	a parameter of type int

	\return	void	
*/
extern "C" __declspec(dllexport) void SetNumOfCurveSegments(int nCurveSegments)
{
	assert((nCurveSegments > 0) && "nCurveSegments is less than 0");

	if(nCurveSegments)
	{
		theOpenSolid.SetNumOfCurveSegments(nCurveSegments);
	}
}

#include <fstream>
/**	
	@brief	interface function.

	@author	HumKyung.Baek

	@param	pPrimitive	a parameter of type PSQPRIMITIVE

	@return	PSQPRIMITIVE	
*/
extern "C" __declspec(dllexport)
PSQPRIMITIVE ExportOpenSolid(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	PSQPRIMITIVE pRet=NULL,pTail=NULL;

	TCHAR szError[MAX_PATH + 1]={0,};
	if(pPrimitive)
	{
		TCHAR szBuf[32]={0,};

		long nCount = 0L , nIndex = 0L,nConeCount=0L;
		{
			//! calculate primitive count.
			for(PSQPRIMITIVE ptr=pPrimitive;ptr;ptr = ptr->next,++nCount);
		}

		PSQPRIMITIVE ptrNext=NULL;
		for(PSQPRIMITIVE ptr=pPrimitive;ptr;ptr = ptrNext,++nIndex)
		{
			ptrNext  = ptr->next;
			ptr->next= NULL;

			switch(ptr->hdr.id)
			{
				case SQ_SURFACE:
					try
					{
						if(0 == strcmp(ptr->hdr.szDesc,"SOLID"))
						{
							PSQPRIMITIVE pSurf=theOpenSolid.CreateSurface(ptr);
							if(pSurf)
							{
								if(NULL == pRet)
									pRet = pTail = ListPush(pRet,pSurf);
								else	pTail= ListPush(pTail,pSurf);
								pTail = pSurf;
							}
						}
						else if(SWEEP_SURF == ptr->body.pSurface->nType)
						{
							PSQPRIMITIVE pSurf=theOpenSolid.CreateSurface(ptr);
							if(pSurf)
							{
								if(NULL == pRet)
									pRet = pTail = ListPush(pRet,pSurf);
								else	pTail= ListPush(pTail,pSurf);
								pTail= pSurf;
							}
						}
						else if(SURF_OF_REVOLUTION == ptr->body.pSurface->nType)
						{
							if(0 == strcmp(ptr->hdr.szDesc,"ELBOW"))
							{
								if(NULL == pRet)
									pRet = pTail = ListPush(pRet,ptr);
								else	pTail = ListPush(pTail,ptr);
								pTail = ptr;
							}
							else
							{
								PSQPRIMITIVE pSurf=theOpenSolid.CreateSurface(ptr);
								if(pSurf)
								{
									if(NULL == pRet)
										pRet = pTail = ListPush(pRet,pSurf);
									else	pTail = ListPush(pTail,pSurf);
									pTail= pSurf;
								}
							}
						}
					}
					catch(...)
					{
						///ofile << "throw SURFACE exception" << std::endl;
					}
					break;
				//////////////////////////////////////////////////////////////////////////
				case SQ_PLANESURFACE:
				{
					try
					{
						PSQPRIMITIVE pPlaneSurf=NULL;

						pPlaneSurf = theOpenSolid.CreatePlaneSurface(ptr);
						if(pPlaneSurf)
						{
							if(NULL == pRet)
								pRet = pTail = ListPush(pRet,pPlaneSurf);
							else	pTail= ListPush(pTail,pPlaneSurf);
							pTail = pPlaneSurf;
						}
					}
					catch(const exception es)
					{
						::OutputDebugString(es.what());
						///ofile << es.what() << std::endl;
					}
					catch(...)
					{
						///ofile << "throw PLANESURFACE exception" << std::endl;
					}
				}
				break;
				case SQ_CONESURFACE:
				{
					try
					{
						PSQPRIMITIVE pConeSurf=NULL;

						pConeSurf = theOpenSolid.CreateConeSurface(ptr);
						if(pConeSurf)
						{
							if(NULL == pRet)
								pRet = pTail = ListPush(pRet,pConeSurf);
							else	pTail= ListPush(pTail,pConeSurf);
							pTail = pConeSurf;
						}
					}
					catch(const exception es)
					{
						///ofile << es.what() << std::endl;
					}
					catch(...)
					{
						///ofile << "throw CONESURFACE exception" << std::endl;
					}
				}
				break;
				case SQ_TORUSSURFACE:
				{
					try
					{
						PSQPRIMITIVE pTorusSurf=NULL;

						pTorusSurf = theOpenSolid.CreateTorusSurface(ptr);
						if(pTorusSurf)
						{
							if(NULL == pRet)
								pRet = pTail = ListPush(pRet,pTorusSurf);
							else	pTail= ListPush(pTail,pTorusSurf);
							pTail = pTorusSurf;
						}
					}
					catch(...)
					{
						///ofile << "throw TORUSSURFACE exception" << std::endl;
					}
				}
				break;
				case SQ_SPHERESURFACE:
				{
					try
					{
						PSQPRIMITIVE pSphereSurf=NULL;

						pSphereSurf = theOpenSolid.CreateSphereSurface(ptr);
						if(pSphereSurf)
						{
							if(NULL == pRet)
								pRet = pTail = ListPush(pRet,pSphereSurf);
							else	pTail= ListPush(pTail,pSphereSurf);
							pTail = pSphereSurf;
						}
					}
					catch(...)
					{
						///ofile << "throw SPHERESURFACE exception" << std::endl;
					}
				}
				break;
				case ENM_GROUP:
					try
					{
						if((0 == strcmp(ptr->hdr.szDesc,"ELBOW")) || (0 == strcmp(ptr->hdr.szDesc,"TEE")))
						{
							if(NULL == pRet)
								pRet = pTail = ListPush(pRet,ptr);
							else	pTail= ListPush(pTail,ptr);
							pTail = ptr;
						}
						else
						{
							if(ptr->body.pPrimitive)
							{
								PSQPRIMITIVE pGroup=NULL;
								pGroup = ExportOpenSolid(ptr->body.pPrimitive);
								if(pGroup)
								{
									ptr->body.pPrimitive = NULL;
									ptr->body.pPrimitive = ListPush(ptr->body.pPrimitive,pGroup);
									theOpenSolid.ComputeVolume(ptr);
									
									if(NULL == pRet)
										pRet = pTail = ListPush(pRet,ptr);
									else	pTail = ListPush(pTail,ptr);
									pTail = ptr;
								}
							}
						}
					}
					catch(...)
					{
						///ofile << "throw GROUP exception" << std::endl;
					}
				break;
			}
		}
	}

#ifdef _DEBUG
	//CProfileIterator* pItr=CProfileManager::Get_Iterator();
#endif
	return pRet;
}

/**	\brief	The DigitizeEllipse function

	\param	pt[]	a parameter of type POINT_T
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	int	
*/
extern "C" __declspec(dllexport)
int DigitizeEllipse(POINT_T pt[],PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");
	assert((ENM_ELLIPSE == pPrimitive->hdr.id) && "pPrimitive's type is not ELLIPSE");
	int nCount=0;

	if(pPrimitive && (ENM_ELLIPSE == pPrimitive->hdr.id))
	{
		CEllipse3d *pEllipse = new CEllipse3d(pPrimitive->body.ellipse);
		if(pEllipse)
		{
			pEllipse->CreateSegments();
			for(nCount = 0;nCount < pEllipse->GetNumOfPoints();nCount++)
			{
				pt[nCount] = CCurve3d::pt(nCount);
			}
			delete pEllipse;
		}
	}

	return nCount;
}

/**	\brief	The FreeOpenSolidPrimitive function


	\return	void	
*/
extern "C" __declspec(dllexport)
void FreeOpenSolidPrimitive()
{
	theOpenSolid.Pure();
}

void COpenSolid::delete_primitive(PSQPRIMITIVE ptr)
{
	assert(ptr && "ptr is NULL");

	if(NULL != m_pPrimitivePool)
	{
		m_pPrimitivePool->free(ptr);
	}
}

vector<POINT_T>* COpenSolid::new_point_vector()
{
	vector<POINT_T>* pRet=NULL;

	if(NULL == m_pPointVectorPool) m_pPointVectorPool = new boost::object_pool< vector<POINT_T> >;
	if(NULL != m_pPointVectorPool)
	{
		pRet = m_pPointVectorPool->construct();
	}

	return pRet;
}

void COpenSolid::delete_point_vector(vector<POINT_T> *pvt)
{
	if(NULL != m_pPointVectorPool)
	{
		m_pPointVectorPool->destroy(pvt);
	}
}

list<SQVERTEX>* COpenSolid::new_vertex_list()
{
	list<SQVERTEX>* pRet=NULL;

	if(NULL == m_pVertexListPool) m_pVertexListPool = new boost::object_pool< list<SQVERTEX> >;
	if(NULL != m_pVertexListPool)
	{
		pRet = m_pVertexListPool->construct();
	}

	return pRet;
}

void COpenSolid::delete_vertex_list(list<SQVERTEX>* plst)
{
	if(NULL != m_pVertexListPool)
	{
		m_pVertexListPool->destroy(plst);
	}
}
