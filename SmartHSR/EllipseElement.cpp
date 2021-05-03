// EllipseElement.cpp: implementation of the CEllipseElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include "SmartHSR.h"
#include "EllipseElement.h"
#include "LineElement.h"

IMPLEMENTS_HSR_ELEMENT(CEllipseElement , CHSRElement , _T("ELLIPSE"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEllipseElement::CEllipseElement()
{
	memset(m_matrix , 0 , sizeof(double)*9);
}

CEllipseElement::~CEllipseElement()
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

#ifdef VER_03
bool CEllipseElement::ParseLine(CHSRScanner *pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet = false;

	if(pHSRScanner)
	{
	}

	return bRet;
}
#else
bool CEllipseElement::ParseLine(CHSRScanner *pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet = false;

	if(pHSRScanner && (pHSRScanner->m_nFactor > 11))
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;
		m_nGroup = ATOI_T((*pHSRScanner->m_aryFactor)[1].c_str());
		
		m_ellipse3d.ptOrigin.x  = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
		m_ellipse3d.ptOrigin.y  = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
		m_ellipse3d.ptOrigin.z  = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
		m_ellipse3d.nAxis[0]    = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
		m_ellipse3d.nAxis[1]    = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
		m_ellipse3d.nStartAngle = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());
		m_ellipse3d.nSweepAngle = ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());
		m_ellipse3d.vecNorm.dx  = ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str());
		m_ellipse3d.vecNorm.dy  = ATOF_T((*pHSRScanner->m_aryFactor)[10].c_str());
		m_ellipse3d.vecNorm.dz  = ATOF_T((*pHSRScanner->m_aryFactor)[11].c_str());

		m_matrix[0][0] = 1;m_matrix[0][1] = 0;m_matrix[0][2] = 0;
		m_matrix[1][0] = 0;m_matrix[1][1] = 1;m_matrix[1][2] = 0;
		m_matrix[2][0] = 0;m_matrix[2][1] = 0;m_matrix[2][2] = 1;
		if(pHSRScanner->m_nFactor > 21)
		{
			m_matrix[0][0] = ATOF_T((*pHSRScanner->m_aryFactor)[12].c_str());
			m_matrix[0][1] = ATOF_T((*pHSRScanner->m_aryFactor)[13].c_str());
			m_matrix[0][2] = ATOF_T((*pHSRScanner->m_aryFactor)[14].c_str());
			m_matrix[1][0] = ATOF_T((*pHSRScanner->m_aryFactor)[15].c_str());
			m_matrix[1][1] = ATOF_T((*pHSRScanner->m_aryFactor)[16].c_str());
			m_matrix[1][2] = ATOF_T((*pHSRScanner->m_aryFactor)[17].c_str());
			m_matrix[2][0] = ATOF_T((*pHSRScanner->m_aryFactor)[18].c_str());
			m_matrix[2][1] = ATOF_T((*pHSRScanner->m_aryFactor)[19].c_str());
			m_matrix[2][2] = ATOF_T((*pHSRScanner->m_aryFactor)[20].c_str());

			SetColorString((*pHSRScanner->m_aryFactor)[21]);
		}
		else if(pHSRScanner->m_nFactor > 12)
		{
			SetColorString((*pHSRScanner->m_aryFactor)[12]);
		}

		m_vol.x[LOW] = m_vol.x[HIGH] = m_ellipse3d.ptOrigin.x;
		m_vol.y[LOW] = m_vol.y[HIGH] = m_ellipse3d.ptOrigin.y;
		m_vol.z[LOW] = m_vol.z[HIGH] = m_ellipse3d.ptOrigin.z;
				
		bRet = true;
	}

	return bRet;
}
#endif

/**
	@brief	check whether eliipse is in volume or not.
 
	@author	BHK

	@date	2009.11.03
 
	@param	volume

	@return	
 */
bool CEllipseElement::IsInVolume(const CIsVolume& volume)
{
	if(m_vol.x[HIGH] < volume.minx()) return false;
	if(m_vol.x[LOW]  > volume.maxx()) return false;
	if(m_vol.y[HIGH] < volume.miny()) return false;
	if(m_vol.y[LOW]  > volume.maxy()) return false;
	if(m_vol.z[HIGH] < volume.minz()) return false;
	if(m_vol.z[LOW]  > volume.maxz()) return false;

	return true;
}

/**
	@brief	CreateFace:
	@author	humkyung
	@date	?
	@param	
	@return	CHSRFace*
 */
CHSRFace* CEllipseElement::CreateFace(CHSRViewFormat* pViewFormat,long& nID)
{
	assert(pViewFormat && "pViewFormat is NULL");
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pViewFormat)
	{
		static POINT_T pts[32];
		
		const int nCount = ConstructVertexList(pts);
		for(int i = 0;i < nCount;++i) pts[i] = pViewFormat->MODEL2VIEW(pts[i]);
		if(IsSamePoint(pts[0] , pts[nCount - 1]))
		{
			CHSRFace* pFace = CHSRFace::CreateFace(nCount , pts);
			if(pFace)
			{
				pFace->id  = nID++;
				pFace->m_enmLayer = GetHSRLayer();
				pFace->type= BOX;
				pFace->ele = HSR_RECTANGLE_SHAPE;
				pFace->SetColor(*m_pstrColor);
				
				pHSRApp->AppendFace(NULL,pFace);
			}
			else
			{
				POINT_T anchor[2] = {0,};
				if(ERROR_SUCCESS == FindLinePassingDataSet(anchor , nCount , pts))
				{
					double dx = anchor[1].x - anchor[0].x;
					double dy = anchor[1].y - anchor[0].y;
					const double length = sqrt(dx*dx + dy*dy);
					if(length > 0.f)
					{
						//! give a thickness virtually.
						dx /= length; dy /= length;
						double temp = -dy;
						dy = dx;
						dx = temp;
						pts[0] = anchor[0];
						pts[0].z = (anchor[0].z + anchor[1].z)*0.5;
						pts[1] = anchor[1];
						pts[1].z = pts[0].z;
						pts[2] = pts[1];
						pts[2].x += dx*0.0001;
						pts[2].y += dy*0.0001;
						pts[3] = pts[0];
						pts[3].x += dx*0.0001;
						pts[3].y += dy*0.0001;
						
						CHSRFace* pFace = CHSRFace::CreateFace(4 , pts);
						if(pFace)
						{
							pFace->id = nID++;
							pFace->m_enmLayer = GetHSRLayer();
							pFace->type= BOX;
							pFace->ele = HSR_RECTANGLE_SHAPE;
							pFace->SetColor(*m_pstrColor);
							
							CHSRApp* pHSRApp = CHSRApp::GetInstance();
							if(pHSRApp)
							{
								int nIndex = 0;
								///pFace->pHead = CHSRFace::CreateVertexList(4 , pts);
								for(PHSR_VERTEX ptr=pFace->pHead;ptr;ptr = ptr->next,nIndex++)
								{
									if(0 == nIndex)
									{
										pFace->volume.x[0] = ptr->pt->x;
										pFace->volume.x[1] = ptr->pt->x;
										pFace->volume.y[0] = ptr->pt->y;
										pFace->volume.y[1] = ptr->pt->y;
										pFace->volume.z[0] = ptr->pt->z;
										pFace->volume.z[1] = ptr->pt->z;
									}
									else
									{
										if(pFace->volume.x[0] > ptr->pt->x) pFace->volume.x[0] = ptr->pt->x;
										if(pFace->volume.x[1] < ptr->pt->x) pFace->volume.x[1] = ptr->pt->x;
										if(pFace->volume.y[0] > ptr->pt->y) pFace->volume.y[0] = ptr->pt->y;
										if(pFace->volume.y[1] < ptr->pt->y) pFace->volume.y[1] = ptr->pt->y;
										if(pFace->volume.z[0] > ptr->pt->z) pFace->volume.z[0] = ptr->pt->z;
										if(pFace->volume.z[1] < ptr->pt->z) pFace->volume.z[1] = ptr->pt->z;
									}
								}
								
								pHSRApp->AppendFace(NULL , pFace);
							}
						}
					}
				}
			}
		}
		else
		{
			CHSRFace* pFace = CHSRFace::CreateFace(nCount , pts);
			if(pFace)
			{
				pFace->id = nID++;
				pFace->m_enmLayer = GetHSRLayer();
				pFace->type= BOX;
				pFace->ele = HSR_RECTANGLE_SHAPE;
				pFace->SetColor(*m_pstrColor);

				int nIndex = 0;
				for(PHSR_VERTEX ptr=pFace->pHead;ptr;ptr = ptr->next,nIndex++)
				{
					if(0 == nIndex)
					{
						pFace->volume.x[0] = ptr->pt->x;
						pFace->volume.x[1] = ptr->pt->x;
						pFace->volume.y[0] = ptr->pt->y;
						pFace->volume.y[1] = ptr->pt->y;
						pFace->volume.z[0] = ptr->pt->z;
						pFace->volume.z[1] = ptr->pt->z;
					}
					else
					{
						if(pFace->volume.x[0] > ptr->pt->x) pFace->volume.x[0] = ptr->pt->x;
						if(pFace->volume.x[1] < ptr->pt->x) pFace->volume.x[1] = ptr->pt->x;
						if(pFace->volume.y[0] > ptr->pt->y) pFace->volume.y[0] = ptr->pt->y;
						if(pFace->volume.y[1] < ptr->pt->y) pFace->volume.y[1] = ptr->pt->y;
						if(pFace->volume.z[0] > ptr->pt->z) pFace->volume.z[0] = ptr->pt->z;
						if(pFace->volume.z[1] < ptr->pt->z) pFace->volume.z[1] = ptr->pt->z;
					}
					/// set the last line is invisible.
					if((NULL != ptr->next) && (NULL == ptr->next->next)) ptr->render = HIDE;	/// 2015.08.27 - modified by humkyung
				}
				pHSRApp->AppendFace(NULL , pFace);
			}
			else
			{
				POINT_T anchor[2] = {0,};
				if(ERROR_SUCCESS == FindLinePassingDataSet(anchor , nCount , pts))
				{
					double dx = anchor[1].x - anchor[0].x;
					double dy = anchor[1].y - anchor[0].y;
					const double length = sqrt(dx*dx + dy*dy);
					if(length > 0.f)
					{
						//! give a thickness virtually.
						dx /= length; dy /= length;
						double temp = -dy;
						dy = dx;
						dx = temp;
						pts[0] = anchor[0];
						pts[0].z = (anchor[0].x + anchor[1].z)*0.5;
						pts[1] = anchor[1];
						pts[1].z = pts[0].z;
						pts[2] = pts[1];
						pts[2].x += dx*0.0001;
						pts[2].y += dy*0.0001;
						pts[3] = pts[0];
						pts[3].x += dx*0.0001;
						pts[3].y += dy*0.0001;
						
						CHSRFace* pFace = CHSRFace::CreateFace(4 , pts);
						if(pFace)
						{
							pFace->id = nID++;
							pFace->m_enmLayer = GetHSRLayer();
							pFace->type= BOX;
							pFace->ele = HSR_RECTANGLE_SHAPE;
							pFace->SetColor(*m_pstrColor);
							
							CHSRApp* pHSRApp = CHSRApp::GetInstance();
							if(pHSRApp)
							{
								int nIndex = 0;
								///pFace->pHead = CHSRFace::CreateVertexList(4 , pts);
								for(PHSR_VERTEX ptr=pFace->pHead;ptr;ptr = ptr->next,nIndex++)
								{
									if(0 == nIndex)
									{
										pFace->volume.x[0] = ptr->pt->x;
										pFace->volume.x[1] = ptr->pt->x;
										pFace->volume.y[0] = ptr->pt->y;
										pFace->volume.y[1] = ptr->pt->y;
										pFace->volume.z[0] = ptr->pt->z;
										pFace->volume.z[1] = ptr->pt->z;
									}
									else
									{
										if(pFace->volume.x[0] > ptr->pt->x) pFace->volume.x[0] = ptr->pt->x;
										if(pFace->volume.x[1] < ptr->pt->x) pFace->volume.x[1] = ptr->pt->x;
										if(pFace->volume.y[0] > ptr->pt->y) pFace->volume.y[0] = ptr->pt->y;
										if(pFace->volume.y[1] < ptr->pt->y) pFace->volume.y[1] = ptr->pt->y;
										if(pFace->volume.z[0] > ptr->pt->z) pFace->volume.z[0] = ptr->pt->z;
										if(pFace->volume.z[1] < ptr->pt->z) pFace->volume.z[1] = ptr->pt->z;
									}
								}
								
								pHSRApp->AppendFace(NULL , pFace);
							}
						}
					}
				}
			}
		}
	}

	return NULL;
}

/**
	@brief	segmentize ellipse3d.

	@author	humkyung

	@date	2009.11.03

	@param	

	@return
*/
int CEllipseElement::CreateLineElements()
{
	static POINT_T pts[128] = {'\0',};

	/// clear already created lines. - 2011.04.01 added by humkyung
	{
		for(list<CLineElement*>::iterator itr = m_LineElementEntry.begin();itr != m_LineElementEntry.end();++itr)
		{
			delete (*itr);
		}
		m_LineElementEntry.clear();
	}
	/// up to here

	const int iCount = ConstructVertexList(pts);
	for(int i = 0;i < iCount - 1;++i)
	{
		CLineElement* pLineElm = new CLineElement;
		if(pLineElm)
		{
			pLineElm->SetLayerString((*m_pstrLayer));
			pLineElm->m_pt[0] = pts[i];
			pLineElm->m_pt[1] = pts[i + 1];

			/// set point' code - 2012.01.11 added by humkyung
			pLineElm->m_pt[0].reserved = (0 == i) ? PT_PRIMARY : PT_CONSTRUCTION;
			pLineElm->m_pt[1].reserved = ((iCount-2) == i) ? PT_PRIMARY : PT_CONSTRUCTION;
			/// up to here

			pLineElm->SetLayerString((*m_pstrLayer));
			
			m_LineElementEntry.push_back(pLineElm);
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	segmentize ellipse3d.

	@author	humkyung

	@date	2009.11.03

	@param	

	@return	vertex count
*/
int CEllipseElement::ConstructVertexList(PPOINT_T pts)
{
	double nStartAng = DEG2RAD(m_ellipse3d.nStartAngle);
	const double nSweepAng = DEG2RAD(m_ellipse3d.nSweepAngle);
	double nStep = nSweepAng / CEllipseElement::SEGMENTS;
	double c=cos(nStep);
	double s=sin(nStep);
	double ab=1.,ba=1.;
	if((0. != m_ellipse3d.nAxis[0]) && (0. != m_ellipse3d.nAxis[1]))
	{
		ab=m_ellipse3d.nAxis[0]/m_ellipse3d.nAxis[1];
		ba=m_ellipse3d.nAxis[1]/m_ellipse3d.nAxis[0];
	}
	pts[0].x = m_ellipse3d.nAxis[0]*cos(nStartAng);
	pts[0].y = m_ellipse3d.nAxis[1]*sin(nStartAng);
	pts[0].z = 0.;
	int i = 0;
	for(i = 1;i < CEllipseElement::SEGMENTS;++i)
	{
		pts[i].x = pts[i-1].x*c - ab*pts[i-1].y*s;
		pts[i].y = ba*pts[i-1].x*s + pts[i-1].y*c;
		pts[i].z = 0.;
	}
	pts[i].x = m_ellipse3d.nAxis[0]*cos(nStartAng + nSweepAng);
	pts[i].y = m_ellipse3d.nAxis[1]*sin(nStartAng + nSweepAng);
	pts[i].z = 0.;

	for(i = 0;i < (CEllipseElement::SEGMENTS + 1);++i)
	{
		const double x = m_matrix[0][0]*pts[i].x + m_matrix[0][1]*pts[i].y + m_matrix[0][2]*pts[i].z;
		const double y = m_matrix[1][0]*pts[i].x + m_matrix[1][1]*pts[i].y + m_matrix[1][2]*pts[i].z;
		const double z = m_matrix[2][0]*pts[i].x + m_matrix[2][1]*pts[i].y + m_matrix[2][2]*pts[i].z;
		pts[i].x = x;
		pts[i].y = y;
		pts[i].z = z;

		pts[i].x += m_ellipse3d.ptOrigin.x;
		pts[i].y += m_ellipse3d.ptOrigin.y;
		pts[i].z += m_ellipse3d.ptOrigin.z;
	}

	return i;
}

/**
	@brief	

	@author	BHK

	@date	2009.11.03

	@param	

	@return	
*/
int CEllipseElement::FindLinePassingDataSet(PPOINT_T anchor , const int& nCount , PPOINT_T pts)
{
	assert(anchor && "anchor is NULL");
	assert(pts && "pts is NULL");

	if(anchor && pts)
	{
		double nMaxLength = 0.f;
		//! find out a line passing point set using linear interpolation.
		for(int i = 0;i < nCount - 1;++i)
		{
			for(int j = i + 1;j < nCount - 1;++j)
			{
				const double dx = pts[j].x - pts[i].x;
				const double dy = pts[j].y - pts[i].y;
				const double length = dx*dx + dy*dy;
				if(length > nMaxLength)
				{
					nMaxLength = length;
					anchor[0] = pts[i];
					anchor[1] = pts[j];
				}
			}
		}

		return ERROR_SUCCESS;
	}

	return ERROR_BAD_ENVIRONMENT;
}
