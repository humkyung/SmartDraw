// PreLINEFormat.cpp: implementation of the CLineElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmartHSR.h"
#include "LineElement.h"

IMPLEMENTS_HSR_ELEMENT(CLineElement , CHSRElement , _T("__LINE_HSR_ELMEENT__"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLineElement::CLineElement() : CHSRElement()
{
	memset(m_pt , '\0' , sizeof(POINT_T)*2);
	m_bVisible = true;
}

CLineElement::~CLineElement()
{

}

#ifdef VER_03
/**	
	@brief	The CLineElement::ParseLine function

	@author	BHK

	@date	?

	@param	pHSRScanner	a parameter of type CHSRScanner*

	@return	bool	
*/
bool CLineElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;
	
	if(pHSRScanner)
	{
		char* pVal=NULL;
		
		(*m_pstrLayer) = pHSRScanner->Val("layer");
		if(pVal = pHSRScanner->Val("group")) m_nGroup = ATOI_T(pVal);
		if(pVal = pHSRScanner->Val("pt1"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[0].x),&(m_pt[0].y),&(m_pt[0].z));
		}
		if(pVal = pHSRScanner->Val("pt2"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[1].x),&(m_pt[1].y),&(m_pt[1].z));
		}
		
		CalcVolume();

		bRet = true;
	}
	
	return bRet;
}
#else
/**	
	@brief	The CLineElement::ParseLine function
	@author	humkyung
	@date	?
	@param	pHSRScanner	a parameter of type CHSRScanner*
	@remarks\n
	;2004.03.29 - store current layer
	@return	bool	
*/
bool CLineElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;
		m_nGroup = ATOI_T((*pHSRScanner->m_aryFactor)[1].c_str());

		m_pt[0].x = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
		m_pt[0].y = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
		m_pt[0].z = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
		m_pt[1].x = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
		m_pt[1].y = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
		m_pt[1].z = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());

		if(pHSRScanner->m_nFactor > 8)
		{
			m_bVisible = (1 == ATOI_T((*pHSRScanner->m_aryFactor)[8].c_str()));
		}

		if(10 == pHSRScanner->m_nFactor)
		{
			SetColorString((*pHSRScanner->m_aryFactor)[9]);
		}

		CalcVolume();
		bRet = true;
	}

	return bRet;
}
#endif

/******************************************************************************
    @author     humkyung
    @date       2012-03-05
    @class      CLineElement
    @function   IsInVolume
    @return     bool
    @param      const CIsVolume& volume
    @brief
******************************************************************************/
bool CLineElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;
}

/**
 * GetBoundingBox:
 *
 * @return CHSRBoundingBox 
 */
CHSRBoundingBox CLineElement::GetBoundingBox()
{
	double mins[3],maxs[3];
	mins[0] = maxs[0] = m_pt[0].x;
	mins[1] = maxs[1] = m_pt[0].y;
	mins[2] = maxs[2] = m_pt[0].z;
	
	if(m_pt[1].x < mins[0]) mins[0] = m_pt[1].x;
	if(m_pt[1].x > maxs[0]) maxs[0] = m_pt[1].x;
	if(m_pt[1].y < mins[1]) mins[1] = m_pt[1].y;
	if(m_pt[1].y > maxs[1]) maxs[1] = m_pt[1].y;
	if(m_pt[1].z < mins[2]) mins[2] = m_pt[1].z;
	if(m_pt[1].z > maxs[2]) maxs[2] = m_pt[1].z;

	CHSRBoundingBox bbox(mins[0],mins[1],mins[2],maxs[0],maxs[1],maxs[2]);
	return bbox;
}

/**
	@brief	calculate volume of line.

	@author	BHK

	@date	2009.11.06

	@param

	@return	ERROR_SUCCESS
*/
int CLineElement::CalcVolume()
{
	m_vol.x[LOW] = m_vol.x[HIGH] = m_pt[0].x;
	m_vol.y[LOW] = m_vol.y[HIGH] = m_pt[0].y;
	m_vol.z[LOW] = m_vol.z[HIGH] = m_pt[0].z;
	if(m_vol.x[LOW] > m_pt[1].x) m_vol.x[LOW] = m_pt[1].x;
	if(m_vol.x[HIGH] < m_pt[1].x) m_vol.x[HIGH] = m_pt[1].x;
	if(m_vol.y[LOW] > m_pt[1].y) m_vol.y[LOW] = m_pt[1].y;
	if(m_vol.y[HIGH] < m_pt[1].y) m_vol.y[HIGH] = m_pt[1].y;
	if(m_vol.z[LOW] > m_pt[1].z) m_vol.z[LOW] = m_pt[1].z;
	if(m_vol.z[HIGH] < m_pt[1].z) m_vol.z[HIGH] = m_pt[1].z;

	return ERROR_SUCCESS;
}

/**
	@brief	create a face for line.

	@author	BHK

	@date	2009.11.06

	@param

	@return	CHSRFace*
*/
CHSRFace* CLineElement::CreateFace(CHSRViewFormat* pViewFormat,long& nID)
{
	assert(pViewFormat && "pViewFormat is NULL");

	if(pViewFormat)
	{
		static POINT_T pts[4];	//! POINT_T buffer
		pts[0] = pViewFormat->MODEL2VIEW(m_pt[0]);
		pts[1] = pViewFormat->MODEL2VIEW(m_pt[1]);
		
		//! give a imaginary thickness.
		double dx = pts[1].x - pts[0].x;
		double dy = pts[1].y - pts[0].y;
		const double length = sqrt(dx*dx + dy*dy);
		if(0.f == length) return NULL;
		dx /= length; dy /= length;
		double temp = -dy;
		dy = dx;
		dx = temp;
		pts[2] = pts[1];
		pts[2].x += dx*0.0001;
		pts[2].y += dy*0.0001;
		pts[3] = pts[0];
		pts[3].x += dx*0.0001;
		pts[3].y += dy*0.0001;

		CHSRFace* pFace = CHSRFace::CreateFace(4 , pts);///new CHSRFace;
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

	return NULL;
}