// PlaneSurfElement.cpp: implementation of the CPlaneSurfElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlaneSurfElement.h"
#include "SmartHSR.h"
#include "LoopElement.h"

#include <sstream>
#include <IsPoint3d.h>
#include <IsPlane3d.h>
#include <DelaunayMesh.h>

IMPLEMENTS_HSR_ELEMENT(CPlaneSurfElement , CHSRElement , _T("PLANESURF="));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlaneSurfElement::CPlaneSurfElement()
{

}

/**
	@brief	dcstr

	@author	BHK
*/
CPlaneSurfElement::~CPlaneSurfElement()
{
	try
	{
		for(list<CHSRElement*>::iterator itr = m_BoundaryElemEntry.begin();itr != m_BoundaryElemEntry.end();++itr)
		{
			delete (*itr);
		}
		m_BoundaryElemEntry.clear();
	}
	catch(...)
	{
	}
}

/**
	@brief	parse surface element.

	@author	BHK

	@date	2009.10.08

	@param	pHSRScanner

	@return	
*/
bool CPlaneSurfElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;

		//! must be '{' - start of PLANESURF
		pHSRScanner->ReadLine();
		if('{' != (*pHSRScanner->m_aryFactor)[0][0]) throw exception("expect '{'");

		int nOpened = 1;
		while(pHSRScanner->ReadLine())
		{
			//! if find end mark of PLANESURF, break loop
			if('}' == (*pHSRScanner->m_aryFactor)[0][0]) --nOpened;///break;
			if('{' == (*pHSRScanner->m_aryFactor)[0][0]) ++nOpened;
			if(0 == nOpened) break;

			///if(string("LOOP=") == string((*pHSRScanner->m_aryFactor)[0]))
			CHSRElement* pElm = CHSRParser::CreateElement((*pHSRScanner->m_aryFactor)[0].c_str());
			if(pElm)
			{	
				CLoopElement* pLoopElm = static_cast<CLoopElement*>(pElm);///CLoopElement::CreateInstance();
				if(pLoopElm)
				{
					try
					{
						if(pLoopElm->ParseLine(pHSRScanner))
							m_BoundaryElemEntry.push_back(pLoopElm);
					}
					catch(const char* es)
					{
						throw exception(es);
					}
				}
			}else	throw exception("unexpected subitem in PLANESURF");
		}

		CalcVolume();
		bRet = true;
	}

	return bRet;
}

/**
	@brief	create a instance of CPlaneSurfElement

	@author	BHK

	@date	2009.07.25
	
	@return	pointer of CPlaneSurfElement*
*/
/*
CHSRElement* CPlaneSurfElement::CreateInstance()
{
	return (new CPlaneSurfElement);
}
*/

/**	
	@brief	return plane surface identifier.
	
	@author	BHK

	@date	2009.09.25

	@return	string plane surface identifier.
*/
/*
string CPlaneSurfElement::TypeString()
{
	static const string __typeString("PLANESURF=");

	return __typeString;
}
*/

/**	
	@brief	뷰의 볼륨에 속하는지 검사를 한다.
	
	@author	BHK

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CPlaneSurfElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;
}

/**	
	@brief	calculate volume of PlaneSurface.
	
	@author	BHK

	@date	2009.10.08

	@param	

	@return	
*/
void CPlaneSurfElement::CalcVolume()
{
	if(!m_BoundaryElemEntry.empty())
	{
		bool bFirst = true;
		for(list<CHSRElement*>::iterator itr = m_BoundaryElemEntry.begin();itr != m_BoundaryElemEntry.end();++itr)
		{
			if(bFirst)
			{
				m_vol = (*itr)->volume();
				bFirst = false;
			}
			else
			{
				VOLUME_T vol = (*itr)->volume();

				if(m_vol.x[0] > vol.x[0]) m_vol.x[0] = vol.x[0];
				if(m_vol.x[1] < vol.x[1]) m_vol.x[1] = vol.x[1];
				if(m_vol.y[0] > vol.y[0]) m_vol.y[0] = vol.y[0];
				if(m_vol.y[1] < vol.y[1]) m_vol.y[1] = vol.y[1];
				if(m_vol.z[0] > vol.z[0]) m_vol.z[0] = vol.z[0];
				if(m_vol.z[1] < vol.z[1]) m_vol.z[1] = vol.z[1];
			}
		}
	}
}

static double PolygonArea(vector<POINT_T>& input)
{
	const int nInputSize = input.size();

	double area = 0.f;
	for(int p = nInputSize - 1,q = 0;q < nInputSize;p = q++)
	{
		area += input[p].x*input[q].y - input[p].y*input[q].x;
	}

	return area;
}

struct __SamePoint2d__ : public binary_function<CIsPoint2d* , CIsPoint2d , bool>
{
	bool operator()(CIsPoint2d* lhs , const CIsPoint2d& rhs) const
	{
		const double dx = fabs(lhs->x() - rhs.x());
		const double dy = fabs(lhs->y() - rhs.y());

		return ((dx <= 0.00000001) && (dy <= 0.00000001));
	}
};

/**
	@brief	create surface for Plane Surface.

	@author	BHK

	@date	2009.10.08

	@param	pViewFormat
	@param	nID

	@return	always return NULL
*/
CHSRFace* CPlaneSurfElement::CreateFace(CHSRViewFormat* pViewFormat,long& nID)
{
	assert(pViewFormat && "pViewFormat is NULL");

	if(pViewFormat && !m_BoundaryElemEntry.empty())
	{
		if(1 == m_BoundaryElemEntry.size())
		{
			CLoopElement* pLoopElm = static_cast<CLoopElement*>(*(m_BoundaryElemEntry.begin()));
			if(CLoopElement::SOLID == pLoopElm->GetCreationFlag())
			{
				for(list<CHSRElement*>::iterator itr = m_BoundaryElemEntry.begin();itr != m_BoundaryElemEntry.end();++itr)
				{
					CLoopElement* pLoopElm = static_cast<CLoopElement*>(*itr);
					if(pLoopElm) pLoopElm->CreateFace(pViewFormat , nID);			
				}
			}
		}
		else
		{
			CIsPoint3d ptOrigin;
			double alpha = 0.f , beta = 0.f , plane_z = 0.f;;

			int iEdgeCount = 1;
			vector<long> oEdgeIndexEntry;

			vector<CIsPoint2d*> PointEntry;
			vector<CDelaunayVertex*> oVertexEntry;
			vector<CDelaunayTriEdge*> EdgeEntry;
			for(list<CHSRElement*>::iterator itr = m_BoundaryElemEntry.begin();itr != m_BoundaryElemEntry.end();++itr)
			{
				CLoopElement* pLoopElm = static_cast<CLoopElement*>(*itr);
				if(pLoopElm)
				{
					double zhigh = 0.f;
					vector<POINT_T> pts , shapepts;
					
					pLoopElm->GetShapePointSet(shapepts , zhigh);
					//! convert 3d coordinates against view direction.
					{
						for(vector<POINT_T>::iterator jtr = shapepts.begin();jtr != shapepts.end();++jtr)
						{
							POINT_T pt;
							pt = pViewFormat->MODEL2VIEW(*jtr);
							pts.push_back(pt);
						}
					}
					
					const double area = PolygonArea(pts);
					if(CLoopElement::HOLE == pLoopElm->GetCreationFlag())
					{
						//! must be clockwise
						if(area > 0.f) std::reverse(pts.begin() , pts.end());
					}
					else if(area < 0.f)
					{
						//! must be counter clockwise
						std::reverse(pts.begin() , pts.end());
					}
					
					vector<CIsPoint3d> pts3d;
					{
						for(vector<POINT_T>::iterator jtr = pts.begin();jtr != pts.end();++jtr)
						{
							POINT_T pt = *jtr;
							pts3d.push_back(CIsPoint3d(pt.x , pt.y , pt.z));						
						}
					}
					if(itr == m_BoundaryElemEntry.begin())
					{
						for(vector<CIsPoint3d>::iterator jtr = pts3d.begin();jtr != pts3d.end();++jtr)
						{
							ptOrigin.m_x += jtr->m_x;
							ptOrigin.m_y += jtr->m_y;
							ptOrigin.m_z += jtr->m_z;
						}
						ptOrigin.m_x /= pts3d.size();
						ptOrigin.m_y /= pts3d.size();
						ptOrigin.m_z /= pts3d.size();

						CIsPlane3d plane3d;
						CIsPlane3d::CreateOf(plane3d , pts3d);
						CIsVect3d axis(plane3d.normal());
						axis.GetRotatedAngleOfAxis(alpha , beta);
					}

					vector<CIsPoint2d*> TempPointEntry;
					for(vector<CIsPoint3d>::iterator jtr = pts3d.begin();jtr != pts3d.end();++jtr)
					{
						CIsPoint3d pt3d((*jtr) - ptOrigin);
						///pt3d = pt3d.RotateAboutXYAxis(alpha , beta);
						///plane_z = pt3d.z();
						
						vector<CIsPoint2d*>::iterator where = find_if(TempPointEntry.begin() , TempPointEntry.end() , bind2nd(__SamePoint2d__() , CIsPoint2d(pt3d.x() , pt3d.y())));
						//! prevent inserting duplicated points.
						if(where == TempPointEntry.end())
						{
							CIsPoint2d* pPt = new CIsPoint2d(pt3d.x() , pt3d.y());
							CDelaunayVertex* pVertex = new CDelaunayVertex(pPt);
							oVertexEntry.push_back( pVertex );

							PointEntry.push_back(pPt);
							TempPointEntry.push_back(pPt);
						}
					}
					
					//! create edges from points.
					if(TempPointEntry.size() >= 2)
					{
						int i = 0;
						for(vector<CDelaunayVertex*>::iterator ktr = oVertexEntry.begin();(ktr + 1) != oVertexEntry.end();++ktr)
						{
							EdgeEntry.push_back(new CDelaunayTriEdge( *ktr , *(ktr + 1)));
							oEdgeIndexEntry.push_back(iEdgeCount + i);
							oEdgeIndexEntry.push_back(iEdgeCount + i + 1);
							i++;
						}
						if(TempPointEntry.size() > 2) EdgeEntry.push_back(new CDelaunayTriEdge( oVertexEntry.back() , oVertexEntry.front()));
						oEdgeIndexEntry.push_back(iEdgeCount + i);
						oEdgeIndexEntry.push_back(iEdgeCount);

						iEdgeCount = oEdgeIndexEntry.size() + 1;
					}
				}
			}

			try
			{
				list<CDelaunayTri*>* pListDT = aTech_Triangulate(&(PointEntry[0]) , PointEntry.size() , &oEdgeIndexEntry[0] , oEdgeIndexEntry.size(),0,0);
				/*aTech::CDelaunayMesh mesh_engine;
				mesh_engine.Initialize();
				mesh_engine.Triangulate(&oVertexEntry[0] , oVertexEntry.size() , &oEdgeIndexEntry[0] , oEdgeIndexEntry.size());
				///mesh_engine.InsertBoundary(&EdgeEntry[0] , EdgeEntry.size());
				mesh_engine.RemoveOutboundTriangles();*/

				CHSRApp* pHSRApp=CHSRApp::GetInstance();
				///list<CDelaunayTri*>* pListDT = mesh_engine.GetListOfDT();
				if(NULL != pListDT)
				{
					POINT_T pts[3];
					for(list<CDelaunayTri*>::iterator itr = pListDT->begin();itr != pListDT->end();++itr)
					{
						bool bShow[3] = {false , false , false};
						for(vector<CDelaunayTriEdge*>::iterator jtr = EdgeEntry.begin();jtr != EdgeEntry.end();++jtr)
						{
							CIsPoint2d* ep1 = (*jtr)->vertex(0)->pt();
							CIsPoint2d* ep2 = (*jtr)->vertex(1)->pt();

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
						}
						CDelaunayTri* pTri = (*itr);
						///bShow[0] = bShow[1] = bShow[2] = true;

						CIsPoint3d pt3d((*itr)->vertex(0)->pt()->x() , (*itr)->vertex(0)->pt()->y() , plane_z);
						///pt3d.RotateAboutYXAxis(-beta , -alpha);
						pt3d += ptOrigin;
						pts[0].x = pt3d.x();
						pts[0].y = pt3d.y();
						pts[0].z = pt3d.z();

						pt3d = CIsPoint3d((*itr)->vertex(1)->pt()->x() , (*itr)->vertex(1)->pt()->y() , plane_z);
						///pt3d.RotateAboutYXAxis(-beta , -alpha);
						pt3d += ptOrigin;
						pts[1].x = pt3d.x();
						pts[1].y = pt3d.y();
						pts[1].z = pt3d.z();

						pt3d = CIsPoint3d((*itr)->vertex(2)->pt()->x() , (*itr)->vertex(2)->pt()->y() , plane_z);
						///pt3d.RotateAboutYXAxis(-beta , -alpha);
						pt3d += ptOrigin;
						pts[2].x = pt3d.x();
						pts[2].y = pt3d.y();
						pts[2].z = pt3d.z();

						CHSRFace* pFace = CHSRFace::CreateFace(3 , pts , false);
						if(pFace)
						{
							if(false == bShow[0]) pFace->pHead->render = INVALID;
							if(false == bShow[1]) pFace->pHead->next->render = INVALID;
							if(false == bShow[2]) pFace->pHead->next->next->render = INVALID;

							pFace->type = OTHER;
							pFace->m_enmLayer = GetHSRLayer();
							pFace->id  = nID++;
							pFace->ele = NORMAL;
							if(_T("PIPE") == (*m_pstrLayer))
								pFace->m_enmLayer = LAYER_PIPE_OTHER;
							else if(_T("STRU") == (*m_pstrLayer))
								pFace->m_enmLayer = LAYER_STRU;
							else	pFace->m_enmLayer = LAYER_OTHER;
							pFace->SetColor(*m_pstrColor);
							pHSRApp->AppendFace(NULL,pFace);
						}
					}
				}
			}
			catch(...)
			{
				int d = 1;
			}

			//! clear points and edges.
			{
				for(vector<CIsPoint2d*>::iterator jtr = PointEntry.begin();jtr != PointEntry.end();++jtr)
				{
					delete (*jtr);
				}

				for(vector<CDelaunayVertex*>::iterator jtr = oVertexEntry.begin();jtr != oVertexEntry.end();++jtr)
				{
					delete (*jtr);
				}
			}
			{
				for(vector<CDelaunayTriEdge*>::iterator jtr = EdgeEntry.begin();jtr != EdgeEntry.end();++jtr)
				{
					delete (*jtr);
				}
			}
		}
	}

	return NULL;
}