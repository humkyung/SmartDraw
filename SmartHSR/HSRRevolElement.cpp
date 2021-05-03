// HSRRevolElement.cpp: implementation of the CHSRRevolElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HSRRevolElement.h"
#include "Tokenizer.h"
#include "SmartHSR.h"
#include "IsVolume.h"

IMPLEMENTS_HSR_ELEMENT(CHSRRevolElement , CHSRElement , _T("REVO="));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHSRRevolElement::CHSRRevolElement() : m_nAngle(0.f)
{
	 m_ptStart.x = 0.f;
	 m_ptStart.y = 0.f;
	 m_ptStart.z = 0.f;
	 m_ptEnd.x = 0.f;
	 m_ptEnd.y = 0.f;
	 m_ptEnd.z = 0.f;

	 m_pLoopElement = new CLoopElement;
}

CHSRRevolElement::~CHSRRevolElement()
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
	@brief	REVOLUTION ELEMENT를 파싱한다.
	
	@author	HumKyung.BAEK

	@param	pHSRScanner	a parameter of type CHSRScanner*

	@return	bool	
*/
bool CHSRRevolElement::ParseLine(CHSRScanner* pHSRScanner)
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
			else if(STRING_T(_T("ANGLE")) == oResult[0])
			{
				m_nAngle = ATOF_T(oResult[1].c_str());
			}
			else if((2 == oResult.size()) && (STRING_T(_T("START")) == oResult[0]))
			{
				vector<STRING_T> oValues;
				CTokenizer<CIsComma>::Tokenize(oValues , oResult[1].c_str() , CIsComma());
				if(oValues.size() > 2)
				{
					m_ptStart.x = ATOF_T(oValues[0].c_str());
					m_ptStart.y = ATOF_T(oValues[1].c_str());
					m_ptStart.z = ATOF_T(oValues[2].c_str());
				}
			}
			else if((2 == oResult.size()) && (STRING_T(_T("END")) == oResult[0]))
			{
				vector<STRING_T> oValues;
				CTokenizer<CIsComma>::Tokenize(oValues , oResult[1].c_str() , CIsComma());
				if(oValues.size() > 2)
				{
					m_ptEnd.x = ATOF_T(oValues[0].c_str());
					m_ptEnd.y = ATOF_T(oValues[1].c_str());
					m_ptEnd.z = ATOF_T(oValues[2].c_str());
				}
			}
		}
		VECTOR_T axis;
		axis.dx = m_ptEnd.x - m_ptStart.x;
		axis.dy = m_ptEnd.y - m_ptStart.y;
		axis.dz = m_ptEnd.z - m_ptStart.z;

		Revolve(axis , m_nAngle , 8 , m_ptStart);
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
void CHSRRevolElement::CalcVolume()
{
	if(m_pLoopElement)
	{
		double zhigh = 0.f;
		vector<POINT_T> pts;
		m_pLoopElement->GetShapePointSet(pts , zhigh);
		const int nCount = pts.size();
		
		CIsVolume volume;
		for(int j = 0;j < 8 + 1;j++)
		{
			for(int i = 0;i < nCount;i++)
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
	}
}

/**	
	@brief	뷰의 볼륨에 속하는지 검사를 한다.
	
	@author	humkyung

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CHSRRevolElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;
}

/**
	@brief	REVOLUTION.를 이루는 형상들을 생성한다.
	
	@author	BHK

	@date	?

	@param	pViewFormat
	@param	nID

	@return	
*/
CHSRFace* CHSRRevolElement::CreateFace(CHSRViewFormat* pViewFormat , long& nID)
{
	assert(pViewFormat && "pViewFormat is NULL");

	if(pViewFormat && m_pLoopElement)
	{
		CHSRApp* pHSRApp = CHSRApp::GetInstance();

		double zhigh = 0.f;
		vector<POINT_T> BasePointEntry , TempPointEntry;
		m_pLoopElement->GetShapePointSet(BasePointEntry , zhigh);
		
		if(BasePointEntry.size() <= 1) return NULL;

		const int nSize = BasePointEntry.size();
		POINT_T ptEdge[5];
		for(int j = 0;j < 8;j++)
		{
			for(int i = 0;i < nSize - 1;++i)
			{
				ptEdge[0] = m_pts[j * nSize + i];
				ptEdge[1] = m_pts[j * nSize + i + 1];
				ptEdge[2] = m_pts[(j + 1) * nSize + i + 1];
				ptEdge[3] = m_pts[(j + 1) * nSize + i];
				ptEdge[4] = ptEdge[0];
				
				ptEdge[0]  = pViewFormat->MODEL2VIEW(ptEdge[0]);
				ptEdge[1]  = pViewFormat->MODEL2VIEW(ptEdge[1]);
				ptEdge[2]  = pViewFormat->MODEL2VIEW(ptEdge[2]);
				ptEdge[3]  = pViewFormat->MODEL2VIEW(ptEdge[3]);
				ptEdge[4]  = pViewFormat->MODEL2VIEW(ptEdge[4]);
				
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
	}

	return NULL;
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
bool CHSRRevolElement::CheckConnection(int conn[], const LINE_T &lhs, const LINE_T &rhs , int desconn[])
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

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.04.19
*/
void CHSRRevolElement::Revolve(const VECTOR_T &vecAxis, const double &nRotate, const int &nParts , const POINT_T& shift)
{
	assert((nParts > 0) && "nParts is less than 0.");
	
	if(m_pLoopElement && (nParts > 0))
	{
		double zhigh = 0.f;
		vector<POINT_T> pts;
		m_pLoopElement->GetShapePointSet(pts , zhigh);

		double alpha=0.,beta=0.;
		CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
		
		size_t nSize = pts.size();
		// set vecAxis to z<0,0,1> by rotating about x,y axis.
		for(int i = 0;i < int(nSize);++i)
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