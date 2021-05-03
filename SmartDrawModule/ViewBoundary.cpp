// ViewBoundary.cpp: implementation of the CViewBoundary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <math.h>
#include <IsLine2d.h>
#include "ViewBoundary.h"

#define TOLER 0.00000001
#define ISEQUAL(a,b) ((fabs((double)(a)-(b)) >= (double) TOLER) ? 0 : 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CViewBoundaySegment::CViewBoundaySegment()
{
	pView= NULL;
	next = NULL;
	memset(szDim[0], '\0' , sizeof(char)*64);
	memset(szDim[1], '\0' , sizeof(char)*64);
	nHeight[0] = nHeight[1] = 0.0;
}
CViewBoundaySegment::~CViewBoundaySegment(){}


ADRAW_IMPLEMENT_FUNC(CViewBoundary , CaDraw_Entity , _T("CViewBoundary"))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CViewBoundary::CViewBoundary()
{
	m_pVBS = NULL;
}

/**	\brief
*/
CViewBoundary::~CViewBoundary()
{
	try
	{
		CViewBoundaySegment* ptrNext=NULL;
		for(CViewBoundaySegment* ptr=m_pVBS;ptr;ptr = ptrNext)
		{
			ptrNext = ptr->next;

			delete (ptr);
			ptr = NULL;
		}
	}
	catch(...)
	{
	}
}

//	parameter	:
//	description	: unique each of contents
//	remarks		: end of pSrc must be NULL.
//	returns		:
template<class T>
T* ListPush(T* pDest,T* pSrc)
{
	assert(pSrc && "pSrc is NULL");

	if(pSrc)
	{
		if(pDest)
		{
			T* ptr = pDest;
			for(ptr = pDest;ptr && ptr->next;ptr = ptr->next)
			{
				assert(ptr != pSrc);
			}
			ptr->next = pSrc;
			//pSrc->prev= ptr;
			//pSrc->next= NULL;

			return pDest;
		}
		else
		{
			pDest = pSrc;
			//pDest->prev = pDest->next = NULL;

			return pSrc;
		}
	}

	return pDest;
}
/**	\brief	The += function

	\param	pVBS	a parameter of type PVBS

	\return	bool CViewBoundary::operator	
*/
bool CViewBoundary::operator +=(CViewBoundaySegment* pVBS)
{
	assert(pVBS && "pVBS is NULL");
	bool bRet=false;

	if(pVBS)
	{
		m_pVBS = ListPush(m_pVBS,pVBS);
		bRet = true;
	}

	return bRet;
}

/**	
	@brief	split face lines by pFace\n

	\param	pViewBoundary	a parameter of type CViewBoundary*

	\return	bool	
*/
bool CViewBoundary::Split(CViewBoundary* pViewBoundary)
{
	assert(pViewBoundary && "pViewBoundary is invalid");
	bool bRet = false;

	if(pViewBoundary)
	{
		for(CViewBoundaySegment* ptr = m_pVBS;ptr && ptr->next;)
		{
			if(SHOW == ptr->render)
				ptr = pViewBoundary->SplitViewSegment(ptr,ptr->next);
			else	ptr = ptr->next;
		}
		bRet = true;
	}
	
	return bRet;
}

/**	
	\brief	The CViewBoundary::SplitViewSegment function

	\param	vt1	a parameter of type PVBS
	\param	vt2	a parameter of type PVBS

	\remarks\n
	;2004.12.15 - set szDim to '\0' when view boundary is intersected;
	;2004.12.17 - modify splitviewsegment code.

	\return	PVBS	
*/
CViewBoundaySegment* CViewBoundary::SplitViewSegment(CViewBoundaySegment* vt1,CViewBoundaySegment* vt2)
{
	assert(vt1 && "vt1 is NULL");
	assert(vt2 && "vt2 is NULL");
	double x=0.,y=0.,z=0.;
	bool   bRet[2]={false};

	if(vt1 && vt2)
	{
		const double nTol=0.f;
		CIsLine2d ln[2];

		CIsVect3d vec;
		vec.Set(vt2->pt.x() - vt1->pt.x() , vt2->pt.y() - vt1->pt.y() , 0.f);
		vec.Normalize();
		
		ln[0].Set(
			CIsPoint2d(vt1->pt.x() - vec.dx()*nTol , vt1->pt.y() - vec.dy()*nTol) , 
			CIsPoint2d(vt2->pt.x() + vec.dx()*nTol , vt2->pt.y() + vec.dy()*nTol));
		for(CViewBoundaySegment* ptr = m_pVBS;(ptr && ptr->next);ptr = ptr->next)
		{
			vec.Set(ptr->next->pt.x() - ptr->pt.x() , ptr->next->pt.y() - ptr->pt.y() , 0.f);
			vec.Normalize();

			ln[1].Set(
				CIsPoint2d(ptr->pt.x() - vec.dx()*nTol , ptr->pt.y() - vec.dy()*nTol),
				CIsPoint2d(ptr->next->pt.x() + vec.dx()*nTol , ptr->next->pt.y() + vec.dy()*nTol));

			for(CViewBoundaySegment* vtr=vt1;vtr != vt2;vtr = vtr->next)
			{
				vec.Set(vtr->next->pt.x() - vtr->pt.x() , vtr->next->pt.y() - vtr->pt.y() , 0.f);
				vec.Normalize();
				
				ln[0].Set(
					CIsPoint2d(vtr->pt.x() - vec.dx() , vtr->pt.y() - vec.dy()) , 
					CIsPoint2d(vtr->next->pt.x() + vec.dx() , vtr->next->pt.y() + vec.dy()));
				
				CIsPoint2d intsec;
				if(IntersectionResult::INTERSECT == ln[0].IntersectWith(intsec , ln[1]))
				{
					CViewBoundaySegment *pReturn = vt2,*pTmp = NULL,*pSelf = NULL;
					if(	!((ISEQUAL(intsec.x(),vtr->pt.x()) && ISEQUAL(intsec.y(),vtr->pt.y())) || (ISEQUAL(intsec.x(),vtr->next->pt.x()) && ISEQUAL(intsec.y(),vtr->next->pt.y()))))
					{
						if(IsInnerPoint(intsec.x() , intsec.y() ,vtr,vtr->next))
						{
							if(NULL != (pTmp = new CViewBoundaySegment))
							{
								memcpy(pTmp,vtr,sizeof(CViewBoundaySegment));
								vtr->szDim[1][0] = '\0';pTmp->szDim[0][0] = '\0';
								pTmp->pt.Set(intsec.x() , intsec.y() , 0.f);
								
								pTmp->next  = vtr->next;
								vtr->next   = pTmp;
								
								bRet[0] = true;
							}
						}
					}
					
					if(!((ISEQUAL(intsec.x(),ptr->pt.x()) && ISEQUAL(intsec.y(),ptr->pt.y())) || (ISEQUAL(intsec.x(),ptr->next->pt.x()) && ISEQUAL(intsec.y(),ptr->next->pt.y()))))
					{
						if(IsInnerPoint(intsec.x() , intsec.y() ,ptr,ptr->next))
						{
							if(pTmp && (NULL != (pSelf = new CViewBoundaySegment)))
							{
								memcpy(pSelf,ptr,sizeof(CViewBoundaySegment));
								ptr->szDim[1][0] = '\0';pSelf->szDim[0][0] = '\0';
								pSelf->pt.Set(intsec.x() , intsec.y() , 0.f);
								
								pSelf->next  = ptr->next;
								ptr->next    = pSelf;
								
								bRet[1] = true;
							}
						}
					}
					
					break;
				}
			}
		}
	}

	return vt2;
}

/**	\brief	The CViewBoundary::IntersectLine function

	\param	x	a parameter of type double*
	\param	y	a parameter of type double*
	\param	x1	a parameter of type double
	\param	y1	a parameter of type double
	\param	x2	a parameter of type double
	\param	y2	a parameter of type double
	\param	x3	a parameter of type double
	\param	y3	a parameter of type double
	\param	x4	a parameter of type double
	\param	y4	a parameter of type double
	\param	tol	a parameter of type double

	\return	bool	
*/
bool CViewBoundary::IntersectLine(double* x,double* y,double x1,double y1,double x2,double y2,double x3,double y3,double x4,double y4,double tol)
{
	double Ax,Bx,Cx,Ay,By,Cy,d,e,f;
	double x1lo,x1hi,y1lo,y1hi;

	Ax = x2 - x1;
	Bx = x3 - x4;
	// X bound box test
	if(Ax < 0)
	{
		x1lo=x2; x1hi=x1;
	}
	else
	{
		x1hi=x2; x1lo=x1;
	}
	if(Bx > 0)
	{
		if(x1hi < x4 || x3 < x1lo) return false;
	}
	else
	{
		if(x1hi < x3 || x4 < x1lo) return false;
	}

	Ay = y2 - y1;
	By = y3 - y4;
	/* Y bound box test*/
	if(Ay < 0)
	{
		y1lo=y2; y1hi=y1;
	}
	else
	{
		y1hi=y2; y1lo=y1;
	}

	if(By > 0)
	{
		if(y1hi < y4 || y3 < y1lo) return false;
	}
	else
	{
		if(y1hi < y3 || y4 < y1lo) return false;
	}

	Cx = x1-x3;
	Cy = y1-y3;
	d = By*Cx - Bx*Cy;					/* alpha numerator*/
	f = Ay*Bx - Ax*By;					/* both denominator*/
	
	if(f > 0.)
	{						/* alpha tests*/
		if((d < (0 + tol)) || (d > (f - tol))) return false;
	}
	else
	{
		if((d > (0 - tol)) || (d < (f + tol))) return false;
	}

	e = Ax*Cy - Ay*Cx;					/* beta numerator*/
	if(f > 0.)
	{/* beta tests*/
		if((e < (0 + tol)) || (e > (f - tol))) return false;
	}
	else
	{
		if((e > (0 - tol)) || (e < (f + tol))) return false;
	}

	// lines are colinear.
	if(f == 0.) return false;

	/*compute intersection coordinates*/
	double alpha = d/f;
	
	if(x1 == x2)
		*x = x1;
	else	*x = x1 + alpha*Ax;
	if(y1 == y2)
		*y = y1;
	else	*y = y1 + alpha*Ay;

	return true;
}

/**	\brief	The CViewBoundary::IsInnerPoint function

	\param	x	a parameter of type double
	\param	y	a parameter of type double
	\param	pvt1	a parameter of type PVBS
	\param	pvt2	a parameter of type PVBS

	\return	bool	
*/
bool CViewBoundary::IsInnerPoint(double x,double y,CViewBoundaySegment* pvt1,CViewBoundaySegment* pvt2)
{
	assert(pvt1 && "pvt1 is NULL");
	assert(pvt2 && "pvt2 is NULL");
	bool bRet=false;

	if(pvt1 && pvt2)
	{
		CIsVect3d vec[2];

		vec[0].Set(pvt1->pt.x() - x , pvt1->pt.y() - y , 0.f);
		vec[1].Set(pvt2->pt.x() - x , pvt2->pt.y() - y , 0.f);

		double nDot=vec[0].DotProduct(vec[1]);
		if(nDot < 0.) bRet = true;
	}

	return bRet;
}

/**	
	@brief	test point is mid-point of between vt1 and vt2. check if test point is inside of view.

	@author	HumKyung.BAEK

	@description
	2005.09.13 - adjust tolerance

	\param	vt1	a parameter of type PVBS
	\param	vt2	a parameter of type PVBS

	\return	bool	
*/
bool CViewBoundary::IsHiddenLine(CViewBoundaySegment* vt1,CViewBoundaySegment* vt2 , const double& nToler)
{
	assert(vt1 && "vt1 is NULL");
	assert(vt2 && "vt2 is NULL");
	CIsPoint3d pt;

	if(vt1 && vt2)
	{
		CIsLine2d line[2];
		double dx[2]={0.,},dy[2]={0.,};
		
		line[0].Set(
			CIsPoint2d(vt1->pt.x() , vt1->pt.y()) , 
			CIsPoint2d(vt2->pt.x() , vt2->pt.y()));
		for(CViewBoundaySegment* ptr=m_pVBS;ptr && ptr->next;ptr = ptr->next)
		{
			if(HIDE != ptr->render)
			{
				line[1].Set(
					CIsPoint2d(ptr->pt.x() , ptr->pt.y()),
					CIsPoint2d(ptr->next->pt.x() , ptr->next->pt.y()));
				
				dx[0] = line[1].end().x() - line[0].start().x();
				dy[0] = line[1].end().y() - line[0].start().y();
				dx[1] = line[1].start().x() - line[0].end().x();
				dy[1] = line[1].start().y() - line[0].end().y();
				
				if((fabs(dx[0]) < nToler) && (fabs(dy[0]) < nToler) &&
					(fabs(dx[1]) < nToler) && (fabs(dy[1]) < nToler))
				{
					vt1->render = HIDE;
					ptr->render = HIDE;
					return true;
				}
			}
		}
		
		pt.Set((vt1->pt.x() + vt2->pt.x())*0.5 , (vt1->pt.y() + vt2->pt.y())*0.5 , 0.f);
		
		if(IsInnerVertex(&pt)) return true;
	}

	return false;
}

/**	\brief	The CViewBoundary::IsInnerVertex function

	\param	pt	a parameter of type PPOINT_T

	\return	bool	
*/
bool CViewBoundary::IsInnerVertex(CIsPoint3d* pt)
{
	assert(pt && "pt is NULL");
	#define TOLERANCE 0.0

	static  CIsPoint3d P[512];
	int     n=0; // point index; i1 = i-1 mod n 
	double  x=0; // x intersection of e with ray 
	int     Rcross = 0; // number of right edge/ray crossings
	int     Lcross = 0; // number of left edge/ray crossings

	if(!pt) return false;
	// Shift so that q is the origin. Note this destroys the polygon.
	// This is done for pedogical clarity.
	int i=0;
	for(CViewBoundaySegment* pVt = m_pVBS;pVt;pVt = pVt->next,++i)
	{
		if(pVt->next)
		{
			CIsVect3d vec[2];
			
			vec[0].Set(pVt->pt.x() - pt->x() , pVt->pt.y() - pt->y() , pVt->pt.z() - pt->z());
			vec[1].Set(pVt->next->pt.x() - pt->x() , pVt->next->pt.y() - pt->y() , pVt->next->pt.z() - pt->z());
			if(vec[0].dx()*vec[1].dy() == vec[0].dy()*vec[1].dx()) return false;
		}

		if((pVt->pt.x() == pt->x()) && (pVt->pt.y() == pt->y())) return false;
		P[i].Set((pVt->pt.x()) - (pt->x()) , (pVt->pt.y()) - (pt->y()) , 0.f);
	}
	n = i;

	int i1=0;
	// For each edge e=(i-1,i), see if crosses ray.
	for(i = 0;i < n;++i) 
	{
		//* First see if q=(0,0) is a vertex.
		//if ((P[i].x == 0.0) &&  (P[i].y == 0.0) && coplane) return false;
		i1 = ( i + n - 1 ) % n;

		//* if e "straddles" the x-axis...
		//* The commented-out statement is logically equivalent to the one 
		//* following.
		//* if( ( ( P[i][Y] > 0 ) && ( P[i1][Y] <= 0 ) ) ||
		//* ( ( P[i1][Y] > 0 ) && ( P[i][Y] <= 0 ) ) ) {   
		if( ( P[i].y()  > 0. ) != ( P[i1].y() > 0. ) ) 
		{
			//* e straddles ray, so compute intersection with ray.
			x = (P[i].x() * (double)P[i1].y() - P[i1].x() * (double)P[i].y())/(double)(P[i1].y() - P[i].y());

			//* crosses ray if strictly positive intersection.
			if (x >= (0 - TOLERANCE)) Rcross++;
		}
    
		//* if e straddles the x-axis when reversed... */
		//* if( ( ( P[i] [Y] < 0 ) && ( P[i1][Y] >= 0 ) ) ||
		//* ( ( P[i1][Y] < 0 ) && ( P[i] [Y] >= 0 ) ) )  {
		if ( ( P[i].y() < 0 ) != ( P[i1].y() < 0 ) ) 
		{ 
			//* e straddles ray, so compute intersection with ray.
			x = (P[i].x() * P[i1].y() - P[i1].x() * P[i].y())/(double)(P[i1].y() - P[i].y());

			//* crosses ray if strictly positive intersection.
			if (x <= (0 + TOLERANCE)) Lcross++;
		}
	}
  
	//* q on the edge if left and right cross are not the same parity.
	if((Rcross%2 ) != (Lcross%2)) return true;

	//* q inside iff an odd number of crossings.
	if((Rcross % 2) == 1) return true;
	else		      return false;
}

/**
	@brief	determine this is enable by checking filters
	@author	humkyung
	@date	2016.03.19
*/
bool CViewBoundary::IsEnable(AnnoFilters* pAnnoFilters) const
{
	return true;
}
