#include "stdafx.h"
#include <math.h>
#include <assert.h>
#include <DgnV7Lib.h>
#include "aDraw_CableTrayParser.h"

#include <functional>
using namespace std;

double CaDraw_CableTrayParser::TOLER = 0.05;

CaDraw_CableTrayParser::CaDraw_CableTrayParser(const double& uor_per_subunit) : m_height(0)
{
	m_uor_per_subunit = uor_per_subunit;

	m_width[0] = m_width[1] = 0.0;

	m_eType = CaDraw_CableTray::NONE_T;
	m_eSubType = CaDraw_CableTray::NONE_ST;

	memset(m_pt , '\0' , sizeof(DPoint3d)*3);
	m_nAngle = 0.0;
}

CaDraw_CableTrayParser::~CaDraw_CableTrayParser(void)
{
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	?
*/
int CaDraw_CableTrayParser::Parse(CDgnCellHeader* pDgnCell)
{
	assert(pDgnCell && "pDgnCell is NULL");

	if(pDgnCell)
	{
		PDGNATTRDATA pUserData = pDgnCell->attribute();
		if(this->IsStraight(pUserData))
		{
			m_eType = CaDraw_CableTray::STRAIGHT_T;
		}
		else if(this->IsElbowType(pUserData))
		{
			m_eType = CaDraw_CableTray::ELBOW_T;
		}
		else if(this->IsTeeType(pUserData))
		{
			m_eType = CaDraw_CableTray::TEE_T;
		}
		else if(this->IsReduceType(pUserData))
		{
			m_eType = CaDraw_CableTray::REDUCE_T;
		}
		GetDimension(pDgnCell);

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	?
*/
bool CaDraw_CableTrayParser::IsStraight(PDGNATTRDATA  pUserData)
{
	assert(pUserData && "pUserData is NULL");

	if(pUserData)
	{
		PDGNATTRDATA ptr = pUserData->next;
		
		if(ptr && ptr->hdr.u)
		{
			short* pData=(short*)(ptr->data.pData);

			if((1 == pData[2]) && (0x1000 == (pData[10] & 0xff00)))
				return true;
		}
	}
	
	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CCableTray
    @function   IsElbowType
    @return     bool
    @param      PXDGNATTRDATA   pUserData
    @brief
******************************************************************************/
bool CaDraw_CableTrayParser::IsElbowType(PDGNATTRDATA pUserData)
{
	assert(pUserData && "pUserData is NULL");

	if(pUserData)
	{
		PDGNATTRDATA ptr = pUserData->next;
		
		if(ptr)
		{
			short* pData=(short*)(ptr->data.pData);

			if(2 == pData[2])
			{
				if(3 == (pData[12] & 0x00ff))
				{
					m_eSubType = CaDraw_CableTray::VERTICAL_ST;
				}
				else if(4 == (pData[12] & 0x00ff))
				{
					m_eSubType = CaDraw_CableTray::VERTICAL_ST;
				}
				else if(5 == (pData[12] & 0x00ff))
				{
					m_eSubType = CaDraw_CableTray::HORIZONTAL_ST;
				}

				return true;
			}
		}
	}

	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_CableTrayParser
    @function   IsTeeType
    @return     bool
    @param      PDGNATTRDATA    pUserData
    @brief
******************************************************************************/
bool CaDraw_CableTrayParser::IsTeeType(PDGNATTRDATA pUserData)
{
	assert(pUserData && "pUserData is NULL");

	if(pUserData)
	{
		PDGNATTRDATA ptr = pUserData->next;
		
		if(ptr)
		{
			short* pData=(short*)(ptr->data.pData);

			if(3 == pData[2])
			{
				if(2 == (pData[12] & 0x00ff))
				{
					m_eSubType = CaDraw_CableTray::VERTICAL_ST;
				}
				else if(5 == (pData[12] & 0x00ff))
				{
					m_eSubType = CaDraw_CableTray::HORIZONTAL_ST;
				}

				return true;
			}
		}
	}

	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-16
    @class      CaDraw_CableTrayParser
    @function   IsReduceType
    @return     bool
    @param      PDGNATTRDATA    pUserData
    @brief
******************************************************************************/
bool CaDraw_CableTrayParser::IsReduceType(PDGNATTRDATA pUserData)
{
	assert(pUserData && "pUserData is NULL");

	if(pUserData)
	{
		PDGNATTRDATA ptr = pUserData->next;
		
		if(ptr)
		{
			short* pData=(short*)(ptr->data.pData);

			if(1 == pData[2])
			{
				if(8 == (pData[12] & 0x00ff))
				{
					m_eSubType = CaDraw_CableTray::STRAIGHT_ST;
				}
				else if(9 == (pData[12] & 0x00ff))
				{
					m_eSubType = CaDraw_CableTray::RIGHT_ST;
				}
				else if(10 == (pData[12] & 0x00ff))
				{
					m_eSubType = CaDraw_CableTray::LEFT_ST;
				}

				return true;
			}
		}
	}

	return false;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	?
*/
void CaDraw_CableTrayParser::GetDimension(CDgnCellHeader* pDgnCell)
{
	assert(pDgnCell && "pDgnCell is NULL");

	if(pDgnCell && (pDgnCell->GetEntityCount() > 1))
	{
		if(CaDraw_CableTray::STRAIGHT_T == m_eType)
		{
			CDgnElement* pDgnEnt = pDgnCell->GetEntityAt(0);
			if(pDgnEnt && pDgnEnt->IsKindOf(CDgnPointString::TypeString()))
			{
				CDgnPointString* pPointString = static_cast<CDgnPointString*>(pDgnEnt);

				m_pt[0] = pPointString->GetPointAt(0);
				m_pt[0].x /= m_uor_per_subunit; m_pt[0].y /= m_uor_per_subunit; m_pt[0].z /= m_uor_per_subunit;
				m_pt[1] = pPointString->GetPointAt(1);
				m_pt[1].x /= m_uor_per_subunit; m_pt[1].y /= m_uor_per_subunit; m_pt[1].z /= m_uor_per_subunit;

				m_lengthDir.Set(m_pt[1].x - m_pt[0].x , m_pt[1].y - m_pt[0].y , m_pt[1].z - m_pt[0].z);
				const double length = m_lengthDir.Length();
				m_lengthDir.Normalize();
			}

			pDgnEnt = pDgnCell->GetEntityAt(1);
			if(pDgnEnt && pDgnEnt->IsKindOf(CDgnSurfHeader3d::TypeString()))
			{
				CDgnSurfHeader3d* pSurface = static_cast<CDgnSurfHeader3d*>(pDgnEnt);
				
				list<CDgnElement*> oElmList;
				for(int j = 0;j < int(pSurface->GetSurfElemCount());++j)
				{
					CDgnSurfElement* pSurfElm = static_cast<CDgnSurfElement*>(pSurface->GetSurfElemAt(j));
					if(NULL == pSurfElm) break;

					oElmList.insert(oElmList.end() , pSurfElm->m_pBoundaryCurve->begin() , pSurfElm->m_pBoundaryCurve->end());
					oElmList.insert(oElmList.end() , pSurfElm->m_pRuleCurve->begin() , pSurfElm->m_pRuleCurve->end());
				}

				if(!oElmList.empty())
				{
					pDgnEnt = oElmList.front();
					if(pDgnEnt && pDgnEnt->IsKindOf(CDgnLineString::TypeString()))
					{
						CIsPoint3d pt[2][128];
						CDgnLineString* pLineString = static_cast<CDgnLineString*>(pDgnEnt);
						for(int i = 0;i < int(pLineString->GetVertexCount()); ++i)
						{
							DPoint3d _pt = pLineString->GetVertexAt(i);
							pt[0][i].Set(_pt.x /m_uor_per_subunit , _pt.y / m_uor_per_subunit , _pt.z / m_uor_per_subunit);
						}

						double dx = pt[0][1].x() - pt[0][2].x();
						double dy = pt[0][1].y() - pt[0][2].y();
						double dz = pt[0][1].z() - pt[0][2].z();
						m_width[0] = sqrt(dx*dx + dy*dy + dz*dz);
						m_widthDir.dx() = dx / m_width[0]; 
						m_widthDir.dy() = dy / m_width[0];
						m_widthDir.dz() = dz / m_width[0];

						dx = pt[0][0].x() - pt[0][1].x();
						dy = pt[0][0].y() - pt[0][1].y();
						dz = pt[0][0].z() - pt[0][1].z();
						m_height = sqrt(dx*dx + dy*dy + dz*dz);
					}
				}
			}
		}
		else if(CaDraw_CableTray::ELBOW_T == m_eType)
		{
			GetDimensionForElbow(pDgnCell);
		}
		else if(CaDraw_CableTray::TEE_T == m_eType)
		{
			GetDimensionForTee(pDgnCell);
		}
		else if(CaDraw_CableTray::REDUCE_T == m_eType)
		{
			GetDimensionForReduce(pDgnCell);
		}
	}
}

/**
	@brief	elbow의 dimension을 구한다.

	@author	HumKyung.BAEK

	@date	2010.09.06

*/
void CaDraw_CableTrayParser::GetDimensionForElbow(CDgnCellHeader* pDgnCell)
{
	assert(pDgnCell && "pDgnCell is NULL");

	if(pDgnCell)
	{
		CDgnElement* pElm = pDgnCell->GetEntityAt(0);
		if((NULL == pElm) || !(pElm->IsKindOf(CDgnPointString::TypeString()))) return;
		if(pElm && pElm->IsKindOf(CDgnPointString::TypeString()))
		{
			CDgnPointString* pPointString = static_cast<CDgnPointString*>(pElm);

			m_pt[0] = pPointString->GetPointAt(0);
			m_pt[0].x /= m_uor_per_subunit;
			m_pt[0].y /= m_uor_per_subunit;
			m_pt[0].z /= m_uor_per_subunit;
			m_pt[1] = pPointString->GetPointAt(1);
			m_pt[1].x /= m_uor_per_subunit;
			m_pt[1].y /= m_uor_per_subunit;
			m_pt[1].z /= m_uor_per_subunit;
		}

		for(int i = 1;i < int(pDgnCell->GetEntityCount());++i)
		{
			pElm = pDgnCell->GetEntityAt(i);
			if(NULL == pElm) break;

			if(pElm && pElm->IsKindOf(CDgnSurfHeader3d::TypeString()))
			{
				CDgnSurfHeader3d* pSurface = static_cast<CDgnSurfHeader3d*>(pElm);

				list<CDgnElement*> oElmList;
				bool bFoundBaseSurf = false;
				for(int j = 0;j < int(pSurface->GetSurfElemCount());++j)
				{
					CDgnSurfElement* pSurfElm = static_cast<CDgnSurfElement*>(pSurface->GetSurfElemAt(j));
					if(NULL == pSurfElm) break;

					oElmList.insert(oElmList.end() , pSurfElm->m_pBoundaryCurve->begin() , pSurfElm->m_pBoundaryCurve->end());
					oElmList.insert(oElmList.end() , pSurfElm->m_pRuleCurve->begin() , pSurfElm->m_pRuleCurve->end());
				}

				for(list<CDgnElement*>::iterator itr = oElmList.begin();itr != oElmList.end();++itr)
				{
					if((*itr)->IsKindOf(CDgnLine::TypeString())/* && pSurfElm->IsPrimaryCurve()*/)
					{
						CDgnLine* pLine = static_cast<CDgnLine*>(*itr);

						DPoint3d start = pLine->GetStartPoint();
						DPoint3d end = pLine->GetEndPoint();

						start.x /= m_uor_per_subunit; start.y /= m_uor_per_subunit; start.z /= m_uor_per_subunit;
						end.x /= m_uor_per_subunit; end.y /= m_uor_per_subunit; end.z /= m_uor_per_subunit;

						CIsVect3d vec[4];
						vec[0].Set(start.x - m_pt[0].x , start.y - m_pt[0].y , start.z - m_pt[0].z);
						vec[1].Set(end.x - m_pt[0].x , end.y - m_pt[0].y , end.z - m_pt[0].z);
						vec[2].Set(start.x - m_pt[1].x , start.y - m_pt[1].y , start.z - m_pt[1].z);
						vec[3].Set(end.x - m_pt[1].x , end.y - m_pt[1].y , end.z - m_pt[1].z);

						const double nDiff = fabs(vec[0].Length() - vec[1].Length());
						if(nDiff < CaDraw_CableTrayParser::TOLER)
						{
							m_widthDir.Set(end.x - start.x , end.y - start.y , end.z - start.z);
							m_width[0] = m_widthDir.Length();
							m_widthDir.Normalize();
							bFoundBaseSurf = true;
							break;
						}
						else if(fabs(vec[2].Length() - vec[3].Length()) < DBL_EPSILON)
						{
						}
					}
				}

				if(true == bFoundBaseSurf)
				{
					if(0 == STRICMP_T(_T("HORIZONTAL") , GetSubType().c_str()))
					{
						for(list<CDgnElement*>::iterator itr = oElmList.begin();itr != oElmList.end();++itr)
						{
							CDgnElement* pSurfElm = (*itr);///pSurface->GetSurfElemAt(j);
							if(NULL == pSurfElm) break;
							if(pSurfElm && pSurfElm->IsKindOf(CDgnArc::TypeString()) && pSurfElm->IsPrimaryCurve())
							{
								CDgnArc* pArc3d = static_cast<CDgnArc*>(pSurfElm);
								m_nAngle = pArc3d->sweepAngle();
								m_pt[2] = pArc3d->origin();
								m_pt[2].x /= m_uor_per_subunit;
								m_pt[2].y /= m_uor_per_subunit;
								m_pt[2].z /= m_uor_per_subunit;
							}
						}
					}
					else
					{
						int nCount = 0;
						DPoint3d pts[2];
						for(list<CDgnElement*>::iterator itr = oElmList.begin();(itr != oElmList.end()) && (nCount < 2);++itr)
						{
							CDgnElement* pSurfElm = (*itr);///pSurface->GetSurfElemAt(j);
							if(NULL == pSurfElm) break;
							if(pSurfElm && pSurfElm->IsKindOf(CDgnArc::TypeString()) && pSurfElm->IsPrimaryCurve())
							{
								CDgnArc* pArc3d = static_cast<CDgnArc*>(pSurfElm);
								m_nAngle = pArc3d->sweepAngle();
								
								pts[nCount] = pArc3d->origin();
								pts[nCount].x /= m_uor_per_subunit;
								pts[nCount].y /= m_uor_per_subunit;
								pts[nCount].z /= m_uor_per_subunit;
								++nCount;
							}
						}

						if(2 == nCount)
						{
							m_pt[2].x = (pts[0].x + pts[1].x) / 2.0;
							m_pt[2].y = (pts[0].y + pts[1].y) / 2.0;
							m_pt[2].z = (pts[0].z + pts[1].z) / 2.0;
						}
					}
				}
				else if(0. == m_height)
				{
					for(list<CDgnElement*>::iterator itr = oElmList.begin();itr != oElmList.end();++itr)
					{
						CDgnElement* pSurfElm = (*itr);///pSurface->GetSurfElemAt(j);
						if(NULL == pSurfElm) break;
						if(pSurfElm && pSurfElm->IsKindOf(CDgnLine::TypeString()) && !pSurfElm->IsPrimaryCurve())
						{
							CDgnLine* pLine = static_cast<CDgnLine*>(pSurfElm);

							DPoint3d start = pLine->GetStartPoint();
							DPoint3d end = pLine->GetEndPoint();
							m_heightDir.Set((end.x - start.x) / m_uor_per_subunit , (end.y - start.y) / m_uor_per_subunit , (end.z - start.z) / m_uor_per_subunit);
							m_height = m_heightDir.Length();
							m_heightDir.Normalize();
							break;
						}
					}
				}
			}
		}

		m_lengthDir.Set(m_pt[1].x - m_pt[0].x , m_pt[1].y - m_pt[0].y , m_pt[1].z - m_pt[0].z);
		m_lengthDir.Normalize();
	}
}

/**
	@brief

	@author	HumKyung.BAEK

	@date	?

*/
void CaDraw_CableTrayParser::GetDimensionForTee(CDgnCellHeader* pDgnCell)
{
	assert(pDgnCell && "pDgnCell is NULL");

	if(pDgnCell)
	{
		CDgnElement* pElm = pDgnCell->GetEntityAt(0);
		if((NULL == pElm) || !(pElm->IsKindOf(CDgnPointString::TypeString()))) return;
		{
			CDgnPointString* pPointString = static_cast<CDgnPointString*>(pElm);
			m_pt[0] = pPointString->GetPointAt(0);
			m_pt[0].x /= m_uor_per_subunit; m_pt[0].y /= m_uor_per_subunit; m_pt[0].z /= m_uor_per_subunit;
			m_pt[1] = pPointString->GetPointAt(1);
			m_pt[1].x /= m_uor_per_subunit; m_pt[1].y /= m_uor_per_subunit; m_pt[1].z /= m_uor_per_subunit;
			m_pt[2] = pPointString->GetPointAt(2);
			m_pt[2].x /= m_uor_per_subunit; m_pt[2].y /= m_uor_per_subunit; m_pt[2].z /= m_uor_per_subunit;
			m_lengthDir.Set(m_pt[1].x - m_pt[0].x , m_pt[1].y - m_pt[0].y , m_pt[1].z - m_pt[0].z);
			m_lengthDir.Normalize();
		}
		
		for(int i = 1;i < int(pDgnCell->GetEntityCount());++i)
		{
			pElm = pDgnCell->GetEntityAt(i);
			if(NULL == pElm) break;

			if(pElm && pElm->IsKindOf(CDgnComplexShapeHeader::TypeString()))
			{
				CDgnComplexShapeHeader *pComplexShapeHeader = static_cast<CDgnComplexShapeHeader*>(pElm);

				pComplexShapeHeader->GetEntityCount();
				CDgnElement* pSubElm = pComplexShapeHeader->GetEntityAt(0);
				
				CDgnLineString* pLineString = static_cast<CDgnLineString*>(pSubElm);
				vector<DPoint3d> oPointList = pLineString->GetPointStringExceptSamePoint();
				if(4 == oPointList.size())
				{
					DPoint3d pts[4];
					int nIndex = 0;
					for(vector<DPoint3d>::iterator itr = oPointList.begin();itr != oPointList.end();++itr,++nIndex)
					{
						pts[nIndex] = *itr;
					}
					m_widthDir.Set(
						(pts[1].x - pts[0].x) / m_uor_per_subunit , 
						(pts[1].y - pts[0].y) / m_uor_per_subunit ,
						(pts[1].z - pts[0].z) / m_uor_per_subunit);
					m_width[0] = m_widthDir.Length();
					m_widthDir.Normalize();
				}
				else if(8 == oPointList.size())
				{
					m_widthDir.Set(
						(oPointList[2].x - oPointList[1].x) / m_uor_per_subunit , 
						(oPointList[2].y - oPointList[1].y) / m_uor_per_subunit ,
						(oPointList[2].z - oPointList[1].z) / m_uor_per_subunit);
					m_width[0] = m_widthDir.Length();
					m_widthDir.Normalize();
				}

				pSubElm = pComplexShapeHeader->GetEntityAt(1);
				pSubElm = pComplexShapeHeader->GetEntityAt(2);
				pLineString = static_cast<CDgnLineString*>(pSubElm);
				oPointList.clear();
				oPointList = pLineString->GetPointStringExceptSamePoint();
				if(2 == oPointList.size())
				{
					DPoint3d pts[2];
					int nIndex = 0;
					for(vector<DPoint3d>::iterator itr = oPointList.begin();itr != oPointList.end();++itr,++nIndex)
					{
						pts[nIndex] = *itr;
					}
					double dx = (pts[1].x - pts[0].x) / m_uor_per_subunit;
					double dy = (pts[1].y - pts[0].y) / m_uor_per_subunit;
					double dz = (pts[1].z - pts[0].z) / m_uor_per_subunit;
					m_width[1] = sqrt(dx*dx + dy*dy + dz*dz);
				}
				else if(4 == oPointList.size())
				{
					double dx = (oPointList[2].x - oPointList[1].x) / m_uor_per_subunit;
					double dy = (oPointList[2].y - oPointList[1].y) / m_uor_per_subunit;
					double dz = (oPointList[2].z - oPointList[1].z) / m_uor_per_subunit;
					m_width[1] = sqrt(dx*dx + dy*dy + dz*dz);
				}

				pSubElm = pComplexShapeHeader->GetEntityAt(3);
			}
			else if(pElm && pElm->IsKindOf(CDgnSurfHeader3d::TypeString()))
			{
				CDgnSurfHeader3d* pSurface = static_cast<CDgnSurfHeader3d*>(pElm);
				
				vector<CDgnElement*> oElmList;
				for(int j = 0;j < int(pSurface->GetSurfElemCount());++j)
				{
					CDgnSurfElement* pSurfElm = static_cast<CDgnSurfElement*>(pSurface->GetSurfElemAt(j));
					oElmList.insert(oElmList.end() , pSurfElm->m_pBoundaryCurve->begin() , pSurfElm->m_pBoundaryCurve->end());
					oElmList.insert(oElmList.end() , pSurfElm->m_pRuleCurve->begin() , pSurfElm->m_pRuleCurve->end());
				}

				CDgnElement* pSurfElm = oElmList[0];
				if(pSurfElm && pSurfElm->IsKindOf(CDgnLine::TypeString()))
				{
					pSurfElm = oElmList[1];
					pSurfElm = oElmList[2];
					if(pSurfElm && pSurfElm->IsKindOf(CDgnLine::TypeString()))
					{
						CDgnLine* pLine = static_cast<CDgnLine*>(pSurfElm);

						DPoint3d start = pLine->GetStartPoint();
						DPoint3d end = pLine->GetEndPoint();
						m_heightDir.Set(
							(end.x - start.x) / m_uor_per_subunit , 
							(end.y - start.y) / m_uor_per_subunit , 
							(end.z - start.z) / m_uor_per_subunit);
						m_height = m_heightDir.Length();
						m_heightDir.Normalize();
					}

					break;
				}
			}
		}
	}
}

struct DataFindFunc : public std::binary_function<DPoint3d , DPoint3d , bool>
{
	bool operator()(const DPoint3d& lhs , const DPoint3d& rhs) const
	{
		return ((lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z));
	}
};

/**
	@brief	get dimension of reduce.

	@author	HumKyung.BAEK

	@date	2010.08.31

*/
void CaDraw_CableTrayParser::GetDimensionForReduce(CDgnCellHeader* pDgnCell)
{
	assert(pDgnCell && "pDgnCell is NULL");

	if(pDgnCell)
	{
		DPoint3d ptConn[2];

		CDgnElement* pElm = pDgnCell->GetEntityAt(0);
		if((NULL == pElm) || !(pElm->IsKindOf(CDgnPointString::TypeString()))) return;
		{
			CDgnPointString* pPointString = static_cast<CDgnPointString*>(pElm);
			ptConn[0] = m_pt[0] = pPointString->GetPointAt(0);
			m_pt[0].x /= m_uor_per_subunit; m_pt[0].y /= m_uor_per_subunit; m_pt[0].z /= m_uor_per_subunit;
			ptConn[1] = m_pt[1] = pPointString->GetPointAt(1);
			m_pt[1].x /= m_uor_per_subunit; m_pt[1].y /= m_uor_per_subunit; m_pt[1].z /= m_uor_per_subunit;

			const double dx = m_pt[1].x - m_pt[0].x;
			const double dy = m_pt[1].y - m_pt[0].y;
			const double dz = m_pt[1].z - m_pt[0].z;
			m_lengthDir.Set(dx , dy , dz);
			m_lengthDir.Normalize();
		}
		
		for(int i = 1;i < int(pDgnCell->GetEntityCount());++i)
		{
			pElm = pDgnCell->GetEntityAt(i);
			if(NULL == pElm) break;

			if(pElm && pElm->IsKindOf(CDgnComplexShapeHeader::TypeString()))
			{
				CDgnComplexShapeHeader *pComplexShapeHeader = static_cast<CDgnComplexShapeHeader*>(pElm);

				m_ptEdgeList.clear();
				const int nEntCount = pComplexShapeHeader->GetEntityCount();
				for(int j = 0;j < nEntCount;++j)
				{
					CDgnElement* pSubElm = pComplexShapeHeader->GetEntityAt(j);
					CDgnLine* pLine = static_cast<CDgnLine*>(pSubElm);
					
					DPoint3d ptStart = pLine->GetStartPoint();
					DPoint3d ptEnd   = pLine->GetEndPoint();
					ptStart.x /= m_uor_per_subunit;
					ptStart.y /= m_uor_per_subunit;
					ptStart.z /= m_uor_per_subunit;
					ptEnd.x /= m_uor_per_subunit;
					ptEnd.y /= m_uor_per_subunit;
					ptEnd.z /= m_uor_per_subunit;

					vector<DPoint3d>::iterator where = find_if(m_ptEdgeList.begin() , m_ptEdgeList.end() , std::bind2nd(DataFindFunc() , ptStart));
					if(where == m_ptEdgeList.end()) m_ptEdgeList.push_back(ptStart);
					where = find_if(m_ptEdgeList.begin() , m_ptEdgeList.end() , std::bind2nd(DataFindFunc() , ptEnd));
					if(where == m_ptEdgeList.end()) m_ptEdgeList.push_back(ptEnd);

					CIsVect3d vec[4];
					vec[0].Set(ptStart.x - m_pt[0].x , ptStart.y - m_pt[0].y , ptStart.z - m_pt[0].z);
					vec[1].Set(ptEnd.x - m_pt[0].x , ptEnd.y - m_pt[0].y , ptEnd.z - m_pt[0].z);
					vec[2].Set(ptStart.x - m_pt[1].x , ptStart.y - m_pt[1].y , ptStart.z - m_pt[1].z);
					vec[3].Set(ptEnd.x - m_pt[1].x , ptEnd.y - m_pt[1].y , ptEnd.z - m_pt[1].z);
					if(fabs(vec[0].Length() - vec[1].Length()) < DBL_EPSILON)
					{
						CIsVect3d dir;
						dir.Set(ptEnd.x - ptStart.x , ptEnd.y - ptStart.y , ptEnd.z - ptStart.z);
						m_width[0] = dir.Length();
						
						m_widthDir = dir;
						m_widthDir.Normalize();
					}
					else if(fabs(vec[2].Length() - vec[3].Length()) < DBL_EPSILON)
					{
						CIsVect3d dir;
						dir.Set(ptEnd.x - ptStart.x , ptEnd.y - ptStart.y , ptEnd.z - ptStart.z);
						m_width[1] = dir.Length();
					}
				}
			}
			else if(pElm && pElm->IsKindOf(CDgnSurfHeader3d::TypeString()))
			{
				CDgnSurfHeader3d* pSurface = static_cast<CDgnSurfHeader3d*>(pElm);
				
				vector<CDgnElement*> oElmList;
				for(int j = 0;j < int(pSurface->GetSurfElemCount());++j)
				{
					CDgnSurfElement* pSurfElm = static_cast<CDgnSurfElement*>(pSurface->GetSurfElemAt(j));
					oElmList.insert(oElmList.end() , pSurfElm->m_pBoundaryCurve->begin() , pSurfElm->m_pBoundaryCurve->end());
					oElmList.insert(oElmList.end() , pSurfElm->m_pRuleCurve->begin() , pSurfElm->m_pRuleCurve->end());
				}

				for(vector<CDgnElement*>::iterator itr = oElmList.begin();itr != oElmList.end();++itr)
				{
					CDgnElement* pSurfElm = (*itr);///pSurface->GetSurfElemAt(i);
					if(NULL == pSurfElm) break;
					if(pSurfElm && pSurfElm->IsKindOf(CDgnLine::TypeString()))
					{
						pSurfElm = oElmList[1];
						pSurfElm = oElmList[2];
						if(pSurfElm && pSurfElm->IsKindOf(CDgnLine::TypeString()))
						{
							CDgnLine* pLine = static_cast<CDgnLine*>(pSurfElm);

							DPoint3d start = pLine->GetStartPoint();
							DPoint3d end = pLine->GetEndPoint();
							m_heightDir.Set(
								(end.x - start.x) / m_uor_per_subunit , 
								(end.y - start.y) / m_uor_per_subunit ,
								(end.z - start.z) / m_uor_per_subunit);
							m_height = m_heightDir.Length();
							m_heightDir.Normalize();
						}

						break;
					}
				}
			}
		}
	}
}

/**
	@brief	return subtype string

	@author	HumKyung.BAEK

	@date	2010.09.01

*/
STRING_T CaDraw_CableTrayParser::GetSubType() const
{
	if(CaDraw_CableTray::NONE_ST == m_eSubType)
		return _T("NONE");
	else if(CaDraw_CableTray::STRAIGHT_ST == m_eSubType)
		return _T("STRAIGHT");
	else if(CaDraw_CableTray::LEFT_ST == m_eSubType)
		return _T("LEFT");
	else if(CaDraw_CableTray::RIGHT_ST == m_eSubType)
		return _T("RIGHT");
	else if(CaDraw_CableTray::HORIZONTAL_ST == m_eSubType)
		return _T("HORIZONTAL");
	else if(CaDraw_CableTray::VERTICAL_ST == m_eSubType)
		return _T("VERTICAL");

	return STRING_T();
}