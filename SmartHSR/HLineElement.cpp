// HLineElement.cpp: implementation of the CHLineElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmartHSR.h"
#include "HLineElement.h"

IMPLEMENTS_HSR_ELEMENT(CHLineElement , CHSRElement , _T("HLINE"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	
	@brief
*/
CHLineElement::CHLineElement()
{
}

CHLineElement::~CHLineElement()
{

}

/**	
	@brief	The CHLineElement::ParseLine function

	@param	pHSRScanner	a parameter of type CHSRScanner*

	@remarks\n
	2004.03.29 - store current layer

	@return	void	
*/
bool CHLineElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet = false;

	if(pHSRScanner)
	{
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;
		m_nGroup = ATOI_T((*pHSRScanner->m_aryFactor)[2].c_str());

		m_pt[0].x = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
		m_pt[0].y = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
		m_pt[0].z = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
		m_pt[1].x = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
		m_pt[1].y = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());
		m_pt[1].z = ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());

		m_bVisible= (1 == ATOI_T((*pHSRScanner->m_aryFactor)[9].c_str())) ? true : false;

		CalcVolume();
		bRet = true;
	}

	return true;
}

/**
	@brief	calculate volume of hline.

	@author	BHK

	@date	2009.11.19

	@param

	@return	ERROR_SUCCESS
*/
int CHLineElement::CalcVolume()
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
	@brief	뷰의 볼륨에 속하는지 검사를 한다.
	
	@author	BHK

	@date	2009.11.19

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CHLineElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;
}

/**
	@brief	

	@author	BHK

	@date	

	@param

	@return
*/
CHSRBoundingBox CHLineElement::GetBoundingBox()
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
