//#include "d:\\project\\spoon_0_0_1\\utility\\profile.h"
#include "StdAfx.h"
#include <graphics\geometry.h>
#include <graphics/plane3d.h>
#include <graphics/arc3d.h>
#include "libmesh/libmesh2d.h"
#include "PlaneSurface.h"

#include <algorithm>
#include <queue>

using namespace mesh;
using namespace std;

/**	
	@brief
*/
CPlaneSurface::CPlaneSurface()
{
	m_sfcPlane.pBoundary = NULL;
	m_nPoints = 0;
	m_nEdge   = 0;

	m_plstBoundaryCurve = new vector<CPolyCurve*>;
	m_pPointEntry = new vector<CIsPoint2d*>;
	m_pDelaunayTriEdgeEntry = new vector<pair<CIsPoint2d* , CIsPoint2d*> >;
	m_pEdgeIndexEntry = new vector<EdgeIndex>;
}

/**	
	@brief
*/
CPlaneSurface::CPlaneSurface(PLANESURF_T& PlaneSurface)
{
	m_sfcPlane = PlaneSurface;
	m_nPoints  = 0;
	m_nEdge    = 0;
	m_plstBoundaryCurve = new vector<CPolyCurve*>;
	m_pPointEntry = new vector<CIsPoint2d*>;
	m_pDelaunayTriEdgeEntry = new vector<pair<CIsPoint2d* , CIsPoint2d*> >;
	m_pEdgeIndexEntry = new vector<EdgeIndex>;

	OnCreate();
}

/**	
	@brief
*/
CPlaneSurface::~CPlaneSurface()
{
	try
	{
		if(m_plstBoundaryCurve)
		{
			for(vector<CPolyCurve*>::iterator itr = m_plstBoundaryCurve->begin() ; itr != m_plstBoundaryCurve->end();itr++)
			{
				delete (*itr);
			}
			delete m_plstBoundaryCurve;
			m_plstBoundaryCurve = NULL;
		}

		delete m_pPointEntry;
		delete m_pDelaunayTriEdgeEntry;
		delete m_pEdgeIndexEntry;

		///delete m_pt;
	}
	catch(...)
	{
	}
}

/**	
	@brief	The CPlaneSurface::OnCreate function

	@author	humkyung

	@date	????.??.??

	@return	void	
*/
void CPlaneSurface::OnCreate()
{
	CPlane3d plane(m_sfcPlane.vecNorm);
	double alpha=0.,beta=0;
	plane.GetRotatedAngleOfAxis(alpha,beta);

	long nStart = 0 , nEnd = 0;
	POINT_T pt={0};
	for(PSQPRIMITIVE pGroup=m_sfcPlane.pBoundary;pGroup;pGroup = pGroup->next)
	{
		vector<POINT_T> pts;
		for(PSQPRIMITIVE ptr=pGroup->body.pPrimitive;ptr;ptr = ptr->next)
		{
			switch(ptr->hdr.id)
			{
				case ENM_LINE:
					if(!ptr->hdr.attr.reversed)
					{
						pt = ptr->body.line.ptStart;
						pt.x -= m_sfcPlane.ptOrigin.x;
						pt.y -= m_sfcPlane.ptOrigin.y;
						pt.z -= m_sfcPlane.ptOrigin.z;
						pt = plane.RotateAboutXYAxis(pt,alpha,beta);
						pt.z = 0.;
						AppendPoint(pts , pt);

						pt = ptr->body.line.ptEnd;
						pt.x -= m_sfcPlane.ptOrigin.x;
						pt.y -= m_sfcPlane.ptOrigin.y;
						pt.z -= m_sfcPlane.ptOrigin.z;
						pt = plane.RotateAboutXYAxis(pt,alpha,beta);
						pt.z = 0.;
						AppendPoint(pts , pt);
					}
					else
					{
						pt = ptr->body.line.ptEnd;
						pt.x -= m_sfcPlane.ptOrigin.x;
						pt.y -= m_sfcPlane.ptOrigin.y;
						pt.z -= m_sfcPlane.ptOrigin.z;
						pt = plane.RotateAboutXYAxis(pt,alpha,beta);
						pt.z = 0.;
						AppendPoint(pts , pt);

						pt = ptr->body.line.ptStart;
						pt.x -= m_sfcPlane.ptOrigin.x;
						pt.y -= m_sfcPlane.ptOrigin.y;
						pt.z -= m_sfcPlane.ptOrigin.z;
						pt = plane.RotateAboutXYAxis(pt,alpha,beta);
						pt.z = 0.;
						AppendPoint(pts , pt);
					}
				break;
				case SQ_POINTS:
					if(!ptr->hdr.attr.reversed)
					{
						for(vector<POINT_T>::iterator itr=ptr->body.pvtPoints->begin();itr != ptr->body.pvtPoints->end();++itr)
						{
							pt = *itr;
							pt.x -= m_sfcPlane.ptOrigin.x;
							pt.y -= m_sfcPlane.ptOrigin.y;
							pt.z -= m_sfcPlane.ptOrigin.z;
							pt = plane.RotateAboutXYAxis(pt,alpha,beta);
							//pt.z = 0.;
							AppendPoint(pts , pt);
						}
					}
					else
					{
						for(vector<POINT_T>::reverse_iterator itr=ptr->body.pvtPoints->rbegin();itr != ptr->body.pvtPoints->rend();++itr)
						{
							pt = *itr;
							pt.x -= m_sfcPlane.ptOrigin.x;
							pt.y -= m_sfcPlane.ptOrigin.y;
							pt.z -= m_sfcPlane.ptOrigin.z;
							pt = plane.RotateAboutXYAxis(pt,alpha,beta);
							//pt.z = 0.;
							AppendPoint(pts , pt);
						}
					}
					break;
				case ENM_ARC:
				{
					CArc3d arc(ptr->body.arc);
					arc.CreateSegments();
					for(int i=0;i < arc.GetNumOfPoints();i++)
					{
						pt    = CCurve3d::pt(i);
						pt.x -= m_sfcPlane.ptOrigin.x;
						pt.y -= m_sfcPlane.ptOrigin.y;
						pt.z -= m_sfcPlane.ptOrigin.z;
						pt = CGeometry::RotateAboutXYAxis(pt,alpha,beta);
						pt.z = 0.;
						AppendPoint(pts , pt);
					}
				}
				break;
			}
		}
		if(!pts.empty() && pGroup->hdr.attr.reversed) reverse(pts.begin() , pts.end());

		if(!pts.empty())///nEnd > nStart)
		{
			int iStart = m_pPointEntry->size();
			for(vector<POINT_T>::iterator itr = pts.begin();itr != pts.end();++itr)///i = nStart;i < nEnd ;++i)
			{
				CIsPoint2d* p = new CIsPoint2d(itr->x , itr->y);
				m_pPointEntry->push_back( p );
			}

			for(vector<POINT_T>::iterator itr = pts.begin();itr != (pts.end() - 1);++itr)///i = nStart;i < nEnd - 1;++i)
			{			
				///CDelaunayTriEdge* pEdge = new CDelaunayTriEdge( (*m_pPointEntry)[iStart] , (*m_pPointEntry)[iStart + 1] );
				///m_pDelaunayTriEdgeEntry->push_back( pair<CIsPoint2d*,CIsPoint2d*>(m_pPointEntry[iStart] , m_pPointEntry[iStart+1]) );

				EdgeIndex edgeIndex;
				edgeIndex.start = iStart;
				edgeIndex.end   = iStart + 1;
				m_pEdgeIndexEntry->push_back( edgeIndex );
				iStart++;
				m_nEdge++;
			}
			
			/// add boundary curve - 2012.02.02 added by humkyung
			CPolyCurve* pPolyCurve = new CPolyCurve(pts.size() , &(pts[0]));
			if(pPolyCurve)
			{
				m_plstBoundaryCurve->push_back(pPolyCurve);
			}
			/// up to here
		}
	}
}

/**	
	@brief	The CPlaneSurface::IsBoundaryEdge function

	@author	HumKyung.Baek

	@date	????.??.??

	@param	pt1	a parameter of type const POINT_T&
	@param	pt2	a parameter of type const POINT_T&

	@return	bool	
*/
bool CPlaneSurface::IsBoundaryEdge(const CIsPoint2d *ppt1,const CIsPoint2d *ppt2)
{
	bool bRet=false;

	for(vector<EdgeIndex>::iterator itr = m_pEdgeIndexEntry->begin();itr != m_pEdgeIndexEntry->end();++itr)
	{
		CIsPoint2d* pStart = (*m_pPointEntry)[itr->start];
		CIsPoint2d* pEnd   = (*m_pPointEntry)[itr->end];
		if(((ppt1 == pStart) && (ppt2 == pEnd)) ||
		       ((ppt1 == pEnd) && (ppt2 == pStart)))
		{
			return true;
		}
	}

	return false;
}

#include <fstream>

/**	
	@brief	The CPlaneSurface::CreateFacets function

	@author	BHK

	@date	?

	@param	

	@return	CTinyFacetContainer*
*/
CTinyFacetContainer* CPlaneSurface::CreateFacets()
{
	try
	{
		CTinyFacetContainer* pFacetContainer = CTinyFacetContainer::CreateInstance();
		if(NULL == pFacetContainer) return NULL;
		
		ofstream ofile("E:\\PROJECT\\hTech\\Projects\\aUtil\\LibMesh2dTestCaseVS2008\\TestData\\TEST3.DAT");
		if(ofile.is_open())
		{
			int iIndex = 1;
			ofile << "POINTS " << m_pPointEntry->size() << std::endl;
			
			{
				for(vector<CIsPoint2d*>::iterator itr = m_pPointEntry->begin();itr != m_pPointEntry->end();++itr)
				{
					ofile << iIndex << "    " << (*itr)->x() << "," << (*itr)->y() << std::endl;
					iIndex++;
				}
			}

			ofile << "EDGES " << m_pEdgeIndexEntry->size() << std::endl;
			{
				iIndex = 1;
				for(vector<EdgeIndex>::iterator itr = m_pEdgeIndexEntry->begin();itr != m_pEdgeIndexEntry->end();++itr)
				{
					ofile << iIndex << "    " << (itr->start + 1) << "," << (itr->end + 1) << std::endl;
					iIndex++;
				}
			}

			ofile.close();
		}

		vector<long> oEdgeIndexList;
		for(vector<EdgeIndex>::iterator itr = m_pEdgeIndexEntry->begin();itr != m_pEdgeIndexEntry->end();++itr)
		{
			oEdgeIndexList.push_back(itr->start);
			oEdgeIndexList.push_back(itr->end);
		}
		list<CDelaunayTri*>* plstDT = aTech_Triangulate(&(*m_pPointEntry)[0] , m_pPointEntry->size() , 
			(!oEdgeIndexList.empty() ? &(oEdgeIndexList[0]) : NULL) , oEdgeIndexList.size()*0.5 , 15 , false);

		if(plstDT)
		{
			CPlane3d plane(m_sfcPlane.vecNorm);
			double alpha=0.,beta=0;
			CGeometry::GetRotatedAngleOfAxis(m_sfcPlane.vecNorm,alpha,beta);
			for(list<CDelaunayTri*>::iterator itr=plstDT->begin();itr != plstDT->end();++itr)
			{
				POINT_T pt[3]={0 , };
				bool    bShow[3]={false , false , false};

				pt[0].x = (*itr)->vertex(0)->pt()->x();
				pt[0].y = (*itr)->vertex(0)->pt()->y();
				pt[0].z = 0.f;
				pt[1].x = (*itr)->vertex(1)->pt()->x();
				pt[1].y = (*itr)->vertex(1)->pt()->y();
				pt[1].z = 0.f;
				pt[2].x = (*itr)->vertex(2)->pt()->x();
				pt[2].y = (*itr)->vertex(2)->pt()->y();
				pt[2].z = 0.f;

				if(IsBoundaryEdge((*itr)->vertex(0)->pt() , (*itr)->vertex(1)->pt())) bShow[0] = true;
				if(IsBoundaryEdge((*itr)->vertex(1)->pt() , (*itr)->vertex(2)->pt())) bShow[1] = true;
				if(IsBoundaryEdge((*itr)->vertex(2)->pt() , (*itr)->vertex(0)->pt())) bShow[2] = true;

				pt[0] = plane.RotateAboutYXAxis(pt[0],-beta,-alpha);
				pt[0].x += m_sfcPlane.ptOrigin.x;
				pt[0].y += m_sfcPlane.ptOrigin.y;
				pt[0].z += m_sfcPlane.ptOrigin.z;
				pt[1] = plane.RotateAboutYXAxis(pt[1],-beta,-alpha);
				pt[1].x += m_sfcPlane.ptOrigin.x;
				pt[1].y += m_sfcPlane.ptOrigin.y;
				pt[1].z += m_sfcPlane.ptOrigin.z;
				pt[2] = plane.RotateAboutYXAxis(pt[2],-beta,-alpha);
				pt[2].x += m_sfcPlane.ptOrigin.x;
				pt[2].y += m_sfcPlane.ptOrigin.y;
				pt[2].z += m_sfcPlane.ptOrigin.z;

				CFacet* pFacet=NULL;
				if(pFacet = new CFacet(pt[0],pt[1],pt[2]))
				{
					pFacet->m_bShow[0] = bShow[0];
					pFacet->m_bShow[1] = bShow[1];
					pFacet->m_bShow[2] = bShow[2];
					pFacetContainer->Add(pFacet);
				}
			}
		}
		
		{
			for(vector<CIsPoint2d*>::iterator itr = m_pPointEntry->begin();itr != m_pPointEntry->end();++itr)
			{
				delete (*itr);
			}
			m_pPointEntry->clear();

			/*for(vector<CDelaunayTriEdge*>::iterator jtr = m_pDelaunayTriEdgeEntry->begin();jtr != m_pDelaunayTriEdgeEntry->end();++jtr)
			{
				delete (*jtr);
			}
			m_pDelaunayTriEdgeEntry->clear();*/
		}

		return pFacetContainer;
	}
	catch(...)
	{
/*
///#ifdef	NDEBUG
		int i = 0;
		{
			ofstream ofile("C:\\TEMP\\TEST.DAT");
			ofile.precision( 9 );         // 소수점 자릿수 9
			ofile.setf(ios_base:: fixed, ios_base:: floatfield); // 소수점 방식으로 표시됨
			
			ofile << "POINTS " << m_pPointEntry->size() << std::endl;
			char szBuf[32] = {'\0',};
			for(vector<CIsPoint2d*>::iterator itr = m_pPointEntry->begin();itr != m_pPointEntry->end();++itr,++i)
			{
				sprintf(szBuf , "%-5d" , i+1);
				ofile << szBuf << (*itr)->x() << "," << (*itr)->y() << std::endl;
			}
			
			{
				i = 0;
				ofile << "EDGES " << m_pEdgeIndexEntry->size() << std::endl;
				for(vector<EdgeIndex>::iterator itr = m_pEdgeIndexEntry->begin();itr != m_pEdgeIndexEntry->end();++itr,++i)
				{
					sprintf(szBuf , "%-5d" , i+1);
					ofile << szBuf << (itr->start+1) << "," << (itr->end + 1) << std::endl;
				}
			}
			ofile.close();
		}
///#endif
	///	list<CDT*>* plstDT=pInst->Mesh(m_pt,m_nPoints,m_lnBoundary,m_nEdge);
*/
		::OutputDebugString("throw exception in CPlaneSurface::CreateFacets");
	}

	return NULL;
}

/**	
	@brief	The CPlaneSurface::AppendPoint function

	@author	humkyung

	@param	pt	a parameter of type POINT_T&

	@return	void	
*/
bool CPlaneSurface::AppendPoint(vector<POINT_T>& pts , POINT_T& pt)
{
	if(!pts.empty())
	{
		POINT_T ptLast = pts.back();///(pts)[m_nPoints - 1];
		if(IsSamePoint(ptLast , pt)) return false;
	}
	pts.push_back(pt);///(*m_pt)[m_nPoints++] = pt;
	
	return false;
}

/**
 * GetBoundaryCurve:
 *
 * @param index 
 * @return CPolyCurve* 
 */
CPolyCurve* CPlaneSurface::GetBoundaryCurve(const long &index)
{
	if(index < int(m_plstBoundaryCurve->size()))
	{
		return (*m_plstBoundaryCurve)[index];
	}

	return NULL;
}
