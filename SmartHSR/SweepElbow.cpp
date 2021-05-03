// SweepElbow.cpp: implementation of the CSweepElbow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <IsPlane3d.h>
#include "SweepElbow.h"
#include "SmartHSR.h"
#include "HSRViewFormat.h"

IMPLEMENTS_HSR_ELEMENT(CSweepElbow , CHSRElement , _T("SWEEP_ELBOW="));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSweepElbow::CSweepElbow()
{
	m_lhs = new vector<POINT_T>;
	m_rhs = new vector<POINT_T>;
}

CSweepElbow::~CSweepElbow()
{
	try
	{
		if(NULL != m_lhs) delete m_lhs;
		if(NULL != m_rhs) delete m_rhs;
	}
	catch(...)
	{
	}
}

/**
	@brief	
	@author	¹éÈì°æ

	@param	

	@return
*/
bool CSweepElbow::ParseLine(CHSRScanner *pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		TCHAR szBuf[MAX_PATH + 1] = {'\0' ,};

		pHSRScanner->GetLine(szBuf , MAX_PATH);
		if(STRING_T(_T("{")) != szBuf) throw exception("expected {");

		POINT_T pt;
		while(pHSRScanner->GetLine(szBuf , MAX_PATH))
		{
			if(STRING_T(_T("}")) == szBuf) break;

			if(0 == STRNCMP_T(szBuf , _T("LHS_ORIGIN") , 10))
			{
				SSCANF_T(szBuf , _T("LHS_ORIGIN=%lf,%lf,%lf,%lf") , &(m_lhs_origin.x) , &(m_lhs_origin.y) , &(m_lhs_origin.z) , &(m_lhs_radius));
			}
			else if(0 == STRNCMP_T(szBuf , _T("LHS") , 3))
			{
				SSCANF_T(szBuf , _T("LHS=%lf,%lf,%lf") , &(pt.x) , &(pt.y) , &(pt.z));
				m_lhs->push_back(pt);
			}
			else if(0 == STRNCMP_T(szBuf , _T("RHS_ORIGIN") , 10))
			{
				SSCANF_T(szBuf , _T("RHS_ORIGIN=%lf,%lf,%lf,%lf") , &(m_rhs_origin.x) , &(m_rhs_origin.y) , &(m_rhs_origin.z) , &(m_rhs_radius));
			}
			else if(0 == STRNCMP_T(szBuf , _T("RHS") , 3))
			{
				SSCANF_T(szBuf , _T("RHS=%lf,%lf,%lf") , &(pt.x) , &(pt.y) , &(pt.z));
				m_rhs->push_back(pt);
			}
		}

		Volume();
	}

	return bRet;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-05
    @class      CSweepElbow
    @function   IsInVolume
    @return     bool
    @param      const   CIsVolume&
    @param      volume
    @brief
******************************************************************************/
bool CSweepElbow::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;	
}

void CSweepElbow::Volume()
{
	m_vol.x[0] = m_vol.x[1] = (*m_lhs)[0].x;
	m_vol.y[0] = m_vol.y[1] = (*m_lhs)[0].y;
	m_vol.z[0] = m_vol.z[1] = (*m_lhs)[0].z;
	
	for(vector<POINT_T>::iterator itr = m_lhs->begin(); itr != m_lhs->end();itr++)
	{
		if(m_vol.x[0] > itr->x) m_vol.x[0] = itr->x;
		if(m_vol.x[1] < itr->x) m_vol.x[1] = itr->x;
		if(m_vol.y[0] > itr->y) m_vol.y[0] = itr->y;
		if(m_vol.y[1] < itr->y) m_vol.y[1] = itr->y;
		if(m_vol.z[0] > itr->z) m_vol.z[0] = itr->z;
		if(m_vol.z[1] < itr->z) m_vol.z[1] = itr->z;
	}

	for(vector<POINT_T>::iterator itr = m_rhs->begin(); itr != m_rhs->end();itr++)
	{
		if(m_vol.x[0] > itr->x) m_vol.x[0] = itr->x;
		if(m_vol.x[1] < itr->x) m_vol.x[1] = itr->x;
		if(m_vol.y[0] > itr->y) m_vol.y[0] = itr->y;
		if(m_vol.y[1] < itr->y) m_vol.y[1] = itr->y;
		if(m_vol.z[0] > itr->z) m_vol.z[0] = itr->z;
		if(m_vol.z[1] < itr->z) m_vol.z[1] = itr->z;
	}
}

//static CIsPlane3d _axisPlane;
//
//static bool __SortByElev(CHSRFace* lhs , CHSRFace* rhs)
//{
//	CIsPoint3d pt[2];
//	CIsPoint3d ptMid[2];
//
//	int i = 0;
//	double lhs_elev = 0;
//	PHSR_VERTEX ptr = NULL;
//	for(ptr = lhs->pHead;ptr && ptr->next;ptr = ptr->next , i++)
//	{
//		/*
//		if(0 == i)
//		{
//			pt[0].Set(ptr->pt->x , ptr->pt->y , ptr->pt->z);
//		}
//		else if(2 == i)
//		{
//			pt[1].Set(ptr->pt->x , ptr->pt->y , ptr->pt->z);
//		}
//		*/
//		lhs_elev += _axisPlane(CIsPoint3d(ptr->pt->x , ptr->pt->y , ptr->pt->z));
//	}
//	ptMid[0].Set((pt[0].x() + pt[1].x()) * 0.5 , (pt[0].y() + pt[1].y()) * 0.5 , (pt[0].z() + pt[1].z()) * 0.5);
//	
//	double rhs_elev = 0;
//	for(i = 0,ptr = rhs->pHead;ptr && ptr->next;ptr = ptr->next , i++)
//	{
//		/*
//		if(0 == i)
//		{
//			pt[0].Set(ptr->pt->x , ptr->pt->y , ptr->pt->z);
//		}
//		else if(2 == i)
//		{
//			pt[1].Set(ptr->pt->x , ptr->pt->y , ptr->pt->z);
//		}
//		*/
//		rhs_elev += _axisPlane(CIsPoint3d(ptr->pt->x , ptr->pt->y , ptr->pt->z));
//	}
//	ptMid[1].Set((pt[0].x() + pt[1].x()) * 0.5 , (pt[0].y() + pt[1].y()) * 0.5 , (pt[0].z() + pt[1].z()) * 0.5);
//
//	//lhs_elev = _axisPlane(ptMid[0]);
//	//rhs_elev = _axisPlane(ptMid[1]);
//
//	return (lhs_elev > rhs_elev);
//}

CHSRFace* CSweepElbow::CreateFace(CHSRViewFormat *pViewFmt, long &nID)
{
	assert(pViewFmt && "pViewFmt is NULL");
	CHSRFace* pRet=NULL;

	if(pViewFmt)
	{
		CHSRApp* pHSRApp=CHSRApp::GetInstance();
		
		size_t lhs_count = 0;
		POINT_T pt[2][32];
		for(vector<POINT_T>::iterator itr = m_lhs->begin();itr != m_lhs->end();itr++)
		{
			pt[0][lhs_count++] = pViewFmt->MODEL2VIEW(*itr);
		}

		size_t rhs_count = 0;
		for(vector<POINT_T>::iterator itr = m_rhs->begin();itr != m_rhs->end();itr++)
		{
			pt[1][rhs_count++] = pViewFmt->MODEL2VIEW(*itr);
		}
		
		POINT_T origin[2];
		origin[0] = pViewFmt->MODEL2VIEW(m_lhs_origin);
		origin[1] = pViewFmt->MODEL2VIEW(m_rhs_origin);

		CIsVect3d zAxis(0.,0.,1.);
		CIsVect3d vec(origin[1].x - origin[0].x , origin[1].y - origin[0].y , origin[1].z - origin[0].z);
		vec.Normalize();
		
		double dot = vec.DotProduct(zAxis);
		
		CIsVect3d norm;
		if(fabs(fabs(dot) - 1) < 0.001)
		{
			if(pRet = CHSRFace::CreateFace(lhs_count , pt[0]))
			{
#ifdef VER_03
				pRet->m_enmLayer = LAYER_ELBOW;
#endif
				pRet->id  = nID++;
				pRet->type = ELBOW;
				pRet->ele = NORMAL;
				pRet->SetColor(*m_pstrColor);
				pHSRApp->AppendFace(NULL , pRet);
			}

			if(pRet = CHSRFace::CreateFace(rhs_count , pt[1]))
			{
#ifdef VER_03
				pRet->m_enmLayer = LAYER_ELBOW;
#endif
				pRet->id  = nID++;
				pRet->type = ELBOW;
				pRet->ele = NORMAL;
				pRet->SetColor(*m_pstrColor);
				pHSRApp->AppendFace(NULL , pRet);
			}

			return NULL;
		}
		else
		{
			const double dot = zAxis.DotProduct(vec);
			const double costheta = dot / vec.Length();
			norm = zAxis - costheta * vec;

			const double d = -(norm.dx() * origin[0].x + norm.dy() * origin[0].y + norm.dz() * origin[0].z);

			CIsPlane3d axisPlane(norm , d);
		
			vector<Edge> EdgeEntry;
			vector<CHSRFace*> FaceEntry;
			for(size_t i = 0;i < lhs_count;i++)
			{
				FacePts facePts;
				facePts.pts[0] = pt[0][i % lhs_count];
				facePts.pts[1] = pt[0][(i + 1) % lhs_count];
				facePts.pts[2] = pt[1][(i + 1) % lhs_count];
				facePts.pts[3] = pt[1][i % lhs_count];
				facePts.pts[4] = facePts.pts[0];
				
				CIsPlane3d::SIGN_T sign1 = axisPlane.WhichSideOf(CIsPoint3d(facePts.pts[0].x , facePts.pts[0].y , facePts.pts[0].z));
				CIsPlane3d::SIGN_T sign2 = axisPlane.WhichSideOf(CIsPoint3d(facePts.pts[1].x , facePts.pts[1].y , facePts.pts[1].z));
				CIsPlane3d::SIGN_T sign3 = axisPlane.WhichSideOf(CIsPoint3d(facePts.pts[2].x , facePts.pts[2].y , facePts.pts[2].z));
				CIsPlane3d::SIGN_T sign4 = axisPlane.WhichSideOf(CIsPoint3d(facePts.pts[3].x , facePts.pts[3].y , facePts.pts[3].z));

				if((CIsPlane3d::NEGATIVE != sign1) && (CIsPlane3d::NEGATIVE != sign2) &&
				   (CIsPlane3d::NEGATIVE != sign3) && (CIsPlane3d::NEGATIVE != sign4))
				{
					if(pRet = CHSRFace::CreateFace(5 , facePts.pts))
					{
#ifdef VER_03
						pRet->m_enmLayer = LAYER_ELBOW;
#endif
						pRet->id  = nID++;
						pRet->type = ELBOW;
						pRet->ele = NORMAL;
						pRet->SetColor(*m_pstrColor);
						pHSRApp->AppendFace(NULL , pRet);

						FaceEntry.push_back(pRet);
						
						Edge edge;
						edge.pts[0] = facePts.pts[1];
						edge.pts[1] = facePts.pts[2];
						EdgeEntry.push_back(edge);
						edge.pts[0] = facePts.pts[3];
						edge.pts[1] = facePts.pts[4];
						EdgeEntry.push_back(edge);
					}
				}
			}

			if(!FaceEntry.empty())
			{
				const size_t siz = FaceEntry.size();
				for(vector<Edge>::iterator itr = EdgeEntry.begin();itr != EdgeEntry.end();itr++)
				{
					size_t has = 0;
					PHSR_VERTEX vertices[5] = {NULL ,};
					
					for(size_t i = 0;i < siz ; i++)
					{
						CHSRFace* pFace = FaceEntry[i];
						if(pFace)
						{
							PHSR_VERTEX vtr = NULL;
							if(NULL != (vtr = pFace->HasEdge(itr->pts[0] , itr->pts[1])))
							{
								vertices[has++] = vtr;
							}
						}
					}

					if(2 == has)
					{
						vertices[0]->render = INVALID;
						vertices[1]->render = INVALID;
					}
				}

				return NULL;
			}

			return NULL;
		}
	}

	return NULL;
}
