#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <graphics\graphics.h>
#include <graphics\curve3d.h>

__declspec(dllexport) POINT_T CCurve3d::m_pt[MAX_CURVE_POINTS]={0,};
// sample number of segments = '6 , 8 , 12 , 16 , 20 , etc'
__declspec(dllexport) int CCurve3d::NUM_OF_SEGMENTS = 12;//20; // default number of segments

extern "C" __declspec(dllexport) void SetCurveSegmentCount(int nCount)
{
	assert((nCount > 0) && "nCount is less than zero.");

	if(nCount > 0) CCurve3d::NUM_OF_SEGMENTS = nCount;
}

/**	\brief
*/
CCurve3d::CCurve3d()
{
}

/**	\brief
*/
CCurve3d::~CCurve3d()
{
}

/**	\brief	The CCurve::SetSegments function

	\param	nSegments	a parameter of type const int

	\return	void	
*/
void CCurve3d::SetSegments(const int nSegments)
{
	assert((nSegments > 0) && "facet's count is less than zero.");
	
	CCurve3d::NUM_OF_SEGMENTS = nSegments;
}

/**	\brief	The CCurve::GetNumOfPoints function


	\return	long	
*/
int CCurve3d::GetNumOfPoints(){
	return (CCurve3d::NUM_OF_SEGMENTS + 1);
}

/**	\brief	The CCurve::GetFacets function


	\return	PPOINT_T	
*/
PPOINT_T CCurve3d::GetFacets(){
	return NULL;
}

/**	\brief	The CCurve3d::Sweep function

	\param	vecAxis	a parameter of type VECTOR_T
	\param	nAngle	a parameter of type double
	\param	nParts	a parameter of type int

	\return	PPOINT_T	
*/
PPOINT_T CCurve3d::Sweep(VECTOR_T vecAxis,double nAngle,int nParts)
{
    	return NULL;
}

/**	\brief	The CCurve3d::Revolve function

	\param	vecAxis	a parameter of type const VECTOR_T&
	\param	nSize	a parameter of type int
	\param	pptOriginal	a parameter of type PPOINT_T
	\param	nRotate	a parameter of type double
	\param	nParts	a parameter of type int

	\remarks	nAngle - rotate angle in degree.

	\return	PPOINT_T	
*/
PPOINT_T CCurve3d::Revolve(const VECTOR_T& vecAxis,int nSize,PPOINT_T pptOriginal,const double& nRotate,const int& nParts)
{
	assert(pptOriginal  && "pptOriginal is NULL");
	assert((nParts > 0) && "nParts is less than 0.");
	
	if(pptOriginal && (nParts > 0))
	{
		double alpha=0.,beta=0.;
		CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
		
		// set vecAxis to z<0,0,1> by rotating about x,y axis.
		int i = 0;
		for(i = 0;i < nSize;i++)
		{
			CCurve3d::m_pt[i] = CGeometry::RotateAboutXYAxis(pptOriginal[i],alpha,beta);
		}
		
		// rotate about z-axis<0,0,1>
		double nSweep= DEG2RAD(nRotate);
		double nStep = nSweep/nParts;
		double nAngle= nStep;
		double c=cos(nStep),s=sin(nStep);
		int j = 0;
		for(j = 1;j < nParts;j++)
		{
			for(i = 0;i < nSize;i++)
			{
				int idxSrc=(j-1)*nSize + i;
				int idxDst=j*nSize + i;

				CCurve3d::m_pt[idxDst].x = CCurve3d::m_pt[idxSrc].x*c - CCurve3d::m_pt[idxSrc].y*s;
				CCurve3d::m_pt[idxDst].y = CCurve3d::m_pt[idxSrc].x*s + CCurve3d::m_pt[idxSrc].y*c;
				CCurve3d::m_pt[idxDst].z = CCurve3d::m_pt[idxSrc].z;
			}
			nAngle += nStep;
		}
		c=cos(nSweep),s=sin(nSweep);
		for(i = 0;i < nSize;i++)
		{
			CCurve3d::m_pt[j*nSize + i].x = CCurve3d::m_pt[i].x*c - CCurve3d::m_pt[i].y*s;
			CCurve3d::m_pt[j*nSize + i].y = CCurve3d::m_pt[i].x*s + CCurve3d::m_pt[i].y*c;
			CCurve3d::m_pt[j*nSize + i].z = CCurve3d::m_pt[i].z;
		}
		
		// set rotate-axis to vecAxis
		for(j = 0;j < nParts + 1;j++)
		{
			for(i = 0;i < nSize;i++)
			{
				CCurve3d::m_pt[j*nSize + i] = CGeometry::RotateAboutYXAxis(CCurve3d::m_pt[j*nSize + i],-beta,-alpha);
			}
		}
		
		return CCurve3d::m_pt;
	}

	return NULL;
}

/**	\brief	The CCurve3d::NUM_OF_SEGMENTS function


	\return	const int	
*/
const int CCurve3d::GetNumOfSegments()
{
	return NUM_OF_SEGMENTS;
}

/**	\brief	The CCurve3d::pt function

	\param	nIndex	a parameter of type const int&

	\return	POINT_T&	
*/
POINT_T& CCurve3d::pt(const int& nIndex)
{
	return m_pt[nIndex];
}
