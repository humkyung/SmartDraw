#include <assert.h>
#include <math.h>
#include <memory.h>
#include <graphics/enum.h>
#include <graphics/arc3d.h>
#include <graphics/geometry.h>

/**	\brief
*/
CArc3d::CArc3d(){
	m_nId = ENM_ITK_ARC;

	ptOrigin.x = 0;
	ptOrigin.y = 0;
	ptOrigin.z = 0;

	ptStart.x = 1;
	ptStart.y = 0;
	ptStart.z = 0;

	vecNorm.dx = 0;
	vecNorm.dy = 0;
	vecNorm.dz = 1;

	nRatio = 1;
	nSweep = 360.;
}

/**	\brief
*/
CArc3d::CArc3d(const ARC_T& arc){
	m_nId = ENM_ITK_ARC;

	ptOrigin = arc.ptOrigin;
	ptStart = arc.ptStart;
	vecNorm = arc.vecNorm;

	nRatio = arc.nRatio;
	nSweep = arc.nSweep;
	
	m_nPoints = 0L;
}

/**	\brief
*/
CArc3d::CArc3d(const CArc3d& arc){
	m_nId = ENM_ITK_ARC;

	ptOrigin.x = arc.ptOrigin.x;
	ptOrigin.y = arc.ptOrigin.y;
	ptOrigin.z = arc.ptOrigin.z;
	
	ptStart.x  = arc.ptStart.x;
	ptStart.y  = arc.ptStart.y;
	ptStart.z  = arc.ptStart.z;

	vecNorm.dx = arc.vecNorm.dx;
	vecNorm.dy = arc.vecNorm.dy;
	vecNorm.dz = arc.vecNorm.dz;

	nRatio = arc.nRatio;
	nSweep = arc.nSweep;

	m_nPoints = 0L;
}

/**	\brief
*/
CArc3d::CArc3d(POINT_T ptOrigin,POINT_T ptStart,const double& nSweepAng){
	m_nId = ENM_ITK_ARC;

	ptOrigin.x = ptOrigin.x;
	ptOrigin.y = ptOrigin.y;
	ptOrigin.z = ptOrigin.z;

	ptStart.x  = ptStart.x;
	ptStart.y  = ptStart.y;
	ptStart.z  = ptStart.z;

	vecNorm.dx = 0;
	vecNorm.dy = 0;
	vecNorm.dz = 1;

	nRatio = 1;
	nSweep = nSweepAng;

	m_nPoints = 0L;
}

/**	\brief
*/
CArc3d::~CArc3d(){
	m_nSegments = 0;
}

/**	\brief	The CArc3d::on function

	\param	ptOrigin	a parameter of type const POINT_T&
	\param	vecNorm	a parameter of type const SQVECTOR
	\param	nRadius	a parameter of type const double
	\param	nAngle	a parameter of type const double

	\return	POINT_T	
*/
POINT_T CArc3d::on(const POINT_T& ptOrigin,const SQVECTOR vecNorm,const double nRadius,const double& nAngle){
	POINT_T ptRet={0};

	double alpha=0.,beta=0.;
	CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);

	ptRet.x = nRadius*cos(nAngle);
	ptRet.y = nRadius*sin(nAngle);
	ptRet.z = 0;

	ptRet = CGeometry::RotateAboutYXAxis(ptRet,-beta,-alpha);
	
	ptRet.x += ptOrigin.x;
	ptRet.y += ptOrigin.y;
	ptRet.z += ptOrigin.z;

	return ptRet;
}

/**	\brief	The CArc3d::segments function

	\param	ptEDGE[]	a parameter of type POINT_T
	\param	ptOrigin	a parameter of type const POINT_T&
	\param	ptStart	a parameter of type const POINT_T&
	\param	vecNorm	a parameter of type const SQVECTOR&
	\param	nSweep	a parameter of type const double
	\param	nSegments	a parameter of type const int

	\return	void	
*/
void CArc3d::segments(POINT_T ptEDGE[],const POINT_T& ptOrigin,const POINT_T& ptStart,const SQVECTOR& vecNorm,const double nSweep,const int& nSegments){
	assert((nSegments > 1) && "nSegments is less than 1.");
	
	double alpha=0.,beta=0;
	CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
	
	POINT_T ptDelta={0};
	ptDelta.x=ptStart.x - ptOrigin.x;
	ptDelta.y=ptStart.y - ptOrigin.y;
	ptDelta.z=ptStart.z - ptOrigin.z;
	double nRadius=sqrt(ptDelta.x*ptDelta.x + ptDelta.y*ptDelta.y + ptDelta.z*ptDelta.z);

	ptDelta = CGeometry::RotateAboutXYAxis(ptDelta,alpha,beta);
	double nStartAngle = CGeometry::GetRotatedAngleInXYPlane(ptDelta);
	double nEndAngle   = nStartAngle + DEG2RAD(nSweep);
	double nStep = (nEndAngle - nStartAngle)/nSegments;
	double nCos=cos(nStep),nSin=sin(nStep);
	
	ptEDGE[0].x = nRadius*cos(nStartAngle);
	ptEDGE[0].y = nRadius*sin(nStartAngle);
	ptEDGE[0].z = 0.;
	int i = 0;
	for(i=1;i < nSegments;i++){
		ptEDGE[i].x = ptEDGE[i - 1].x*nCos - ptEDGE[i - 1].y*nSin;
		ptEDGE[i].y = ptEDGE[i - 1].x*nSin + ptEDGE[i - 1].y*nCos;
		ptEDGE[i].z = 0.;
	}
	ptEDGE[i].x = nRadius*cos(nEndAngle);
	ptEDGE[i].y = nRadius*sin(nEndAngle);
	ptEDGE[i].z = 0.;

	for(i = 0;i < nSegments + 1;i++){
		ptEDGE[i] = CGeometry::RotateAboutYXAxis(ptEDGE[i],-beta,-alpha);
		ptEDGE[i].x += ptOrigin.x;
		ptEDGE[i].y += ptOrigin.y;
		ptEDGE[i].z += ptOrigin.z;
	}	
}

/**	\brief	The CArc3d::OnPoint function

	\param	ptOrigin	a parameter of type const POINT_T
	\param	nRadius	a parameter of type const double
	\param	vecAxis	a parameter of type const VECTOR_T
	\param	nAngle	a parameter of type const double

	\return	POINT_T	
*/
POINT_T CArc3d::OnPoint(const POINT_T ptOrigin,const double nRadius,const VECTOR_T vecAxis,const double& nAngle){
	POINT_T ptRet={0};

	double alpha=0.,beta=0.;
	CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);

	ptRet.x = nRadius*cos(nAngle);
	ptRet.y = nRadius*sin(nAngle);
	ptRet.z = 0;

	ptRet = CGeometry::RotateAboutYXAxis(ptRet,-beta,-alpha);
	
	ptRet.x += ptOrigin.x;
	ptRet.y += ptOrigin.y;
	ptRet.z += ptOrigin.z;

	return ptRet;
}

/**	\brief	The CArc3d::CreateSegments function\n
	user must have a copy of points of segments.\n
	because of another curve will overwrite the point value.


	\return	void	
*/
/*
void CArc3d::CreateSegments(){
	double alpha=0.,beta=0;
	CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
	
	POINT_T ptRelative={0.,0.,0.};
	ptRelative.x = ptStart.x - ptOrigin.x;
	ptRelative.y = ptStart.y - ptOrigin.y;
	ptRelative.z = ptStart.z - ptOrigin.z;
	double l=sqrt(ptRelative.x*ptRelative.x + ptRelative.y*ptRelative.y + ptRelative.z*ptRelative.z);
	
	ptRelative = CGeometry::RotateAboutXYAxis(ptRelative,alpha,beta);
	double nStartAngle = CGeometry::GetRotatedAngleInXYPlane(ptRelative);
	double nAngle=nStartAngle,rad=DEG2RAD(nSweep);
	if(360. == nSweep) nStartAngle = 0.;
	
	double ab=1./nRatio,ba=nRatio;
	const double nStepAngle=DEG2RAD(360)/(CCurve3d::NUM_OF_SEGMENTS);
	const int nStart=static_cast<int>(nStartAngle/nStepAngle) + 1;
	const int nEnd  =static_cast<int>((nStartAngle + nSweep) / nStepAngle);

	//const double nStepAngle = DEG2RAD(360)/(CCurve3d::NUM_OF_SEGMENTS);
	//const int nCount=static_cast<int>(rad/nStepAngle);
	const double c=cos(nStepAngle);
	const double s=sin(nStepAngle);
	CCurve3d::pt(0).x = l*cos(nStartAngle);
	CCurve3d::pt(0).y = l*sin(nStartAngle);
	CCurve3d::pt(0).z = 0;
	CCurve3d::pt(1).x = l*cos(nStart*nStepAngle);
	CCurve3d::pt(1).y = l*sin(nStart*nStepAngle);
	CCurve3d::pt(1).z = 0;
	for(int i = 2;i <= nEnd - nStart + 1;i++){
		CCurve3d::m_pt[i].x = CCurve3d::m_pt[i-1].x*c - ab*CCurve3d::m_pt[i-1].y*s;
		CCurve3d::m_pt[i].y = ba*CCurve3d::m_pt[i-1].x*s + CCurve3d::m_pt[i-1].y*c;
		CCurve3d::m_pt[i].z = 0.;
	}
	if((nEnd*nStepAngle) != (nStartAngle + rad)){
		if(1 == ab){
			CCurve3d::m_pt[i].x = l*cos(nStartAngle + rad);
			CCurve3d::m_pt[i].y = l*sin(nStartAngle + rad);
			CCurve3d::m_pt[i].z = 0.;
		}else{
			CCurve3d::m_pt[i].x = CCurve3d::m_pt[i-1].x*c - ab*CCurve3d::m_pt[i-1].y*s;
			CCurve3d::m_pt[i].y = ba*CCurve3d::m_pt[i-1].x*s + CCurve3d::m_pt[i-1].y*c;
			CCurve3d::m_pt[i].z = 0.;
		}
		i++;
	}
	m_nPoints = i;
	
	for(i = 0;i < m_nPoints;i++){
		CCurve3d::m_pt[i] = CGeometry::RotateAboutYXAxis(CCurve3d::m_pt[i],-beta,-alpha);
		CCurve3d::m_pt[i].x += ptOrigin.x;
		CCurve3d::m_pt[i].y += ptOrigin.y;
		CCurve3d::m_pt[i].z += ptOrigin.z;
	}
}
*/
void CArc3d::CreateSegments()
{
	double alpha=0.,beta=0;
	CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
	
	POINT_T ptRelative={0.,0.,0.};
	ptRelative.x = ptStart.x - ptOrigin.x;
	ptRelative.y = ptStart.y - ptOrigin.y;
	ptRelative.z = ptStart.z - ptOrigin.z;
	double l=sqrt(ptRelative.x*ptRelative.x + ptRelative.y*ptRelative.y + ptRelative.z*ptRelative.z);
	
	ptRelative = CGeometry::RotateAboutXYAxis(ptRelative,alpha,beta);
	
	double nStartAngle = CGeometry::GetRotatedAngleInXYPlane(ptRelative);
	const double nAngle=nStartAngle,rad=DEG2RAD(nSweep);
	if(360. == nSweep) nStartAngle = 0.;
	
	const double ab= 1.f / nRatio;
	const double ba= nRatio;
	const double nStepAngle=rad/(CCurve3d::NUM_OF_SEGMENTS);
	const double c=cos(nStepAngle);
	const double s=sin(nStepAngle);
	
	CCurve3d::pt(0).x = ptRelative.x;
	CCurve3d::pt(0).y = ptRelative.y;
	CCurve3d::pt(0).z = 0.f;
	int i = 0;
	for(i = 1;i < CCurve3d::NUM_OF_SEGMENTS;++i)
	{
		CCurve3d::m_pt[i].x = CCurve3d::m_pt[i-1].x*c - ab*CCurve3d::m_pt[i-1].y*s;
		CCurve3d::m_pt[i].y = ba*CCurve3d::m_pt[i-1].x*s + CCurve3d::m_pt[i-1].y*c;
		CCurve3d::m_pt[i].z = 0.;
	}
	if(nSweep == 360.f)
	{
		CCurve3d::m_pt[i].x = ptRelative.x;
		CCurve3d::m_pt[i].y = ptRelative.y;
		CCurve3d::m_pt[i].z = 0.;
	}
	else
	{
		if(1 == ab)
		{
			CCurve3d::m_pt[i].x = l*cos(nStartAngle + rad);
			CCurve3d::m_pt[i].y = l*sin(nStartAngle + rad);
			CCurve3d::m_pt[i].z = 0.;
		}
		else
		{
			CCurve3d::m_pt[i].x = CCurve3d::m_pt[i-1].x*c - ab*CCurve3d::m_pt[i-1].y*s;
			CCurve3d::m_pt[i].y = ba*CCurve3d::m_pt[i-1].x*s + CCurve3d::m_pt[i-1].y*c;
			CCurve3d::m_pt[i].z = 0.;
		}
	}
	
	for(i = 0;i < (CCurve3d::NUM_OF_SEGMENTS) + 1;++i)
	{
		CCurve3d::m_pt[i] = CGeometry::RotateAboutYXAxis(CCurve3d::m_pt[i],-beta,-alpha);
		CCurve3d::m_pt[i].x += ptOrigin.x;
		CCurve3d::m_pt[i].y += ptOrigin.y;
		CCurve3d::m_pt[i].z += ptOrigin.z;
	}
}

/**	\brief	The CArc3d::Revolve function

	\param	vecAxis	a parameter of type VECTOR_T
	\param	nAngle	a parameter of type double
	\param	nParts	a parameter of type int

	\return	PPOINT_T	
*/
PPOINT_T CArc3d::Revolve(VECTOR_T vecAxis,double nAngle,int nParts){
	int nPoints = GetNumOfPoints();

	double alpha=0.,beta=0.;
	CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
	int i = 0;
	for(i = 0;i < nPoints;i++){
		CCurve3d::m_pt[i].x -= ptOrigin.x;
		CCurve3d::m_pt[i].y -= ptOrigin.y;
		CCurve3d::m_pt[i].z -= ptOrigin.z;
		CCurve3d::m_pt[i]    = CGeometry::RotateAboutXYAxis(CCurve3d::m_pt[i],alpha,beta);
	}

	double nStartAngle=0.,nStepAngle = nAngle/nParts;
	int j = 0;
	for(j = 1;j < nParts + 1;j++){
		for(int i = 0;i < nPoints;i++){
			CCurve3d::m_pt[j*nPoints + i].x = CCurve3d::m_pt[i].x*cos(nStartAngle) - CCurve3d::m_pt[i].y*sin(nStartAngle);
			CCurve3d::m_pt[j*nPoints + i].y = CCurve3d::m_pt[i].x*sin(nStartAngle) + CCurve3d::m_pt[i].y*cos(nStartAngle);
			CCurve3d::m_pt[j*nPoints + i].z = CCurve3d::m_pt[i].z;
		}
		nStartAngle += nStepAngle;
	}
	for(i = 0;i < nPoints;i++){
		CCurve3d::m_pt[j*nPoints + i].x = CCurve3d::m_pt[i].x*cos(nAngle) - CCurve3d::m_pt[i].y*sin(nAngle);
		CCurve3d::m_pt[j*nPoints + i].y = CCurve3d::m_pt[i].x*sin(nAngle) + CCurve3d::m_pt[i].y*cos(nAngle);	
		CCurve3d::m_pt[j*nPoints + i].z = CCurve3d::m_pt[i].z;
	}
	
	for(j = 0;j < nParts + 2;j++){
		for(i = 0;i < nPoints;i++){
			CCurve3d::m_pt[j*nPoints + i] = CGeometry::RotateAboutYXAxis(CCurve3d::m_pt[j*nPoints + i],-beta,-alpha);
			CCurve3d::m_pt[j*nPoints + i].x += ptOrigin.x;
			CCurve3d::m_pt[j*nPoints + i].y += ptOrigin.y;
			CCurve3d::m_pt[j*nPoints + i].z += ptOrigin.z;
		}
	}

	return (CCurve3d::m_pt);
}
