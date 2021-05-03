#include <assert.h>
#include <string.h>
#include <math.h>
#include <math\itkmath.h>
#include <graphics\ITKGeo.h>

/**	\brief	The ITKGeo::GetRotatedAngleInXYPlane function

	\param	pt	a parameter of type POINT_T

	\return	double	
*/
double ITKGeo::GetRotatedAngleInXYPlane(POINT_T pt){
    	double l = sqrt(pt.x*pt.x + pt.y*pt.y);
	double theta=0.;

	if(l > 0){
	    	theta=acos(pt.x/l);

		if((pt.x >= 0.) && (pt.y <= 0.)){
		    	theta = 2*PI - theta;
		}else if((pt.x <= 0.) && (pt.y <= 0.)){
		    	theta = 2*PI - theta;
		}
	}

	return theta;
}

/**	\brief	The ITKGeo::IsOverlapRect function

	\param	pRect1	a parameter of type PRECT_T
	\param	pRect2	a parameter of type PRECT_T

	\return	bool	
*/
bool ITKGeo::IsOverlapRect(PRECT_T pRect1,PRECT_T pRect2){
	bool ret = true;

	if((pRect1->x > (pRect2->x + pRect2->width)) || ((pRect1->x + pRect1->width) < pRect2->x) || 
		(pRect1->y > (pRect2->y + pRect2->height)) || ((pRect1->y + pRect1->height) < pRect2->y))
		ret = false;

	return ret;

}

/**	\brief	The ITKGeo::IsPointInRect function

	\param	point	a parameter of type const POINT_T&
	\param	rect	a parameter of type const RECT_T&

	\return	bool	
*/
bool ITKGeo::IsPointInRect(const POINT_T& point,const RECT_T& rect){
	bool nRet = false;

	if((point.x >= rect.x) && (point.x <= rect.x + rect.width) &&
	   (point.y >= rect.y) && (point.y <= rect.y + rect.height)) 
	   nRet = true;

	return nRet;
}

/**	\brief	The ITKGeo::IntersectLineToVolume function

	\param	pLine	a parameter of type PLINE_T
	\param	pVolume	a parameter of type PVOLUME_T
	\param	pRet	a parameter of type PPOINT_T

	\return	bool	
*/
bool ITKGeo::IntersectLineToVolume(PLINE_T pLine,PVOLUME_T pVolume,PPOINT_T pRet){
	assert(pVolume && "pVolume is invalid-[IntersectToVolume]");
	bool ret=false;

	if(pVolume){
		// get volume of line.
		SQVOLUME vol={0};
		vol.x[LOW] = vol.x[HIGH] = pLine->ptStart.x;
		vol.y[LOW] = vol.y[HIGH] = pLine->ptStart.y;
		vol.z[LOW] = vol.z[HIGH] = pLine->ptStart.z;
		if(vol.x[LOW] > pLine->ptEnd.x) vol.x[LOW] = pLine->ptEnd.x;
		if(vol.x[HIGH]< pLine->ptEnd.x) vol.x[HIGH]= pLine->ptEnd.x;
		if(vol.y[LOW] > pLine->ptEnd.y) vol.y[LOW] = pLine->ptEnd.y;
		if(vol.y[HIGH]< pLine->ptEnd.y) vol.y[HIGH]= pLine->ptEnd.y;
		if(vol.z[LOW] > pLine->ptEnd.z) vol.z[LOW] = pLine->ptEnd.z;
		if(vol.z[HIGH]< pLine->ptEnd.z) vol.z[HIGH]= pLine->ptEnd.z;
		
		if((vol.x[LOW] > pVolume->x[HIGH]) || (vol.x[HIGH] < pVolume->x[LOW]) ||
			(vol.y[LOW] > pVolume->y[HIGH]) || (vol.y[HIGH] < pVolume->y[LOW]) ||
			(vol.z[LOW] > pVolume->z[HIGH]) || (vol.z[HIGH] < pVolume->z[LOW])){
			// line exists out of volume.
			return false;
		}

		// get edge of volume boundary.
		int nBoundary=4;
		LINE_T lnBoundary[4]={0};
		lnBoundary[0].ptStart.x = pVolume->x[LOW];
		lnBoundary[0].ptStart.y = pVolume->y[LOW];
		lnBoundary[0].ptStart.z = 0.;
		lnBoundary[0].ptEnd.x = pVolume->x[HIGH];
		lnBoundary[0].ptEnd.y = pVolume->y[LOW];
		lnBoundary[0].ptEnd.z = 0.;
		lnBoundary[1].ptStart.x = pVolume->x[HIGH];
		lnBoundary[1].ptStart.y = pVolume->y[LOW];
		lnBoundary[1].ptStart.z = 0.;
		lnBoundary[1].ptEnd.x = pVolume->x[HIGH];
		lnBoundary[1].ptEnd.y = pVolume->y[HIGH];
		lnBoundary[1].ptEnd.z = 0.;
		lnBoundary[2].ptStart.x = pVolume->x[HIGH];
		lnBoundary[2].ptStart.y = pVolume->y[HIGH];
		lnBoundary[2].ptStart.z = 0.;
		lnBoundary[2].ptEnd.x = pVolume->x[LOW];
		lnBoundary[2].ptEnd.y = pVolume->y[HIGH];
		lnBoundary[2].ptEnd.z = 0.;
		lnBoundary[3].ptStart.x = pVolume->x[LOW];
		lnBoundary[3].ptStart.y = pVolume->y[HIGH];
		lnBoundary[3].ptStart.z = 0.;
		lnBoundary[3].ptEnd.x = pVolume->x[LOW];
		lnBoundary[3].ptEnd.y = pVolume->y[LOW];
		lnBoundary[3].ptEnd.z = 0.;

		int nCount=0;
		LINE_T ln[4]={0};
		memcpy(&ln[nCount++],pLine,sizeof(LINE_T));
		
		POINT_T pt={0};
		bool bIntersection=false;
		int i = 0;
		int j = 0;
		for(i=0;i < nCount;bIntersection=false){
			for(j=0;j < nBoundary;j++){
				if(DO_INTERSECTION == IntersectLine2D(pt,lnBoundary[j],ln[i])){
					if(!(IsEqual(pt,ln[i].ptStart) || IsEqual(pt,ln[i].ptEnd))){
						// occur line intersect!
						bIntersection=true;
						double z = CMath::GetInterpolationZ(ln[i],pt);
						ln[nCount].ptStart.x = pt.x;
						ln[nCount].ptStart.y = pt.y;
						ln[nCount].ptStart.z = z;
						ln[nCount].ptEnd.x = ln[i].ptEnd.x;
						ln[nCount].ptEnd.y = ln[i].ptEnd.y;
						ln[nCount++].ptEnd.z = z;

						ln[i].ptEnd.x = pt.x;
						ln[i].ptEnd.y = pt.y;
						ln[i].ptEnd.z = z;
						ret=true;
					}
				}
			}
			if(bIntersection)
				i=0;
			else	i++;
		}

		RECT_T rect={0};
		rect.x = pVolume->x[LOW];
		rect.y = pVolume->y[LOW];
		rect.width = pVolume->x[HIGH] - pVolume->x[LOW];
		rect.height= pVolume->y[HIGH] - pVolume->y[LOW];
		for(i=0;i < nCount;i++){
			pt.x = (ln[i].ptStart.x + ln[i].ptEnd.x)*0.5;
			pt.y = (ln[i].ptStart.y + ln[i].ptEnd.y)*0.5;
			if(IsPointInRect(pt,rect)){
				pLine->ptStart.x = ln[i].ptStart.x;
				pLine->ptStart.y = ln[i].ptStart.y;
				pLine->ptStart.z = ln[i].ptStart.z;
				pLine->ptEnd.x = ln[i].ptEnd.x;
				pLine->ptEnd.y = ln[i].ptEnd.y;
				pLine->ptEnd.z = ln[i].ptEnd.z;
				break;
			}
		}
	}

	return ret;
}

/*int IntersectLineToVolume(PLINE_T pLine,PSQVOLUME pVolume,PPOINT_T pRet){
	assert(pLine && pVolume && pRet);
	int cnt = 0;
	double len,dlen;
	POINT_T  pt;

	// get volume of line.
	SQVOLUME vol={0};
	vol.x[LOW] = vol.x[HIGH] = pLine->ptStart.x;
	vol.y[LOW] = vol.y[HIGH] = pLine->ptStart.y;
	vol.z[LOW] = vol.z[HIGH] = pLine->ptStart.z;
	if(vol.x[LOW] > pLine->ptEnd.x) vol.x[LOW] = pLine->ptEnd.x;
	if(vol.x[HIGH]< pLine->ptEnd.x) vol.x[HIGH]= pLine->ptEnd.x;
	if(vol.y[LOW] > pLine->ptEnd.y) vol.y[LOW] = pLine->ptEnd.y;
	if(vol.y[HIGH]< pLine->ptEnd.y) vol.y[HIGH]= pLine->ptEnd.y;
	if(vol.z[LOW] > pLine->ptEnd.z) vol.z[LOW] = pLine->ptEnd.z;
	if(vol.z[HIGH]< pLine->ptEnd.z) vol.z[HIGH]= pLine->ptEnd.z;
	
	if((vol.x[LOW] > pVolume->x[HIGH]) || (vol.x[HIGH] < pVolume->x[LOW]) ||
	   (vol.y[LOW] > pVolume->y[HIGH]) || (vol.y[HIGH] < pVolume->y[LOW]) ||
	   (vol.z[LOW] > pVolume->z[HIGH]) || (vol.z[HIGH] < pVolume->z[LOW])){
		// line exists out of volume.
		return 0;
	}

	len = sqrt((pLine->ptEnd.x - pLine->ptStart.x)*(pLine->ptEnd.x - pLine->ptStart.x) +
		(pLine->ptEnd.y - pLine->ptStart.y)*(pLine->ptEnd.y - pLine->ptStart.y) + 
		(pLine->ptEnd.z - pLine->ptStart.z)*(pLine->ptEnd.z - pLine->ptStart.z));
	if(len != 0.){
		if(pLine->ptStart.x != pLine->ptEnd.x){
			if((pLine->ptStart.x - pVolume->x[LOW])*(pLine->ptEnd.x - pVolume->x[LOW]) < 0.){
				pt.x = pVolume->x[LOW];
				
				dlen = (pRet[cnt].x - pLine->ptStart.x)*len/(pLine->ptEnd.x - pLine->ptStart.x);
				pt.y = pLine->ptStart.y + (pLine->ptEnd.y - pLine->ptStart.y)*dlen/len;
				pt.z = pLine->ptStart.z + (pLine->ptEnd.z - pLine->ptStart.z)*dlen/len;

				if(pLine->ptStart.x < pLine->ptEnd.x){
					pLine->ptStart.x = pt.x;
					pLine->ptStart.y = pt.y;
					pLine->ptStart.z = pt.z;
				}else{
					pLine->ptEnd.x = pt.x;
					pLine->ptEnd.y = pt.y;
					pLine->ptEnd.z = pt.z;
				}
			}
			if((pLine->ptStart.x - pVolume->x[HIGH])*(pLine->ptEnd.x - pVolume->x[HIGH]) < 0.){
				pt.x = pVolume->x[HIGH];
				
				dlen = (pRet[cnt].x - pLine->ptStart.x)*len/(pLine->ptEnd.x - pLine->ptStart.x);
				pt.y = pLine->ptStart.y + (pLine->ptEnd.y - pLine->ptStart.y)*dlen/len;
				pt.z = pLine->ptStart.z + (pLine->ptEnd.z - pLine->ptStart.z)*dlen/len;

				if(pLine->ptStart.x < pLine->ptEnd.x){
					pLine->ptEnd.x = pt.x;
					pLine->ptEnd.y = pt.y;
					pLine->ptEnd.z = pt.z;
				}else{
					pLine->ptStart.x = pt.x;
					pLine->ptStart.y = pt.y;
					pLine->ptStart.z = pt.z;
				}
			}
		}

		if(pLine->ptStart.y != pLine->ptEnd.y){
			if((pLine->ptStart.y - pVolume->y[LOW])*(pLine->ptEnd.y - pVolume->y[LOW]) < 0.){
				pt.y = pVolume->y[LOW];
				
				dlen = (pRet[cnt].y - pLine->ptStart.y)*len/(pLine->ptEnd.y - pLine->ptStart.y);
				pt.x = pLine->ptStart.x + (pLine->ptEnd.x - pLine->ptStart.x)*dlen/len;
				pt.z = pLine->ptStart.z + (pLine->ptEnd.z - pLine->ptStart.z)*dlen/len;

				if(pLine->ptStart.y < pLine->ptEnd.y){
					pLine->ptStart.x = pt.x;
					pLine->ptStart.y = pt.y;
					pLine->ptStart.z = pt.z;
				}else{
					pLine->ptEnd.x = pt.x;
					pLine->ptEnd.y = pt.y;
					pLine->ptEnd.z = pt.z;
				}
			}
			if((pLine->ptStart.y - pVolume->y[HIGH])*(pLine->ptEnd.y - pVolume->y[HIGH]) < 0.){
				pt.y = pVolume->y[HIGH];
				
				dlen = (pRet[cnt].y - pLine->ptStart.y)*len/(pLine->ptEnd.y - pLine->ptStart.y);
				pt.x = pLine->ptStart.x + (pLine->ptEnd.x - pLine->ptStart.x)*dlen/len;
				pt.z = pLine->ptStart.z + (pLine->ptEnd.z - pLine->ptStart.z)*dlen/len;

				if(pLine->ptStart.y < pLine->ptEnd.y){
					pLine->ptEnd.x = pt.x;
					pLine->ptEnd.y = pt.y;
					pLine->ptEnd.z = pt.z;
				}else{
					pLine->ptStart.x = pt.x;
					pLine->ptStart.y = pt.y;
					pLine->ptStart.z = pt.z;
				}
			}
		}

		if(pLine->ptStart.z != pLine->ptEnd.z){
			if((pLine->ptStart.z - pVolume->z[LOW])*(pLine->ptEnd.z - pVolume->z[LOW]) < 0.){
				pt.z = pVolume->z[LOW];
				
				dlen = (pRet[cnt].z - pLine->ptStart.z)*len/(pLine->ptEnd.z - pLine->ptStart.z);
				pt.x = pLine->ptStart.x + (pLine->ptEnd.x - pLine->ptStart.x)*dlen/len;
				pt.y = pLine->ptStart.z + (pLine->ptEnd.y - pLine->ptStart.y)*dlen/len;

				if(pLine->ptStart.z < pLine->ptEnd.z){
					pLine->ptStart.x = pt.x;
					pLine->ptStart.y = pt.y;
					pLine->ptStart.z = pt.z;
				}else{
					pLine->ptEnd.x = pt.x;
					pLine->ptEnd.y = pt.y;
					pLine->ptEnd.z = pt.z;
				}
			}
			if((pLine->ptStart.z - pVolume->z[HIGH])*(pLine->ptEnd.z - pVolume->z[HIGH]) < 0.){
				pt.z = pVolume->z[HIGH];
				
				dlen = (pRet[cnt].z - pLine->ptStart.z)*len/(pLine->ptEnd.z - pLine->ptStart.z);
				pt.x = pLine->ptStart.x + (pLine->ptEnd.x - pLine->ptStart.x)*dlen/len;
				pt.y = pLine->ptStart.y + (pLine->ptEnd.y - pLine->ptStart.y)*dlen/len;

				if(pLine->ptStart.z < pLine->ptEnd.z){
					pLine->ptEnd.x = pt.x;
					pLine->ptEnd.y = pt.y;
					pLine->ptEnd.z = pt.z;
				}else{
					pLine->ptStart.x = pt.x;
					pLine->ptStart.y = pt.y;
					pLine->ptStart.z = pt.z;
				}
			}
		}
	}

	return 1;
}
*/

/**	\brief	The ITKGeo::ComputePlaneEquation function

	\param	pPlaneEquation	a parameter of type PPLANE_T
	\param	pt1	a parameter of type POINT_T
	\param	pt2	a parameter of type POINT_T
	\param	pt3	a parameter of type POINT_T

	\return	bool	
*/
bool ITKGeo::ComputePlaneEquation(PPLANE_T pPlaneEquation,POINT_T pt1,POINT_T pt2,POINT_T pt3){
	assert(pPlaneEquation && "pPlaneEquation is invalid");
	bool bRet = false;

	if(pPlaneEquation){		
		assert(!((pt1.x == pt2.x) && (pt1.y == pt2.y) && (pt1.z == pt2.z)));
		assert(!((pt1.x == pt3.x) && (pt1.y == pt3.y) && (pt1.z == pt3.z)));
		if(!((pt1.x == pt2.x) && (pt1.y == pt2.y) && (pt1.z == pt2.z)) && 
			!((pt1.x == pt3.x) && (pt1.y == pt3.y) && (pt1.z == pt3.z))){
			
			VECTOR_T vec[2]={0.,};
			vec[0].dx = pt2.x - pt1.x,vec[0].dy = pt2.y - pt1.y,vec[0].dz = pt2.z - pt1.z;
			vec[1].dx = pt3.x - pt1.x,vec[1].dy = pt3.y - pt1.y,vec[1].dz = pt3.z - pt1.z;
			
			double iicp=0.,jjcp=0.,kkcp=0.;
			
			VECTOR_T vecRet=CrossProduct(vec[0],vec[1]);
			assert(!((vecRet.dx == 0.0) && (vecRet.dy == 0.0) && (vecRet.dz == 0.0)));
			if(!((0.0 == vecRet.dx) && (0.0 == vecRet.dy) && (0.0 == vecRet.dz))){
				// normalize plane equation 
				Normalize(vecRet);
				
				//* compute D of plane equation
				pPlaneEquation->d= -(vecRet.dx*pt1.x) - (vecRet.dy*pt1.y) - (vecRet.dz*pt1.z);
				
				bRet = true;
			}
		}
	}

	return bRet;
}

/**	\brief	The ITKGeo::GetMinDistPoint function

	\param	pt	the point
	\param	nLoc	location of the point
	\param	line	the line.

	\return	POINT_T	
*/
POINT_T ITKGeo::GetMinDistPoint(const POINT_T& pt,int& nLoc,const LINE_T& line){
	VECTOR_T vec13 = pt - line.ptStart;
	VECTOR_T vec12 = line.ptEnd - line.ptStart;

	double t = (vec13.dx*vec12.dx + vec13.dy*vec12.dy + vec13.dz*vec12.dz)/
		   (vec12.dx*vec12.dx + vec12.dy*vec12.dy + vec12.dz*vec12.dz);

	POINT_T ret={0,};
	ret.x = line.ptStart.x + t*vec12.dx;
	ret.y = line.ptStart.y + t*vec12.dy;
	ret.z = line.ptStart.z + t*vec12.dz;

	nLoc = ((t > 1.) || (t < 0.)) ? OUTER_POINT : INNER_POINT;
	if(OUTER_POINT == nLoc){
		double dx[2]={0,},dy[2]={0,},dz[2]={0,};
		double nLen[2]={0,};

		dx[0] = line.ptStart.x - pt.x;
		dy[0] = line.ptStart.y - pt.y;
		dz[0] = line.ptStart.z - pt.z;
		dx[1] = line.ptEnd.x   - pt.x;
		dy[1] = line.ptEnd.y   - pt.y;
		dz[1] = line.ptEnd.z   - pt.z;

		nLen[0] = dx[0]*dx[0] + dy[0]*dy[0] + dz[0]*dz[0];
		nLen[1] = dx[1]*dx[1] + dy[1]*dy[1] + dz[1]*dz[1];
		
		ret = (nLen[0] < nLen[1]) ? line.ptStart : line.ptEnd;
	}

	return ret;
}

/**	\brief	The - function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	const VECTOR_T& operator	
*/
const VECTOR_T& operator -(const POINT_T& pt1,const POINT_T& pt2){
	static VECTOR_T ret={0,};

	ret.dx = pt1.x - pt2.x;
	ret.dy = pt1.y - pt2.y;
	ret.dz = pt1.z - pt2.z;

	return ret;
}

/**	\brief	The + function

	\param	pt	a parameter of type const POINT_T&
	\param	vec	a parameter of type const VECTOR_T&

	\return	const POINT_T& operator	
*/
const POINT_T& operator +(const POINT_T& pt,const VECTOR_T& vec){
	static POINT_T ret={0,};

	ret.x = pt.x + vec.dx;
	ret.y = pt.y + vec.dy;
	ret.z = pt.z + vec.dz;

	return ret;
}
