// FaceElement.cpp: implementation of the CFaceElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FaceElement.h"
#include "SmartHSR.h"
#include "LineElement.h"
#include "ArcElement.h"
#include "3d_FaceElement.h"
#include "EllipseElement.h"
#include "Tokenizer.h"

static const double nTOLER=0.0001;

IMPLEMENTS_HSR_ELEMENT(CFaceElement , CHSRElement , _T("FACE="));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static long logloop = 0L;

CFaceElement::CFaceElement() : CHSRElement()
{
	m_log = (++logloop);
}

CFaceElement::~CFaceElement()
{
	try
	{
		m_PointList.clear();
		m_oPointVisibleList.clear();
	}
	catch(...)
	{
	}
}

/**
	@brief	parse loop element texts

	@author	humkyung

	@date	????.??.??

	@param	pHSRScanner

	@return	true if success,otherwise false
*/
bool CFaceElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet = false;

	if(pHSRScanner)
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;
		
		pHSRScanner->ReadLine();
		if('{' != (*pHSRScanner->m_aryFactor)[0][0]) throw exception("expect '{'");

		m_PointList.clear();
		m_oPointVisibleList.clear();
		while(pHSRScanner->ReadLine())
		{
			if('}' == (*pHSRScanner->m_aryFactor)[0][0]) break;
			
			STRING_T::size_type at = (*pHSRScanner->m_pstrLine).find(_T("="));
			if(STRING_T::npos != at)
			{
				vector<STRING_T> oResult;
				CTokenizer<CIsFromString>::Tokenize(oResult , (*pHSRScanner->m_pstrLine) , CIsFromString(_T("=")));
				if((_T("COLOR") == oResult[0]) && (2 == oResult.size())) SetColorString(oResult[1]);
				continue;
			}
			
			/// read verteices
			if((3 == pHSRScanner->m_nFactor) || (4 == pHSRScanner->m_nFactor))
			{
				POINT_T pt;
				pt.x = ATOF_T((*pHSRScanner->m_aryFactor)[0].c_str());
				pt.y = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
				pt.z = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
				pt.reserved = 0;

				bool bVisible = true;
				if(4 == pHSRScanner->m_nFactor)
				{
					bVisible = (1 == ATOI_T((*pHSRScanner->m_aryFactor)[3].c_str())) ? true : false;
				}
				m_PointList.push_back(pt);
				m_oPointVisibleList.push_back(bVisible);
			}else	throw exception("unexpected item in CFaceElement");
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
	
	@author	HumKyung Baek

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CFaceElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;
}

/**	
	@brief	calculate voluem of FaceElement
	
	@author	HumKyung Baek

	@date	2009.12.01

	@param	

	@return	
*/
void CFaceElement::CalcVolume()
{
	if(!m_PointList.empty())
	{
		m_vol.x[0] = m_vol.x[1] = m_PointList[0].x;
		m_vol.y[0] = m_vol.y[1] = m_PointList[0].y;
		m_vol.z[0] = m_vol.z[1] = m_PointList[0].z;
		for(vector<POINT_T>::iterator itr = m_PointList.begin();itr != m_PointList.end();++itr)
		{
			POINT_T pt = (*itr);
			if(m_vol.x[0] > pt.x) m_vol.x[0] = pt.x;
			if(m_vol.x[1] < pt.x) m_vol.x[1] = pt.x;
			if(m_vol.y[0] > pt.y) m_vol.y[0] = pt.y;
			if(m_vol.y[1] < pt.y) m_vol.y[1] = pt.y;
			if(m_vol.z[0] > pt.z) m_vol.z[0] = pt.z;
			if(m_vol.z[1] < pt.z) m_vol.z[1] = pt.z;
		}
	}
}

/**	
	@brief	create a face for FaceElement
	
	@author	HumKyung Baek

	@param	pViewFormat - 
	@param	nID -

	@date	2010.12.01

	@return	CHSRFace	NULL.
*/	
CHSRFace* CFaceElement::CreateFace(CHSRViewFormat* pViewFormat , long& nID)
{
	assert(pViewFormat && "pViewFormat is NULL");
	CHSRFace* pRet=NULL;
	CHSRApp* pHSRApp=CHSRApp::GetInstance();

	bool bVisible[MAX_FACE_VERTICES]={false,};
	POINT_T ptEDGE[MAX_FACE_VERTICES];
	memset(ptEDGE , 0 , sizeof(POINT_T)*MAX_FACE_VERTICES);

	if(pViewFormat)
	{
		bool bAllInvisible = true;
		double zhigh=0.;
		int nCount = 0 , i = 0;
		for(i = 0;i < int(m_PointList.size());++i,++nCount)
		{
			if(0 == i)
				zhigh = m_PointList[i].z;
			else	zhigh = (m_PointList[i].z > zhigh) ? m_PointList[i].z : zhigh;
			
			ptEDGE[nCount]  = pViewFormat->MODEL2VIEW(m_PointList[i]);
			bVisible[nCount]= m_oPointVisibleList[i];
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
			ptEDGE[nCount]  = m_PointList[0];
			bVisible[nCount]= m_oPointVisibleList[0];
			///nCount++;
		}
		
		if((nCount >= 3) && (pRet = CHSRFace::CreateFace(nCount , ptEDGE)))
		{
			nCount = 0;
			for(PHSR_VERTEX ptr=pRet->pHead;ptr;ptr = ptr->next,nCount++)
			{
				if(false == bVisible[nCount])	ptr->render= INVALID;
				else				ptr->render=INTERSECTION;
			}
			
			pRet->type = OTHER;
			pRet->m_enmLayer = GetHSRLayer();
			pRet->id  = nID++;
			pRet->ele = NORMAL;
			pRet->SetColor(*m_pstrColor);
			pHSRApp->AppendFace(NULL,pRet);
		}
		else if((nCount >= 3))
		{ // vertical face
			const CIsVolume model_volume = pViewFormat->GetModelVolume();
			
			bool bCutted=false;
			PLANE_T plane={0,0,1,-model_volume.maxz()};
			if(CHSRViewFormat::TOP_VIEW_T == pViewFormat->GetViewDir())
			{
				bCutted = (m_vol.z[1] > model_volume.maxz());
				plane.a = 0;plane.b = 0;plane.c = 1;
				plane.d = -model_volume.maxz();
			}
			else if(CHSRViewFormat::BOTTOM_VIEW_T == pViewFormat->GetViewDir())
			{
				bCutted = (m_vol.z[0] < model_volume.minz());
				plane.a = 0;plane.b = 0;plane.c = 1;
				plane.d = -model_volume.minz();
			}
			else if(CHSRViewFormat::FRONT_VIEW_T == pViewFormat->GetViewDir())
			{
				bCutted = (m_vol.y[0] < model_volume.miny());
				plane.a = 0;plane.b = 1;plane.c = 0;
				plane.d = -model_volume.miny();
			}
			else if(CHSRViewFormat::BACK_VIEW_T == pViewFormat->GetViewDir())
			{
				bCutted = (m_vol.y[1] > model_volume.maxy());
				plane.a = 0;plane.b = 1;plane.c = 0;
				plane.d = -model_volume.maxy();
			}
			else if(CHSRViewFormat::LEFT_VIEW_T == pViewFormat->GetViewDir())
			{
				bCutted = (m_vol.x[0] < model_volume.minx());
				plane.a = 1;plane.b = 0;plane.c = 0;
				plane.d = -model_volume.minx();
			}
			else if(CHSRViewFormat::RIGHT_VIEW_T == pViewFormat->GetViewDir())
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
				const int nSize = m_PointList.size();
				for(i = 0;i < nSize;++i)
				{
					line.ptStart = m_PointList[i % nSize];
					line.ptEnd   = m_PointList[(i + 1) % nSize];
					if(CGeometry::IntersectLineWithPlane(&ptIntersect,line,plane))
					{
						ptEDGE[nCount] = pViewFormat->MODEL2VIEW(ptIntersect);
						nCount++;
					}
				}
				
				nCount = int((nCount*0.5)*2);
				for(i = 0;i < nCount;i+=2)
				{
					PSQPRIMITIVE pPrimitive=pHSRApp->alloc_primitive();
					if(pPrimitive)
					{
						double nMaxX=pViewFormat->GetViewOrigin().x + pViewFormat->GetViewWidth()*0.5;
						double nMinX=pViewFormat->GetViewOrigin().x - pViewFormat->GetViewWidth()*0.5;
						double nMaxY=pViewFormat->GetViewOrigin().y + pViewFormat->GetViewHeight()*0.5;
						double nMinY=pViewFormat->GetViewOrigin().y - pViewFormat->GetViewHeight()*0.5;
						
						pPrimitive->hdr.id = ENM_LINE;
						pPrimitive->body.line.ptStart = ptEDGE[i];
						pPrimitive->body.line.ptEnd   = ptEDGE[i+1];
						
						pPrimitive->body.line.ptStart.x = (pPrimitive->body.line.ptStart.x > nMaxX) ? nMaxX : pPrimitive->body.line.ptStart.x;
						pPrimitive->body.line.ptStart.x = (pPrimitive->body.line.ptStart.x < nMinX) ? nMinX : pPrimitive->body.line.ptStart.x;
						pPrimitive->body.line.ptStart.y = (pPrimitive->body.line.ptStart.y > nMaxY) ? nMaxY : pPrimitive->body.line.ptStart.y;
						pPrimitive->body.line.ptStart.y = (pPrimitive->body.line.ptStart.y < nMinY) ? nMinY : pPrimitive->body.line.ptStart.y;
						pPrimitive->body.line.ptEnd.x   = (pPrimitive->body.line.ptEnd.x > nMaxX) ? nMaxX : pPrimitive->body.line.ptEnd.x;
						pPrimitive->body.line.ptEnd.x   = (pPrimitive->body.line.ptEnd.x < nMinX) ? nMinX : pPrimitive->body.line.ptEnd.x;
						pPrimitive->body.line.ptEnd.y   = (pPrimitive->body.line.ptEnd.y > nMaxY) ? nMaxY : pPrimitive->body.line.ptEnd.y;
						pPrimitive->body.line.ptEnd.y   = (pPrimitive->body.line.ptEnd.y < nMinY) ? nMinY : pPrimitive->body.line.ptEnd.y;
						pPrimitive->hdr.display.lcolor = (BYTE)(ATOI_T(m_pstrColor->c_str()));
						
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
