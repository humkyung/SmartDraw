// PreHNormalFace.cpp: implementation of the CPreHNormalFace class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PreHNormalFace.h"
#include "SmartHSR.h"

static const double nTOLER=0.0001;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	
	@brief
*/
CPreHNormalFace::CPreHNormalFace(const long& nGroup)
{
	m_nGroup = nGroup;
	m_nLINEs = 0;
	m_paryHLINE[0] = NULL;
}

CPreHNormalFace::~CPreHNormalFace()
{

}

/**	
	@brief	insert a hline into hface.

	@author	BHK

	@date	?

	@param	pPreHLINEFormat	a parameter of type CHLineElement*

	@remarks\n
	;2003-10-24 - fix a routine checking how two lines are connected,
	
	@return	bool CPreHNormalFace::operator	
*/
bool CPreHNormalFace::operator +=(CHLineElement* pPreHLINEFormat)
{
	assert(pPreHLINEFormat && "pPreHLINEFormat is NULL");
	assert((m_nLINEs < MAX_FACE_HLINES) && "number of lines is larger than MAXHLINES");
	bool bRet=false;

	if(pPreHLINEFormat && (m_nLINEs < MAX_FACE_HLINES))
	{
		m_model = pPreHLINEFormat->m_model;

		if(m_nLINEs)
		{
			POINT_T ptStart[2]={0,},ptEnd[2]={0,};
			ptStart[0] = m_paryHLINE[m_nLINEs - 1]->m_pt[0];
			ptEnd[0] = m_paryHLINE[m_nLINEs - 1]->m_pt[1];
			ptStart[1] = pPreHLINEFormat->m_pt[0];
			ptEnd[1] = pPreHLINEFormat->m_pt[1];
			
			double dx[4]={0.,},dy[4]={0.,},dz[4]={0.,};
			dx[0] = ptEnd[0].x - ptStart[1].x;dy[0] = ptEnd[0].y - ptStart[1].y;dz[0] = ptEnd[0].z - ptStart[1].z;
			dx[1] = ptEnd[0].x - ptEnd[1].x;dy[1] = ptEnd[0].y - ptEnd[1].y;dz[1] = ptEnd[0].z - ptEnd[1].z;
			dx[2] = ptStart[0].x - ptStart[1].x;dy[2] = ptStart[0].y - ptStart[1].y;dz[2] = ptStart[0].z - ptStart[1].z;
			dx[3] = ptStart[0].x - ptEnd[1].x;dy[3] = ptStart[0].y - ptEnd[1].y;dz[3] = ptStart[0].z - ptEnd[1].z;
			
			if((fabs(dx[1]) < nTOLER) && (fabs(dy[1]) < nTOLER) && (fabs(dz[1]) < nTOLER))
			{
				swap(pPreHLINEFormat->m_pt[0],pPreHLINEFormat->m_pt[1]);
			}
			else if((fabs(dx[2]) < nTOLER) && (fabs(dy[2]) < nTOLER) && (fabs(dz[2]) < nTOLER))
			{
				swap(m_paryHLINE[m_nLINEs-1]->m_pt[0],m_paryHLINE[m_nLINEs-1]->m_pt[1]);
			}
			else if((fabs(dx[3]) < nTOLER) && (fabs(dy[3]) < nTOLER) && (fabs(dz[3]) < nTOLER))
			{
				swap(m_paryHLINE[m_nLINEs-1]->m_pt[0],m_paryHLINE[m_nLINEs-1]->m_pt[1]);
				swap(pPreHLINEFormat->m_pt[0],pPreHLINEFormat->m_pt[1]);
			}
		}else	m_strLayer = pPreHLINEFormat->GetLayerString();

		const double dx = pPreHLINEFormat->m_pt[1].x - pPreHLINEFormat->m_pt[0].x;
		const double dy = pPreHLINEFormat->m_pt[1].y - pPreHLINEFormat->m_pt[0].y;
		const double dz = pPreHLINEFormat->m_pt[1].z - pPreHLINEFormat->m_pt[0].z;
		if((fabs(dx) < nTOLER) && (fabs(dy) < nTOLER) && (fabs(dz) < nTOLER))
		{
		}
		else
		{
			m_paryHLINE[m_nLINEs++] = pPreHLINEFormat;
			bRet = true;
		}
	}
	
	return bRet;
}

/**	
	@brief	The CPreHNormalFace::Layer function

	@return	const char*	
*/
const TCHAR* CPreHNormalFace::Layer()
{
	return m_strLayer.c_str();
}

/**	
	@brief	The CPreHNormalFace::CalcVolume function

	@return	void	
*/
void CPreHNormalFace::CalcVolume()
{
	m_vol.x[0] = m_vol.x[1] = m_paryHLINE[0]->m_pt[0].x;
	m_vol.y[0] = m_vol.y[1] = m_paryHLINE[0]->m_pt[0].y;
	m_vol.z[0] = m_vol.z[1] = m_paryHLINE[0]->m_pt[0].z;
	for(int i=1;i < m_nLINEs;i++)
	{
		if(m_vol.x[0] > m_paryHLINE[i]->m_pt[0].x) m_vol.x[0] = m_paryHLINE[i]->m_pt[0].x;
		if(m_vol.x[1] < m_paryHLINE[i]->m_pt[0].x) m_vol.x[1] = m_paryHLINE[i]->m_pt[0].x;
		if(m_vol.y[0] > m_paryHLINE[i]->m_pt[0].y) m_vol.y[0] = m_paryHLINE[i]->m_pt[0].y;
		if(m_vol.y[1] < m_paryHLINE[i]->m_pt[0].y) m_vol.y[1] = m_paryHLINE[i]->m_pt[0].y;
		if(m_vol.z[0] > m_paryHLINE[i]->m_pt[0].z) m_vol.z[0] = m_paryHLINE[i]->m_pt[0].z;
		if(m_vol.z[1] < m_paryHLINE[i]->m_pt[0].z) m_vol.z[1] = m_paryHLINE[i]->m_pt[0].z;
	}
}

/**	
	@brief	check this face is in volume.

	@param	volume	a parameter of type const SQVOLUME

	@remarks\n
	;2003.05.16 - first calculate bouding box of face and compare bounding box and volume.

	@return	bool	
*/
bool CPreHNormalFace::IsInVolume(const CIsVolume& volume)
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
	@brief	The CPreHNormalFace::CreateFace function

	@author	BHK

	@date	?

	@param	pVIEWFormat	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@return	PFACE	
*/
CHSRFace* CPreHNormalFace::CreateFace(CHSRViewFormat* pVIEWFormat,long& nID)
{
	assert(pVIEWFormat && "pVIEWFormat is NULL");
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pVIEWFormat)
	{
		bool bVisible[MAX_FACE_HLINES]={false,};
		POINT_T ptEDGE[MAX_FACE_HLINES];
		
		bool bAllInvisible = true;
		double zhigh=0.;
		int nCount=0 , i = 0;
		for(i = 0;i < m_nLINEs;++i,++nCount)
		{
			if(0 == i)
				zhigh = m_paryHLINE[i]->m_pt[0].z;
			else	zhigh = (m_paryHLINE[i]->m_pt[0].z > zhigh) ? m_paryHLINE[i]->m_pt[0].z : zhigh;
			
			ptEDGE[nCount]  = pVIEWFormat->MODEL2VIEW(m_paryHLINE[i]->m_pt[0]);
			bVisible[nCount]= m_paryHLINE[i]->m_bVisible;
			if(true == bVisible[nCount]) bAllInvisible = false;
		}
		if(true == bAllInvisible) return NULL;

		//! match end of face and start of face.
		const double dx = ptEDGE[nCount - 1].x - ptEDGE[0].x;
		const double dy = ptEDGE[nCount - 1].y - ptEDGE[0].y;
		const double dz = ptEDGE[nCount - 1].z - ptEDGE[0].z;
		if((fabs(dx) < nTOLER) && (fabs(dy) < nTOLER) && (fabs(dz) < nTOLER))
		{
		}
		else
		{
			ptEDGE[nCount]  = pVIEWFormat->MODEL2VIEW(m_paryHLINE[0]->m_pt[0]);
			bVisible[nCount]= m_paryHLINE[0]->m_bVisible;
			nCount++;
		}
				
		if((nCount > 3) && (pRet = CHSRFace::CreateFace(nCount,ptEDGE)))
		{
			nCount = 0;
			for(PHSR_VERTEX ptr=pRet->pHead;ptr;ptr = ptr->next,nCount++)
			{
				if(false == bVisible[nCount])	ptr->render= INVALID;
				else				ptr->render=INTERSECTION;
			}
			
			pRet->m_enmLayer = GetHSRLayer();
			pRet->id  = nID++;
			pRet->ele = NORMAL;
			//pRet->SetColor(*m_pstrColor);
			pHSRApp->AppendFace(NULL,pRet);
		}
		else if((i >= 3))
		{ // vertical face
			const CIsVolume model_volume=pVIEWFormat->GetModelVolume();

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
					line.ptStart = m_paryHLINE[i]->m_pt[0];
					line.ptEnd   = m_paryHLINE[i]->m_pt[1];
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

						if(_T("PIPE") == m_strLayer)
						{
							if(pHSRApp->m_pExporter) pHSRApp->m_pExporter->Add(LAYER_PIPE_OTHER,SHOW,pPrimitive);
						}
						else if(_T("EQP") == m_strLayer)
						{
							if(pHSRApp->m_pExporter) pHSRApp->m_pExporter->Add(LAYER_EQUI,SHOW,pPrimitive);
						}
						else if(_T("STRU") == m_strLayer)
						{
							if(pHSRApp->m_pExporter) pHSRApp->m_pExporter->Add(LAYER_STRU,SHOW,pPrimitive);
						}
						else if(_T("CABLE") == m_strLayer)
						{
							if(pHSRApp->m_pExporter) pHSRApp->m_pExporter->Add(LAYER_CABLE,SHOW,pPrimitive);
						}
						else
						{
							if(pHSRApp->m_pExporter) pHSRApp->m_pExporter->Add(LAYER_OTHER,SHOW,pPrimitive);
						}
					}
				}
			}
		}
	}
		
	return NULL;
}

void CPreHNormalFace::Init(const long nGroup)
{
	m_nGroup = nGroup;
	m_nLINEs = 0L;
}

/**
 * GetHSRLayer:
 *
 * @return HSR_LAYER_E 
 */
HSR_LAYER_E CPreHNormalFace::GetHSRLayer()
{
	if(_T("PIPE") == m_strLayer)
		return LAYER_PIPE_OTHER;
	else if(_T("EQP") == m_strLayer)
		return LAYER_EQUI;
	else if(_T("STRU") == m_strLayer)
		return LAYER_STRU;
	else if(_T("CABLE") == m_strLayer)
		return LAYER_CABLE;
	else	return LAYER_OTHER;
}
