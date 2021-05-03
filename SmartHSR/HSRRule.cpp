// CHSRRule.cpp: implementation of the CHSRRule class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include "SmartHSR.h"
#include "Geo.h"

#include "HSRRule.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHSRRule::CHSRRule()
{
}

CHSRRule::~CHSRRule()
{
}

/**	\brief	The CHSRRule::Rule function\n
	take rule for face.

	\param	ret	a parameter of type PSQPRIMITIVE
	\param	pFace	a parameter of type PFACE

	\remarks\n
	;2002.05.24 - add routine for extended tee symbol.

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRRule::Rule(PSQPRIMITIVE ret,CHSRFace* pFace)
{
	assert(pFace && "face is invalid");

	if(pFace)
	{
		switch(pFace->type)
		{
			case ELBOW:
				ret = RunForElbow(ret,pFace);
				break;
			case SW_ELBOW:
				ret = RunForSWElbow(ret,pFace);
				break;
			case TEE:
				ret = RunForTee(ret,pFace);
				break;
			case TEEX:
				ret = RunForTeeX(ret,pFace);
				break;
		}
	}

	return ret;
}

/**	
	@brief	The CHSRRule::AppendWeldElbowMark function
	@author	humkyung
	@param	ret	a parameter of type PSQPRIMITIVE
	@param	pFace	a parameter of type PFACE
	@remarks\n
	;2002.03.15 - multiplied dot size divided by 2, fix it
	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRRule::AppendWeldElbowMark(PSQPRIMITIVE ret, CHSRFace* pFace)
{
	assert(pFace && "pFace is NULL");

	if(pFace && pFace->pCenterline)
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		CHSROption* pEnv = pHSRApp->GetEnv();
		if(pEnv->m_oCompProp[LAYER_ELBOW_WELD_MARK_CIRCLE].IsOn())
		{
			const double nWeldMinDotDist = pHSRApp->GetEnv()->WeldMinDotDist();

			PSQPRIMITIVE pPrimitive = NULL;
			if(pFace->mark[0])
			{
				PHSR_VERTEX ptr = pFace->pCenterline;

				VECTOR_T vecDir={0,};
				vecDir.dx=ptr->pt->x - pFace->ptCenter.x;
				vecDir.dy=ptr->pt->y - pFace->ptCenter.y;
				vecDir.dz=ptr->pt->z - pFace->ptCenter.z;
				double nDist=sqrt(vecDir.dx*vecDir.dx + vecDir.dy*vecDir.dy + vecDir.dz*vecDir.dz);
				POINT_T ptMark={0,};
#ifdef VER_03
				if(nDist < nWeldMinDotDist)
				{
					if(!CMath::NormalizeVector(vecDir)) ret;
					ptMark.x = ptr->pt->x + vecDir.dx*(nWeldMinDotDist - nDist);
					ptMark.y = ptr->pt->y + vecDir.dy*(nWeldMinDotDist - nDist);
					ptMark.z = ptr->pt->z + vecDir.dz*(nWeldMinDotDist - nDist);
				}
				else
				{
					ptMark.x = ptr->pt->x;
					ptMark.y = ptr->pt->y;
					ptMark.z = ptr->pt->z;
				}
#else
				ptMark.x = ptr->pt->x;
				ptMark.y = ptr->pt->y;
				ptMark.z = ptr->pt->z;
#endif
				if(NULL != (pPrimitive = pHSRApp->alloc_primitive()))
				{
					pPrimitive->hdr.id = SQ_CIRCLE;
					pPrimitive->body.circle.ptOrigin.x = ptMark.x;
					pPrimitive->body.circle.ptOrigin.y = ptMark.y;
					pPrimitive->body.circle.ptOrigin.z = 0.;
					pPrimitive->body.circle.nRadius    = pHSRApp->GetEnv()->GetWeldDotSize();

					ret = ListPushFront(ret,pPrimitive);
				}
			}

			if(pFace->mark[1])
			{
				PHSR_VERTEX ptr = NULL;
				for(ptr = pFace->pCenterline;(ptr && ptr->next && ptr->next->next);ptr = ptr->next);

				if(ptr && ptr->next)
				{
					VECTOR_T vecDir={0,};
					vecDir.dx=ptr->next->pt->x - pFace->ptCenter.x;
					vecDir.dy=ptr->next->pt->y - pFace->ptCenter.y;
					vecDir.dz=ptr->next->pt->z - pFace->ptCenter.z;
					double nDist=sqrt(vecDir.dx*vecDir.dx + vecDir.dy*vecDir.dy + vecDir.dz*vecDir.dz);
					POINT_T ptMark={0,};
#ifdef VER_03
					if(nDist < nWeldMinDotDist)
					{
						if(!CMath::NormalizeVector(vecDir)) ret;
						ptMark.x = ptr->next->pt->x + vecDir.dx*(nWeldMinDotDist - nDist);
						ptMark.y = ptr->next->pt->y + vecDir.dy*(nWeldMinDotDist - nDist);
						ptMark.z = ptr->next->pt->z + vecDir.dz*(nWeldMinDotDist - nDist);
					}
					else
					{
						ptMark.x = ptr->next->pt->x;
						ptMark.y = ptr->next->pt->y;
						ptMark.z = ptr->next->pt->z;
					}
#else
					ptMark.x = ptr->next->pt->x;
					ptMark.y = ptr->next->pt->y;
					ptMark.z = ptr->next->pt->z;
#endif

					pPrimitive = NULL;
					if(NULL != (pPrimitive = pHSRApp->alloc_primitive()))
					{
						pPrimitive->hdr.id = SQ_CIRCLE;
						pPrimitive->body.circle.ptOrigin.x = ptMark.x;
						pPrimitive->body.circle.ptOrigin.y = ptMark.y;
						pPrimitive->body.circle.ptOrigin.z = 0.;
						pPrimitive->body.circle.nRadius    = pHSRApp->GetEnv()->GetWeldDotSize();

						ret = ListPushFront(ret,pPrimitive);
					}
				}
			}
		}
	}

	return ret;
}

/**	\brief	The CHSRRule::AppendSWElbowMark function\n
	Face type is SIDEFACE and must exists CETERLINE

	\param	ret	a parameter of type PSQPRIMITIVE
	\param	pFace	a parameter of type PFACE

	\remarks\n
	;2002.04.2 - append min size check routine.\n
	;2002.05.22- extrusion length is pFace->radius if if linemode is SINGLE_LINE

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRRule::AppendSWElbowMark(PSQPRIMITIVE ret,CHSRFace* pFace)
{
	assert(pFace && "pFace is NULL");
	
	CHSRApp* pHiddenApp = CHSRApp::GetInstance();
	PIPELINEMODE linemode=pHiddenApp->GetEnv()->GetPipeLineMode();

	if(pFace && (SINGLE_LINE == linemode) || (BOTH == linemode) && (pFace->radius <= pHiddenApp->GetEnv()->GetCriticalRadius()))
	{
		POINT_T  mark={0,},pt[2]={0,};
		VECTOR_T vt[3]={0,};

		double extrusion=0.;
		
		// check min size check.
		if((pFace->radius*2) < pHiddenApp->GetEnv()->GetSWElbowMinSize())
			extrusion = pHiddenApp->GetEnv()->GetSWElbowMinSize()*0.5;
		else	extrusion = pFace->radius;
		
		if(pFace->mark[0])
		{
			mark.x = pFace->ptEdge[1].x;
			mark.y = pFace->ptEdge[1].y;
			mark.z = pFace->ptEdge[1].z;
		}
		else if(pFace->mark[1])
		{
			mark.x = pFace->ptEdge[1].x;
			mark.y = pFace->ptEdge[1].y;
			mark.z = pFace->ptEdge[1].z;
		}else	return ret;
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		
		vt[0].dx= mark.x - pFace->ptEdge[0].x;
		vt[0].dy= mark.y - pFace->ptEdge[0].y;
		vt[0].dz= mark.z - pFace->ptEdge[0].z;
		if((0. == vt[0].dx) && (0. == vt[0].dy) && (0. == vt[0].dz)) return ret;
		if(!CMath::NormalizeVector(vt[0].dx,vt[0].dy,vt[0].dz,&(vt[1].dx),&(vt[1].dy),&(vt[1].dz)))
			return ret;
		
		assert((pFace->radius > 0.) && "pFace->radius is less equal than 0");
		if(pFace->radius > 0.)
		{
			pt[0].x = -vt[1].dy;
			pt[0].y = vt[1].dx;
			pt[0].z = 0;
			pt[0].x*= extrusion;
			pt[0].y*= extrusion;
			pt[0].z*= extrusion;
			pt[0].x+= mark.x;
			pt[0].y+= mark.y;
			pt[0].z+= mark.z;
			
			pt[1].x = vt[1].dy;
			pt[1].y = -vt[1].dx;
			pt[1].z = 0.;
			pt[1].x*= extrusion;
			pt[1].y*= extrusion;
			pt[1].z*= extrusion;
			pt[1].x+= mark.x;
			pt[1].y+= mark.y;
			pt[1].z+= mark.z;
			
			PSQPRIMITIVE pPrimitive=NULL;
			if(pPrimitive = pHiddenApp->alloc_primitive())
			{
				//InitPrimitive(pPrimitive);
				pPrimitive->hdr.id              = ENM_LINE;
				pPrimitive->body.line.ptStart.x = pt[0].x;
				pPrimitive->body.line.ptStart.y = pt[0].y;
				pPrimitive->body.line.ptStart.z = pt[0].z;
				pPrimitive->body.line.ptEnd.x = pt[1].x;
				pPrimitive->body.line.ptEnd.y = pt[1].y;
				pPrimitive->body.line.ptEnd.z = pt[1].z;
				
				ret = ListPushFront(ret,pPrimitive);
			}
		}
	}
	
	return ret;
}

/**	
	@brief	append tee mark
	@author	humkyung
	@param	pPrimitiveList	a parameter of type PSQPRIMITIVE
	@param	pFace	a parameter of type PFACE
	@remarks\n
	;2002.03.15 - multiplied dot size divided by 2, fix it\n
	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRRule::RunForTee(PSQPRIMITIVE pPrimitiveList, CHSRFace* pFace)
{
	assert(pFace && "CHSRRule::RunForTee : pFace is NULL");
	assert(pFace->pCenterline && "CHSRRule::RunForTee : pFace->pCenterline is NULL");

	if(pFace && pFace->pCenterline)
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		double nWeldMinDotDist = pHSRApp->GetEnv()->WeldMinDotDist();
		PSQPRIMITIVE pPrimitive = NULL;
		
		/// get edge points
		POINT_T ptMark[2]={0,};
		PHSR_VERTEX ptr = pFace->pCenterline;
		ptMark[0].x = ptr->pt->x;ptMark[0].y = ptr->pt->y;ptMark[0].z = ptr->pt->z;
		int nShowVertices=0;
		for(ptr = pFace->pCenterline;ptr && ptr->next;ptr = ptr->next)
		{
			if((SHOW == ptr->render) || (INTERSECTION == ptr->render)) nShowVertices++;
		}
		ptMark[1].x = ptr->pt->x;ptMark[1].y = ptr->pt->y;ptMark[1].z = ptr->pt->z;
		///

		if(pFace->mark[0])
		{
			ptr = pFace->pCenterline;
			if(ptr && ((SHOW == ptr->render) || (INTERSECTION == ptr->render)))
			{
				VECTOR_T vecDir={0,};
				vecDir.dx=ptMark[0].x - ptMark[1].x;
				vecDir.dy=ptMark[0].y - ptMark[1].y;
				vecDir.dz=ptMark[0].z - ptMark[1].z;
				double nDist=sqrt(vecDir.dx*vecDir.dx + vecDir.dy*vecDir.dy + vecDir.dz*vecDir.dz);
#ifdef VER_03
				if(nDist < nWeldMinDotDist)
				{
					if(!CMath::NormalizeVector(vecDir)) pPrimitiveList;
					ptMark[0].x += vecDir.dx*(nWeldMinDotDist - nDist);
					ptMark[0].y += vecDir.dy*(nWeldMinDotDist - nDist);
					ptMark[0].z += vecDir.dz*(nWeldMinDotDist - nDist);
				}
#else
#endif
				if(NULL != (pPrimitive = pHSRApp->alloc_primitive()))
				{
					pPrimitive->hdr.id                 = SQ_CIRCLE;
					pPrimitive->body.circle.ptOrigin.x = ptMark[0].x;
					pPrimitive->body.circle.ptOrigin.y = ptMark[0].y;
					pPrimitive->body.circle.ptOrigin.z = ptMark[0].z;
					pPrimitive->body.circle.nRadius    = pHSRApp->GetEnv()->GetWeldDotSize();
					
					pPrimitiveList = ListPushFront(pPrimitiveList,pPrimitive);
				}
			}
		}
		
		if(pFace->mark[1])
		{
			PHSR_VERTEX ptrPrev=NULL;
			for(ptr = pFace->pCenterline;ptr && ptr->next;ptrPrev=ptr,ptr = ptr->next);
			if((ptrPrev && ptr) && ((SHOW == ptrPrev->render) || (INTERSECTION == ptrPrev->render)))
			{
				VECTOR_T vecDir={0,};
				vecDir.dx=ptMark[1].x - ptMark[0].x;
				vecDir.dy=ptMark[1].y - ptMark[0].y;
				vecDir.dz=ptMark[1].z - ptMark[0].z;
				double nDist=sqrt(vecDir.dx*vecDir.dx + vecDir.dy*vecDir.dy + vecDir.dz*vecDir.dz);
#ifdef VER_03
				if(nDist < nWeldMinDotDist)
				{
					if(!CMath::NormalizeVector(vecDir)) pPrimitiveList;
					ptMark[1].x += vecDir.dx*(nWeldMinDotDist - nDist);
					ptMark[1].y += vecDir.dy*(nWeldMinDotDist - nDist);
					ptMark[1].z += vecDir.dz*(nWeldMinDotDist - nDist);
				}
#else
#endif
				pPrimitive= NULL;
				//ptr = ptr->next;
				if(NULL != (pPrimitive = pHSRApp->alloc_primitive()))
				{
					pPrimitive->hdr.id                 = SQ_CIRCLE;
					pPrimitive->body.circle.ptOrigin.x = ptMark[1].x;
					pPrimitive->body.circle.ptOrigin.y = ptMark[1].y;
					pPrimitive->body.circle.ptOrigin.z = ptMark[1].z;
					pPrimitive->body.circle.nRadius    = pHSRApp->GetEnv()->GetWeldDotSize();
					
					pPrimitiveList = ListPushFront(pPrimitiveList,pPrimitive);
				}
			}
		}
	}

	return pPrimitiveList;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
PSQPRIMITIVE CHSRRule::RunForTeeX(PSQPRIMITIVE pPrimitiveList, CHSRFace* pFace)
{
	assert(pFace && "pFace is NULL");
	
	if(pFace && (HSR_RECTANGLE_SHAPE == pFace->ele) && (pFace->mark[0] || pFace->mark[1]))
	{
		CHSRApp* pHiddenApp = CHSRApp::GetInstance();
		PIPELINEMODE linemode=pHiddenApp->GetEnv()->GetPipeLineMode();
		if((SINGLE_LINE == linemode) || 
		   ((BOTH == linemode) && (pFace->radius <= pHiddenApp->GetEnv()->GetCriticalRadius())))
		{
			double extrusion=0.;
			
			// check min size check.
			if(pFace->radius < pHiddenApp->GetEnv()->GetSWElbowMinSize())
				extrusion = pHiddenApp->GetEnv()->GetSWElbowMinSize()*0.5;
			else	extrusion = pFace->radius;

			POINT_T  pt[2]={0};
			if(pFace->pCenterline && pFace->pCenterline->next)
			{
				if(pFace->mark[1])
				{
					pt[0].x = pFace->pCenterline->pt->x;
					pt[0].y = pFace->pCenterline->pt->y;
					pt[0].z = pFace->pCenterline->pt->z;

					pt[1].x = pFace->pCenterline->next->pt->x;
					pt[1].y = pFace->pCenterline->next->pt->y;
					pt[1].z = pFace->pCenterline->next->pt->z;
				}
				else if(pFace->mark[0])
				{
					pt[0].x = pFace->pCenterline->next->pt->x;
					pt[0].y = pFace->pCenterline->next->pt->y;
					pt[0].z = pFace->pCenterline->next->pt->z;

					pt[1].x = pFace->pCenterline->pt->x;
					pt[1].y = pFace->pCenterline->pt->y;
					pt[1].z = pFace->pCenterline->pt->z;
				}

				VECTOR_T vt[3]={0};
				vt[0].dx= pt[1].x - pt[0].x;
				vt[0].dy= pt[1].y - pt[0].y;
				vt[0].dz= pt[1].z - pt[0].z;
				if((0. == vt[0].dx) && (0. == vt[0].dy) && (0. == vt[0].dz)) return pPrimitiveList;
				if(!CMath::NormalizeVector(vt[0].dx,vt[0].dy,vt[0].dz,&(vt[1].dx),&(vt[1].dy),&(vt[1].dz)))
					return pPrimitiveList;
			
				assert((pFace->radius > 0.) && "pFace->radius is less equal than 0");
				if(pFace->radius > 0.)
				{
					POINT_T mark={pt[1].x,pt[1].y,pt[1].z};

					pt[0].x = -vt[1].dy;
					pt[0].y = vt[1].dx;
					pt[0].z = 0;
					pt[0].x*= extrusion;
					pt[0].y*= extrusion;
					pt[0].z*= extrusion;
					pt[0].x+= mark.x;
					pt[0].y+= mark.y;
					pt[0].z+= mark.z;
					
					pt[1].x = vt[1].dy;
					pt[1].y = -vt[1].dx;
					pt[1].z = 0.;
					pt[1].x*= extrusion;
					pt[1].y*= extrusion;
					pt[1].z*= extrusion;
					pt[1].x+= mark.x;
					pt[1].y+= mark.y;
					pt[1].z+= mark.z;
					
					PSQPRIMITIVE pPrimitive=NULL;
					if(pPrimitive = pHiddenApp->alloc_primitive())
					{
						//InitPrimitive(pPrimitive);
						pPrimitive->hdr.id              = ENM_LINE;
						pPrimitive->hdr.display.lcolor  = (BYTE)ATOI_T(pFace->color().c_str());
						pPrimitive->body.line.ptStart.x = pt[0].x;
						pPrimitive->body.line.ptStart.y = pt[0].y;
						pPrimitive->body.line.ptStart.z = pt[0].z;
						pPrimitive->body.line.ptEnd.x = pt[1].x;
						pPrimitive->body.line.ptEnd.y = pt[1].y;
						pPrimitive->body.line.ptEnd.z = pt[1].z;
						
						pPrimitiveList = ListPushFront(pPrimitiveList,pPrimitive);
					}
				}
			}
		}
	}
	
	return pPrimitiveList;
}

/**	
	@brief	append weld-elbow mark
	@author	humkyung

	@param	pPrimitiveList	a parameter of type PSQPRIMITIVE
	@param	pFace	a parameter of type PFACE

	@remarks
	center line이 반 이상 보일때 WELD MARK를 출력한다.

	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRRule::RunForElbow(PSQPRIMITIVE pPrimitiveList , CHSRFace* pFace)
{
	assert(pFace && "pFace is NULL");
	assert(pFace->pCenterline && "pFace's CenterLine not exists");

	if(pFace && pFace->pCenterline)
	{
		CHSRApp* pHSRApp  = CHSRApp::GetInstance();

		/// layer elbow weld mark line이 켜져 있을때는 무조건 WELD MARK를 출력한다.
		if(pHSRApp->GetEnv()->IsHiddenlineOn(LAYER_ELBOW_WELD_MARK_LINE))
		{
			pPrimitiveList = AppendWeldElbowMark(pPrimitiveList,pFace);
		}
		else
		{
			size_t vertices = 0 , visible_vertices = 0;
			for(PHSR_VERTEX ptr = pFace->pCenterline ; ptr ; ptr = ptr->next , vertices++)
			{
				if((INTERSECTION == ptr->render) || (SHOW == ptr->render)) visible_vertices++;
			}
			if((double)visible_vertices > (double)(((double)vertices) * 0.5))
			{
				if(pFace->mark[0])
				{
					PHSR_VERTEX ptr = pFace->pCenterline;
					if(ptr)// && ((SHOW == ptr->render) || (INTERSECTION == ptr->render)))
					{
						pPrimitiveList = AppendWeldElbowMark(pPrimitiveList,pFace);
					}
				}

				if(pFace->mark[1])
				{
					PHSR_VERTEX ptr = NULL;
					for(ptr = pFace->pCenterline;(ptr && ptr->next && ptr->next->next);ptr = ptr->next);
					if((ptr && ptr->next))// && ((SHOW == ptr->render) || (ptr->render == INTERSECTION)))
					{
						pPrimitiveList = AppendWeldElbowMark(pPrimitiveList,pFace);
					}
				}
			}
		}
	}

	return pPrimitiveList;
}

/**	
	@brief	append weld-elbow mark
	@author	humkyung
	
	@param	pPrimitiveList	a parameter of type PSQPRIMITIVE
	@param	pFace	a parameter of type PFACE

	@return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CHSRRule::RunForSWElbow(PSQPRIMITIVE pPrimitiveList, CHSRFace* pFace)
{
	assert(pFace && "pFace is NULL");

	if(pFace)
	{
		CHSRApp* pHiddenApp = CHSRApp::GetInstance();

		if(((SIDEFACEX == HSR_ELEMENT_MARK(pFace->ele)) && pFace->pCenterline) && (pFace->mark[0] || pFace->mark[1]))
		{
			if(pHiddenApp->GetEnv()->IsHiddenlineOn(LAYER_ELBOW_WELD_MARK_LINE))
			{
				pPrimitiveList = AppendSWElbowMark(pPrimitiveList,pFace);
			}
			else
			{
				PHSR_VERTEX ptr = pFace->pCenterline;
				for(;ptr && ptr->next && ptr->next->next;ptr = ptr->next);
				if(ptr && ((INTERSECTION == ptr->render) || (SHOW == ptr->render)))
				{
					pPrimitiveList = AppendSWElbowMark(pPrimitiveList,pFace);
				}
			}
		}
	}

	return pPrimitiveList;
}






















