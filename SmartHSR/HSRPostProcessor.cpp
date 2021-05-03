// CHSRPostProcessor.cpp: implementation of the CHSRPostProcessor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include "HSRPostProcessor.h"

#include "SmartHSR.h"
#include "Geo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHSRPostProcessor::CHSRPostProcessor()
{

}

CHSRPostProcessor::~CHSRPostProcessor()
{

}

/**	\brief	The CHSRPostProcessor::Run function\n
	running postprocessor.

	\param	pFace	pointer to face list.

	\return	void	
*/
void CHSRPostProcessor::Run(CHSRFace* pFace)
{
	assert(pFace && "pFace is invalid");

	if(pFace)
	{
		for(CHSRFace* ptr = pFace;ptr;ptr = ptr->next)
		{
			if(ptr->pSibling)
			{
				ptr->UnionFaces();
				//assert((NULL == ptr->pSibling) && "fault in UnionFaces()");
				if(NULL != ptr->pSibling)
				{
					// not completely union faces. so need under routin...
					ptr->pSibling->next = ptr->next;
					ptr->next = ptr->pSibling;
				}
			}

			switch(ptr->type)
			{
				case PIPE:
				break;
				case CYLINDER:
				break;
				case ELBOW:
					RunForElbow(ptr);
				break;
				case SW_ELBOW:
					RunForSWElbow(ptr);
				break;
				case TEE:case TEEX:
				break;
				default:
				break;
			}
		}
	}
}

/**	\brief	The CHSRPostProcessor::RunForElbow function\n
	add elbow cross line for symbol and body.

	\param	pFace	a parameter of type PFACE

	\return	void	
*/
void CHSRPostProcessor::RunForElbow(CHSRFace* pFace)
{
	assert(pFace && "pFace is NULL");
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	if(pFace && pHSRApp->GetEnv()->IsWeldElbowCrosslineOn())
	{
		if((SIDEFACE == HSR_ELEMENT_MARK(pFace->ele)) && (pFace->pCenterline))
		{
			if(pFace->mark[0] && pFace->mark[1]) AddCrossline(pFace);
		}
		else if((SYMBOL == HSR_ELEMENT_MARK(pFace->ele)) && pFace->IsPerpendicularToViewPoint())
		{
			AddCrossline(pFace);
		}
	}
}

/**	\brief	The CHSRPostProcessor::AddCrossline function

	\param	pFace	a parameter of type PFACE

	\return	void	
*/
void CHSRPostProcessor::AddCrossline(CHSRFace* pFace)
{
	assert(pFace && "pFace is NULL");
	bool ret=false;
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	if(pFace)
	{
		POINT_T pt[4]={0};

		if((HSR_POLYGON_SHAPE == HSR_SHAPE_MARK(pFace->ele)) || (HSR_ARC_SHAPE == HSR_SHAPE_MARK(pFace->ele)))
		{
			if(pFace->pCenterline && (pFace->mark[0] && pFace->mark[1]))
			{
				ret = GetCrosslinePoints(pt,pFace);
			}else	return;
		}
		else if(((HSR_CIRCLE_SHAPE == HSR_SHAPE_MARK(pFace->ele)) || (SYMBOL == HSR_ELEMENT_MARK(pFace->ele))) && pFace->IsPerpendicularToViewPoint())
		{
			ret = GetCrosslinePoints(pt,pFace);
		}else	return;
		
		if(true == ret)
		{
			PHSR_VERTEX  pvt[4]={0};
			if(NULL != (pvt[0] = pHSRApp->alloc_vertex()))
			{
				pvt[0]->render= INTERSECTION;
				if(NULL != (pvt[0]->pt = pHSRApp->alloc_point()))
				{
					pvt[0]->pt->x = pt[0].x;
					pvt[0]->pt->y = pt[0].y;
					pvt[0]->pt->z = pt[0].z;
					
					pFace->pCrossline = ListPush(pFace->pCrossline,pvt[0]);
				}
				else
				{
					pHSRApp->free_vertex(pvt[0]);
					return;
				}
			}else	return;
			
			if(NULL != (pvt[1] = pHSRApp->alloc_vertex()))
			{
				pvt[1]->render= INVALID;
				if(NULL != (pvt[1]->pt = pHSRApp->alloc_point()))
				{
					pvt[1]->pt->x = pt[1].x;
					pvt[1]->pt->y = pt[1].y;
					pvt[1]->pt->z = pt[1].z;
					
					pFace->pCrossline = ListPush(pFace->pCrossline,pvt[1]);
				}
				else
				{
					pHSRApp->free_vertex(pvt[1]);
				}
			}
			else
			{
				pHSRApp->free_vertex(pvt[0]);
			}
			
			if(NULL != (pvt[2] = pHSRApp->alloc_vertex()))
			{
				pvt[2]->render= INTERSECTION;
				if(NULL != (pvt[2]->pt = pHSRApp->alloc_point()))
				{
					pvt[2]->pt->x  = pt[2].x;
					pvt[2]->pt->y  = pt[2].y;
					pvt[2]->pt->z  = pt[2].z;
					
					pFace->pCrossline = ListPush(pFace->pCrossline,pvt[2]);;
				}else	pHSRApp->free_vertex(pvt[2]);
			}
			else
			{
				pHSRApp->free_vertex(pvt[0]);
				pHSRApp->free_vertex(pvt[1]);
			}
			
			if(NULL != (pvt[3] = pHSRApp->alloc_vertex()))
			{
				pvt[3]->render= INTERSECTION;
				if(NULL != (pvt[3]->pt = pHSRApp->alloc_point()))
				{
					pvt[3]->pt->x = pt[3].x;
					pvt[3]->pt->y = pt[3].y;
					pvt[3]->pt->z = pt[3].z;
					
					pFace->pCrossline = ListPush(pFace->pCrossline,pvt[3]);
				}else	pHSRApp->free_vertex(pvt[3]);
			}
			else
			{
				pHSRApp->free_vertex(pvt[0]);
				pHSRApp->free_vertex(pvt[1]);
				pHSRApp->free_vertex(pvt[2]);
			}
		}
	}
}

/**	\brief	The CHSRPostProcessor::GetCrosslinePoints function\n
	get cross line points from edge points and center point.

	\param	pt	a parameter of type PPOINT_T
	\param	pFace	a parameter of type PFACE

	\remarks\n
	;2002.03.15 - when calculate extrusion, not considered SINGLE_LINE mode.\n
	so fix to consider SINGLE_LINE mode.\n
	;2002.04.26 - when face's element type is section or cirlce,\n
	we calculate vectors ptCenter to two ptEdges.\n
	when calculate vector, dz must be zero.

	\return	bool	
*/
bool CHSRPostProcessor::GetCrosslinePoints(PPOINT_T pt, CHSRFace* pFace)
{
    	assert(pt && "pt is NULL");
    	assert(pFace && "pFace is NULL");
	double extrusion,dx[3]={0},dy[3]={0},dz[3]={0};
	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	PIPELINEMODE LineMode  = pHSRApp->GetEnv()->GetPipeLineMode();

	if(pt && pFace)
	{
		if(HSR_ARC_SHAPE == HSR_SHAPE_MARK(pFace->ele))
		{
			if((SINGLE_LINE == LineMode) || ((BOTH == LineMode) && (pFace->radius <= pHSRApp->GetEnv()->GetCriticalRadius())))
			{
				extrusion = pHSRApp->GetEnv()->GetWeldElbowDistA();
			}
			else
			{
				extrusion = pHSRApp->GetEnv()->GetWeldElbowDistB();
				extrusion *= (2*pFace->radius);
			}

			if(!IsPointsOnLine(&(pFace->ptEdge[0]),&(pFace->ptCenter),&(pFace->ptEdge[1])))
			{
				pt[0].x = pFace->ptEdge[0].x;
				pt[0].y = pFace->ptEdge[0].y;
				pt[0].z = pFace->ptEdge[0].z;
				pt[2].x = pFace->ptEdge[1].x;
				pt[2].y = pFace->ptEdge[1].y;
				pt[2].z = pFace->ptEdge[1].z;
				
				dx[0] = pFace->ptEdge[0].x - pFace->ptCross.x;
				dy[0] = pFace->ptEdge[0].y - pFace->ptCross.y;
				dz[0] = 0;//pFace->ptEdge[0].z - pFace->ptCenter.z;
				if(!(ISEQUAL(dx[0],0.0) && ISEQUAL(dy[0],0.0) && ISEQUAL(dz[0],0.0)))
				{
					CMath::NormalizeVector(dx[0],dy[0],dz[0],&dx[1],&dy[1],&dz[1]);
					
					pt[1].x = -dx[1];
					pt[1].y = -dy[1];
					pt[1].z = 0.;
					pt[1].x*= extrusion;
					pt[1].y*= extrusion;
					pt[1].z*= extrusion;
					pt[1].x+= pFace->ptCross.x;
					pt[1].y+= pFace->ptCross.y;
					pt[1].z+= pFace->ptCross.z;
				}else	return false;
				
				dx[0] = pFace->ptEdge[1].x - pFace->ptCross.x;
				dy[0] = pFace->ptEdge[1].y - pFace->ptCross.y;
				dz[0] = 0;//pFace->ptEdge[1].z - pFace->ptCenter.z;
				if(!(ISEQUAL(dx[0],0.0) && ISEQUAL(dy[0],0.0) && ISEQUAL(dz[0],0.0)))
				{
					CMath::NormalizeVector(dx[0],dy[0],dz[0],&dx[1],&dy[1],&dz[1]);

					pt[3].x = -dx[1];
					pt[3].y = -dy[1];
					pt[3].z = 0;
					pt[3].x*= extrusion;
					pt[3].y*= extrusion;
					pt[3].z*= extrusion;
					pt[3].x+= pFace->ptCross.x;
					pt[3].y+= pFace->ptCross.y;
					pt[3].z+= pFace->ptCross.z; 
				}else	return false;
			}
		}
		else if((HSR_CIRCLE_SHAPE == HSR_SHAPE_MARK(pFace->ele)) || (SYMBOL == HSR_ELEMENT_MARK(pFace->ele)))
		{
			assert((pFace->radius > 0.) && "pFace's radius is less than 0.");
			
			if(pFace->radius > 0.)
			{
				if((SINGLE_LINE == LineMode) || ((BOTH == LineMode) && pFace->radius <= pHSRApp->GetEnv()->GetCriticalRadius()))
					extrusion = pHSRApp->GetEnv()->GetWeldElbowDistA();
				else	extrusion = pHSRApp->GetEnv()->GetWeldElbowDistB();
				dx[0]= pFace->ptEdge[0].x - pFace->ptCenter.x;
				dy[0]= pFace->ptEdge[0].y - pFace->ptCenter.y;
				dz[0]= 0;
				if(!(ISEQUAL(dx[0],0.0) && ISEQUAL(dy[0],0.0) && ISEQUAL(dz[0],0.0)))
				{
					CMath::NormalizeVector(dx[0],dy[0],dz[0],&dx[1],&dy[1],&dz[1]);

					pt[0].x = dx[1];
					pt[0].y = dy[1];
					pt[0].z = 0.;
					pt[0].x*= (pFace->radius + extrusion);
					pt[0].y*= (pFace->radius + extrusion);
					pt[0].z*= (pFace->radius + extrusion);
					pt[0].x+= pFace->ptCenter.x;
					pt[0].y+= pFace->ptCenter.y;
					pt[0].z+= pFace->ptCenter.z;
					
					pt[1].x = -dx[1];
					pt[1].y = -dy[1];
					pt[1].z = 0.;
					pt[1].x*= (pFace->radius + extrusion);
					pt[1].y*= (pFace->radius + extrusion);
					pt[1].z*= (pFace->radius + extrusion);
					pt[1].x+= pFace->ptCenter.x;
					pt[1].y+= pFace->ptCenter.y;
					pt[1].z+= pFace->ptCenter.z;
					
					pt[2].x = -dy[1];
					pt[2].y = dx[1];
					pt[2].z = 0.;
					pt[2].x*= (pFace->radius + extrusion);
					pt[2].y*= (pFace->radius + extrusion);
					pt[2].z*= (pFace->radius + extrusion);
					pt[2].x+= pFace->ptCenter.x;
					pt[2].y+= pFace->ptCenter.y;
					pt[2].z+= pFace->ptCenter.z;
					
					pt[3].x = dy[1];
					pt[3].y = -dx[1];
					pt[3].z = 0.;
					pt[3].x*= (pFace->radius + extrusion);
					pt[3].y*= (pFace->radius + extrusion);
					pt[3].z*= (pFace->radius + extrusion);
					pt[3].x+= pFace->ptCenter.x;
					pt[3].y+= pFace->ptCenter.y;
					pt[3].z+= pFace->ptCenter.z;
				}else	return false;
			}
		}
	}

	return true;
}

/**	\brief	The CHSRPostProcessor::RunForSWElbow function\n
	call AddCrossline function.

	\param	pFace	a parameter of type PFACE

	\return	void	
*/
void CHSRPostProcessor::RunForSWElbow(CHSRFace* pFace)
{
	assert(pFace && "pFace is invalid");
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	// add cross line
	if(pFace && pHSRApp->GetEnv()->IsSWElbowCrosslineOn())
	{
		if((SIDEFACE == HSR_ELEMENT_MARK(pFace->ele)) && pFace->pCenterline)
		{
			if(pFace->mark[0] && pFace->mark[1]) AddCrossline(pFace);
		}
		else if((SYMBOL == HSR_ELEMENT_MARK(pFace->ele)) && pFace->IsPerpendicularToViewPoint())
		{
			AddCrossline(pFace);
		}
	}
}

static double is_round(double a , int b)
{
	double ret = a;

	ret*= pow(10.0 , b-1);
	ret = (ret > 0) ?  floor(ret + 0.5) : ceil(ret - 0.5);
	ret*= pow(10.0 , -(b-1));
	
	return ret;
}

/**	\brief	The CHSRPostProcessor::IsPointsOnLine function\n
	if dot product of vec(pt1,pt2),vec(pt3,pt2) is greather than (1 - 0.001) or less than (-1 + 0.001)\n
	three points(pt1,pt2,pt3) is on one line.

	\param	pt1	a parameter of type PPOINT_T
	\param	pt2	a parameter of type PPOINT_T
	\param	pt3	a parameter of type PPOINT_T

	\return	bool	
*/
bool CHSRPostProcessor::IsPointsOnLine(PPOINT_T pt1, PPOINT_T pt2, PPOINT_T pt3)
{
	assert(pt1 && "pt1 is NULL");
	assert(pt2 && "pt2 is NULL");
	assert(pt3 && "pt3 is NULL");
	bool ret = false;

	if(pt1 && pt2 && pt3)
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();

		double dx[2],dy[2],dz[2];
		dx[0] = is_round(pt1->x - pt2->x , 9);
		dy[0] = is_round(pt1->y - pt2->y , 9);
		dz[0] = is_round(pt1->z - pt2->z , 9);
		dx[1] = is_round(pt3->x - pt2->x , 9);
		dy[1] = is_round(pt3->y - pt2->y , 9);
		dz[1] = is_round(pt3->z - pt2->z , 9);
		
		if(TOP == pHSRApp->m_clsView.GetViewPoint())
		{
			dz[0] = 0.; // z1 = 0.
			dz[1] = 0.; // z2 = 0.
			
		}
		else if(FRONT == pHSRApp->m_clsView.GetViewPoint())
		{
			dx[0] = 0.; // z1 = 0.
			dx[1] = 0.; // z2 = 0.
		}
		
		const double area = fabs(dx[0]*dy[1] - dy[0]*dx[1]);
		return (0 == area);
		/*
		len[0] = sqrt(dx[0]*dx[0] + dy[0]*dy[0] + dz[0]*dz[0]);
		len[1] = sqrt(dx[1]*dx[1] + dy[1]*dy[1] + dz[1]*dz[1]);
		dot    = dx[0]*dx[1] + dy[0]*dy[1] + dz[0]*dz[1];
		cosval = dot/(len[0]*len[1]);

		if((cosval <= (-1 + 0.001)) || (cosval >= (1 - 0.001))) ret = true;
		*/
	}

	return ret;
}
