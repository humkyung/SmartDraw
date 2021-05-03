//-------------------------------------------------------------------
//	file name	: face.cpp
//	author		: baek hum kyung
//	date		: 2001.10. ~
//	description	: source file of face
//			  all vertices in face must be on a plane.
//	update		: convert face class to template class
//-------------------------------------------------------------------
#include "stdafx.h"
#include <stdlib.h>

#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "HSR.h"
#include "SmartHSR.h"
#include "HSRFace.h"
#include "HiddenDef.h"
#include "Geo.h"

#define FACE_MAX_POINTS	512
static const HSR_PRIMITIVE_E __INTERESTINGS__[] = { PIPE , PIPEX , PIPE_BEND , TEE , TEEX , ELBOW , SW_ELBOW , MITER };

/**	
	@brief
*/
CHSRFace::CHSRFace()
{
	Plane.a = Plane.b = Plane.c = Plane.d = 0;
	hidden  = false;
	pHead = NULL;
	pCenterline = pCenterline = pSymboline = pCrossline = NULL;
	pSibling = NULL;
	prev = next = NULL;
	front= back = NULL;
	region= 0;

	symbol[0] = symbol[1] = false;
	mark[0] = mark[1] = false;
	radius = 0.f;

	body = NULL;	/// 2012.01.12 added by humkyung
	memset(&volume , 0 , sizeof(VOLUME_T));	/// 2012.12.10 added by humkyung
	m_enmLayer = LAYER_NONE;
}

CHSRFace::~CHSRFace()
{
	try
	{
		/// do nothing
	}
	catch(...)
	{
	}
}

/**	
	@brief	The CHSRFace::IsInnerVertex function

	@param	pt	a parameter of type PPOINT_T
	@param	coplane	a parameter of type bool

	@remarks\n
	;2002.05.22 - add routine checking point located on boundary

	@return	bool	
*/
bool CHSRFace::IsInnerVertex(PPOINT_T pt,bool coplane)
{
	assert(pt && "pt is NULL");
	#define TOLERANCE 0.0

	static  POINT_T P[512]={0,};
	int     n=0;   // point index; i1 = i-1 mod n 
	double  x=0;          // x intersection of e with ray 
	int     Rcross = 0; // number of right edge/ray crossings
	int     Lcross = 0; // number of left edge/ray crossings

	if(!pt) return false;
	// Shift so that q is the origin. Note this destroys the polygon.
	// This is done for pedogical clarity.
	int i=0;
	for(PHSR_VERTEX pVt = pHead;pVt;pVt = pVt->next,i++)
	{
		if(pVt->next)
		{
			VECTOR_T vec[2]={0,};
			
			vec[0].dx = pVt->pt->x - pt->x;
			vec[0].dy = pVt->pt->y - pt->y;
			vec[0].dz = pVt->pt->z - pt->z;
			vec[1].dx = pVt->next->pt->x - pt->x;
			vec[1].dy = pVt->next->pt->y - pt->y;
			vec[1].dz = pVt->next->pt->z - pt->z;
			/// check if has same direction
			if(vec[0].dx*vec[1].dy == vec[0].dy*vec[1].dx)
			{
				/// check if pt is located on between two vertices - 2012.02.22 added by humkyung
				vec[0].dz = vec[1].dz = 0.0;
				const bool a = CMath::NormalizeVector(vec[0]);
				const bool b = CMath::NormalizeVector(vec[1]);
				if(a && b)
				{
					VECTOR_T res;
					res.dx = vec[0].dx + vec[1].dx;
					res.dy = vec[0].dy + vec[1].dy;
					res.dz = vec[0].dz + vec[1].dz;
					const double length = res.dx*res.dx + res.dy*res.dy + res.dz*res.dz;
					if(0.0 == length) return false;
				}
				else
				{
					return false;
				}
			}	
		}

		if(ISEQUAL(pVt->pt->x,pt->x) && ISEQUAL(pVt->pt->y,pt->y)) return false;
		P[i].x = (pVt->pt->x) - (pt->x);
		P[i].y = (pVt->pt->y) - (pt->y);
	}
	n = i;

	int i1=0;
	// For each edge e=(i-1,i), see if crosses ray.
	for(i = 0;i < n;i++) 
	{
		//* First see if q=(0,0) is a vertex.
		//if ((P[i].x == 0.0) &&  (P[i].y == 0.0) && coplane) return false;
		i1 = ( i + n - 1 ) % n;

		//* if e "straddles" the x-axis...
		//* The commented-out statement is logically equivalent to the one 
		//* following.
		//* if( ( ( P[i][Y] > 0 ) && ( P[i1][Y] <= 0 ) ) ||
		//* ( ( P[i1][Y] > 0 ) && ( P[i][Y] <= 0 ) ) ) {   
		if( ( P[i].y  > 0. ) != ( P[i1].y > 0. ) ) 
		{
			//* e straddles ray, so compute intersection with ray.
			x = (P[i].x * (double)P[i1].y - P[i1].x * (double)P[i].y)/(double)(P[i1].y - P[i].y);

			//* crosses ray if strictly positive intersection.
			if (x >= (0 + HSR_TOLER)) Rcross++;
		}
    
		//* if e straddles the x-axis when reversed... */
		//* if( ( ( P[i] [Y] < 0 ) && ( P[i1][Y] >= 0 ) ) ||
		//* ( ( P[i1][Y] < 0 ) && ( P[i] [Y] >= 0 ) ) )  {
		if ( ( P[i].y < 0 ) != ( P[i1].y < 0 ) ) 
		{ 
			//* e straddles ray, so compute intersection with ray.
			x = (P[i].x * P[i1].y - P[i1].x * P[i].y)/(double)(P[i1].y - P[i].y);

			//* crosses ray if strictly positive intersection.
			if (x <= (0 + HSR_TOLER)) Lcross++;
		}
	}
  
	//* q on the edge if left and right cross are not the same parity.
	if((Rcross%2 ) != (Lcross%2)) return true;

	//* q inside iff an odd number of crossings.
	if((Rcross % 2) == 1) return true;
	else		      return false;
}

/**	
	@brief	split head and centerline by pFace.

	@author	humkyung

	@param	pFace	face cutting(에지를 자를 면.)
	@param	coplane	a parameter of type bool

	@remarks
	2007.03.30 - delete single line mode section.

	@return	bool	
*/
bool CHSRFace::SplitFaceLine(CHSRFace* pFace,bool coplane)
{
	assert(pFace && "face is invalid");
	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	PIPELINEMODE LineMode  = pHSRApp->GetEnv()->GetPipeLineMode();
	bool ret = false;

	if(pFace && pFace->GetRange().CollideWith(this->GetRange()))///!pHSRApp->IsOuterVolume(&(pFace->volume),&volume))
	{
		PHSR_VERTEX ptr = NULL;

		const size_t nCount = sizeof(__INTERESTINGS__) / sizeof(__INTERESTINGS__[0]);
		const HSR_PRIMITIVE_E* p  = find(__INTERESTINGS__ , __INTERESTINGS__ + nCount , type);

		if((p - __INTERESTINGS__) < nCount)
		{
			if((SINGLE_LINE == LineMode) || ((BOTH == LineMode) && (radius <= pHSRApp->GetEnv()->GetCriticalRadius())))
			{
				for(ptr = pCenterline;(ptr && ptr->next);)
				{
					if((SHOW == ptr->render) || (INTERSECTION == ptr->render))
						ptr = pFace->SplitEdge(ptr,ptr->next,coplane);
					else	ptr = ptr->next;
				}
				
				return ret;
			}
		}

		/// split face edges by pFace.
		for(ptr = pHead;ptr && ptr->next;)
		{
			if((SHOW == ptr->render) || (INTERSECTION == ptr->render))
				ptr = pFace->SplitEdge(ptr,ptr->next,coplane);
			else	ptr = ptr->next;
		}
		
		//! split symbol line by pFace.
		for(ptr = pSymboline;(ptr && ptr->next);)
		{
			if((SHOW == ptr->render) || (INTERSECTION == ptr->render))
				ptr = pFace->SplitEdge(ptr,ptr->next,coplane);
			else	ptr = ptr->next;
		}
		
		if(!coplane)
		{
			for(ptr = pCenterline;(ptr && ptr->next);)
			{
				if((SHOW == ptr->render) || (INTERSECTION == ptr->render))
					ptr = pFace->SplitEdge(ptr,ptr->next,coplane);
				else	ptr = ptr->next;
			}
		}
	}
	
	return ret;
}

/**	
	@brief	split edge line(v1,vt2)

	\param	vt1	a parameter of type PVERTEX
	\param	vt2	a parameter of type PVERTEX
	\param	coplane	coplane indicates vt1 and vt2 is coplane with this.

	\remarks\n
	;2003.08.02 - intersect line with 'TOLER'.
	;2004.12.03 - check for loop end condition.(vtr != vt2)

	\return	PVERTEX	
*/
PHSR_VERTEX CHSRFace::SplitEdge(PHSR_VERTEX vt1,PHSR_VERTEX vt2,bool coplane)
{
	assert(vt1 && "vt1 is NULL");
	assert(vt2 && "vt2 is NULL");
	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	double x=0.,y=0.,z=0.;
	bool ret[2]={false,};

	if(vt1 && vt2)
	{
		PHSR_VERTEX ptr_next=NULL;
		for(PHSR_VERTEX ptr = pHead;(ptr && ptr->next);ptr = ptr_next)
		{
			ptr_next = ptr->next;
			for(PHSR_VERTEX vtr=vt1;vtr != vt2;vtr = vtr->next)
			{
				if(DO_INTERSECT == CGeometry::IntersectLine2D(&x,&y,
					vtr->pt->x,vtr->pt->y,vtr->next->pt->x,vtr->next->pt->y,
					ptr->pt->x,ptr->pt->y,ptr->next->pt->x,ptr->next->pt->y,0.))
				{
					PHSR_VERTEX pReturn = vt2,pTmp = NULL,pSelf = NULL;
					if(!((ISEQUAL(x,vtr->pt->x) && ISEQUAL(y,vtr->pt->y)) || 
					     (ISEQUAL(x,vtr->next->pt->x) && ISEQUAL(y,vtr->next->pt->y))))
					{
						if(NULL != (pTmp = pHSRApp->NewVertex()))
						{
							pTmp->pt->x = x;
							pTmp->pt->y = y;					
							double len = sqrt(
								(vtr->next->pt->x - vtr->pt->x)*(vtr->next->pt->x - vtr->pt->x) + 
								(vtr->next->pt->y - vtr->pt->y)*(vtr->next->pt->y - vtr->pt->y) + 
								(vtr->next->pt->z - vtr->pt->z)*(vtr->next->pt->z - vtr->pt->z));
							
							if(vtr->next->pt->x - vtr->pt->x)
							{
								double width = len*(x - vtr->pt->x)/(vtr->next->pt->x - vtr->pt->x);
								pTmp->pt->z = vtr->pt->z + width*(vtr->next->pt->z - vtr->pt->z)/len;
							}
							else if(vtr->next->pt->y - vtr->pt->y)
							{
								double width = len*(y - vtr->pt->y)/(vtr->next->pt->y - vtr->pt->y);
								pTmp->pt->z = vtr->pt->z + width*(vtr->next->pt->z - vtr->pt->z)/len;
							}else	pTmp->pt->z = vtr->pt->z;
							
							pTmp->id    = vtr->id;
							pTmp->render= vtr->render;
							pTmp->next  = vtr->next;
							vtr->next   = pTmp;
							
							ret[0] = true;
						}
					}
					
					if(coplane)
					{
						if(!((ISEQUAL(x,ptr->pt->x) && ISEQUAL(y,ptr->pt->y)) || (ISEQUAL(x,ptr->next->pt->x) && ISEQUAL(y,ptr->next->pt->y))))
						{
							if(pTmp && (NULL != (pSelf = pHSRApp->NewVertex())))
							{
								pSelf->pt->x = x;
								pSelf->pt->y = y;
								pSelf->pt->z = z;
								
								pSelf->id    = ptr->id;
								pSelf->render= ptr->render;
								pSelf->next  = ptr->next;
								ptr->next    = pSelf;
								
								ret[1] = true;
								ptr_next = ptr;
							}
						}
					}else	ret[1] = true;

					break;
				}
			}
		}
	}
	
	return vt2;
}

/**	\brief	The CHSRFace::SplitLine function\n
	split line which has vt1 and vt2 as edge point.

	\param	vt1	a parameter of type PVERTEX
	\param	vt2	a parameter of type PVERTEX
	\param	selfsplit	a parameter of type bool

	\return	bool	
*/
bool CHSRFace::SplitLine(PHSR_VERTEX vt1,PHSR_VERTEX vt2,bool selfsplit)
{
	assert(vt1 && "vt1 is NULL");
	assert(vt2 && "vt2 is NULL");
	bool ret[2] = {false};
	PHSR_VERTEX pVt=NULL;
	VIEWPOINT v=TOP;
	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	double x=0.,y=0.,z=0.;

	if(TOP == (v = pHSRApp->m_clsView.GetViewPoint()))
	{
		for(pVt = pHead;(pVt && pVt->next);pVt = pVt->next)
		{
			for(PHSR_VERTEX ptrDest=vt1;ptrDest != vt2;ptrDest = ptrDest->next)
			{
				if(DO_INTERSECT == CGeometry::IntersectLine2D(&x,&y,
					ptrDest->pt->x,ptrDest->pt->y,ptrDest->next->pt->x,ptrDest->next->pt->y,
					pVt->pt->x,pVt->pt->y,pVt->next->pt->x,pVt->next->pt->y,0))
				{
					
					PHSR_VERTEX pTmp = NULL,pSelf = NULL;
					if(!((ISEQUAL(x,ptrDest->pt->x) && ISEQUAL(y,ptrDest->pt->y)) || 
						(ISEQUAL(x,ptrDest->next->pt->x) && ISEQUAL(y,ptrDest->next->pt->y))))
					{
						if(NULL != (pTmp = pHSRApp->NewVertex()))
						{
							pTmp->pt->x = x;
							pTmp->pt->y = y;
							double len = sqrt(
								(ptrDest->next->pt->x - ptrDest->pt->x)*(ptrDest->next->pt->x - ptrDest->pt->x) + 
								(ptrDest->next->pt->y - ptrDest->pt->y)*(ptrDest->next->pt->y - ptrDest->pt->y) + 
								(ptrDest->next->pt->z - ptrDest->pt->z)*(ptrDest->next->pt->z - ptrDest->pt->z));
							
							if(ptrDest->next->pt->x - ptrDest->pt->x)
							{
								double width = len*(x - ptrDest->pt->x)/(ptrDest->next->pt->x - ptrDest->pt->x);
								pTmp->pt->z = ptrDest->pt->z + width*(ptrDest->next->pt->z - ptrDest->pt->z)/len;
							}
							else if(ptrDest->next->pt->y - ptrDest->pt->y)
							{
								double width = len*(y - ptrDest->pt->y)/(ptrDest->next->pt->y - ptrDest->pt->y);
								pTmp->pt->z = ptrDest->pt->z + width*(ptrDest->next->pt->z - ptrDest->pt->z)/len;
							}
							else	pTmp->pt->z = ptrDest->pt->z;
							
							pTmp->id      = ptrDest->id;
							pTmp->render  = ptrDest->render;
							pTmp->next    = ptrDest->next;
							ptrDest->next = pTmp;
							
							ret[0] = true;
						}
					}else	continue;
					
					if(selfsplit)
					{
						if(!((ISEQUAL(x,pVt->pt->x) && ISEQUAL(y,pVt->pt->y)) || (ISEQUAL(x,pVt->next->pt->x) && ISEQUAL(y,pVt->next->pt->y))))
						{
							if(pTmp && (NULL != (pSelf = pHSRApp->NewVertex())))
							{
								pSelf->pt->x = x;
								pSelf->pt->y = y;
								pSelf->pt->z = z;
								
								pSelf->id    = pVt->id;
								pSelf->render= pVt->render;
								pSelf->next  = pVt->next;
								pVt->next    = pSelf;
								
								ret[1] = true;
							}
						}
					}else	ret[1] = true;
					break;
				}
			}
		}
	}

	return false;//(ret[0] && ret[1]);
}

/**	
	@brief	test point is mid-point of between vt1 and vt2.

	@author	humkyung

	@param	vt1	a parameter of type PVERTEX
	@param	vt2	a parameter of type PVERTEX
	@param	coplane	a parameter of type bool

	@return	true or false
*/
bool CHSRFace::IsHiddenLine(PHSR_VERTEX vt1,PHSR_VERTEX vt2,bool coplane)
{
	assert(vt1 && "vt1 is NULL");
	assert(vt2 && "vt2 is NULL");
	POINT_T pt={0,};

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(TOP == pHSRApp->m_clsView.GetViewPoint())
	{
		pt.x = (vt1->pt->x + vt2->pt->x)*0.5;
		pt.y = (vt1->pt->y + vt2->pt->y)*0.5;

		if(IsInnerVertex(&pt,coplane)) return true;
	}
	else if(FRONT == pHSRApp->m_clsView.GetViewPoint())
	{
		pt.y = (vt1->pt->y + vt2->pt->y)*0.5;
		pt.z = (vt1->pt->z + vt2->pt->z)*0.5;

		if(IsInnerVertex(&pt,coplane)) return true;
	}

	return false;
}

/**	
	@brief	: if line is invisible from view point, set the line to hidden. if coplane is setted ...

	@author	: humkyung

	@param	: pDestFace	face hiding
	@param	: coplane	a parameter of type bool

	@remarks
	;2002.04.26 - when line type is single line type,\n
	if this face is tee and symbol, run hidden routine.\n
	;2002.05.20 - when line type is single line type\n
	if this face is (elbow or sw_elbow) and symbol, run hidden routine.

	@return	void	
*/
void CHSRFace::ExtractHiddenLine(CHSRFace* pDestFace,bool coplane)
{
	assert(pDestFace && "face is invalid");
	
	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	PIPELINEMODE LineMode  = pHSRApp->GetEnv()->GetPipeLineMode();
	
	if(pDestFace && (id != pDestFace->id)/* && !pHSRApp->IsOuterVolume(&(pDestFace->volume),&volume)*/)
	{
		PHSR_VERTEX ptr = NULL;
		
		{
			const size_t nCount = sizeof(__INTERESTINGS__) / sizeof(__INTERESTINGS__[0]);
			const HSR_PRIMITIVE_E* p  = find(__INTERESTINGS__ , __INTERESTINGS__ + nCount , type);

			///((PIPE == type) || (PIPEX == type) || (PIPE_BEND == type) ||
			///(TEE == type) || (TEEX == type) || (ELBOW == type) || (SW_ELBOW == type))
			if((p - __INTERESTINGS__) < nCount)
			{
				if((SINGLE_LINE == LineMode) || ((BOTH == LineMode) && (radius <= pHSRApp->GetEnv()->GetCriticalRadius())))
				{
					for(ptr = pCenterline,hidden = true;ptr && ptr->next;ptr = ptr->next)
					{
						if(((SHOW == ptr->render) || (INTERSECTION == ptr->render)) && pDestFace->IsHiddenLine(ptr,ptr->next,coplane))
						{
							ptr->render = HIDE;
						}else	hidden = false;
					}

					//--> 2002.04.26
					if(((TEE == type) || (TEEX == type)) && (SYMBOL == HSR_ELEMENT_MARK(ele)))
					{
						for(ptr = pHead,hidden = true;ptr && ptr->next;ptr = ptr->next)
						{
							if(((SHOW == ptr->render) || (INTERSECTION == ptr->render)) && pDestFace->IsHiddenLine(ptr,ptr->next,coplane))
							{
								ptr->render = HIDE;
							}else	hidden = false;
						}
					}
					else if(((ELBOW == type) || (SW_ELBOW == type)) && (SYMBOL == HSR_ELEMENT_MARK(ele)))
					{ //--> 2002.05.20
						for(ptr = pHead,hidden = true;ptr && ptr->next;ptr = ptr->next)
						{
							if(((SHOW == ptr->render) || (INTERSECTION == ptr->render)) && pDestFace->IsHiddenLine(ptr,ptr->next,coplane))
							{
								ptr->render = HIDE;
							}else	hidden = false;
						}
					}
					//<--
					return;
				}
			}

			//--> 2001.10.13
			for(ptr = pHead,hidden = true;ptr && ptr->next;ptr = ptr->next)
			{
				if((SHOW == ptr->render) || (INTERSECTION == ptr->render))
				{
					if(pDestFace->IsHiddenLine(ptr,ptr->next,coplane))
					{
						ptr->render = HIDE;
						ptr->hider  = pDestFace->body;	/// 2012.01.12 added by humkyung
					}else	hidden = false;
				}
			}

			for(ptr = pCenterline;ptr && ptr->next;ptr = ptr->next)
			{
				if(((SHOW == ptr->render) || (INTERSECTION == ptr->render)) && pDestFace->IsHiddenLine(ptr,ptr->next,coplane))
					ptr->render = HIDE;
			}
		}
	}
}

/**
	@brief		

	@parameter	
	
	@author		BHK

	@description	Computes plane equation.
	
	@remarks	nCount is number of vertices.(head and tail must be equal vertex)
			nCount is greater than 3.
			plane - plane equation returned

	@returns	success - true , fail - false
*/
bool CHSRFace::CalcPlaneEquation()
{
	assert(pHead        && "pHead is invalid");
	assert((nCount >= 3) && "nCount less than 3");
	bool bRet = false;

	if(pHead && (nCount >= 3))
	{
		static double x[FACE_MAX_POINTS]={0.,};
		static double y[FACE_MAX_POINTS]={0.,};
		static double z[FACE_MAX_POINTS]={0.,};
		double p_x=0.,p_y=0.,p_z=0.;
		
		long nCount=0L;
		for(PHSR_VERTEX vtr=pHead;vtr;vtr = vtr->next,nCount++)
		{
			x[nCount] = vtr->pt->x;
			y[nCount] = vtr->pt->y;
			z[nCount] = vtr->pt->z;
			p_x += x[nCount];p_y += y[nCount];p_z += z[nCount];
		}
		p_x /= nCount;p_y /= nCount;p_z /= nCount;

		for(long i=0;i < nCount;i++)
		{
			Plane.a += (y[i] - y[(i+1)%nCount])*(z[i] + z[(i+1)%nCount]);
			Plane.b += (z[i] - z[(i+1)%nCount])*(x[i] + x[(i+1)%nCount]);
			Plane.c += (x[i] - x[(i+1)%nCount])*(y[i] + y[(i+1)%nCount]);
		}
		const double l=sqrt(Plane.a*Plane.a + Plane.b*Plane.b + Plane.c*Plane.c);
		if(0 == l) return false;
		Plane.a /= l;Plane.b /= l;Plane.c /= l;
		Plane.d = -(p_x*Plane.a + p_y*Plane.b + p_z*Plane.c);
		
		return true;
	}

	return bRet;
}

/**
	@brief	calculate plane equation of points on one plane.

	@author	humkyung

	@param	plane = result of calculation
		nPoints = number of points
		ppts = pointer of point

	@return	true of false
*/
bool CHSRFace::CalcPlaneEquationOf(PLANE_T& plane , const int& nPoints , POINT_T* ppts)
{
	assert(ppts         && "ppts is invalid");
	assert((nPoints > 3) && "nPoints less than 3");
	bool bRet = false;
	
	if(ppts && (nPoints > 3))
	{
		static double x[FACE_MAX_POINTS]={0.,};
		static double y[FACE_MAX_POINTS]={0.,};
		static double z[FACE_MAX_POINTS]={0.,};
		double p_x=0.,p_y=0.,p_z=0.;
		
		int i = 0;
		for(i = 0;i < nPoints;++i)
		{
			x[i] = ppts[i].x;
			y[i] = ppts[i].y;
			z[i] = ppts[i].z;
			p_x += x[i];p_y += y[i];p_z += z[i];
		}
		p_x /= nPoints;p_y /= nPoints;p_z /= nPoints;
		
		for(i = 0;i < nPoints;++i)
		{
			plane.a += (y[i] - y[(i+1)%nPoints])*(z[i] + z[(i+1)%nPoints]);
			plane.b += (z[i] - z[(i+1)%nPoints])*(x[i] + x[(i+1)%nPoints]);
			plane.c += (x[i] - x[(i+1)%nPoints])*(y[i] + y[(i+1)%nPoints]);
		}
		const double l = sqrt(plane.a*plane.a + plane.b*plane.b + plane.c*plane.c);
		if(0.f == l) return false;
		plane.a /= l;plane.b /= l;plane.c /= l;
		plane.d = -(p_x*plane.a + p_y*plane.b + p_z*plane.c);
		
		return true;
	}
	
	return bRet;
}

/**	\brief	The CHSRFace::UnionFaces function


	\return	void	
*/
void CHSRFace::UnionFaces()
{
	assert(pSibling && "pSibling is NULL");

	if(pSibling)
	{
		int nSiblings=0;
		CHSRFace *ptr = NULL,*part=NULL,*ptrPrev = NULL;

		// union face separated from this face.
		while(pSibling && UnionFace(pSibling))
		{
			pSibling = pSibling->pSibling;
		}
		// union sub faces.
		if(pSibling && pSibling->pSibling)
		{
			pSibling->UnionFaces();
		}
		// union face separated from this face.
		if(pSibling)
		{
			while(pSibling && UnionFace(pSibling))
			{
				pSibling = pSibling->pSibling;
			}
		}
	}
}

/**	\brief	The CHSRFace::UnionFace function
	원래 하나의 변에서 분리된 두개의 면을 다시 하나로 합친다.

	\param	pSiblingFace	a part of face.

	\remarks
	find connection points.
	merge tow faces and face's centerline.

	\return	bool	
*/
bool CHSRFace::UnionFace(CHSRFace* pSiblingFace)
{
	assert(pSiblingFace && "pSibling is invaid");
	bool ret = false;

	if(pSiblingFace)
	{
		PHSR_VERTEX pVt1,pVt2,pBeforeVt1,pBeforeVt2=NULL;
		PHSR_VERTEX ptr=NULL;

		pVt1 = pSiblingFace->pHead;
		for(ptr = pHead;ptr && ptr->next && !IS_EQUAL_VERTEX(ptr,pVt1);ptr = ptr->next);
		pBeforeVt1 = ptr;
		
		if(pBeforeVt1 && pBeforeVt1->next)
		{ // exclude one point face(?)
			for(pVt2 = pBeforeVt1->next;pVt2 && pVt2->next;pVt2 = pVt2->next)
			{
				for(ptr = pVt1->next;ptr && ptr->next;ptr = ptr->next)
				{
					if(IS_EQUAL_VERTEX(ptr,pVt2))
					{
						if(ptr && (CUT == ptr->render) && ptr->next)
						{
							pBeforeVt2 = ptr;
							break;
						}
					}
				}
				if(pBeforeVt2) break;
			}
			if(pBeforeVt2 && pBeforeVt2->next)
			{
				pBeforeVt1->render = pSiblingFace->pHead->render;
				pBeforeVt1->next   = pSiblingFace->pHead->next;
				pBeforeVt2->render = pVt2->render;
				pBeforeVt2->next   = pVt2->next;
				
				/// merge center line
				if(pCenterline && pSiblingFace->pCenterline)
				{
					for(ptr = pCenterline;ptr && ptr->next;ptr = ptr->next);
					if(NULL != ptr)
					{
						ptr->render = pSiblingFace->pCenterline->render;
						ptr->next   = pSiblingFace->pCenterline->next;
					}
				}

				ret = true;
			}
		}
	}

	return ret;
}

/**	\brief	The CHSRFace::IsPerpendicularToViewPoint function


	\return	bool	
*/
bool CHSRFace::IsPerpendicularToViewPoint()
{
	bool   ret = false;
	double vtx,vty,vtz;
	CHSRApp* pHiddenApp = CHSRApp::GetInstance();
	
	if(TOP == pHiddenApp->m_clsView.GetViewPoint())
	{
		vtx = 0.;
		vty = 0.;
		vtz = 1.;
	}
	else
	{
		vtx = 1.;
		vty = 0.;
		vtz = 0.;
	}

	CIsVect3d norm(Plane.a,Plane.b,Plane.c);
	const double dot = norm.DotProduct(CIsVect3d(vtx,vty,vtz));
	///double dot = theGeo.DotProduct(Plane.a,Plane.b,Plane.c,vtx,vty,vtz);
	if((dot <= (-1. + 0.001)) || (dot >= (1. - 0.001))) ret = true;

	return ret;
}

/**	\brief	The CHSRFace::IsParallelToViewPoint function


	\return	bool	
*/
bool CHSRFace::IsParallelToViewPoint()
{
	bool   ret = false;
	double vtx,vty,vtz;
	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	
	if(TOP == pHSRApp->m_clsView.GetViewPoint())
	{
		vtx = 0.;
		vty = 0.;
		vtz = 1.;
	}
	else
	{
		vtx = 1.;
		vty = 0.;
		vtz = 0.;
	}

	CIsVect3d norm(Plane.a,Plane.b,Plane.c);
	const double dot = norm.DotProduct(CIsVect3d(vtx,vty,vtz));
	///double dot = theGeo.DotProduct(Plane.a,Plane.b,Plane.c,vtx,vty,vtz);
	if((dot < 0.001) && (dot > -0.001)) ret = true;

	return ret;
}

/**	
	@brief	The CHSRFace::CreateVertexList function

	@author	humkyung

	@date	????.??.??

	@param	nCount	a parameter of type const int
	@param	ptEDGE[]	a parameter of type POINT_T
	@param	merge	서로 연결된 edge의 기울기가 같다면 하나의 edge로 처리할지 여부

	@return	PVERTEX	
*/
PHSR_VERTEX CHSRFace::CreateVertexList(const int& nCount , POINT_T ptEDGE[] , const bool& merge)
{
	assert((nCount > 0) && "nCount is less than 0");
	const static double _tol=0.000001;
	static POINT_T _pt[512]={0,};
	
	PHSR_VERTEX pRet=NULL;
	if(nCount > 0)
	{
		int nIndex=0;
		double dx[2]={0.},dy[2]={0.};
		double dz[2]={0.},l[2]={0.};
		
		dx[0] = ptEDGE[1].x - ptEDGE[0].x;
		dy[0] = ptEDGE[1].y - ptEDGE[0].y;
		dz[0] = ptEDGE[1].z - ptEDGE[0].z;
		l[0]  = sqrt(dx[0]*dx[0] + dy[0]*dy[0] + dz[0]*dz[0]);
		_pt[nIndex++]= ptEDGE[0];
			
		int i = 0;
		for(i = 1;(i + 1) < nCount;++i)
		{
			if(true == merge)
			{
				dx[1] = ptEDGE[i+1].x - ptEDGE[i].x;
				dy[1] = ptEDGE[i+1].y - ptEDGE[i].y;
				dz[1] = ptEDGE[i+1].z - ptEDGE[i].z;
				l[1]  = sqrt(dx[1]*dx[1] + dy[1]*dy[1] + dz[1]*dz[1]);
			
				//<--
				// check whether two points are on a line
				if(l[0] && l[1])
				{
					dx[0] /= l[0];
					dy[0] /= l[0];
					dz[0] /= l[0];
					dx[1] /= l[1];
					dy[1] /= l[1];
					dz[1] /= l[1];
					l[0]   = 1.;
					
					if(!(fabs(dx[1] - dx[0]) <= _tol) || !(fabs(dy[1] - dy[0]) < _tol) || !(fabs(dz[1] - dz[0]) < _tol))
					{
						_pt[nIndex++] = ptEDGE[i];
						dx[0] = dx[1];
						dy[0] = dy[1];
						dz[0] = dz[1];
					}
				}
				else
				{
					dx[0] = dx[1];
					dy[0] = dy[1];
					dz[0] = dz[1];
					l[0]  = l[1];
				}
			}
			else
			{
				_pt[nIndex++] = ptEDGE[i];
			}
		}		
		_pt[nIndex++] = ptEDGE[i];	/// let start and end point has the same value!!!

		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		PHSR_VERTEX  pvt=NULL,pTail=NULL;
		PPOINT_T ppt=NULL;
		for(i = 0;i < nIndex;i++)
		{
			if(pvt = pHSRApp->alloc_vertex())
			{
				pvt->render = INTERSECTION;
				if(pvt->pt  = pHSRApp->alloc_point())
				{
					pvt->pt->x = _pt[i].x;
					pvt->pt->y = _pt[i].y;
					pvt->pt->z = _pt[i].z;
					
					if(NULL == pRet)
						pRet  = ListPush(pRet,pvt);
					else	pTail = ListPush(pTail,pvt);
					pTail = pvt;
				}else	pHSRApp->free_vertex(pvt);//else free((void*)pvt);
			}
		}
	}

	return pRet;
}

/**	
	@brief	create a face and computate plane equation

	@author	humkyung

	@date	????-??-??

	@param	nCount	a parameter of type const int
	@param	ptEDGE[]	a parameter of type POINT_T
	@param	mergeEdge	서로 연결된 edge의 기울기가 같다면 하나의 edge로 처리할지 여부

	@return	CHSRFace*	
*/
CHSRFace* CHSRFace::CreateFace(const int nCount,POINT_T ptEDGE[] , const bool& mergeEdge)
{
	assert((nCount > 0) && "nCount is less than 0");
	CHSRFace* pRet = NULL;

	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	if((nCount > 0) && pHSRApp)
	{
		static PHSR_VERTEX  pVertex[512]={0,};

		CHSRFace* pFace=pHSRApp->new_hsr_face();
		if(pFace)
		{
			PHSR_VERTEX pvt=CHSRFace::CreateVertexList(nCount,ptEDGE , mergeEdge);
			
			if(pvt)
			{
				POINT_T _pt={0};
				int nIndex=0;
				for(PHSR_VERTEX ptr=pvt;ptr;ptr = ptr->next,nIndex++)
				{
					if(0 == nIndex)
					{
						pFace->volume.x[0] = ptr->pt->x;
						pFace->volume.x[1] = ptr->pt->x;
						pFace->volume.y[0] = ptr->pt->y;
						pFace->volume.y[1] = ptr->pt->y;
						pFace->volume.z[0] = ptr->pt->z;
						pFace->volume.z[1] = ptr->pt->z;
					}
					else
					{
						if(pFace->volume.x[0] > ptr->pt->x) pFace->volume.x[0] = ptr->pt->x;
						if(pFace->volume.x[1] < ptr->pt->x) pFace->volume.x[1] = ptr->pt->x;
						if(pFace->volume.y[0] > ptr->pt->y) pFace->volume.y[0] = ptr->pt->y;
						if(pFace->volume.y[1] < ptr->pt->y) pFace->volume.y[1] = ptr->pt->y;
						if(pFace->volume.z[0] > ptr->pt->z) pFace->volume.z[0] = ptr->pt->z;
						if(pFace->volume.z[1] < ptr->pt->z) pFace->volume.z[1] = ptr->pt->z;
					}
					_pt.x = ptr->pt->x;
					_pt.y = ptr->pt->y;
					_pt.z = ptr->pt->z;
				}
				
				if(nIndex >= 3)
				{
					if(     (pvt->pt->x != _pt.x) || 
						(pvt->pt->y != _pt.y) ||
						(pvt->pt->z != _pt.z))
					{
						
						PHSR_VERTEX pEnd = NULL;
						if(pEnd = pHSRApp->alloc_vertex())
						{
							pEnd->render= INTERSECTION;
							pEnd->pt    = pvt->pt;
							pvt         = ListPush(pvt,pEnd);
							nIndex++;
						}
						else
						{
							pHSRApp->FreeFace(pFace);
							
							return NULL;
						}
					}
					pFace->nCount = nIndex;
					pFace->pHead  = pvt;

					if(pFace->CalcPlaneEquation())
					{
						if(pHSRApp->IsParallelPlane(pFace))
						{
							pHSRApp->FreeFace(pFace);
							
							return NULL;
						}
						pRet = ListPush(pRet,pFace);
					}
					else
					{
						pHSRApp->FreeFace(pFace);
							
						return NULL;
					}
				}
				else
				{
					pHSRApp->FreeFace(pFace);
				}
			}
		}
		else
		{
			OutputDebugString(_T("Can't Allocate Memory!!!"));
		}
	}

	return pRet;
}

/**
	@brief	circle 형상을 이루는 점들을 구한다.

	@author	humkyung

	@date	2009.07.23
*/
int CHSRFace::CreateCirclePts(POINT_T pts[] , const POINT_T& ptOrigin,const double& nRadius,const VECTOR_T& vecNorm,const int& nSegments)
{
	double alpha=0.,beta=0;
	CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
	const double nStep = 2*PI/nSegments;
	
	int i=0;
	for(double t=0;t < 2*PI;t += nStep,++i)
	{
		pts[i].x = nRadius*cos(t);
		pts[i].y = nRadius*sin(t);
		pts[i].z = 0.;
		pts[i]   = CGeometry::RotateAboutYXAxis(pts[i],-beta,-alpha);
		pts[i].x = ptOrigin.x + pts[i].x;
		pts[i].y = ptOrigin.y + pts[i].y;
		pts[i].z = ptOrigin.z + pts[i].z;
	}
	pts[i].x = nRadius*cos(2*PI);
	pts[i].y = nRadius*sin(2*PI);
	pts[i].z = 0.;
	pts[i]   = CGeometry::RotateAboutYXAxis(pts[i],-beta,-alpha);
	pts[i].x = ptOrigin.x + pts[i].x;
	pts[i].y = ptOrigin.y + pts[i].y;
	pts[i].z = ptOrigin.z + pts[i].z;

	return ERROR_SUCCESS;
}

/**	
	@brief	The CHSRFace::CreateCircleFace function

	@author	humkyung

	@param	ptOrigin	a parameter of type POINT_T
	@param	nRadius	a parameter of type double
	@param	vecNorm	a parameter of type VECTOR_T
	@param	nSegments	a parameter of type int

	@return	CHSRFace*	
*/
CHSRFace* CHSRFace::CreateCircleFace(POINT_T ptOrigin,double nRadius,VECTOR_T vecNorm,const int& nSegments)
{
	CHSRFace* pRet=NULL;

	double alpha=0.,beta=0;
	CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
	double nStep = 2*PI/nSegments;
	
	static POINT_T ptEDGE[64];
	int i=0;
	for(double t=0;t < 2*PI;t += nStep,i++)
	{
		ptEDGE[i].x = nRadius*cos(t);
		ptEDGE[i].y = nRadius*sin(t);
		ptEDGE[i].z = 0.;
		ptEDGE[i]   = CGeometry::RotateAboutYXAxis(ptEDGE[i],-beta,-alpha);
		ptEDGE[i].x = ptOrigin.x + ptEDGE[i].x;
		ptEDGE[i].y = ptOrigin.y + ptEDGE[i].y;
		ptEDGE[i].z = ptOrigin.z + ptEDGE[i].z;
	}
	ptEDGE[i].x = nRadius*cos(2*PI);
	ptEDGE[i].y = nRadius*sin(2*PI);
	ptEDGE[i].z = 0.;
	ptEDGE[i]   = CGeometry::RotateAboutYXAxis(ptEDGE[i],-beta,-alpha);
	ptEDGE[i].x = ptOrigin.x + ptEDGE[i].x;
	ptEDGE[i].y = ptOrigin.y + ptEDGE[i].y;
	ptEDGE[i].z = ptOrigin.z + ptEDGE[i].z;
	
	pRet = CHSRFace::CreateFace(i+1,ptEDGE);

	return pRet;
}

/**	\brief	The CHSRFace::CreateLine function

	\param	nCount	a parameter of type const int
	\param	ptEDGE[]	a parameter of type POINT_T

	\return	PVERTEX	
*/
PHSR_VERTEX CHSRFace::CreateLine(const int nCount,POINT_T ptEDGE[])
{
	assert(ptEDGE && "ptEDGE is NULL");
	PHSR_VERTEX pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(ptEDGE)
	{
		PHSR_VERTEX  pvt[64] = {NULL,};
		PPOINT_T ppt[64] = {NULL,};

		for(int i=0;i < nCount;i++)
		{
			pvt[i] = pHSRApp->alloc_vertex();
			ppt[i] = pHSRApp->alloc_point();

			if(pvt[i] && ppt[i])
			{
				ppt[i]->x = ptEDGE[i].x;
				ppt[i]->y = ptEDGE[i].y;
				ppt[i]->z = ptEDGE[i].z;
				pvt[i]->pt= ppt[i];
				pRet = ListPush(pRet,pvt[i]);
			}
			else
			{
				//if(pvt[i]) free((void*)pvt[i]);
				//if(ppt[i]) free((void*)ppt[i]);
			}
		}
	}

	return pRet;
}

PHSR_VERTEX CHSRFace::HasEdge(const POINT_T &pt1, const POINT_T &pt2)
{
	static const double tol = 0.001;

	for(PHSR_VERTEX ptr = pHead;ptr && ptr->next;ptr = ptr->next)
	{
		if(INVALID == ptr->render) continue;
		
		POINT_T temp1 = *(ptr->pt);
		POINT_T temp2 = *(ptr->next->pt);

		double dx1 = fabs(temp1.x - pt1.x);
		double dy1 = fabs(temp1.y - pt1.y);
		double dz1 = fabs(temp1.z - pt1.z);
		double dx2 = fabs(temp2.x - pt2.x);
		double dy2 = fabs(temp2.y - pt2.y);
		double dz2 = fabs(temp2.z - pt2.z);

		if((dx1 < tol) && (dy1 < tol) && (dz1 < tol) && 
		   (dx2 < tol) && (dy2 < tol) && (dz2 < tol))
		{
			return ptr;
		}

		dx1 = fabs(temp1.x - pt2.x);
		dy1 = fabs(temp1.y - pt2.y);
		dz1 = fabs(temp1.z - pt2.z);
		dx2 = fabs(temp2.x - pt1.x);
		dy2 = fabs(temp2.y - pt1.y);
		dz2 = fabs(temp2.z - pt1.z);

		if((dx1 < tol) && (dy1 < tol) && (dz1 < tol) && 
		   (dx2 < tol) && (dy2 < tol) && (dz2 < tol))
		{
			return ptr;
		}
	}

	return NULL;
}

/**
	@brief	return color of face
	@author	humkyung
	@date	2012.02.08
*/
string CHSRFace::color() const
{
	return m_sColor;
}

/**
	@brief	set color of face
	@author	humkyung
	@date	2012.02.08
*/
int CHSRFace::SetColor(const string& sColor)
{
	m_sColor = sColor;
	return ERROR_SUCCESS;
}

/**
	@brief	get range of face

	@author	humkyung

	@date	2012.10.18
*/
CIsVolume CHSRFace::GetRange() const
{
	CIsVolume vol(volume.x[0] , volume.y[0] , 0.0 , volume.x[1] , volume.y[1] , 0.0);
	return vol;
}