// 3d_FaceElement.cpp: implementation of the C3d_FaceElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmartHSR.h"
#include "3d_FaceElement.h"

IMPLEMENTS_HSR_ELEMENT(C3d_FaceElement , CHSRElement , _T("<3D_FACE>"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3d_FaceElement::C3d_FaceElement()
{
}

C3d_FaceElement::~C3d_FaceElement()
{
	try
	{
		for(list<CLineElement*>::iterator itr = m_LineElementEntry.begin();itr != m_LineElementEntry.end();++itr)
		{
			delete (*itr);
		}
		m_LineElementEntry.clear();
	}
	catch(...)
	{
	}
}

/**	
	@brief	The C3d_FaceElement::ParseLine function

	@author	HumKyung.BAEK

	@date	2009.11.02

	@param	pHSRScanner	a parameter of type CHSRScanner*

	@return	bool	
*/
bool C3d_FaceElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;
		for(int i = 1;i < int(pHSRScanner->m_aryFactor->size());++i)
		{
			POINT_T pt;
			SSCANF_T((*(pHSRScanner->m_aryFactor))[i].c_str() , _T("%lf %lf %lf") ,&(pt.x),&(pt.y),&(pt.z));
			m_pts.push_back(pt);
		}
		///m_nVertices = i;
		bRet = true;
		///bRet = (4 == i) ? true : false;
	}

	return bRet;
}

/**	
	@brief	The C3d_FaceElement::IsInVolume function

	@author	HumKyung.BAEK

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool C3d_FaceElement::IsInVolume(const CIsVolume& volume)
{
	m_vol.x[0] = m_vol.x[1] = m_pts[0].x;
	m_vol.y[0] = m_vol.y[1] = m_pts[0].y;
	m_vol.z[0] = m_vol.z[1] = m_pts[0].z;
	for(int i=0;i < int(m_pts.size());++i)
	{
		if(m_vol.x[0] > m_pts[i].x) m_vol.x[0] = m_pts[i].x;
		if(m_vol.x[1] < m_pts[i].x) m_vol.x[1] = m_pts[i].x;
		if(m_vol.y[0] > m_pts[i].y) m_vol.y[0] = m_pts[i].y;
		if(m_vol.y[1] < m_pts[i].y) m_vol.y[1] = m_pts[i].y;
		if(m_vol.z[0] > m_pts[i].z) m_vol.z[0] = m_pts[i].z;
		if(m_vol.z[1] < m_pts[i].z) m_vol.z[1] = m_pts[i].z;
	}

	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;

	return true;
}

/**	
	@brief	create faces for FACE element.

	@author	HumKyung.BAEK

	@param	pViewFmt	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@return	PFACE	
*/
CHSRFace* C3d_FaceElement::CreateFace(CHSRViewFormat* pViewFmt,long& nID)
{
	assert(pViewFmt && "pViewFmt is NULL");
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pViewFmt && pHSRApp)
	{
		vector<POINT_T> ptEDGE;

		for(int i = 0;i < int(m_pts.size());++i)
		{
			ptEDGE.push_back(pViewFmt->MODEL2VIEW(m_pts[i]));
		}
		if(pRet=CHSRFace::CreateFace(ptEDGE.size(),&(ptEDGE[0])))
		{
			pRet->id  = nID++;
			pRet->ele = NORMAL;
			if(_T("PIPE") == (*m_pstrLayer))
				pRet->m_enmLayer = LAYER_PIPE_OTHER;
			else	pRet->m_enmLayer = CHSRLayer::GetLayerCode(GetTypeString() , m_model , *m_pstrLayer);
			pRet->SetColor(*m_pstrColor);
			
			pHSRApp->AppendFace(NULL,pRet);
		}
	}

	return NULL;
}

/**
	@brief	

	@author	BHK

	@date	2009.111.02

	@param
	@param

	@return
*/
int C3d_FaceElement::CreateLineSegments()
{
	//! clear already created lines.
	{
		for(list<CLineElement*>::iterator itr = m_LineElementEntry.begin();itr != m_LineElementEntry.end();++itr)
		{
			delete (*itr);
		}
		m_LineElementEntry.clear();
	}

	const int nVertices = m_pts.size();
	for(int i = 0;i < nVertices;++i)
	{
		if(IsSamePoint(m_pts[(i % nVertices)] , m_pts[((i + 1) % nVertices)])) continue;
		CLineElement* pLineElm = new CLineElement;
		if(pLineElm)
		{
			pLineElm->m_pt[0] = m_pts[(i % nVertices)];
			pLineElm->m_pt[1] = m_pts[((i + 1) % nVertices)];
			pLineElm->SetLayerString((*m_pstrLayer));
			
			m_LineElementEntry.push_back(pLineElm);
		}
	}

	return ERROR_SUCCESS;
}