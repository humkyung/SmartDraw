#include <math.h>
#include <graphics\ITKGeo.h>
#include <graphics\geometry.h>
#include <graphics/arc3d.h>
#include "libmesh/libmesh2d.h"

#include "ConeSurface.h"
#include "OpenSolid.h"

#include <queue>
using namespace mesh;
using namespace std;

/**	\brief
*/
CConeSurface::CConeSurface(){
	m_sfcCone.ptOrigin.x = 0.;
	m_sfcCone.ptOrigin.y = 0.;
	m_sfcCone.ptOrigin.z = 0.;
	m_sfcCone.vecAxis.dx = 0.;
	m_sfcCone.vecAxis.dy = 0.;
	m_sfcCone.vecAxis.dz = 1.;
	m_sfcCone.ratio.a    = 0.;
	m_sfcCone.ratio.b    = 1.;
	m_sfcCone.nRadius    = 10.;
	
	m_nStartAngle   = 0.;
	m_nBoundaryPoint= 0;
	m_nBoundary     = 0;
	m_bReversed     = false;

	m_pEdge = NULL;

	OnCreate();
}

/**	\brief
*/
CConeSurface::CConeSurface(CONESURF_T& conesurface,bool bReversed){
	memcpy(&m_sfcCone,&conesurface,sizeof(CONESURF_T));
	m_nStartAngle   = 0.;
	m_nBoundaryPoint= 0;
	m_nBoundary     = 0;
	m_bReversed     = bReversed;

	m_pEdge = NULL;

	OnCreate();
}

/**	\brief	delete facets and boundary edges.
*/
CConeSurface::~CConeSurface(){
	for(list<CFacet*>::iterator itr=m_lstFacet.begin();itr != m_lstFacet.end();){
		delete (*itr);
		m_lstFacet.erase(itr++);
	}

	for(snode<LINE_T>* ptr=m_pEdge;ptr;){
		snode<LINE_T>* ptrNext=ptr->next;

		delete ptr;
		ptr = ptrNext;
	}
}

/**	\brief	The CConeSurface::OnCreate function


	\return	void	
*/
void CConeSurface::OnCreate(){
	double alpha=0.,beta=0.;
	CGeometry::GetRotatedAngleOfAxis(m_sfcCone.vecAxis,alpha,beta);
	
	POINT_T ptRelative={0.,0.,0.};
	ptRelative.x = m_sfcCone.vecStart.dx;
	ptRelative.y = m_sfcCone.vecStart.dy;
	ptRelative.z = m_sfcCone.vecStart.dz;
	double l=sqrt(ptRelative.x*ptRelative.x + ptRelative.y*ptRelative.y + ptRelative.z*ptRelative.z);
	
	ptRelative   = CGeometry::RotateAboutXYAxis(ptRelative,alpha,beta);
	m_nStartAngle= ITKGeo::GetRotatedAngleInXYPlane(ptRelative);

	int n=GetNumOfBoundary();
	CreateBoundary();
}

/**	\brief	The CConeSurface::CreateBoundaryCurvePoint function

	\param	aryPara[]	a parameter of type SURFACEPARA
	\param	nPoints	a parameter of type int&

	\return	void	
*/
void CConeSurface::CreateBoundaryCurvePoint(CMeshPos2d aryPara[],int& nPoints){
	assert(aryPara       && "aryPara is NULL");
	assert((nPoints > 0) && "nPoints is less than 0.");

	if(aryPara && (nPoints > 0)){
		for(int i = 0;i < nPoints - 1;i++){
			double dv = aryPara[i + 1].y() - aryPara[i].y();
			
			if(fabs(dv) > PI*0.5){ // may be over end of 'v' parameter.
				if(dv > 0.){ // may be over 0
					double t=(-aryPara[i].y())/(aryPara[i + 1].y() - aryPara[i].y());
					double u=(aryPara[i + 1].x() - aryPara[i].x())*t + aryPara[i].x();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(u,0.);
					i++;
					nPoints++;
					
					//t=(2*PI - aryPara[i].y)/(aryPara[i + 1].y - aryPara[i].y);
					//u=(aryPara[i + 1].x - aryPara[i].x)*t + aryPara[i].x;
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(u,2*PI);
					i++;
					nPoints++;
				}else if(dv < 0.){ // may be over 2*pi
					double t=(2*PI - aryPara[i].y())/(aryPara[i + 1].y() - aryPara[i].y() +  + 2*PI);
					double u=(aryPara[i + 1].x() - aryPara[i].x())*t + aryPara[i].x();
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(u,2*PI);
					i++;
					nPoints++;
					
					//t=(-aryPara[i].y)/(aryPara[i + 1].y - aryPara[i].y);
					//u=(aryPara[i + 1].x - aryPara[i].x)*t + aryPara[i].x;
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(CMeshPos2d)*(nPoints - i));
					aryPara[i + 1].Set(u,0.);
					i++;
					nPoints++;
				}
			}
		}
	}
}

int _curve_idx=0;
CMeshPos2d _last;
/**	\brief	The CConeSurface::CreateArcBoundary function\n
	create a arc and extract points on curve.

	\param	para[]	a parameter of type SURFACEPARA
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	int	
*/
int CConeSurface::CreateArcBoundary(CMeshPos2d para[],PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	int nCount=0;

	if(pPrimitive){
		CArc3d arc(pPrimitive->body.arc);
		arc.CreateSegments();
		nCount = arc.GetNumOfPoints();
		for(int i=0;i < nCount;i++){
			para[i]=func(CCurve3d::pt(i));
			para[i].Set(para[i].x(),para[i].y());
		}
		if(0 != _curve_idx) para[0] = _last;
		if(pPrimitive->hdr.attr.reversed) reverse(&para[0],&para[nCount]);
		_last = para[nCount - 1];
		CreateBoundaryCurvePoint(para,nCount);
	}

	return nCount;
}

/**	\brief	The CConeSurface::CreateIntCurveBoundary function\n
	create a intcurve and extract points on curve.

	\param	para[]	a parameter of type SURFACEPARA
	\param	pPrimitive	a parameter of type PSQPRIMITIVE

	\return	int	
*/
int CConeSurface::CreateIntCurveBoundary(CMeshPos2d para[],PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	int nCount=0;

	if(pPrimitive){
		nCount = pPrimitive->body.intcurve.nPoints;
		for(int i=0;i < nCount;i++){
			para[i]=func(pPrimitive->body.intcurve.ppt[i]);
			para[i].Set(para[i].x(),para[i].y());
		}
		if(0 != _curve_idx) para[0] = _last;
		if(pPrimitive->hdr.attr.reversed) reverse(&para[0],&para[nCount]);
		_last = para[nCount - 1];
		CreateBoundaryCurvePoint(para,nCount);
	}

	return nCount;
}

/**	\brief	The CConeSurface::CreateBoundary function


	\return	void	
*/
void CConeSurface::CreateBoundary(){	
	m_nBoundary=0;
	_curve_idx = 0;
	for(int i=0;i < m_sfcCone.nLoop;i++){
		int nLine=0,nPoint=0,nArc=0,nIntCurve=0;
		for(PSQPRIMITIVE ptr=m_sfcCone.pBoundary[i];ptr;ptr=ptr->next){
			if(ENM_POINT == ptr->hdr.id)		nPoint++;
			else if(ENM_LINE == ptr->hdr.id)	nLine++;
			else if(ENM_ARC == ptr->hdr.id)		nArc++;
			else if(SQ_INTCURVE == ptr->hdr.id)	nIntCurve++;
		}
		if((nLine < 3) && (0 == nPoint) && (0 == nArc) && (0 == nIntCurve)) continue;
		
		_curve_idx = 0;
		for(ptr=m_sfcCone.pBoundary[i];ptr && (m_nBoundaryPoint < MAX_SURF_POINTS);ptr = ptr->next,_curve_idx++){
			if(m_bReversed) ptr->hdr.attr.reversed = !ptr->hdr.attr.reversed;
			switch(ptr->hdr.id){
				case ENM_POINT:
					CLibMesh2d::pos[0]=func(ptr->body.point);
					CSurface3D::pt[m_nBoundaryPoint].x=CLibMesh2d::pos[0].x();
					CSurface3D::pt[m_nBoundaryPoint].y=CLibMesh2d::pos[0].y();
					CSurface3D::pt[m_nBoundaryPoint].z=0;
					m_nBoundaryPoint++;
					
					break;
				case ENM_LINE:
					if(ptr->hdr.attr.reversed){
						CLibMesh2d::pos[0]=func(ptr->body.line.ptEnd);
						CLibMesh2d::pos[0].Set(CLibMesh2d::pos[0].x(),CLibMesh2d::pos[0].y());
						CLibMesh2d::pos[1]=func(ptr->body.line.ptStart);
						CLibMesh2d::pos[1].Set(CLibMesh2d::pos[1].x(),CLibMesh2d::pos[1].y());
						double dv=CLibMesh2d::pos[1].y() - CLibMesh2d::pos[0].y();
						if((fabs(dv - 2*PI) < 0.0000000001)){
							CLibMesh2d::pos[0].SetY(CLibMesh2d::pos[1].y());
						}
						
						if(0 != _curve_idx) CLibMesh2d::pos[0] = _last;
						CSurface3D::pt[m_nBoundaryPoint].x=CLibMesh2d::pos[0].x();
						CSurface3D::pt[m_nBoundaryPoint].y=CLibMesh2d::pos[0].y();
						CSurface3D::pt[m_nBoundaryPoint].z=0;
						m_nBoundaryPoint++;
						CSurface3D::pt[m_nBoundaryPoint].x=CLibMesh2d::pos[1].x();
						CSurface3D::pt[m_nBoundaryPoint].y=CLibMesh2d::pos[1].y();
						CSurface3D::pt[m_nBoundaryPoint].z=0;
						m_nBoundaryPoint++;
						
						snode<LINE_T>* node=(snode<LINE_T>*)calloc(1,sizeof(snode<LINE_T>));
						if(node){
							node->element.ptStart.x = CLibMesh2d::pos[0].x();
							node->element.ptStart.y = CLibMesh2d::pos[0].y();
							node->element.ptStart.z = 0;
							node->element.ptEnd.x = CLibMesh2d::pos[1].x();
							node->element.ptEnd.y = CLibMesh2d::pos[1].y();
							node->element.ptEnd.z = 0;
							_last = CLibMesh2d::pos[1];
							
							m_pEdge = ListPush(m_pEdge,node);
						}
						m_nBoundary++;
					}else{
						CLibMesh2d::pos[0]=func(ptr->body.line.ptStart);
						CLibMesh2d::pos[0].Set(CLibMesh2d::pos[0].x(),CLibMesh2d::pos[0].y());
						CLibMesh2d::pos[1]=func(ptr->body.line.ptEnd);
						CLibMesh2d::pos[1].Set(CLibMesh2d::pos[1].x(),CLibMesh2d::pos[1].y());
						double dv=fabs(CLibMesh2d::pos[1].y() - CLibMesh2d::pos[0].y());
						if((fabs(dv - 2*PI) < 0.0000000001)){
							CLibMesh2d::pos[1].SetY(0.);
							CLibMesh2d::pos[0].SetY(0.);
						}
						
						if(0 != _curve_idx) CLibMesh2d::pos[0] = _last;
						CSurface3D::pt[m_nBoundaryPoint].x=CLibMesh2d::pos[0].x();
						CSurface3D::pt[m_nBoundaryPoint].y=CLibMesh2d::pos[0].y();
						CSurface3D::pt[m_nBoundaryPoint].z=0;
						m_nBoundaryPoint++;
						CSurface3D::pt[m_nBoundaryPoint].x=CLibMesh2d::pos[1].x();
						CSurface3D::pt[m_nBoundaryPoint].y=CLibMesh2d::pos[1].y();
						CSurface3D::pt[m_nBoundaryPoint].z=0;
						m_nBoundaryPoint++;
						
						snode<LINE_T>* node=(snode<LINE_T>*)calloc(1,sizeof(snode<LINE_T>));
						if(node){
							node->element.ptStart.x = CLibMesh2d::pos[0].x();
							node->element.ptStart.y = CLibMesh2d::pos[0].y();
							node->element.ptStart.z = 0;
							node->element.ptEnd.x = CLibMesh2d::pos[1].x();
							node->element.ptEnd.y = CLibMesh2d::pos[1].y();
							node->element.ptEnd.z = 0;
							_last = CLibMesh2d::pos[1];
							
							m_pEdge = ListPush(m_pEdge,node);
						}
						m_nBoundary++;
					}
					break;
				case ENM_ARC:{
					int nCount=CreateArcBoundary(CLibMesh2d::pos,ptr);
					if(nCount){
						//m_aryBoundary[m_nBoundary].yecNorm=ptr->body.arc.yecNorm;
						m_nBoundary++;
						
						for(int i = 0;i < nCount - 1;i++){
							if((fabs(CLibMesh2d::pos[i+1].y() - CLibMesh2d::pos[i].y()) < 0.5*PI)){
								POINT_T _pt[2]={0,};
								
								_pt[0].x = CLibMesh2d::pos[i].x();
								_pt[0].y = CLibMesh2d::pos[i].y();
								_pt[0].z = 0;
								_pt[1].x = CLibMesh2d::pos[i+1].x();
								_pt[1].y = CLibMesh2d::pos[i+1].y();
								_pt[1].z = 0;
								if(!IsSamePoint(_pt[0],_pt[1])){
									snode<LINE_T>* node=(snode<LINE_T>*)calloc(1,sizeof(snode<LINE_T>));
									if(node){
										node->element.ptStart.x = CLibMesh2d::pos[i].x();
										node->element.ptStart.y = CLibMesh2d::pos[i].y();
										node->element.ptStart.z = 0;
										node->element.ptEnd.x = CLibMesh2d::pos[i+1].x();
										node->element.ptEnd.y = CLibMesh2d::pos[i+1].y();
										node->element.ptEnd.z = 0;
										
										m_pEdge = ListPush(m_pEdge,node);
									}
									
									CSurface3D::pt[m_nBoundaryPoint].x =CLibMesh2d::pos[i].x();
									CSurface3D::pt[m_nBoundaryPoint].y =CLibMesh2d::pos[i].y();
									CSurface3D::pt[m_nBoundaryPoint].z =0;
									m_nBoundaryPoint++;
								}
							}
							
							CSurface3D::pt[m_nBoundaryPoint].x = CLibMesh2d::pos[nCount - 1].x();
							CSurface3D::pt[m_nBoundaryPoint].y = CLibMesh2d::pos[nCount - 1].y();
							CSurface3D::pt[m_nBoundaryPoint].z = 0;
							m_nBoundaryPoint++;
						}
					}
					    }
					break;
				case SQ_INTCURVE:{
					int nCount=CreateIntCurveBoundary(CLibMesh2d::pos,ptr);
					if(nCount){
						//m_aryBoundary[m_nBoundary].yecNorm=ptr->body.intcurve.yecNorm;
						m_nBoundary++;
						
						for(int i = 0;i < nCount - 1;i++){
							if((fabs(CLibMesh2d::pos[i+1].y() - CLibMesh2d::pos[i].y()) < 0.5*PI)){
								snode<LINE_T>* node=(snode<LINE_T>*)calloc(1,sizeof(snode<LINE_T>));
								if(node){
									node->element.ptStart.x = CLibMesh2d::pos[i].x();
									node->element.ptStart.y = CLibMesh2d::pos[i].y();
									node->element.ptStart.z = 0;
									node->element.ptEnd.x = CLibMesh2d::pos[i+1].x();
									node->element.ptEnd.y = CLibMesh2d::pos[i+1].y();
									node->element.ptEnd.z = 0;
									
									m_pEdge = ListPush(m_pEdge,node);
								}
								CSurface3D::pt[m_nBoundaryPoint].x=CLibMesh2d::pos[i].x();
								CSurface3D::pt[m_nBoundaryPoint].y=CLibMesh2d::pos[i].y();
								CSurface3D::pt[m_nBoundaryPoint].z=0;
								m_nBoundaryPoint++;
							}
						}
						
						CSurface3D::pt[m_nBoundaryPoint].x=CLibMesh2d::pos[nCount - 1].x();
						CSurface3D::pt[m_nBoundaryPoint].y=CLibMesh2d::pos[nCount - 1].y();
						CSurface3D::pt[m_nBoundaryPoint].z=0;
						m_nBoundaryPoint++;
					}
						 }
					break;
			}
		}
	}
}

/**	\brief	The CConeSurface::func function

	\param	t	a parameter of type double
	\param	phi	a parameter of type double

	\return	POINT_T	
*/
POINT_T CConeSurface::func(double t,double phi){
	POINT_T ptRet={0};
	if(m_sfcCone.ratio.a){
		double ratio=m_sfcCone.ratio.a/m_sfcCone.ratio.b;
		ptRet.x = (ratio*t + m_sfcCone.nRadius)*cos(phi);
		ptRet.y = (ratio*t + m_sfcCone.nRadius)*sin(phi);
		ptRet.z = t;
	}else{
		ptRet.x = m_sfcCone.nRadius*cos(phi);
		ptRet.y = m_sfcCone.nRadius*sin(phi);
		ptRet.z = t*m_sfcCone.ratio.b;
		//ptRet.z = t;
	}

	double alpha=0,beta=0;
	CGeometry::GetRotatedAngleOfAxis(m_sfcCone.vecAxis,alpha,beta);
	ptRet = CGeometry::RotateAboutYXAxis(ptRet,-beta,-alpha);
	ptRet.x += m_sfcCone.ptOrigin.x;
	ptRet.y += m_sfcCone.ptOrigin.y;
	ptRet.z += m_sfcCone.ptOrigin.z;

	return ptRet;
}

/**	\brief	The CConeSurface::func function\n
	calculate t and phi for pt.

	\param	pt	a parameter of type const POINT_T&

	\return	SURFACEPARA	
*/
CMeshPos2d CConeSurface::func(const POINT_T& pt){
	CMeshPos2d para;

	double dir=(m_sfcCone.ratio.b < 0) ? -1 : 1;
	double a=dir*m_sfcCone.vecAxis.dx;
	double b=dir*m_sfcCone.vecAxis.dy;
	double c=dir*m_sfcCone.vecAxis.dz;
	double d=-(a*pt.x + b*pt.y + c*pt.z);
	para.SetX(-(a*m_sfcCone.ptOrigin.x + b*m_sfcCone.ptOrigin.y + c*m_sfcCone.ptOrigin.z + d)/
		(a*a + b*b + c*c));

	double alpha=0,beta=0;
	CGeometry::GetRotatedAngleOfAxis(m_sfcCone.vecAxis,alpha,beta);
	POINT_T _pt={0};
	_pt.x = pt.x - m_sfcCone.ptOrigin.x;
	_pt.y = pt.y - m_sfcCone.ptOrigin.y;
	_pt.z = pt.z - m_sfcCone.ptOrigin.z;
	_pt = CGeometry::RotateAboutXYAxis(_pt,alpha,beta);
	_pt.z  = 0.;
	para.SetY(CGeometry::GetRotatedAngleInXYPlane(_pt));

	return para;
}

/**	\brief	The CConeSurface::IsBoundaryEdge function\n
	check whether line is bounday edge or not.

	\param	line	a parameter of type const LINE_T&

	\return	true if line is bounday edge.
*/
bool CConeSurface::IsBoundaryEdge(const LINE_T& line){
	bool bRet=false;

	if(m_pEdge){
		for(snode<LINE_T>* ptr=m_pEdge;ptr;ptr=ptr->next){
			//if(	CGeometry::IsSamePoint(ptr->element.ptStart,line.ptStart,0.0000000001) && 
			//	CGeometry::IsSamePoint(ptr->element.ptEnd,line.ptEnd,0.0000000001)){
			if(	IsSamePoint(ptr->element.ptStart,line.ptStart) && 
				IsSamePoint(ptr->element.ptEnd,line.ptEnd)){
				bRet = true;
				break;
			}
		}
	}

	return bRet;
}

/**	\brief	The CConeSurface::IsOutboundGrid function\n
	find grid has an edge and exist left side of the edge.

	\param	nIndex	a parameter of type int&
	\param	pGrid	a parameter of type CGrid*

	\return	outboundgrid - true , not - false
*/
bool CConeSurface::IsOutboundGrid(int& nIndex,CGrid* pGrid){
	assert(pGrid && "pGrid is NULL");

	if(pGrid){
		POINT_T ptMid={0};
		for(snode<LINE_T>* ptr=m_pEdge;ptr;ptr=ptr->next){
			if(nIndex = pGrid->HasEdge(ptr->element)){
				if(pGrid->IsLeftSide(nIndex,ptr->element)){
					return true;
				}else	return false;
			}
		}
	}

	return false;
}

/**	\brief	The CConeSurface::RemoveOutboundGrids function\n
	first, erase outbound grid and then\n
	erase adjacent grid of outbound grid.

	\param	plstGrid	a parameter of type list<CGrid*>*

	\return	void	
*/
void CConeSurface::RemoveOutboundGrids(list<CGrid*>* plstGrid){
	assert(plstGrid && "plstGrid is NULL");
	
	if(plstGrid){
		queue<LINE_T> queEdge;

		int nIndex=0;
		for(list<CGrid*>::iterator itr=plstGrid->begin();itr != plstGrid->end();){
			if(IsOutboundGrid(nIndex,*itr)){
				int i=1;
				for(snode<POINT_T>* ptr=(*itr)->m_pNode;ptr && ptr->next;ptr=ptr->next,i++){
					LINE_T line={0};
					
					line.ptStart = ptr->element;
					line.ptEnd   = ptr->next->element;
					if((i != nIndex) && !IsBoundaryEdge(line)){
						queEdge.push(line);
					}
				}
				LINE_T line={0};
				
				line.ptStart = ptr->element;
				line.ptEnd   = (*itr)->m_pNode->element;
				if((i != nIndex) && !IsBoundaryEdge(line)){
					queEdge.push(line);
				}
				plstGrid->erase(itr++);
			}else	itr++;
		}

		LINE_T line={0.,};
		while(!queEdge.empty()){
			POINT_T pt={0};
			line = queEdge.front();queEdge.pop();
			for(list<CGrid*>::iterator itr = plstGrid->begin();itr != plstGrid->end();){
				if((*itr)->DoIntersection(pt,line)){
					list<CGrid*> lstGridTmp;

					CGrid* pGrid=*itr;
					(*itr)->InsertPoint(lstGridTmp,pt);
					plstGrid->insert(plstGrid->end(),lstGridTmp.begin(),lstGridTmp.end());
					plstGrid->erase(itr++);
					
					POINT_T _pt=line.ptEnd;
					line.ptEnd = pt;
					queEdge.push(line);
					line.ptStart = pt;
					line.ptEnd   = _pt;
					queEdge.push(line);
					break;
				}else if(nIndex = (*itr)->HasEdge(line)){
					CGrid* pGrid=*itr;
					int i=1;
					for(snode<POINT_T>* ptr=(*itr)->m_pNode;ptr && ptr->next;ptr=ptr->next,i++){
						if(i != nIndex){
							LINE_T _line={0};
							
							_line.ptStart = ptr->element;
							_line.ptEnd   = ptr->next->element;
							queEdge.push(_line);
						}
					}
					if(i != nIndex){
						LINE_T _line={0};
						
						_line.ptStart = ptr->element;
						_line.ptEnd   = (*itr)->m_pNode->element;
						queEdge.push(_line);
					}
					plstGrid->erase(itr++);
					break;
				}else	itr++;
			}
		}
	}
}

/**	\brief	The CConeSurface::CreateGrids function

	\param	lstGrid	a parameter of type list<CGrid*>&
	\param	tMin	a parameter of type double
	\param	tMax	a parameter of type double

	\return	void	
*/
void CConeSurface::CreateGrids(list<CGrid*>& lstGrid,double tMin,double tMax){
	POINT_T pt[4]={0};

	int NUM_OF_SEGMENTS=CCurve3d::GetNumOfSegments();
	double nStep=_round((2*PI)/(NUM_OF_SEGMENTS),LIBMESH2D_PRECISION);
	m_nStartAngle = 0;
	for(double v=0;v < 2*PI - nStep;v += nStep){
		pt[0].x = tMin;
		pt[0].y = v;
		//pt[0].y = v - m_nStartAngle;
		//pt[0].y = (pt[0].y < 0.) ? pt[0].y + 2*PI : pt[0].y;
		pt[0].z = 0;
		pt[1].x = tMax;
		pt[1].y = v;
		//pt[1].y = v + m_nStartAngle;
		//pt[1].y = (pt[1].y < 0.) ? pt[1].y + 2*PI : pt[1].y;
		pt[1].z = 0;
		pt[2].x = tMax;
		pt[2].y = v + nStep;
		//pt[2].y = v + nStep + m_nStartAngle;
		//pt[2].y = (pt[2].y < 0.) ? pt[2].y + 2*PI : pt[2].y;
		pt[2].z = 0;
		pt[3].x = tMin;
		pt[3].y = v + nStep;
		//pt[3].y = v + nStep + m_nStartAngle;
		//pt[3].y = (pt[3].y < 0.) ? pt[3].y + 2*PI : pt[3].y;
		pt[3].z = 0;
		CGrid* pGrid=NULL;
		if(pGrid = new CGrid(pt)){
			lstGrid.push_back(pGrid);
		}
	}
	pt[0].x = tMin;
	pt[0].y = v;
	//pt[0].y = v + m_nStartAngle;
	//pt[0].y = (pt[0].y < 0.) ? pt[0].y + 2*PI : pt[0].y;
	pt[0].z = 0;
	pt[1].x = tMax;
	pt[1].y = v;
	//pt[1].y = v + m_nStartAngle;
	//pt[1].y = (pt[1].y < 0.) ? pt[1].y + 2*PI : pt[1].y;
	pt[1].z = 0;
	pt[2].x = tMax;
	pt[2].y = 2*PI;
	//pt[2].y = 2*PI + m_nStartAngle;
	//pt[2].y = (pt[2].y < 0.) ? pt[2].y + 2*PI : pt[2].y;
	pt[2].z = 0;
	pt[3].x = tMin;
	pt[3].y = 2*PI;
	//pt[3].y = 2*PI + m_nStartAngle;
	//pt[3].y = (pt[3].y < 0.) ? pt[3].y + 2*PI : pt[3].y;
	pt[3].z = 0;
	CGrid* pGrid=NULL;
	if(pGrid = new CGrid(pt)){
		lstGrid.push_back(pGrid);
	}
}

/**	\brief	The CConeSurface::CreateFacets function


	\return	list<CFacet*>*	
*/
list<CFacet*>* CConeSurface::CreateFacets(){
	assert(m_pEdge && "m_pEdge is NULL");

	if(m_pEdge){
		double tMin=CSurface3D::pt[0].x;
		double tMax=tMin;
		double vMin=CSurface3D::pt[0].y;
		double vMax=vMin;
		for(int i=0;i < m_nBoundaryPoint;i++){
			if(tMin > CSurface3D::pt[i].x) tMin = CSurface3D::pt[i].x;
			if(tMax < CSurface3D::pt[i].x) tMax = CSurface3D::pt[i].x;
			if(vMin > CSurface3D::pt[i].y) vMin = CSurface3D::pt[i].y;
			if(vMax < CSurface3D::pt[i].y) vMax = CSurface3D::pt[i].y;
		}
		list<CGrid*> lstGrid;
		CreateGrids(lstGrid,tMin,tMax);
		list<CGrid*>* plstGrid=&(lstGrid);

		if(m_nBoundary >= 4){
			long nIndex=0;

			CLibMesh2d mesh;
			mesh.InsertGrids(lstGrid);
			mesh.InsertGridPoints(CSurface3D::pt,m_nBoundaryPoint);

			mesh.InsertGridEdges(m_pEdge);
			plstGrid = mesh.GetListOfGrid();

			list<CGrid*> lstGridTmp;
			for(snode<LINE_T>* ptr=m_pEdge;ptr;ptr=ptr->next){
				for(list<CGrid*>::iterator itr=plstGrid->begin();itr != plstGrid->end();itr++){
					CGrid* pGrid=*itr;
					if((*itr)->IsDiagonalLine(ptr->element)){
						lstGridTmp.push_back((*itr)->Split(ptr->element));
						break;
					}
				}
			}
			plstGrid->insert(plstGrid->end(),lstGridTmp.begin(),lstGridTmp.end());
			RemoveOutboundGrids(plstGrid);
			//if(m_sfcCone.vecAxis.dz > 0)
			//	RemoveOutboundGrids(plstGrid,false);
			//else	RemoveOutboundGrids(plstGrid,true);
						
			for(list<CGrid*>::iterator itr=plstGrid->begin();itr != plstGrid->end();){
				CFacet* pFacet=NULL;
				
				POINT_T pt[4]={0};
				int nIndex=0;
				for(snode<POINT_T>* ptr=(*itr)->m_pNode;ptr;ptr = ptr->next,nIndex++){
					pt[nIndex].x = ptr->element.x;
					pt[nIndex].y = ptr->element.y;
					pt[nIndex].z = 0;
				}
				
				/*
				if(4 == nIndex){
				pFacet = new CFacet(pt[0],pt[1],pt[2],pt[3]);
				}else if(3 == nIndex){
				pFacet = new CFacet(pt[0],pt[1],pt[2]);
				}
				*/				
				if(4 == nIndex){
					pFacet = new CFacet(func(pt[0].x,pt[0].y),func(pt[1].x,pt[1].y),
						func(pt[2].x,pt[2].y),func(pt[3].x,pt[3].y));
				}else if(3 == nIndex){
					pFacet = new CFacet(func(pt[0].x,pt[0].y),func(pt[1].x,pt[1].y),func(pt[2].x,pt[2].y));
				}
				if(pFacet) m_lstFacet.push_back(pFacet);
				
				delete (*itr);
				plstGrid->erase(itr++);
			}
		}else{
			for(list<CGrid*>::iterator itr=plstGrid->begin();itr != plstGrid->end();){
				CFacet* pFacet=NULL;
				
				POINT_T pt[4]={0};
				int nIndex=0;
				for(snode<POINT_T>* ptr=(*itr)->m_pNode;ptr;ptr = ptr->next,nIndex++){
					pt[nIndex].x = ptr->element.x;
					pt[nIndex].y = ptr->element.y;
					pt[nIndex].z = 0;
				}
				
				/*
				if(4 == nIndex){
				pFacet = new CFacet(pt[0],pt[1],pt[2],pt[3]);
				}else if(3 == nIndex){
				pFacet = new CFacet(pt[0],pt[1],pt[2]);
				}
				*/
				
				if(4 == nIndex){
					pFacet = new CFacet(func(pt[0].x,pt[0].y),func(pt[1].x,pt[1].y),
						func(pt[2].x,pt[2].y),func(pt[3].x,pt[3].y));
				}else if(3 == nIndex){
					pFacet = new CFacet(func(pt[0].x,pt[0].y),func(pt[1].x,pt[1].y),func(pt[2].x,pt[2].y));
				}
				m_lstFacet.push_back(pFacet);
				
				delete (*itr);
				plstGrid->erase(itr++);
			}
		}
	}

	return &m_lstFacet;
}

/**	\brief	The CConeSurface::GetNumOfBoundary function


	\return	int	
*/
int CConeSurface::GetNumOfBoundary(){
	int n=0;
	for(int i=0;i < m_sfcCone.nLoop;i++){
		for(PSQPRIMITIVE ptr=m_sfcCone.pBoundary[i];ptr;ptr = ptr->next,n++);
	}
	
	return n;
}
