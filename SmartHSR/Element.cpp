// Element.cpp: implementation of the CHSRElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Element.h"
#include "HSRElementFactory.h"

const double CHSRElement::m_tol = 0.001;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHSRElement::CHSRElement()
{
	m_pstrLayer = new STRING_T(_T(""));
	m_pstrColor = new STRING_T(_T(""));
	m_vol.x[0] = m_vol.x[1] = 0.;
	m_vol.y[0] = m_vol.y[1] = 0.;
	m_vol.z[0] = m_vol.z[1] = 0.;
}

CHSRElement::~CHSRElement()
{
	try
	{
		if(m_pstrLayer)
		{
			delete m_pstrLayer;
			m_pstrLayer = NULL;
		}

		SAFE_DELETE(m_pstrColor);
	}
	catch(...)
	{
	}
}

/**
	@brief	

	@author	

	@date	2009.09.25
*/
int CHSRElement::Copy(CHSRElement* pCopied)
{
	assert(pCopied && "pCopied is NULL");

	if(pCopied)
	{
		m_pstrLayer = pCopied->m_pstrLayer;
		m_vol = pCopied->m_vol;
		m_model = pCopied->m_model;

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	

	@author	

	@date	2009.09.25
*/
STRING_T CHSRElement::TypeString()
{
	static const STRING_T pTypeString(_T("__HSR_ELEMENT__"));
	return pTypeString;
}

/**
	@brief	

	@author	

	@date	2009.09.25
*/
STRING_T CHSRElement::GetTypeString() const
{
	return CHSRElement::TypeString();
}

/**
	@brief return layer code match layer string.
	@author	BHK
	@date	2009.09.25
	@param	
	@return
 */
HSR_LAYER_E CHSRElement::GetHSRLayer() const
{
	return CHSRLayer::GetLayerCode(GetTypeString() , m_model , m_pstrLayer->c_str());
}

/**
	@brief	check two points if they are same or not.

	@author	BHK
	
	@date	2009.11.04

	@param	
	@param

	@return
*/
bool CHSRElement::IsSamePoint(const POINT_T& pt1,const POINT_T& pt2)
{
	double dx=0.,dy=0.,dz=0.;

	dx = pt2.x - pt1.x;
	dy = pt2.y - pt1.y;
	dz = pt2.z - pt1.z;
	if((fabs(dx) < 0.001) && (fabs(dy) < 0.001) && (fabs(dz) < 0.001))
		return true;

	return false;
}