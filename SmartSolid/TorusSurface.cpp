#include "stdafx.h"
#include <math.h>
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include <graphics/arc3d.h>
#include <graphics/circle3d.h>
#include "PolyCurve.h"
#include "libmesh/libmesh2d.h"
#include "TorusSurface.h"
#include "TinyTorusSurfTester.h"

#include "SmartSolid.h"
#include <IsPoint2d.h>
#include <DelaunayMesh.h>

#include <queue>
using namespace std;
using namespace mesh;

/**	\brief
*/
CTorusSurface::CTorusSurface() : m_paryBoundary(NULL)
{
	m_sfcTorus.ptOrigin.x   = 0.;
	m_sfcTorus.ptOrigin.y   = 0.;
	m_sfcTorus.ptOrigin.z   = 0.;
	m_sfcTorus.nTorusRadius = 10.;
	m_sfcTorus.nTubeRadius  = 1.;
//	m_sfcTorus.pBoundary  = NULL;
	
	m_nUStartAngle = 0.;
	m_nUEndAngle   = 2*PI;
	m_nUStepAngle  = PI/8;
	m_nVStepAngle  = PI/8;
	m_nUOffsetAngle= 0.;
	m_nBoundary    = 0;
	m_paryBoundary = NULL;
	m_bReversed    = false;

	OnCreate();
}

/**	\brief
*/
CTorusSurface::CTorusSurface(SQTORUSSURFACE& torussurface,bool bReversed)
{
	memcpy(&m_sfcTorus,&torussurface,sizeof(SQTORUSSURFACE));
	m_nUStartAngle = 0.;
	m_nUEndAngle   = 2*PI;
	m_nUStepAngle  = PI/8;
	m_nVStepAngle  = PI/8;
	m_nUOffsetAngle= 0.;
	m_nBoundary    = 0;
	m_paryBoundary = NULL;
	m_bReversed    = bReversed;

	OnCreate();
}

/**	\brief

	\remarks	[2003.07.10] - free 'm_paryBoundary' if necessary.
*/
CTorusSurface::~CTorusSurface()
{
	for(list<CFacet*>::iterator itr=m_lstFacet.begin();itr != m_lstFacet.end();)
	{
		delete (*itr);
		///m_lstFacet.erase(itr++);
	}
	m_lstFacet.clear();

	if(m_paryBoundary)
	{
		for(int i=0;i < m_nBoundary;++i)
		{
			delete m_paryBoundary[i].plstEdge;
		}

		if(m_paryBoundary) delete []m_paryBoundary;
	}
}

/**	
	@brief	The CTorusSurface::OnCreate function

	@author	HumKyung.BAEK

	@return	bool	
*/
bool CTorusSurface::OnCreate()
{
	double nTorusRadius= fabs(m_sfcTorus.nTorusRadius);
	double nTubeRadius = fabs(m_sfcTorus.nTubeRadius);

	if(nTorusRadius >= nTubeRadius)
	{
		m_nUStartAngle = 0.;
		m_nUEndAngle   = 2*PI;
		m_nUOffsetAngle= 0.;
	}
	else if(m_sfcTorus.nTorusRadius >= 0.)
	{
		double phi  = acos(nTorusRadius/nTubeRadius);
		double theta= PI*0.5 - phi;

		m_nUStartAngle = 0.;
		m_nUEndAngle   = PI + 2*theta;
		m_nUOffsetAngle= -(PI*0.5 + theta);
	}
	else
	{
		double phi  = acos(nTorusRadius/nTubeRadius);
		double theta= PI*0.5 - phi;

		m_nUStartAngle = 0.;
		m_nUEndAngle   = PI - theta;
		m_nUOffsetAngle= -(PI*0.5 - theta);
	}

	int NUM_OF_SEGMENTS=CCurve3d::GetNumOfSegments();
	int nCurveSegments = NUM_OF_SEGMENTS;
	m_nUStepAngle = m_nUEndAngle/nCurveSegments;
	m_nVStepAngle = (2*PI)/nCurveSegments;
	m_nFacets = nCurveSegments*nCurveSegments;
	int nIndex=0 , i = 0;
	for(i = 0;i < nCurveSegments;i++)
	{
		for(int j=0;j < nCurveSegments;j++)
		{
			CSurface3D::para[nIndex++].Set(i*m_nUStepAngle,j*m_nVStepAngle);
			//CSurface3D::para[nIndex++].v= CMath::round(j*m_nVStepAngle,13);
		}
		CSurface3D::para[nIndex++].Set(i*m_nUStepAngle,2*PI);
		//CSurface3D::para[nIndex++].v= CMath::round(2*PI,13);
	}
	for(int j = 0;j < nCurveSegments;j++)
	{
		CSurface3D::para[nIndex++].Set(m_nUEndAngle,j*m_nVStepAngle);
		//CSurface3D::para[nIndex++].v= CMath::round(j*m_nVStepAngle,13);
	}
	CSurface3D::para[nIndex].Set(m_nUEndAngle,2*PI);
	//CSurface3D::para[nIndex++].v= CMath::round(2*PI,13);
	
	//! allocate memory for m_paryBoundary.
	m_paryBoundary = new BOUNDARY_T[m_sfcTorus.nLoop];
	for(i = 0;i < m_sfcTorus.nLoop;++i)
	{
		memset(&m_paryBoundary[i] , 0 , sizeof(BOUNDARY_T));
		CreateBoundary(m_sfcTorus.pBoundary[i]);
		++m_nBoundary;
	}

	return true;
}

/**	\brief	The CTorusSurface::CreateBoundaryCurvePoint function

	\param	aryPara[]	a parameter of type CSurfacePara
	\param	nPoints	a parameter of type int&

	\return	void	
*/
void CTorusSurface::CreateBoundaryCurvePoint(CMeshPos2d aryPara[],int& nPoints)
{
	assert(aryPara       && "aryPara is NULL");
	assert((nPoints > 0) && "nPoints is less than 0.");

	if(aryPara && (nPoints > 0))
	{
		for(int i = 0;i < nPoints - 1;i++)
		{
			double du = aryPara[i + 1].x() - aryPara[i].x();
			double dv = aryPara[i + 1].y() - aryPara[i].y();
			
			if(fabs(du) > PI*0.5)
			{ /// may be over end of 'u' parameter.
				if(du > 0.)
				{ /// may be over 0 
					double t=-aryPara[i].x()/(aryPara[i + 1].x() - aryPara[i].x());
					double v=(aryPara[i + 1].y() - aryPara[i].y())*t + aryPara[i].y();
					CMeshPos2d pos2d(0.,v , 0.);
					if(!((pos2d == aryPara[i]) || (pos2d == aryPara[i+1])))
					{
						memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
						aryPara[i + 1].Set(0.,v , 0.);
						i++;
						nPoints++;
					}
					
					t=(2*PI - aryPara[i].x())/(aryPara[i + 1].x() - aryPara[i].x());
					v=(aryPara[i + 1].y() - aryPara[i].y())*t + aryPara[i].y();
					pos2d.Set(2*PI,v , 0.);
					if(!((pos2d == aryPara[i]) || (pos2d == aryPara[i+1])))
					{
						memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
						aryPara[i + 1].Set(2*PI,v , 0.);
						i++;
						nPoints++;
					}
				}
				else if(du < 0.)
				{ /// may be over 2*phi
					double t=(2*PI - aryPara[i].x())/(aryPara[i + 1].x() - aryPara[i].x());
					double v=(aryPara[i + 1].y() - aryPara[i].y())*t + aryPara[i].y();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(2*PI,v , 0.);
					i++;
					nPoints++;

					t=-aryPara[i].x()/(aryPara[i + 1].x() - aryPara[i].x());
					v=(aryPara[i + 1].y() - aryPara[i].y())*t + aryPara[i].y();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(0.,v , 0.);
					i++;
					nPoints++;
				}
			
			}
			else if(fabs(dv) > PI*0.5)
			{ /// may be over end of 'v' parameter.
				if(dv > 0.)
				{ /// may be over 0
					double t=-aryPara[i].y()/(aryPara[i + 1].y() - aryPara[i].y());
					double u=(aryPara[i + 1].x() - aryPara[i].x())*t + aryPara[i].x();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(u,0. , 0.);
					i++;
					nPoints++;
					
					t=(2*PI - aryPara[i].y())/(aryPara[i + 1].y() - aryPara[i].y());
					u=(aryPara[i + 1].x() - aryPara[i].x())*t + aryPara[i].x();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(u,2*PI , 0.);
					i++;
					nPoints++;
				}
				else if(dv < 0.)
				{ /// may be over 2*phi
					double t=(2*PI - aryPara[i].y())/(aryPara[i + 1].y() - aryPara[i].y());
					double u=(aryPara[i + 1].x() - aryPara[i].x())*t + aryPara[i].x();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(u,2*PI , 0.);
					i++;
					nPoints++;

					t=-aryPara[i].y()/(aryPara[i + 1].y() - aryPara[i].y());
					u=(aryPara[i + 1].x() - aryPara[i].x())*t + aryPara[i].x();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(u,0. , 0.);
					i++;
					nPoints++;
				}
			}
		}
	}
}

/**	
	@brief	The CTorusSurface::CreateBoundary function

	@author	HumKyung.BAEK

	@param	pPrimitive	a parameter of type PSQPRIMITIVE

	@return	void	
*/
void CTorusSurface::CreateBoundary(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		//! calculate primitive count.
		int nCount=0;
		for(PSQPRIMITIVE ptr=pPrimitive;ptr;ptr = ptr->next,++nCount);

		CMeshPos2d last;
		///if(NULL != m_paryBoundary[m_nBoundary])
		{
			bool bClosed=false;
			int nBoundaryIndex=0;
			for(PSQPRIMITIVE ptr=pPrimitive;ptr;ptr = ptr->next,nBoundaryIndex++)
			{
				if(m_bReversed) ptr->hdr.attr.reversed = !ptr->hdr.attr.reversed;
				
				switch(ptr->hdr.id)
				{
					case ENM_ARC:
					{
						CArc3d arc(ptr->body.arc);
						arc.CreateSegments();
						for(int i=0;i < arc.GetNumOfPoints();i++)
						{
							CLibMesh2d::pos[i]=func(CCurve3d::pt(i));
							CMeshPos2d pos2d = CLibMesh2d::pos[i];
							CLibMesh2d::pos[i].Set(CLibMesh2d::pos[i].x(),CLibMesh2d::pos[i].y() , 0.);
						}
						int nIndex = arc.GetNumOfPoints();
						bClosed = (IsSameValue(2*PI,DEG2RAD(arc.nSweep))) ? true : false;
						
						CreateBoundaryCurvePoint(CLibMesh2d::pos,nIndex);
						
						if(ptr->hdr.attr.reversed)
						{
							reverse(&CLibMesh2d::pos[0],&CLibMesh2d::pos[nIndex]);
							m_paryBoundary[m_nBoundary].vecNorm.dx = -ptr->body.arc.vecNorm.dx;
							m_paryBoundary[m_nBoundary].vecNorm.dy = -ptr->body.arc.vecNorm.dy;
							m_paryBoundary[m_nBoundary].vecNorm.dz = -ptr->body.arc.vecNorm.dz;
						}
						else
						{
							m_paryBoundary[m_nBoundary].vecNorm=ptr->body.arc.vecNorm;
						}
						if(bClosed)
						{
							POINT_T pt[2]={0,};
							pt[0].x = CLibMesh2d::pos[0].x();pt[0].y = CLibMesh2d::pos[0].y();pt[0].z = 0;
							pt[1].x = CLibMesh2d::pos[nIndex - 1].x();pt[1].y = CLibMesh2d::pos[nIndex - 1].y();pt[0].z = 0;
							if(!IsSamePoint(pt[0],pt[1])) CLibMesh2d::pos[nIndex++] = CLibMesh2d::pos[0];
						}else if(0 != nBoundaryIndex) CLibMesh2d::pos[0] = last;
						
						//if(ptr->hdr.attr.reversed){
						//	m_paryBoundary[m_nBoundary].vecNorm.dx = -ptr->body.arc.vecNorm.dx;
						//	m_paryBoundary[m_nBoundary].vecNorm.dy = -ptr->body.arc.vecNorm.dy;
						//	m_paryBoundary[m_nBoundary].vecNorm.dz = -ptr->body.arc.vecNorm.dz;
						//}else{
						//	m_paryBoundary[m_nBoundary].vecNorm=ptr->body.arc.vecNorm;
						//}
						if(NULL == m_paryBoundary[m_nBoundary].plstEdge) m_paryBoundary[m_nBoundary].plstEdge = new list<EDGE_T>;
						if(m_paryBoundary[m_nBoundary].plstEdge)
						{
							EDGE_T edge;
							for(int i = 0;i < nIndex - 1;i++)
							{
								CMeshPos2d pos2d[2];
								pos2d[0] = CLibMesh2d::pos[i];
								pos2d[1] = CLibMesh2d::pos[i+1];
								if((fabs(CLibMesh2d::pos[i+1].x() - CLibMesh2d::pos[i].x()) < PI) && (fabs(CLibMesh2d::pos[i+1].y() - CLibMesh2d::pos[i].y()) < PI))
								{
									CLibMesh2d::pos[i].SetX(CLibMesh2d::pos[i].x());
									CLibMesh2d::pos[i+1].SetX(CLibMesh2d::pos[i+1].x());
									edge.pos[0] = CLibMesh2d::pos[i];
									edge.pos[1] = CLibMesh2d::pos[i+1];
									last = CLibMesh2d::pos[i+1];
									m_paryBoundary[m_nBoundary].plstEdge->push_back(edge);
								}
								else
								{
									int d = 1;
								}
							}
							///m_nBoundary++;
						}
					}
					break;
					case SQ_INTCURVE:
					{
						bool _bClosed=false;
						int i=0;
						for(i=0;i < ptr->body.intcurve.nPoints;i++)
						{
							CLibMesh2d::pos[i]=func(ptr->body.intcurve.ppt[i]);
							CLibMesh2d::pos[i].Set(CLibMesh2d::pos[i].x(),CLibMesh2d::pos[i].y() , 0.);
						}
						if(CLibMesh2d::pos[0] == CLibMesh2d::pos[i-1]) _bClosed = true;
						int nIndex = ptr->body.intcurve.nPoints;
						if(ptr->hdr.attr.reversed) reverse(&CLibMesh2d::pos[0],&CLibMesh2d::pos[nIndex]);
						if((0 != nBoundaryIndex) && !bClosed) CLibMesh2d::pos[0] = last;
						bClosed = _bClosed;
						CreateBoundaryCurvePoint(CLibMesh2d::pos,nIndex);
						
						m_paryBoundary[m_nBoundary].vecNorm=ptr->body.intcurve.vecNorm;
						if(NULL == m_paryBoundary[m_nBoundary].plstEdge) m_paryBoundary[m_nBoundary].plstEdge = new list<EDGE_T>;
						if(m_paryBoundary[m_nBoundary].plstEdge)
						{
							EDGE_T edge;
							for(i = 0;i < nIndex - 1;i++)
							{
								if((fabs(CLibMesh2d::pos[i+1].x() - CLibMesh2d::pos[i].x()) < PI) && (fabs(CLibMesh2d::pos[i+1].y() - CLibMesh2d::pos[i].y()) < PI))
								{
									edge.pos[0].Set(CLibMesh2d::pos[i].x(),CLibMesh2d::pos[i].y() , 0.);
									edge.pos[1].Set(CLibMesh2d::pos[i+1].x(),CLibMesh2d::pos[i+1].y() , 0.);
									m_paryBoundary[m_nBoundary].plstEdge->push_back(edge);
								}
							}
							///m_nBoundary++;
						}
					}
					break;
				}
			}
		}
	}
}

/**	\brief	The CTorusSurface::func function

	\param	para	a parameter of type CSurfacePara&

	\return	POINT_T	
*/
POINT_T CTorusSurface::func(CSurfacePara& para)
{
	POINT_T ptRet={0.,0.,0.};

	double _u = para.u() + m_nUOffsetAngle;
	ptRet.x = (m_sfcTorus.nTorusRadius + m_sfcTorus.nTubeRadius*cos(_u))*cos(para.v());
	ptRet.y = (m_sfcTorus.nTorusRadius + m_sfcTorus.nTubeRadius*cos(_u))*sin(para.v());
	ptRet.z = m_sfcTorus.nTubeRadius*sin(_u);
	
	double alpha=0,beta=0;
	CGeometry::GetRotatedAngleOfAxis(m_sfcTorus.vecNorm,alpha,beta);
	ptRet = CGeometry::RotateAboutYXAxis(ptRet,-beta,-alpha);
	ptRet.x += m_sfcTorus.ptOrigin.x;
	ptRet.y += m_sfcTorus.ptOrigin.y;
	ptRet.z += m_sfcTorus.ptOrigin.z;
	ptRet.x = _round(ptRet.x,9);
	ptRet.y = _round(ptRet.y,9);
	ptRet.z = _round(ptRet.z,9);

	return ptRet;
}

/**	\brief	The CTorusSurface::func function

	\param	u	a parameter of type double
	\param	v	a parameter of type double

	\return	POINT_T	
*/
POINT_T CTorusSurface::func(double u,double v)
{
	POINT_T ptRet={0.,0.,0.};

	double _u = u + m_nUOffsetAngle;
	ptRet.x = (m_sfcTorus.nTorusRadius + m_sfcTorus.nTubeRadius*cos(_u))*cos(v);
	ptRet.y = (m_sfcTorus.nTorusRadius + m_sfcTorus.nTubeRadius*cos(_u))*sin(v);
	ptRet.z = m_sfcTorus.nTubeRadius*sin(_u);
	
	double alpha=0,beta=0;
	CGeometry::GetRotatedAngleOfAxis(m_sfcTorus.vecNorm,alpha,beta);
	ptRet = CGeometry::RotateAboutYXAxis(ptRet,-beta,-alpha);
	ptRet.x += m_sfcTorus.ptOrigin.x;
	ptRet.y += m_sfcTorus.ptOrigin.y;
	ptRet.z += m_sfcTorus.ptOrigin.z;

	return ptRet;
}

/**	\brief	The CTorusSurface::func function

	\param	pt	a parameter of type POINT_T&

	\remarks	[BUG FIX]\n
	2002.05.22 - add routine supporting torus which nTorusRadus is less than 0\n
	2003.07.11 - update a routine which calculate u and v.

	\return	CSurfacePara	
*/
CMeshPos2d CTorusSurface::func(POINT_T& pt)
{
	#define TOLERANCE	0.00000001
	CMeshPos2d para;

	if(m_sfcTorus.nTubeRadius < 0.)
	{
		POINT_T _pt={0};
		_pt.x = pt.x - m_sfcTorus.ptOrigin.x;
		_pt.y = pt.y - m_sfcTorus.ptOrigin.y;
		_pt.z = pt.z - m_sfcTorus.ptOrigin.z;
		
		double alpha=0,beta=0;
		CGeometry::GetRotatedAngleOfAxis(m_sfcTorus.vecNorm,alpha,beta);
		_pt   = CGeometry::RotateAboutXYAxis(_pt,alpha,beta);

		double cval  = _pt.x/(m_sfcTorus.nTorusRadius + m_sfcTorus.nTubeRadius);
		double sval  = _pt.y/(m_sfcTorus.nTorusRadius + m_sfcTorus.nTubeRadius);
		SQVECTOR vec={0};
		vec.dx = cval;
		vec.dy = sval;
		vec.dz = 0.;
		CMath::NormalizeVector(vec);
		cval = acos(vec.dx);
		sval = asin(vec.dy);
		if(sval > 0)	para.SetY(cval);
		else		para.SetY(2*PI - cval);

		cval = ((_pt.x)/cos(para.y()) - m_sfcTorus.nTorusRadius)/m_sfcTorus.nTubeRadius;
		sval = (_pt.z )/m_sfcTorus.nTubeRadius;
		vec.dx = cval;
		vec.dy = sval;
		vec.dz = 0.;
		CMath::NormalizeVector(vec);
		cval = acos(vec.dx);
		sval = asin(vec.dy);
		if(sval > 0)	para.SetX(cval);
		else		para.SetX(2*PI - cval);
	}
	else if(m_sfcTorus.nTorusRadius >= m_sfcTorus.nTubeRadius)
	{
		m_nUStartAngle = 0.;
		m_nUEndAngle   = 2*PI;
		m_nUOffsetAngle= 0.;
		POINT_T _pt={0};
		_pt.x = pt.x - m_sfcTorus.ptOrigin.x;
		_pt.y = pt.y - m_sfcTorus.ptOrigin.y;
		_pt.z = pt.z - m_sfcTorus.ptOrigin.z;
		
		double alpha=0,beta=0;
		CGeometry::GetRotatedAngleOfAxis(m_sfcTorus.vecNorm,alpha,beta);
		_pt   = CGeometry::RotateAboutXYAxis(_pt,alpha,beta);
		
		//-->
		double nLen=sqrt(_pt.x*_pt.x + _pt.y*_pt.y);
		double cval  = _pt.x/nLen;
		double sval  = _pt.y/nLen;
		//<--
		SQVECTOR vec={0};
		vec.dx = cval;
		vec.dy = sval;
		vec.dz = 0.;
		CMath::NormalizeVector(vec);
		cval = acos(vec.dx);
		sval = asin(vec.dy);
		if(sval > 0)	para.SetY(cval);
		else		para.SetY(2*PI - cval);
		
		//<--
		POINT_T tmp=_pt;
		_pt.x = tmp.x*cos(para.y())    + tmp.y*sin(para.y());
		_pt.y = -(tmp.x*sin(para.y())) + tmp.y*cos(para.y());
		_pt.z = tmp.z;
		//-->
		
		_pt.x -= m_sfcTorus.nTorusRadius;
		
		tmp = _pt;
		_pt.y =  tmp.z;
		_pt.z = -tmp.y;
		
		para.SetX(CGeometry::GetRotatedAngleInXYPlane(_pt));
	}
	else
	{
		double u=0.,u1=0.,u2=m_nUEndAngle,tu=0.,v=0,v1=0,v2=2*PI,tv=0.;
		double du=u2/16,dv=v2/16;
		double min=0.,len=0.;
		do
		{
			POINT_T pt1=func((u1 + du*0.5),(v1 + dv*0.5));
			double dx=pt1.x - pt.x;
			double dy=pt1.y - pt.y;
			double dz=pt1.z - pt.z;
			min=len=dx*dx + dy*dy + dz*dz;
			for(u = u1;u < u2;u += du)
			{
				for(v = v1;v < v2;v += dv)
				{
					pt1 = func((u + du*0.5),(v + dv*0.5));
					dx  = pt1.x - pt.x;
					dy  = pt1.y - pt.y;
					dz  = pt1.z - pt.z;
					len = dx*dx + dy*dy + dz*dz;
					if(len < min){
						min = len;
						tu  = u;
						tv  = v;
					}
				}
			}
			u1 = tu;
			u2 = tu + du;
			v1 = tv;
			v2 = tv + dv;
			du/= 16.;
			dv/= 16;
		}while(min > TOLERANCE && ((u2 - u1) > TOLERANCE) && ((v2 - v1) > TOLERANCE));
		para.SetX(u1 + du);
		if(para.x() > 2*PI) para.SetX(2*PI);
		para.SetY(v1 + dv);
		if(para.y() > 2*PI) para.SetY(2*PI);
	}

	return para;
}

/**	\brief	The CTorusSurface::HasSegment function

	\param	plstEdge	a parameter of type list<EDGE>*
	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	bool	
*/
bool CTorusSurface::HasSegment(list<EDGE>* plstEdge,const POINT_T& pt1,const POINT_T& pt2)
{
	assert(plstEdge && "plstEdge is NULL");
	bool bRet=false;

	if(plstEdge)
	{
		for(list<EDGE>::iterator itr=plstEdge->begin();itr != plstEdge->end();++itr)
		{
			POINT_T pt[2]={0};

			pt[0].x = itr->para[0].u();
			pt[0].y = itr->para[0].v();
			pt[0].z = 0.;
			pt[1].x = itr->para[1].u();
			pt[1].y = itr->para[1].v();
			pt[1].z = 0.;
			//if(0 == memcmp(&(pt[0]),&pt1,sizeof(POINT_T))){
			if(IsSamePoint(pt[0],pt1))
			{
				//if(0 == memcmp(&(pt[1]),&pt2,sizeof(POINT_T))){
				if(IsSamePoint(pt[1],pt2))
				{
					bRet = true;
					break;
				}
			//}else if(0 == memcmp(&(pt[0]),&pt2,sizeof(POINT_T))){
			}
			else if(IsSamePoint(pt[0],pt2))
			{
				//if(0 == memcmp(&(pt[1]),&pt1,sizeof(POINT_T))){
				if(IsSamePoint(pt[1],pt1))
				{
					bRet = true;
					break;
				}
			}
		}
	}

	return bRet;
}

/**	\brief	The CTorusSurface::IsBoundaryEdge function

	\param	edge	a parameter of type const EDGE&

	\return	bool	
*/
bool CTorusSurface::IsBoundaryEdge(EDGE_T edge)
{
	POINT_T pt[2][2]={0,};

	pt[0][0].x = edge.pos[0].x();pt[0][0].y = edge.pos[0].y();
	pt[0][1].x = edge.pos[1].x();pt[0][1].y = edge.pos[1].y();
	for(int i=0;i < m_nBoundary;i++)
	{
		for(list<EDGE_T>::iterator itr=m_paryBoundary[i].plstEdge->begin();itr != m_paryBoundary[i].plstEdge->end();++itr)
		{
			pt[1][0].x = itr->pos[0].x();pt[1][0].y = itr->pos[0].y();
			pt[1][1].x = itr->pos[1].x();pt[1][1].y = itr->pos[1].y();
			if(IsSamePoint(pt[0][0],pt[1][0]) && IsSamePoint(pt[0][1],pt[1][1])) return true;
			if(IsSamePoint(pt[0][0],pt[1][1]) && IsSamePoint(pt[0][1],pt[1][0])) return true;
		}
	}

	return false;
}

/**	\brief	The CTorusSurface::IsOutboundTriangle function

	\param	nOutboundIndex	a parameter of type int&
	\param	BoundaryEdge	a parameter of type EDGE&
	\param	pDT	a parameter of type CDT*

	\return	int	
*/
int CTorusSurface::IsOutboundTriangle(int& nOutboundIndex,EDGE_T& BoundaryEdge,CDT* pDT)
{
	assert(pDT && "pDT is NULL");

	if(pDT)
	{
		RECT_T lhs = pDT->rect();

		POINT_T para[2]={0};
		for(int i=0;i < m_nBoundary;i++)
		{
			for(list<EDGE_T>::iterator itr=m_paryBoundary[i].plstEdge->begin();itr != m_paryBoundary[i].plstEdge->end();++itr)
			{
				para[0].x = itr->pos[0].x();
				para[0].y = itr->pos[0].y();
				para[0].z = 0.;
				para[1].x = itr->pos[1].x();
				para[1].y = itr->pos[1].y();
				para[1].z = 0.;

				RECT_T rhs = GetRectOf(2 , para);
				if(!CollideWith(lhs , rhs)) continue;

				if(pDT->HasEdge(para[0],para[1]))
				{
					if(!pDT->HasPosAtIndex(para[0],0) && !pDT->HasPosAtIndex(para[1],0))
					{
						if(CDT::Area(para[0],para[1],(*pDT)[0]) > 0.)
						{
							nOutboundIndex = 0;
							BoundaryEdge   = *itr;
							return 1;
						}
						//}else	return 0;
					}
					else if(!pDT->HasPosAtIndex(para[0],1) && !pDT->HasPosAtIndex(para[1],1))
					{
						if(CDT::Area(para[0],para[1],(*pDT)[1]) > 0.)
						{
							nOutboundIndex = 1;
							BoundaryEdge   = *itr;
							return 1;
						}
						//}else	return 0;
					}
					else if(!pDT->HasPosAtIndex(para[0],2) && !pDT->HasPosAtIndex(para[1],2))
					{
						if(CDT::Area(para[0],para[1],(*pDT)[2]) > 0.)
						{
							nOutboundIndex = 2;
							BoundaryEdge   = *itr;
							return 1;
						}
						//}else	return 0;
					}
				}	
			}
		}
	}

	return 0;
}

/**	\brief	The CTorusSurface::FindTriangleHasEdge function

	\param	nIndex	a parameter of type int&
	\param	plstDT	a parameter of type list<CDT*>*
	\param	edge	a parameter of type EDGE&

	\return	CDT*	
*/
CDT* CTorusSurface::FindTriangleHasEdge(int& nIndex,list<CDT*>* plstDT,EDGE_T& edge)
{
	CDT* pRet=NULL;

	POINT_T para[2]={0,};
	para[0].x = edge.pos[0].x();
	para[0].y = edge.pos[0].y();
	para[0].z = 0.;
	para[1].x = edge.pos[1].x();
	para[1].y = edge.pos[1].y();
	para[1].z = 0.;
	for(list<CDT*>::iterator itr=plstDT->begin();itr != plstDT->end();)
	{
		if((*itr)->m_bDeleted)
		{
			itr++;
			continue;
		}
		pRet = *itr;
		if(pRet->HasEdge(para[0],para[1]))
		{
			if(!pRet->HasPosAtIndex(para[0],0) && !pRet->HasPosAtIndex(para[1],0))
			{
				if(CDT::Area(para[0],para[1],(*pRet)[0]) > 0.)
				{
					nIndex = 0;
					return pRet;
				}else itr++;
			}
			else if(!pRet->HasPosAtIndex(para[0],1) && !pRet->HasPosAtIndex(para[1],1))
			{
				if(CDT::Area(para[0],para[1],(*pRet)[1]) > 0.)
				{
					nIndex = 1;
					return pRet;
				}else itr++;
			}
			else if(!pRet->HasPosAtIndex(para[0],2) && !pRet->HasPosAtIndex(para[1],2))
			{
				if(CDT::Area(para[0],para[1],(*pRet)[2]) > 0.)
				{
					nIndex = 2;
					return pRet;
				}else itr++;
			}
		}else	itr++;	

	}

	return NULL;
}

/**	\brief	The CTorusSurface::RemoveOutboundTriangles function

	\param	plstDT	a parameter of type list<CDT*>*

	\return	void	
*/
void CTorusSurface::RemoveOutboundTriangles(list<CDT*>* plstDT)
{
	assert(plstDT && "plstDT is NULL");
	if(plstDT)
	{
		queue<EDGE_T> queEdge;
		EDGE_T edge;
		
		for(list<CDT*>::iterator itr=plstDT->begin();itr != plstDT->end();)
		{
			POINT_T pt[3]={0};
			pt[0] = (**itr)[0];
			pt[1] = (**itr)[1];
			pt[2] = (**itr)[2];
			int ret=0,nOutboundIndex=0;
			EDGE_T BoundaryEdge;
			if(ret=IsOutboundTriangle(nOutboundIndex,BoundaryEdge,*itr))
			{
				if(1 == ret)
				{
					int nIndex[2]={0};
					
					nIndex[0] = (nOutboundIndex + 1)%3;
					nIndex[1] = (nOutboundIndex + 2)%3;
					
					EDGE_T edge;
					edge.pos[0].Set((**itr)[nOutboundIndex].x,(**itr)[nOutboundIndex].y , 0.);
					edge.pos[1].Set((**itr)[nIndex[0]].x,(**itr)[nIndex[0]].y , 0.);
					if(IsValidEdge(edge) && !IsSameEdge(edge,BoundaryEdge) && !IsBoundaryEdge(edge))
						queEdge.push(edge);
					edge.pos[0].Set((**itr)[nOutboundIndex].x,(**itr)[nOutboundIndex].y , 0.);
					edge.pos[1].Set((**itr)[nIndex[1]].x,(**itr)[nIndex[1]].y , 0.);
					if(IsValidEdge(edge) && !IsSameEdge(edge,BoundaryEdge) && !IsBoundaryEdge(edge))
						queEdge.push(edge);
				}
				(*itr)->m_bDeleted = true;
				itr++;
				//plstDT->erase(itr++);
			}else	itr++;
		}
		
		while(!queEdge.empty())
		{
			edge = queEdge.front(); queEdge.pop();
			POINT_T pt[2]={0};
			
			pt[0].x = edge.pos[0].x();
			pt[0].y = edge.pos[0].y();
			pt[0].z = 0;
			pt[1].x = edge.pos[1].x();
			pt[1].y = edge.pos[1].y();
			pt[1].z = 0;

			RECT_T lhs = GetRectOf(2 , pt);
			for(list<CDT*>::iterator itr = plstDT->begin();itr != plstDT->end();)
			{
				if((*itr)->m_bDeleted)
				{
					itr++;
					continue;
				}

				RECT_T rhs = (*itr)->rect();
				if(!CollideWith(lhs , rhs))
				{
					itr++;
					continue;
				}

				if((*itr)->HasEdge(pt[0],pt[1]))
				{
					if(!IsSamePoint((**itr)[0],pt[0]) && !IsSamePoint((**itr)[0],pt[1]))
					{
						edge.pos[0].Set((**itr)[0].x,(**itr)[0].y , 0.);
						edge.pos[1].Set((**itr)[1].x,(**itr)[1].y , 0.);
						if(!IsBoundaryEdge(edge)) queEdge.push(edge);
						edge.pos[0].Set((**itr)[0].x,(**itr)[0].y , 0.);
						edge.pos[1].Set((**itr)[2].x,(**itr)[2].y , 0.);
						if(!IsBoundaryEdge(edge)) queEdge.push(edge);
					}
					else if(!IsSamePoint((**itr)[1],pt[0]) && !IsSamePoint((**itr)[1],pt[1]))
					{
						edge.pos[0].Set((**itr)[1].x,(**itr)[1].y , 0.);
						edge.pos[1].Set((**itr)[0].x,(**itr)[0].y , 0.);
						if(!IsBoundaryEdge(edge)) queEdge.push(edge);
						edge.pos[0].Set((**itr)[1].x,(**itr)[1].y , 0.);
						edge.pos[1].Set((**itr)[2].x,(**itr)[2].y , 0.);
						if(!IsBoundaryEdge(edge)) queEdge.push(edge);
					}
					else if(!IsSamePoint((**itr)[2],pt[0]) && !IsSamePoint((**itr)[2],pt[1]))
					{
						edge.pos[0].Set((**itr)[2].x,(**itr)[2].y , 0.);
						edge.pos[1].Set((**itr)[0].x,(**itr)[0].y , 0.);
						if(!IsBoundaryEdge(edge)) queEdge.push(edge);
						edge.pos[0].Set((**itr)[2].x,(**itr)[2].y , 0.);
						edge.pos[1].Set((**itr)[1].x,(**itr)[1].y , 0.);
						if(!IsBoundaryEdge(edge)) queEdge.push(edge);
					}
					(*itr)->m_bDeleted = true;
					break;
					//plstDT->erase(itr++);
					//break;
				}else	itr++;
			}
		}
	}
}

/**	
	@brief	if delaunay triangle is bad , doesn't attach this to list

	@author	BHK

	@date	?

	@return	CTinyFacetContainer*
*/
CTinyFacetContainer* CTorusSurface::CreateFacets()
{
	CTinyFacetContainer* pFacetContainer = new CTinyFacetContainer;
	if(NULL == pFacetContainer) return NULL;
	
	vector<CIsPoint2d*> PointEntry;
	vector<long> oEdgeIndexEntry;
	
	//! create DTs
	const int NUM_OF_SEGMENTS=CCurve3d::GetNumOfSegments();
	int nPoints=NUM_OF_SEGMENTS+1 , i = 0;
	
	///
	for(i = 0;i < (NUM_OF_SEGMENTS + 1);i++)
	{
		for(int j = 0;j < (NUM_OF_SEGMENTS + 1);j++)
		{
			PointEntry.push_back(new CIsPoint2d(CSurface3D::para[nPoints*i + j].u() , CSurface3D::para[nPoints*i + j].v()));
		}
	}
	///

	static int _nCount=0;
	_nCount    =0;

	CMeshPos2d pos[2];
	for(i=0;i < m_nBoundary;++i)
	{
		for(list<EDGE_T>::iterator itr=m_paryBoundary[i].plstEdge->begin();itr != m_paryBoundary[i].plstEdge->end();++itr)
		{
			long nIndex[2]={-1,-1} , cnt=0;
			CIsPoint2d* ppt2d[2] = {NULL , NULL};
			for(vector<CIsPoint2d*>::iterator jtr = PointEntry.begin();jtr != PointEntry.end();++jtr,++cnt)
			{
				POINT_T pt[2];
				pt[0].x = (*jtr)->x();
				pt[0].y = (*jtr)->y();
				pt[0].z = 0.f;
				pt[1].x = itr->pos[0].x();
				pt[1].y = itr->pos[0].y();
				pt[1].z = 0.f;
				if((NULL == ppt2d[0]) && IsSamePoint(pt[0] , pt[1]))
				{
					ppt2d[0] = *jtr;
					nIndex[0]= cnt;
				}

				pt[1].x   = itr->pos[1].x();
				pt[1].y   = itr->pos[1].y();
				pt[1].z = 0.f;
				if((NULL == ppt2d[1]) && IsSamePoint(pt[0] , pt[1]))
				{
					ppt2d[1] = *jtr;
					nIndex[1]= cnt;
				}

				if((NULL != ppt2d[0]) && (NULL != ppt2d[1])) break;
			}
			if(NULL == ppt2d[0])
			{
				ppt2d[0] = new CIsPoint2d(itr->pos[0].x() , itr->pos[0].y());
				nIndex[0]= PointEntry.size();
				PointEntry.push_back(ppt2d[0]);
			}
			if(NULL == ppt2d[1])
			{
				ppt2d[1] = new CIsPoint2d(itr->pos[1].x() , itr->pos[1].y());
				nIndex[1]= PointEntry.size();
				PointEntry.push_back(ppt2d[1]);
			}
			//! set clockwise.
			oEdgeIndexEntry.push_back(nIndex[0]);
			oEdgeIndexEntry.push_back(nIndex[1]);
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	// TEST CODE.
	/*
	CTinyTorusSurfTester tester(true);
	tester.m_nSegments = NUM_OF_SEGMENTS;
	tester.m_nRows = nPoints;
	tester.m_nBoundaries = _nCount;
	tester.OutputTestString();
	*/
	//////////////////////////////////////////////////////////////////////////
	
	/*
	list<CDT*>* plstDT=NULL;
	try
	{		
		///plstDT=pInst->Mesh(lstDT,CSurface3D::pt,nIndex,CLibMesh2d::lnBoundary,_nCount);
	}
	catch(...)
	{
		delete pFacetContainer;
		string str;
		str += __FILE__ + __LINE__;
		//str += " : "; str += ex.what();
		OutputDebugString(str.c_str());
		return NULL;
	}
	*/

	///if(plstDT)
	{
		list<CDelaunayTri*>* pListDT = aTech_Triangulate(&(PointEntry[0]) , PointEntry.size() , 
			&(oEdgeIndexEntry[0]) , oEdgeIndexEntry.size()*0.5 , 15 , false);

		if(NULL != pListDT)
		{
			///POINT_T pts[3];
			for(list<CDelaunayTri*>::iterator itr = pListDT->begin();itr != pListDT->end();++itr)
			{
				CFacet* pFacet=NULL;

				const long lSize = oEdgeIndexEntry.size()*0.5;
				bool bShow[3] = {false , false , false};
				for(long l = 0;l < lSize;++l)
				{
					CIsPoint2d* ep1 = PointEntry[2*l];
					CIsPoint2d* ep2 = PointEntry[2*l + 1];
					
					CIsPoint2d* p1 = (*itr)->vertex(0)->pt();
					CIsPoint2d* p2 = (*itr)->vertex(1)->pt();
					CIsPoint2d* p3 = (*itr)->vertex(2)->pt();
					
					if(((ep1 == p1) && (ep2 == p2)) || ((ep2 == p1) && (ep1 == p2)))
					{
						bShow[0] = true;
					}
					else if(((ep1 == p2) && (ep2 == p3)) || ((ep2 == p2) && (ep1 == p3)))
					{
						bShow[1] = true;
					}
					else if(((ep1 == p3) && (ep2 == p1)) || ((ep2 == p3) && (ep1 == p1)))
					{
						bShow[2] = true;
					}

					if(pFacet = new CFacet(func(p1->x() , p1->y()),
						func(p2->x() , p2->y()),
						func(p3->x() , p3->y())))
					{
						pFacet->m_bShow[0] = bShow[0];
						pFacet->m_bShow[1] = bShow[1];
						pFacet->m_bShow[2] = bShow[2];
						pFacetContainer->Add(pFacet);
					}
				}
			}
		}

		/*
		for(list<CDT*>::iterator itr=plstDT->begin();itr != plstDT->end();++itr)
		{
			CFacet* pFacet=NULL;
			
			if(0)
			{
			}
			else
			{
				if(!((*itr)->m_bDeleted))
				{
					if(pFacet = new CFacet(func((**itr)[0].x,(**itr)[0].y),
						func((**itr)[1].x,(**itr)[1].y),
						func((**itr)[2].x,(**itr)[2].y)))
					{
						pFacetContainer->Add(pFacet);
					}
				}
			}
		}
		*/
	}
	
	{
		for(vector<CIsPoint2d*>::iterator jtr = PointEntry.begin();jtr != PointEntry.end();++jtr)
		{
			delete (*jtr);
		}
	}
	/*{
		for(vector<CDelaunayTriEdge*>::iterator jtr = EdgeEntry.begin();jtr != EdgeEntry.end();++jtr)
		{
			delete (*jtr);
		}
	}*/
	
	return pFacetContainer;
}

/**	\brief	The CTorusSurface::GetFacets function


	\return	list<CFacet*>*	
*/
list<CFacet*>* CTorusSurface::GetFacets()
{
	return &m_lstFacet;
}