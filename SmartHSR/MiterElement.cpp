// MiterElement.cpp: implementation of the CMiterElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "MiterElement.h"
#include "SmartHSR.h"

#define	MITER_SEGMENT	20

IMPLEMENTS_HSR_ELEMENT(CMiterElement , CHSRElement , _T("<MITER>"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMiterElement::CMiterElement()
{
	m_eType = MITER;
}

CMiterElement::~CMiterElement()
{

}

/**	
	@brief	MITER ELEMENT를 파싱한다.
	
	@author	HumKyung.BAEK

	@param	pHSRScanner	a parameter of type CHSRScanner*

	@return	bool	
*/
bool CMiterElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet = false;

	if(pHSRScanner && ((31 == pHSRScanner->m_nFactor) || (38 == pHSRScanner->m_nFactor) || (45 == pHSRScanner->m_nFactor)))
	{
		CYLINDER_T cylinder;
		
		for(int i = 10;i < pHSRScanner->m_nFactor;)
		{
			cylinder.radius = ATOF_T((*pHSRScanner->m_aryFactor)[i].c_str());
			cylinder.pt[0].x= ATOF_T((*pHSRScanner->m_aryFactor)[i+1].c_str());
			cylinder.pt[0].y= ATOF_T((*pHSRScanner->m_aryFactor)[i+2].c_str());
			cylinder.pt[0].z= ATOF_T((*pHSRScanner->m_aryFactor)[i+3].c_str());
			cylinder.pt[1].x= ATOF_T((*pHSRScanner->m_aryFactor)[i+4].c_str());
			cylinder.pt[1].y= ATOF_T((*pHSRScanner->m_aryFactor)[i+5].c_str());
			cylinder.pt[1].z= ATOF_T((*pHSRScanner->m_aryFactor)[i+6].c_str());
			i += 7;
			
			m_CylinderEntry.push_back(cylinder);
		}

		AlignCylinder();
		Volume();
	}
	
	return bRet;
}

/**	
	@brief	뷰의 볼륨에 속하는지 검사를 한다.

	@author	HumKyung.BAEK

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CMiterElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;
}

/**	
	@brief	볼륨을 계산한다. 각 실린더의 끝점으로 볼륨을 계산한다.
	
	@author	HumKyung.BAEK

	@remarks

	@return	void	
*/
void CMiterElement::Volume()
{
	if(!m_CylinderEntry.empty())
	{
		m_vol.x[0] = m_vol.x[1] = m_CylinderEntry.front().pt[0].x;
		m_vol.y[0] = m_vol.y[1] = m_CylinderEntry.front().pt[0].y;
		m_vol.z[0] = m_vol.z[1] = m_CylinderEntry.front().pt[0].z;
		for(vector<CYLINDER_T>::iterator itr = m_CylinderEntry.begin();itr != m_CylinderEntry.end();++itr)
		{
			if(m_vol.x[0] > itr->pt[0].x) m_vol.x[0] = itr->pt[0].x;
			if(m_vol.x[1] < itr->pt[0].x) m_vol.x[1] = itr->pt[0].x;
			if(m_vol.y[0] > itr->pt[0].y) m_vol.y[0] = itr->pt[0].y;
			if(m_vol.y[1] < itr->pt[0].y) m_vol.y[1] = itr->pt[0].y;
			if(m_vol.z[0] > itr->pt[0].z) m_vol.z[0] = itr->pt[0].z;
			if(m_vol.z[1] < itr->pt[0].z) m_vol.z[1] = itr->pt[0].z;

			if(m_vol.x[0] > itr->pt[1].x) m_vol.x[0] = itr->pt[1].x;
			if(m_vol.x[1] < itr->pt[1].x) m_vol.x[1] = itr->pt[1].x;
			if(m_vol.y[0] > itr->pt[1].y) m_vol.y[0] = itr->pt[1].y;
			if(m_vol.y[1] < itr->pt[1].y) m_vol.y[1] = itr->pt[1].y;
			if(m_vol.z[0] > itr->pt[1].z) m_vol.z[0] = itr->pt[1].z;
			if(m_vol.z[1] < itr->pt[1].z) m_vol.z[1] = itr->pt[1].z;
			
		}
	}
}

/**	
	@brief	MITER를 이루는 면들을 계산한다.
	
	@author	HumKyung.BAEK

	@param	pViewFmt	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@return	PFACE	
*/
CHSRFace* CMiterElement::CreateFace(CHSRViewFormat* pViewFormat,long& nID)
{
	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	if(pHSRApp)
	{
		vector<CYLINDER_T> CylinderEntry;
		CylinderEntry.insert(CylinderEntry.begin() , m_CylinderEntry.begin() , m_CylinderEntry.end());
		
		///< 3D --> 2D
		for(vector<CYLINDER_T>::iterator itr = CylinderEntry.begin();itr != CylinderEntry.end();++itr)
		{
			itr->radius = pViewFormat->MODEL2VIEW(itr->radius);
			itr->pt[0]  = pViewFormat->MODEL2VIEW(itr->pt[0]);
			itr->pt[1]  = pViewFormat->MODEL2VIEW(itr->pt[1]);
		}
		
		POINT_T ptEDGE[33];
		//! 하나의 실린더 형상으로 이루어져 있을때...
		if(1 == CylinderEntry.size())
		{
			VECTOR_T norm;
			norm.dx = CylinderEntry[0].pt[1].x - CylinderEntry[0].pt[0].x;
			norm.dy = CylinderEntry[0].pt[1].y - CylinderEntry[0].pt[0].y;
			norm.dz = CylinderEntry[0].pt[1].z - CylinderEntry[0].pt[0].z;
			CMath::NormalizeVector(norm);
			
			POINT_T ptEDGE[33];
			int res = CHSRFace::CreateCirclePts(ptEDGE , CylinderEntry[0].pt[0] , CylinderEntry[0].radius , norm , 32);
			if(ERROR_SUCCESS == res)
			{
				const double l = CPoint3d::Distance(CylinderEntry[0].pt[0] , CylinderEntry[0].pt[1]);
				for(int i = 0;i < 33 - 1;++i)
				{
					POINT_T pts[4];
					pts[0] = ptEDGE[i];
					pts[1].x = pts[0].x + norm.dx * l;
					pts[1].y = pts[0].y + norm.dy * l;
					pts[1].z = pts[0].z + norm.dz * l;
					pts[3] = ptEDGE[i + 1];
					pts[2].x = pts[3].x + norm.dx * l;
					pts[2].y = pts[3].y + norm.dy * l;
					pts[2].z = pts[3].z + norm.dz * l;
					CHSRFace* pFace = CHSRFace::CreateFace(4 , pts);
					if(pFace)
					{
						pFace->SetColor(*m_pstrColor);
						pHSRApp->AppendFace(NULL , pFace);
					}
				}
			}

			return NULL;
		}

		VECTOR_T vecNorm = {0.f,} , vec[2] = {0.f,};
		vec[0].dx = CylinderEntry[0].pt[1].x - CylinderEntry[0].pt[0].x;
		vec[0].dy = CylinderEntry[0].pt[1].y - CylinderEntry[0].pt[0].y;
		vec[0].dz = CylinderEntry[0].pt[1].z - CylinderEntry[0].pt[0].z;
		vec[1].dx = CylinderEntry[1].pt[1].x - CylinderEntry[1].pt[0].x;
		vec[1].dy = CylinderEntry[1].pt[1].y - CylinderEntry[1].pt[0].y;
		vec[1].dz = CylinderEntry[1].pt[1].z - CylinderEntry[1].pt[0].z;
		CMath::GetCrossProduct(vecNorm , vec[0] , vec[1]);
		CMath::NormalizeVector(vecNorm);
		
		const VECTOR_T ZAxis = {0,0,1.f};
		const double nDot = CMath::DotProduct(vecNorm.dx , vecNorm.dy , vecNorm.dz , ZAxis.dx , ZAxis.dy , ZAxis.dz);
		/// HORIZONTAL.
		if(fabs(1.f - fabs(nDot)) < CHSRElement::m_tol)
		{
			if(CreateHorizontalFace(CylinderEntry , nID)) ++nID;
		}
		else
		{
			VECTOR_T norm;
			norm = vec[0];
			CMath::NormalizeVector(norm);

			int res = CHSRFace::CreateCirclePts(ptEDGE , CylinderEntry[0].pt[0] , CylinderEntry[0].radius , norm , 32);
			if(ERROR_SUCCESS == res)
			{
				//! 시작면을 이루는 점들을 저장한다.
				vector<POINT_T> pts , new_pts;
				for(int i = 0;i < 33;++i) pts.push_back(ptEDGE[i]);

				VECTOR_T norm;
				for(vector<CYLINDER_T>::iterator itr = CylinderEntry.begin();(itr+1) != CylinderEntry.end();++itr)
				{
					CPlane3d plane3d;
					plane3d.ComputePlaneEquation(itr->pt[0] , itr->pt[1], (itr+1)->pt[1]);
					norm = plane3d.GetNormVector();

					CPlane3d joinplane3d;
					CreateJoinPlane(joinplane3d , itr->pt[0] , itr->pt[1] , (itr+1)->pt[1]);
					
					CreateCylinderFace(new_pts , *itr , pts , joinplane3d , nID);

					pts.clear();
					pts.insert(pts.begin() , new_pts.begin() , new_pts.end());
				}

				VECTOR_T vec[2];
				vec[0].dx = CylinderEntry.back().pt[0].x - CylinderEntry.back().pt[1].x;
				vec[0].dy = CylinderEntry.back().pt[0].y - CylinderEntry.back().pt[1].y;
				vec[0].dz = CylinderEntry.back().pt[0].z - CylinderEntry.back().pt[1].z;
				CMath::GetCrossProduct(vec[1] , norm , vec[0]);
				POINT_T pt1 , pt2;
				pt1.x = CylinderEntry.back().pt[1].x + norm.dx * 1.f;
				pt1.y = CylinderEntry.back().pt[1].y + norm.dy * 1.f;
				pt1.z = CylinderEntry.back().pt[1].z + norm.dz * 1.f;
				pt2.x = CylinderEntry.back().pt[1].x + vec[1].dx * 1.f;
				pt2.y = CylinderEntry.back().pt[1].y + vec[1].dy * 1.f;
				pt2.z = CylinderEntry.back().pt[1].z + vec[1].dz * 1.f;
				CPlane3d joinplane3d;
				joinplane3d.ComputePlaneEquation(CylinderEntry.back().pt[1] , pt1 , pt2);
				CreateCylinderFace(new_pts , CylinderEntry.back() , pts , joinplane3d , nID);
			}
		}
		++nID;
	}

	return NULL;
}

/**
 * ClipAxisByElevation:
 *
 * @param ptStart 
 * @param ptEnd 
 * @param clipStart 
 * @param clipEnd 
 * @param clipStartZ 
 * @param clipEndZ 
 * @param zhigh 
 * @param zlow 
 * @return void 
 */
void CMiterElement::ClipAxisByElevation(POINT_T &ptStart, POINT_T& ptEnd, bool& clipStart , bool& clipEnd , double& clipStartZ , double& clipEndZ , const double& zhigh , const double& zlow)
{
	VECTOR_T vec=
	{
		ptEnd.x - ptStart.x,
		ptEnd.y - ptStart.y,
		ptEnd.z - ptStart.z
	};
	if(!CMath::NormalizeVector(vec)) return;
	
	//--> reposition the pipe point for clipping
	if(ptStart.z > zhigh)
	{
		const double t=(zhigh - ptStart.z) / vec.dz;
		ptStart.x = ptStart.x + vec.dx*t;
		ptStart.y = ptStart.y + vec.dy*t;
		ptStart.z = ptStart.z + vec.dz*t;
		
		clipStart  = true;
		clipStartZ = zhigh;
	}
	else if(ptStart.z < zlow)
	{
		const double t=(zlow - ptStart.z) / vec.dz;
		ptStart.x = ptStart.x + vec.dx*t;
		ptStart.y = ptStart.y + vec.dy*t;
		ptStart.z = ptStart.z + vec.dz*t;
		
		clipStart  = true;
		clipStartZ = zlow;
	}
	if(ptEnd.z > zhigh)
	{
		const double t=(zhigh - ptStart.z) / vec.dz;
		ptEnd.x = ptStart.x + vec.dx*t;
		ptEnd.y = ptStart.y + vec.dy*t;
		ptEnd.z = ptStart.z + vec.dz*t;
		
		clipEnd  = true;
		clipEndZ = zhigh;
	}
	else if(ptEnd.z < zlow)
	{
		const double t=(zlow - ptStart.z) / vec.dz;
		ptEnd.x = ptStart.x + vec.dx*t;
		ptEnd.y = ptStart.y + vec.dy*t;
		ptEnd.z = ptStart.z + vec.dz*t;
		
		clipEnd  = true;
		clipEndZ = zlow;
	}
}

/**
	@brief	평평한 면일때에 외곽선을 구한다.

	@author	HumKyung.BAEK

	@date	2009.07.23
*/
bool CMiterElement::CreateHorizontalFace(vector<CYLINDER_T>& CylinderEntry , const long& nID)
{
	const VECTOR_T ZAxis = {0,0,1.f};

	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	if(pHSRApp)
	{
		/// PLANE
		vector<VECTOR_T> NormVecEntry;
		for(vector<CYLINDER_T>::iterator itr = CylinderEntry.begin();itr != CylinderEntry.end();++itr)
		{
			VECTOR_T vec;
			vec.dx = itr->pt[1].x - itr->pt[0].x;
			vec.dy = itr->pt[1].y - itr->pt[0].y;
			vec.dz = itr->pt[1].z - itr->pt[0].z;
			
			VECTOR_T NormVec = CMath::CrossProduct(ZAxis , vec);
			if(false == CMath::NormalizeVector(NormVec))
			{
				NormVec.dx = NormVec.dy = NormVec.dz = 0.f;
			}
			NormVecEntry.push_back(NormVec);
		}
		
		/// 인접한 두 직선의 교점을 구한다.
		/// 교점들로 POLYGON을 구한다.
		int index = 0;
		vector<POINT_T> IntersecPtEntry;
		for(vector<CYLINDER_T>::iterator itr = CylinderEntry.begin();(itr + 1) != CylinderEntry.end();++itr,++index)
		{
			if((0.f == NormVecEntry[index].dx) && (0.f == NormVecEntry[index].dy) && (0.f == NormVecEntry[index].dz)) continue;
			
			double x = 0.f , y = 0.f , z = 0.f;
			POINT_T pts[4] = {0.f,};
			pts[0].x = itr->pt[0].x + NormVecEntry[index].dx * itr->radius;
			pts[0].y = itr->pt[0].y + NormVecEntry[index].dy * itr->radius;
			pts[0].z = itr->pt[0].z + NormVecEntry[index].dz * itr->radius;
			pts[1].x = itr->pt[1].x + NormVecEntry[index].dx * itr->radius;
			pts[1].y = itr->pt[1].y + NormVecEntry[index].dy * itr->radius;
			pts[1].z = itr->pt[1].z + NormVecEntry[index].dz * itr->radius;
			x = pts[1].x - pts[0].x;  
			y = pts[1].y - pts[0].y;
			z = pts[1].z - pts[0].z;
			pts[0].x -= x;
			pts[0].y -= y;
			pts[0].z -= z;
			pts[1].x += x;
			pts[1].y += y;
			pts[1].z += z;
			
			pts[2].x = (itr+1)->pt[0].x + NormVecEntry[index + 1].dx * (itr+1)->radius;
			pts[2].y = (itr+1)->pt[0].y + NormVecEntry[index + 1].dy * (itr+1)->radius;
			pts[2].z = (itr+1)->pt[0].z + NormVecEntry[index + 1].dz * (itr+1)->radius;
			pts[3].x = (itr+1)->pt[1].x + NormVecEntry[index + 1].dx * (itr+1)->radius;
			pts[3].y = (itr+1)->pt[1].y + NormVecEntry[index + 1].dy * (itr+1)->radius;
			pts[3].z = (itr+1)->pt[1].z + NormVecEntry[index + 1].dz * (itr+1)->radius;
			x = pts[3].x - pts[2].x;
			y = pts[3].y - pts[2].y;
			z = pts[3].z - pts[2].z;
			pts[2].x -= x;
			pts[2].y -= y;
			pts[2].z -= z;
			pts[3].x += x;
			pts[3].y += y;
			pts[3].z += z;
			
			if(0 == index)
			{
				POINT_T pt;
				pt.x = itr->pt[0].x + NormVecEntry[index].dx * itr->radius;
				pt.y = itr->pt[0].y + NormVecEntry[index].dy * itr->radius;
				pt.z = itr->pt[0].z + NormVecEntry[index].dz * itr->radius;
				IntersecPtEntry.push_back(pt);
			}
			if(DO_INTERSECTION == CGeometry::IntersectLine2D(&x,&y,
				pts[0].x,pts[0].y,pts[1].x,pts[1].y,
				pts[2].x,pts[2].y,pts[3].x,pts[3].y,0.))
			{
				POINT_T pt;
				pt.x = x; pt.y = y; pt.z = 0;
				double d = (0.f != pts[1].x - pts[0].x) ? (x - pts[0].x) / (pts[1].x - pts[0].x) : (y - pts[0].y) / (pts[1].y - pts[0].y);
				pt.z = pts[0].z + (pts[1].z - pts[0].z) * d;
				
				IntersecPtEntry.push_back(pt);
			}
		}
		
		if((0.f != NormVecEntry[index].dx) || (0.f != NormVecEntry[index].dy))
		{
			POINT_T pt;
			pt.x = CylinderEntry.back().pt[1].x + NormVecEntry[index].dx * CylinderEntry.back().radius;
			pt.y = CylinderEntry.back().pt[1].y + NormVecEntry[index].dy * CylinderEntry.back().radius;
			pt.z = CylinderEntry.back().pt[1].z + NormVecEntry[index].dz * CylinderEntry.back().radius;
			IntersecPtEntry.push_back(pt);
		}
		
		index = NormVecEntry.size() - 1;
		for(vector<CYLINDER_T>::reverse_iterator jtr = CylinderEntry.rbegin();(jtr + 1) != CylinderEntry.rend();++jtr,--index)
		{
			if((0.f == NormVecEntry[index].dx) && (0.f == NormVecEntry[index].dy) && (0.f == NormVecEntry[index].dz)) continue;
			
			double x = 0.f , y = 0.f , z = 0.f;
			POINT_T pts[4] = {0.f,};
			pts[0].x = jtr->pt[1].x - NormVecEntry[index].dx * jtr->radius;
			pts[0].y = jtr->pt[1].y - NormVecEntry[index].dy * jtr->radius;
			pts[0].z = jtr->pt[1].z - NormVecEntry[index].dz * jtr->radius;
			pts[1].x = jtr->pt[0].x - NormVecEntry[index].dx * jtr->radius;
			pts[1].y = jtr->pt[0].y - NormVecEntry[index].dy * jtr->radius;
			pts[1].z = jtr->pt[0].z - NormVecEntry[index].dz * jtr->radius;
			x = pts[1].x - pts[0].x;
			y = pts[1].y - pts[0].y;
			z = pts[1].z - pts[0].z;
			pts[0].x -= x;
			pts[0].y -= y;
			pts[0].z -= z;
			pts[1].x += x;
			pts[1].y += y;
			pts[1].z += z;
			
			pts[2].x = (jtr+1)->pt[1].x - NormVecEntry[index - 1].dx * (jtr+1)->radius;
			pts[2].y = (jtr+1)->pt[1].y - NormVecEntry[index - 1].dy * (jtr+1)->radius;
			pts[2].z = (jtr+1)->pt[1].z - NormVecEntry[index - 1].dz * (jtr+1)->radius;
			pts[3].x = (jtr+1)->pt[0].x - NormVecEntry[index - 1].dx * (jtr+1)->radius;
			pts[3].y = (jtr+1)->pt[0].y - NormVecEntry[index - 1].dy * (jtr+1)->radius;
			pts[3].z = (jtr+1)->pt[0].z - NormVecEntry[index - 1].dz * (jtr+1)->radius;
			x = pts[3].x - pts[2].x;
			y = pts[3].y - pts[2].y;
			z = pts[3].z - pts[2].z;
			pts[2].x -= x;
			pts[2].y -= y;
			pts[2].z -= z;
			pts[3].x += x;
			pts[3].y += y;
			pts[3].z += z;
			
			if(jtr == CylinderEntry.rbegin())
			{
				POINT_T pt;
				pt.x = jtr->pt[1].x - NormVecEntry[index].dx * jtr->radius;
				pt.y = jtr->pt[1].y - NormVecEntry[index].dy * jtr->radius;
				pt.z = jtr->pt[1].z - NormVecEntry[index].dz * jtr->radius;
				IntersecPtEntry.push_back(pt);
			}
			if(DO_INTERSECTION == CGeometry::IntersectLine2D(&x,&y,
				pts[0].x,pts[0].y,pts[1].x,pts[1].y,
				pts[2].x,pts[2].y,pts[3].x,pts[3].y,0.))
			{
				POINT_T pt;
				pt.x = x; pt.y = y; pt.z = 0;
				double d = (0.f != pts[1].x - pts[0].x) ? (x - pts[0].x) / (pts[1].x - pts[0].x) : (y - pts[0].y) / (pts[1].y - pts[0].y);
				pt.z = pts[0].z + (pts[1].z - pts[0].z) * d;
				
				IntersecPtEntry.push_back(pt);
			}
		}
		
		if((0.f != NormVecEntry[0].dx) || (0.f != NormVecEntry[0].dy))
		{
			POINT_T pt;
			pt.x = CylinderEntry.front().pt[0].x - NormVecEntry[0].dx * CylinderEntry.front().radius;
			pt.y = CylinderEntry.front().pt[0].y - NormVecEntry[0].dy * CylinderEntry.front().radius;
			pt.z = CylinderEntry.front().pt[0].z - NormVecEntry[0].dz * CylinderEntry.front().radius;
			IntersecPtEntry.push_back(pt);
		}
		
		/// 위에서 구한 OUTLINE으로 면을 생성한다.
		{
			POINT_T ptEDGE[36];
			
			index = 0;
			for(vector<POINT_T>::iterator itr = IntersecPtEntry.begin();itr != IntersecPtEntry.end();++itr)
			{
				ptEDGE[index++] = *itr;
			}
			
			CHSRFace* pSide = NULL;
			if(pSide=CHSRFace::CreateFace((index), ptEDGE))
			{
				pSide->m_enmLayer = LAYER_ELBOW_DOU;				
				pSide->type  = m_eType;
				pSide->id    = nID;
				pSide->ele   = HSR_RECTANGLE_SHAPE;
				pSide->radius= CylinderEntry.front().radius;
				pSide->SetColor(*m_pstrColor);
				
				int nSymbolVertices = 0;
				POINT_T ptSYMBOL[36]={0.,};
				for(int i = 1;i < (int)(index / 2);++i)
				{
					ptSYMBOL[nSymbolVertices++] = ptEDGE[i];
					ptSYMBOL[nSymbolVertices++] = ptEDGE[index - 1 - i];
				}
				pSide->pSymboline  = CHSRFace::CreateLine(nSymbolVertices , ptSYMBOL);
				if(pSide->pSymboline)
				{
					for(PHSR_VERTEX ptr = pSide->pSymboline;ptr && ptr->next;ptr = ptr->next)
					{
						ptr->next->render = INVALID;
						ptr = ptr->next;
						if(NULL == ptr) break;
					}
				}
				
				index = 0;
				for(vector<CYLINDER_T>::iterator jtr = CylinderEntry.begin();jtr != CylinderEntry.end();++jtr)
				{
					if(0 == index) ptEDGE[index++] = jtr->pt[0];
					ptEDGE[index++] = jtr->pt[1];
				}
				
				pSide->pCenterline = CHSRFace::CreateLine(index , ptEDGE);
				
				pHSRApp->AppendFace(NULL , pSide);
			}
		}

		return true;
	}

	return false;
}

/**
	@brief	VERTICAL일 경우 FACE를 생성한다.

	@author	HumKyung.BAEK

	@return	bool - FACE 생성 여부
*/
bool CMiterElement::CreateVertFace(vector<CYLINDER_T>& CylinderEntry , const long& nID)
{
	CHSRApp* pHSRApp = CHSRApp::GetInstance();
	if(pHSRApp)
	{
		vector<POINT_T> IntersecPtEntry;
		for(vector<CYLINDER_T>::iterator itr = CylinderEntry.begin();itr != CylinderEntry.end();++itr)
		{
			IntersecPtEntry.push_back(itr->pt[0]);
			IntersecPtEntry.push_back(itr->pt[1]);
		}
		
		/// 2D로 매핑했을 때 가장 멀리 떨어지 두개의 PT를 구한다.
		POINT_T anchor[2];
		double maxlen = 0.f;
		for(vector<POINT_T>::iterator jtr = IntersecPtEntry.begin();jtr != IntersecPtEntry.end();++jtr)
		{
			for(vector<POINT_T>::iterator ktr = jtr + 1;ktr != IntersecPtEntry.end();++ktr)
			{
				double dx = ktr->x - jtr->x;
				double dy = ktr->y - jtr->y;
				double dz = ktr->z - jtr->z;
				double len = dx * dx + dy * dy + dz * dz;
				if(len > maxlen)
				{
					maxlen = len;
					anchor[0] = *jtr;
					anchor[1] = *ktr;
				}
			}
		}
		
		VECTOR_T vecNorm = {0.f,};
		const VECTOR_T ZAxis = {0,0,1.f};
		vecNorm.dx = anchor[1].x - anchor[0].x;
		vecNorm.dy = anchor[1].y - anchor[0].y;
		vecNorm.dz = 0.f;
		vecNorm    = CMath::CrossProduct(ZAxis , vecNorm);
		if(false == CMath::NormalizeVector(vecNorm))
		{
			vecNorm.dx = vecNorm.dy = vecNorm.dz = 0.f;
		}
		
		{
			static POINT_T ptEDGE[36];
			
			ptEDGE[0].x = anchor[0].x + vecNorm.dx * CylinderEntry.front().radius;
			ptEDGE[0].y = anchor[0].y + vecNorm.dy * CylinderEntry.front().radius;
			ptEDGE[0].z = anchor[0].z;
			ptEDGE[1].x = anchor[1].x + vecNorm.dx * CylinderEntry.front().radius;
			ptEDGE[1].y = anchor[1].y + vecNorm.dy * CylinderEntry.front().radius;
			ptEDGE[1].z = anchor[1].z;
			ptEDGE[2].x = anchor[1].x - vecNorm.dx * CylinderEntry.front().radius;
			ptEDGE[2].y = anchor[1].y - vecNorm.dy * CylinderEntry.front().radius;
			ptEDGE[2].z = anchor[1].z;
			ptEDGE[3].x = anchor[0].x - vecNorm.dx * CylinderEntry.front().radius;
			ptEDGE[3].y = anchor[0].y - vecNorm.dy * CylinderEntry.front().radius;
			ptEDGE[3].z = anchor[0].z;
			
			CHSRFace* pSide = NULL;
			if(pSide=CHSRFace::CreateFace(4, ptEDGE))
			{
				pSide->m_enmLayer = LAYER_ELBOW_DOU;
				
				pSide->type  = m_eType;
				pSide->id    = nID;
				pSide->ele   = HSR_RECTANGLE_SHAPE;
				pSide->radius= CylinderEntry.front().radius;
				pSide->SetColor(*m_pstrColor);
				
				pSide->pCenterline = CHSRFace::CreateLine(2 , anchor);
				
				pHSRApp->AppendFace(NULL , pSide);
				
				return true;
			}
		}
	}

	return false;
}

/**
	@brief	MITER를 구성하는 CYLINDER를 일렬로 재구성한다.

	@author	HumKyung.BAEK

	@return	void
*/
void CMiterElement::AlignCylinder()
{
	vector<CYLINDER_T> TempCylinderEntry;

	//! 한쪽만 다른 CYLINDER와 연결되는 CYLINDER를 찾는다.
	for(vector<CYLINDER_T>::iterator itr = m_CylinderEntry.begin();itr != m_CylinderEntry.end();++itr)
	{
		int conn[2] = {0 , 0} , desconn[2] = {0 , 0};
		for(vector<CYLINDER_T>::iterator jtr = m_CylinderEntry.begin();jtr != m_CylinderEntry.end();++jtr)
		{
			if(itr == jtr) continue;
			CheckConnection(conn , *itr , *jtr , desconn) ;
		}

		if((1 == conn[0]) && (0 == conn[1]))
		{
			CYLINDER_T cylinder(*itr);
			swap(cylinder.pt[0] , cylinder.pt[1]);
			TempCylinderEntry.push_back(cylinder);
			m_CylinderEntry.erase(itr);
			break;
		}
		else if((0 == conn[0]) && (1 == conn[1]))
		{
			CYLINDER_T cylinder(*itr);
			TempCylinderEntry.push_back(cylinder);
			m_CylinderEntry.erase(itr);
			break;
		}
	}

	if(!TempCylinderEntry.empty())
	{
		while(!m_CylinderEntry.empty())
		{
			CYLINDER_T cylinder(TempCylinderEntry.back());
			size_t start = m_CylinderEntry.size();
			for(vector<CYLINDER_T>::iterator itr = m_CylinderEntry.begin();itr != m_CylinderEntry.end();++itr)
			{
				int conn[2] = {0 , 0} , desconn[2] = {0 , 0};
				if(CheckConnection(conn , cylinder , *itr , desconn))
				{
					if((0 == conn[0]) && (1 == conn[1]))
					{
						CYLINDER_T rhs(*itr);
						if((0 == desconn[0]) && (1 == desconn[1])) swap(rhs.pt[0] , rhs.pt[1]);

						TempCylinderEntry.push_back(rhs);
						m_CylinderEntry.erase(itr);
						break;
					}
				}
			}
			size_t end = m_CylinderEntry.size();

			//! 크기의 변동이 없으면 무한 루프를 막기위해서 루프를 빠져 나온다.
			if(start == end) break;
		}

		assert(m_CylinderEntry.empty());
		m_CylinderEntry.clear();
		m_CylinderEntry.insert(m_CylinderEntry.begin() , TempCylinderEntry.begin() , TempCylinderEntry.end());
	}
}

/**
	@brief	두 CYLINDER의 연결 방식을 검사한다.

	@author	HumKyung.BAEK

	@param	conn = conn[0]이 1이면 lhs의 START POINT가 연결 지점 , conn[1]이 1이면 lhs의 END POINT가 연결 지점
	@param	lhs
	@param	rhs
	@param	desconn = destconn[0]이 1이면 rhs의 START POINT가 연결 지점 , destconn[1]이 1이면 rhs의 END POINT가 연결 지점

	@return	bool = 서로 연결되면 true , otherwise return false
*/
bool CMiterElement::CheckConnection(int conn[], const CYLINDER_T &lhs, const CYLINDER_T &rhs , int desconn[])
{
	double dx = lhs.pt[0].x - rhs.pt[0].x;
	double dy = lhs.pt[0].y - rhs.pt[0].y;
	double dz = lhs.pt[0].z - rhs.pt[0].z;
	double len= sqrt(dx * dx + dy * dy + dz * dz);
	if(len < HSR_TOLER)
	{
		++conn[0];
		++desconn[0];
	}
	
	dx = lhs.pt[0].x - rhs.pt[1].x;
	dy = lhs.pt[0].y - rhs.pt[1].y;
	dz = lhs.pt[0].z - rhs.pt[1].z;
	len= sqrt(dx * dx + dy * dy + dz * dz);
	if(len < HSR_TOLER)
	{
		++conn[0];
		++desconn[1];
	}
	
	dx = lhs.pt[1].x - rhs.pt[0].x;
	dy = lhs.pt[1].y - rhs.pt[0].y;
	dz = lhs.pt[1].z - rhs.pt[0].z;
	len= sqrt(dx * dx + dy * dy + dz * dz);
	if(len < HSR_TOLER)
	{
		++conn[1];
		++desconn[0];
	}
	
	dx = lhs.pt[1].x - rhs.pt[1].x;
	dy = lhs.pt[1].y - rhs.pt[1].y;
	dz = lhs.pt[1].z - rhs.pt[1].z;
	len= sqrt(dx * dx + dy * dy + dz * dz);
	if(len < HSR_TOLER)
	{
		++conn[1];
		++desconn[1];
	}
	
	return ((conn[0] > 0) || (conn[1] > 0));
}

/**
	@brief	접합 평면을 구한다.

	@author	HumKyung.BAEK

	@date	2009.07.20
*/
int CMiterElement::CreateJoinPlane(CPlane3d &plane, POINT_T &pt1, POINT_T &pt2, POINT_T &pt3)
{
	plane.ComputePlaneEquation(pt1 , pt2, pt3);
	const VECTOR_T norm = plane.GetNormVector();

	VECTOR_T vec1,vec2;
	vec1.dx = pt1.x - pt2.x;
	vec1.dy = pt1.y - pt2.y;
	vec1.dz = pt1.z - pt2.z;
	vec2.dx = pt3.x - pt2.x;
	vec2.dy = pt3.y - pt2.y;
	vec2.dz = pt3.z - pt2.z;
	const double deg = CMath::GetAngle(vec1 , vec2);
	
	//! 이등변 삼각형을 만든다. 이등변 삼각형을 통하여 접선 평면을 이루는 한 점을 구한다.
	const double l = CPoint3d::Distance(pt1 , pt2);
	CMath::NormalizeVector(vec2);	//! 단위 벡터로 만든다.
	POINT_T new_pt3;
	new_pt3.x = pt2.x + vec2.dx * l;
	new_pt3.y = pt2.y + vec2.dy * l;
	new_pt3.z = pt2.z + vec2.dz * l;

	VECTOR_T vec3;
	vec3.dx = new_pt3.x - pt1.x;
	vec3.dy = new_pt3.y - pt1.y;
	vec3.dz = new_pt3.z - pt1.z;
	CMath::NormalizeVector(vec3);
	
	const double ml = 0.5 * CPoint3d::Distance(pt1 , new_pt3);
	POINT_T mid;
	mid.x = pt1.x + vec3.dx * ml;
	mid.y = pt1.y + vec3.dy * ml;
	mid.z = pt1.z + vec3.dz * ml;
	
	POINT_T pts[3];
	pts[0].x = pt2.x + norm.dx * 1.f;
	pts[0].y = pt2.y + norm.dy * 1.f;
	pts[0].z = pt2.z + norm.dz * 1.f;
	pts[1]   = pt2;
	pts[2]   = mid;

	plane.ComputePlaneEquation(pts[0] , pts[1] , pts[2]);

	return ERROR_SUCCESS;
}

/**
	@brief	평면과 두 점을 지나는 직선과의 교점을 구한다.

	@author	HumKyung.BAEK

	@date	2009.07.21
*/
int CMiterElement::GetIntersectionOf(POINT_T &res, CPlane3d &plane, const POINT_T &pt1, const POINT_T &pt2)
{
	VECTOR_T vec;
	vec.dx = pt2.x - pt1.x;
	vec.dy = pt2.y - pt1.y;
	vec.dz = pt2.z - pt1.z;
	CMath::NormalizeVector(vec);

	const VECTOR_T norm = plane.GetNormVector();
	const double t = -(norm.dx*pt1.x + norm.dy*pt1.y + norm.dz*pt1.z + plane.m_nD) / (norm.dx*vec.dx + norm.dy*vec.dy + norm.dz*vec.dz);

	res = pt1;
	res.x += vec.dx * t;
	res.y += vec.dy * t;
	res.z += vec.dz * t;

	return ERROR_SUCCESS;
}

/**
	@brief	평면의 법선 벡터와 시점과의 사이각이 0에서 90도 사이인지 검사한다.

	@author	HumKyung.BAEK

	@date	2009.07.23
*/
bool CMiterElement::IsPositivePlaneOnView(CPlane3d &plane3d)
{
	VECTOR_T sight;
	sight.dx = 0.f;
	sight.dy = 0.f;
	sight.dz = 1.f;

	VECTOR_T norm = plane3d.GetNormVector();
	const double deg = CMath::GetAngle(sight , norm);
	return (fabs(deg) >= 90.f);
}

/**
	@brief	pt1과 pt2로 이루는 직선이 가장 자리 직선인지를 검사한다.

	@author	HumKyung.BAEK

	@date	2009.07.23
*/
bool CMiterElement::IsEdgeLine(vector<POINT_T>& pts , POINT_T* pt1 , POINT_T* pt2)
{
	assert(pt1 && "pt1 is NULL");
	assert(pt2 && "pt2 is NULL");

	if(pt1 && pt2)
	{
		bool bLeftSide = false, bRightSide = false;

		VECTOR_T vec[2];
		vec[0].dx = pt2->x - pt1->x;
		vec[0].dy = pt2->y - pt1->y;
		vec[0].dz = 0.f;
		CMath::NormalizeVector(vec[0]);
		for(vector<POINT_T>::iterator itr = pts.begin();itr != pts.end();++itr)
		{
			vec[1].dx = itr->x - pt1->x;
			vec[1].dy = itr->y - pt1->y;
			vec[1].dz = 0.f;
			CMath::NormalizeVector(vec[1]);

			const double sign = vec[0].dx*vec[1].dy - vec[0].dy*vec[1].dx;
			if(sign < -CHSRElement::m_tol)
				bLeftSide = true;
			else if(sign > CHSRElement::m_tol)
				bRightSide = true;

			if(bLeftSide && bRightSide) return false;
		}

		return true;
	}

	return false;
}

bool IsSamePoint(const POINT_T& pt1,const POINT_T& pt2)
{
	double dx=0.,dy=0.,dz=0.;

	dx = pt2.x - pt1.x;
	dy = pt2.y - pt1.y;
	dz = pt2.z - pt1.z;
	if((fabs(dx) < 0.001) && (fabs(dy) < 0.001) && (fabs(dz) < 0.001))
		return true;

	return false;
}

/**
	@brief	공유하는 EDGE의 render를 false로 설정한다.

	@author	HumKyung.BAEK

	@date	2009.07.24
*/
int CMiterElement::RemoveCoEdge(Facet *pFacet1, Facet *pFacet2)
{
	assert(pFacet1 && "pFacet1 is NULL");
	assert(pFacet2 && "pFacet2 is NULL");

	if(pFacet1 && pFacet2)
	{
		/*
		for(PVERTEX ptr = pFacet1->pFace->pHead;ptr && ptr->next;ptr = ptr->next)
		{
			for(PVERTEX jtr = pFacet2->pFace->pHead;jtr && jtr->next;jtr = jtr->next)
			{
				const bool a = IsSamePoint(*(ptr->pt),*(jtr->pt)) && IsSamePoint(*(ptr->next->pt) , *(jtr->next->pt));
				const bool b = IsSamePoint(*(ptr->next->pt),*(jtr->pt)) && IsSamePoint(*(ptr->pt) , *(jtr->next->pt));
				if(a || b)
				{
					ptr->render = INVALID;
					jtr->render = INVALID;
				} 
			}
		}
		*/

		for(int i = 0;i < 4;i+=2)
		{
			POINT_T* node1 = pFacet1->pts[i % 4];
			POINT_T* node2 = pFacet1->pts[(i+1) % 4];
			for(int j = 0;j < 4;j+=2)
			{
				POINT_T* node3 = pFacet2->pts[j % 4];
				POINT_T* node4 = pFacet2->pts[(j+1) % 4];

				const bool a = (node1 == node3) && (node2 == node4);
				const bool b = (node2 == node3) && (node1 == node4);
				if(a || b)
				{
					pFacet1->render[i] = false;
					pFacet2->render[j] = false;
				}
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	Miter에서 하나의 cylinder부분의 형상을 생성한다.

	@author	HumKyung.BAEK

	@date	2009.07.24
*/
int CMiterElement::CreateCylinderFace(vector<POINT_T>& new_pts , CYLINDER_T cylinder , vector<POINT_T>& pts , CPlane3d& joinplane3d , const int& nID)
{
	CHSRApp* pHSRApp = CHSRApp::GetInstance();

	if(pHSRApp)
	{
		new_pts.clear();
		for(vector<POINT_T>::iterator jtr = pts.begin();jtr != pts.end();++jtr)
		{
			POINT_T res;
			POINT_T pt1 = (*jtr) , pt2;
			pt2.x = pt1.x + (cylinder.pt[1].x - cylinder.pt[0].x);
			pt2.y = pt1.y + (cylinder.pt[1].y - cylinder.pt[0].y);
			pt2.z = pt1.z + (cylinder.pt[1].z - cylinder.pt[0].z);
			GetIntersectionOf(res , joinplane3d , pt1 , pt2);
			new_pts.push_back(res);
		}
		
		list<Facet> FacetList;
		{
			list<Facet> TempFacetList;
			bool reverse = false;
			int nIndex = 0;
			for(vector<POINT_T>::iterator jtr = pts.begin();(jtr+1) != pts.end();++jtr,++nIndex)
			{
				POINT_T* _pts[4];
				_pts[0] = &(pts[nIndex]);
				_pts[1] = &(new_pts[nIndex]);
				_pts[2] = &(new_pts[nIndex+1]);
				_pts[3] = &(pts[nIndex+1]);
				
				CPlane3d plane3d(*(_pts[0]) , *(_pts[1]) , *(_pts[2]));
				const SIGN_T sign = plane3d.WhichSideOfPlane(cylinder.pt[0]);
				if(NEGATIVE != sign) std::reverse(_pts , _pts + 4);
				
				if(IsPositivePlaneOnView(plane3d))
				{
					Facet facet;
					facet.pts[0] = _pts[0];facet.pts[1] = _pts[1];facet.pts[2] = _pts[2];facet.pts[3] = _pts[3];
					facet.render[0] = true;facet.render[1] = true;facet.render[2] = true;facet.render[3] = true;
					(reverse) ? FacetList.push_front(facet) : FacetList.push_back(facet);
				}else	reverse = !reverse;
			}
			///FacetList.insert(FacetList.begin() , TempFacetList.begin() , TempFacetList.end());
		}
		{	
			vector<POINT_T> allocated_pts;
			for(list<Facet>::iterator jtr = FacetList.begin();jtr != FacetList.end();)
			{
				POINT_T _pts[4];
				_pts[0] = *(jtr->pts[0]);
				_pts[1] = *(jtr->pts[1]);
				_pts[2] = *(jtr->pts[2]);
				_pts[3] = *(jtr->pts[3]);
				
				CHSRFace* pFace = CHSRFace::CreateFace(4 , _pts , false);
				if(pFace)
				{
					allocated_pts.push_back(_pts[0]);
					allocated_pts.push_back(_pts[1]);
					allocated_pts.push_back(_pts[2]);
					allocated_pts.push_back(_pts[3]);
					jtr->pFace = pFace;
										
					pFace->m_enmLayer = LAYER_ELBOW_DOU;				
					pFace->type  = m_eType;
					pFace->id    = nID;
					pFace->ele   = HSR_RECTANGLE_SHAPE;
					pFace->radius= cylinder.radius;
					pFace->SetColor(*m_pstrColor);
					
					pHSRApp->AppendFace(NULL , pFace);
					++jtr;
				}else	jtr = FacetList.erase(jtr);
			}
			
			{
				const int nSize = FacetList.size();
				int i = 0;
				for(list<Facet>::iterator jtr = FacetList.begin();jtr != FacetList.end();++jtr,++i)
				{
					if(false == IsEdgeLine(allocated_pts , jtr->pFace->pHead->pt , jtr->pFace->pHead->next->pt))
					{
						jtr->pFace->pHead->render = INVALID;
					}
					
					if(false == IsEdgeLine(allocated_pts , jtr->pFace->pHead->next->next->pt , jtr->pFace->pHead->next->next->next->pt))
					{
						jtr->pFace->pHead->next->next->render = INVALID;
					}
				}
			}
		}
		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}