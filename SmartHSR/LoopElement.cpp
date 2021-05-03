// LoopElement.cpp: implementation of the CLoopElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LoopElement.h"
#include "SmartHSR.h"
#include "LineElement.h"
#include "ArcElement.h"
#include "3d_FaceElement.h"
#include "EllipseElement.h"
#include "Tokenizer.h"

IMPLEMENTS_HSR_ELEMENT(CLoopElement , CHSRElement , _T("LOOP="));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static long logloop = 0L;

CLoopElement::CLoopElement() : CHSRElement() , m_creation(SOLID)
{
	m_log = (++logloop);
}

CLoopElement::~CLoopElement()
{
	try
	{
		for(list<CHSRElement*>::iterator itr = m_ElementEntry.begin();itr != m_ElementEntry.end();++itr)
		{
			delete (*itr);
		}
	}
	catch(...)
	{
	}
}

/**
	@brief	parse loop element texts

	@author	BHK

	@date	?

	@param	pHSRScanner

	@return	true if success,otherwise false
*/
bool CLoopElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet = false;

	if(pHSRScanner)
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;
		
		pHSRScanner->ReadLine();
		if('{' != (*pHSRScanner->m_aryFactor)[0][0]) throw exception("expect '{'");

		while(pHSRScanner->ReadLine())
		{
			if('}' == (*pHSRScanner->m_aryFactor)[0][0]) break;

			//! read loop configuration
			//! date : 2009.10.08 
			if(0 == STRNCMP_T(_T("CREATION") , (*pHSRScanner->m_aryFactor)[0].c_str() , STRLEN_T(_T("CREATION"))))
			{
				vector<STRING_T> oResult;
				CTokenizer<CIsFromString>::Tokenize(oResult , (*pHSRScanner->m_aryFactor)[0].c_str() , CIsFromString(_T("=")));
				if(2 == oResult.size())
				{
					m_creation = (_T("HOLE") == oResult[1]) ? HOLE : SOLID;
				}
				continue;
			}

			if(0 == STRCMP_T((*pHSRScanner->m_aryFactor)[0].c_str(), _T("LINE=")))
			{
				CLineElement* pElm=NULL;
				if(pElm = new CLineElement)
				{
					if(pElm->ParseLine(pHSRScanner)) m_ElementEntry.push_back(pElm);
				}
			}
			else if(0 == STRCMP_T((*pHSRScanner->m_aryFactor)[1].c_str(), _T("HLINE")))
			{
				CHLineElement* pElm=NULL;
				if(pElm = new CHLineElement)
				{
					if(pElm->ParseLine(pHSRScanner)) m_ElementEntry.push_back(pElm);
				}
			}
			else if(0 == STRCMP_T((*pHSRScanner->m_aryFactor)[0].c_str(), _T("ARC")))
			{
				CArcElement* pElm=NULL;
				if(pElm = new CArcElement)
				{
					if(pElm->ParseLine(pHSRScanner)) m_ElementEntry.push_back(pElm);
				}
			}
			else if(C3d_FaceElement::TypeString() == (*pHSRScanner->m_aryFactor)[0])
			{
				C3d_FaceElement* pElm=NULL;
				if(pElm = static_cast<C3d_FaceElement*>(C3d_FaceElement::CreateInstance()))
				{
					if(pElm->ParseLine(pHSRScanner)) m_ElementEntry.push_back(pElm);
				}
			}
			else if(CEllipseElement::TypeString() == (*pHSRScanner->m_aryFactor)[1])
			{
				CEllipseElement* pElm=NULL;
				if(pElm = static_cast<CEllipseElement*>(CEllipseElement::CreateInstance()))
				{
					if(pElm->ParseLine(pHSRScanner)) m_ElementEntry.push_back(pElm);
				}
			}
		}

		try
		{
			CalcVolume();
		}
		catch(...)
		{
			::OutputDebugString(_T("Error in CalcValume"));
		}
		bRet = true;
	}

	return bRet;
}

/**	
	@brief	뷰의 볼륨에 속하는지 검사를 한다.
	
	@author	humkyung

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CLoopElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;
}

/**	
	@brief	calculate voluem of LoopElement
	
	@author	BHK

	@date	2009.09.25

	@param	

	@return	
*/
void CLoopElement::CalcVolume()
{
	if(!m_ElementEntry.empty())
	{
		/*
		CLineElement* pLineEle = static_cast<CLineElement*>(m_ElementEntry.front());

		m_vol.x[0] = m_vol.x[1] = pLineEle->m_pt[0].x;
		m_vol.y[0] = m_vol.y[1] = pLineEle->m_pt[0].y;
		m_vol.z[0] = m_vol.z[1] = pLineEle->m_pt[0].z;
		*/
		bool bFirst = true;
		for(list<CHSRElement*>::iterator itr = m_ElementEntry.begin();itr != m_ElementEntry.end();++itr)
		{
			if((*itr)->IsKindOf(CLineElement::TypeString()))
			{
				CLineElement* pLineEle = static_cast<CLineElement*>(*itr);
				if(bFirst)
				{
					m_vol.x[0] = m_vol.x[1] = pLineEle->m_pt[0].x;
					m_vol.y[0] = m_vol.y[1] = pLineEle->m_pt[0].y;
					m_vol.z[0] = m_vol.z[1] = pLineEle->m_pt[0].z;
				}

				POINT_T pt = pLineEle->m_pt[0];
				if(m_vol.x[0] > pt.x) m_vol.x[0] = pt.x;
				if(m_vol.x[1] < pt.x) m_vol.x[1] = pt.x;
				if(m_vol.y[0] > pt.y) m_vol.y[0] = pt.y;
				if(m_vol.y[1] < pt.y) m_vol.y[1] = pt.y;
				if(m_vol.z[0] > pt.z) m_vol.z[0] = pt.z;
				if(m_vol.z[1] < pt.z) m_vol.z[1] = pt.z;

				pt = pLineEle->m_pt[1];
				if(m_vol.x[0] > pt.x) m_vol.x[0] = pt.x;
				if(m_vol.x[1] < pt.x) m_vol.x[1] = pt.x;
				if(m_vol.y[0] > pt.y) m_vol.y[0] = pt.y;
				if(m_vol.y[1] < pt.y) m_vol.y[1] = pt.y;
				if(m_vol.z[0] > pt.z) m_vol.z[0] = pt.z;
				if(m_vol.z[1] < pt.z) m_vol.z[1] = pt.z;

				bFirst = false;
			}
			else if((*itr)->IsKindOf(CHLineElement::TypeString()))
			{
				CHLineElement* pLineEle = static_cast<CHLineElement*>(*itr);
				if(bFirst)
				{
					m_vol.x[0] = m_vol.x[1] = pLineEle->m_pt[0].x;
					m_vol.y[0] = m_vol.y[1] = pLineEle->m_pt[0].y;
					m_vol.z[0] = m_vol.z[1] = pLineEle->m_pt[0].z;
				}
				
				POINT_T pt = pLineEle->m_pt[0];
				if(m_vol.x[0] > pt.x) m_vol.x[0] = pt.x;
				if(m_vol.x[1] < pt.x) m_vol.x[1] = pt.x;
				if(m_vol.y[0] > pt.y) m_vol.y[0] = pt.y;
				if(m_vol.y[1] < pt.y) m_vol.y[1] = pt.y;
				if(m_vol.z[0] > pt.z) m_vol.z[0] = pt.z;
				if(m_vol.z[1] < pt.z) m_vol.z[1] = pt.z;
				
				pt = pLineEle->m_pt[1];
				if(m_vol.x[0] > pt.x) m_vol.x[0] = pt.x;
				if(m_vol.x[1] < pt.x) m_vol.x[1] = pt.x;
				if(m_vol.y[0] > pt.y) m_vol.y[0] = pt.y;
				if(m_vol.y[1] < pt.y) m_vol.y[1] = pt.y;
				if(m_vol.z[0] > pt.z) m_vol.z[0] = pt.z;
				if(m_vol.z[1] < pt.z) m_vol.z[1] = pt.z;
				
				bFirst = false;
			}
			else if((*itr)->IsKindOf(CArcElement::TypeString()))
			{
				CArcElement* pArcEle = static_cast<CArcElement*>(*itr);
				if(bFirst)
				{
					m_vol.x[0] = m_vol.x[1] = pArcEle->GetCenterPoint().x;
					m_vol.y[0] = m_vol.y[1] = pArcEle->GetCenterPoint().y;
					m_vol.z[0] = m_vol.z[1] = pArcEle->GetCenterPoint().z;
				}
				POINT_T pt = pArcEle->GetCenterPoint();
				if(m_vol.x[0] > pt.x) m_vol.x[0] = pt.x;
				if(m_vol.x[1] < pt.x) m_vol.x[1] = pt.x;
				if(m_vol.y[0] > pt.y) m_vol.y[0] = pt.y;
				if(m_vol.y[1] < pt.y) m_vol.y[1] = pt.y;
				if(m_vol.z[0] > pt.z) m_vol.z[0] = pt.z;
				if(m_vol.z[1] < pt.z) m_vol.z[1] = pt.z;

				pt = pArcEle->GetStartPoint();
				if(m_vol.x[0] > pt.x) m_vol.x[0] = pt.x;
				if(m_vol.x[1] < pt.x) m_vol.x[1] = pt.x;
				if(m_vol.y[0] > pt.y) m_vol.y[0] = pt.y;
				if(m_vol.y[1] < pt.y) m_vol.y[1] = pt.y;
				if(m_vol.z[0] > pt.z) m_vol.z[0] = pt.z;
				if(m_vol.z[1] < pt.z) m_vol.z[1] = pt.z;

				bFirst = false;
			}
			else if((*itr)->IsKindOf(C3d_FaceElement::TypeString()))
			{
				C3d_FaceElement* p3d_FaceElm = static_cast<C3d_FaceElement*>(*itr);
				if(p3d_FaceElm)
				{
					for(int i = 0;i < p3d_FaceElm->GetVertexCount();++i)
					{
						if(bFirst)
						{
							m_vol.x[0] = m_vol.x[1] = p3d_FaceElm->GetVertexAt(i).x;
							m_vol.y[0] = m_vol.y[1] = p3d_FaceElm->GetVertexAt(i).y;
							m_vol.z[0] = m_vol.z[1] = p3d_FaceElm->GetVertexAt(i).z;
						}

						POINT_T pt = p3d_FaceElm->GetVertexAt(i);
						if(m_vol.x[0] > pt.x) m_vol.x[0] = pt.x;
						if(m_vol.x[1] < pt.x) m_vol.x[1] = pt.x;
						if(m_vol.y[0] > pt.y) m_vol.y[0] = pt.y;
						if(m_vol.y[1] < pt.y) m_vol.y[1] = pt.y;
						if(m_vol.z[0] > pt.z) m_vol.z[0] = pt.z;
						if(m_vol.z[1] < pt.z) m_vol.z[1] = pt.z;
						bFirst = false;
					}
				}
			}
			else if((*itr)->IsKindOf(CEllipseElement::TypeString()))
			{
				CEllipseElement* pEllipseElm = static_cast<CEllipseElement*>(*itr);
				if(pEllipseElm)
				{
					if(bFirst) m_vol = pEllipseElm->volume();
					else
					{
						VOLUME_T vol = pEllipseElm->volume();
						if(m_vol.x[LOW] > vol.x[LOW]) m_vol.x[LOW] = vol.x[LOW];
						if(m_vol.x[HIGH] < vol.x[HIGH]) m_vol.x[HIGH] = vol.x[HIGH];
						if(m_vol.y[LOW] > vol.y[LOW]) m_vol.y[LOW] = vol.y[LOW];
						if(m_vol.y[HIGH] < vol.y[HIGH]) m_vol.y[HIGH] = vol.y[HIGH];
						if(m_vol.z[LOW] > vol.z[LOW]) m_vol.z[LOW] = vol.z[LOW];
						if(m_vol.z[HIGH] < vol.z[HIGH]) m_vol.z[HIGH] = vol.z[HIGH];
					}
					
					bFirst = false;
				}
			}
		}
	}
}

///static ofstream ofile;

/**	
	@brief	create a face for LoopElement
	
	@author	BHK

	@param	pViewFormat - 
	@param	nID -

	@return	CHSRFace	NULL.
*/	
CHSRFace* CLoopElement::CreateFace(CHSRViewFormat* pViewFormat,long& nID)
{
	bool bAllHLineElm = true , bAllInvisible = true;
	for(list<CHSRElement*>::iterator itr = m_ElementEntry.begin();itr != m_ElementEntry.end();++itr)
	{
		CHSRElement* pElm = *itr;
		if(!(*itr)->IsKindOf(CHLineElement::TypeString()))
		{
			bAllHLineElm = false;
			break;
		}
		else
		{
			CHLineElement* pElm = static_cast<CHLineElement*>(*itr);
			if(pElm->m_bVisible) bAllInvisible = false;
		}
	}

	if(false == bAllHLineElm)
	{
		auto_ptr<CHSRNormalFace> NormalFace(new CHSRNormalFace(0));
		NormalFace->SetLayerString((*m_pstrLayer));
		
		for(list<CHSRElement*>::iterator itr = m_ElementEntry.begin();itr != m_ElementEntry.end();++itr)
		{
			if((*itr)->IsKindOf(CLineElement::TypeString()))
			{
				(*NormalFace) += static_cast<CLineElement*>(*itr);
			}
			else if((*itr)->IsKindOf(CArcElement::TypeString()))
			{
				(*NormalFace) += static_cast<CArcElement*>(*itr);
			}
			else if((*itr)->IsKindOf(C3d_FaceElement::TypeString()))
			{
				C3d_FaceElement* p3d_FaceElm = static_cast<C3d_FaceElement*>(*itr);
				if(p3d_FaceElm)
				{
					p3d_FaceElm->CreateLineSegments();
					for(list<CLineElement*>::iterator jtr = p3d_FaceElm->m_LineElementEntry.begin();jtr != p3d_FaceElm->m_LineElementEntry.end();++jtr)
					{
						(*NormalFace) += static_cast<CLineElement*>(*jtr);
					}
				}
			}
			else if((*itr)->IsKindOf(CEllipseElement::TypeString()))
			{
				CEllipseElement* pEllipseElm = static_cast<CEllipseElement*>(*itr);
				if(pEllipseElm)
				{
					pEllipseElm->CreateLineElements();
					for(list<CLineElement*>::iterator jtr = pEllipseElm->m_LineElementEntry.begin();jtr != pEllipseElm->m_LineElementEntry.end();++jtr)
					{
						(*NormalFace) += static_cast<CLineElement*>(*jtr);
					}
				}
			}
		}

		return NormalFace->CreateFace(pViewFormat , nID);
	}
	else if(false == bAllInvisible)
	{
		if(m_ElementEntry.size() > 2)
		{
			auto_ptr<CPreHNormalFace> HNormalFace(new CPreHNormalFace(0));
			HNormalFace->SetLayerString((*m_pstrLayer));
			
			for(list<CHSRElement*>::iterator itr = m_ElementEntry.begin();itr != m_ElementEntry.end();++itr)
			{
				if((*itr)->IsKindOf(CHLineElement::TypeString()))
				{
					(*HNormalFace) += static_cast<CHLineElement*>(*itr);
				}
			}

			return HNormalFace->CreateFace(pViewFormat , nID);
		}
	}

	return NULL;
}

/**
	@brief	get point set of shape

	@author	BHK

	@param	pts
	@param	zhigh

	@return	
*/
int CLoopElement::GetShapePointSet(vector<POINT_T>& pts , double& zhigh)
{
	auto_ptr<CHSRNormalFace> NormalFace(new CHSRNormalFace(0));
	NormalFace->SetLayerString((*m_pstrLayer));
	for(list<CHSRElement*>::iterator itr = m_ElementEntry.begin();itr != m_ElementEntry.end();++itr)
	{
		if((*itr)->IsKindOf(CLineElement::TypeString()))
		{
			/*
			CLineElement* pLineElm = static_cast<CLineElement*>(*itr);
			pts.push_back(pLineElm->m_pt[0]);
			pts.push_back(pLineElm->m_pt[1]);
			*/

			(*NormalFace) += static_cast<CLineElement*>(*itr);
		}
		else if((*itr)->IsKindOf(CArcElement::TypeString()))
		{
			CArcElement* pArcElm = static_cast<CArcElement*>(*itr);
			if(pArcElm)
			{
				/*
				pArcElm->CreateLineElements();
				for(list<CLineElement*>::iterator jtr = pArcElm->m_LineElementEntry.begin();jtr != pArcElm->m_LineElementEntry.end();++jtr)
				{
					CLineElement* pLineElm = static_cast<CLineElement*>(*jtr);
					pts.push_back(pLineElm->m_pt[0]);
					pts.push_back(pLineElm->m_pt[1]);
				}
				*/
				(*NormalFace) += static_cast<CArcElement*>(*itr);
			}
		}
		else if((*itr)->IsKindOf(C3d_FaceElement::TypeString()))
		{
			C3d_FaceElement* p3d_FaceElm = static_cast<C3d_FaceElement*>(*itr);
			if(p3d_FaceElm)
			{
				p3d_FaceElm->CreateLineSegments();
				for(list<CLineElement*>::iterator jtr = p3d_FaceElm->m_LineElementEntry.begin();jtr != p3d_FaceElm->m_LineElementEntry.end();++jtr)
				{
					CLineElement* pLineElm = static_cast<CLineElement*>(*jtr);
					///pts.push_back(pLineElm->m_pt[0]);
					///pts.push_back(pLineElm->m_pt[1]);
					(*NormalFace) += static_cast<CLineElement*>(*jtr);
				}
			}
		}
		else if((*itr)->IsKindOf(CEllipseElement::TypeString()))
		{
			CEllipseElement* pEllipseElm = static_cast<CEllipseElement*>(*itr);
			if(pEllipseElm)
			{
				CLineElement* pLastLine = NormalFace->GetLastLineElement();

				bool bReverse = false;
				pEllipseElm->CreateLineElements();
				//! check connection
				if(NULL != pLastLine)
				{
					POINT_T ptConn = pLastLine->m_pt[1];

					POINT_T ptFront = pEllipseElm->m_LineElementEntry.front()->m_pt[0];
					POINT_T ptBack  = pEllipseElm->m_LineElementEntry.back()->m_pt[1];
					
					double dx = ptFront.x - ptConn.x;
					double dy = ptFront.y - ptConn.y;
					double dz = ptFront.z - ptConn.z;
					const double dDistToFront = sqrt(dx*dx + dy*dy + dz*dz);

					dx = ptBack.x - ptConn.x;
					dy = ptBack.y - ptConn.y;
					dz = ptBack.z - ptConn.z;
					const double dDistToBack = sqrt(dx*dx + dy*dy + dz*dz);

					bReverse = (dDistToBack < dDistToFront);
				}

				if(!bReverse)
				{
					for(list<CLineElement*>::iterator jtr = pEllipseElm->m_LineElementEntry.begin();jtr != pEllipseElm->m_LineElementEntry.end();++jtr)
					{
						CLineElement* pLineElm = static_cast<CLineElement*>(*jtr);
						(*NormalFace) += static_cast<CLineElement*>(*jtr);
					}
				}
				else
				{
					for(list<CLineElement*>::reverse_iterator jtr = pEllipseElm->m_LineElementEntry.rbegin();jtr != pEllipseElm->m_LineElementEntry.rend();++jtr)
					{
						CLineElement* pLineElm = static_cast<CLineElement*>(*jtr);
						(*NormalFace) += static_cast<CLineElement*>(*jtr);
					}
				}
			}
		}
	}
	
	return NormalFace->GetShapePointSet(pts , zhigh);
}