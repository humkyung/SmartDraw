// TyVolume.cpp: implementation of the CTyVolume class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TyVolume.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTyVolume::CTyVolume()
{

}

/**
 * CTyVolume:
 *
 * @param ptMin 
 * @param ptMax 
 * @return  
 */
CTyVolume::CTyVolume(const CTyPoint3d& ptMin,const CTyPoint3d& ptMax)
{
	m_minPoint = ptMin;
	m_maxPoint = ptMax;
}

/**
 * ~CTyVolume:
 *
 * @return  
 */
CTyVolume::~CTyVolume()
{

}


/**
 * GetMinPoint:
 *
 * @return CTyPoint3d 
 */
CTyPoint3d CTyVolume::GetMinPoint()
{
	return m_minPoint;
}

/**
 * GetMaxPoint:
 *
 * @return CTyPoint3d 
 */
CTyPoint3d CTyVolume::GetMaxPoint()
{
	return m_maxPoint;
}

/**
 * SetMinPoint:
 *
 * @param point 
 * @return void 
 */
void CTyVolume::SetMinPoint(const CTyPoint3d &point)
{
	m_minPoint = point;
}

/**
 * SetMaxPoint:
 *
 * @param point 
 * @return void 
 */
void CTyVolume::SetMaxPoint(const CTyPoint3d &point)
{
	m_maxPoint = point;
}
