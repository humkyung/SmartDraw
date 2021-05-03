// HSRRevolvedElement.cpp: implementation of the CHSRRevolvedElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IsVolume.h"
#include "SmartHSR.h"
#include "HSRScanner.h"
#include "HSRRevolvedElement.h"

IMPLEMENTS_HSR_ELEMENT(CHSRRevolvedElement , CHSRElement , _T("<REVOLVED>"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHSRRevolvedElement::CHSRRevolvedElement() : m_rotateAngle(0)
{

}

CHSRRevolvedElement::~CHSRRevolvedElement()
{

}

bool CHSRRevolvedElement::ParseLine(CHSRScanner *pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");

	if(pHSRScanner)
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;

		size_t nCount = ATOI_T((*pHSRScanner->m_aryFactor)[1].c_str());

		POINT_T ptSet[2];
		ptSet[0].x = ATOF_T((*pHSRScanner->m_aryFactor)[2 + nCount * 3 ].c_str());
		ptSet[0].y = ATOF_T((*pHSRScanner->m_aryFactor)[2 + nCount * 3 + 1].c_str());
		ptSet[0].z = ATOF_T((*pHSRScanner->m_aryFactor)[2 + nCount * 3 + 2].c_str());
		ptSet[1].x = ATOF_T((*pHSRScanner->m_aryFactor)[2 + nCount * 3 + 3].c_str());
		ptSet[1].y = ATOF_T((*pHSRScanner->m_aryFactor)[2 + nCount * 3 + 4].c_str());
		ptSet[1].z = ATOF_T((*pHSRScanner->m_aryFactor)[2 + nCount * 3 + 5].c_str());
		m_axis.dx = ptSet[1].x - ptSet[0].x;
		m_axis.dy = ptSet[1].y - ptSet[0].y;
		m_axis.dz = ptSet[1].z - ptSet[0].z;

		for(size_t i = 0;i < nCount;i++)
		{
			POINT_T pt; 
			pt.x = ATOF_T((*pHSRScanner->m_aryFactor)[2 + i * 3].c_str());
			pt.y = ATOF_T((*pHSRScanner->m_aryFactor)[2 + i * 3 + 1].c_str());
			pt.z = ATOF_T((*pHSRScanner->m_aryFactor)[2 + i * 3 + 2].c_str());

			m_PointSet.push_back(pt);
		}

		m_rotateAngle = ATOF_T((*pHSRScanner->m_aryFactor)[2 + nCount * 3 + 6].c_str());

		Revolve(m_axis , m_PointSet , m_rotateAngle , 8 , ptSet[0]);

		CIsVolume volume;
		for(int j = 0;j < 8 + 1;j++)
		{
			for(int i = 0;i < int(nCount);i++)
			{
				volume.Add(CIsPoint3d(m_pts[j*nCount + i].x , m_pts[j*nCount + i].y , m_pts[j*nCount + i].z));
			}
		}
		m_vol.x[0] = volume.minx();
		m_vol.x[1] = volume.maxx();
		m_vol.y[0] = volume.miny();
		m_vol.y[1] = volume.maxy();
		m_vol.z[0] = volume.minz();
		m_vol.z[1] = volume.maxz();

		return true;
	}

	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-05
    @class      CHSRRevolvedElement
    @function   IsInVolume
    @return     bool
    @param      const   CIsVolume&
    @param      volume
    @brief
******************************************************************************/
bool CHSRRevolvedElement::IsInVolume(const CIsVolume& volume)
{
	if(m_vol.x[0] > volume.maxx()) return false;
	if(m_vol.x[1] < volume.minx()) return false;
	if(m_vol.y[0] > volume.maxy()) return false;
	if(m_vol.y[1] < volume.miny()) return false;
	if(m_vol.z[0] > volume.maxz()) return false;
	if(m_vol.z[1] < volume.minz()) return false;

	return true;
}

CHSRFace* CHSRRevolvedElement::CreateFace(CHSRViewFormat* pVIEWFormat,long& nID)
{
	CHSRApp* pHSRApp=CHSRApp::GetInstance();

	POINT_T ptEdge[5];

	size_t nSize = m_PointSet.size();
	for(int j = 0;j < 8;j++)
	{
		for(int i = 0;i < int(nSize - 1);i++)
		{
			ptEdge[0] = m_pts[j * nSize + i];
			ptEdge[1] = m_pts[j * nSize + i + 1];
			ptEdge[2] = m_pts[(j + 1) * nSize + i + 1];
			ptEdge[3] = m_pts[(j + 1) * nSize + i];
			ptEdge[4] = ptEdge[0];

			ptEdge[0]  = pVIEWFormat->MODEL2VIEW(ptEdge[0]);
			ptEdge[1]  = pVIEWFormat->MODEL2VIEW(ptEdge[1]);
			ptEdge[2]  = pVIEWFormat->MODEL2VIEW(ptEdge[2]);
			ptEdge[3]  = pVIEWFormat->MODEL2VIEW(ptEdge[3]);
			ptEdge[4]  = pVIEWFormat->MODEL2VIEW(ptEdge[4]);

			CHSRFace* pRet = NULL;
			if(pRet = CHSRFace::CreateFace(5 , ptEdge))
			{
				if(0 == j)
				{
					pRet->pHead->next->next->render = INVALID;
				}
				else if(7 == j)
				{
					pRet->pHead->render = INVALID;
				}
				else
				{
					pRet->pHead->render = INVALID;
					pRet->pHead->next->next->render = INVALID;
				}

				try
				{
					pRet->m_enmLayer = GetHSRLayer();
				}
				catch(...)
				{
					pRet->m_enmLayer = LAYER_OTHER;
				}
				pRet->id  = nID++;
				pRet->ele = NORMAL;
				pRet->SetColor(*m_pstrColor);
				pHSRApp->AppendFace(NULL , pRet);
			}
		}
	}

	return NULL;
}

void CHSRRevolvedElement::Revolve(const VECTOR_T &vecAxis, vector<POINT_T>& pts , const double &nRotate, const int &nParts , const POINT_T& shift)
{
	assert((nParts > 0) && "nParts is less than 0.");
	
	if(nParts > 0)
	{
		double alpha=0.,beta=0.;
		CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
		
		size_t nSize = pts.size();
		// set vecAxis to z<0,0,1> by rotating about x,y axis.
		for(int i = 0;i < int(nSize);i++)
		{
			POINT_T pt = pts[i];
			pt.x -= shift.x;
			pt.y -= shift.y;
			pt.z -= shift.z;

			m_pts[i] = CGeometry::RotateAboutXYAxis(pt , alpha , beta);
		}
		
		// rotate about z-axis<0,0,1>
		double nSweep= nRotate;
		double nStep = nSweep/nParts;
		double nAngle= nStep;
		double c=cos(nStep),s=sin(nStep);
		int j = 0;
		for(j = 1;j < nParts;j++)
		{
			for(int i = 0;i < int(nSize);i++)
			{
				int idxSrc=(j-1)*nSize + i;
				int idxDst=j*nSize + i;

				m_pts[idxDst].x = m_pts[idxSrc].x*c - m_pts[idxSrc].y*s;
				m_pts[idxDst].y = m_pts[idxSrc].x*s + m_pts[idxSrc].y*c;
				m_pts[idxDst].z = m_pts[idxSrc].z;
			}
			nAngle += nStep;
		}
		c=cos(nSweep),s=sin(nSweep);
		for(int i = 0;i < int(nSize);i++)
		{
			m_pts[j*nSize + i].x = m_pts[i].x*c - m_pts[i].y*s;
			m_pts[j*nSize + i].y = m_pts[i].x*s + m_pts[i].y*c;
			m_pts[j*nSize + i].z = m_pts[i].z;
		}
		
		// set rotate-axis to vecAxis
		for(int j = 0;j < nParts + 1;j++)
		{
			for(int i = 0;i < int(nSize);i++)
			{
				 POINT_T pt = CGeometry::RotateAboutYXAxis(m_pts[j*nSize + i] , -beta , -alpha);
				 pt.x += shift.x;
				 pt.y += shift.y;
				 pt.z += shift.z;

				 m_pts[j*nSize + i] = pt;
			}
		}
	}
}

/**
	@brief	
	@author	¹éÈì°æ

	@return
*/
/*
const char* CHSRRevolvedElement::TypeString()
{
	static const string __typeString("<REVOLVED>");

	return __typeString.c_str();
}
*/