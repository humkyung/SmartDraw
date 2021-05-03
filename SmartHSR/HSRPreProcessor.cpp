// CHSRPreProcessor.cpp: implementation of the CHSRPreProcessor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include "HSRPreProcessor.h"

#include "SmartHSR.h"
#include "Geo.h"
#include "HSROption.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CHSRPreProcessor::CHSRPreProcessor()
{

}

CHSRPreProcessor::~CHSRPreProcessor()
{

}

/**	\brief	The CHSRPreProcessor::Run function\n
	run preprocessor for primitive.

	\param	pFaceList	a parameter of type PFACE
	\param	type	a parameter of type int
	\param	pTop	a parameter of type PFACE
	\param	pSide	a parameter of type PFACE
	\param	pBottom	a parameter of type PFACE

	\return	PFACE	
*/
CHSRFace* CHSRPreProcessor::Run(CHSRFace* pFaceList,int type,CHSRFace* pTop,CHSRFace* pSide,CHSRFace* pBottom)
{
	switch(type)
	{
		case PIPE:
			pFaceList = RunForPipe(pFaceList,pTop,pSide,pBottom);
			break;
		case CYLINDER:case EQP_CYLINDER:
			pFaceList = RunForCylinder(pFaceList,pTop,pSide,pBottom);
			break;
		case TEE:case TEEX:
			pFaceList = RunForTee(pFaceList,pTop,pSide,pBottom);
			break;
		case CONE:case EQP_CONE:
			pFaceList = RunForCone(pFaceList,pTop,pSide,pBottom);
			break;
		case ELBOW:
			if(pSide) pFaceList = RunForElbow(pFaceList,pTop,pSide,pBottom);
			break;
		case SW_ELBOW:
		{
			if(pSide)
			{
				CHSRApp* pHSRApp= CHSRApp::GetInstance();
				pFaceList = RunForSWElbow(pFaceList,pTop,pSide,pBottom);
			}
		}
			break;
		case FLANGE:
			pFaceList = RunForFlange(pFaceList,pTop,pSide,pBottom);
			break;
		case DOME:
			pFaceList = RunForDome(pFaceList,pTop,pSide,pBottom);
			break;
		case PIPE_BEND:
			pFaceList = RunForSWElbow(pFaceList,pTop,pSide,pBottom);
			break;
	}

	return pFaceList;
}

/**	
	@brief	The CHSRPreProcessor::RunForElbow function

	@author	humkyung

	@param	pFaceList	a parameter of type PFACE
	@param	pTop	a parameter of type PFACE
	@param	pSide	a parameter of type PFACE
	@param	pBottom	a parameter of type PFACE

	@remarks\n
	;2002.05.22 - if linemode is SINGLE_LINE, does'nt append top face and bottom face

	@return	PFACE	
*/
CHSRFace* CHSRPreProcessor::RunForElbow(CHSRFace* pFaceList,CHSRFace* pTop, CHSRFace* pSide, CHSRFace* pBottom)
{
	PHSR_VERTEX ptr=NULL;
	CHSRFace*   pFace=NULL;
	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	
	if((SINGLE_LINE == pHSRApp->GetEnv()->GetPipeLineMode()) || ((BOTH == pHSRApp->GetEnv()->GetPipeLineMode()) 
		&& (pSide->radius <= pHSRApp->GetEnv()->GetCriticalRadius())))
	{
		pFaceList = pHSRApp->AppendFace(pFaceList,pSide);

		if(pSide->symbol[0])	 pFace = pTop;	 // this face is prependicular to view point.
		else if(pSide->symbol[1])pFace = pBottom;// this face is prependicular to view point.
		else
		{
			//if(pTop)    pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			//if(pBottom) pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
			
			return pFaceList;
		}

		pFaceList = AddElbowSymbol(pFaceList,pSide,pFace);
	}
	else if((DOUBLE_LINE == pHSRApp->GetEnv()->GetPipeLineMode()) || ((BOTH == pHSRApp->GetEnv()->GetPipeLineMode()) 
		&& (pSide->radius > pHSRApp->GetEnv()->GetCriticalRadius())))
	{
		if(pSide) pFaceList = pHSRApp->AppendFace(pFaceList,pSide);

		if(pTop)
		{
			for(ptr = pSide->pHead;ptr && ptr->next;)
			{
				if(!pTop->SplitLine(ptr,ptr->next)) ptr = ptr->next;
			}
			for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
			{
				if(pTop->IsHiddenLine(ptr,ptr->next))
				{
					ptr->render = INVALID;
				}
			}

			pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
		}

		if(pBottom)
		{
			for(ptr = pSide->pHead;ptr && ptr->next;)
			{
				if(!pBottom->SplitLine(ptr,ptr->next)) ptr = ptr->next;
			}
			for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
			{
				if((INTERSECTION == ptr->render) && pBottom->IsHiddenLine(ptr,ptr->next))
				{
					ptr->render = INVALID;
				}
			}
			
			pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
		}
	}
	
	return pFaceList;
}

/**	\brief	The CHSRPreProcessor::RunForSWElbow function\n
	append elbow's faces,elbow's symbol.

	\param	pFaceList	a parameter of type PFACE
	\param	pTop	a parameter of type PFACE
	\param	pSide	a parameter of type PFACE
	\param	pBottom	a parameter of type PFACE

	\remarks\n
	;2002.03.15 - now treat square type elbow.\n
	;2002.05.22 - if linemode is SINGLE_LINE, does not append top face and bottom face

	\return	PFACE	
*/
CHSRFace* CHSRPreProcessor::RunForSWElbow(CHSRFace* pFaceList, CHSRFace* pTop, CHSRFace* pSide, CHSRFace* pBottom)
{
	assert(pSide && "pSide is NULL");
	PHSR_VERTEX ptr=NULL;
	CHSRFace*   pFace=NULL;
	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	
	if((SINGLE_LINE == pHSRApp->GetEnv()->GetPipeLineMode()) || ((BOTH == pHSRApp->GetEnv()->GetPipeLineMode()) 
		&& (pSide->radius <= pHSRApp->GetEnv()->GetCriticalRadius())))
	{
		if(pHSRApp->GetEnv()->IsSquareTypeElbow())
		{
			for(PHSR_VERTEX ptr=pSide->pCenterline;ptr;)
			{
				PHSR_VERTEX ptrTmp = ptr;
				ptr = ptr->next;

				pHSRApp->free_point(ptrTmp->pt);
				pHSRApp->free_vertex(ptrTmp);
			}
			pSide->pCenterline = NULL;

			if(pSide->pCenterline = pHSRApp->alloc_vertex())
			{
				pSide->pCenterline->pt=pHSRApp->alloc_point();
				pSide->pCenterline->pt->x = pSide->ptEdge[0].x;
				pSide->pCenterline->pt->y = pSide->ptEdge[0].y;
				pSide->pCenterline->pt->z = pSide->ptEdge[0].z;
			}
			if(pSide->pCenterline->next = pHSRApp->alloc_vertex())
			{
				pSide->pCenterline->next->pt = pHSRApp->alloc_point();
				pSide->pCenterline->next->pt->x = pSide->ptCenter.x;
				pSide->pCenterline->next->pt->y = pSide->ptCenter.y;
				pSide->pCenterline->next->pt->z = pSide->ptCenter.z;
			}
			if(pSide->pCenterline->next->next = pHSRApp->alloc_vertex())
			{
				pSide->pCenterline->next->next->pt = pHSRApp->alloc_point();
				pSide->pCenterline->next->next->pt->x = pSide->ptEdge[1].x;
				pSide->pCenterline->next->next->pt->y = pSide->ptEdge[1].y;
				pSide->pCenterline->next->next->pt->z = pSide->ptEdge[1].z;
			}
		}
		pFaceList = pHSRApp->AppendFace(pFaceList,pSide);

		if(pSide->symbol[0])	  pFace = pTop;	  // this face is prependicular to view point.
		else if(pSide->symbol[1]) pFace = pBottom;// this face is prependicular to view point.
		else{
			//if(pTop)    pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			//if(pBottom) pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
			
			return pFaceList;
		}

		pFaceList = AddElbowSymbol(pFaceList,pSide,pFace);
	}
	else if((DOUBLE_LINE == pHSRApp->GetEnv()->GetPipeLineMode()) || 
		((BOTH == pHSRApp->GetEnv()->GetPipeLineMode()) && (pSide->radius > pHSRApp->GetEnv()->GetCriticalRadius())))
	{
		if(pSide)	pFaceList = pHSRApp->AppendFace(pFaceList,pSide);

		if(pTop)
		{
			int nCount=0;
			for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next) nCount++;
			
			int i=0,nMid=(int)(nCount*0.5);
			for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next,i++)
			{
				if((nMid == (i+1)) || (nCount == (i+1)))
				{
					if(((INTERSECTION == ptr->render) || (SHOW == ptr->render)) && pTop->IsHiddenLine(ptr,ptr->next))
						ptr->render = INVALID;
				}
			}

			pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
		}

		if(pBottom)
		{
			int nCount=0;
			for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next) nCount++;
			
			int i=0,nMid=(int)(nCount*0.5);
			for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next,i++)
			{
				if((nMid == (i+1)) || (nCount == (i+1)))
				{
					if(((INTERSECTION == ptr->render) || (SHOW == ptr->render)) && pBottom->IsHiddenLine(ptr,ptr->next))
						ptr->render = INVALID;
				}
			}

			pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
		}
	}
	
	return pFaceList;
}

/**	
	@brief	append elbow symbol to face list and set elbow's element to SYMBOL
	@author	humkyung
	@param	pFaceList	a parameter of type PFACE
	@param	pSide	a parameter of type PFACE
	@param	pFace	a parameter of type PFACE

	@return	PFACE	
*/
CHSRFace* CHSRPreProcessor::AddElbowSymbol(CHSRFace* pFaceList, CHSRFace* pSide,CHSRFace* pFace)
{
	assert(pSide && "pSide is NULL");
	assert(pFace && "pFace is NULL");
	assert(((ELBOW == pFace->type) || (SW_ELBOW == pFace->type)) && "pFace's type is invalid");
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	if(pSide && pFace && ((ELBOW == pFace->type) || (SW_ELBOW == pFace->type)))
	{
		double h=0.,len=0.;
		double angle = 0.;
		double dx[3]={0.},dy[3]={0.},dz[3]={0.};
		PHSR_VERTEX ptr=NULL;
		static FACE    face;
		static HSR_VERTEX  vt[5]={0};
		static POINT_T pt[5]={0};
	
		memset(vt,0,sizeof(HSR_VERTEX)*5);
		memset(pt,0,sizeof(POINT_T)*5);
		if(TOP == pHSRApp->m_clsView.GetViewPoint())
		{
			dx[0] = pFace->ptEdge[0].x - pFace->ptCenter.x;
			dy[0] = pFace->ptEdge[0].y - pFace->ptCenter.y;
			len   = sqrt(dx[0]*dx[0] + dy[0]*dy[0]);
			angle = (ELBOW == pFace->type) ? pHSRApp->GetEnv()->GetWeldElbowAngle() : pHSRApp->GetEnv()->GetSWElbowAngle();
			assert((angle > 0.) && (angle < 90.));
			assert(len > 0.);

			if(((angle > 0.) && (angle < 90.)) && (len > 0.))
			{
				h     = len*tan(DEG2RAD(angle)/2.);

				dx[1] = -dy[0]; // rotate 90 degree.
				dy[1] = dx[0];  //
				dx[2] = dy[0];  // rotate -90 degree.
				dy[2] = -dx[0]; //

				CMath::NormalizeVector(dx[1],dy[1],dz[1],&dx[0],&dy[0],&dz[0]);
				dx[1] = dx[0];
				dy[1] = dy[0];
				dz[1] = dz[0];
				dx[1] *= h;
				dy[1] *= h;
				dz[1] *= h;

				CMath::NormalizeVector(dx[2],dy[2],dz[2],&dx[0],&dy[0],&dz[0]);
				dx[2] = dx[0];
				dy[2] = dy[0];
				dz[2] = dz[0];
				dx[2] *= h;
				dy[2] *= h;
				dz[2] *= h;
		
				pt[0].x = pFace->ptCenter.x;
				pt[0].y = pFace->ptCenter.y;
				pt[1].x = pFace->ptEdge[0].x + dx[1];
				pt[1].y = pFace->ptEdge[0].y + dy[1];
				pt[2].x = pFace->ptEdge[0].x + dx[2];
				pt[2].y = pFace->ptEdge[0].y + dy[2];
				pt[3].x = pt[0].x;
				pt[3].y = pt[0].y;
			}else	return pFaceList;
		}
		else
		{
			dy[0] = pFace->ptEdge[0].y - pFace->ptCenter.y;
			dz[0] = pFace->ptEdge[0].z - pFace->ptCenter.z;
			len   = sqrt(dy[0]*dy[0] + dz[0]*dz[0]);
		}

		vt[0].pt= &pt[0];
		vt[1].pt= &pt[1];
		vt[2].pt= &pt[2];
		vt[3].pt= &pt[3];
		
		vt[0].next = &vt[1];
		vt[1].next = &vt[2];
		vt[2].next = &vt[3];
		vt[3].next = NULL;
		
		face.pHead = &vt[0];
		/// split line.
		for(ptr = pFace->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			face.SplitLine(ptr,ptr->next);
		}
		/// hidden test.
		for(ptr = pFace->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			if(face.IsHiddenLine(ptr,ptr->next)) ptr->render = INVALID;
		}
			
		pFace->ele = SYMBOL;
		pFaceList  = pHSRApp->AppendFace(pFaceList,pFace);
	}

	return pFaceList;
}

/**	\brief	The CHSRPreProcessor::RunForCone function

	\param	pFaceList	a parameter of type PFACE
	\param	pTop	a parameter of type PFACE
	\param	pSide	a parameter of type PFACE
	\param	pBottom	a parameter of type PFACE

	\remarks\n
	;2002.04.02 - if top face's center and bottom face's center is not on axis from view point,\n
	append top face and bottom face.

	\return	PFACE	
*/
CHSRFace* CHSRPreProcessor::RunForCone(CHSRFace* pFaceList, CHSRFace* pTop, CHSRFace* pSide, CHSRFace* pBottom)
{
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	if(pSide) pFaceList = pHSRApp->AppendFace(pFaceList,pSide);
	if((pTop && (SECTION == HSR_ELEMENT_MARK(pTop->ele))) && (pBottom && (SECTION == HSR_ELEMENT_MARK(pBottom->ele))))
	{
		if(pHSRApp->IsEqualPointFromViewPoint(pTop->ptCenter,pBottom->ptCenter))
		{
			if(TOP == pHSRApp->m_clsView.GetViewPoint())
			{
				if(pHSRApp->IsGreatNumber(pTop->ptCenter.z,pBottom->ptCenter.z))
				{
					if(!pHSRApp->IsLessNumber(pTop->radius,pBottom->radius))
					{
						pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
					}
					else
					{
						pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
						pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
					}
				}
				else
				{
					if(!pHSRApp->IsLessNumber(pBottom->radius,pTop->radius))
					{
						pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
					}
					else
					{
						pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
						pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
					}
				}
			}
			else if(FRONT == pHSRApp->m_clsView.GetViewPoint())
			{
				if(pHSRApp->IsGreatNumber(pTop->ptCenter.x,pBottom->ptCenter.x))
				{
					if(pHSRApp->IsGreatNumber(pTop->radius,pBottom->radius))
						pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
				}
				else
				{
					if(pHSRApp->IsGreatNumber(pBottom->radius,pTop->radius))
						pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
				}
			}
		}
		else
		{
			if(pTop)    pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			if(pBottom) pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
		}
	}
	else
	{
		if(pTop)    pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
		if(pBottom) pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
	}
	
	POINT_T ptMid={0};
	PHSR_VERTEX ptr = NULL;
	if(pSide && pTop)
	{
		for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			ptMid.x = (ptr->pt->x + ptr->next->pt->x)*0.5;
			ptMid.y = (ptr->pt->y + ptr->next->pt->y)*0.5;
			ptMid.z = (ptr->pt->z + ptr->next->pt->z)*0.5;
			if(pHSRApp->IsEqualPoint(ptMid,pTop->ptCenter))
			{
				ptr->render = INVALID;
				break;
			}
		}
	}
	
	if(pSide && pBottom)
	{
		for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			ptMid.x = (ptr->pt->x + ptr->next->pt->x)*0.5;
			ptMid.y = (ptr->pt->y + ptr->next->pt->y)*0.5;
			ptMid.z = (ptr->pt->z + ptr->next->pt->z)*0.5;
			if(pHSRApp->IsEqualPoint(ptMid,pBottom->ptCenter))
			{
				ptr->render = INVALID;
				break;
			}
		}
	}

	return pFaceList;
}

/**	\brief	The CHSRPreProcessor::RunForCylinder function\n
	run preprocessor for cylinder.

	\param	pFaceList	a parameter of type PFACE
	\param	pTop	a parameter of type PFACE
	\param	pSide	a parameter of type PFACE
	\param	pBottom	a parameter of type PFACE

	\return	PFACE	
*/
CHSRFace* CHSRPreProcessor::RunForCylinder(CHSRFace* pFaceList, CHSRFace* pTop, CHSRFace* pSide, CHSRFace* pBottom)
{
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	if(pSide) pFaceList = pHSRApp->AppendFace(pFaceList,pSide);
	if((pTop && (SECTION == HSR_ELEMENT_MARK(pTop->ele))) && (pBottom && (SECTION == HSR_ELEMENT_MARK(pBottom->ele))))
	{
		if(TOP == pHSRApp->m_clsView.GetViewPoint())
		{
			if(pTop->ptCenter.z > pBottom->ptCenter.z)
				pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			else	pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
		}
		else if(FRONT == pHSRApp->m_clsView.GetViewPoint())
		{
			if(pTop->ptCenter.x > pBottom->ptCenter.x)
				pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			else	pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
		}
	}
	else
	{
		if(pTop)    pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
		if(pBottom) pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
	}

		
	POINT_T ptMid={0};
	PHSR_VERTEX ptr = NULL;
	if(pSide && pTop)
	{
		for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			ptMid.x = (ptr->pt->x + ptr->next->pt->x)*0.5;
			ptMid.y = (ptr->pt->y + ptr->next->pt->y)*0.5;
			ptMid.z = (ptr->pt->z + ptr->next->pt->z)*0.5;
			if(pHSRApp->IsEqualPoint(ptMid,pTop->ptCenter))
			{
				ptr->render = INVALID;
				break;
			}
		}
	}
	
	if(pSide && pBottom)
	{
		for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			ptMid.x = (ptr->pt->x + ptr->next->pt->x)*0.5;
			ptMid.y = (ptr->pt->y + ptr->next->pt->y)*0.5;
			ptMid.z = (ptr->pt->z + ptr->next->pt->z)*0.5;
			if(pHSRApp->IsEqualPoint(ptMid,pBottom->ptCenter))
			{
				ptr->render = INVALID;
				break;
			}
		}
	}

	return pFaceList;
}

/**	\brief	The CHSRPreProcessor::RunForPipe function\n
	run preprocessor for pipe.

	\param	pFaceList	a parameter of type PFACE
	\param	pTop	a parameter of type PFACE
	\param	pSide	a parameter of type PFACE
	\param	pBottom	a parameter of type PFACE

	\return	PFACE	
*/
CHSRFace* CHSRPreProcessor::RunForPipe(CHSRFace* pFaceList, CHSRFace* pTop, CHSRFace* pSide, CHSRFace* pBottom)
{
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	if(pSide) pFaceList = pHSRApp->AppendFace(pFaceList,pSide);
	if((pTop && (SECTION == HSR_ELEMENT_MARK(pTop->ele))) && (pBottom && (SECTION == HSR_ELEMENT_MARK(pBottom->ele))))
	{
		if(TOP == pHSRApp->m_clsView.GetViewPoint())
		{
			if(pHSRApp->IsGreatNumber(pTop->ptCenter.z,pBottom->ptCenter.z))
				pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			else	pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
		}
		else if(FRONT == pHSRApp->m_clsView.GetViewPoint())
		{
			if(pHSRApp->IsGreatNumber(pTop->ptCenter.x,pBottom->ptCenter.x))
				pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			else	pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
		}
	}
	else
	{
		if(pTop)
		{
			PIPELINEMODE linemode=pHSRApp->GetEnv()->GetPipeLineMode();
			if((DOUBLE_LINE == linemode) || ((BOTH == linemode) && pTop->radius > pHSRApp->GetEnv()->GetCriticalRadius()))
			{
				pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			}else	pHSRApp->FreeFace(pTop);
		}
		if(pBottom)
		{
			PIPELINEMODE linemode=pHSRApp->GetEnv()->GetPipeLineMode();
			if((DOUBLE_LINE == linemode) || ((BOTH == linemode) && pBottom->radius > pHSRApp->GetEnv()->GetCriticalRadius()))
			{
				pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
			}else	pHSRApp->FreeFace(pBottom);
		}
	}

	POINT_T ptMid={0};
	PHSR_VERTEX ptr = NULL;
	if(pSide && pTop)
	{
		for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			ptMid.x = (ptr->pt->x + ptr->next->pt->x)*0.5;
			ptMid.y = (ptr->pt->y + ptr->next->pt->y)*0.5;
			ptMid.z = (ptr->pt->z + ptr->next->pt->z)*0.5;
			if(pHSRApp->IsEqualPoint(ptMid,pTop->ptCenter))
			{
				ptr->render = INVALID;
				break;
			}
		}
	}
	
	if(pSide && pBottom)
	{
		for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			ptMid.x = (ptr->pt->x + ptr->next->pt->x)*0.5;
			ptMid.y = (ptr->pt->y + ptr->next->pt->y)*0.5;
			ptMid.z = (ptr->pt->z + ptr->next->pt->z)*0.5;
			if(pHSRApp->IsEqualPoint(ptMid,pBottom->ptCenter))
			{
				ptr->render = INVALID;
				break;
			}
		}
	}

	return pFaceList;
}

/**	\brief	The CHSRPreProcessor::RunForFlange function\n
	run preprocessor for flange.

	\param	pFaceList	a parameter of type PFACE
	\param	pTop	a parameter of type PFACE
	\param	pSide	a parameter of type PFACE
	\param	pBottom	a parameter of type PFACE

	\return	PFACE	
*/
CHSRFace* CHSRPreProcessor::RunForFlange(CHSRFace* pFaceList, CHSRFace* pTop, CHSRFace* pSide, CHSRFace* pBottom)
{
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	if(pSide) pFaceList = pHSRApp->AppendFace(pFaceList,pSide);
	if((pTop && (SECTION == HSR_ELEMENT_MARK(pTop->ele))) && (pBottom && (SECTION == HSR_ELEMENT_MARK(pBottom->ele))))
	{
		if(TOP == pHSRApp->m_clsView.GetViewPoint())
		{
			if(pHSRApp->IsGreatNumber(pTop->ptCenter.z,pBottom->ptCenter.z))
				pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			else	pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
		}
		else if(FRONT == pHSRApp->m_clsView.GetViewPoint())
		{
			if(pHSRApp->IsGreatNumber(pTop->ptCenter.x,pBottom->ptCenter.x))
				pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			else	pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
		}
	}
	else
	{
		if(pTop)    pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
		if(pBottom) pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
	}

	POINT_T ptMid={0};
	PHSR_VERTEX ptr = NULL;
	if(pSide && pTop)
	{
		for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			ptMid.x = (ptr->pt->x + ptr->next->pt->x)*0.5;
			ptMid.y = (ptr->pt->y + ptr->next->pt->y)*0.5;
			ptMid.z = (ptr->pt->z + ptr->next->pt->z)*0.5;
			if(pHSRApp->IsEqualPoint(ptMid,pTop->ptCenter))
			{
				ptr->render = INVALID;
				break;
			}
		}
	}
	
	if(pSide && pBottom)
	{
		for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			ptMid.x = (ptr->pt->x + ptr->next->pt->x)*0.5;
			ptMid.y = (ptr->pt->y + ptr->next->pt->y)*0.5;
			ptMid.z = (ptr->pt->z + ptr->next->pt->z)*0.5;
			if(pHSRApp->IsEqualPoint(ptMid,pBottom->ptCenter))
			{
				ptr->render = INVALID;
				break;
			}
		}
	}

	return pFaceList;
}

/**	
	@brief	run preprocessor for Tee.

	@author	humkyung

	@param	pFaceList	a parameter of type PFACE
	@param	pTop	a parameter of type PFACE
	@param	pSide	a parameter of type PFACE
	@param	pBottom	a parameter of type PFACE

	@return	PFACE	
*/
CHSRFace* CHSRPreProcessor::RunForTee(CHSRFace* pFaceList, CHSRFace* pTop, CHSRFace* pSide, CHSRFace* pBottom)
{
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	if(pSide) pFaceList = pHSRApp->AppendFace(pFaceList,pSide);
	if((pTop && (SECTION == HSR_ELEMENT_MARK(pTop->ele))) && (pBottom && (SECTION == HSR_ELEMENT_MARK(pBottom->ele))))
	{
		if(TOP == pHSRApp->m_clsView.GetViewPoint())
		{
			if(pHSRApp->IsGreatNumber(pTop->ptCenter.z,pBottom->ptCenter.z))
				pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			else	pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
		}
		else if(FRONT == pHSRApp->m_clsView.GetViewPoint())
		{
			if(pHSRApp->IsGreatNumber(pTop->ptCenter.x,pBottom->ptCenter.x))
				pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			else	pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
		}
	}
	else
	{
		if(pTop)    pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
		if(pBottom) pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
	}

	POINT_T ptMid={0};
	PHSR_VERTEX ptr = NULL;
	if(pSide && pTop)
	{
		for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			ptMid.x = (ptr->pt->x + ptr->next->pt->x)*0.5;
			ptMid.y = (ptr->pt->y + ptr->next->pt->y)*0.5;
			ptMid.z = (ptr->pt->z + ptr->next->pt->z)*0.5;
			if(pHSRApp->IsEqualPoint(ptMid,pTop->ptCenter))
			{
				ptr->render = INVALID;
				break;
			}
		}
	}
	
	if(pSide && pBottom)
	{
		for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			ptMid.x = (ptr->pt->x + ptr->next->pt->x)*0.5;
			ptMid.y = (ptr->pt->y + ptr->next->pt->y)*0.5;
			ptMid.z = (ptr->pt->z + ptr->next->pt->z)*0.5;
			if(pHSRApp->IsEqualPoint(ptMid,pBottom->ptCenter))
			{
				ptr->render = INVALID;
				break;
			}
		}
	}

	return pFaceList;
}

/**	
	@brief	run preprocessor for Dome.

	@author	humkyung

	@param	pFaceList	a parameter of type PFACE
	@param	pTop	a parameter of type PFACE
	@param	pSide	a parameter of type PFACE
	@param	pBottom	a parameter of type PFACE

	@return	PFACE	
*/
CHSRFace* CHSRPreProcessor::RunForDome(CHSRFace* pFaceList, CHSRFace* pTop, CHSRFace* pSide, CHSRFace* pBottom)
{
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	if(pSide) pFaceList = pHSRApp->AppendFace(pFaceList,pSide);
	if((pTop && (SECTION == HSR_ELEMENT_MARK(pTop->ele))) && (pBottom && (SECTION == HSR_ELEMENT_MARK(pBottom->ele))))
	{
		if(TOP == pHSRApp->m_clsView.GetViewPoint())
		{
			if(pTop->ptCenter.z > pBottom->ptCenter.z)
				pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			else	pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
		}
		else if(FRONT == pHSRApp->m_clsView.GetViewPoint())
		{
			if(pTop->ptCenter.x > pBottom->ptCenter.x)
				pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
			else	pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
		}
	}
	else
	{
		if(pTop)    pFaceList = pHSRApp->AppendFace(pFaceList,pTop);
		if(pBottom) pFaceList = pHSRApp->AppendFace(pFaceList,pBottom);
	}

		
	POINT_T ptMid={0};
	PHSR_VERTEX ptr = NULL;
	if(pSide && pTop)
	{
		for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			ptMid.x = (ptr->pt->x + ptr->next->pt->x)*0.5;
			ptMid.y = (ptr->pt->y + ptr->next->pt->y)*0.5;
			ptMid.z = (ptr->pt->z + ptr->next->pt->z)*0.5;
			if(pHSRApp->IsEqualPoint(ptMid,pTop->ptCenter))
			{
				ptr->render = INVALID;
				break;
			}
		}
	}
	
	if(pSide && pBottom)
	{
		for(ptr = pSide->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			ptMid.x = (ptr->pt->x + ptr->next->pt->x)*0.5;
			ptMid.y = (ptr->pt->y + ptr->next->pt->y)*0.5;
			ptMid.z = (ptr->pt->z + ptr->next->pt->z)*0.5;
			if(pHSRApp->IsEqualPoint(ptMid,pBottom->ptCenter))
			{
				ptr->render = INVALID;
				break;
			}
		}
	}

	return pFaceList;
}
