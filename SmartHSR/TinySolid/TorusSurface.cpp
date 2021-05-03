#include "stdafx.h"
#include <math.h>
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include <graphics/arc3d.h>
#include <graphics/circle3d.h>
#include "PolyCurve.h"
#include "libmesh/libmesh2d.h"
#include "TorusSurface.h"

#include "OpenSolid.h"

#include <queue>
using namespace std;
using namespace mesh;

/**	\brief
*/
CTorusSurface::CTorusSurface(){
	m_sfcTorus.ptOrigin.x   = 0.;
	m_sfcTorus.ptOrigin.y   = 0.;
	m_sfcTorus.ptOrigin.z   = 0.;
	m_sfcTorus.nTorusRadius = 10.;
	m_sfcTorus.nTubeRadius  = 1.;
	m_sfcTorus.pBoundary  = NULL;
	
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
CTorusSurface::CTorusSurface(SQTORUSSURFACE& torussurface,bool bReversed){
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
CTorusSurface::~CTorusSurface(){
	for(list<CFacet*>::iterator itr=m_lstFacet.begin();itr != m_lstFacet.end();){
		delete (*itr);
		m_lstFacet.erase(itr++);
	}

	if(m_paryBoundary){
		for(int i=0;i < m_nBoundary;i++){
			delete m_paryBoundary[i].plstEdge;
		}

		if(m_paryBoundary) free((void*)m_paryBoundary);
	}
}

/**	\brief	The CTorusSurface::OnCreate function


	\return	bool	
*/
bool CTorusSurface::OnCreate(){
	double nTorusRadius= fabs(m_sfcTorus.nTorusRadius);
	double nTubeRadius = fabs(m_sfcTorus.nTubeRadius);

	if(nTorusRadius >= nTubeRadius){
		m_nUStartAngle = 0.;
		m_nUEndAngle   = 2*PI;
		m_nUOffsetAngle= 0.;
	}else if(m_sfcTorus.nTorusRadius >= 0.){
		double phi  = acos(nTorusRadius/nTubeRadius);
		double theta= PI*0.5 - phi;

		m_nUStartAngle = 0.;
		m_nUEndAngle   = PI + 2*theta;
		m_nUOffsetAngle= -(PI*0.5 + theta);
	}else{
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
	int nIndex=0;
	for(int i=0;i < nCurveSegments;i++){
		for(int j=0;j < nCurveSegments;j++){
			CSurface3D::para[nIndex++].Set(i*m_nUStepAngle,j*m_nVStepAngle);
			//CSurface3D::para[nIndex++].v= CMath::round(j*m_nVStepAngle,13);
		}
		CSurface3D::para[nIndex++].Set(i*m_nUStepAngle,2*PI);
		//CSurface3D::para[nIndex++].v= CMath::round(2*PI,13);
	}
	for(int j = 0;j < nCurveSegments;j++){
		CSurface3D::para[nIndex++].Set(m_nUEndAngle,j*m_nVStepAngle);
		//CSurface3D::para[nIndex++].v= CMath::round(j*m_nVStepAngle,13);
	}
	CSurface3D::para[nIndex].Set(m_nUEndAngle,2*PI);
	//CSurface3D::para[nIndex++].v= CMath::round(2*PI,13);
	
	if(m_sfcTorus.pBoundary){
		CreateBoundary(m_sfcTorus.pBoundary);
	}

	return true;
}

/**	\brief	The CTorusSurface::CreateBoundaryCurvePoint function

	\param	aryPara[]	a parameter of type CSurfacePara
	\param	nPoints	a parameter of type int&

	\return	void	
*/
void CTorusSurface::CreateBoundaryCurvePoint(CMeshPos2d aryPara[],int& nPoints){
	assert(aryPara       && "aryPara is NULL");
	assert((nPoints > 0) && "nPoints is less than 0.");

	if(aryPara && (nPoints > 0)){
		for(int i = 0;i < nPoints - 1;i++){
			double du = aryPara[i + 1].x() - aryPara[i].x();
			double dv = aryPara[i + 1].y() - aryPara[i].y();
			
			if(fabs(du) > PI*0.5){ /// may be over end of 'u' parameter.
				if(du > 0.){ /// may be over 0 
					double t=-aryPara[i].x()/(aryPara[i + 1].x() - aryPara[i].x());
					double v=(aryPara[i + 1].y() - aryPara[i].y())*t + aryPara[i].y();
					CMeshPos2d pos2d(0.,v);
					if(!((pos2d == aryPara[i]) || (pos2d == aryPara[i+1]))){
						memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
						aryPara[i + 1].Set(0.,v);
						i++;
						nPoints++;
					}
					
					t=(2*PI - aryPara[i].x())/(aryPara[i + 1].x() - aryPara[i].x());
					v=(aryPara[i + 1].y() - aryPara[i].y())*t + aryPara[i].y();
					pos2d.Set(2*PI,v);
					if(!((pos2d == aryPara[i]) || (pos2d == aryPara[i+1]))){
						memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
						aryPara[i + 1].Set(2*PI,v);
						i++;
						nPoints++;
					}
				}else if(du < 0.){ /// may be over 2*phi
					double t=(2*PI - aryPara[i].x())/(aryPara[i + 1].x() - aryPara[i].x());
					double v=(aryPara[i + 1].y() - aryPara[i].y())*t + aryPara[i].y();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(2*PI,v);
					i++;
					nPoints++;

					t=-aryPara[i].x()/(aryPara[i + 1].x() - aryPara[i].x());
					v=(aryPara[i + 1].y() - aryPara[i].y())*t + aryPara[i].y();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(0.,v);
					i++;
					nPoints++;
				}
			
			}else if(fabs(dv) > PI*0.5){ /// may be over end of 'v' parameter.
				if(dv > 0.){ /// may be over 0
					double t=-aryPara[i].y()/(aryPara[i + 1].y() - aryPara[i].y());
					double u=(aryPara[i + 1].x() - aryPara[i].x())*t + aryPara[i].x();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(u,0.);
					i++;
					nPoints++;
					
					t=(2*PI - aryPara[i].y())/(aryPara[i + 1].y() - aryPara[i].y());
					u=(aryPara[i + 1].x() - aryPara[i].x())*t + aryPara[i].x();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(u,2*PI);
					i++;
					nPoints++;
				}else if(dv < 0.){ /// may be over 2*phi
					double t=(2*PI - aryPara[i].y())/(aryPara[i + 1].y() - aryPara[i].y());
					double u=(aryPara[i + 1].x() - aryPara[i].x())*t + aryPara[i].x();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(u,2*PI);
					i++;
					nPoints++;

					t=-aryPara[i].y()/(aryPara[i + 1].y() - aryPara[i].y());
					u=(aryPara[i + 1].x() - aryPara[i].x())*t + aryPara[i].x();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(u,0.);
					i++;
					nPoints++;
				}
			}
		}
	}
}

/**	\brief	The CTorusSurface::CreateBoundary function

	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	void	
*/
void CTorusSurface::CreateBoundary(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");

	if(pPrimitive){
		int nCount=0;
		for(PSQPRIMITIVE ptr=pPrimitive;ptr;ptr = ptr->next,nCount++);

		CMeshPos2d last;
		if(m_paryBoundary = (BOUNDARY_T*)calloc(nCount,sizeof(BOUNDARY_T))){
			bool bClosed=false;
			int nBoundaryIndex=0;
			for(ptr=pPrimitive;ptr;ptr = ptr->next,nBoundaryIndex++){
				if(m_bReversed) ptr->hdr.attr.reversed = !ptr->hdr.attr.reversed;
				
				switch(ptr->hdr.id){
					case ENM_ARC:{
						CArc3d arc(ptr->body.arc);
						arc.CreateSegments();
						for(int i=0;i < arc.GetNumOfPoints();i++){
							CLibMesh2d::pos[i]=func(CCurve3d::pt(i));
							CMeshPos2d pos2d = CLibMesh2d::pos[i];
							CLibMesh2d::pos[i].Set(CLibMesh2d::pos[i].x(),CLibMesh2d::pos[i].y());
						}
						int nIndex = arc.GetNumOfPoints();
						//if(ptr->hdr.attr.reversed) reverse(&CLibMesh2d::pos[0],&CLibMesh2d::pos[nIndex]);
						//if((0 != nBoundaryIndex) && !bClosed) CLibMesh2d::pos[0] = last;
						bClosed = (2*PI == DEG2RAD(arc.nSweep)) ? true : false;
						CreateBoundaryCurvePoint(CLibMesh2d::pos,nIndex);
						
						if(ptr->hdr.attr.reversed){
							reverse(&CLibMesh2d::pos[0],&CLibMesh2d::pos[nIndex]);
							m_paryBoundary[m_nBoundary].vecNorm.dx = -ptr->body.arc.vecNorm.dx;
							m_paryBoundary[m_nBoundary].vecNorm.dy = -ptr->body.arc.vecNorm.dy;
							m_paryBoundary[m_nBoundary].vecNorm.dz = -ptr->body.arc.vecNorm.dz;
						}else{
							m_paryBoundary[m_nBoundary].vecNorm=ptr->body.arc.vecNorm;
						}
						if(bClosed){
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
						m_paryBoundary[m_nBoundary].plstEdge = new list<EDGE_T>;
						if(m_paryBoundary[m_nBoundary].plstEdge){
							EDGE_T edge;
							for(i = 0;i < nIndex - 1;i++){
								CMeshPos2d pos2d[2];
								pos2d[0] = CLibMesh2d::pos[i];
								pos2d[1] = CLibMesh2d::pos[i+1];
								if((fabs(CLibMesh2d::pos[i+1].x() - CLibMesh2d::pos[i].x()) < PI) && (fabs(CLibMesh2d::pos[i+1].y() - CLibMesh2d::pos[i].y()) < PI)){
									CLibMesh2d::pos[i].SetX(CLibMesh2d::pos[i].x());
									CLibMesh2d::pos[i+1].SetX(CLibMesh2d::pos[i+1].x());
									edge.pos[0] = CLibMesh2d::pos[i];
									edge.pos[1] = CLibMesh2d::pos[i+1];
									last = CLibMesh2d::pos[i+1];
									m_paryBoundary[m_nBoundary].plstEdge->push_back(edge);
								}
							}
							m_nBoundary++;
						}
							}
					break;
					case SQ_INTCURVE:{
						bool _bClosed=false;
						for(int i=0;i < ptr->body.intcurve.nPoints;i++){
							CLibMesh2d::pos[i]=func(ptr->body.intcurve.ppt[i]);
							CLibMesh2d::pos[i].Set(CLibMesh2d::pos[i].x(),CLibMesh2d::pos[i].y());
						}
						if(CLibMesh2d::pos[0] == CLibMesh2d::pos[i-1]) _bClosed = true;
						int nIndex = ptr->body.intcurve.nPoints;
						if(ptr->hdr.attr.reversed) reverse(&CLibMesh2d::pos[0],&CLibMesh2d::pos[nIndex]);
						if((0 != nBoundaryIndex) && !bClosed) CLibMesh2d::pos[0] = last;
						bClosed = _bClosed;
						CreateBoundaryCurvePoint(CLibMesh2d::pos,nIndex);
						
						m_paryBoundary[m_nBoundary].vecNorm=ptr->body.intcurve.vecNorm;
						m_paryBoundary[m_nBoundary].plstEdge = new list<EDGE_T>;
						if(m_paryBoundary[m_nBoundary].plstEdge){
							EDGE_T edge;
							for(i = 0;i < nIndex - 1;i++){
								if((fabs(CLibMesh2d::pos[i+1].x() - CLibMesh2d::pos[i].x()) < PI) && (fabs(CLibMesh2d::pos[i+1].y() - CLibMesh2d::pos[i].y()) < PI)){
									edge.pos[0].Set(CLibMesh2d::pos[i].x(),CLibMesh2d::pos[i].y());
									edge.pos[1].Set(CLibMesh2d::pos[i+1].x(),CLibMesh2d::pos[i+1].y());
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

/**	\brief	The CTorusSurface::func function

	\param	para	a parameter of type CSurfacePara&

	\return	POINT_T	
*/
POINT_T CTorusSurface::func(CSurfacePara& para){
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
POINT_T CTorusSurface::func(double u,double v){
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
CMeshPos2d CTorusSurface::func(POINT_T& pt){
	#define TOLERANCE	0.00000001
	CMeshPos2d para;

	if(m_sfcTorus.nTubeRadius < 0.){
		POINT_T _pt={0};
		_pt.x = pt.x - m_sfcTorus.ptOrigin.x;
		_pt.y = pt.y - m_sfcTorus.ptOrigin.y;
		_pt.z = pt.z - m_sfcTorus.ptOrigin.z;
		
		double alpha=0,beta=0;
		CGeometry::GetRotatedAngleOfAxis(m_sfcTorus.vecNorm,alpha,beta);
		_pt   = CGeometry::RotateAboutXYAxis(_pt,alpha,beta);

		double cval  = _pt.x/(m_sfcTorus.nTorusRadius + m_sfcTorus.nTubeRadius);
		double sval  = _pt.y/(m_sfcTorus.nTorusRadius + m_sfcTorus.nTubeRadius);
		VECTOR_T vec={0};
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
	}else if(m_sfcTorus.nTorusRadius >= m_sfcTorus.nTubeRadius){
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
		VECTOR_T vec={0};
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
	}else{
		double u=0.,u1=0.,u2=m_nUEndAngle,tu=0.,v=0,v1=0,v2=2*PI,tv=0.;
		double du=u2/16,dv=v2/16;
		double min=0.,len=0.;
		do{
			POINT_T pt1=func((u1 + du*0.5),(v1 + dv*0.5));
			double dx=pt1.x - pt.x;
			double dy=pt1.y - pt.y;
			double dz=pt1.z - pt.z;
			min=len=dx*dx + dy*dy + dz*dz;
			for(u = u1;u < u2;u += du){
				for(v = v1;v < v2;v += dv){
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
bool CTorusSurface::HasSegment(list<EDGE>* plstEdge,const POINT_T& pt1,const POINT_T& pt2){
	assert(plstEdge && "plstEdge is NULL");
	bool bRet=false;

	if(plstEdge){
		for(list<EDGE>::iterator itr=plstEdge->begin();itr != plstEdge->end();++itr){
			POINT_T pt[2]={0};

			pt[0].x = itr->para[0].u();
			pt[0].y = itr->para[0].v();
			pt[0].z = 0.;
			pt[1].x = itr->para[1].u();
			pt[1].y = itr->para[1].v();
			pt[1].z = 0.;
			//if(0 == memcmp(&(pt[0]),&pt1,sizeof(POINT_T))){
			if(IsSamePoint(pt[0],pt1)){
				//if(0 == memcmp(&(pt[1]),&pt2,sizeof(POINT_T))){
				if(IsSamePoint(pt[1],pt2)){
					bRet = true;
					break;
				}
			//}else if(0 == memcmp(&(pt[0]),&pt2,sizeof(POINT_T))){
			}else if(IsSamePoint(pt[0],pt2)){
				//if(0 == memcmp(&(pt[1]),&pt1,sizeof(POINT_T))){
				if(IsSamePoint(pt[1],pt1)){
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
bool CTorusSurface::IsBoundaryEdge(EDGE_T edge){
	POINT_T pt[2][2]={0,};

	pt[0][0].x = edge.pos[0].x();pt[0][0].y = edge.pos[0].y();
	pt[0][1].x = edge.pos[1].x();pt[0][1].y = edge.pos[1].y();
	for(int i=0;i < m_nBoundary;i++){
		for(list<EDGE_T>::iterator itr=m_paryBoundary[i].plstEdge->begin();itr != m_paryBoundary[i].plstEdge->end();++itr){
			pt[1][0].x = itr->pos[0].x();pt[1][0].y = itr->pos[0].y();
			pt[1][1].x = itr->pos[1].x();pt[1][1].y = itr->pos[1].y();
			if(IsSamePoint(pt[0][0],pt[1][0]) && IsSamePoint(pt[0][1],pt[1][1])) return true;
			if(IsSamePoint(pt[0][0],pt[1][1]) && IsSamePoint(pt[0][1],pt[1][0])) return true;
		}
	}

	return false;
}

/**	\brief	The CTorusSurface::IsValidEdge function

	\param	edge	a parameter of type const EDGE&

	\return	bool	
*/
//DEL bool CTorusSurface::IsValidEdge(const EDGE_T& edge){
//DEL 	double du=_round(edge.pos[1].x - edge.pos[0].x,9);
//DEL 	double dv=_round(edge.pos[1].y - edge.pos[0].y,9);
//DEL 
//DEL 	if((fabs(du) < 0.0) && (fabs(dv) < 0.0)) return false;
//DEL 	//if((fabs(du) < 0.0000000000001) && (fabs(dv) < 0.0000000000001)) return false;
//DEL 	
//DEL 	return true;
//DEL }

/**	\brief	The CTorusSurface::IsSamePoint function

	\param	pt1	a parameter of type const POINT_T&
	\param	pt2	a parameter of type const POINT_T&

	\return	bool	
*/
//DEL bool CTorusSurface::IsSamePoint(const POINT_T& pt1,const POINT_T& pt2){
//DEL 	double dx=_round(pt1.x - pt2.x,9);
//DEL 	double dy=_round(pt1.y - pt2.y,9);
//DEL 
//DEL 	//if((fabs(dx) < 0.0) && (fabs(dy) < 0.0)) return true;
//DEL 	if((fabs(dx) < 0.0000000000001) && (fabs(dy) < 0.0000000000001)) return true;
//DEL 
//DEL 	return false;
//DEL }

/**	\brief	The CTorusSurface::IsSameEdge function

	\param	edge1	a parameter of type const EDGE&
	\param	edge2	a parameter of type const EDGE&

	\return	bool	
*/
//DEL bool CTorusSurface::IsSameEdge(const EDGE_T& edge1,const EDGE_T& edge2){
//DEL 	double du[2]={0};
//DEL 	double dv[2]={0};
//DEL 	du[0] = edge1.pos[0].x - edge2.pos[0].x;
//DEL 	dv[0] = edge1.pos[0].y - edge2.pos[0].y;
//DEL 	du[1] = edge1.pos[1].x - edge2.pos[1].x;
//DEL 	dv[1] = edge1.pos[1].y - edge2.pos[1].y;
//DEL 	//if((fabs(du[0]) < 0.0) && (fabs(du[1]) < 0.0) && (fabs(dv[0]) < 0.0) && (fabs(dv[1]) < 0.0)) return true;
//DEL 	if((fabs(du[0]) < 0.0000000000001) && (fabs(du[1]) < 0.0000000000001) && (fabs(dv[0]) < 0.0000000000001) && (fabs(dv[1]) < 0.0000000000001)) return true;
//DEL 	
//DEL 	du[0] = edge1.pos[0].x - edge2.pos[1].x;
//DEL 	dv[0] = edge1.pos[0].y - edge2.pos[1].y;
//DEL 	du[1] = edge1.pos[1].x - edge2.pos[0].x;
//DEL 	dv[1] = edge1.pos[1].y - edge2.pos[0].y;
//DEL 	//if((fabs(du[0]) < 0.0) && (fabs(du[1]) < 0.0) && (fabs(dv[0]) < 0.0) && (fabs(dv[1]) < 0.0)) return true;
//DEL 	if((fabs(du[0]) < 0.0000000000001) && (fabs(du[1]) < 0.0000000000001) && (fabs(dv[0]) < 0.0000000000001) && (fabs(dv[1]) < 0.0000000000001)) return true;
//DEL 
//DEL 	return false;
//DEL }

/**	\brief	The CTorusSurface::IsOutboundTriangle function

	\param	nOutboundIndex	a parameter of type int&
	\param	BoundaryEdge	a parameter of type EDGE&
	\param	pDT	a parameter of type CDT*

	\return	int	
*/
int CTorusSurface::IsOutboundTriangle(int& nOutboundIndex,EDGE_T& BoundaryEdge,CDT* pDT){
	assert(pDT && "pDT is NULL");

	if(pDT){
		POINT_T para[2]={0};
		for(int i=0;i < m_nBoundary;i++){
			for(list<EDGE_T>::iterator itr=m_paryBoundary[i].plstEdge->begin();itr != m_paryBoundary[i].plstEdge->end();++itr){
				para[0].x = itr->pos[0].x();
				para[0].y = itr->pos[0].y();
				para[0].z = 0.;
				para[1].x = itr->pos[1].x();
				para[1].y = itr->pos[1].y();
				para[1].z = 0.;
				if(pDT->HasEdge(para[0],para[1])){
					if(!pDT->HasPosAtIndex(para[0],0) && !pDT->HasPosAtIndex(para[1],0)){
						if(CDT::Area(para[0],para[1],(*pDT)[0]) > 0.){
							nOutboundIndex = 0;
							BoundaryEdge   = *itr;
							return 1;
						}
						//}else	return 0;
					}else if(!pDT->HasPosAtIndex(para[0],1) && !pDT->HasPosAtIndex(para[1],1)){
						if(CDT::Area(para[0],para[1],(*pDT)[1]) > 0.){
							nOutboundIndex = 1;
							BoundaryEdge   = *itr;
							return 1;
						}
						//}else	return 0;
					}else if(!pDT->HasPosAtIndex(para[0],2) && !pDT->HasPosAtIndex(para[1],2)){
						if(CDT::Area(para[0],para[1],(*pDT)[2]) > 0.){
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
CDT* CTorusSurface::FindTriangleHasEdge(int& nIndex,list<CDT*>* plstDT,EDGE_T& edge){
	CDT* pRet=NULL;

	POINT_T para[2]={0,};
	para[0].x = edge.pos[0].x();
	para[0].y = edge.pos[0].y();
	para[0].z = 0.;
	para[1].x = edge.pos[1].x();
	para[1].y = edge.pos[1].y();
	para[1].z = 0.;
	for(list<CDT*>::iterator itr=plstDT->begin();itr != plstDT->end();){
		if((*itr)->m_bDeleted){
			itr++;
			continue;
		}
		pRet = *itr;
		if(pRet->HasEdge(para[0],para[1])){
			if(!pRet->HasPosAtIndex(para[0],0) && !pRet->HasPosAtIndex(para[1],0)){
				if(CDT::Area(para[0],para[1],(*pRet)[0]) > 0.){
					nIndex = 0;
					return pRet;
				}else itr++;
			}else if(!pRet->HasPosAtIndex(para[0],1) && !pRet->HasPosAtIndex(para[1],1)){
				if(CDT::Area(para[0],para[1],(*pRet)[1]) > 0.){
					nIndex = 1;
					return pRet;
				}else itr++;
			}else if(!pRet->HasPosAtIndex(para[0],2) && !pRet->HasPosAtIndex(para[1],2)){
				if(CDT::Area(para[0],para[1],(*pRet)[2]) > 0.){
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
void CTorusSurface::RemoveOutboundTriangles(list<CDT*>* plstDT){
	assert(plstDT && "plstDT is NULL");
	if(plstDT){
		queue<EDGE_T> queEdge;
		EDGE_T edge;
		
		for(list<CDT*>::iterator itr=plstDT->begin();itr != plstDT->end();){
			POINT_T pt[3]={0};
			pt[0] = (**itr)[0];
			pt[1] = (**itr)[1];
			pt[2] = (**itr)[2];
			int ret=0,nOutboundIndex=0;
			EDGE_T BoundaryEdge;
			if(ret=IsOutboundTriangle(nOutboundIndex,BoundaryEdge,*itr)){
				if(1 == ret){
					int nIndex[2]={0};
					
					nIndex[0] = (nOutboundIndex + 1)%3;
					nIndex[1] = (nOutboundIndex + 2)%3;
					
					EDGE_T edge;
					edge.pos[0].Set((**itr)[nOutboundIndex].x,(**itr)[nOutboundIndex].y);
					edge.pos[1].Set((**itr)[nIndex[0]].x,(**itr)[nIndex[0]].y);
					if(IsValidEdge(edge) && !IsSameEdge(edge,BoundaryEdge) && !IsBoundaryEdge(edge))
						queEdge.push(edge);
					edge.pos[0].Set((**itr)[nOutboundIndex].x,(**itr)[nOutboundIndex].y);
					edge.pos[1].Set((**itr)[nIndex[1]].x,(**itr)[nIndex[1]].y);
					if(IsValidEdge(edge) && !IsSameEdge(edge,BoundaryEdge) && !IsBoundaryEdge(edge))
						queEdge.push(edge);
				}
				(*itr)->m_bDeleted = true;
				itr++;
				//plstDT->erase(itr++);
			}else	itr++;
		}
		
		while(!queEdge.empty()){
			edge = queEdge.front(); queEdge.pop();
			for(list<CDT*>::iterator itr = plstDT->begin();itr != plstDT->end();){
				if((*itr)->m_bDeleted){
					itr++;
					continue;
				}
				
				POINT_T pt[2]={0};
				
				pt[0].x = edge.pos[0].x();
				pt[0].y = edge.pos[0].y();
				pt[0].z = 0;
				pt[1].x = edge.pos[1].x();
				pt[1].y = edge.pos[1].y();
				pt[1].z = 0;
				if((*itr)->HasEdge(pt[0],pt[1])){
					if(!IsSamePoint((**itr)[0],pt[0]) && !IsSamePoint((**itr)[0],pt[1])){
						edge.pos[0].Set((**itr)[0].x,(**itr)[0].y);
						edge.pos[1].Set((**itr)[1].x,(**itr)[1].y);
						if(!IsBoundaryEdge(edge)) queEdge.push(edge);
						edge.pos[0].Set((**itr)[0].x,(**itr)[0].y);
						edge.pos[1].Set((**itr)[2].x,(**itr)[2].y);
						if(!IsBoundaryEdge(edge)) queEdge.push(edge);
					}else if(!IsSamePoint((**itr)[1],pt[0]) && !IsSamePoint((**itr)[1],pt[1])){
						edge.pos[0].Set((**itr)[1].x,(**itr)[1].y);
						edge.pos[1].Set((**itr)[0].x,(**itr)[0].y);
						if(!IsBoundaryEdge(edge)) queEdge.push(edge);
						edge.pos[0].Set((**itr)[1].x,(**itr)[1].y);
						edge.pos[1].Set((**itr)[2].x,(**itr)[2].y);
						if(!IsBoundaryEdge(edge)) queEdge.push(edge);
					}else if(!IsSamePoint((**itr)[2],pt[0]) && !IsSamePoint((**itr)[2],pt[1])){
						edge.pos[0].Set((**itr)[2].x,(**itr)[2].y);
						edge.pos[1].Set((**itr)[0].x,(**itr)[0].y);
						if(!IsBoundaryEdge(edge)) queEdge.push(edge);
						edge.pos[0].Set((**itr)[2].x,(**itr)[2].y);
						edge.pos[1].Set((**itr)[1].x,(**itr)[1].y);
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

/**	\brief	The CTorusSurface::CreateFacets function\n
	if delaunay triangle is bad , doesn't attach this to list


	\return	CTinyFacetContainer*
*/
CTinyFacetContainer* CTorusSurface::CreateFacets(){
	CTinyFacetContainer* pFacetContainer=new CTinyFacetContainer;
	if(NULL == pFacetContainer) return NULL;

	list<CDT*> lstDT;
	CLibMesh2d mesh;
	// create DTs
	int NUM_OF_SEGMENTS=CCurve3d::GetNumOfSegments();
	int nPoints=NUM_OF_SEGMENTS+1;
	for(int i = 0;i < NUM_OF_SEGMENTS;i++){
		for(int j = 0;j < NUM_OF_SEGMENTS;j++){
			POINT_T pt[3]={0};
			
			pt[0].x = CSurface3D::para[nPoints*i + j].u();
			pt[0].y = CSurface3D::para[nPoints*i + j].v();
			pt[0].z = 0.;
			pt[1].x = CSurface3D::para[nPoints*(i+1) + j].u();
			pt[1].y = CSurface3D::para[nPoints*(i+1) + j].v();
			pt[1].z = 0.;
			pt[2].x = CSurface3D::para[nPoints*(i+1) + j+1].u();
			pt[2].y = CSurface3D::para[nPoints*(i+1) + j+1].v();
			pt[2].z = 0.;
			CDT* pDT=new CDT(pt[0],pt[1],pt[2]);
			if(pDT) lstDT.push_back(pDT);
			
			pt[0].x = CSurface3D::para[nPoints*i + j].u();
			pt[0].y = CSurface3D::para[nPoints*i + j].v();
			pt[0].z = 0.;
			pt[1].x = CSurface3D::para[nPoints*(i+1) + j+1].u();
			pt[1].y = CSurface3D::para[nPoints*(i+1) + j+1].v();
			pt[1].z = 0.;
			pt[2].x = CSurface3D::para[nPoints*i + (j+1)].u();
			pt[2].y = CSurface3D::para[nPoints*i + (j+1)].v();
			pt[2].z = 0.;
			pDT=new CDT(pt[0],pt[1],pt[2]);
			if(pDT) lstDT.push_back(pDT);
		}
	}
	
	static int _nCount=0;
	_nCount    =0;

	long nIndex=0;
	CMeshPos2d pos[2];
	for(i=0;i < m_nBoundary;i++){
		for(list<EDGE_T>::iterator itr=m_paryBoundary[i].plstEdge->begin();itr != m_paryBoundary[i].plstEdge->end();++itr){
			pos[0] = itr->pos[0];pos[1] = itr->pos[1];
			CSurface3D::pt[nIndex].x = itr->pos[0].x();
			CSurface3D::pt[nIndex].y = itr->pos[0].y();
			CSurface3D::pt[nIndex++].z = 0.;
			
			POINT_T pt = itr->pos[1].point();
			if(!IsSamePoint(CSurface3D::pt[nIndex-1],pt)){
				CSurface3D::pt[nIndex++] = pt;
			}

			CLibMesh2d::lnBoundary[_nCount].ptStart.x = itr->pos[0].x();
			CLibMesh2d::lnBoundary[_nCount].ptStart.y = itr->pos[0].y();
			CLibMesh2d::lnBoundary[_nCount].ptStart.z = 0.;
			CLibMesh2d::lnBoundary[_nCount].ptEnd.x = itr->pos[1].x();
			CLibMesh2d::lnBoundary[_nCount].ptEnd.y = itr->pos[1].y();
			CLibMesh2d::lnBoundary[_nCount].ptEnd.z = 0.;
			_nCount++;
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	// TEST CODE.
	/*
	for(i = 0;i < _nCount;i++){
		CFacet* pFacet=NULL;
		
		if(pFacet = new CFacet(
			func(CLibMesh2d::lnBoundary[i].ptStart.x,CLibMesh2d::lnBoundary[i].ptStart.y),
			func(CLibMesh2d::lnBoundary[i].ptEnd.x,CLibMesh2d::lnBoundary[i].ptEnd.y),
			func(CLibMesh2d::lnBoundary[i].ptStart.x,CLibMesh2d::lnBoundary[i].ptStart.y))){
			m_lstFacet.push_back(pFacet);
		}
	}
	
	FILE* fp=NULL;
	if(fp = fopen("d:\\opensolid.txt","wb")){
		fprintf(fp,"[POINT]\r\n");
		for(int i = 0;i < NUM_OF_SEGMENTS + 1;i++){
			for(int j = 0;j < NUM_OF_SEGMENTS + 1;j++){
				POINT_T pt={0,};
				
				pt.x = CSurface3D::para[nPoints*i + j].u();
				pt.y = CSurface3D::para[nPoints*i + j].v();
				fprintf(fp,"%.9lf,%.9lf\r\n",pt.x,pt.y);
			}
		}
		fprintf(fp,"[EDGE]\r\n");
		for(i = 0;i < _nCount;i++){
			fprintf(fp,"%.9lf,%.9lf,%.9lf,%.9lf\r\n",
				CLibMesh2d::lnBoundary[i].ptStart.x,CLibMesh2d::lnBoundary[i].ptStart.y,
				CLibMesh2d::lnBoundary[i].ptEnd.x,CLibMesh2d::lnBoundary[i].ptEnd.y
				);
		}
		fclose(fp);
	}
	*/
	//////////////////////////////////////////////////////////////////////////
	
	list<CDT*>* plstDT=mesh.Mesh(lstDT,CSurface3D::pt,nIndex,CLibMesh2d::lnBoundary,_nCount);
	if(plstDT){
		RemoveOutboundTriangles(plstDT);
		
		for(list<CDT*>::iterator itr=plstDT->begin();itr != plstDT->end();++itr){
			CFacet* pFacet=NULL;
			
			if(0){
			}else{
				if(!((*itr)->m_bDeleted)){
					/*
					POINT_T pos2d[3]={0,};
					pos2d[0] = (**itr)[0],pos2d[1] = (**itr)[1],pos2d[2] = (**itr)[2];
					pos2d[0].x *= 100;pos2d[0].y *= 100;pos2d[0].z *= 100;
					pos2d[1].x *= 100;pos2d[1].y *= 100;pos2d[1].z *= 100;
					pos2d[2].x *= 100;pos2d[2].y *= 100;pos2d[2].z *= 100;
					if(pFacet = new CFacet(pos2d[0],pos2d[1],pos2d[2])){
						pFacetContainer->Add(pFacet);
					}
					*/
					if(pFacet = new CFacet(func((**itr)[0].x,(**itr)[0].y),
						func((**itr)[1].x,(**itr)[1].y),
						func((**itr)[2].x,(**itr)[2].y))){
						pFacetContainer->Add(pFacet);
					}
				}
			}
		}
	}
	
	return pFacetContainer;
}

/**	\brief	The CTorusSurface::GetFacets function


	\return	list<CFacet*>*	
*/
list<CFacet*>* CTorusSurface::GetFacets(){
	return &m_lstFacet;
}