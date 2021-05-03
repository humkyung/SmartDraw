// PreNormalFace.cpp: implementation of the CHSRNormalFace class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <graphics\geometry.h>
#include "HSRNormalFace.h"
#include "SmartHSR.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	
	@brief
*/
CHSRNormalFace::CHSRNormalFace(long nGroup)
{
	(*m_pstrLayer) = _T("OTHER");
	m_nGroup = nGroup;
	memset(m_ptEDGE,0,sizeof(POINT_T)*(MAX_FACE_LINES + 1));
	
	m_vol.x[0] = m_vol.x[1] = 0.;
	m_vol.y[0] = m_vol.y[1] = 0.;
	m_vol.z[0] = m_vol.z[1] = 0.;

	m_nLINEs = 0;
}

CHSRNormalFace::~CHSRNormalFace()
{

}

/******************************************************************************
    @author     humkyung
    @date       2012-03-06
    @class      CHSRNormalFace
    @function   GetLineCount
    @return     int
    @brief
******************************************************************************/
int CHSRNormalFace::GetLineCount() const
{
	return m_nLINEs;
}

/**	
	@brief	insert a line into face.

	@param	pLineElm	a parameter of type CLineElement*

	@author	BHK

	@remarks\n
	;2003-10-24 - fix a routine checking how two lines are connected,

	@return	bool CHSRNormalFace::operator	
*/
bool CHSRNormalFace::operator +=(CLineElement* pLineElm)
{
	assert(pLineElm && "pPreLINEFormat is NULL");
	assert((m_nLINEs < MAX_FACE_LINES) && "number of lines is larger than MAXLINES");
	const double nTOLER=0.0001;
	bool bRet=false;

	if(pLineElm && (m_nLINEs < MAX_FACE_LINES))
	{
		m_model = pLineElm->m_model;

		if(m_nLINEs)
		{
			POINT_T ptStart[2]={0,},ptEnd[2]={0,};
			ptStart[0] = m_paryLINE[m_nLINEs - 1]->m_pt[0];
			ptEnd[0] = m_paryLINE[m_nLINEs - 1]->m_pt[1];
			ptStart[1] = pLineElm->m_pt[0];
			ptEnd[1] = pLineElm->m_pt[1];

			double dx[4]={0.,},dy[4]={0.,},dz[4]={0.,};
			dx[0] = ptEnd[0].x - ptStart[1].x;
			dy[0] = ptEnd[0].y - ptStart[1].y;
			dz[0] = ptEnd[0].z - ptStart[1].z;
			
			dx[1] = ptEnd[0].x - ptEnd[1].x;
			dy[1] = ptEnd[0].y - ptEnd[1].y;
			dz[1] = ptEnd[0].z - ptEnd[1].z;
			
			dx[2] = ptStart[0].x - ptStart[1].x;
			dy[2] = ptStart[0].y - ptStart[1].y;
			dz[2] = ptStart[0].z - ptStart[1].z;
			
			dx[3] = ptStart[0].x - ptEnd[1].x;
			dy[3] = ptStart[0].y - ptEnd[1].y;
			dz[3] = ptStart[0].z - ptEnd[1].z;
			
			if((fabs(dx[1]) < nTOLER) && (fabs(dy[1]) < nTOLER) && (fabs(dz[1]) < nTOLER))
			{
				swap(pLineElm->m_pt[0],pLineElm->m_pt[1]);
			}
			else if((fabs(dx[2]) < nTOLER) && (fabs(dy[2]) < nTOLER) && (fabs(dz[2]) < nTOLER))
			{
				swap(m_paryLINE[m_nLINEs-1]->m_pt[0],m_paryLINE[m_nLINEs-1]->m_pt[1]);
			}
			else if((fabs(dx[3]) < nTOLER) && (fabs(dy[3]) < nTOLER) && (fabs(dz[3]) < nTOLER))
			{
				swap(m_paryLINE[m_nLINEs-1]->m_pt[0],m_paryLINE[m_nLINEs-1]->m_pt[1]);
				swap(pLineElm->m_pt[0],pLineElm->m_pt[1]);
			}
		}else	SetLayerString(pLineElm->GetLayerString());

		const double dx=pLineElm->m_pt[1].x - pLineElm->m_pt[0].x;
		const double dy=pLineElm->m_pt[1].y - pLineElm->m_pt[0].y;
		const double dz=pLineElm->m_pt[1].z - pLineElm->m_pt[0].z;
		// skip shortest line
		if((fabs(dx) < nTOLER) && (fabs(dy) < nTOLER) && (fabs(dz) < nTOLER))
		{
		}
		else
		{
			m_paryLINE[m_nLINEs++] = pLineElm;
			bRet = true;
		}
	}
	
	return bRet;
}

/**
	@brief	

	@author	BHK

	@param pArcElm 
	
	@return bool 
 */
bool CHSRNormalFace::operator +=(CArcElement* pArcElm)
{
	assert(pArcElm && "pArcElm is NULL");
	const double nTOLER=0.0001;
	bool bRet = false;

	if(pArcElm)
	{
		m_model = pArcElm->m_model;

		pArcElm->CreateLineElements();

		for(list<CLineElement*>::iterator itr = pArcElm->m_LineElementEntry.begin();itr != pArcElm->m_LineElementEntry.end();++itr)
		{
			(*this).operator +=(*itr);
		}
		bRet = true;
	}
	
	return bRet;
}

/**	
	@brief	The CHSRNormalFace::Layer function

	@author	humkyung

	@param	m_strLayer.c_str(	a parameter of type ){return

	@return	const char*	
*/
LPCTSTR CHSRNormalFace::Layer()
{
	return m_pstrLayer->c_str();
}

/**	
	@brief	The CHSRNormalFace::Volume function

	@author	BHK
	
	\return	void	
*/
void CHSRNormalFace::CalcVolume()
{
	m_vol.x[0] = m_vol.x[1] = m_paryLINE[0]->m_pt[0].x;
	m_vol.y[0] = m_vol.y[1] = m_paryLINE[0]->m_pt[0].y;
	m_vol.z[0] = m_vol.z[1] = m_paryLINE[0]->m_pt[0].z;
	for(int i=1;i < m_nLINEs;i++)
	{
		if(m_vol.x[0] > m_paryLINE[i]->m_pt[0].x) m_vol.x[0] = m_paryLINE[i]->m_pt[0].x;
		if(m_vol.x[1] < m_paryLINE[i]->m_pt[0].x) m_vol.x[1] = m_paryLINE[i]->m_pt[0].x;
		if(m_vol.y[0] > m_paryLINE[i]->m_pt[0].y) m_vol.y[0] = m_paryLINE[i]->m_pt[0].y;
		if(m_vol.y[1] < m_paryLINE[i]->m_pt[0].y) m_vol.y[1] = m_paryLINE[i]->m_pt[0].y;
		if(m_vol.z[0] > m_paryLINE[i]->m_pt[0].z) m_vol.z[0] = m_paryLINE[i]->m_pt[0].z;
		if(m_vol.z[1] < m_paryLINE[i]->m_pt[0].z) m_vol.z[1] = m_paryLINE[i]->m_pt[0].z;
	}
}

/**	
	@brief	The CHSRNormalFace::IsInVolume function

	@author	humkyung

	@param	volume	a parameter of type const VOLUME_T

	@return	bool
*/
bool CHSRNormalFace::IsInVolume(const CIsVolume& volume)
{
	if(m_nLINEs > 0)
	{
		CalcVolume();
		
		if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
			(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
			(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
		
		return true;
	}
	return false;
}

/**	
	@brief	The CHSRNormalFace::CreateFace function

	@author	humkyung

	@param	pVIEWFormat	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@remarks\n
	;2003.01.20 - add a routine which treats vertical face\n
	;2003.03.25 - clipping vertical line by view area\n
	;2003.07.11 - if number of line is greater than 3 or equal to 3, create this face.\n
	;2004.04.29 - assign layer type to face
	;2005.12.22 - assign 'CABLE' layer.

	@return	PFACE	
*/
CHSRFace* CHSRNormalFace::CreateFace(CHSRViewFormat* pVIEWFormat,long& nID)
{
	assert(pVIEWFormat && "pVIEWFormat is NULL");
	CHSRFace* pRet=NULL;
	
	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pVIEWFormat)
	{
		int i = 0;
        bool bVisible[MAX_FACE_LINES]={false,};
        bool bAllInvisible = true;
		double zhigh=0.;
		for(i=0;i < m_nLINEs;i++)
		{
			if(0 == i)
				zhigh = m_paryLINE[i]->m_pt[0].z;
			else	zhigh = (m_paryLINE[i]->m_pt[0].z > zhigh) ? m_paryLINE[i]->m_pt[0].z : zhigh;
			m_ptEDGE[i] = pVIEWFormat->MODEL2VIEW(m_paryLINE[i]->m_pt[0]);
            bVisible[i]= m_paryLINE[i]->m_bVisible;
			if(true == bVisible[i]) bAllInvisible = false;
		}
        if(true == bAllInvisible) return NULL;
		m_ptEDGE[i] = m_ptEDGE[0];
		
		if((i >= 3) && (pRet=CHSRFace::CreateFace(i,m_ptEDGE)))
		{
            i = 0;
            for(PHSR_VERTEX ptr=pRet->pHead;ptr;ptr = ptr->next,++i)
			{
				if(false == bVisible[i])	ptr->render= INVALID;
				else				ptr->render=INTERSECTION;
			}

			pRet->type = OTHER;
			pRet->m_enmLayer = GetHSRLayer();
			pRet->id  = nID++;
			pRet->ele = NORMAL;
			pRet->SetColor(*m_pstrColor);
			pHSRApp->AppendFace(NULL,pRet);
		}
		else if((i >= 3))
		{ /// vertical face
			const CIsVolume model_volume = pVIEWFormat->GetModelVolume();

			bool bCutted=false;
			PLANE_T plane={0,0,1,-model_volume.maxz()};
			if(CHSRViewFormat::TOP_VIEW_T == pVIEWFormat->GetViewDir())
			{
				bCutted = (m_vol.z[1] > model_volume.maxz());
				plane.a = 0;plane.b = 0;plane.c = 1;
				plane.d = -model_volume.maxz();
			}
			else if(CHSRViewFormat::BOTTOM_VIEW_T == pVIEWFormat->GetViewDir())
			{
				bCutted = (m_vol.z[0] < model_volume.minz());
				plane.a = 0;plane.b = 0;plane.c = 1;
				plane.d = -model_volume.minz();
			}
			else if(CHSRViewFormat::FRONT_VIEW_T == pVIEWFormat->GetViewDir())
			{
				bCutted = (m_vol.y[0] < model_volume.miny());
				plane.a = 0;plane.b = 1;plane.c = 0;
				plane.d = -model_volume.miny();
			}
			else if(CHSRViewFormat::BACK_VIEW_T == pVIEWFormat->GetViewDir())
			{
				bCutted = (m_vol.y[1] > model_volume.maxy());
				plane.a = 0;plane.b = 1;plane.c = 0;
				plane.d = -model_volume.maxy();
			}
			else if(CHSRViewFormat::LEFT_VIEW_T == pVIEWFormat->GetViewDir())
			{
				bCutted = (m_vol.x[0] < model_volume.minx());
				plane.a = 1;plane.b = 0;plane.c = 0;
				plane.d = -model_volume.minx();
			}
			else if(CHSRViewFormat::RIGHT_VIEW_T == pVIEWFormat->GetViewDir())
			{
				bCutted = (m_vol.x[1] > model_volume.maxx());
				plane.a = 1;plane.b = 0;plane.c = 0;
				plane.d = -model_volume.maxx();
			}

			if(bCutted)
			{
				POINT_T ptIntersect={0,},ptTmp={0,};
				int nCount=0;
				
				LINE_T  line={0};
				for(i = 0;i < m_nLINEs;i++)
				{
					line.ptStart = m_paryLINE[i]->m_pt[0];
					line.ptEnd   = m_paryLINE[i]->m_pt[1];
					if(CGeometry::IntersectLineWithPlane(&ptIntersect,line,plane))
					{
						m_ptEDGE[nCount] = pVIEWFormat->MODEL2VIEW(ptIntersect);
						nCount++;
					}
				}
				
				nCount = int((nCount*0.5)*2);
				for(i = 0;i < nCount;i+=2)
				{
					PSQPRIMITIVE pPrimitive=pHSRApp->alloc_primitive();
					if(pPrimitive)
					{
						double nMaxX=pVIEWFormat->GetViewOrigin().x + pVIEWFormat->GetViewWidth()*0.5;
						double nMinX=pVIEWFormat->GetViewOrigin().x - pVIEWFormat->GetViewWidth()*0.5;
						double nMaxY=pVIEWFormat->GetViewOrigin().y + pVIEWFormat->GetViewHeight()*0.5;
						double nMinY=pVIEWFormat->GetViewOrigin().y - pVIEWFormat->GetViewHeight()*0.5;

						pPrimitive->hdr.id = ENM_LINE;
						pPrimitive->body.line.ptStart = m_ptEDGE[i];
						pPrimitive->body.line.ptEnd   = m_ptEDGE[i+1];
						
						pPrimitive->body.line.ptStart.x = (pPrimitive->body.line.ptStart.x > nMaxX) ? nMaxX : pPrimitive->body.line.ptStart.x;
						pPrimitive->body.line.ptStart.x = (pPrimitive->body.line.ptStart.x < nMinX) ? nMinX : pPrimitive->body.line.ptStart.x;
						pPrimitive->body.line.ptStart.y = (pPrimitive->body.line.ptStart.y > nMaxY) ? nMaxY : pPrimitive->body.line.ptStart.y;
						pPrimitive->body.line.ptStart.y = (pPrimitive->body.line.ptStart.y < nMinY) ? nMinY : pPrimitive->body.line.ptStart.y;
						pPrimitive->body.line.ptEnd.x   = (pPrimitive->body.line.ptEnd.x > nMaxX) ? nMaxX : pPrimitive->body.line.ptEnd.x;
						pPrimitive->body.line.ptEnd.x   = (pPrimitive->body.line.ptEnd.x < nMinX) ? nMinX : pPrimitive->body.line.ptEnd.x;
						pPrimitive->body.line.ptEnd.y   = (pPrimitive->body.line.ptEnd.y > nMaxY) ? nMaxY : pPrimitive->body.line.ptEnd.y;
						pPrimitive->body.line.ptEnd.y   = (pPrimitive->body.line.ptEnd.y < nMinY) ? nMinY : pPrimitive->body.line.ptEnd.y;
						pPrimitive->hdr.display.lcolor = (BYTE)ATOI_T(m_pstrColor->c_str());

						if(_T("PIPE") == (*m_pstrLayer))
						{
							if(pHSRApp->m_pExporter) pHSRApp->m_pExporter->Add(LAYER_PIPE_OTHER,SHOW,pPrimitive);
						}
						else if(_T("EQP") == (*m_pstrLayer))
						{
							if(pHSRApp->m_pExporter) pHSRApp->m_pExporter->Add(LAYER_EQUI,SHOW,pPrimitive);
						}
						else if(_T("STRU") == (*m_pstrLayer))
						{
							if(pHSRApp->m_pExporter) pHSRApp->m_pExporter->Add(LAYER_STRU,SHOW,pPrimitive);
						}
						else if(_T("CABLE") == (*m_pstrLayer))
						{
							if(pHSRApp->m_pExporter) pHSRApp->m_pExporter->Add(LAYER_CABLE,SHOW,pPrimitive);
						}
						else if(pHSRApp->m_pExporter) pHSRApp->m_pExporter->Add(LAYER_OTHER,SHOW,pPrimitive);
					}
				}
			}
		}
	}

	return NULL;
}

/**
	@brief	initialize variables.

	@author	BHK
*/
void CHSRNormalFace::Init(const long nGroup)
{
	m_nGroup = nGroup;
	m_nLINEs = 0;
}

/**
 * GetHSRLayer:
 *
 * @return HSR_LAYER_E 
 */
HSR_LAYER_E CHSRNormalFace::GetHSRLayer()
{
	if(_T("PIPE") == (*m_pstrLayer))
	{
		return LAYER_PIPE_OTHER;
	}
	else if(_T("EQP") == (*m_pstrLayer))
	{
		return LAYER_EQUI;
	}
	else if(_T("STRU") == (*m_pstrLayer))
	{
		return LAYER_STRU;
	}
	else if(_T("CABLE") == (*m_pstrLayer))
	{
		return LAYER_CABLE;
	}
	else	return LAYER_OTHER;
}

/**
	@brief	get point set of shape

	@author	humkyung

	@param	pts
	@param	zhigh

	@return	
*/
int CHSRNormalFace::GetShapePointSet(vector<POINT_T>& pts , double& zhigh)
{
	zhigh = 0.f;
	for(int i = 0;i < m_nLINEs;++i)///vector<CLineElement*>::iterator itr = m_LineElmRefEntry.begin();itr != m_LineElmRefEntry.end();++itr)
	{
		if(0 == i)///if(m_LineElmRefEntry.begin() == itr)
			zhigh = m_paryLINE[i]->m_pt[0].z;
		else	zhigh = (m_paryLINE[i]->m_pt[0].z > zhigh) ? m_paryLINE[i]->m_pt[0].z : zhigh;
		pts.push_back(m_paryLINE[i]->m_pt[0]);
	}
	if(m_nLINEs > 1) pts.push_back(m_paryLINE[m_nLINEs - 1]->m_pt[1]);

	return pts.size();
}

/*
	@brief	

	@author	humkyung

	@return	CLineElement*
*/
CLineElement* CHSRNormalFace::GetLastLineElement()
{
	if(m_nLINEs > 0) return m_paryLINE[m_nLINEs - 1];
	return NULL;
}