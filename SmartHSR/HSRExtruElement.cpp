// HSRExtruElement.cpp: implementation of the CHSRExtruElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HSRExtruElement.h"
#include "Tokenizer.h"
#include "SmartHSR.h"

IMPLEMENTS_HSR_ELEMENT(CHSRExtruElement , CHSRElement , _T("EXTR="));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHSRExtruElement::CHSRExtruElement() : m_nHeight(0.f) , m_Type(CHSRExtruElement::SOLID)
{
	 m_axis.dx = 0.f;
	 m_axis.dy = 0.f;
	 m_axis.dz = 0.f;

	 m_pLoopElement = new CLoopElement;
}

CHSRExtruElement::~CHSRExtruElement()
{
	try
	{
		if(m_pLoopElement)
		{
			delete m_pLoopElement;
			m_pLoopElement = NULL;
		}
	}
	catch(...)
	{
	}
}

/**	
	@brief	EXTRU ELEMENT를 파싱한다.
	@author	HumKyung.Baek
	@param	pHSRScanner	a parameter of type CHSRScanner*
	@return	bool	
*/
bool CHSRExtruElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet = false;

	if(pHSRScanner)
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;

		pHSRScanner->ReadLine();
		if('{' != (*pHSRScanner->m_aryFactor)[0][0]) throw exception("expect '{'");
		
		vector<STRING_T> oResult;
		while(pHSRScanner->ReadLine())
		{
			if('}' == (*pHSRScanner->m_aryFactor)[0][0]) break;
			
			CTokenizer<CIsFromString>::Tokenize(oResult , (*pHSRScanner->m_pstrLine) , CIsFromString(_T("=")));
			if(STRING_T(_T("LOOP")) == oResult[0])
			{
				if(m_pLoopElement) m_pLoopElement->ParseLine(pHSRScanner);
			}
			else if(STRING_T(_T("HEIGHT")) == oResult[0])
			{
				m_nHeight = ATOF_T(oResult[1].c_str());
			}
			else if(STRING_T(_T("COLOR")) == oResult[0])
			{
				SetColorString(oResult[1]);
			}
			else if((2 == oResult.size()) && (STRING_T(_T("AXIS")) == oResult[0]))
			{
				vector<STRING_T> oValues;
				CTokenizer<CIsComma>::Tokenize(oValues , oResult[1].c_str() , CIsComma());
				if(oValues.size() > 2)
				{
					m_axis.dx = ATOF_T(oValues[0].c_str());
					m_axis.dy = ATOF_T(oValues[1].c_str());
					m_axis.dz = ATOF_T(oValues[2].c_str());
				}
			}
			//! added by HumKyung.Baek - 2010.11.12
			else if(0 == STRICMP_T(_T("TYPE") , oResult[0].c_str()))
			{
				m_Type = (0 == STRICMP_T(_T("SURFACE") , oResult[1].c_str())) ? CHSRExtruElement::SURFACE : CHSRExtruElement::SOLID;
			}
		}

		CalcVolume();
		bRet = true;
	}

	return bRet;
}

/**	@brief	볼륨을 계산한다. 각 실린더의 끝점으로 볼륨을 계산한다.
	
	@author	BHK

	@remarks

	@return	void	
*/
void CHSRExtruElement::CalcVolume()
{
	if(m_pLoopElement)
	{
		double zhigh = 0.f;
		vector<POINT_T> BasePointEntry;
		const int nCount = m_pLoopElement->GetShapePointSet(BasePointEntry , zhigh);
		if(nCount > 0)
		{
			m_vol.x[0] = m_vol.x[1] = BasePointEntry.front().x;
			m_vol.y[0] = m_vol.y[1] = BasePointEntry.front().y;
			m_vol.z[0] = m_vol.z[1] = BasePointEntry.front().z;
			for(vector<POINT_T>::iterator itr = BasePointEntry.begin();itr != BasePointEntry.end();++itr)
			{
				POINT_T pt = (*itr);
				if(m_vol.x[0] > pt.x) m_vol.x[0] = pt.x;
				if(m_vol.x[1] < pt.x) m_vol.x[1] = pt.x;
				if(m_vol.y[0] > pt.y) m_vol.y[0] = pt.y;
				if(m_vol.y[1] < pt.y) m_vol.y[1] = pt.y;
				if(m_vol.z[0] > pt.z) m_vol.z[0] = pt.z;
				if(m_vol.z[1] < pt.z) m_vol.z[1] = pt.z;

				pt.x += m_axis.dx * m_nHeight;
				pt.y += m_axis.dy * m_nHeight;
				pt.z += m_axis.dz * m_nHeight;
				if(m_vol.x[0] > pt.x) m_vol.x[0] = pt.x;
				if(m_vol.x[1] < pt.x) m_vol.x[1] = pt.x;
				if(m_vol.y[0] > pt.y) m_vol.y[0] = pt.y;
				if(m_vol.y[1] < pt.y) m_vol.y[1] = pt.y;
				if(m_vol.z[0] > pt.z) m_vol.z[0] = pt.z;
				if(m_vol.z[1] < pt.z) m_vol.z[1] = pt.z;
			}
		}
	}
}

/**	
	@brief	뷰의 볼륨에 속하는지 검사를 한다.
	
	@author	humkyung

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CHSRExtruElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;
}

/**
	@brief	EXTRU를 이루는 형상들을 생성한다.
	
	@author	humkyung

	@date	????.??.??

	@param	pViewFormat
	@param	nID

	@return	
*/
CHSRFace* CHSRExtruElement::CreateFace(CHSRViewFormat* pViewFormat , long& nID)
{
	assert(pViewFormat && "pViewFormat is NULL");

	if(pViewFormat && m_pLoopElement)
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();
		const long body = nID;	/// save id for body id - 2012.01.12 added by humkyung

		double zhigh = 0.f;
		vector<POINT_T> BasePointEntry , TempPointEntry;
		m_pLoopElement->GetShapePointSet(BasePointEntry , zhigh);
		//! create two faces if boundary is closed.
		if(CHSRExtruElement::SOLID == m_Type)
		{
			if((BasePointEntry.size() > 1) && 
				CHSRElement::IsSamePoint(BasePointEntry[0] , BasePointEntry[BasePointEntry.size() - 1]))
			{
				for(vector<POINT_T>::iterator itr = BasePointEntry.begin();itr != BasePointEntry.end();++itr)
				{
					TempPointEntry.push_back(pViewFormat->MODEL2VIEW(*itr));
				}
				
				//! Base면 데이타를 생성합니다.
				CHSRFace* pFace = NULL;
				if(pFace = CHSRFace::CreateFace(TempPointEntry.size() , &(TempPointEntry)[0]))
				{
					pFace->m_enmLayer = GetHSRLayer();				
					pFace->type       = OTHER;
					pFace->id         = nID++;
					pFace->ele        = HSR_RECTANGLE_SHAPE;
					pFace->radius     = 0.f;
					pFace->body		  = this;	/// 2012.01.12 added by humkyung
					pFace->SetColor(*m_pstrColor);
					
					pHSRApp->AppendFace(NULL , pFace);
				}

				vector<POINT_T> TopPointEntry;
				{
					for(vector<POINT_T>::iterator itr = BasePointEntry.begin();itr != BasePointEntry.end();++itr)
					{
						POINT_T pt;
						pt.x = itr->x + m_axis.dx * m_nHeight;
						pt.y = itr->y + m_axis.dy * m_nHeight;
						pt.z = itr->z + m_axis.dz * m_nHeight;
						
						TopPointEntry.push_back(pViewFormat->MODEL2VIEW(pt));
					}
					
					//! TOP 면 데이타를 생성합니다.
					CHSRFace* pFace = NULL;
					if(pFace = CHSRFace::CreateFace(TopPointEntry.size() , &(TopPointEntry)[0]))
					{
						pFace->m_enmLayer = GetHSRLayer();				
						pFace->type       = OTHER;
						pFace->id         = nID++;
						pFace->ele        = HSR_RECTANGLE_SHAPE;
						pFace->radius     = 0.f;
						pFace->body		  = this;	/// 2012.01.12 added by humkyung
						pFace->SetColor(*m_pstrColor);
						
						pHSRApp->AppendFace(NULL , pFace);
					}
				}
			}
		}

		//! 두 단면을 연결하는 옆면들을 생성한다.
		for(int i = 0;(i + 1) < int(BasePointEntry.size());++i)
		{
			POINT_T pts[4];

			pts[0] = BasePointEntry[i];
			pts[1] = BasePointEntry[i + 1];
			pts[2] = pts[1];
			pts[2].x += m_axis.dx * m_nHeight;
			pts[2].y += m_axis.dy * m_nHeight;
			pts[2].z += m_axis.dz * m_nHeight;
			pts[3] = pts[0];
			pts[3].x += m_axis.dx * m_nHeight;
			pts[3].y += m_axis.dy * m_nHeight;
			pts[3].z += m_axis.dz * m_nHeight;

			pts[0] = pViewFormat->MODEL2VIEW(pts[0]);
			pts[1] = pViewFormat->MODEL2VIEW(pts[1]);
			pts[2] = pViewFormat->MODEL2VIEW(pts[2]);
			pts[3] = pViewFormat->MODEL2VIEW(pts[3]);

			//! 면 데이타를 생성합니다.
			CHSRFace* pFace = NULL;
			if(pFace = CHSRFace::CreateFace(4 , pts))
			{
				pFace->m_enmLayer = GetHSRLayer();				
				pFace->type       = OTHER;
				pFace->id         = nID++;
				pFace->ele        = HSR_RECTANGLE_SHAPE;
				pFace->radius     = 0.f;
				pFace->body		  = this;	/// 2012.01.12 added by humkyung
				pFace->SetColor(*m_pstrColor);
				
				/// set edge property(primary or construction) - 2012.01.11 added by humkyung
				if(pFace->pHead && pFace->pHead->next && pFace->pHead->next->next && pFace->pHead->next->next->next)
				{
					if(PT_CONSTRUCTION == pts[0].reserved)
					{
						pFace->pHead->next->next->next->render = INVALID;
					}
					if(PT_CONSTRUCTION == pts[1].reserved)
					{
						pFace->pHead->next->render = INVALID;
					}
				}
				
				/// up to here

				pHSRApp->AppendFace(NULL , pFace);
			}
		}
	}

	return NULL;
}

/**
	@brief	면을 생성하는 LINE들을 연결 순서대로 재정렬한다.

	@author	BHK
*/
void CHSRExtruElement::AlignLines()
{
	vector<LINE_T> TempLineEntry;
	
	/*
	if(m_pLineEntry && !m_pLineEntry->empty())
	{
		//! 한쪽만 다른 LINE와 연결되는 LINE를 찾는다.
		for(vector<LINE_T>::iterator itr = m_pLineEntry->begin();itr != m_pLineEntry->end();++itr)
		{
			int conn[2] = {0 , 0} , desconn[2] = {0 , 0};
			for(vector<LINE_T>::iterator jtr = m_pLineEntry->begin();jtr != m_pLineEntry->end();++jtr)
			{
				if(itr == jtr) continue;
				CheckConnection(conn , *itr , *jtr , desconn) ;
			}
			
			if((1 == conn[0]) && (0 == conn[1]))
			{
				LINE_T line(*itr);
				swap(line.ptStart , line.ptEnd);
				TempLineEntry.push_back(line);
				m_pLineEntry->erase(itr);
				break;
			}
			else if((0 == conn[0]) && (1 == conn[1]))
			{
				LINE_T line(*itr);
				TempLineEntry.push_back(line);
				m_pLineEntry->erase(itr);
				break;
			}
		}
		
		if(!TempLineEntry.empty())
		{
			while(!m_pLineEntry->empty())
			{
				LINE_T cylinder(TempLineEntry.back());
				size_t start = m_pLineEntry->size();
				for(vector<LINE_T>::iterator itr = m_pLineEntry->begin();itr != m_pLineEntry->end();++itr)
				{
					int conn[2] = {0 , 0} , desconn[2] = {0 , 0};
					if(CheckConnection(conn , cylinder , *itr , desconn))
					{
						if((0 == conn[0]) && (1 == conn[1]))
						{
							LINE_T rhs(*itr);
							if((0 == desconn[0]) && (1 == desconn[1])) swap(rhs.ptStart , rhs.ptEnd);
							
							TempLineEntry.push_back(rhs);
							m_pLineEntry->erase(itr);
							break;
						}
					}
				}
				size_t end = m_pLineEntry->size();
				
				//! 크기의 변동이 없으면 무한 루프를 막기위해서 루프를 빠져 나온다.
				if(start == end) break;
			}
			
			assert(m_pLineEntry->empty());
			m_pLineEntry->clear();
			m_pLineEntry->insert(m_pLineEntry->begin() , TempLineEntry.begin() , TempLineEntry.end());
		}
	}
	*/
}

/**
	@brief	두 LINE의 연결 방식을 검사한다.
	@author	백흠경

	@param	conn = conn[0]이 1이면 lhs의 START POINT가 연결 지점 , conn[1]이 1이면 lhs의 END POINT가 연결 지점
	@param	lhs
	@param	rhs
	@param	desconn = destconn[0]이 1이면 rhs의 START POINT가 연결 지점 , destconn[1]이 1이면 rhs의 END POINT가 연결 지점

	@return	bool = 서로 연결되면 true , otherwise return false
*/
bool CHSRExtruElement::CheckConnection(int conn[], const LINE_T &lhs, const LINE_T &rhs , int desconn[])
{
	double dx = lhs.ptStart.x - rhs.ptStart.x;
	double dy = lhs.ptStart.y - rhs.ptStart.y;
	double dz = lhs.ptStart.z - rhs.ptStart.z;
	double len= sqrt(dx * dx + dy * dy + dz * dz);
	if(len < HSR_TOLER)
	{
		++conn[0];
		++desconn[0];
	}
	
	dx = lhs.ptStart.x - rhs.ptEnd.x;
	dy = lhs.ptStart.y - rhs.ptEnd.y;
	dz = lhs.ptStart.z - rhs.ptEnd.z;
	len= sqrt(dx * dx + dy * dy + dz * dz);
	if(len < HSR_TOLER)
	{
		++conn[0];
		++desconn[1];
	}
	
	dx = lhs.ptEnd.x - rhs.ptStart.x;
	dy = lhs.ptEnd.y - rhs.ptStart.y;
	dz = lhs.ptEnd.z - rhs.ptStart.z;
	len= sqrt(dx * dx + dy * dy + dz * dz);
	if(len < HSR_TOLER)
	{
		++conn[1];
		++desconn[0];
	}
	
	dx = lhs.ptEnd.x - rhs.ptEnd.x;
	dy = lhs.ptEnd.y - rhs.ptEnd.y;
	dz = lhs.ptEnd.z - rhs.ptEnd.z;
	len= sqrt(dx * dx + dy * dy + dz * dz);
	if(len < HSR_TOLER)
	{
		++conn[1];
		++desconn[1];
	}
	
	return ((conn[0] > 0) || (conn[1] > 0));
}
