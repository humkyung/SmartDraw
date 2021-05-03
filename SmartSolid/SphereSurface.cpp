#include "StdAfx.h"
#include <math.h>
#include <graphics\geometry.h>
#include <graphics/arc3d.h>
#include "libmesh/libmesh2d.h"
#include "SphereSurface.h"
#include "TinyTorusSurfTester.h"

#include <IsPoint2d.h>
#include <DelaunayMesh.h>

#include <fstream>
#include <sstream>
#include <queue>
using namespace std;
using namespace mesh;

/**	\brief
*/
CSphereSurface::CSphereSurface()
{
	m_sfcSphere.ptOrigin.x = 0.;
	m_sfcSphere.ptOrigin.y = 0.;
	m_sfcSphere.ptOrigin.z = 0.;
	m_sfcSphere.nRadius    = 1.;
	//m_sfcSphere.pBoundary  = NULL;
	m_nBoundary    = 0;
	m_paryBoundary = NULL;

	m_bReversed = false;

	OnCreate();
}

/**	\brief
*/
CSphereSurface::CSphereSurface(SQSPHERESURFACE& spheresurface,bool bReversed)
{
	memcpy(&m_sfcSphere,&spheresurface,sizeof(SQSPHERESURFACE));
	m_bReversed = bReversed;
	m_nBoundary    = 0;
	m_paryBoundary = NULL;

	OnCreate();
}

/**	\brief
*/
CSphereSurface::~CSphereSurface()
{
	//for(list<KPolyCurve*>::iterator itrCurve=m_lstBoundaryCurve.begin();itrCurve != m_lstBoundaryCurve.end();){
	//	delete (*itrCurve);
	//	m_lstBoundaryCurve.erase(itrCurve++);
	//}

	if(m_paryBoundary)
	{
		for(int i=0;i < m_nBoundary;i++)
		{
			delete m_paryBoundary[i].plstEdge;
		}

		::free((void*)m_paryBoundary);
	}
}

/**	
	@brief	The CSphereSurface::OnCreate function

	@author	BHK

	@date	?

	@return	bool	
*/
bool CSphereSurface::OnCreate()
{
	int NUM_OF_SEGMENTS=CCurve3d::GetNumOfSegments();
	m_nFacets = NUM_OF_SEGMENTS*NUM_OF_SEGMENTS;
	
	double du=PI/(NUM_OF_SEGMENTS),dv=(2*PI)/(NUM_OF_SEGMENTS);
	int nIndex=0;
	for(int i=0;i < NUM_OF_SEGMENTS;i++)
	{
		for(int j=0;j < (NUM_OF_SEGMENTS);j++)
		{
			CSurface3D::para[nIndex++].Set(i*du,j*dv);
			//CSurface3D::para[nIndex++].v= j*dv;
		}
		CSurface3D::para[nIndex++].Set(i*du,2*PI);
		//CSurface3D::para[nIndex++].v= 2*PI;
	}
	for(int j = 0;j < NUM_OF_SEGMENTS;j++)
	{
		CSurface3D::para[nIndex++].Set(PI,j*dv);
		//CSurface3D::para[nIndex++].v= j*dv;
	}
	CSurface3D::para[nIndex++].Set(PI,2*PI);
	//CSurface3D::para[nIndex++].v= 2*PI;

	if(m_sfcSphere.pBoundary && m_sfcSphere.pBoundary[0])
	{
		CreateBoundary(m_sfcSphere.pBoundary[0]);
	}

	return true;
}

static CSurfacePara p[256];

/**	\brief	The CSphereSurface::CreateBoundaryCurvePoint function

	\param	aryPara[]	a parameter of type CSurfacePara
	\param	nPoints	a parameter of type int&

	\return	void	
*/
void CSphereSurface::CreateBoundaryCurvePoint(CMeshPos2d aryPara[],int& nPoints)
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
			{ // may be over end of 'u' parameter.
				if(du > 0.)
				{ // may be over 0 
					double t=-aryPara[i].x()/(aryPara[i + 1].x() - aryPara[i].x());
					double v=(aryPara[i + 1].y() - aryPara[i].y())*t + aryPara[i].y();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(0.,v , 0.);
					i++;
					nPoints++;
					
					t=(PI - aryPara[i].x())/(aryPara[i + 1].x() - aryPara[i].x());
					v=(aryPara[i + 1].y() - aryPara[i].y())*t + aryPara[i].y();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(PI,v , 0.);
					i++;
					nPoints++;
				}
				else if(du < 0.)
				{ // may be over phi
					double t=(PI - aryPara[i].x())/(aryPara[i + 1].x() - aryPara[i].x());
					double v=(aryPara[i + 1].y() - aryPara[i].y())*t + aryPara[i].y();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(PI,v , 0.);
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
			{ // may be over end of 'v' parameter.
				if(dv > 0.)
				{ // may be over 0
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
				{ // may be over 2*phi
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
	@brief	The CSphereSurface::CreateBoundary function

	@author	BHK

	@date	?

	@param	pPrimitive	a parameter of type PSQPRIMITIVE

	@return	void	
*/
void CSphereSurface::CreateBoundary(PSQPRIMITIVE pPrimitive)
{
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive)
	{
		int nCount=0;
		for(PSQPRIMITIVE ptr=pPrimitive;ptr;ptr = ptr->next,nCount++);

		if(m_paryBoundary = (BOUNDARY_T*)calloc(nCount,sizeof(BOUNDARY_T)))
		{
			for(PSQPRIMITIVE ptr=pPrimitive;ptr;ptr = ptr->next)
			{
				if(m_bReversed) ptr->hdr.attr.reversed = !ptr->hdr.attr.reversed;
				switch(ptr->hdr.id)
				{
					case ENM_ARC:
					{
						CArc3d arc(ptr->body.arc);
						arc.CreateSegments();
						int nPoints = arc.GetNumOfPoints();
						for(int i=0;i < nPoints;i++)
						{
							CLibMesh2d::pos[i]=func(CCurve3d::pt(i));
							CLibMesh2d::pos[i].Set(CLibMesh2d::pos[i].x(),CLibMesh2d::pos[i].y() , 0.);
						}
						int i=0;
						for(i=0;i < nPoints - 1;i++)
						{
							if(p[i].v() < 0)
							{
								if(i > 0)
								{
									memcpy(&CLibMesh2d::pos[i+1],&CLibMesh2d::pos[i],sizeof(CMeshPos2d)*(nPoints - i + 1));
									CLibMesh2d::pos[i].Set(CLibMesh2d::pos[i+1].x(),CLibMesh2d::pos[i+1].y() , 0.);
									nPoints++;
									i++;
								}
								if(i < nPoints)
								{
									CLibMesh2d::pos[i].SetY(CLibMesh2d::pos[i+1].y());
								}
							}
						}
						if(CLibMesh2d::pos[i].y() < 0) CLibMesh2d::pos[i].SetY(CLibMesh2d::pos[i-1].y());
						CreateBoundaryCurvePoint(CLibMesh2d::pos,nPoints);
							
						if(ptr->hdr.attr.reversed)
						{
							reverse(&CLibMesh2d::pos[0],&CLibMesh2d::pos[nPoints]);
							m_paryBoundary[m_nBoundary].vecNorm.dx = -ptr->body.arc.vecNorm.dx;
							m_paryBoundary[m_nBoundary].vecNorm.dy = -ptr->body.arc.vecNorm.dy;
							m_paryBoundary[m_nBoundary].vecNorm.dz = -ptr->body.arc.vecNorm.dz;
						}
						else
						{
							m_paryBoundary[m_nBoundary].vecNorm=ptr->body.arc.vecNorm;
						}

						m_paryBoundary[m_nBoundary].plstEdge = new list<EDGE_T>;
						if(m_paryBoundary[m_nBoundary].plstEdge)
						{
							EDGE_T edge;
							for(i = 0;i < nPoints - 1;i++)
							{
								if((fabs(CLibMesh2d::pos[i+1].x() - CLibMesh2d::pos[i].x()) < PI) && (fabs(CLibMesh2d::pos[i+1].y() - CLibMesh2d::pos[i].y()) < 0.5*PI))
								{
									edge.pos[0] = CLibMesh2d::pos[i];
									edge.pos[1] = CLibMesh2d::pos[i+1];
									m_paryBoundary[m_nBoundary].plstEdge->push_back(edge);
								}
							}
							m_nBoundary++;
						}
						    }
					break;
				}
			}
		}
	}
}

/**	\brief	The CSphereSurface::func function

	\param	para	a parameter of type CSurfacePara&

	\remarks	0 <= u <= pi , 0 <= v <= 2*pi

	\return	POINT_T	
*/
POINT_T CSphereSurface::func(CMeshPos2d& para)
{
	POINT_T ptRet={0.,0.,0.};

	ptRet.x = /*m_sfcSphere.ptOrigin.x + */m_sfcSphere.nRadius*cos(para.x());
	ptRet.y = /*m_sfcSphere.ptOrigin.y + */m_sfcSphere.nRadius*sin(para.x())*cos(para.y());
	ptRet.z = /*m_sfcSphere.ptOrigin.z + */m_sfcSphere.nRadius*sin(para.x())*sin(para.y());

	double alpha=0,beta=0;
	CGeometry::GetRotatedAngleOfAxis(m_sfcSphere.vecNorm,alpha,beta);
	ptRet = CGeometry::RotateAboutYXAxis(ptRet,-beta,-alpha);
	ptRet.x += m_sfcSphere.ptOrigin.x;
	ptRet.y += m_sfcSphere.ptOrigin.y;
	ptRet.z += m_sfcSphere.ptOrigin.z;

	return ptRet;
}

/**	\brief	The CSphereSurface::func function

	\param	u	a parameter of type double
	\param	v	a parameter of type double

	\remarks	0 <= u <= pi , 0 <= v <= 2*pi

	\return	POINT_T	
*/
POINT_T CSphereSurface::func(double u,double v)
{
	POINT_T ptRet={0.,0.,0.};

	ptRet.x = /*m_sfcSphere.ptOrigin.x + */m_sfcSphere.nRadius*cos(u);
	ptRet.y = /*m_sfcSphere.ptOrigin.y + */m_sfcSphere.nRadius*sin(u)*cos(v);
	ptRet.z = /*m_sfcSphere.ptOrigin.z + */m_sfcSphere.nRadius*sin(u)*sin(v);

	double alpha=0,beta=0;
	CGeometry::GetRotatedAngleOfAxis(m_sfcSphere.vecNorm,alpha,beta);
	ptRet = CGeometry::RotateAboutYXAxis(ptRet,-beta,-alpha);
	ptRet.x += m_sfcSphere.ptOrigin.x;
	ptRet.y += m_sfcSphere.ptOrigin.y;
	ptRet.z += m_sfcSphere.ptOrigin.z;

	return ptRet;
}

/**	
	@brief	The CSphereSurface::func function

	@author	BHK

	@date	?

	@param	pt	a parameter of type const POINT_T&

	@remarks	0 <= u <= pi , 0 <= v <= 2*pi

	@return	CSurfacePara	
*/
CMeshPos2d CSphereSurface::func(const POINT_T& pt)
{
	#define TOLERANCE 0.00000001
	CMeshPos2d para;

	POINT_T _pt={0,};
	_pt.x = pt.x - m_sfcSphere.ptOrigin.x;
	_pt.y = pt.y - m_sfcSphere.ptOrigin.y;
	_pt.z = pt.z - m_sfcSphere.ptOrigin.z;
	
	double alpha=0,beta=0;
	CGeometry::GetRotatedAngleOfAxis(m_sfcSphere.vecNorm,alpha,beta);
	_pt   = CGeometry::RotateAboutXYAxis(_pt,alpha,beta);

	//if(CGeometry::IsSameValue(_pt.x,m_sfcSphere.nRadius,0.0000000001)){
	if(IsSameValue(_pt.x,m_sfcSphere.nRadius))
	{
		para.Set(0,-1 , 0.);
	}
	else if(IsSameValue(_pt.x,-m_sfcSphere.nRadius))
	{
		//}else if(CGeometry::IsSameValue(_pt.x,-m_sfcSphere.nRadius,0.0000000001)){
		para.Set(PI,-1 , 0.);
	}
	else
	{
		double d=acos(_pt.x/m_sfcSphere.nRadius);
		para.SetX(d);
		double cval  = _pt.y/(m_sfcSphere.nRadius*sin(para.x()));
		double sval  = _pt.z/(m_sfcSphere.nRadius*sin(para.x()));
		if(sval > 0.)
		{
			d = acos(cval);
			para.SetY(d);
		}
		else
		{
			d = 2*PI - acos(cval);
			para.SetY(d);
		}
	}
	
	return para;
}

/**	\brief	The CSphereSurface::refine function

	\param	aryPara[]	a parameter of type CSurfacePara
	\param	para1	a parameter of type CSurfacePara&
	\param	para2	a parameter of type CSurfacePara&

	\return	int	
*/
int CSphereSurface::refine(CSurfacePara aryPara[],CSurfacePara& para1,CSurfacePara& para2)
{
	int NUM_OF_SEGMENTS=CCurve3d::GetNumOfSegments();
//	#define TOLERANCE 0.001
	#define STEP ((2*PI)/(NUM_OF_SEGMENTS))
	double du=para2.u() - para1.u();
	double dv=para2.v() - para1.v();

	int nIndex=0;
	aryPara[nIndex++].Set(para1.u(),para1.v());
	//aryPara[nIndex++].v() = para1.v();
	if(fabs(du) >= TOLERANCE)
	{
		double u1=(para1.u() < para2.u()) ? para1.u() : para2.u();
		double u2=(para2.u() > para1.u()) ? para2.u() : para1.u();
		double v1=(para1.v() < para2.v()) ? para1.v() : para2.v();
		double v2=(para2.v() > para1.v()) ? para2.v() : para1.v();

		double v=0.;
		for(double u=((u1+STEP)/STEP)*STEP;u < (u2/STEP)*STEP;u+=STEP)
		{
			v = v1 + (u - u1)*fabs(dv)/fabs(du);
			aryPara[nIndex++].Set(u,v);
			//aryPara[nIndex++].v = v;
		}
	}

	if(fabs(dv) >= TOLERANCE)
	{
		double u1=(para1.u() < para2.u()) ? para1.u() : para2.u();
		double u2=(para2.u() > para1.u()) ? para2.u() : para1.u();
		double v1=(para1.v() < para2.v()) ? para1.v() : para2.v();
		double v2=(para2.v() > para1.v()) ? para2.v() : para1.v();

		double u=0.;
		for(double v=((v1+STEP)/STEP)*STEP;v < (v2/STEP)*STEP;v+=STEP)
		{
			u = u1 + (v - v1)*fabs(du)/fabs(dv);
			aryPara[nIndex++].Set(u,v);
			//aryPara[nIndex++].v = v;
		}
	}
	aryPara[nIndex++].Set(para2.u(),para2.v());
	//aryPara[nIndex++].v = para2.v;

	return nIndex;
}

/**	\brief	The CSphereSurface::HasSegment function

	\param	plstEdge	a parameter of type list<EDGE>*
	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	bool	
*/
bool CSphereSurface::HasSegment(list<EDGE_T>* plstEdge,const POINT_T& pt1,const POINT_T& pt2)
{
	assert(plstEdge && "plstEdge is NULL");
	bool bRet=false;

	if(plstEdge)
	{
		for(list<EDGE_T>::iterator itr=plstEdge->begin();itr != plstEdge->end();++itr)
		{
			POINT_T pt[2]={0};

			pt[0].x = itr->pos[0].x();
			pt[0].y = itr->pos[0].y();
			pt[0].z = 0.;
			pt[1].x = itr->pos[1].x();
			pt[1].y = itr->pos[1].y();
			pt[1].z = 0.;
			if(IsSamePoint(pt[0],pt1))
			{
				if(IsSamePoint(pt[1],pt2))
				{
					bRet = true;
					break;
				}
			}
			else if(IsSamePoint(pt[0],pt2))
			{
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

/**	\brief	The CSphereSurface::IsOutboundTriangle function

	\param	nOutboundIndex	a parameter of type int&
	\param	pDT	a parameter of type CDT*

	\return	int	
*/
int CSphereSurface::IsOutboundTriangle(int& nOutboundIndex,CDT* pDT)
{
	assert(pDT && "pDT is NULL");

	if(pDT)
	{
		POINT_T para[3]={0};

		para[0] = (*pDT)[0];
		para[1] = (*pDT)[1];
		para[2] = (*pDT)[2];

		POINT_T pt[3]={0};
		pt[0] = func(para[0].x,para[0].y);
		pt[1] = func(para[1].x,para[1].y);
		pt[2] = func(para[2].x,para[2].y);
		for(int i=0;i < m_nBoundary;i++)
		{
			if(HasSegment(m_paryBoundary[i].plstEdge,para[0],para[1]))
			{
				VECTOR_T vecNorm=m_paryBoundary[i].vecNorm;
				double d[2]={0},v[2]={0};
				d[0] = -(vecNorm.dx*pt[0].x + vecNorm.dy*pt[0].y + vecNorm.dz*pt[0].z);
				d[1] = -(vecNorm.dx*pt[1].x + vecNorm.dy*pt[1].y + vecNorm.dz*pt[1].z);
				v[0] = vecNorm.dx*pt[2].x + vecNorm.dy*pt[2].y + vecNorm.dz*pt[2].z + d[0];
				v[1] = vecNorm.dx*pt[2].x + vecNorm.dy*pt[2].y + vecNorm.dz*pt[2].z + d[1];
				nOutboundIndex = 2;

				if((v[0] < 0.) && (v[1] < 0.))	return 1;
				else if(v[0]*v[1] < 0.)	return 2;
				else			return 0;
			}
			else if(HasSegment(m_paryBoundary[i].plstEdge,para[1],para[2]))
			{
				VECTOR_T vecNorm=m_paryBoundary[i].vecNorm;
				double d[2]={0},v[2]={0};
				d[0] = -(vecNorm.dx*pt[1].x + vecNorm.dy*pt[1].y + vecNorm.dz*pt[1].z);
				d[1] = -(vecNorm.dx*pt[2].x + vecNorm.dy*pt[2].y + vecNorm.dz*pt[2].z);
				v[0] = vecNorm.dx*pt[0].x + vecNorm.dy*pt[0].y + vecNorm.dz*pt[0].z + d[0];
				v[1] = vecNorm.dx*pt[0].x + vecNorm.dy*pt[0].y + vecNorm.dz*pt[0].z + d[1];
				nOutboundIndex = 0;

				if((v[0] < 0.) && (v[1] < 0.))	return 1;
				else if(v[0]*v[1] < 0.)	return 2;
				else			return 0;
			}
			else if(HasSegment(m_paryBoundary[i].plstEdge,para[2],para[0]))
			{
				VECTOR_T vecNorm=m_paryBoundary[i].vecNorm;
				double d[2]={0},v[2]={0};
				d[0] = -(vecNorm.dx*pt[2].x + vecNorm.dy*pt[2].y + vecNorm.dz*pt[2].z);
				d[1] = -(vecNorm.dx*pt[0].x + vecNorm.dy*pt[0].y + vecNorm.dz*pt[0].z);
				v[0] = vecNorm.dx*pt[1].x + vecNorm.dy*pt[1].y + vecNorm.dz*pt[1].z + d[0];
				v[1] = vecNorm.dx*pt[1].x + vecNorm.dy*pt[1].y + vecNorm.dz*pt[1].z + d[1];
				nOutboundIndex = 1;

				if((v[0] < 0.) && (v[1] < 0.))	return 1;
				else if(v[0]*v[1] < 0.)	return 2;
				else			return 0;
			}
		}
	}

	return 0;
}

/**	\brief	The CSphereSurface::RemoveOutboundTriangles function

	\param	plstDT	a parameter of type list<CDT*>*

	\return	void	
*/
void CSphereSurface::RemoveOutboundTriangles(list<CDT*>* plstDT)
{
	assert(plstDT && "plstDT is NULL");
	if(plstDT)
	{
		queue<EDGE> queEdge;

		for(list<CDT*>::iterator itr=plstDT->begin();itr != plstDT->end();)
		{
			POINT_T pt[3]={0};
			pt[0] = (**itr)[0];
			pt[1] = (**itr)[1];
			pt[2] = (**itr)[2];
			
			CDT* pDT=*itr;
			int ret=0,nOutboundIndex=0;
			if(ret=IsOutboundTriangle(nOutboundIndex,*itr))
			{
				if(1 == ret)
				{
					int nIndex[2]={0};

					nIndex[0] = (nOutboundIndex + 1)%3;
					nIndex[1] = (nOutboundIndex + 2)%3;
					
					EDGE edge;
					edge.para[0].Set((**itr)[nOutboundIndex].x,(**itr)[nOutboundIndex].y);
					//edge.para[0].v = (**itr)[nOutboundIndex].y;
					edge.para[1].Set((**itr)[nIndex[0]].x,(**itr)[nIndex[0]].y);
					//edge.para[1].v = (**itr)[nIndex[0]].y;
					queEdge.push(edge);
					edge.para[0].Set((**itr)[nOutboundIndex].x,(**itr)[nOutboundIndex].y);
					//edge.para[0].v = (**itr)[nOutboundIndex].y;
					edge.para[1].Set((**itr)[nIndex[1]].x,(**itr)[nIndex[1]].y);
					//edge.para[1].v = (**itr)[nIndex[1]].y;
					queEdge.push(edge);
				}
				plstDT->erase(itr++);
			}else	itr++;
		}

		EDGE edge;
		while(!queEdge.empty())
		{
			edge=queEdge.front();queEdge.pop();
			POINT_T pt[2]={0};

			pt[0].x = edge.para[0].u();
			pt[0].y = edge.para[0].v();
			pt[0].z = 0;
			pt[1].x = edge.para[1].u();
			pt[1].y = edge.para[1].v();
			pt[1].z = 0;
			RECT_T lhs = GetRectOf(2 , pt);
			for(list<CDT*>::iterator itr = plstDT->begin();itr != plstDT->end();)
			{
				RECT_T rhs = (*itr)->rect();
				if(!CollideWith(lhs , rhs))
				{
					itr++;
					continue;
				}
				if((*itr)->HasEdge(pt[0],pt[1]))
				{
					if(	(((**itr)[0].x != pt[0].x) || ((**itr)[0].y != pt[0].y)) &&
						(((**itr)[0].x != pt[1].x) || ((**itr)[0].y != pt[1].y)))
					{
						edge.para[0].Set((**itr)[0].x,(**itr)[0].y);
						//edge.para[0].v = (**itr)[0].y;
						edge.para[1].Set((**itr)[1].x,(**itr)[1].y);
						//edge.para[1].v = (**itr)[1].y;
						queEdge.push(edge);
						edge.para[0].Set((**itr)[0].x,(**itr)[0].y);
						//edge.para[0].v = (**itr)[0].y;
						edge.para[1].Set((**itr)[2].x,(**itr)[2].y);
						//edge.para[1].v = (**itr)[2].y;
						queEdge.push(edge);
					}
					else if((((**itr)[1].x != pt[0].x) || ((**itr)[1].y != pt[0].y)) &&
						 (((**itr)[1].x != pt[1].x) || ((**itr)[1].y != pt[1].y)))
					{
						edge.para[0].Set((**itr)[1].x,(**itr)[1].y);
						//edge.para[0].v = (**itr)[1].y;
						edge.para[1].Set((**itr)[0].x,(**itr)[0].y);
						//edge.para[1].v = (**itr)[0].y;
						queEdge.push(edge);
						edge.para[0].Set((**itr)[1].x,(**itr)[1].y);
						//edge.para[0].v = (**itr)[1].y;
						edge.para[1].Set((**itr)[2].x,(**itr)[2].y);
						//edge.para[1].v = (**itr)[2].y;
						queEdge.push(edge);
					}
					else if((((**itr)[2].x != pt[0].x) || ((**itr)[2].y != pt[0].y)) &&
						 (((**itr)[2].x != pt[1].x) || ((**itr)[2].y != pt[1].y)))
					{
						edge.para[0].Set((**itr)[2].x,(**itr)[2].y);
						//edge.para[0].v = (**itr)[2].y;
						edge.para[1].Set((**itr)[0].x,(**itr)[0].y);
						//edge.para[1].v = (**itr)[0].y;
						queEdge.push(edge);
						edge.para[0].Set((**itr)[2].x,(**itr)[2].y);
						//edge.para[0].v = (**itr)[2].y;
						edge.para[1].Set((**itr)[1].x,(**itr)[1].y);
						//edge.para[1].v = (**itr)[1].y;
						queEdge.push(edge);
					}
					plstDT->erase(itr++);
					break;
				}else	itr++;
			}
		}
	}
}


/**	
	@brief	The CSphereSurface::CreateFacets function

	@author	BHK

	@date	?

	@param

	@return	CTinyFacetContainer*
*/
CTinyFacetContainer* CSphereSurface::CreateFacets()
{
	CTinyFacetContainer* pFacetContainer=new CTinyFacetContainer;
	if(NULL == pFacetContainer) return NULL;

	vector<CIsPoint2d*> PointEntry;
	vector<long> oEdgeIndexEntry;
	//! create points
	const int NUM_OF_SEGMENTS = CCurve3d::GetNumOfSegments();
	int nPoints=NUM_OF_SEGMENTS + 1;
	for(int i = 0;i < (NUM_OF_SEGMENTS + 1);++i)
	{
		for(int j = 0;j < (NUM_OF_SEGMENTS + 1);++j)
		{
			PointEntry.push_back(new CIsPoint2d(CSurface3D::para[nPoints*i + j].u() , CSurface3D::para[nPoints*i + j].v()));
		}
	}
	
	static int _nCount=0;
	_nCount    =0;
	
	CMeshPos2d pos[2];
	for(int i = 0;i < m_nBoundary;i++)
	{
		for(list<EDGE_T>::iterator itr=m_paryBoundary[i].plstEdge->begin();itr != m_paryBoundary[i].plstEdge->end();++itr)
		{
			long nIndex[2]={-1,-1} , cnt = 0;
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

	/*
	ofstream ofile("d:\\shpere.txt");
	for(vector<CDelaunayTriEdge*>::iterator itr = EdgeEntry.begin();itr != EdgeEntry.end();++itr)
	{
		ofile << "L " << (*itr)->point(0)->x() << "," << (*itr)->point(0)->y() << " " << (*itr)->point(1)->x() << "," << (*itr)->point(1)->y() << " " << std::endl;
	}
	ofile << std::endl;
	ofile.close();
	*/
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
	list<CDT*>* plstDT=pInst->Mesh(lstDT,CSurface3D::pt,nIndex,CLibMesh2d::lnBoundary,_nCount);
	if(plstDT)
	{
		RemoveOutboundTriangles(plstDT);
		
		for(list<CDT*>::iterator itr=plstDT->begin();itr != plstDT->end();++itr)
		{
			CFacet* pFacet=NULL;
			
			//if(!((*itr)->IsBad())){
				if(pFacet = new CFacet(func((**itr)[0].x,(**itr)[0].y),
					func((**itr)[1].x,(**itr)[1].y),
					func((**itr)[2].x,(**itr)[2].y)))
				{
					pFacetContainer->Add(pFacet);
				}
			//}
		}
	}
	*/

	list<CDelaunayTri*>* pListDT = aTech_Triangulate(&(PointEntry[0]) , PointEntry.size() ,
		(!oEdgeIndexEntry.empty() ? &(oEdgeIndexEntry[0]) : NULL) , oEdgeIndexEntry.size()*0.5 , 15 , false);
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

	//! delete already created points and edges.
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
	//!

	return pFacetContainer;
}
