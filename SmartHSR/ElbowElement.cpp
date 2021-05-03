// PreELBOWFormat.cpp: implementation of the CElbowElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "ElbowElement.h"
#include "SmartHSR.h"

IMPLEMENTS_HSR_ELEMENT(CElbowElement , CHSRElement , _T("__ELBOW_HSR_ELEMENT__"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CElbowElement::CElbowElement()
{
	m_nSegments = 20;
}

CElbowElement::CElbowElement(ELBOW_TYPE_E type,POINT_T ptOrigin,double nTorusRadius,double nTubeRadius)
{
	m_eType = type;
	m_ptOrigin = ptOrigin;
	m_nTorusRadius = nTorusRadius;
	m_nTubeRadius  = nTubeRadius;
	m_nSegments = 16;
}

CElbowElement::~CElbowElement()
{

}

#ifdef VER_03
/**	\brief	The CElbowElement::ParseLine function

	\param	pHSRScanner	a parameter of type CHSRScanner*

	\return	bool	
*/
bool CElbowElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		if(8 == pHSRScanner->m_nFactor)
		{
			m_eType = BW;
			if(true == (bRet = ParseBWLine(pHSRScanner)))
				Volume();
		}
		else if(14 == pHSRScanner->m_nFactor)
		{
			m_eType = SW;
			if(true == (bRet = ParseSWLine(pHSRScanner)))
				Volume();
		}
	}

	return bRet;
}
#else
/**	
	@brief	The CElbowElement::ParseLine function
	@param	pHSRScanner	a parameter of type CHSRScanner*
	@return	bool	
*/
bool CElbowElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		if(pHSRScanner->m_nFactor >= 29)
		{
			m_eType = SW;
			bRet = ParseSWLine(pHSRScanner);
			CalcVolume();
		}
		else if(pHSRScanner->m_nFactor >= 15)
		{
			m_eType = BW;
			bRet = ParseBWLine(pHSRScanner);
			CalcVolume();
		}
	}

	return bRet;
}
#endif

/**	\brief	The CElbowElement::Volume function

	\remarks\n
	;2004.12.15 - recalculate volume vector.

	\return	void	
*/
void CElbowElement::CalcVolume()
{
	m_vol.x[0] = m_vol.x[1] = m_pt[0].x;
	m_vol.y[0] = m_vol.y[1] = m_pt[0].y;
	m_vol.z[0] = m_vol.z[1] = m_pt[0].z;
	/*
	VECTOR_T vecAxis={	m_pt[1].x - m_pt[0].x,
				m_pt[1].y - m_pt[0].y,
				m_pt[1].z - m_pt[0].z};
	CMath::NormalizeVector(vecAxis);
		
	double alpha=0.,beta=0.;
	CGeometry::GetRotatedAngleOfAxis(vecAxis,alpha,beta);
	VECTOR_T vecX={1.,0.,0.},vecY={0.,1.,0.},vecTmp={1,0,0};
	vecX = CGeometry::RotateAboutYXAxis(vecX,-beta,-alpha);
	vecY = CGeometry::RotateAboutYXAxis(vecY,-beta,-alpha);
	*/
	
	VECTOR_T vecX={1.,0.,0.},vecY={0.,1.,0.},vecTmp={1,0,0};
	POINT_T ptEDGE[8]={0.,};
	vecX.dx = m_pt[0].x - m_ptOrigin.x;
	vecX.dy = m_pt[0].y - m_ptOrigin.y;
	vecX.dz = m_pt[0].z - m_ptOrigin.z;
	vecTmp.dx = m_ptCross.x - m_ptOrigin.x;
	vecTmp.dy = m_ptCross.y - m_ptOrigin.y;
	vecTmp.dz = m_ptCross.z - m_ptOrigin.z;
	vecY = CMath::CrossProduct(vecX,vecTmp);
	CMath::NormalizeVector(vecX);
	CMath::NormalizeVector(vecY);

	ptEDGE[0].x = m_pt[0].x - vecX.dx*m_nTubeRadius - vecY.dx*m_nTubeRadius;
	ptEDGE[0].y = m_pt[0].y - vecX.dy*m_nTubeRadius - vecY.dy*m_nTubeRadius;
	ptEDGE[0].z = m_pt[0].z - vecX.dz*m_nTubeRadius - vecY.dz*m_nTubeRadius;
	ptEDGE[1].x = m_pt[0].x - vecX.dx*m_nTubeRadius + vecY.dx*m_nTubeRadius;
	ptEDGE[1].y = m_pt[0].y - vecX.dy*m_nTubeRadius + vecY.dy*m_nTubeRadius;
	ptEDGE[1].z = m_pt[0].z - vecX.dz*m_nTubeRadius + vecY.dz*m_nTubeRadius;
	ptEDGE[2].x = m_pt[0].x + vecX.dx*m_nTubeRadius + vecY.dx*m_nTubeRadius;
	ptEDGE[2].y = m_pt[0].y + vecX.dy*m_nTubeRadius + vecY.dy*m_nTubeRadius;
	ptEDGE[2].z = m_pt[0].z + vecX.dz*m_nTubeRadius + vecY.dz*m_nTubeRadius;
	ptEDGE[3].x = m_pt[0].x + vecX.dx*m_nTubeRadius - vecY.dx*m_nTubeRadius;
	ptEDGE[3].y = m_pt[0].y + vecX.dy*m_nTubeRadius - vecY.dy*m_nTubeRadius;
	ptEDGE[3].z = m_pt[0].z + vecX.dz*m_nTubeRadius - vecY.dz*m_nTubeRadius;
	
	vecX.dx = m_pt[1].x - m_ptOrigin.x;
	vecX.dy = m_pt[1].y - m_ptOrigin.y;
	vecX.dz = m_pt[1].z - m_ptOrigin.z;
	vecTmp.dx = m_ptCross.x - m_ptOrigin.x;
	vecTmp.dy = m_ptCross.y - m_ptOrigin.y;
	vecTmp.dz = m_ptCross.z - m_ptOrigin.z;
	vecY = CMath::CrossProduct(vecTmp,vecX);
	CMath::NormalizeVector(vecX);
	CMath::NormalizeVector(vecY);
	ptEDGE[4].x = m_pt[1].x - vecX.dx*m_nTubeRadius - vecY.dx*m_nTubeRadius;
	ptEDGE[4].y = m_pt[1].y - vecX.dy*m_nTubeRadius - vecY.dy*m_nTubeRadius;
	ptEDGE[4].z = m_pt[1].z - vecX.dz*m_nTubeRadius - vecY.dz*m_nTubeRadius;
	ptEDGE[5].x = m_pt[1].x - vecX.dx*m_nTubeRadius + vecY.dx*m_nTubeRadius;
	ptEDGE[5].y = m_pt[1].y - vecX.dy*m_nTubeRadius + vecY.dy*m_nTubeRadius;
	ptEDGE[5].z = m_pt[1].z - vecX.dz*m_nTubeRadius + vecY.dz*m_nTubeRadius;
	ptEDGE[6].x = m_pt[1].x + vecX.dx*m_nTubeRadius + vecY.dx*m_nTubeRadius;
	ptEDGE[6].y = m_pt[1].y + vecX.dy*m_nTubeRadius + vecY.dy*m_nTubeRadius;
	ptEDGE[6].z = m_pt[1].z + vecX.dz*m_nTubeRadius + vecY.dz*m_nTubeRadius;
	ptEDGE[7].x = m_pt[1].x + vecX.dx*m_nTubeRadius - vecY.dx*m_nTubeRadius;
	ptEDGE[7].y = m_pt[1].y + vecX.dy*m_nTubeRadius - vecY.dy*m_nTubeRadius;
	ptEDGE[7].z = m_pt[1].z + vecX.dz*m_nTubeRadius - vecY.dz*m_nTubeRadius;
		
	for(int i=0;i < 8;i++)
	{
		if(m_vol.x[0] > ptEDGE[i].x) m_vol.x[0] = ptEDGE[i].x;
		if(m_vol.x[1] < ptEDGE[i].x) m_vol.x[1] = ptEDGE[i].x;
		if(m_vol.y[0] > ptEDGE[i].y) m_vol.y[0] = ptEDGE[i].y;
		if(m_vol.y[1] < ptEDGE[i].y) m_vol.y[1] = ptEDGE[i].y;
		if(m_vol.z[0] > ptEDGE[i].z) m_vol.z[0] = ptEDGE[i].z;
		if(m_vol.z[1] < ptEDGE[i].z) m_vol.z[1] = ptEDGE[i].z;
	}
}

/**	
	@brief	The CElbowElement::IsInVolume function

	@author	humkyung

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CElbowElement::IsInVolume(const CIsVolume& volume)
{
	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;
	
	return true;
}

/**	
	@brief	The CElbowElement::CreateFace function
	@param	pViewFmt	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&
	@return	PFACE	
*/
CHSRFace* CElbowElement::CreateFace(CHSRViewFormat* pViewFmt,long& nID)
{
	assert(pViewFmt && "pViewFmt is NULL");
	CHSRFace* pRet=NULL;

	if(pViewFmt)
	{
		if(BW == m_eType)
		{
			CreateBWFace(pViewFmt,nID);
		}
		else if(SW == m_eType)
		{
			CreateSWFace(pViewFmt,nID);
		}
	}

	return pRet;
}

#ifdef VER_03
/**	\brief	The CElbowElement::ParseBWLine function

	\param	pHSRScanner	a parameter of type CHSRScanner*

	\return	bool	
*/
bool CElbowElement::ParseBWLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pPreFormatParse is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		char* pVal=NULL;

		if(pVal = pHSRScanner->Val("layer")) m_strLayer = pVal;
		if(pVal = pHSRScanner->Val("pt1"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[0].x),&(m_pt[0].y),&(m_pt[0].z));
		}
		if(pVal = pHSRScanner->Val("pt2"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[1].x),&(m_pt[1].y),&(m_pt[1].z));
		}
		
		if(pVal = pHSRScanner->Val("cross pt"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_ptCross.x),&(m_ptCross.y),&(m_ptCross.z));
		}
		
		if(pVal = pHSRScanner->Val("origin pt"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_ptOrigin.x),&(m_ptOrigin.y),&(m_ptOrigin.z));
		}
		
		if(pVal = pHSRScanner->Val("torus radius")) m_nTorusRadius = ATOF_T(pVal);
		if(pVal = pHSRScanner->Val("tube radius")) m_nTubeRadius = ATOF_T(pVal);

		bRet = true;
	}

	return bRet;
}

/**	\brief	The CElbowElement::ParseSWLine function
	---pt1-ptPIPE[0][1]---ptPIPE[0][0]\n
	|\n
	|\n
	|\n
	pt0\n
	|\n
	ptPIPE[1][1]\n
	|\n
	|\n
	|\n
	ptPIPE[1][0]

	\param	pHSRScanner	a parameter of type CHSRScanner*

	\return	bool	
*/
bool CElbowElement::ParseSWLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pPreFormatParse is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		char* pVal=NULL;

		if(pVal = pHSRScanner->Val("layer")) m_strLayer = pVal;
		if(pVal = pHSRScanner->Val("pt1"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[0].x),&(m_pt[0].y),&(m_pt[0].z));
		}
		if(pVal = pHSRScanner->Val("pt2"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[1].x),&(m_pt[1].y),&(m_pt[1].z));
		}
		
		if(pVal = pHSRScanner->Val("cross pt"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_ptCross.x),&(m_ptCross.y),&(m_ptCross.z));
		}
		
		if(pVal = pHSRScanner->Val("origin pt"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_ptOrigin.x),&(m_ptOrigin.y),&(m_ptOrigin.z));
		}else	return false;
		
		if(pVal = pHSRScanner->Val("torus radius")) m_nTorusRadius = ATOF_T(pVal);
		if(pVal = pHSRScanner->Val("tube radius")) m_nTubeRadius = ATOF_T(pVal);
		
		if(pVal = pHSRScanner->Val("radius1")) m_nRadius[0] = ATOF_T(pVal);
		if(pVal = pHSRScanner->Val("top pt1"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_ptPIPE[0][0].x),&(m_ptPIPE[0][0].y),&(m_ptPIPE[0][0].z));
		}else	return false;
		if(pVal = pHSRScanner->Val("bottom pt1"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_ptPIPE[0][1].x),&(m_ptPIPE[0][1].y),&(m_ptPIPE[0][1].z));
		}else	return false;

		double dx[2]={0,},dy[2]={0,},dz[2]={0,};
		dx[0] = m_ptPIPE[0][0].x - m_ptCross.x;
		dy[0] = m_ptPIPE[0][0].y - m_ptCross.y;
		dz[0] = m_ptPIPE[0][0].z - m_ptCross.z;
		dx[1] = m_ptPIPE[0][1].x - m_ptCross.x;
		dy[1] = m_ptPIPE[0][1].y - m_ptCross.y;
		dz[1] = m_ptPIPE[0][1].z - m_ptCross.z;
		if((dx[0]*dx[0] + dy[0]*dy[0] + dz[0]*dz[0]) < (dx[1]*dx[1] + dy[1]*dy[1] + dz[1]*dz[1]))
			swap(m_ptPIPE[0][0],m_ptPIPE[0][1]);
		
		if(pVal = pHSRScanner->Val("radius2")) m_nRadius[1] = ATOF_T(pVal);
		if(pVal = pHSRScanner->Val("top pt2"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_ptPIPE[1][0].x),&(m_ptPIPE[1][0].y),&(m_ptPIPE[1][0].z));
		}else	return false;
		if(pVal = pHSRScanner->Val("bottom pt2"))
		{
			sscanf(pVal,"%lf,%lf,%lf",&(m_ptPIPE[1][1].x),&(m_ptPIPE[1][1].y),&(m_ptPIPE[1][1].z));
		}else	return false;
		
		dx[0] = m_ptPIPE[1][0].x - m_ptCross.x;
		dy[0] = m_ptPIPE[1][0].y - m_ptCross.y;
		dz[0] = m_ptPIPE[1][0].z - m_ptCross.z;
		dx[1] = m_ptPIPE[1][1].x - m_ptCross.x;
		dy[1] = m_ptPIPE[1][1].y - m_ptCross.y;
		dz[1] = m_ptPIPE[1][1].z - m_ptCross.z;
		if((dx[0]*dx[0] + dy[0]*dy[0] + dz[0]*dz[0]) < (dx[1]*dx[1] + dy[1]*dy[1] + dz[1]*dz[1]))
			swap(m_ptPIPE[1][0],m_ptPIPE[1][1]);
		m_pt[0] = m_ptPIPE[0][1];
		m_pt[1] = m_ptPIPE[1][1];
		
		bRet = true;
	}
	
	return bRet;
}
#else
/**	
	@brief	The CElbowElement::ParseBWLine function
	@param	pHSRScanner	a parameter of type CHSRScanner*
	@return	void	
*/
bool CElbowElement::ParseBWLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pPreFormatParse is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{	
		m_pt[0].x = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
		m_pt[0].y = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
		m_pt[0].z = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
		m_pt[1].x = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());
		m_pt[1].y = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
		m_pt[1].z = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
		
		m_ptCross.x = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());
		m_ptCross.y = ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());
		m_ptCross.z = ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str());
		
		m_ptOrigin.x= ATOF_T((*pHSRScanner->m_aryFactor)[10].c_str());
		m_ptOrigin.y= ATOF_T((*pHSRScanner->m_aryFactor)[11].c_str());
		m_ptOrigin.z= ATOF_T((*pHSRScanner->m_aryFactor)[12].c_str());
		
		m_nTorusRadius= ATOF_T((*pHSRScanner->m_aryFactor)[13].c_str());
		m_nTubeRadius = ATOF_T((*pHSRScanner->m_aryFactor)[14].c_str());

		if(pHSRScanner->m_nFactor > 15)
		{
			SetColorString((*pHSRScanner->m_aryFactor)[15]);
		}

		bRet = true;
	}

	return bRet;
}

/**	
	@brief	The CElbowElement::ParseSWLine function
	@param	pHSRScanner	a parameter of type CHSRScanner*
	@return	bool	
*/
bool CElbowElement::ParseSWLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pPreFormatParse is NULL");
	bool bRet=false;
	
	if(pHSRScanner)
	{
		m_ptCross.x = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());
		m_ptCross.y = ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());
		m_ptCross.z = ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str());
		
		m_ptOrigin.x= ATOF_T((*pHSRScanner->m_aryFactor)[10].c_str());
		m_ptOrigin.y= ATOF_T((*pHSRScanner->m_aryFactor)[11].c_str());
		m_ptOrigin.z= ATOF_T((*pHSRScanner->m_aryFactor)[12].c_str());
		
		m_nTorusRadius = ATOF_T((*pHSRScanner->m_aryFactor)[13].c_str());
		m_nTubeRadius  = ATOF_T((*pHSRScanner->m_aryFactor)[14].c_str());

		m_nRadius[0]     = ATOF_T((*pHSRScanner->m_aryFactor)[15].c_str());
		m_ptPIPE[0][0].x = ATOF_T((*pHSRScanner->m_aryFactor)[16].c_str());
		m_ptPIPE[0][0].y = ATOF_T((*pHSRScanner->m_aryFactor)[17].c_str());
		m_ptPIPE[0][0].z = ATOF_T((*pHSRScanner->m_aryFactor)[18].c_str());
		m_ptPIPE[0][1].x = ATOF_T((*pHSRScanner->m_aryFactor)[19].c_str());
		m_ptPIPE[0][1].y = ATOF_T((*pHSRScanner->m_aryFactor)[20].c_str());
		m_ptPIPE[0][1].z = ATOF_T((*pHSRScanner->m_aryFactor)[21].c_str());
		
		double dx[2]={0,},dy[2]={0,},dz[2]={0,};
		dx[0] = m_ptPIPE[0][0].x - m_ptCross.x;
		dy[0] = m_ptPIPE[0][0].y - m_ptCross.y;
		dz[0] = m_ptPIPE[0][0].z - m_ptCross.z;
		dx[1] = m_ptPIPE[0][1].x - m_ptCross.x;
		dy[1] = m_ptPIPE[0][1].y - m_ptCross.y;
		dz[1] = m_ptPIPE[0][1].z - m_ptCross.z;
		if((dx[0]*dx[0] + dy[0]*dy[0] + dz[0]*dz[0]) > (dx[1]*dx[1] + dy[1]*dy[1] + dz[1]*dz[1]))
			swap(m_ptPIPE[0][0],m_ptPIPE[0][1]);
	
		m_nRadius[1]     = ATOF_T((*pHSRScanner->m_aryFactor)[22].c_str());	
		m_ptPIPE[1][0].x = ATOF_T((*pHSRScanner->m_aryFactor)[23].c_str());
		m_ptPIPE[1][0].y = ATOF_T((*pHSRScanner->m_aryFactor)[24].c_str());
		m_ptPIPE[1][0].z = ATOF_T((*pHSRScanner->m_aryFactor)[25].c_str());
		m_ptPIPE[1][1].x = ATOF_T((*pHSRScanner->m_aryFactor)[26].c_str());
		m_ptPIPE[1][1].y = ATOF_T((*pHSRScanner->m_aryFactor)[27].c_str());
		m_ptPIPE[1][1].z = ATOF_T((*pHSRScanner->m_aryFactor)[28].c_str());

		if(pHSRScanner->m_nFactor > 29)
		{
			SetColorString((*pHSRScanner->m_aryFactor)[29]);
		}

		dx[0] = m_ptPIPE[1][0].x - m_ptCross.x;
		dy[0] = m_ptPIPE[1][0].y - m_ptCross.y;
		dz[0] = m_ptPIPE[1][0].z - m_ptCross.z;
		dx[1] = m_ptPIPE[1][1].x - m_ptCross.x;
		dy[1] = m_ptPIPE[1][1].y - m_ptCross.y;
		dz[1] = m_ptPIPE[1][1].z - m_ptCross.z;
		if((dx[0]*dx[0] + dy[0]*dy[0] + dz[0]*dz[0]) > (dx[1]*dx[1] + dy[1]*dy[1] + dz[1]*dz[1]))
			swap(m_ptPIPE[1][0],m_ptPIPE[1][1]);

		m_pt[0] = m_ptPIPE[0][0];
		m_pt[1] = m_ptPIPE[1][0];

		bRet = true;
	}

	return bRet;
}
#endif

#ifdef VER_03
/**	\brief	The CElbowElement::CreateBWFace function

	\param	pViewFmt	a parameter of type CPreViewFormat*
	\param	nID	a parameter of type long&

	\return	PFACE	
*/
CHSRFace* CElbowElement::CreateBWFace(CHSRViewFormat* pViewFmt,long& nID)
{
	assert(pViewFmt && "pViewFmt is NULL");
	const static VECTOR_T vecZ={0.,0.,1};
	CHSRFace* pRet=NULL;

	if(pViewFmt)
	{
		CHSRFace *pTop=NULL,*pSide=NULL,*pBottom=NULL;
		
		POINT_T ptOrigin[3]={0.,};
		ptOrigin[0] = pViewFmt->MODEL2VIEW(m_ptOrigin);

		POINT_T pt[4]={0.,};
		pt[0] = pViewFmt->MODEL2VIEW(m_pt[0]);
		pt[1] = pViewFmt->MODEL2VIEW(m_pt[1]);

		POINT_T ptCross={0,0,0};
		ptCross = pViewFmt->MODEL2VIEW(m_ptCross);
		const double nTubeRadius=pViewFmt->MODEL2VIEW(m_nTubeRadius);

		VECTOR_T vec[4]={0,};
		vec[0].dx = pt[0].x - ptOrigin[0].x;vec[0].dy = pt[0].y - ptOrigin[0].y;vec[0].dz = pt[0].z - ptOrigin[0].z;
		vec[1].dx = pt[1].x - ptOrigin[0].x;vec[1].dy = pt[1].y - ptOrigin[0].y;vec[1].dz = pt[1].z - ptOrigin[0].z;
		
		VECTOR_T vecNorm={0.,},vecRotate={0.,};
		CMath::GetCrossProduct(vecNorm,vec[0],vec[1]);
		CMath::GetCrossProduct(vecRotate,vecNorm,vec[0]);
		if(!CMath::NormalizeVector(vecRotate)) return pRet;
		if(!CMath::NormalizeVector(vecNorm)) return pRet;
		
		double nDot=CMath::DotProduct(vecNorm.dx,vecNorm.dy,vecNorm.dz,vecZ.dx,vecZ.dy,vecZ.dz);
		/// VERTICAL.
		if(fabs(nDot) < CHSRElement::m_tol)
		{
			CMath::GetCrossProduct(vec[2],vecRotate,vec[0]);
			bool b=CMath::NormalizeVector(vec[2]);
			assert(true == b);
			
			ptOrigin[1].x = ptOrigin[0].x + vec[2].dx*nTubeRadius;
			ptOrigin[1].y = ptOrigin[0].y + vec[2].dy*nTubeRadius;
			ptOrigin[1].z = ptOrigin[0].z + vec[2].dz*nTubeRadius;
			ptOrigin[2].x = ptOrigin[0].x - vec[2].dx*nTubeRadius;
			ptOrigin[2].y = ptOrigin[0].y - vec[2].dy*nTubeRadius;
			ptOrigin[2].z = ptOrigin[0].z - vec[2].dz*nTubeRadius;
			
			pt[2].x = pt[0].x + vec[2].dx*nTubeRadius;
			pt[2].y = pt[0].y + vec[2].dy*nTubeRadius;
			pt[2].z = pt[0].z + vec[2].dz*nTubeRadius;
			pt[3].x = pt[0].x - vec[2].dx*nTubeRadius;
			pt[3].y = pt[0].y - vec[2].dy*nTubeRadius;
			pt[3].z = pt[0].z - vec[2].dz*nTubeRadius;
			
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
			vec[0] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
			vec[1] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
			
			double nSweepAngle[3]={0,};
			nSweepAngle[0] = CGeometry::GetRotatedAngleInXYPlane(vec[0]);
			nSweepAngle[1] = CGeometry::GetRotatedAngleInXYPlane(vec[1]);
			
			nSweepAngle[2] = RAD2DEG(nSweepAngle[1] - nSweepAngle[0]);
			if(nSweepAngle[2] < 0.) nSweepAngle[2] += 360.;
			StoreArcPoints(m_ptEDGE[0],ptOrigin[1],pt[2],vecNorm,nSweepAngle[2]);
			StoreArcPoints(m_ptEDGE[1],ptOrigin[2],pt[3],vecNorm,nSweepAngle[2]);
			m_ptEDGE[0][0] = m_ptEDGE[0][0];
			m_ptEDGE[0][1] = m_ptEDGE[0][m_nSegments];
			m_ptEDGE[0][2] = m_ptEDGE[1][m_nSegments];
			m_ptEDGE[0][3] = m_ptEDGE[1][0];
			m_ptEDGE[0][4] = m_ptEDGE[0][0];

			if(pSide=CHSRFace::CreateFace(5,m_ptEDGE[0]))
			{
				pSide->m_enmLayer = LAYER_ELBOW;
				pSide->id     = nID++;
				pSide->type   = ELBOW;
				pSide->ele    = SIDEFACE | HSR_POLYGON_SHAPE;
				pSide->radius = nTubeRadius;
				pSide->ptCenter  = ptCross;
				pSide->ptCross   = ptCross;
				pSide->ptEdge[0] = pt[0];
				pSide->ptEdge[1] = pt[1];

				m_ptCenter[0].x = (m_ptEDGE[0][0].x + m_ptEDGE[0][3].x)*0.5;
				m_ptCenter[0].y = (m_ptEDGE[0][0].y + m_ptEDGE[0][3].y)*0.5;
				m_ptCenter[0].z = (m_ptEDGE[0][0].z + m_ptEDGE[0][3].z)*0.5;
				m_ptCenter[2].x = (m_ptEDGE[0][1].x + m_ptEDGE[0][2].x)*0.5;
				m_ptCenter[2].y = (m_ptEDGE[0][1].y + m_ptEDGE[0][2].y)*0.5;
				m_ptCenter[2].z = (m_ptEDGE[0][1].z + m_ptEDGE[0][2].z)*0.5;
				m_ptCenter[1].x = (m_ptCenter[0].x + m_ptCenter[2].x)*0.5;
				m_ptCenter[1].y = (m_ptCenter[0].y + m_ptCenter[2].y)*0.5;
				m_ptCenter[1].z = (m_ptCenter[0].z + m_ptCenter[2].z)*0.5;
				pSide->pCenterline = CHSRFace::CreateLine(3,m_ptCenter);
			}

			VECTOR_T vecNorm={0.};
			vecNorm.dx = pt[0].x - ptCross.x;
			vecNorm.dy = pt[0].y - ptCross.y;
			vecNorm.dz = pt[0].z - ptCross.z;
			if(!CMath::NormalizeVector(vecNorm)) return pRet;

			double nDot=CMath::DotProduct(vecNorm,vecZ);
			if(fabs(nDot) > CHSRElement::m_tol)
			{
				if(pTop = CHSRFace::CreateCircleFace(pt[0],nTubeRadius,vecNorm,m_nSegments))
				{
					pTop->m_enmLayer = LAYER_ELBOW;
					pTop->type = ELBOW;
					pTop->id   = nID++;

					if(fabs(fabs(nDot) - 1.) < CHSRElement::m_tol)
						pTop->ele  = HSR_CIRCLE_SHAPE;
					else	pTop->ele  = HSR_POLYGON_SHAPE;
					pTop->ptCenter = pt[0];
					pTop->radius   = nTubeRadius;
					pTop->ptEdge[0]= pt[1];

					if((fabs(fabs(nDot) - 1.) < CHSRElement::m_tol) && pSide)
					{
						double dx=pt[0].x - pt[1].x;
						double dy=pt[0].y - pt[1].y;
						double dz=pt[0].z - pt[1].z;
						double nLen= sqrt(dx*dx + dy*dy + dz*dz);

						if(nLen > 0.) pSide->symbol[0]  = true;
					}
				}
			}
			else
			{
				if(pSide) pSide->mark[0] = true;
			}

			vecNorm.dx = pt[1].x - ptCross.x;
			vecNorm.dy = pt[1].y - ptCross.y;
			vecNorm.dz = pt[1].z - ptCross.z;
			if(!CMath::NormalizeVector(vecNorm)) return pRet;

			nDot=CMath::DotProduct(vecNorm,vecZ);
			if(fabs(nDot) > CHSRElement::m_tol)
			{
				if(pBottom = CHSRFace::CreateCircleFace(pt[1],nTubeRadius,vecNorm,m_nSegments))
				{
					pBottom->m_enmLayer = LAYER_ELBOW;
					pBottom->type = ELBOW;
					pBottom->id   = nID++;
					if(fabs(fabs(nDot) - 1.) < 0.001)
						pBottom->ele  = HSR_CIRCLE_SHAPE;
					else	pBottom->ele  = HSR_POLYGON_SHAPE;
					pBottom->ptCenter = pt[1];
					pBottom->radius   = nTubeRadius;
					pBottom->ptEdge[0]= pt[0];

					if((fabs(fabs(nDot) - 1.) < CHSRElement::m_tol) && pSide)
					{
						double dx=pt[0].x - pt[1].x;
						double dy=pt[0].y - pt[1].y;
						double dz=pt[0].z - pt[1].z;
						double nLen= sqrt(dx*dx + dy*dy + dz*dz);

						if(nLen > 0.) pSide->symbol[1]  = true;
					}
				}
			}else{
				if(pSide) pSide->mark[1] = true;
			}
		/// HORIZONTAL.
		}
		else if(fabs(fabs(nDot) - 1.) < CHSRElement::m_tol)
		{
			CMath::GetCrossProduct(vec[2],vecRotate,vecZ);
			if(!CMath::NormalizeVector(vec[2])) return pRet;
			
			ptOrigin[1] = ptOrigin[0];
			ptOrigin[2] = ptOrigin[0];
			
			pt[2].x = pt[0].x + vec[2].dx*nTubeRadius;
			pt[2].y = pt[0].y + vec[2].dy*nTubeRadius;
			pt[2].z = pt[0].z + vec[2].dz*nTubeRadius;
			pt[3].x = pt[0].x - vec[2].dx*nTubeRadius;
			pt[3].y = pt[0].y - vec[2].dy*nTubeRadius;
			pt[3].z = pt[0].z - vec[2].dz*nTubeRadius;
			
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
			vec[0] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
			vec[1] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
			
			double nSweepAngle[3]={0,};
			nSweepAngle[0] = CGeometry::GetRotatedAngleInXYPlane(vec[0]);
			nSweepAngle[1] = CGeometry::GetRotatedAngleInXYPlane(vec[1]);
			
			POINT_T ptEDGE[2][64]={0.,};
			nSweepAngle[2] = RAD2DEG(nSweepAngle[1] - nSweepAngle[0]);
			if(nSweepAngle[2] < 0.) nSweepAngle[2] += 360.;
			StoreArcPoints(ptEDGE[0],ptOrigin[1],pt[2],vecNorm,nSweepAngle[2]);
			StoreArcPoints(ptEDGE[1],ptOrigin[2],pt[3],vecNorm,nSweepAngle[2]);
			for(int i=0;i < m_nSegments + 1;i++)
			{
				m_ptCenter[i].x = (ptEDGE[0][i].x + ptEDGE[1][i].x)*0.5;
				m_ptCenter[i].y = (ptEDGE[0][i].y + ptEDGE[1][i].y)*0.5;
				m_ptCenter[i].z = (ptEDGE[0][i].z + ptEDGE[1][i].z)*0.5;
				ptEDGE[0][m_nSegments + i + 1] = ptEDGE[1][m_nSegments - i];
			}
			ptEDGE[0][m_nSegments + i + 1] = ptEDGE[0][0];

			if(pSide=CHSRFace::CreateFace(m_nSegments + i + 2,ptEDGE[0]))
			{
				pSide->m_enmLayer = LAYER_ELBOW;
				pSide->id     = nID++;
				pSide->type   = ELBOW;
				pSide->ele    = SIDEFACE | HSR_ARC_SHAPE;
				pSide->radius = nTubeRadius;
				pSide->ptCenter  = ptOrigin[0];
				pSide->ptCross   = ptCross;
				pSide->ptEdge[0] = pt[0];
				pSide->ptEdge[1] = pt[1];

				pSide->pCenterline = CHSRFace::CreateLine(m_nSegments + 1,m_ptCenter);

				pSide->mark[0]  = true;
				pSide->ptMark[0]= pt[0];
				pSide->symbol[0]= false; 
				pSide->mark[1]  = true;
				pSide->ptMark[1]= pt[1];
				pSide->symbol[1]= false; 
			}
		}
		else
		{ /// SLOPE.
			double dx=pt[0].x - ptOrigin[0].x;
			double dy=pt[0].y - ptOrigin[0].y;
			double dz=pt[0].z - ptOrigin[0].z;
			double nTorusRadius=sqrt(dx*dx + dy*dy + dz*dz);
	
			vec[0].dx = pt[0].x - ptOrigin[0].x;vec[0].dy = pt[0].y - ptOrigin[0].y;vec[0].dz = pt[0].z - ptOrigin[0].z;
			vec[1].dx = pt[1].x - ptOrigin[0].x;vec[1].dy = pt[1].y - ptOrigin[0].y;vec[1].dz = pt[1].z - ptOrigin[0].z;
	
			CMath::GetCrossProduct(vecNorm,vec[0],vec[1]);
			if(!CMath::NormalizeVector(vecRotate)) return pRet;
			if(!CMath::NormalizeVector(vecNorm)) return pRet;

			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
			vec[0] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
			vec[1] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
		
			double nStartAngle = CGeometry::GetRotatedAngleInXYPlane(vec[0]);
			double nEndAngle   = CGeometry::GetRotatedAngleInXYPlane(vec[1]);
			double nSweepAngle = nEndAngle - nStartAngle;
			if(nSweepAngle < 0.) nSweepAngle += 2*PI;
			double nStepAngle  = nSweepAngle/m_nSegments;
	
			POINT_T ptEDGE[2][64]={0,};
			double nAngle=nStartAngle;
			POINT_T ptCenter={0};
			for(int i=0;i < m_nSegments;i++)
			{
				ptCenter = CArc3d::on(ptOrigin[0],vecNorm,nTorusRadius,nAngle);
				nAngle  += nStepAngle;
				
				vec[0].dx = ptCenter.x - ptOrigin[0].x;
				vec[0].dy = ptCenter.y - ptOrigin[0].y;
				vec[0].dz = ptCenter.z - ptOrigin[0].z;
				CMath::GetCrossProduct(vecRotate,vecNorm,vec[0]);
				
				CMath::GetCrossProduct(vec[3],vecRotate,vecZ);
				if(!CMath::NormalizeVector(vec[3])) return pRet;
				
				double nDot = CMath::DotProduct(vecRotate,vec[3]);
				vec[3].dx -= nDot*vecRotate.dx;
				vec[3].dy -= nDot*vecRotate.dy;
				vec[3].dz -= nDot*vecRotate.dz;
				
				ptEDGE[0][i].x = ptCenter.x + vec[3].dx*nTubeRadius;
				ptEDGE[0][i].y = ptCenter.y + vec[3].dy*nTubeRadius;
				ptEDGE[0][i].z = ptCenter.z + vec[3].dz*nTubeRadius;
				ptEDGE[1][i].x = ptCenter.x - vec[3].dx*nTubeRadius;
				ptEDGE[1][i].y = ptCenter.y - vec[3].dy*nTubeRadius;
				ptEDGE[1][i].z = ptCenter.z - vec[3].dz*nTubeRadius;
			}
			vec[0].dx = pt[1].x - ptOrigin[0].x;
			vec[0].dy = pt[1].y - ptOrigin[0].y;
			vec[0].dz = pt[1].z - ptOrigin[0].z;
			CMath::GetCrossProduct(vecRotate,vecNorm,vec[0]);
			if(!CMath::NormalizeVector(vecRotate)) return pRet;
	
			CMath::GetCrossProduct(vec[3],vecRotate,vecZ);
			if(!CMath::NormalizeVector(vec[3])) return pRet;
			
			if(fabs(vecRotate.dz - vecZ.dz)	< 0.005)
			{
				vec[3] = vecNorm;
				if(!CMath::NormalizeVector(vec[3])) return pRet;
			}
			else
			{
				CMath::GetCrossProduct(vec[3],vecRotate,vecZ);
				if(!CMath::NormalizeVector(vec[3])) return pRet;
			}
			
			double nDot = CMath::DotProduct(vecRotate,vec[3]);
			vec[3].dx -= nDot*vecRotate.dx;
			vec[3].dy -= nDot*vecRotate.dy;
			vec[3].dz -= nDot*vecRotate.dz;
	
			ptEDGE[0][i].x = pt[1].x + vec[3].dx*nTubeRadius;
			ptEDGE[0][i].y = pt[1].y + vec[3].dy*nTubeRadius;
			ptEDGE[0][i].z = pt[1].z + vec[3].dz*nTubeRadius;
			ptEDGE[1][i].x = pt[1].x - vec[3].dx*nTubeRadius;
			ptEDGE[1][i].y = pt[1].y - vec[3].dy*nTubeRadius;
			ptEDGE[1][i].z = pt[1].z - vec[3].dz*nTubeRadius;

			for(i=0;i < m_nSegments + 1;i++)
			{
				m_ptCenter[i].x = (ptEDGE[0][i].x + ptEDGE[1][i].x)*0.5;
				m_ptCenter[i].y = (ptEDGE[0][i].y + ptEDGE[1][i].y)*0.5;
				m_ptCenter[i].z = (ptEDGE[0][i].z + ptEDGE[1][i].z)*0.5;
				ptEDGE[0][m_nSegments + i + 1] = ptEDGE[1][m_nSegments - i];
			}
			ptEDGE[0][m_nSegments + i + 1] = ptEDGE[0][0];
			
			if(pSide=CHSRFace::CreateFace(m_nSegments + i + 2,ptEDGE[0]))
			{
				pSide->m_enmLayer = LAYER_ELBOW;
				pSide->id     = nID++;
				pSide->type   = ELBOW;
				pSide->ele    = SIDEFACE | HSR_POLYGON_SHAPE;
				pSide->radius = nTubeRadius;
				pSide->ptCenter  = ptCross;
				pSide->ptEdge[0]  = pt[0];
				pSide->ptEdge[1]  = pt[1];

				pSide->mark[0]  = true;
				pSide->ptMark[0]   = pt[0];
				pSide->symbol[0]= false; 
				pSide->mark[1]  = true;
				pSide->ptMark[1]   = pt[1];
				pSide->symbol[1]= false; 

				pSide->pCenterline = CHSRFace::CreateLine(m_nSegments + 1,m_ptCenter);
			}

			vec[0].dx = pt[0].x - ptOrigin[0].x;vec[0].dy = pt[0].y - ptOrigin[0].y;vec[0].dz = pt[0].z - ptOrigin[0].z;
			vec[1].dx = pt[1].x - ptOrigin[0].x;vec[1].dy = pt[1].y - ptOrigin[0].y;vec[1].dz = pt[1].z - ptOrigin[0].z;
			CMath::GetCrossProduct(vecNorm,vec[0],vec[1]);
			CMath::GetCrossProduct(vecRotate,vecNorm,vec[0]);
			if(!CMath::NormalizeVector(vecRotate)) return pRet;

			bool bIsVertical=(fabs(nDot) < 0.005) ? true : false;
			if(!bIsVertical)
			{
				if(pTop = CHSRFace::CreateCircleFace(pt[0],nTubeRadius,vecRotate,m_nSegments))
				{
					pTop->m_enmLayer = LAYER_ELBOW;
					pTop->id     = nID++;
					pTop->type   = ELBOW;
					pTop->ele    = HSR_CIRCLE_SHAPE;
					pTop->ptCenter = pt[0];
					pTop->radius   = nTubeRadius;
				}
			}
			
			vec[0].dx = pt[0].x - ptOrigin[0].x;vec[0].dy = pt[0].y - ptOrigin[0].y;vec[0].dz = pt[0].z - ptOrigin[0].z;
			vec[1].dx = pt[1].x - ptOrigin[0].x;vec[1].dy = pt[1].y - ptOrigin[0].y;vec[1].dz = pt[1].z - ptOrigin[0].z;
			CMath::GetCrossProduct(vecNorm,vec[0],vec[1]);
			CMath::GetCrossProduct(vecRotate,vecNorm,vec[1]);
			if(!CMath::NormalizeVector(vecRotate)) return pRet;
			
			nDot=CMath::DotProduct(vecRotate.dx,vecRotate.dy,vecRotate.dz,vecZ.dx,vecZ.dy,vecZ.dz);
			bIsVertical=(fabs(nDot) < 0.005) ? true : false;
			if(!bIsVertical)
			{
				if(pBottom = CHSRFace::CreateCircleFace(pt[1],nTubeRadius,vecRotate,m_nSegments))
				{
					pBottom->m_enmLayer = LAYER_ELBOW;
					pBottom->id     = nID++;
					pBottom->type   = ELBOW;
					pBottom->ele    = HSR_CIRCLE_SHAPE;
					pBottom->ptCenter = pt[1];
					pBottom->radius   = nTubeRadius;
				}
			}
		}
		
		CHSRApp* pHSRApp=CHSRApp::GetInstance();
		if(pSide) pHSRApp->m_clsPreProcessor.Run(NULL,ELBOW,pTop,pSide,pBottom);
	}

	return pRet;
}
#else
/**	@brief	create outline of BW_ELBOW.

	@param	pViewFmt	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@return	PFACE	
*/
CHSRFace* CElbowElement::CreateBWFace(CHSRViewFormat* pViewFmt,long& nID)
{
	assert(pViewFmt && "pViewFmt is NULL");
	static const double tol=0.001;
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pViewFmt)
	{
		CHSRFace *pTop=NULL,*pSide=NULL,*pBottom=NULL;
		
		POINT_T ptOrigin[3]={0.,};
		ptOrigin[0] = pViewFmt->MODEL2VIEW(m_ptOrigin);

		POINT_T pt[4]={0.,};
		pt[0] = pViewFmt->MODEL2VIEW(m_pt[0]);
		pt[1] = pViewFmt->MODEL2VIEW(m_pt[1]);
		if(pt[0].z > pt[1].z) swap(pt[0],pt[1]);

		POINT_T ptCross={0};
		ptCross = pViewFmt->MODEL2VIEW(m_ptCross);
		const double nTubeRadius =pViewFmt->MODEL2VIEW(m_nTubeRadius);
		const double nTorusRadius=pViewFmt->MODEL2VIEW(m_nTorusRadius);

		VECTOR_T vec[4]={0,};
		vec[0].dx = pt[0].x - ptOrigin[0].x;vec[0].dy = pt[0].y - ptOrigin[0].y;vec[0].dz = pt[0].z - ptOrigin[0].z;
		vec[1].dx = pt[1].x - ptOrigin[0].x;vec[1].dy = pt[1].y - ptOrigin[0].y;vec[1].dz = pt[1].z - ptOrigin[0].z;
		
		VECTOR_T vecNorm={0},vecRotate={0.},vecZ={0.,0.,1.};
		CMath::GetCrossProduct(vecNorm,vec[0],vec[1]);
		CMath::GetCrossProduct(vecRotate,vecNorm,vec[0]);
		if(!CMath::NormalizeVector(vecRotate)) return pRet;
		if(!CMath::NormalizeVector(vecNorm)) return pRet;
		
		double nDot=CMath::DotProduct(vecNorm.dx,vecNorm.dy,vecNorm.dz,vecZ.dx,vecZ.dy,vecZ.dz);
		/// VERTICAL.
		if(fabs(nDot) < CHSRElement::m_tol)
		{
			CMath::GetCrossProduct(vec[2],vecRotate,vec[0]);
			CMath::NormalizeVector(vec[2]);
			
			ptOrigin[1].x = ptOrigin[0].x + vec[2].dx*nTubeRadius;
			ptOrigin[1].y = ptOrigin[0].y + vec[2].dy*nTubeRadius;
			ptOrigin[1].z = ptOrigin[0].z + vec[2].dz*nTubeRadius;
			ptOrigin[2].x = ptOrigin[0].x - vec[2].dx*nTubeRadius;
			ptOrigin[2].y = ptOrigin[0].y - vec[2].dy*nTubeRadius;
			ptOrigin[2].z = ptOrigin[0].z - vec[2].dz*nTubeRadius;
			
			pt[2].x = pt[0].x + vec[2].dx*nTubeRadius;
			pt[2].y = pt[0].y + vec[2].dy*nTubeRadius;
			pt[2].z = pt[0].z + vec[2].dz*nTubeRadius;
			pt[3].x = pt[0].x - vec[2].dx*nTubeRadius;
			pt[3].y = pt[0].y - vec[2].dy*nTubeRadius;
			pt[3].z = pt[0].z - vec[2].dz*nTubeRadius;
			
			VOLUME_T view_volume=pViewFmt->GetViewVolume();
			if(ptCross.z > view_volume.z[1])
			{
				/*
				const double dz=(pt[0].z > pt[1].z ? pt[0].z : pt[1].z) - view_volume.z[1];
				const double dist=sqrt(nTorusRadius*nTorusRadius - dz*dz);
				pt[1].x = ptOrigin[0].x + vec[1].dx*dist;
				pt[1].y = ptOrigin[0].y + vec[1].dy*dist;
				pt[1].z = view_volume.z[1];
				vec[1].dx = pt[1].x - ptOrigin[0].x;vec[1].dy = pt[1].y - ptOrigin[0].y;vec[1].dz = pt[1].z - ptOrigin[0].z;
				*/
			}
			else if(ptCross.z < view_volume.z[0])
			{
				/*
				const double dz= view_volume.z[0] - (pt[0].z > pt[1].z ? pt[0].z : pt[1].z);
				const double dist=sqrt(nTorusRadius*nTorusRadius - dz*dz);
				pt[0].x = ptOrigin[0].x + vec[0].dx*dist;
				pt[0].y = ptOrigin[0].y + vec[0].dy*dist;
				pt[0].z = view_volume.z[0];
				vec[0].dx = pt[0].x - ptOrigin[0].x;vec[0].dy = pt[0].y - ptOrigin[0].y;vec[0].dz = pt[0].z - ptOrigin[0].z;
		
				pt[2].x = pt[0].x + vec[2].dx*nTubeRadius;
				pt[2].y = pt[0].y + vec[2].dy*nTubeRadius;
				pt[2].z = pt[0].z + vec[2].dz*nTubeRadius;
				pt[3].x = pt[0].x - vec[2].dx*nTubeRadius;
				pt[3].y = pt[0].y - vec[2].dy*nTubeRadius;
				pt[3].z = pt[0].z - vec[2].dz*nTubeRadius;
				//nSweepAngle[2] = asin(dz / nTorusRadius);
				//nSweepAngle[2] = RAD2DEG(nSweepAngle[2]);
				*/
			}

			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
			vec[0] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
			vec[1] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
			
			double nSweepAngle[3]={0,};
			nSweepAngle[0] = CGeometry::GetRotatedAngleInXYPlane(vec[0]);
			nSweepAngle[1] = CGeometry::GetRotatedAngleInXYPlane(vec[1]);
			
			nSweepAngle[2] = RAD2DEG(nSweepAngle[1] - nSweepAngle[0]);
			if(nSweepAngle[2] < 0.) nSweepAngle[2] += 360.;

			StoreArcPoints(m_ptEDGE[0],ptOrigin[1],pt[2],vecNorm,nSweepAngle[2]);
			StoreArcPoints(m_ptEDGE[1],ptOrigin[2],pt[3],vecNorm,nSweepAngle[2]);
			m_ptEDGE[0][0] = m_ptEDGE[0][0];
			m_ptEDGE[0][1] = m_ptEDGE[0][m_nSegments];
			m_ptEDGE[0][2] = m_ptEDGE[1][m_nSegments];
			m_ptEDGE[0][3] = m_ptEDGE[1][0];
			m_ptEDGE[0][4] = m_ptEDGE[0][0];
			/*
			m_ptEDGE[0][0].z = (m_ptEDGE[0][0].z + m_ptEDGE[0][1].z) * 0.5;
			m_ptEDGE[0][1].z = m_ptEDGE[0][0].z;
			m_ptEDGE[0][2].z = (m_ptEDGE[0][2].z + m_ptEDGE[0][3].z) * 0.5;
			m_ptEDGE[0][3].z = m_ptEDGE[0][2].z;
			m_ptEDGE[0][4].z = m_ptEDGE[0][2].z;
			*/
			
			if(pSide=CHSRFace::CreateFace(5,m_ptEDGE[0]))
			{
				pSide->id     = nID++;
				pSide->type   = ELBOW;
				pSide->ele    = SIDEFACE | HSR_POLYGON_SHAPE;
				pSide->radius = nTubeRadius;
				pSide->ptCenter  = ptCross;
				pSide->ptEdge[0] = pt[0];
				pSide->ptEdge[1] = pt[1];
				pSide->SetColor(*m_pstrColor);

				m_ptCenter[0].x = (m_ptEDGE[0][0].x + m_ptEDGE[0][3].x)*0.5;
				m_ptCenter[0].y = (m_ptEDGE[0][0].y + m_ptEDGE[0][3].y)*0.5;
				m_ptCenter[0].z = (m_ptEDGE[0][0].z + m_ptEDGE[0][3].z)*0.5;
				m_ptCenter[1].x = (m_ptEDGE[0][1].x + m_ptEDGE[0][2].x)*0.5;
				m_ptCenter[1].y = (m_ptEDGE[0][1].y + m_ptEDGE[0][2].y)*0.5;
				m_ptCenter[1].z = (m_ptEDGE[0][1].z + m_ptEDGE[0][2].z)*0.5;
				pSide->pCenterline = CHSRFace::CreateLine(2,m_ptCenter);
			}

			VECTOR_T vecNorm={0.};
			vecNorm.dx = pt[0].x - ptCross.x;
			vecNorm.dy = pt[0].y - ptCross.y;
			vecNorm.dz = pt[0].z - ptCross.z;
			CMath::NormalizeVector(vecNorm);
			double nDot=CMath::DotProduct(vecNorm,vecZ);
			if(fabs(nDot) > CHSRElement::m_tol)
			{
				if(pTop = CHSRFace::CreateCircleFace(pt[0] , nTubeRadius,vecNorm,m_nSegments))
				{
					pTop->type = ELBOW;
					pTop->id   = nID++;
					if(fabs(fabs(nDot) - 1.) < CHSRElement::m_tol)
						pTop->ele  = HSR_CIRCLE_SHAPE;
					else	pTop->ele  = HSR_POLYGON_SHAPE;
					pTop->ptCenter = pt[0];
					pTop->radius   = nTubeRadius;
					pTop->ptEdge[0]= pt[1];
					pTop->SetColor(*m_pstrColor);

					if((fabs(fabs(nDot) - 1.) < CHSRElement::m_tol) && pSide)
					{
						double dx=pt[0].x - pt[1].x;
						double dy=pt[0].y - pt[1].y;
						double dz=pt[0].z - pt[1].z;
						double nLen= sqrt(dx*dx + dy*dy + dz*dz);

						if(nLen > 0.) pSide->symbol[0]  = true;
					}
				}
			}
			else
			{
				if(pSide) pSide->mark[0] = true;
			}

			vecNorm.dx = pt[1].x - ptCross.x;
			vecNorm.dy = pt[1].y - ptCross.y;
			vecNorm.dz = pt[1].z - ptCross.z;
			CMath::NormalizeVector(vecNorm);
			nDot=CMath::DotProduct(vecNorm,vecZ);
			if((fabs(nDot) > CHSRElement::m_tol))
			{
				if(pBottom = CHSRFace::CreateCircleFace(pt[1] , nTubeRadius,vecNorm,m_nSegments))
				{
					pBottom->type = ELBOW;
					pBottom->id   = nID++;
					if(fabs(fabs(nDot) - 1.) < CHSRElement::m_tol)
						pBottom->ele  = HSR_CIRCLE_SHAPE;
					else	pBottom->ele  = HSR_POLYGON_SHAPE;
					pBottom->ptCenter = pt[1];
					pBottom->radius   = nTubeRadius;
					pBottom->ptEdge[0]= pt[0];
					pBottom->SetColor(*m_pstrColor);

					if((fabs(fabs(nDot) - 1.) < CHSRElement::m_tol) && pSide)
					{
						double dx=pt[0].x - pt[1].x;
						double dy=pt[0].y - pt[1].y;
						double dz=pt[0].z - pt[1].z;
						double nLen= sqrt(dx*dx + dy*dy + dz*dz);

						if(nLen > 0.) pSide->symbol[1]  = true;
					}
				}
			}
			else
			{
				if(pSide) pSide->mark[1] = true;
			}
		/// HORIZONTAL.
		}
		else if(fabs(fabs(nDot) - 1.) < CHSRElement::m_tol)
		{
			CMath::GetCrossProduct(vec[2],vecRotate,vecZ);
			CMath::NormalizeVector(vec[2]);
			
			ptOrigin[1] = ptOrigin[0];
			ptOrigin[2] = ptOrigin[0];
			
			pt[2].x = pt[0].x + vec[2].dx*nTubeRadius;
			pt[2].y = pt[0].y + vec[2].dy*nTubeRadius;
			pt[2].z = pt[0].z + vec[2].dz*nTubeRadius;
			pt[3].x = pt[0].x - vec[2].dx*nTubeRadius;
			pt[3].y = pt[0].y - vec[2].dy*nTubeRadius;
			pt[3].z = pt[0].z - vec[2].dz*nTubeRadius;
			
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
			vec[0] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
			vec[1] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
			
			double nSweepAngle[3]={0,};
			nSweepAngle[0] = CGeometry::GetRotatedAngleInXYPlane(vec[0]);
			nSweepAngle[1] = CGeometry::GetRotatedAngleInXYPlane(vec[1]);
			
			POINT_T ptEDGE[2][64];
			nSweepAngle[2] = RAD2DEG(nSweepAngle[1] - nSweepAngle[0]);
			if(nSweepAngle[2] < 0.) nSweepAngle[2] += 360.;
			StoreArcPoints(ptEDGE[0],ptOrigin[1],pt[2],vecNorm,nSweepAngle[2]);
			StoreArcPoints(ptEDGE[1],ptOrigin[2],pt[3],vecNorm,nSweepAngle[2]);
			int i=0;
			for(i = 0;i < m_nSegments + 1;i++)
			{
				m_ptCenter[i].x = (ptEDGE[0][i].x + ptEDGE[1][i].x)*0.5;
				m_ptCenter[i].y = (ptEDGE[0][i].y + ptEDGE[1][i].y)*0.5;
				m_ptCenter[i].z = (ptEDGE[0][i].z + ptEDGE[1][i].z)*0.5;
				ptEDGE[0][m_nSegments + i + 1] = ptEDGE[1][m_nSegments - i];
			}
			ptEDGE[0][m_nSegments + i + 1] = ptEDGE[0][0];

			if(pSide=CHSRFace::CreateFace(m_nSegments + i + 2,ptEDGE[0]))
			{
				pSide->id     = nID++;
				pSide->type   = ELBOW;
				pSide->ele    = SIDEFACE | HSR_ARC_SHAPE;
				pSide->radius = nTubeRadius;
				pSide->ptCenter  = ptOrigin[0];
				pSide->ptCross   = ptCross;
				pSide->ptEdge[0] = pt[0];
				pSide->ptEdge[1] = pt[1];
				pSide->SetColor(*m_pstrColor);

				pSide->pCenterline = CHSRFace::CreateLine(m_nSegments + 1,m_ptCenter);

				pSide->mark[0]  = true;
				pSide->ptMark[0]= pt[0];
				pSide->symbol[0]= false; 
				pSide->mark[1]  = true;
				pSide->ptMark[1]= pt[1];
				pSide->symbol[1]= false; 
			}
		}
		else
		{	// SLOPE.
			double dx=pt[0].x - ptOrigin[0].x;
			double dy=pt[0].y - ptOrigin[0].y;
			double dz=pt[0].z - ptOrigin[0].z;
			double nTorusRadius=sqrt(dx*dx + dy*dy + dz*dz);
	
			VECTOR_T vec[4]={0,};
			vec[0].dx = pt[0].x - ptOrigin[0].x;vec[0].dy = pt[0].y - ptOrigin[0].y;vec[0].dz = pt[0].z - ptOrigin[0].z;
			vec[1].dx = pt[1].x - ptOrigin[0].x;vec[1].dy = pt[1].y - ptOrigin[0].y;vec[1].dz = pt[1].z - ptOrigin[0].z;
	
			VECTOR_T vecNorm[3]={0},vecRotate={0.},vecZ={0.,0.,1.};
			CMath::GetCrossProduct(vecNorm[0],vec[0],vec[1]);
			CMath::NormalizeVector(vecRotate);
			CMath::NormalizeVector(vecNorm[0]);

			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecNorm[0],alpha,beta);
			vec[0] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
			vec[1] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
		
			double nStartAngle = CGeometry::GetRotatedAngleInXYPlane(vec[0]);
			double nEndAngle   = CGeometry::GetRotatedAngleInXYPlane(vec[1]);
			double nSweepAngle = nEndAngle - nStartAngle;
			if(nSweepAngle < 0.) nSweepAngle += 2*PI;
			double nStepAngle  = nSweepAngle/m_nSegments;
	
			POINT_T ptEDGE[2][64];
			double nAngle=nStartAngle;
			POINT_T ptCenter={0};
			int i=0;
			for(i=0;i < m_nSegments;i++)
			{
				ptCenter = CArc3d::on(ptOrigin[0],vecNorm[0],nTorusRadius,nAngle);
				nAngle  += nStepAngle;
				
				vec[0].dx = ptCenter.x - ptOrigin[0].x;
				vec[0].dy = ptCenter.y - ptOrigin[0].y;
				vec[0].dz = ptCenter.z - ptOrigin[0].z;
				CMath::GetCrossProduct(vecRotate,vecNorm[0],vec[0]);
				
				CMath::GetCrossProduct(vec[3],vecRotate,vecZ);
				CMath::NormalizeVector(vec[3]);
				
				double nDot = CMath::DotProduct(vecRotate,vec[3]);
				vec[3].dx -= nDot*vecRotate.dx;
				vec[3].dy -= nDot*vecRotate.dy;
				vec[3].dz -= nDot*vecRotate.dz;
				
				ptEDGE[0][i].x = ptCenter.x + vec[3].dx*nTubeRadius;
				ptEDGE[0][i].y = ptCenter.y + vec[3].dy*nTubeRadius;
				ptEDGE[0][i].z = ptCenter.z + vec[3].dz*nTubeRadius;
				ptEDGE[1][i].x = ptCenter.x - vec[3].dx*nTubeRadius;
				ptEDGE[1][i].y = ptCenter.y - vec[3].dy*nTubeRadius;
				ptEDGE[1][i].z = ptCenter.z - vec[3].dz*nTubeRadius;
			}
			vec[0].dx = pt[1].x - ptOrigin[0].x;
			vec[0].dy = pt[1].y - ptOrigin[0].y;
			vec[0].dz = pt[1].z - ptOrigin[0].z;
			CMath::GetCrossProduct(vecRotate,vecNorm[0],vec[0]);
			CMath::NormalizeVector(vecRotate);
	
			CMath::GetCrossProduct(vec[3],vecRotate,vecZ);
			CMath::NormalizeVector(vec[3]);
			
			if(fabs(vecRotate.dz - vecZ.dz)	< 0.005)
			{
				vec[3] = vecNorm[0];
				CMath::NormalizeVector(vec[3]);
			}
			else
			{
				CMath::GetCrossProduct(vec[3],vecRotate,vecZ);
				CMath::NormalizeVector(vec[3]);
			}
			
			double nDot = CMath::DotProduct(vecRotate,vec[3]);
			vec[3].dx -= nDot*vecRotate.dx;
			vec[3].dy -= nDot*vecRotate.dy;
			vec[3].dz -= nDot*vecRotate.dz;
	
			ptEDGE[0][i].x = pt[1].x + vec[3].dx*nTubeRadius;
			ptEDGE[0][i].y = pt[1].y + vec[3].dy*nTubeRadius;
			ptEDGE[0][i].z = pt[1].z + vec[3].dz*nTubeRadius;
			ptEDGE[1][i].x = pt[1].x - vec[3].dx*nTubeRadius;
			ptEDGE[1][i].y = pt[1].y - vec[3].dy*nTubeRadius;
			ptEDGE[1][i].z = pt[1].z - vec[3].dz*nTubeRadius;

			for(i=0;i < m_nSegments + 1;i++)
			{
				m_ptCenter[i].x = (ptEDGE[0][i].x + ptEDGE[1][i].x)*0.5;
				m_ptCenter[i].y = (ptEDGE[0][i].y + ptEDGE[1][i].y)*0.5;
				m_ptCenter[i].z = (ptEDGE[0][i].z + ptEDGE[1][i].z)*0.5;
				ptEDGE[0][m_nSegments + i + 1] = ptEDGE[1][m_nSegments - i];
			}
			ptEDGE[0][m_nSegments + i + 1] = ptEDGE[0][0];
			
			if(pSide=CHSRFace::CreateFace(m_nSegments + i + 2,ptEDGE[0]))
			{
				pSide->id     = nID++;
				pSide->type   = ELBOW;
				pSide->ele    = SIDEFACE | HSR_POLYGON_SHAPE;
				pSide->radius = nTubeRadius;
				pSide->ptCenter  = ptCross;
				pSide->ptEdge[0]  = pt[0];
				pSide->ptEdge[1]  = pt[1];
				pSide->SetColor(*m_pstrColor);

				pSide->mark[0]  = true;
				pSide->ptMark[0]   = pt[0];
				pSide->symbol[0]= false; 
				pSide->mark[1]  = true;
				pSide->ptMark[1]   = pt[1];
				pSide->symbol[1]= false; 

				pSide->pCenterline = CHSRFace::CreateLine(m_nSegments + 1,m_ptCenter);
			}

			vec[0].dx = pt[0].x - ptOrigin[0].x;vec[0].dy = pt[0].y - ptOrigin[0].y;vec[0].dz = pt[0].z - ptOrigin[0].z;
			vec[1].dx = pt[1].x - ptOrigin[0].x;vec[1].dy = pt[1].y - ptOrigin[0].y;vec[1].dz = pt[1].z - ptOrigin[0].z;
			CMath::GetCrossProduct(vecNorm[0],vec[0],vec[1]);
			CMath::GetCrossProduct(vecRotate,vecNorm[0],vec[0]);
			CMath::NormalizeVector(vecRotate);
			bool bIsVertical=(fabs(nDot) < 0.005) ? true : false;
			if(!bIsVertical)
			{
				if(pTop = CHSRFace::CreateCircleFace(pt[0],nTubeRadius,vecRotate,m_nSegments))
				{
					pTop->id     = nID++;
					pTop->type   = ELBOW;
					pTop->ele    = HSR_POLYGON_SHAPE;
					pTop->ptCenter = pt[0];
					pTop->radius   = nTubeRadius;
					pTop->SetColor(*m_pstrColor);
				}
			}
			
			vec[0].dx = pt[0].x - ptOrigin[0].x;vec[0].dy = pt[0].y - ptOrigin[0].y;vec[0].dz = pt[0].z - ptOrigin[0].z;
			vec[1].dx = pt[1].x - ptOrigin[0].x;vec[1].dy = pt[1].y - ptOrigin[0].y;vec[1].dz = pt[1].z - ptOrigin[0].z;		
			CMath::GetCrossProduct(vecNorm[0],vec[0],vec[1]);
			CMath::GetCrossProduct(vecRotate,vecNorm[0],vec[1]);
			CMath::NormalizeVector(vecRotate);
			nDot=CMath::DotProduct(vecRotate.dx,vecRotate.dy,vecRotate.dz,vecZ.dx,vecZ.dy,vecZ.dz);
			bIsVertical=(fabs(nDot) < 0.005) ? true : false;
			if(!bIsVertical)
			{
				if(pBottom = CHSRFace::CreateCircleFace(pt[1],nTubeRadius,vecRotate,m_nSegments))
				{
					pBottom->id     = nID++;
					pBottom->type   = ELBOW;
					pBottom->ele    = HSR_POLYGON_SHAPE;
					pBottom->ptCenter = pt[1];
					pBottom->radius   = nTubeRadius;
					pBottom->SetColor(*m_pstrColor);
				}
			}
		}
		
		if(pSide) pHSRApp->m_clsPreProcessor.Run(NULL,ELBOW,pTop,pSide,pBottom);
	}

	return pRet;
}
#endif

/**	\brief	The CElbowElement::CreateSWFace function\n
	create outline of SW_ELBOW.

	\param	pViewFmt	pointer of view
	\param	nID	id of face

	\remarks\n
	;2003.03.29 - only 90 degree elbow has circle mark.

	\return	PFACE	
*/
CHSRFace* CElbowElement::CreateSWFace(CHSRViewFormat* pViewFmt,long& nID)
{
	assert(pViewFmt && "pViewFmt is NULL");
	CHSRFace* pRet=NULL;

	if(pViewFmt)
	{
		CHSRFace *pTop=NULL,*pSide=NULL,*pBottom=NULL;
		
		POINT_T ptOrigin[3]={0.,};
		ptOrigin[0] = pViewFmt->MODEL2VIEW(m_ptOrigin);

		POINT_T pt[4]={0.,},ptMark[2]={0,};
		pt[0] = pViewFmt->MODEL2VIEW(m_pt[0]);
		pt[1] = pViewFmt->MODEL2VIEW(m_pt[1]);
		ptMark[0] = pViewFmt->MODEL2VIEW(m_ptPIPE[0][1]);
		ptMark[1] = pViewFmt->MODEL2VIEW(m_ptPIPE[1][1]);
		const POINT_T ptCross=pViewFmt->MODEL2VIEW(m_ptCross);
		const double nTubeRadius =pViewFmt->MODEL2VIEW(m_nTubeRadius);
		const double nTorusRadius=pViewFmt->MODEL2VIEW(m_nTorusRadius);

		VECTOR_T vec[4]={0,};
		vec[0].dx = pt[0].x - ptOrigin[0].x;vec[0].dy = pt[0].y - ptOrigin[0].y;vec[0].dz = pt[0].z - ptOrigin[0].z;
		vec[1].dx = pt[1].x - ptOrigin[0].x;vec[1].dy = pt[1].y - ptOrigin[0].y;vec[1].dz = pt[1].z - ptOrigin[0].z;
		
		VECTOR_T vecZ={0.,0.,1.};
		CVect vecRotate;
		CVect vecNorm;
		vecNorm =  CMath::CrossProduct(vec[0],vec[1]);
		vecRotate= CMath::CrossProduct(vecNorm,vec[0]);
		if(!CMath::NormalizeVector(vecRotate)) return pRet;
		if(!CMath::NormalizeVector(vecNorm)) return pRet;
		
		double nDot=CMath::DotProduct(vecNorm,vecZ);
		if(fabs(nDot) < CHSRElement::m_tol) /// VERTICAL ELBOW
		{
			double zhigh = (pt[0].z > pt[1].z) ? pt[0].z : pt[1].z;
			POINT_T ptVertices[4]={0.,};
			
			VOLUME_T view_volume=pViewFmt->GetViewVolume();
			if(ptCross.z > view_volume.z[1])
			{				
				VECTOR_T vecTmp={pt[1].x - pt[0].x,pt[1].y - pt[0].y,pt[1].z - pt[0].z};
				const double z_ratio=(view_volume.z[1] - pt[0].z)/vecTmp.dz;
				vecTmp.dx *= z_ratio;vecTmp.dy *= z_ratio;vecTmp.dz *= z_ratio;

				ptVertices[0].x = pt[0].x + vecNorm.dx*nTubeRadius;
				ptVertices[0].y = pt[0].y + vecNorm.dy*nTubeRadius;
				ptVertices[0].z = pt[0].z;
				ptVertices[1].x = pt[0].x + vecTmp.dx + vecNorm.dx*nTubeRadius;
				ptVertices[1].y = pt[0].y + vecTmp.dy + vecNorm.dy*nTubeRadius;
				ptVertices[1].z = pt[0].z + vecTmp.dz;
				ptVertices[2].x = pt[0].x + vecTmp.dx - vecNorm.dx*nTubeRadius;
				ptVertices[2].y = pt[0].y + vecTmp.dy - vecNorm.dy*nTubeRadius;
				ptVertices[2].z = pt[0].z + vecTmp.dz;
				ptVertices[3].x = pt[0].x - vecNorm.dx*nTubeRadius;
				ptVertices[3].y = pt[0].y - vecNorm.dy*nTubeRadius;
				ptVertices[3].z = pt[0].z;
			}
			else if(ptCross.z < view_volume.z[0])
			{
				VECTOR_T vecTmp={pt[0].x - pt[1].x,pt[0].y - pt[1].y,pt[0].z - pt[1].z};
				const double z_ratio=(view_volume.z[0] - pt[1].z)/vecTmp.dz;
				vecTmp.dx *= z_ratio;vecTmp.dy *= z_ratio;vecTmp.dz *= z_ratio;

				ptVertices[0].x = pt[1].x + vecNorm.dx*nTubeRadius;
				ptVertices[0].y = pt[1].y + vecNorm.dy*nTubeRadius;
				ptVertices[0].z = pt[1].z;
				ptVertices[1].x = pt[1].x + vecTmp.dx + vecNorm.dx*nTubeRadius;
				ptVertices[1].y = pt[1].y + vecTmp.dy + vecNorm.dy*nTubeRadius;
				ptVertices[1].z = pt[1].z + vecTmp.dz;
				ptVertices[2].x = pt[1].x + vecTmp.dx - vecNorm.dx*nTubeRadius;
				ptVertices[2].y = pt[1].y + vecTmp.dy - vecNorm.dy*nTubeRadius;
				ptVertices[2].z = pt[1].z + vecTmp.dz;
				ptVertices[3].x = pt[1].x - vecNorm.dx*nTubeRadius;
				ptVertices[3].y = pt[1].y - vecNorm.dy*nTubeRadius;
				ptVertices[3].z = pt[1].z;
			}
			else
			{
				const double z = (pt[0].z > pt[1].z) ? pt[0].z : pt[1].z;

				ptVertices[0].x = pt[0].x + vecNorm.dx*nTubeRadius;
				ptVertices[0].y = pt[0].y + vecNorm.dy*nTubeRadius;
				ptVertices[0].z = z;
				ptVertices[1].x = pt[1].x + vecNorm.dx*nTubeRadius;
				ptVertices[1].y = pt[1].y + vecNorm.dy*nTubeRadius;
				ptVertices[1].z = z;
				ptVertices[2].x = pt[1].x - vecNorm.dx*nTubeRadius;
				ptVertices[2].y = pt[1].y - vecNorm.dy*nTubeRadius;
				ptVertices[2].z = z;
				ptVertices[3].x = pt[0].x - vecNorm.dx*nTubeRadius;
				ptVertices[3].y = pt[0].y - vecNorm.dy*nTubeRadius;
				ptVertices[3].z = z;
			}

			m_ptEDGE[0][0] = ptVertices[0];
			CVect vecDir(	ptVertices[2].x - ptVertices[1].x,
					ptVertices[2].y - ptVertices[1].y,
					ptVertices[2].z - ptVertices[1].z);
			for(int i=0;i < 11;i++)
			{
				m_ptEDGE[0][i + 1].x = ptVertices[1].x + vecDir.dx*(i*0.1);
				m_ptEDGE[0][i + 1].y = ptVertices[1].y + vecDir.dy*(i*0.1);
				m_ptEDGE[0][i + 1].z = ptVertices[1].z + vecDir.dz*(i*0.1);
			}
			vecDir.dx = ptVertices[0].x - ptVertices[3].x;
			vecDir.dy = ptVertices[0].y - ptVertices[3].y;
			vecDir.dz = ptVertices[0].z - ptVertices[3].z;
			for(int i = 0;i < 11;i++)
			{
				m_ptEDGE[0][i + 12].x = ptVertices[3].x + vecDir.dx*(i*0.1);
				m_ptEDGE[0][i + 12].y = ptVertices[3].y + vecDir.dy*(i*0.1);
				m_ptEDGE[0][i + 12].z = ptVertices[3].z + vecDir.dz*(i*0.1);
			}

			m_ptEDGE[0][0] = ptVertices[0];
			m_ptEDGE[0][1] = ptVertices[1];
			m_ptEDGE[0][2] = ptVertices[2];
			m_ptEDGE[0][3] = ptVertices[3];
			m_ptEDGE[0][4] = ptVertices[0];

			if(pSide=CHSRFace::CreateFace(5,m_ptEDGE[0]))
			{
#ifdef VER_03
				pSide->m_enmLayer = LAYER_ELBOW;
#endif
				pSide->id    = nID++;
				pSide->type  = SW_ELBOW;
				pSide->ele   = SIDEFACE | HSR_POLYGON_SHAPE;
				pSide->radius= nTubeRadius;
				pSide->ptEdge[0]= pt[0];
				pSide->ptEdge[1]= pt[1];
				pSide->ptCenter = ptCross;
				pSide->ptCross  = ptCross;
				pSide->SetColor(*m_pstrColor);
	
				m_ptCenter[0].x = (m_ptEDGE[0][1].x + m_ptEDGE[0][2].x)*0.5;
				m_ptCenter[0].y = (m_ptEDGE[0][1].y + m_ptEDGE[0][2].y)*0.5;
				m_ptCenter[0].z = (m_ptEDGE[0][1].z + m_ptEDGE[0][2].z)*0.5;
				m_ptCenter[1].x = (m_ptEDGE[0][0].x + m_ptEDGE[0][3].x)*0.5;
				m_ptCenter[1].y = (m_ptEDGE[0][0].y + m_ptEDGE[0][3].y)*0.5;
				m_ptCenter[1].z = (m_ptEDGE[0][0].z + m_ptEDGE[0][3].z)*0.5;
				pSide->pCenterline = CHSRFace::CreateLine(2,m_ptCenter);
				
				int nCount=0;
				for(PHSR_VERTEX ptr=pSide->pHead;ptr;ptr = ptr->next,nCount++)
				{
					ptr->render = SHOW;
				}
			}
		
			vecNorm.dx = pt[0].x - ptCross.x;
			vecNorm.dy = pt[0].y - ptCross.y;
			vecNorm.dz = pt[0].z - ptCross.z;
			if(!CMath::NormalizeVector(vecNorm)) return pRet;
			double nDot=CMath::DotProduct(vecNorm,vecZ);
			if(fabs(nDot) > CHSRElement::m_tol)
			{
				pt[2] = pViewFmt->MODEL2VIEW(m_ptPIPE[0][0]);
				pt[3] = pViewFmt->MODEL2VIEW(m_ptPIPE[0][1]);
				double zTop=(pt[2].z > pt[3].z) ? pt[2].z : pt[3].z;
				pt[2].x = pt[0].x;pt[2].y = pt[0].y;pt[2].z = zTop;
				double nTubeRadiusEx=pViewFmt->MODEL2VIEW(m_nRadius[0]);
				if(pTop = CHSRFace::CreateCircleFace(pt[2],nTubeRadius,vecNorm,m_nSegments))
				{
#ifdef VER_03
					pTop->m_enmLayer = LAYER_ELBOW;
#endif
					pTop->type  = SW_ELBOW;
					pTop->id    = nID++;
					if(fabs(fabs(nDot) - 1.) < CHSRElement::m_tol)
						pTop->ele   = HSR_CIRCLE_SHAPE;
					else	pTop->ele   = HSR_POLYGON_SHAPE;
					
					pTop->radius= nTubeRadius;
					pTop->ptCenter = pt[2];
					pTop->ptEdge[0]= pt[1];
					pTop->ptEdge[0].z = zTop;
					pTop->SetColor(*m_pstrColor);
					
					if((fabs(fabs(nDot) - 1.) < CHSRElement::m_tol) && pSide)
					{
						double dx=pt[2].x - pt[1].x;
						double dy=pt[2].y - pt[1].y;
						double dz=pt[2].z - pt[1].z;
						double nLen= sqrt(dx*dx + dy*dy + dz*dz);

						if(nLen > 0.001) pSide->symbol[0]  = true;
					}
				}
			}
			else
			{
				if(pSide)
				{
					pSide->mark[0]  = true;
					pSide->ptMark[0]= ptMark[0];
					pSide->symbol[0]= false;
				}
			}
			
			vecNorm.dx = pt[1].x - ptCross.x;
			vecNorm.dy = pt[1].y - ptCross.y;
			vecNorm.dz = pt[1].z - ptCross.z;
			if(!CMath::NormalizeVector(vecNorm)) return pRet;
			nDot=CMath::DotProduct(vecNorm,vecZ);
			if(fabs(nDot) > CHSRElement::m_tol)
			{
				pt[2] = pViewFmt->MODEL2VIEW(m_ptPIPE[1][0]);
				pt[3] = pViewFmt->MODEL2VIEW(m_ptPIPE[1][1]);
				double zTop=(pt[2].z > pt[3].z) ? pt[2].z : pt[3].z;
				pt[2].x = pt[1].x;pt[2].y = pt[1].y;pt[2].z = zTop;
				double nTubeRadiusEx=pViewFmt->MODEL2VIEW(m_nRadius[1]);
				if(pBottom = CHSRFace::CreateCircleFace(pt[2],nTubeRadius,vecNorm,m_nSegments))
				{
#ifdef VER_03
					pBottom->m_enmLayer = LAYER_ELBOW;
#endif
					pBottom->type  = SW_ELBOW;
					pBottom->id    = nID++;
					if(fabs(fabs(nDot) - 1.) < CHSRElement::m_tol)
						pBottom->ele   = HSR_CIRCLE_SHAPE;
					else	pBottom->ele   = HSR_POLYGON_SHAPE;
					
					pBottom->radius   = nTubeRadius;
					pBottom->ptCenter = pt[2];
					pBottom->ptEdge[0]= pt[0];
					pBottom->ptEdge[0].z = zTop;
					pBottom->SetColor(*m_pstrColor);
					
					if((fabs(fabs(nDot) - 1.) < 0.001) && pSide)
					{
						double dx=pt[2].x - pt[0].x;
						double dy=pt[2].y - pt[0].y;
						double dz=pt[2].z - pt[0].z;
						double nLen= sqrt(dx*dx + dy*dy + dz*dz);

						if(nLen > 0.001) pSide->symbol[1]  = true;
					}
				}
			}
			else
			{
				if(pSide)
				{
					pSide->mark[1]  = true;
					pSide->ptMark[1]= ptMark[1];
					pSide->symbol[1]= false;
				}
			}
		}
		else if(fabs(fabs(nDot) - 1.) < CHSRElement::m_tol)
		{// X-Y plane(PLANE)
			CMath::GetCrossProduct(vec[2],vecRotate,vecZ);
			if(!CMath::NormalizeVector(vec[2])) return pRet;
			
			ptOrigin[1] = ptOrigin[0];
			ptOrigin[2] = ptOrigin[0];
			
			pt[2].x = pt[0].x + vec[2].dx*nTubeRadius;
			pt[2].y = pt[0].y + vec[2].dy*nTubeRadius;
			pt[2].z = pt[0].z + vec[2].dz*nTubeRadius;
			pt[3].x = pt[0].x - vec[2].dx*nTubeRadius;
			pt[3].y = pt[0].y - vec[2].dy*nTubeRadius;
			pt[3].z = pt[0].z - vec[2].dz*nTubeRadius;
			
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
			vec[0] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
			vec[1] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
			
			double nSweepAngle[3]={0,};
			nSweepAngle[0] = CGeometry::GetRotatedAngleInXYPlane(vec[0]);
			nSweepAngle[1] = CGeometry::GetRotatedAngleInXYPlane(vec[1]);
			nSweepAngle[2] = RAD2DEG(nSweepAngle[1] - nSweepAngle[0]);
			if(nSweepAngle[2] < 0.) nSweepAngle[2] += 360.;

			StoreArcPoints(m_ptEDGE[0],ptOrigin[1],pt[2],vecNorm,nSweepAngle[2]);
			StoreArcPoints(m_ptEDGE[1],ptOrigin[2],pt[3],vecNorm,nSweepAngle[2]);
			int i = 0;
			for(i = 0;i < m_nSegments + 1;i++)
			{
				m_ptCenter[i].x = (m_ptEDGE[0][i].x + m_ptEDGE[1][i].x)*0.5;
				m_ptCenter[i].y = (m_ptEDGE[0][i].y + m_ptEDGE[1][i].y)*0.5;
				m_ptCenter[i].z = (m_ptEDGE[0][i].z + m_ptEDGE[1][i].z)*0.5;
				m_ptEDGE[0][m_nSegments + i + 1] = m_ptEDGE[1][m_nSegments - i];
			}
			m_ptEDGE[0][m_nSegments + i + 1] = m_ptEDGE[0][0];

			if(pSide=CHSRFace::CreateFace(m_nSegments + i + 2,m_ptEDGE[0]))
			{
#ifdef VER_03
				pSide->m_enmLayer = LAYER_ELBOW;
#endif				
				pSide->id     = nID++;
				pSide->type   = SW_ELBOW;
				pSide->ele    = SIDEFACE | HSR_ARC_SHAPE;
				pSide->radius = nTubeRadius;
				pSide->ptCenter = ptOrigin[0];
				pSide->ptCross  = ptCross;
				pSide->ptEdge[0]= pt[0];
				pSide->ptEdge[1]= pt[1];
				pSide->SetColor(*m_pstrColor);
				
				pSide->pCenterline = CHSRFace::CreateLine(m_nSegments + 1,m_ptCenter);

				pSide->mark[0]   = true;
				pSide->ptMark[0] = ptMark[0];
				pSide->mark[1]   = true;
				pSide->ptMark[1] = ptMark[1];
			}
		}
		else
		{	/// SLOPE.
			double dx=pt[0].x - ptOrigin[0].x;
			double dy=pt[0].y - ptOrigin[0].y;
			double dz=pt[0].z - ptOrigin[0].z;
			double nTorusRadius=sqrt(dx*dx + dy*dy + dz*dz);
	
			VECTOR_T vec[4]={0,};
			vec[0].dx = pt[0].x - ptOrigin[0].x;vec[0].dy = pt[0].y - ptOrigin[0].y;vec[0].dz = pt[0].z - ptOrigin[0].z;
			vec[1].dx = pt[1].x - ptOrigin[0].x;vec[1].dy = pt[1].y - ptOrigin[0].y;vec[1].dz = pt[1].z - ptOrigin[0].z;
	
			VECTOR_T vecZ={0.,0.,1.};
			double alpha=0.,beta=0.;
			CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
			vec[0] = CGeometry::RotateAboutXYAxis(vec[0],alpha,beta);
			vec[1] = CGeometry::RotateAboutXYAxis(vec[1],alpha,beta);
			vec[3] = vecNorm;
			vec[3].dz = 0.;
			if(!CMath::NormalizeVector(vec[3])) return pRet;
		
			double nStartAngle = CGeometry::GetRotatedAngleInXYPlane(vec[0]);
			double nEndAngle   = CGeometry::GetRotatedAngleInXYPlane(vec[1]);
			double nSweepAngle = nEndAngle - nStartAngle;
			if(nSweepAngle < 0.) nSweepAngle += 2*PI;
			double nStepAngle  = nSweepAngle/m_nSegments;
	
			CVect vecDir;
			POINT_T ptEDGE[2][64];
			double nAngle=nStartAngle;
			int i = 0;
			for(i = 0;i < m_nSegments + 1;i++,nAngle += nStepAngle)
			{
				m_ptCenter[i] = CArc3d::on(ptOrigin[0],vecNorm,nTorusRadius,nAngle);
				vecDir.dx = m_ptCenter[i].x - ptOrigin[0].x;
				vecDir.dy = m_ptCenter[i].y - ptOrigin[0].y;
				vecDir.dz = m_ptCenter[i].z - ptOrigin[0].z;
				vecDir = vecNorm * vecDir;
				vecDir = vecDir.Perpendicular();
				vecDir.Normalize();

				ptEDGE[0][i].x = m_ptCenter[i].x + vecDir.dx*nTubeRadius;
				ptEDGE[0][i].y = m_ptCenter[i].y + vecDir.dy*nTubeRadius;
				ptEDGE[0][i].z = m_ptCenter[i].z + vecDir.dz*nTubeRadius;
				ptEDGE[1][i].x = m_ptCenter[i].x - vecDir.dx*nTubeRadius;
				ptEDGE[1][i].y = m_ptCenter[i].y - vecDir.dy*nTubeRadius;
				ptEDGE[1][i].z = m_ptCenter[i].z - vecDir.dz*nTubeRadius;
			}
			//--> the last point
			vecDir.dx = pt[1].x - ptOrigin[0].x;
			vecDir.dy = pt[1].y - ptOrigin[0].y;
			vecDir.dz = pt[1].z - ptOrigin[0].z;
			vecDir = vecNorm * vecDir;
			vecDir = vecDir.Perpendicular();
			vecDir.Normalize();
			
			ptEDGE[0][i].x = pt[1].x + vecDir.dx*nTubeRadius;
			ptEDGE[0][i].y = pt[1].y + vecDir.dy*nTubeRadius;
			ptEDGE[0][i].z = pt[1].z + vecDir.dz*nTubeRadius;
			ptEDGE[1][i].x = pt[1].x - vecDir.dx*nTubeRadius;
			ptEDGE[1][i].y = pt[1].y - vecDir.dy*nTubeRadius;
			ptEDGE[1][i].z = pt[1].z - vecDir.dz*nTubeRadius;
			
			for(i=0;i < m_nSegments + 1;i++)
			{
				ptEDGE[0][m_nSegments + i + 1] = ptEDGE[1][m_nSegments - i];
			}
			ptEDGE[0][m_nSegments + i + 1] = ptEDGE[0][0];
			
			if(pSide=CHSRFace::CreateFace(m_nSegments + i + 2,ptEDGE[0]))
			{
#ifdef VER_03
				pSide->m_enmLayer = LAYER_ELBOW;
#endif
				pSide->id     = nID++;
				pSide->type   = SW_ELBOW;
				pSide->ele    = SIDEFACE;
				pSide->radius = nTubeRadius;
				pSide->ptCenter = ptCross;
				pSide->ptEdge[0]= pt[0];
				pSide->ptEdge[1]= pt[1];
				pSide->SetColor(*m_pstrColor);

				POINT_T pt[2]={0.,};
				pt[0] = pViewFmt->MODEL2VIEW(m_ptPIPE[0][0]);
				pt[1] = pViewFmt->MODEL2VIEW(m_ptPIPE[1][0]);
				pSide->mark[0]   = true;
				pSide->ptMark[0] = ptMark[0];
				pSide->mark[1]   = true;
				pSide->ptMark[1] = ptMark[1];

				pSide->pCenterline = CHSRFace::CreateLine(m_nSegments + 1,m_ptCenter);

				PHSR_VERTEX ptr = NULL;
				for(PHSR_VERTEX ptr = pSide->pHead;ptr && ptr->next && ptr->next->next;ptr = ptr->next);
				if(ptr) ptr->render = INVALID;
			}

			CMath::GetCrossProduct(vecRotate,vecNorm,vec[0]);
			if(!CMath::NormalizeVector(vecRotate)) return pRet;
			bool bIsVertical=(fabs(nDot) < 0.005) ? true : false;
			if(!bIsVertical)
			{
				if(pTop = CHSRFace::CreateCircleFace(pt[0],nTubeRadius,vecRotate,m_nSegments))
				{
#ifdef VER_03
					pTop->m_enmLayer = LAYER_ELBOW;
#endif
					pTop->id     = nID++;
					pTop->type   = SW_ELBOW;
					pTop->ele    = HSR_CIRCLE_SHAPE;
					pTop->ptCenter = pt[0];
					pTop->radius   = nTubeRadius;
					pTop->SetColor(*m_pstrColor);
				}
			}
			
			CMath::GetCrossProduct(vecRotate,vecNorm,vec[1]);
			if(!CMath::NormalizeVector(vecRotate)) return pRet;
			nDot=CMath::DotProduct(vecRotate.dx,vecRotate.dy,vecRotate.dz,vecZ.dx,vecZ.dy,vecZ.dz);
			bIsVertical=(fabs(nDot) < 0.005) ? true : false;
			if(!bIsVertical)
			{
				if(pBottom = CHSRFace::CreateCircleFace(pt[1],nTubeRadius,vecRotate,m_nSegments))
				{
#ifdef VER_03
					pBottom->m_enmLayer = LAYER_ELBOW;
#endif
					pBottom->id     = nID++;
					pBottom->type   = SW_ELBOW;
					pBottom->ele    = HSR_CIRCLE_SHAPE;
					pBottom->ptCenter = pt[1];
					pBottom->radius   = nTubeRadius;
					pBottom->SetColor(*m_pstrColor);
				}
			}
		}

		CHSRApp* pHSRApp=CHSRApp::GetInstance();
		if(pSide) pHSRApp->m_clsPreProcessor.Run(NULL,SW_ELBOW,pTop,pSide,pBottom);
		CreateSWPIPE(pViewFmt,nID);
	}

	return pRet;
}

/**	\brief	The CElbowElement::CreateSWPIPE function

	\param	pViewFmt	a parameter of type CPreViewFormat*
	\param	nID	a parameter of type long&

	\return	void	
*/
void CElbowElement::CreateSWPIPE(CHSRViewFormat* pViewFmt,long& nID)
{
	assert(pViewFmt && "pViewFmt is NULL");

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pViewFmt)
	{
		static POINT_T ptEDGE[36];
		CHSRFace *pTop=NULL,*pSide=NULL,*pBottom=NULL;
		
		POINT_T ptPipe[2]={0.,};
		double  nRadius=0.,nTubeRadius=pViewFmt->MODEL2VIEW(m_nTubeRadius);
		for(int i=0;i < 2;i++)
		{	
			pTop = pSide = pBottom = NULL;

			ptPipe[0] = pViewFmt->MODEL2VIEW(m_ptPIPE[i][0]);
			ptPipe[1] = pViewFmt->MODEL2VIEW(m_ptPIPE[i][1]);
			nRadius   = pViewFmt->MODEL2VIEW(m_nRadius[i]);
			
			VECTOR_T vec={0.},vecZ={0.,0.,1.};
			vec.dx = ptPipe[1].x - ptPipe[0].x;
			vec.dy = ptPipe[1].y - ptPipe[0].y;
			vec.dz = ptPipe[1].z - ptPipe[0].z;
			if(!CMath::NormalizeVector(vec)) return;
			double nDot=CMath::DotProduct(vec,vecZ);
			
			// VERTICAL PIPE - create only circle.
			if(fabs(fabs(nDot) - 1) < CHSRElement::m_tol)
			{
				POINT_T ptOrigin = ptPipe[0];
				ptOrigin.z = (ptPipe[0].z + ptPipe[1].z) * 0.5;
				if(pTop=CHSRFace::CreateCircleFace(ptOrigin,nRadius,vec,m_nSegments))
				{
#ifdef VER_03
					pTop->m_enmLayer = LAYER_ELBOW;
#endif
					pTop->type  = SW_ELBOW;
					pTop->ele   = TOPFACEX;
					pTop->id    = nID++;
					pTop->radius= nTubeRadius;
					pTop->SetColor(*m_pstrColor);

					pHSRApp->AppendFace(NULL,pTop);
				}
			}
			else if(fabs(nDot) < CHSRElement::m_tol)	/// HORIZONTAL
			{
				VECTOR_T vecCross={0,};
				CMath::GetCrossProduct(vecCross,vec,vecZ);
				if(!CMath::NormalizeVector(vecCross)) return;
				
				ptEDGE[0].x = ptPipe[0].x + vecCross.dx*nRadius;
				ptEDGE[0].y = ptPipe[0].y + vecCross.dy*nRadius;
				ptEDGE[0].z = (ptPipe[0].z + nRadius) + vecCross.dz*nRadius;
				ptEDGE[1].x = ptPipe[0].x - vecCross.dx*nRadius;
				ptEDGE[1].y = ptPipe[0].y - vecCross.dy*nRadius;
				ptEDGE[1].z = (ptPipe[0].z + nRadius) - vecCross.dz*nRadius;
				ptEDGE[2].x = ptPipe[1].x - vecCross.dx*nRadius;
				ptEDGE[2].y = ptPipe[1].y - vecCross.dy*nRadius;
				ptEDGE[2].z = (ptPipe[1].z + nRadius) - vecCross.dz*nRadius;
				ptEDGE[3].x = ptPipe[1].x + vecCross.dx*nRadius;
				ptEDGE[3].y = ptPipe[1].y + vecCross.dy*nRadius;
				ptEDGE[3].z = (ptPipe[1].z + nRadius) + vecCross.dz*nRadius;
				ptEDGE[4]   = ptEDGE[0];
				
				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
					double nTubeRadius = pViewFmt->MODEL2VIEW(m_nTubeRadius);
#ifdef VER_03
					pSide->m_enmLayer = LAYER_ELBOW;
#endif
					pSide->type  = SW_ELBOW;
					pSide->ele   = SIDEFACEX | HSR_POLYGON_SHAPE;
					pSide->id    = nID++;
					pSide->radius= nTubeRadius;
					pSide->ptEdge[0] = ptPipe[0];
					pSide->ptEdge[1] = ptPipe[1];
					pSide->mark[0] = true;
					pSide->mark[1] = true;
					pSide->SetColor(*m_pstrColor);

					pSide->pCenterline = CHSRFace::CreateLine(2,ptPipe);
					
					pHSRApp->AppendFace(NULL,pSide);
				}
			}
			else
			{
				if(pTop=CHSRFace::CreateCircleFace(ptPipe[0],nRadius,vec,m_nSegments))
				{
#ifdef VER_03
					pTop->m_enmLayer = LAYER_ELBOW;
#endif
					pTop->type    = SW_ELBOW;
					pTop->id      = nID++;
					pTop->ele     = TOPFACEX;
					pTop->ptCenter= ptPipe[0];
					pTop->radius  = nRadius;
					pTop->SetColor(*m_pstrColor);

					pHSRApp->AppendFace(NULL,pTop);
				}

				if(pBottom=CHSRFace::CreateCircleFace(ptPipe[1],nRadius,vec,m_nSegments))
				{
#ifdef VER_03
					pBottom->m_enmLayer = LAYER_ELBOW;
#endif
					pBottom->type    = SW_ELBOW;
					pBottom->id      = nID++;
					pBottom->ele     = BOTTOMFACEX;
					pBottom->ptCenter= ptPipe[1];
					pBottom->radius  = nRadius;
					pBottom->SetColor(*m_pstrColor);

					pHSRApp->AppendFace(NULL,pBottom);
				}

				VECTOR_T vecCross={0,};
				CMath::GetCrossProduct(vecCross,vec,vecZ);
				if(!CMath::NormalizeVector(vecCross)) return;
				
				ptEDGE[0].x = ptPipe[0].x + vecCross.dx*nRadius;
				ptEDGE[0].y = ptPipe[0].y + vecCross.dy*nRadius;
				ptEDGE[0].z = (ptPipe[0].z + nRadius) + vecCross.dz*nRadius;
				ptEDGE[1].x = ptPipe[0].x - vecCross.dx*nRadius;
				ptEDGE[1].y = ptPipe[0].y - vecCross.dy*nRadius;
				ptEDGE[1].z = (ptPipe[0].z + nRadius) - vecCross.dz*nRadius;
				ptEDGE[2].x = ptPipe[1].x - vecCross.dx*nRadius;
				ptEDGE[2].y = ptPipe[1].y - vecCross.dy*nRadius;
				ptEDGE[2].z = (ptPipe[1].z + nRadius) - vecCross.dz*nRadius;
				ptEDGE[3].x = ptPipe[1].x + vecCross.dx*nRadius;
				ptEDGE[3].y = ptPipe[1].y + vecCross.dy*nRadius;
				ptEDGE[3].z = (ptPipe[1].z + nRadius) + vecCross.dz*nRadius;
				ptEDGE[4].x = ptPipe[0].x + vecCross.dx*nRadius;
				ptEDGE[4].y = ptPipe[0].y + vecCross.dy*nRadius;
				ptEDGE[4].z = (ptPipe[0].z + nRadius) + vecCross.dz*nRadius;
				
				if(pSide=CHSRFace::CreateFace(5,ptEDGE))
				{
					if(pTop)
					{
						if(pSide->pHead) pSide->pHead->render = INVALID;
					}
					if(pBottom)
					{
						if(pSide->pHead && pSide->pHead->next && pSide->pHead->next->next)
							pSide->pHead->next->next->render = INVALID;
					}

#ifdef VER_03
					pSide->m_enmLayer = LAYER_ELBOW;
#endif					
					pSide->type  = SW_ELBOW;
					pSide->ele   = SIDEFACEX;
					pSide->id    = nID++;
					pSide->radius= nRadius;
					pSide->ptEdge[0] = ptPipe[0];
					pSide->ptEdge[1] = ptPipe[1];
					pSide->mark[0]   = true;
					pSide->mark[1]   = true;
					pSide->SetColor(*m_pstrColor);
					
					pSide->pCenterline = CHSRFace::CreateLine(2,ptPipe);

					pHSRApp->AppendFace(NULL,pSide);
				}
			}
		}
	}
}

/**	\brief	The CElbowElement::CreateCapFace function

	\param	pViewFmt	a parameter of type CPreViewFormat*
	\param	ptOrigin	a parameter of type POINT_T
	\param	vecNorm	a parameter of type VECTOR_T
	\param	nRadius	a parameter of type double

	\return	PFACE	
*/
CHSRFace* CElbowElement::CreateCapFace(CHSRViewFormat* pViewFmt,POINT_T ptOrigin,VECTOR_T vecNorm,double nRadius)
{
	assert(pViewFmt && "pViewFmt is NULL");
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pViewFmt && pHSRApp)
	{
		static POINT_T ptEDGE[36];

		VECTOR_T vecZ={0.,0.,1.};
		double nDot=CMath::DotProduct(vecNorm,vecZ);
		if(fabs(fabs(nDot) - 1.) > 0.001)
		{
			double alpha=0.,beta=0;
			CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
			
			double nStep = 2*PI/m_nSegments;
			
			int i=0;
			for(double t=0;t < 2*PI;t += nStep,i++)
			{
				ptEDGE[i].x = nRadius*cos(t);
				ptEDGE[i].y = nRadius*sin(t);
				ptEDGE[i].z = 0.;
				ptEDGE[i] = CGeometry::RotateAboutYXAxis(ptEDGE[i],-beta,-alpha);
				ptEDGE[i].x += ptOrigin.x;
				ptEDGE[i].y += ptOrigin.y;
				ptEDGE[i].z += ptOrigin.z;
			}
			ptEDGE[i].x = nRadius*cos(2*PI);
			ptEDGE[i].y = nRadius*sin(2*PI);
			ptEDGE[i].z = 0.;
			ptEDGE[i]   = CGeometry::RotateAboutYXAxis(ptEDGE[i],-beta,-alpha);
			ptEDGE[i].x += ptOrigin.x;
			ptEDGE[i].y += ptOrigin.y;
			ptEDGE[i].z += ptOrigin.z;
			
			pRet=CHSRFace::CreateFace(i + 1,ptEDGE);
			if(pRet)
			{
				pRet->CalcPlaneEquation();
				pRet->radius= nRadius;
				pRet->SetColor(*m_pstrColor);
			}
		}
	}

	return pRet;
}

/**	\brief	The CElbowElement::StoreArcPoints function

	\param	ptEDGE[]	a parameter of type POINT_T
	\param	ptOrigin	a parameter of type POINT_T
	\param	ptStart	a parameter of type POINT_T
	\param	vecNorm	a parameter of type VECTOR_T
	\param	nSweep	a parameter of type double

	\return	void	
*/
void CElbowElement::StoreArcPoints(POINT_T ptEDGE[],POINT_T ptOrigin,POINT_T ptStart,VECTOR_T vecNorm,double nSweep)
{
	double alpha=0.,beta=0;
	CGeometry::GetRotatedAngleOfAxis(vecNorm,alpha,beta);
	
	POINT_T ptDelta={0};
	ptDelta.x=ptStart.x - ptOrigin.x;
	ptDelta.y=ptStart.y - ptOrigin.y;
	ptDelta.z=ptStart.z - ptOrigin.z;
	double nRadius=sqrt(ptDelta.x*ptDelta.x + ptDelta.y*ptDelta.y + ptDelta.z*ptDelta.z);

	ptDelta = CGeometry::RotateAboutXYAxis(ptDelta,alpha,beta);
	double nStartAngle = CGeometry::GetRotatedAngleInXYPlane(ptDelta);
	double nEndAngle   = nStartAngle + DEG2RAD(nSweep);
	double nStep = (nEndAngle - nStartAngle)/m_nSegments;
			
	int i=0;
	for(double t=nStartAngle;t < nEndAngle;t += nStep,i++)
	{
		ptEDGE[i].x = nRadius*cos(t);
		ptEDGE[i].y = nRadius*sin(t);
		ptEDGE[i].z = 0.;
		ptEDGE[i] = CGeometry::RotateAboutYXAxis(ptEDGE[i],-beta,-alpha);
		ptEDGE[i].x += ptOrigin.x;
		ptEDGE[i].y += ptOrigin.y;
		ptEDGE[i].z += ptOrigin.z;
	}
	ptEDGE[i].x = nRadius*cos(nEndAngle);
	ptEDGE[i].y = nRadius*sin(nEndAngle);
	ptEDGE[i].z = 0.;
	ptEDGE[i] = CGeometry::RotateAboutYXAxis(ptEDGE[i],-beta,-alpha);
	ptEDGE[i].x += ptOrigin.x;
	ptEDGE[i].y += ptOrigin.y;
	ptEDGE[i].z += ptOrigin.z;
}

/**
 * SetPosition:
 *
 * @param ptStart 
 * @param ptEnd 
 * @return void 
 */
void CElbowElement::SetPosition(POINT_T ptStart, POINT_T ptEnd)
{
	m_pt[0] = ptStart;
	m_pt[1] = ptEnd;
}

/**
 * SetCrossPoint:
 *
 * @param ptCross 
 * @return void 
 */
void CElbowElement::SetCrossPoint(POINT_T ptCross)
{
	m_ptCross = ptCross;
}

/**
	@brief	

	@author	BHK

	@date	2009.09.25
*/
int CElbowElement::Copy(CHSRElement* pCopied)
{
	assert(pCopied && "pCopied is NULL");

	if(pCopied)
	{
		CHSRElement::Copy(pCopied);

		if(pCopied->IsKindOf(CElbowElement::TypeString()))
		{
			CElbowElement* pElbo = static_cast<CElbowElement*>(pCopied);

			m_eType = pElbo->m_eType;
			m_pt[0] = pElbo->m_pt[0];
			m_pt[1] = pElbo->m_pt[1];
			m_ptCross = pElbo->m_ptCross;
			m_ptOrigin= pElbo->m_ptOrigin;
			m_nTorusRadius = pElbo->m_nTorusRadius;
			m_nTubeRadius  = pElbo->m_nTubeRadius;
			
			m_nRadius[0] = pElbo->m_nRadius[0];
			m_nRadius[1] = pElbo->m_nRadius[1];

			m_ptPIPE[0][0] = pElbo->m_ptPIPE[0][0];
			m_ptPIPE[0][1] = pElbo->m_ptPIPE[0][1];
			m_ptPIPE[1][0] = pElbo->m_ptPIPE[1][0];
			m_ptPIPE[1][1] = pElbo->m_ptPIPE[1][1];
			
			for(int i = 0;i < 64;++i)
			{
				m_ptEDGE[0][i] = pElbo->m_ptEDGE[0][i];
				m_ptEDGE[1][i] = pElbo->m_ptEDGE[1][i];

				m_ptCenter[i] = pElbo->m_ptCenter[i];
			}
			
			m_vol = pElbo->m_vol;
		}
		
		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}