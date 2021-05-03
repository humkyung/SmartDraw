#include <math.h>
#include "surface3d.h"
#include "face3d.h"
#include "wedge.h"
#include <graphics/circle3d.h>
#include "Cone.h"

/**	\brief
*/
CTinyCone::CTinyCone()
{
	m_sldCone.vecAxis.dx = 0.;
	m_sldCone.vecAxis.dy = 0.;
	m_sldCone.vecAxis.dz = 1.;

	m_sldCone.ptOrigin[0].x = 0.;
	m_sldCone.ptOrigin[0].y = 0.;
	m_sldCone.ptOrigin[0].z = 0.;
	m_sldCone.nRadius[0]    = 1.;

	m_sldCone.ptOrigin[1].x = 0.;
	m_sldCone.ptOrigin[1].y = 0.;
	m_sldCone.ptOrigin[1].z = 10.;
	m_sldCone.nRadius[1]    = 1.;
	
	OnCreate();
}

/**	\brief
*/
CTinyCone::CTinyCone(const CONE_T& cone)
{
	memcpy(&m_sldCone,&cone,sizeof(CONE_T));
	OnCreate();
}

/**	\brief
*/
CTinyCone::~CTinyCone()
{
}

/**	\brief	The CTinyCone::OnCreate function


	\return	void	
*/
void CTinyCone::OnCreate()
{
}

/**	\brief	The CTinyCone::CreateFacets function


	\return	CTinyFacetContainer*	
*/
CTinyFacetContainer* CTinyCone::CreateFacets()
{
	CTinyFacetContainer* pFacetContainer=new CTinyFacetContainer;
	if(NULL == pFacetContainer) return NULL;

	if((0. == m_sldCone.nRadius[0]) && (m_sldCone.nRadius[1] > 0.))
	{
		CIRCLE_T circle={0,};
		circle.ptOrigin = m_sldCone.ptOrigin[1];
		circle.nRadius  = m_sldCone.nRadius[1];
		circle.vecNorm  = m_sldCone.vecAxis;

		CCircle3d circle3d(circle);
		circle3d.CreateSegments();
		int nPoints = circle3d.GetNumOfPoints();
		for(int i=0;i < nPoints - 1;i++)
		{
			CFacet* pFacet=NULL;
			if(pFacet = new CFacet(CCurve3d::pt(i),CCurve3d::pt(i+1),m_sldCone.ptOrigin[0]))
			{
				pFacetContainer->Add(pFacet);
			}
		}
	}
	else if((m_sldCone.nRadius[0] > 0.) && (0. == m_sldCone.nRadius[1]))
	{
		CIRCLE_T circle={0,};
		circle.ptOrigin = m_sldCone.ptOrigin[0];
		circle.nRadius  = m_sldCone.nRadius[0];
		circle.vecNorm  = m_sldCone.vecAxis;

		CCircle3d circle3d(circle);
		circle3d.CreateSegments();
		int nPoints = circle3d.GetNumOfPoints();
		for(int i=0;i < nPoints - 1;i++)
		{
			CFacet* pFacet=NULL;
			if(pFacet = new CFacet(CCurve3d::pt(i),CCurve3d::pt(i+1),m_sldCone.ptOrigin[1]))
			{
				pFacetContainer->Add(pFacet);
			}
		}
	}
	else if((m_sldCone.nRadius[0] > 0) && (m_sldCone.nRadius[1] > 0))
	{
		CIRCLE_T circle[2]={0};
		circle[0].ptOrigin = m_sldCone.ptOrigin[0];
		circle[0].nRadius  = m_sldCone.nRadius[0];
		circle[0].vecNorm  = m_sldCone.vecAxis;
		circle[1].ptOrigin = m_sldCone.ptOrigin[1];
		circle[1].nRadius  = m_sldCone.nRadius[1];
		circle[1].vecNorm  = m_sldCone.vecAxis;

		static POINT_T pt[256]={0,};
		
		CCircle3d circle3d(circle[0]);
		circle3d.CreateSegments();
		int nPoints= circle3d.GetNumOfPoints();
		memcpy(pt,CCurve3d::pt,sizeof(POINT_T)*nPoints);
		circle3d = circle[1];
		circle3d.CreateSegments();
		for(int i=0;i < nPoints - 1;i++)
		{
			CFacet* pFacet=NULL;
			if(pFacet = new CFacet(CCurve3d::pt(i),CCurve3d::pt(i+1),pt[i+1],pt[i]))
			{
				pFacetContainer->Add(pFacet);
			}
		}
	}

	return pFacetContainer;
}

bool CTinyCone::IsKindOf(CONE_T& cone,PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive && ((SQ_CONESURFACE == pPrimitive->hdr.id) && (NULL != pPrimitive->body.pConeSurf)))
	{
		int nLoop=pPrimitive->body.pConeSurf->nLoop;
		if(2 == nLoop)
		{
			int nPoints=0,nArcs=0;

			for(int i=0;(i < nLoop);i++)
			{
				PSQPRIMITIVE pSurf=pPrimitive->body.pConeSurf->pBoundary[i];
				switch(pSurf->hdr.id)
				{
					case ENM_POINT:
						cone.ptOrigin[i] = pSurf->body.point;
						cone.nRadius[i] = 0.;
						nPoints++;
						break;
					case ENM_ARC:
					{
						cone.ptOrigin[i] = pSurf->body.arc.ptOrigin;
						double dx=pSurf->body.arc.ptStart.x - pSurf->body.arc.ptOrigin.x;
						double dy=pSurf->body.arc.ptStart.y - pSurf->body.arc.ptOrigin.y;
						double dz=pSurf->body.arc.ptStart.z - pSurf->body.arc.ptOrigin.z;
						cone.nRadius[i] = sqrt(dx*dx + dy*dy + dz*dz);
						cone.vecAxis = pSurf->body.arc.vecNorm;
						nArcs++;
					}
						break;
				}
			}
			if((1 == nPoints) && (1 == nArcs)) return true;
		}
	}

	return false;
}
