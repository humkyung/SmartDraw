#include <math.h>
#include "queue.h"
#include "KGeometry.h"
#include "KArc.h"
#include "KMesh.h"
using namespace mesh;

#include "KConeSurface.h"
#include "include\OpenSolid.h"

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KConeSurface::KConeSurface(){
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
	m_nBoundary     = 0;
	m_nBoundaryPoint= 0;
	m_bReversed     = false;

	m_pEdge = NULL;

	OnCreate();
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KConeSurface::KConeSurface(SQCONESURFACE& conesurface,bool bReversed){
	memcpy(&m_sfcCone,&conesurface,sizeof(SQCONESURFACE));
	m_nStartAngle   = 0.;
	m_nBoundary     = 0;
	m_nBoundaryPoint= 0;
	m_bReversed     = bReversed;

	m_pEdge = NULL;

	OnCreate();
}

//	parameter	:
//	description	: delete facets and boundary edges.
//	remarks		:
//	returns		:
KConeSurface::~KConeSurface(){
	for(list<KFacet*>::iterator itr=m_lstFacet.begin();itr != m_lstFacet.end();){
		delete (*itr);
		m_lstFacet.erase(itr++);
	}

	for(snode<SQLINE>* ptr=m_pEdge;ptr;){
		snode<SQLINE>* ptrNext=ptr->next;

		delete ptr;
		ptr = ptrNext;
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KConeSurface::OnCreate(){
	double alpha=0.,beta=0.;
	KGeometry::GetRotatedAngleOfAxis(m_sfcCone.vecAxis,alpha,beta);
	
	SQPOINT ptRelative={0.,0.,0.};
	ptRelative.x = m_sfcCone.vecStart.dx;
	ptRelative.y = m_sfcCone.vecStart.dy;
	ptRelative.z = m_sfcCone.vecStart.dz;
	double l=sqrt(ptRelative.x*ptRelative.x + ptRelative.y*ptRelative.y + ptRelative.z*ptRelative.z);
	
	ptRelative   = KGeometry::RotateAboutXYAxis(ptRelative,alpha,beta);
	m_nStartAngle= GetRotatedAngleInXYPlane(ptRelative);

	//int n=GetNumOfBoundary();
	//KCurve::NumOfSegments = theOpenSolid.GetNumOfCurveSegments();
	CreateBoundary(m_sfcCone.pBoundary);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KConeSurface::CreateBoundaryCurvePoint(SURFACEPARA aryPara[],int& nPoints){
	assert(aryPara       && "aryPara is NULL");
	assert((nPoints > 0) && "nPoints is less than 0.");

	if(aryPara && (nPoints > 0)){
		for(int i = 0;i < nPoints - 1;i++){
			double dv = aryPara[i + 1].v - aryPara[i].v;
			
			if(fabs(dv) > PI*0.5){ // may be over end of 'v' parameter.
				if(dv > 0.){ // may be over 0
					double t=(-aryPara[i].v)/(aryPara[i + 1].v - aryPara[i].v);
					double u=(aryPara[i + 1].u - aryPara[i].u)*t + aryPara[i].u;
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(SURFACEPARA)*(nPoints - i));
					aryPara[i + 1].u = u;
					aryPara[i + 1].v = 0.;
					i++;
					nPoints++;
					
					//t=(2*PI - aryPara[i].v)/(aryPara[i + 1].v - aryPara[i].v);
					//u=(aryPara[i + 1].u - aryPara[i].u)*t + aryPara[i].u;
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(SURFACEPARA)*(nPoints - i));
					aryPara[i + 1].u = u;
					aryPara[i + 1].v = 2*PI;
					i++;
					nPoints++;
				}else if(dv < 0.){ // may be over 2*pi
					double t=(2*PI - aryPara[i].v)/(aryPara[i + 1].v - aryPara[i].v +  + 2*PI);
					double u=(aryPara[i + 1].u - aryPara[i].u)*t + aryPara[i].u;
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(SURFACEPARA)*(nPoints - i));
					aryPara[i + 1].u = u;
					aryPara[i + 1].v = 2*PI;
					i++;
					nPoints++;
					
					//t=(-aryPara[i].v)/(aryPara[i + 1].v - aryPara[i].v);
					//u=(aryPara[i + 1].u - aryPara[i].u)*t + aryPara[i].u;
					memmove(&(aryPara[i + 2]),&(aryPara[i + 1]),sizeof(SURFACEPARA)*(nPoints - i));
					aryPara[i + 1].u = u;
					aryPara[i + 1].v = 0.;
					i++;
					nPoints++;
				}
			}
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KConeSurface::Reverse(SURFACEPARA para[],int nStart,int nEnd){
	assert((nStart < nEnd) && "nStart is less than nEnd.");

	if(nStart < nEnd){
		int nMid = (int)((nEnd - nStart)*0.5);
		for(int i=0;i < nMid;i++){
			SURFACEPARA _p=para[nEnd - i - 1];
			para[nEnd - i - 1] = para[nStart + i];
			para[nStart + i]   = _p;
		}
	}
}

//	parameter	:
//	description	: create a arc and extract points on curve.
//	remarks		:
//	returns		:
int KConeSurface::CreateArcBoundary(SURFACEPARA para[],PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	int nCount=0;

	if(pPrimitive){
		KArc* pArc=NULL;
		if(pArc = new KArc(pPrimitive->body.arc)){
			nCount = pArc->GetNumOfPoints();
			for(int i=0;i < nCount;i++){
				para[i]=func(KCurve::pt[i]);
			}
			CreateBoundaryCurvePoint(para,nCount);
			if(pPrimitive->hdr.type.reversed) Reverse(para,0,nCount);
			
			delete pArc;
		}
	}

	return nCount;
}

//	parameter	:
//	description	: create a intcurve and extract points on curve.
//	remarks		:
//	returns		:
int KConeSurface::CreateIntCurveBoundary(SURFACEPARA para[],PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	int nCount=0;

	if(pPrimitive){
		nCount = pPrimitive->body.intcurve.nPoints;
		for(int i=0;i < nCount;i++){
			para[i]=func(pPrimitive->body.intcurve.ppt[i]);
		}
		CreateBoundaryCurvePoint(para,nCount);
		if(pPrimitive->hdr.type.reversed) Reverse(para,0,nCount);
	}

	return nCount;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KConeSurface::CreateBoundary(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive && "pPrimitive is NULL");
	static SURFACEPARA _p[2][256]={0.};
	
	if(pPrimitive){
		m_nBoundary=0;
		for(PSQPRIMITIVE ptr=pPrimitive;ptr;ptr = ptr->next){
			if(m_bReversed) ptr->hdr.type.reversed = !ptr->hdr.type.reversed;
			switch(ptr->hdr.id){
				/*
				case SQ_GROUP:{
					int nIndex=0;
					for(PSQPRIMITIVE pGroup=ptr->body.pPrimitive;pGroup;pGroup = pGroup->next){
						if(SQ_ARC == pGroup->hdr.id){
							int nCount=CreateArcBoundary(_p[0],pGroup);
							memcpy(&(_p[1][nIndex]),_p[0],sizeof(SURFACEPARA)*nCount);
							nIndex += nCount;
						}else if(SQ_INTCURVE == pGroup->hdr.id){
							int nCount=CreateIntCurveBoundary(_p[0],pGroup);
							memcpy(&(_p[1][nIndex]),_p[0],sizeof(SURFACEPARA)*nCount);
							nIndex += nCount;
						}
					}
					if(nIndex){
						m_Boundary[m_nBoundary].plstEdge = new list<EDGE>;
						if(m_Boundary[m_nBoundary].plstEdge){
							EDGE edge={0};
							for(int i = 0;i < nIndex - 1;i++){
								if((fabs(_p[1][i+1].v - _p[1][i].v) < 0.5*PI)){
									edge.para[0].u = _p[1][i].u;
									edge.para[0].v = _p[1][i].v;
									edge.para[1].u = _p[1][i+1].u;
									edge.para[1].v = _p[1][i+1].v;
									m_Boundary[m_nBoundary].plstEdge->push_back(edge);
								}
							}
							m_nBoundary++;
						}
					}
					      }
					break;
					*/
				case SQ_POINT:
					_p[0][0]=func(ptr->body.point);
					KSurface::pt[m_nBoundaryPoint].x=_p[0][0].u;
					KSurface::pt[m_nBoundaryPoint].y=_p[0][0].v;
					KSurface::pt[m_nBoundaryPoint].z=0;
					m_nBoundaryPoint++;
					m_nBoundary++;
					
					break;
				case SQ_LINE:
					if(ptr->hdr.type.reversed){
						_p[0][0]=func(ptr->body.line.ptEnd);
						_p[0][1]=func(ptr->body.line.ptStart);
						double dv=_p[0][1].v - _p[0][0].v;
						if((fabs(dv - 2*PI) < 0.0000000001)){
							_p[0][0].v = _p[0][1].v;
						}
						
						KSurface::pt[m_nBoundaryPoint].x=_p[0][0].u;
						KSurface::pt[m_nBoundaryPoint].y=_p[0][0].v;
						KSurface::pt[m_nBoundaryPoint].z=0;
						m_nBoundaryPoint++;
						KSurface::pt[m_nBoundaryPoint].x=_p[0][1].u;
						KSurface::pt[m_nBoundaryPoint].y=_p[0][1].v;
						KSurface::pt[m_nBoundaryPoint].z=0;
						m_nBoundaryPoint++;

						snode<SQLINE>* node=(snode<SQLINE>*)calloc(1,sizeof(snode<SQLINE>));
						if(node){
							node->element.ptStart.x = _p[0][0].u;
							node->element.ptStart.y = _p[0][0].v;
							node->element.ptStart.z = 0;
							node->element.ptEnd.x = _p[0][1].u;
							node->element.ptEnd.y = _p[0][1].v;
							node->element.ptEnd.z = 0;

							m_pEdge = ListPush(m_pEdge,node);
						}
						m_nBoundary++;
					}else{
						_p[0][0]=func(ptr->body.line.ptStart);
						_p[0][1]=func(ptr->body.line.ptEnd);
						double dv=fabs(_p[0][1].v - _p[0][0].v);
						if((fabs(dv - 2*PI) < 0.0000000001)){
							_p[0][1].v = _p[0][0].v = 0.;
						}

						KSurface::pt[m_nBoundaryPoint].x=_p[0][0].u;
						KSurface::pt[m_nBoundaryPoint].y=_p[0][0].v;
						KSurface::pt[m_nBoundaryPoint].z=0;
						m_nBoundaryPoint++;
						KSurface::pt[m_nBoundaryPoint].x=_p[0][1].u;
						KSurface::pt[m_nBoundaryPoint].y=_p[0][1].v;
						KSurface::pt[m_nBoundaryPoint].z=0;
						m_nBoundaryPoint++;

						snode<SQLINE>* node=(snode<SQLINE>*)calloc(1,sizeof(snode<SQLINE>));
						if(node){
							node->element.ptStart.x = _p[0][0].u;
							node->element.ptStart.y = _p[0][0].v;
							node->element.ptStart.z = 0;
							node->element.ptEnd.x = _p[0][1].u;
							node->element.ptEnd.y = _p[0][1].v;
							node->element.ptEnd.z = 0;

							m_pEdge = ListPush(m_pEdge,node);
						}
						m_nBoundary++;
					}
					break;
				case SQ_ARC:{
					int nCount=CreateArcBoundary(_p[0],ptr);
					if(nCount){
						m_Boundary[m_nBoundary].vecNorm=ptr->body.arc.vecNorm;
						m_nBoundary++;
						
						for(int i = 0;i < nCount - 1;i++){
							if((fabs(_p[0][i+1].v - _p[0][i].v) < 0.5*PI)){
								SQPOINT _pt[2]={0};

								_pt[0].x = _p[0][i].u;
								_pt[0].y = _p[0][i].v;
								_pt[0].z = 0;
								_pt[1].x = _p[0][i+1].u;
								_pt[1].y = _p[0][i+1].v;
								_pt[1].z = 0;
								if(!KGeometry::IsSamePoint(_pt[0],_pt[1],0.0000000001)){
									snode<SQLINE>* node=(snode<SQLINE>*)calloc(1,sizeof(snode<SQLINE>));
									if(node){
										node->element.ptStart.x = _p[0][i].u;
										node->element.ptStart.y = _p[0][i].v;
										node->element.ptStart.z = 0;
										node->element.ptEnd.x = _p[0][i+1].u;
										node->element.ptEnd.y = _p[0][i+1].v;
										node->element.ptEnd.z = 0;
										
										m_pEdge = ListPush(m_pEdge,node);
									}

									KSurface::pt[m_nBoundaryPoint].x =_p[0][i].u;
									KSurface::pt[m_nBoundaryPoint].y =_p[0][i].v;
									KSurface::pt[m_nBoundaryPoint].z =0;
									m_nBoundaryPoint++;
								}
							}

							KSurface::pt[m_nBoundaryPoint].x = _p[0][nCount - 1].u;
							KSurface::pt[m_nBoundaryPoint].y = _p[0][nCount - 1].v;
							KSurface::pt[m_nBoundaryPoint].z = 0;
							m_nBoundaryPoint++;
						}
					}
					    }
				break;
				case SQ_INTCURVE:{
					int nCount=CreateIntCurveBoundary(_p[0],ptr);
					if(nCount){
						m_Boundary[m_nBoundary].vecNorm=ptr->body.intcurve.vecNorm;
						m_nBoundary++;

						for(int i = 0;i < nCount - 1;i++){
							if((fabs(_p[0][i+1].v - _p[0][i].v) < 0.5*PI)){
								snode<SQLINE>* node=(snode<SQLINE>*)calloc(1,sizeof(snode<SQLINE>));
								if(node){
									node->element.ptStart.x = _p[0][i].u;
									node->element.ptStart.y = _p[0][i].v;
									node->element.ptStart.z = 0;
									node->element.ptEnd.x = _p[0][i+1].u;
									node->element.ptEnd.y = _p[0][i+1].v;
									node->element.ptEnd.z = 0;
									
									m_pEdge = ListPush(m_pEdge,node);
								}
								KSurface::pt[m_nBoundaryPoint].x=_p[0][i].u;
								KSurface::pt[m_nBoundaryPoint].y=_p[0][i].v;
								KSurface::pt[m_nBoundaryPoint].z=0;
								m_nBoundaryPoint++;
							}
						}
						
						KSurface::pt[m_nBoundaryPoint].x=_p[0][nCount - 1].u;
						KSurface::pt[m_nBoundaryPoint].y=_p[0][nCount - 1].v;
						KSurface::pt[m_nBoundaryPoint].z=0;
						m_nBoundaryPoint++;
					}
					       }
				break;
			}
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
SQPOINT KConeSurface::func(double t,double phi){
	SQPOINT ptRet={0};
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
	KGeometry::GetRotatedAngleOfAxis(m_sfcCone.vecAxis,alpha,beta);
	ptRet = KGeometry::RotateAboutYXAxis(ptRet,-beta,-alpha);
	ptRet.x += m_sfcCone.ptOrigin.x;
	ptRet.y += m_sfcCone.ptOrigin.y;
	ptRet.z += m_sfcCone.ptOrigin.z;

	return ptRet;
}

//	parameter	:
//	description	: calculate t and phi for pt.
//	remarks		:
//	returns		:
SURFACEPARA KConeSurface::func(const SQPOINT& pt){
	SURFACEPARA para={0.,0.};

	double dir=(m_sfcCone.ratio.b < 0) ? -1 : 1;
	double a=dir*m_sfcCone.vecAxis.dx;
	double b=dir*m_sfcCone.vecAxis.dy;
	double c=dir*m_sfcCone.vecAxis.dz;
	double d=-(a*pt.x + b*pt.y + c*pt.z);
	para.u  = -(a*m_sfcCone.ptOrigin.x + b*m_sfcCone.ptOrigin.y + c*m_sfcCone.ptOrigin.z + d)/
		(a*a + b*b + c*c);

	double alpha=0,beta=0;
	KGeometry::GetRotatedAngleOfAxis(m_sfcCone.vecAxis,alpha,beta);
	SQPOINT _pt={0};
	_pt.x = pt.x - m_sfcCone.ptOrigin.x;
	_pt.y = pt.y - m_sfcCone.ptOrigin.y;
	_pt.z = pt.z - m_sfcCone.ptOrigin.z;
	_pt = KGeometry::RotateAboutXYAxis(_pt,alpha,beta);
	_pt.z  = 0.;
	para.v = KGeometry::GetRotatedAngleInXYPlane(_pt);

	return para;
}

//	parameter	:
//	description	: pt1 and pt1 are same points.
//	remarks		: check two points locate in tolerance.
//	returns		: return true if two points are same points otherwise return flase.
bool KConeSurface::IsSamePoint(const SQPOINT& pt1,const SQPOINT& pt2){
	double dx=pt2.x - pt1.x;
	double dy=pt2.y - pt1.y;
	double dz=pt2.z - pt1.z;

	if((fabs(dx) < 0.0000000001) && (fabs(dy) < 0.0000000001) && (fabs(dz) < 0.0000000001))
		return true;
	
	return false;
}

//	parameter	:
//	description	: check whether line is bounday edge or not.
//	remarks		:
//	returns		: return true if line is bounday edge.
bool KConeSurface::IsBoundaryEdge(const SQLINE& line){
	bool bRet=false;

	if(m_pEdge){
		for(snode<SQLINE>* ptr=m_pEdge;ptr;ptr=ptr->next){
			if(	KGeometry::IsSamePoint(ptr->element.ptStart,line.ptStart,0.0000000001) && 
				KGeometry::IsSamePoint(ptr->element.ptEnd,line.ptEnd,0.0000000001)){
				bRet = true;
				break;
			}
		}
	}

	return bRet;
}

//	parameter	:
//	description	: find grid has an edge and exist left side of the edge.
//	remarks		:
//	returns		: outboundgrid - true , not - false
bool KConeSurface::IsOutboundGrid(int& nIndex,KGrid* pGrid){
	assert(pGrid && "pGrid is NULL");

	if(pGrid){
		SQPOINT ptMid={0};
		for(snode<SQLINE>* ptr=m_pEdge;ptr;ptr=ptr->next){
			if(nIndex = pGrid->HasEdge(ptr->element)){
				if(pGrid->IsLeftSide(nIndex,ptr->element)){
					return true;
				}else	return false;
			}
		}
	}

	return false;
}

//	parameter	:
//	description	: first, erase outbound grid and then
//			: erase adjacent grid of outbound grid.
//	remarks		:
//	returns		:
void KConeSurface::RemoveOutboundGrids(list<KGrid*>* plstGrid){
	assert(plstGrid && "plstGrid is NULL");
	
	if(plstGrid){
		CQueue<SQLINE> queEdge;

		int nIndex=0;
		for(list<KGrid*>::iterator itr=plstGrid->begin();itr != plstGrid->end();){
			if(IsOutboundGrid(nIndex,*itr)){
				int i=1;
				for(snode<SQPOINT>* ptr=(*itr)->m_pNode;ptr && ptr->next;ptr=ptr->next,i++){
					SQLINE line={0};
					
					line.ptStart = ptr->element;
					line.ptEnd   = ptr->next->element;
					if((i != nIndex) && !IsBoundaryEdge(line)){
						queEdge.Push(line);
					}
				}
				SQLINE line={0};
				
				line.ptStart = ptr->element;
				line.ptEnd   = (*itr)->m_pNode->element;
				if((i != nIndex) && !IsBoundaryEdge(line)){
					queEdge.Push(line);
				}
				plstGrid->erase(itr++);
			}else	itr++;
		}

		while(!queEdge.IsEmpty()){
			SQPOINT pt={0};
			SQLINE  line={0.};
			queEdge.Pop(line);
			for(list<KGrid*>::iterator itr = plstGrid->begin();itr != plstGrid->end();){
				if((*itr)->DoIntersection(pt,line)){
					list<KGrid*> lstGridTmp;

					KGrid* pGrid=*itr;
					(*itr)->InsertPoint(lstGridTmp,pt);
					plstGrid->insert(plstGrid->end(),lstGridTmp.begin(),lstGridTmp.end());
					plstGrid->erase(itr++);
					
					SQPOINT _pt=line.ptEnd;
					line.ptEnd = pt;
					queEdge.Push(line);
					line.ptStart = pt;
					line.ptEnd   = _pt;
					queEdge.Push(line);
					break;
				}else if(nIndex = (*itr)->HasEdge(line)){
					KGrid* pGrid=*itr;
					int i=1;
					for(snode<SQPOINT>* ptr=(*itr)->m_pNode;ptr && ptr->next;ptr=ptr->next,i++){
						if(i != nIndex){
							SQLINE _line={0};
							
							_line.ptStart = ptr->element;
							_line.ptEnd   = ptr->next->element;
							queEdge.Push(_line);
						}
					}
					if(i != nIndex){
						SQLINE _line={0};
						
						_line.ptStart = ptr->element;
						_line.ptEnd   = (*itr)->m_pNode->element;
						queEdge.Push(_line);
					}
					plstGrid->erase(itr++);
					break;
				}else	itr++;
			}
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KConeSurface::CreateGrids(list<KGrid*>& lstGrid,double tMin,double tMax){
	SQPOINT pt[4]={0};

	double nStep=(2*PI)/(KCurve::NumOfSegments);
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
		KGrid* pGrid=NULL;
		if(pGrid = new KGrid(pt)){
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
	KGrid* pGrid=NULL;
	if(pGrid = new KGrid(pt)){
		lstGrid.push_back(pGrid);
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
list<KFacet*>* KConeSurface::CreateFacets(){
	assert(m_pEdge && "m_pEdge is NULL");

	if(m_pEdge){
		double tMin=KSurface::pt[0].x;
		double tMax=tMin;
		double vMin=KSurface::pt[0].y;
		double vMax=vMin;
		for(int i=0;i < m_nBoundaryPoint;i++){
			if(tMin > KSurface::pt[i].x) tMin = KSurface::pt[i].x;
			if(tMax < KSurface::pt[i].x) tMax = KSurface::pt[i].x;
			if(vMin > KSurface::pt[i].y) vMin = KSurface::pt[i].y;
			if(vMax < KSurface::pt[i].y) vMax = KSurface::pt[i].y;
		}
		list<KGrid*> lstGrid;
		CreateGrids(lstGrid,tMin,tMax);
		list<KGrid*>* plstGrid=&(lstGrid);

		if(m_nBoundary >= 4){
			long nIndex=0;

			KMesh mesh;
			mesh.InsertGrids(lstGrid);
			mesh.InsertGridPoints(KSurface::pt,m_nBoundaryPoint);

			mesh.InsertGridEdges(m_pEdge);
			plstGrid = mesh.GetListOfGrid();

			list<KGrid*> lstGridTmp;
			for(snode<SQLINE>* ptr=m_pEdge;ptr;ptr=ptr->next){
				for(list<KGrid*>::iterator itr=plstGrid->begin();itr != plstGrid->end();itr++){
					KGrid* pGrid=*itr;
					if((*itr)->IsDiagonalLine(ptr->element)){
						lstGridTmp.push_back((*itr)->Split(ptr->element));
					}
				}
			}
			plstGrid->insert(plstGrid->end(),lstGridTmp.begin(),lstGridTmp.end());
			RemoveOutboundGrids(plstGrid);
			//if(m_sfcCone.vecAxis.dz > 0)
			//	RemoveOutboundGrids(plstGrid,false);
			//else	RemoveOutboundGrids(plstGrid,true);
			
			/*
			FILE* fp=NULL;
			fp = fopen("d:\\boundary.txt","wb");
			for(ptr=m_pEdge;ptr;ptr = ptr->next){
				KFacet* pFacet=NULL;
				
				SQPOINT _pt[4]={0};
				_pt[0].x = ptr->element.ptStart.x;
				_pt[0].y = ptr->element.ptStart.y;
				_pt[0].z = 0;
				_pt[1].x = ptr->element.ptEnd.x;
				_pt[1].y = ptr->element.ptEnd.y;
				_pt[1].z = 0;
				_pt[2].x = _pt[0].x;
				_pt[2].y = _pt[0].y;
				_pt[2].z = _pt[0].z;
				pFacet = new KFacet(_pt[0],_pt[1],_pt[2]);
				//pFacet = new KFacet(func(_pt[0].x,_pt[0].y),
				//	func(_pt[1].x,_pt[1].y),
				//	func(_pt[2].x,_pt[2].y));
				m_lstFacet.push_back(pFacet);
				
				
				fprintf(fp,"%lf %lf %lf %lf\r\n",_pt[0].x,_pt[0].y,_pt[1].x,_pt[1].y);
			}
			fclose(fp);
			*/

			for(list<KGrid*>::iterator itr=plstGrid->begin();itr != plstGrid->end();){
				KFacet* pFacet=NULL;
				
				SQPOINT pt[4]={0};
				int nIndex=0;
				for(snode<SQPOINT>* ptr=(*itr)->m_pNode;ptr;ptr = ptr->next,nIndex++){
					pt[nIndex].x = ptr->element.x;
					pt[nIndex].y = ptr->element.y;
					pt[nIndex].z = 0;
				}
				
				/*
				if(4 == nIndex){
				pFacet = new KFacet(pt[0],pt[1],pt[2],pt[3]);
				}else if(3 == nIndex){
				pFacet = new KFacet(pt[0],pt[1],pt[2]);
				}
				*/				
				if(4 == nIndex){
					pFacet = new KFacet(func(pt[0].x,pt[0].y),func(pt[1].x,pt[1].y),
						func(pt[2].x,pt[2].y),func(pt[3].x,pt[3].y));
				}else if(3 == nIndex){
					pFacet = new KFacet(func(pt[0].x,pt[0].y),func(pt[1].x,pt[1].y),func(pt[2].x,pt[2].y));
				}
				m_lstFacet.push_back(pFacet);
				
				delete (*itr);
				plstGrid->erase(itr++);
			}
		}else{
			for(list<KGrid*>::iterator itr=plstGrid->begin();itr != plstGrid->end();){
				KFacet* pFacet=NULL;
				
				SQPOINT pt[4]={0};
				int nIndex=0;
				for(snode<SQPOINT>* ptr=(*itr)->m_pNode;ptr;ptr = ptr->next,nIndex++){
					pt[nIndex].x = ptr->element.x;
					pt[nIndex].y = ptr->element.y;
					pt[nIndex].z = 0;
				}
				
				/*
				if(4 == nIndex){
				pFacet = new KFacet(pt[0],pt[1],pt[2],pt[3]);
				}else if(3 == nIndex){
				pFacet = new KFacet(pt[0],pt[1],pt[2]);
				}
				*/
				
				if(4 == nIndex){
					pFacet = new KFacet(func(pt[0].x,pt[0].y),func(pt[1].x,pt[1].y),
						func(pt[2].x,pt[2].y),func(pt[3].x,pt[3].y));
				}else if(3 == nIndex){
					pFacet = new KFacet(func(pt[0].x,pt[0].y),func(pt[1].x,pt[1].y),func(pt[2].x,pt[2].y));
				}
				m_lstFacet.push_back(pFacet);
				
				delete (*itr);
				plstGrid->erase(itr++);
			}
		}
	}

	return &m_lstFacet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
int KConeSurface::GetNumOfBoundary(){
	int n=0;
	for(PSQPRIMITIVE ptr=m_sfcCone.pBoundary;ptr;ptr = ptr->next,n++);
	
	return n;
}