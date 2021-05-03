// ViewBoundary.cpp: implementation of the CViewBoundary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "SmartHSR.h"
#include "ViewBoundary.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CViewBoundary::CViewBoundary()
{
	m_nViewBoundaryEdge = 0;
}

/**	\brief
*/
CViewBoundary::~CViewBoundary()
{

}

/**	\brief	The += function

	\param	pViewFace	a parameter of type PFACE

	\return	bool CViewBoundary::operator	
*/
bool CViewBoundary::operator +=(CHSRFace* pViewFace)
{
	assert(pViewFace && "pViewFace is NULL");
	bool bRet=false;

	if(pViewFace)
	{
		m_lstViewBoundary.push_back(pViewFace);
		bRet = true;
	}

	return bRet;
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
bool CViewBoundary::IntersectLine(double* x,double* y,double x1,double y1,double x2,double y2,double x3,double y3,double x4,double y4,double tol){
	double Ax,Bx,Cx,Ay,By,Cy,d,e,f;
	double x1lo,x1hi,y1lo,y1hi;

	Ax = x2 - x1;
	Bx = x3 - x4;
	// X bound box test
	if(Ax < 0){
		x1lo=x2; x1hi=x1;
	} else {
		x1hi=x2; x1lo=x1;
	}
	if(Bx > 0){
		if(x1hi < x4 || x3 < x1lo) return false;
	} else {
		if(x1hi < x3 || x4 < x1lo) return false;
	}

	Ay = y2 - y1;
	By = y3 - y4;
	/* Y bound box test*/
	if(Ay < 0) {
		y1lo=y2; y1hi=y1;
	} else {
		y1hi=y2; y1lo=y1;
	}

	if(By > 0) {
		if(y1hi < y4 || y3 < y1lo) return false;
	} else {
		if(y1hi < y3 || y4 < y1lo) return false;
	}

	Cx = x1-x3;
	Cy = y1-y3;
	d = By*Cx - Bx*Cy;					/* alpha numerator*/
	f = Ay*Bx - Ax*By;					/* both denominator*/
	
	if(f > 0.) {						/* alpha tests*/
		if((d < (0 + tol)) || (d > (f - tol))) return false;
	} else {
		if((d > (0 - tol)) || (d < (f + tol))) return false;
	}

	e = Ax*Cy - Ay*Cx;					/* beta numerator*/
	if(f > 0.) {						/* beta tests*/
		if((e < (0 + tol)) || (e > (f - tol))) return false;
	} else {
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
	\param	pvt1	a parameter of type PVERTEX
	\param	pvt2	a parameter of type PVERTEX

	\return	bool	
*/
bool CViewBoundary::IsInnerPoint(double x,double y,PHSR_VERTEX pvt1,PHSR_VERTEX pvt2){
	assert(pvt1 && "pvt1 is NULL");
	assert(pvt2 && "pvt2 is NULL");
	bool bRet=false;

	if(pvt1 && pvt2){
		CVect vec[2];

		vec[0].dx = pvt1->pt->x - x;
		vec[0].dy = pvt1->pt->y - y;
		vec[0].dz = 0.;
		vec[1].dx = pvt2->pt->x - x;
		vec[1].dy = pvt2->pt->y - y;
		vec[1].dz = 0.;

		double nDot=CMath::DotProduct(vec[0],vec[1]);
		if(nDot < 0.) bRet = true;
	}

	return bRet;
}

/**	\brief	The += function

	\param	pView	a parameter of type CHSRViewFormat*

	\return	bool CViewBoundary::operator	
*/
bool CViewBoundary::operator +=(CHSRViewFormat* pView){
	assert(pView && "pView is NULL");
	bool bRet=false;

	if(pView){
		m_lstView.push_back(pView);
		bRet = true;
	}

	return bRet;
}

/**	\brief	The CViewBoundary::IsHiddenLine function\n
	test point is mid-point of between vt1 and vt2.

	\param	vt1	a parameter of type PVERTEX
	\param	vt2	a parameter of type PVERTEX
	\param	pFace	a parameter of type CHSRFace*

	\remarks\n
	;2003.04.01 - check two line is eual? if so, set render INVALID.

	\return	bool	
*/
bool CViewBoundary::IsHiddenLine(PHSR_VERTEX vt1,PHSR_VERTEX vt2,CHSRFace* pFace){
	assert(vt1   && "vt1 is NULL");
	assert(vt2   && "vt2 is NULL");
	assert(pFace && "pFace is NULL");

	if(vt1 && vt2 && pFace){
		LINE_T line[2]={0.,};
		double dx[2]={0.,},dy[2]={0.,};

		line[0].ptStart.x = vt1->pt->x;
		line[0].ptStart.y = vt1->pt->y;
		line[0].ptStart.z = 0;
		line[0].ptEnd.x   = vt2->pt->x;
		line[0].ptEnd.y   = vt2->pt->y;
		line[0].ptEnd.z   = 0;
		for(PHSR_VERTEX ptr=pFace->pHead;ptr && ptr->next;ptr = ptr->next){
			if(INVALID != ptr->render){
				if(IsEqualLine(line[0],line[1])){
					vt1->render = INVALID;
					ptr->render = INVALID;
					return true;
				}
			}
		}

		POINT_T pt={0.};
		pt.x = (vt1->pt->x + vt2->pt->x)*0.5;
		pt.y = (vt1->pt->y + vt2->pt->y)*0.5;
		
		if(IsInnerVertex(&pt,pFace)) return true;
	}

	return false;
}

/**	\brief	The CViewBoundary::IsInnerVertex function

	\param	pt	a parameter of type PPOINT_T
	\param	pFace	a parameter of type CHSRFace*

	\return	bool	
*/
bool CViewBoundary::IsInnerVertex(PPOINT_T pt,CHSRFace* pFace){
	assert(pt    && "pt is NULL");
	assert(pFace && "pFace is NULL");

	static  POINT_T P[512]={0.,};
	int     n=0;   // point index; i1 = i-1 mod n 
	double  x=0;          // x intersection of e with ray 
	int     Rcross = 0; // number of right edge/ray crossings
	int     Lcross = 0; // number of left edge/ray crossings

	if(!pt || !pFace) return false;
	// Shift so that q is the origin. Note this destroys the polygon.
	// This is done for pedogical clarity.
	int i=0;
	for(PHSR_VERTEX pvt = pFace->pHead;pvt;pvt = pvt->next,i++){
		if(pvt->next){
			VECTOR_T vec[2]={0};
			
			vec[0].dx = pvt->pt->x - pt->x;
			vec[0].dy = pvt->pt->y - pt->y;
			vec[0].dz = pvt->pt->z - pt->z;
			vec[1].dx = pvt->next->pt->x - pt->x;
			vec[1].dy = pvt->next->pt->y - pt->y;
			vec[1].dz = pvt->next->pt->z - pt->z;
			if(vec[0].dx*vec[1].dy == vec[0].dy*vec[1].dx) return false;
		}

		if((pvt->pt->x == pt->x) && (pvt->pt->y == pt->y)) return false;
		P[i].x = (pvt->pt->x) - (pt->x);
		P[i].y = (pvt->pt->y) - (pt->y);
	}
	n = i;

	int i1=0;
	// For each edge e=(i-1,i), see if crosses ray.
	for(i = 0;i < n;i++) {
		//* First see if q=(0,0) is a vertex.
		//if ((P[i].x == 0.0) &&  (P[i].y == 0.0) && coplane) return false;
		i1 = ( i + n - 1 ) % n;

		//* if e "straddles" the x-axis...
		//* The commented-out statement is logically equivalent to the one 
		//* following.
		//* if( ( ( P[i][Y] > 0 ) && ( P[i1][Y] <= 0 ) ) ||
		//* ( ( P[i1][Y] > 0 ) && ( P[i][Y] <= 0 ) ) ) {   
		if( ( P[i].y  > 0. ) != ( P[i1].y > 0. ) ) {
			//* e straddles ray, so compute intersection with ray.
			x = (P[i].x * (double)P[i1].y - P[i1].x * (double)P[i].y)/(double)(P[i1].y - P[i].y);

			//* crosses ray if strictly positive intersection.
			if (x >= (0 - 0.0)) Rcross++;
		}
    
		//* if e straddles the x-axis when reversed... */
		//* if( ( ( P[i] [Y] < 0 ) && ( P[i1][Y] >= 0 ) ) ||
		//* ( ( P[i1][Y] < 0 ) && ( P[i] [Y] >= 0 ) ) )  {
		if ( ( P[i].y < 0 ) != ( P[i1].y < 0 ) ) { 
			//* e straddles ray, so compute intersection with ray.
			x = (P[i].x * P[i1].y - P[i1].x * P[i].y)/(double)(P[i1].y - P[i].y);

			//* crosses ray if strictly positive intersection.
			if (x <= (0 + 0.0)) Lcross++;
		}
	}
  
	//* q on the edge if left and right cross are not the same parity.
	if((Rcross%2 ) != (Lcross%2)) return true;

	//* q inside iff an odd number of crossings.
	if((Rcross % 2) == 1) return true;
	else		      return false;
}

/**	\brief	The CViewBoundary::ComputateViewBoundary function


	\return	void	
*/
void CViewBoundary::ComputateViewBoundary(){
	for(list<CHSRViewFormat*>::iterator itr=m_lstView.begin();itr != m_lstView.end();++itr){
		CHSRFace* pFace = (*itr)->GetViewFace();
		list<CHSRViewFormat*>::iterator itrNext=itr;
		for(itrNext++;itrNext != m_lstView.end();++itrNext)
		{
			//! ignore inside view.
			if((itr != itrNext) && (!(*itr)->IsIn(*itrNext) && !(*itrNext)->IsIn(*itr)))
			{
				CHSRFace* pFaceNext= (*itrNext)->GetViewFace();
				
				Split(pFace,pFaceNext);
				for(PHSR_VERTEX ptr = pFaceNext->pHead;ptr && ptr->next;ptr = ptr->next)
				{
					if(IsHiddenLine(ptr,ptr->next,pFace))
						ptr->render = INVALID;
				}
				for(PHSR_VERTEX ptr = pFace->pHead;ptr && ptr->next;ptr = ptr->next)
				{
					if(IsHiddenLine(ptr,ptr->next,pFaceNext))
						ptr->render = INVALID;
				}
			}
		}
		if(pFace) m_lstViewBoundary.push_back(pFace);
	}

	for(list<CHSRFace*>::iterator itrFace=m_lstViewBoundary.begin();itrFace != m_lstViewBoundary.end();++itrFace){
		for(PHSR_VERTEX ptr = (*itrFace)->pHead;ptr && ptr->next;ptr = ptr->next){
			if(INVALID != ptr->render){
				m_aryViewBoundary[m_nViewBoundaryEdge].ptStart.x = ptr->pt->x;
				m_aryViewBoundary[m_nViewBoundaryEdge].ptStart.y = ptr->pt->y;
				m_aryViewBoundary[m_nViewBoundaryEdge].ptStart.z = ptr->pt->z;
				m_aryViewBoundary[m_nViewBoundaryEdge].ptEnd.x = ptr->next->pt->x;
				m_aryViewBoundary[m_nViewBoundaryEdge].ptEnd.y = ptr->next->pt->y;
				m_aryViewBoundary[m_nViewBoundaryEdge].ptEnd.z = ptr->next->pt->z;
				m_nViewBoundaryEdge++;
			}
		}
	}

	RemoveSameViewBoundary();
	JoinSeparateViewLine();
}

/**	\brief	The CViewBoundary::Split function\n
	split face lines by pFace\n
	;1. if face type is PIPE,PIPEX,PIPE_BEND,TEE,TEEX,ELBOW,SW_ELBOW and current line mode is SINGLE_LINE or BOTH and radius is less than user option,\n
	;split face's center line by pFace.\n
	2. not under upper condition, split head and centerline by pFace.

	\param	pViewBoundary1	a parameter of type PFACE
	\param	pViewBoundary2	a parameter of type PFACE

	\return	bool	
*/
bool CViewBoundary::Split(CHSRFace* pViewBoundary1,CHSRFace* pViewBoundary2){
	assert(pViewBoundary1 && "pViewBoundary1 is NULL");
	assert(pViewBoundary2 && "pViewBoundary2 is NULL");
	bool bRet = false;

	if(pViewBoundary1 && pViewBoundary2){
		for(PHSR_VERTEX ptr = pViewBoundary2->pHead;ptr && ptr->next;){
			if(SHOW == ptr->render)
				ptr = SplitViewSegment(pViewBoundary1,ptr,ptr->next);
			else	ptr = ptr->next;
		}
		bRet = true;
	}
	
	return bRet;
}

/**	\brief	The CViewBoundary::SplitViewSegment function

	\param	pViewBoundary	a parameter of type PFACE
	\param	vt1	a parameter of type PVERTEX
	\param	vt2	a parameter of type PVERTEX

	\remarks\n
	;2004.12.14 - add for loop to split view line.
	
	\return	PVERTEX	
*/
PHSR_VERTEX CViewBoundary::SplitViewSegment(CHSRFace* pViewBoundary,PHSR_VERTEX vt1,PHSR_VERTEX vt2){
	assert(pViewBoundary && "pViewBoundary is NULL");
	assert(vt1 && "vt1 is NULL");
	assert(vt2 && "vt2 is NULL");
	double x=0.,y=0.,z=0.;
	bool   bRet[2]={false};

	if(pViewBoundary && vt1 && vt2){
		CHSRApp* pHSRApp=CHSRApp::GetInstance();
		const static double nTol=0.;

		CVect vec;
		vec.dx = vt2->pt->x - vt1->pt->x;
		vec.dy = vt2->pt->y - vt1->pt->y;
		vec.dz = 0.;
		vec.Normalize();
		
		LINE_T ln[2]={0.,};
		PHSR_VERTEX ptr_next=NULL;
		for(PHSR_VERTEX ptr = pViewBoundary->pHead;(ptr && ptr->next);ptr = ptr_next){
			ptr_next = ptr->next;

			vec.dx = ptr->next->pt->x - ptr->pt->x;
			vec.dy = ptr->next->pt->y - ptr->pt->y;
			vec.dz = 0.;
			vec.Normalize();
			
			ln[1].ptStart.x = ptr->pt->x - vec.dx*nTol;
			ln[1].ptStart.y = ptr->pt->y - vec.dy*nTol;
			ln[1].ptStart.z = 0.;
			ln[1].ptEnd.x = ptr->next->pt->x + vec.dx*nTol;
			ln[1].ptEnd.y = ptr->next->pt->y + vec.dy*nTol;
			ln[1].ptEnd.z = 0.;
			for(PHSR_VERTEX vtr=vt1;vtr != vt2;vtr = vtr->next){
				vec.dx = vtr->next->pt->x - vtr->pt->x;
				vec.dy = vtr->next->pt->y - vtr->pt->y;
				vec.dz = 0.;
				vec.Normalize();
				
				ln[0].ptStart.x = vtr->pt->x - vec.dx*nTol;
				ln[0].ptStart.y = vtr->pt->y - vec.dy*nTol;
				ln[0].ptStart.z = 0.;
				ln[0].ptEnd.x = vtr->next->pt->x + vec.dx*nTol;
				ln[0].ptEnd.y = vtr->next->pt->y + vec.dy*nTol;
				ln[0].ptEnd.z = 0.;
				if(DO_INTERSECTION == CGeometry::IntersectLine2D(&x,&y,
					ln[0].ptStart.x,ln[0].ptStart.y,ln[0].ptEnd.x,ln[0].ptEnd.y,
					ln[1].ptStart.x,ln[1].ptStart.y,ln[1].ptEnd.x,ln[1].ptEnd.y,0.)){
					PHSR_VERTEX pReturn = vt2,pTmp = NULL,pSelf = NULL;
					if(!((ISEQUAL(x,vtr->pt->x) && ISEQUAL(y,vtr->pt->y)) || 
					     (ISEQUAL(x,vtr->next->pt->x) && ISEQUAL(y,vtr->next->pt->y)))){
						if(IsInnerPoint(x,y,vt1,vt2)){
							if(NULL != (pTmp = pHSRApp->alloc_vertex())){
								pTmp->pt = pHSRApp->alloc_point();
								if(pTmp->pt){
									pTmp->pt->x = x;
									pTmp->pt->y = y;	
									pTmp->pt->z = 0.;
									
									pTmp->next  = vtr->next;
									vtr->next   = pTmp;
								}
								
								bRet[0] = true;
							}
						}
					}
					
					if(!((ISEQUAL(x,ptr->pt->x) && ISEQUAL(y,ptr->pt->y)) || 
					     (ISEQUAL(x,ptr->next->pt->x) && ISEQUAL(y,ptr->next->pt->y)))){
						if(IsInnerPoint(x,y,ptr,ptr->next)){
							if(pTmp && (NULL != (pSelf = pHSRApp->alloc_vertex()))){
								pSelf->pt = pHSRApp->alloc_point();
								if(pSelf->pt){
									pSelf->pt->x = x;
									pSelf->pt->y = y;
									pSelf->pt->z = 0.;
									
									pSelf->next  = ptr->next;
									ptr->next    = pSelf;
									ptr_next = pSelf;
								}
								
								bRet[1] = true;
							}
						}
					}
					break;
					//return (bRet[0] || bRet[1]) ? vt1 : vt2;
				}
			}
		}
	}

	return vt2;
}

/**	\brief	The CViewBoundary::RemoveSameViewBoundary function


	\return	void	
*/
void CViewBoundary::RemoveSameViewBoundary(){
	for(int i=0;i < m_nViewBoundaryEdge;i++){
		for(int j=i+1;j < m_nViewBoundaryEdge;j++){
			if(IsEqualLine(m_aryViewBoundary[i],m_aryViewBoundary[j])){
				memcpy(&(m_aryViewBoundary[j]),&(m_aryViewBoundary[j + 1]),sizeof(LINE_T)*(m_nViewBoundaryEdge - j - 1));
				m_nViewBoundaryEdge--;
			}
		}
	}
}

/**	\brief	The CViewBoundary::JoinSeparateViewLine function


	\return	void	
*/
void CViewBoundary::JoinSeparateViewLine(){
	VECTOR_T vec[2]={0.,};

	for(int i=0;i < m_nViewBoundaryEdge;i++){
		vec[0].dx = m_aryViewBoundary[i].ptEnd.x - m_aryViewBoundary[i].ptStart.x;
		vec[0].dy = m_aryViewBoundary[i].ptEnd.y - m_aryViewBoundary[i].ptStart.y;
		vec[0].dz = 0.;
		CMath::NormalizeVector(vec[0]);
		for(int j=i+1;j < m_nViewBoundaryEdge;j++){
			vec[1].dx = m_aryViewBoundary[j].ptEnd.x - m_aryViewBoundary[j].ptStart.x;
			vec[1].dy = m_aryViewBoundary[j].ptEnd.y - m_aryViewBoundary[j].ptStart.y;
			vec[1].dz = 0.;
			CMath::NormalizeVector(vec[1]);

			if((fabs(vec[1].dx - vec[0].dx) < 0.001) && (fabs(vec[1].dy - vec[0].dy) < 0.001)){
				if(IsSamePoint(m_aryViewBoundary[i].ptEnd,m_aryViewBoundary[j].ptStart)){
					m_aryViewBoundary[i].ptEnd = m_aryViewBoundary[j].ptEnd;
					memcpy(&(m_aryViewBoundary[j]),&(m_aryViewBoundary[j + 1]),sizeof(LINE_T)*(m_nViewBoundaryEdge - j - 1));
					m_nViewBoundaryEdge--;
				}else if(IsSamePoint(m_aryViewBoundary[i].ptStart,m_aryViewBoundary[j].ptEnd)){
					m_aryViewBoundary[i].ptStart = m_aryViewBoundary[j].ptStart;
					memcpy(&(m_aryViewBoundary[j]),&(m_aryViewBoundary[j + 1]),sizeof(LINE_T)*(m_nViewBoundaryEdge - j - 1));
					m_nViewBoundaryEdge--;
				}
			}
		}
	}
}

/**	\brief	The CViewBoundary::IsSamePoint function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	bool	
*/
bool CViewBoundary::IsSamePoint(const POINT_T& pt1,const POINT_T& pt2){
	double dx=0.,dy=0.,dz=0.;

	dx = pt2.x - pt1.x;
	dy = pt2.y - pt1.y;
	dz = pt2.z - pt1.z;
	if((fabs(dx) < 0.001) && (fabs(dy) < 0.001) && (fabs(dz) < 0.001))
		return true;

	return false;
}

/**	\brief	The CViewBoundary::IsEqualLine function

	\param	line1	a parameter of type const LINE_T&
	\param	line2	a parameter of type const LINE_T&

	\return	bool	
*/
bool CViewBoundary::IsEqualLine(const LINE_T& line1,const LINE_T& line2){
	const static double nTol=0.001;

	double dx[2]={0.,},dy[2]={0.,};

	dx[0] = line2.ptStart.x - line1.ptStart.x;
	dy[0] = line2.ptStart.y - line1.ptStart.y;
	dx[1] = line2.ptEnd.x - line1.ptEnd.x;
	dy[1] = line2.ptEnd.y - line1.ptEnd.y;
	if((fabs(dx[0]) < nTol) && (fabs(dx[1]) < nTol) && (fabs(dy[0]) < 0.001) && (fabs(dy[1]) < nTol))
		return true;

	dx[0] = line2.ptEnd.x - line1.ptStart.x;
	dy[0] = line2.ptEnd.y - line1.ptStart.y;
	dx[1] = line2.ptStart.x - line1.ptEnd.x;
	dy[1] = line2.ptStart.y - line1.ptEnd.y;
	if((fabs(dx[0]) < nTol) && (fabs(dx[1]) < nTol) && (fabs(dy[0]) < 0.001) && (fabs(dy[1]) < nTol))
		return true;

	return false;
}