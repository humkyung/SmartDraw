// BOXElement.cpp: implementation of the CBoxElement class.
//

//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math\ITKMath.h>
#include <graphics\geometry.h>
#include "SmartHSR.h"
#include "BOXElement.h"

IMPLEMENTS_HSR_ELEMENT(CBoxElement , CHSRElement , _T("__BOX_HSR_ELEMENT__"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**	\brief
*/
CBoxElement::CBoxElement()
{
	m_vol.x[0] = m_vol.x[1] = 0.;
	m_vol.y[0] = m_vol.y[1] = 0.;
	m_vol.z[0] = m_vol.z[1] = 0.;
}

/**	\brief
*/
CBoxElement::~CBoxElement()
{
}

#ifdef VER_03
/**	
	@brief	The CBoxElement::ParseLine function

	@param	pHSRScanner	a parameter of type CHSRScanner*

	@remarks\n
	;2003-10-27 - error in checking routine to get m_vecDist

	@return	bool	
*/
bool CBoxElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;
	
	if(pHSRScanner)
	{
		char* pVal=NULL;
		
		if(pVal = pHSRScanner->Val("layer")) (*m_pstrLayer) = pVal;
		
		if(pVal = pHSRScanner->Val("width1")) m_nLen[0] = ATOF_T(pVal);
		if(pVal = pHSRScanner->Val("height1")) m_nLen[1] = ATOF_T(pVal);
		if(pVal = pHSRScanner->Val("width2")) m_nLen[2] = ATOF_T(pVal);
		if(pVal = pHSRScanner->Val("height2")) m_nLen[3] = ATOF_T(pVal);
		
		if(pVal = pHSRScanner->Val("pt1")){
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[0].x),&(m_pt[0].y),&(m_pt[0].z));
		}
		if(pVal = pHSRScanner->Val("pt2")){
			sscanf(pVal,"%lf,%lf,%lf",&(m_pt[1].x),&(m_pt[1].y),&(m_pt[1].z));
		}
		if(pVal = pHSRScanner->Val("vec1")){
			sscanf(pVal,"%lf,%lf,%lf",&(m_vec.dx),&(m_vec.dy),&(m_vec.dz));
		}
		
		if(pHSRScanner->m_nFactor > 8){
			if(pVal = pHSRScanner->Val("vec2")){
				sscanf(pVal,"%lf,%lf,%lf",&(m_vecDist.dx),&(m_vecDist.dy),&(m_vecDist.dz));
			}
		}else{
			m_vecDist.dx = 0.;
			m_vecDist.dy = 0.;
			m_vecDist.dz = 0.;
		}
		
		if(m_nLen[3] < 0.001)
			m_eType = BOX_TRIANGLE;
		else if(fabs(m_nLen[1] - m_nLen[3]) > 1.) 
			m_eType = BOX_PHYRAMID;
		else if(fabs(m_nLen[0] - m_nLen[2]) > 1.)
			m_eType = BOX_PHYRAMID;
		else	m_eType = BOX_RECTANGLE;
		
		Volume();
		
		bRet = true;
	}
	
	return bRet;
}
#else
/**	
	@brief	The CBoxElement::ParseLine function
	
	@author	humkyung

	@param	pHSRScanner	a parameter of type CHSRScanner*

	@remarks\n
	;2003-10-27 - error in checking routine to get m_vecDist

	@return	bool	
*/
bool CBoxElement::ParseLine(CHSRScanner* pHSRScanner)
{
	assert(pHSRScanner && "pHSRScanner is NULL");
	bool bRet=false;

	if(pHSRScanner)
	{
		m_nLen[0] = ATOF_T((*pHSRScanner->m_aryFactor)[1].c_str());
		m_nLen[1] = ATOF_T((*pHSRScanner->m_aryFactor)[2].c_str());
		m_nLen[2] = ATOF_T((*pHSRScanner->m_aryFactor)[3].c_str());
		m_nLen[3] = ATOF_T((*pHSRScanner->m_aryFactor)[4].c_str());

		m_pt[0].x = ATOF_T((*pHSRScanner->m_aryFactor)[5].c_str());
		m_pt[0].y = ATOF_T((*pHSRScanner->m_aryFactor)[6].c_str());
		m_pt[0].z = ATOF_T((*pHSRScanner->m_aryFactor)[7].c_str());

		m_pt[1].x = ATOF_T((*pHSRScanner->m_aryFactor)[8].c_str());
		m_pt[1].y = ATOF_T((*pHSRScanner->m_aryFactor)[9].c_str());
		m_pt[1].z = ATOF_T((*pHSRScanner->m_aryFactor)[10].c_str());

		m_vec.dx  = ATOF_T((*pHSRScanner->m_aryFactor)[11].c_str());
		m_vec.dy  = ATOF_T((*pHSRScanner->m_aryFactor)[12].c_str());
		m_vec.dz  = ATOF_T((*pHSRScanner->m_aryFactor)[13].c_str());

		if(pHSRScanner->m_nFactor > 14)
		{
			m_vecDist.dx = ATOF_T((*pHSRScanner->m_aryFactor)[14].c_str());
			m_vecDist.dy = ATOF_T((*pHSRScanner->m_aryFactor)[15].c_str());
			m_vecDist.dz = ATOF_T((*pHSRScanner->m_aryFactor)[16].c_str());
		}
		else
		{
			m_vecDist.dx = 0.;
			m_vecDist.dy = 0.;
			m_vecDist.dz = 0.;
		}
		(*m_pstrLayer) = CHSRApp::m_strCurLayer;

		if((m_nLen[2] < CHSRElement::m_tol) || (m_nLen[3] < CHSRElement::m_tol))
			m_eType = BOX_TRIANGLE;
		else if(fabs(m_nLen[1] - m_nLen[3]) > 1.) 
			m_eType = BOX_PHYRAMID;
		else if(fabs(m_nLen[0] - m_nLen[2]) > 1.)
			m_eType = BOX_PHYRAMID;
		else	m_eType = BOX_RECTANGLE;

		Volume();
		bRet = true;
	}

	return bRet;
}
#endif

/**	\brief	The CBoxElement::Volume function


	\return	void	
*/
void CBoxElement::Volume()
{
	VECTOR_T vecAxis=
	{	m_pt[1].x + m_vecDist.dx - m_pt[0].x,
		m_pt[1].y + m_vecDist.dy - m_pt[0].y,
		m_pt[1].z + m_vecDist.dz - m_pt[0].z
	};

	VECTOR_T vecCross=CMath::CrossProduct(m_vec,vecAxis);
	CMath::NormalizeVector(vecCross);

	POINT_T ptEDGE[8]={0,};
	ptEDGE[0].x = m_pt[0].x - m_vec.dx*m_nLen[0]*0.5 - vecCross.dx*m_nLen[1]*0.5;
	ptEDGE[0].y = m_pt[0].y - m_vec.dy*m_nLen[0]*0.5 - vecCross.dy*m_nLen[1]*0.5;
	ptEDGE[0].z = m_pt[0].z - m_vec.dz*m_nLen[0]*0.5 - vecCross.dz*m_nLen[1]*0.5;
	ptEDGE[1].x = m_pt[0].x - m_vec.dx*m_nLen[0]*0.5 + vecCross.dx*m_nLen[1]*0.5;
	ptEDGE[1].y = m_pt[0].y - m_vec.dy*m_nLen[0]*0.5 + vecCross.dy*m_nLen[1]*0.5;
	ptEDGE[1].z = m_pt[0].z - m_vec.dz*m_nLen[0]*0.5 + vecCross.dz*m_nLen[1]*0.5;
	ptEDGE[2].x = m_pt[0].x + m_vec.dx*m_nLen[0]*0.5 + vecCross.dx*m_nLen[1]*0.5;
	ptEDGE[2].y = m_pt[0].y + m_vec.dy*m_nLen[0]*0.5 + vecCross.dy*m_nLen[1]*0.5;
	ptEDGE[2].z = m_pt[0].z + m_vec.dz*m_nLen[0]*0.5 + vecCross.dz*m_nLen[1]*0.5;
	ptEDGE[3].x = m_pt[0].x + m_vec.dx*m_nLen[0]*0.5 - vecCross.dx*m_nLen[1]*0.5;
	ptEDGE[3].y = m_pt[0].y + m_vec.dy*m_nLen[0]*0.5 - vecCross.dy*m_nLen[1]*0.5;
	ptEDGE[3].z = m_pt[0].z + m_vec.dz*m_nLen[0]*0.5 - vecCross.dz*m_nLen[1]*0.5;
	ptEDGE[4].x = m_pt[1].x - m_vec.dx*m_nLen[2]*0.5 - vecCross.dx*m_nLen[3]*0.5;
	ptEDGE[4].y = m_pt[1].y - m_vec.dy*m_nLen[2]*0.5 - vecCross.dy*m_nLen[3]*0.5;
	ptEDGE[4].z = m_pt[1].z - m_vec.dz*m_nLen[2]*0.5 - vecCross.dz*m_nLen[3]*0.5;
	ptEDGE[5].x = m_pt[1].x - m_vec.dx*m_nLen[2]*0.5 + vecCross.dx*m_nLen[3]*0.5;
	ptEDGE[5].y = m_pt[1].y - m_vec.dy*m_nLen[2]*0.5 + vecCross.dy*m_nLen[3]*0.5;
	ptEDGE[5].z = m_pt[1].z - m_vec.dz*m_nLen[2]*0.5 + vecCross.dz*m_nLen[3]*0.5;
	ptEDGE[6].x = m_pt[1].x + m_vec.dx*m_nLen[2]*0.5 + vecCross.dx*m_nLen[3]*0.5;
	ptEDGE[6].y = m_pt[1].y + m_vec.dy*m_nLen[2]*0.5 + vecCross.dy*m_nLen[3]*0.5;
	ptEDGE[6].z = m_pt[1].z + m_vec.dz*m_nLen[2]*0.5 + vecCross.dz*m_nLen[3]*0.5;
	ptEDGE[7].x = m_pt[1].x + m_vec.dx*m_nLen[2]*0.5 - vecCross.dx*m_nLen[3]*0.5;
	ptEDGE[7].y = m_pt[1].y + m_vec.dy*m_nLen[2]*0.5 - vecCross.dy*m_nLen[3]*0.5;
	ptEDGE[7].z = m_pt[1].z + m_vec.dz*m_nLen[2]*0.5 - vecCross.dz*m_nLen[3]*0.5;

	m_vol.x[0] = m_vol.x[1] = ptEDGE[0].x;
	m_vol.y[0] = m_vol.y[1] = ptEDGE[0].y;
	m_vol.z[0] = m_vol.z[1] = ptEDGE[0].z;

	for(int i=1;i < 8;i++)
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
	@brief	The CBoxElement::IsInVolume function

	@author	humkyung

	@param	volume	a parameter of type const VOLUME_T

	@return	bool	
*/
bool CBoxElement::IsInVolume(const CIsVolume& volume)
{
	if((m_vol.x[0] < volume.minx()) && (m_vol.x[1] > volume.maxx()) &&
	   (m_vol.y[0] < volume.miny()) && (m_vol.y[1] > volume.maxy()) &&
	   (m_vol.z[0] < volume.minz()) && (m_vol.z[1] > volume.maxz())) return false;

	if(	(m_vol.x[1] < volume.minx()) || (m_vol.x[0] > volume.maxx()) ||
		(m_vol.y[1] < volume.miny()) || (m_vol.y[0] > volume.maxy()) ||
		(m_vol.z[1] < volume.minz()) || (m_vol.z[0] > volume.maxz())) return false;

	return true;
}

/**	
	@brief	BOX ELEMENT의 형상을 생성한다.
	
	@author	humkyung

	@param	pViewFmt	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@return	PFACE	
*/
CHSRFace* CBoxElement::CreateFace(CHSRViewFormat* pViewFmt,long& nID)
{
	assert(pViewFmt && "pViewFmt is NULL");
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	const long body = nID;	/// 2012.01.13 added by humkyung
	if(pViewFmt)
	{
		typedef struct
		{
			int nCount;
			int aryRow[5];
			int aryCol[5];
		}INDEX_TABLE_T;

		POINT_T ptEDGE[3][5];
		INDEX_TABLE_T IndexTable[5]={0,};
		if((BOX_RECTANGLE == m_eType))
		{
			VECTOR_T vec={0.};
			vec.dx = (m_pt[1].x + m_vecDist.dx) - m_pt[0].x;
			vec.dy = (m_pt[1].y + m_vecDist.dy) - m_pt[0].y;
			vec.dz = (m_pt[1].z + m_vecDist.dz) - m_pt[0].z;
			if(!CMath::NormalizeVector(vec)) return pRet;

			VECTOR_T vecCross = CMath::CrossProduct(vec , m_vec);
			bool b = CMath::NormalizeVector(vecCross);
			assert(true == b);

			//POINT_T ptCenter={	(m_pt[0].x + m_pt[1].x)*0.5,
			//			(m_pt[0].y + m_pt[1].y)*0.5,
			//			(m_pt[0].z + m_pt[1].z)*0.5};

			ptEDGE[0][0].x = m_pt[0].x + m_vec.dx*m_nLen[0]*0.5 - vecCross.dx*m_nLen[1]*0.5;
			ptEDGE[0][0].y = m_pt[0].y + m_vec.dy*m_nLen[0]*0.5 - vecCross.dy*m_nLen[1]*0.5;
			ptEDGE[0][0].z = m_pt[0].z + m_vec.dz*m_nLen[0]*0.5 - vecCross.dz*m_nLen[1]*0.5;
			ptEDGE[0][1].x = m_pt[0].x - m_vec.dx*m_nLen[0]*0.5 - vecCross.dx*m_nLen[1]*0.5;
			ptEDGE[0][1].y = m_pt[0].y - m_vec.dy*m_nLen[0]*0.5 - vecCross.dy*m_nLen[1]*0.5;
			ptEDGE[0][1].z = m_pt[0].z - m_vec.dz*m_nLen[0]*0.5 - vecCross.dz*m_nLen[1]*0.5;
			ptEDGE[0][2].x = m_pt[0].x - m_vec.dx*m_nLen[0]*0.5 + vecCross.dx*m_nLen[1]*0.5;
			ptEDGE[0][2].y = m_pt[0].y - m_vec.dy*m_nLen[0]*0.5 + vecCross.dy*m_nLen[1]*0.5;
			ptEDGE[0][2].z = m_pt[0].z - m_vec.dz*m_nLen[0]*0.5 + vecCross.dz*m_nLen[1]*0.5;
			ptEDGE[0][3].x = m_pt[0].x + m_vec.dx*m_nLen[0]*0.5 + vecCross.dx*m_nLen[1]*0.5;
			ptEDGE[0][3].y = m_pt[0].y + m_vec.dy*m_nLen[0]*0.5 + vecCross.dy*m_nLen[1]*0.5;
			ptEDGE[0][3].z = m_pt[0].z + m_vec.dz*m_nLen[0]*0.5 + vecCross.dz*m_nLen[1]*0.5;

			ptEDGE[1][0].x = m_pt[1].x + m_vec.dx*m_nLen[2]*0.5 - vecCross.dx*m_nLen[3]*0.5;
			ptEDGE[1][0].y = m_pt[1].y + m_vec.dy*m_nLen[2]*0.5 - vecCross.dy*m_nLen[3]*0.5;
			ptEDGE[1][0].z = m_pt[1].z + m_vec.dz*m_nLen[2]*0.5 - vecCross.dz*m_nLen[3]*0.5;
			ptEDGE[1][1].x = m_pt[1].x - m_vec.dx*m_nLen[2]*0.5 - vecCross.dx*m_nLen[3]*0.5;
			ptEDGE[1][1].y = m_pt[1].y - m_vec.dy*m_nLen[2]*0.5 - vecCross.dy*m_nLen[3]*0.5;
			ptEDGE[1][1].z = m_pt[1].z - m_vec.dz*m_nLen[2]*0.5 - vecCross.dz*m_nLen[3]*0.5;
			ptEDGE[1][2].x = m_pt[1].x - m_vec.dx*m_nLen[2]*0.5 + vecCross.dx*m_nLen[3]*0.5;
			ptEDGE[1][2].y = m_pt[1].y - m_vec.dy*m_nLen[2]*0.5 + vecCross.dy*m_nLen[3]*0.5;
			ptEDGE[1][2].z = m_pt[1].z - m_vec.dz*m_nLen[2]*0.5 + vecCross.dz*m_nLen[3]*0.5;
			ptEDGE[1][3].x = m_pt[1].x + m_vec.dx*m_nLen[2]*0.5 + vecCross.dx*m_nLen[3]*0.5;
			ptEDGE[1][3].y = m_pt[1].y + m_vec.dy*m_nLen[2]*0.5 + vecCross.dy*m_nLen[3]*0.5;
			ptEDGE[1][3].z = m_pt[1].z + m_vec.dz*m_nLen[2]*0.5 + vecCross.dz*m_nLen[3]*0.5;

			IndexTable[0].nCount = 5;
			IndexTable[0].aryRow[0]=0;IndexTable[0].aryCol[0]=0;
			IndexTable[0].aryRow[1]=0;IndexTable[0].aryCol[1]=1;
			IndexTable[0].aryRow[2]=0;IndexTable[0].aryCol[2]=2;
			IndexTable[0].aryRow[3]=0;IndexTable[0].aryCol[3]=3;
			IndexTable[0].aryRow[4]=0;IndexTable[0].aryCol[4]=0;

			IndexTable[1].nCount = 5;
			IndexTable[1].aryRow[0]=0;IndexTable[1].aryCol[0]=1;
			IndexTable[1].aryRow[1]=1;IndexTable[1].aryCol[1]=1;
			IndexTable[1].aryRow[2]=1;IndexTable[1].aryCol[2]=2;
			IndexTable[1].aryRow[3]=0;IndexTable[1].aryCol[3]=2;
			IndexTable[1].aryRow[4]=0;IndexTable[1].aryCol[4]=1;

			IndexTable[2].nCount = 5;
			IndexTable[2].aryRow[0]=1;IndexTable[2].aryCol[0]=0;
			IndexTable[2].aryRow[1]=1;IndexTable[2].aryCol[1]=1;
			IndexTable[2].aryRow[2]=1;IndexTable[2].aryCol[2]=2;
			IndexTable[2].aryRow[3]=1;IndexTable[2].aryCol[3]=3;
			IndexTable[2].aryRow[4]=1;IndexTable[2].aryCol[4]=0;

			IndexTable[3].nCount = 5;
			IndexTable[3].aryRow[0]=0;IndexTable[3].aryCol[0]=0;
			IndexTable[3].aryRow[1]=1;IndexTable[3].aryCol[1]=0;
			IndexTable[3].aryRow[2]=1;IndexTable[3].aryCol[2]=3;
			IndexTable[3].aryRow[3]=0;IndexTable[3].aryCol[3]=3;
			IndexTable[3].aryRow[4]=0;IndexTable[3].aryCol[4]=0;

			IndexTable[4].nCount = 5;
			IndexTable[4].aryRow[0]=0;IndexTable[4].aryCol[0]=2;
			IndexTable[4].aryRow[1]=1;IndexTable[4].aryCol[1]=2;
			IndexTable[4].aryRow[2]=1;IndexTable[4].aryCol[2]=3;
			IndexTable[4].aryRow[3]=0;IndexTable[4].aryCol[3]=3;
			IndexTable[4].aryRow[4]=0;IndexTable[4].aryCol[4]=2;

			const size_t nCount = sizeof(IndexTable) / sizeof(IndexTable[0]);
			for(int i = 0;i < int(nCount);++i)
			{
				for(int j = 0;j < IndexTable[i].nCount;++j)
				{
					ptEDGE[2][j] = ptEDGE[IndexTable[i].aryRow[j]][IndexTable[i].aryCol[j]];
					ptEDGE[2][j] = pViewFmt->MODEL2VIEW(ptEDGE[2][j]);
				}

				CHSRFace* pFace=CHSRFace::CreateFace(IndexTable[i].nCount , ptEDGE[2]);
				if(pFace)
				{
					pFace->id  = nID++;
					pFace->m_enmLayer = GetHSRLayer();
					pFace->type= BOX;
					pFace->ele = HSR_RECTANGLE_SHAPE;
					pFace->body = this;	/// 2012.01.13 added by humkyung
					pFace->SetColor(*m_pstrColor);

					pHSRApp->AppendFace(NULL,pFace);
				}
			}
		}
		else if(BOX_PHYRAMID == m_eType)
		{
			VECTOR_T vec={0.};
			vec.dx = (m_pt[1].x + m_vecDist.dx) - m_pt[0].x;
			vec.dy = (m_pt[1].y + m_vecDist.dy) - m_pt[0].y;
			vec.dz = (m_pt[1].z + m_vecDist.dz) - m_pt[0].z;
			bool b = CMath::NormalizeVector(vec);
			assert(true == b);

			VECTOR_T vecCross = CMath::CrossProduct(vec,m_vec);
			b = CMath::NormalizeVector(vecCross);
			assert(true == b);

			POINT_T ptCenter=
			{
				(m_pt[0].x + m_pt[1].x)*0.5,
				(m_pt[0].y + m_pt[1].y)*0.5,
				(m_pt[0].z + m_pt[1].z)*0.5
			};

			ptEDGE[0][0].x = m_pt[0].x + m_vec.dx*m_nLen[0]*0.5 - vecCross.dx*m_nLen[1]*0.5;
			ptEDGE[0][0].y = m_pt[0].y + m_vec.dy*m_nLen[0]*0.5 - vecCross.dy*m_nLen[1]*0.5;
			ptEDGE[0][0].z = m_pt[0].z + m_vec.dz*m_nLen[0]*0.5 - vecCross.dz*m_nLen[1]*0.5;
			ptEDGE[0][1].x = m_pt[0].x - m_vec.dx*m_nLen[0]*0.5 - vecCross.dx*m_nLen[1]*0.5;
			ptEDGE[0][1].y = m_pt[0].y - m_vec.dy*m_nLen[0]*0.5 - vecCross.dy*m_nLen[1]*0.5;
			ptEDGE[0][1].z = m_pt[0].z - m_vec.dz*m_nLen[0]*0.5 - vecCross.dz*m_nLen[1]*0.5;
			ptEDGE[0][2].x = m_pt[0].x - m_vec.dx*m_nLen[0]*0.5 + vecCross.dx*m_nLen[1]*0.5;
			ptEDGE[0][2].y = m_pt[0].y - m_vec.dy*m_nLen[0]*0.5 + vecCross.dy*m_nLen[1]*0.5;
			ptEDGE[0][2].z = m_pt[0].z - m_vec.dz*m_nLen[0]*0.5 + vecCross.dz*m_nLen[1]*0.5;
			ptEDGE[0][3].x = m_pt[0].x + m_vec.dx*m_nLen[0]*0.5 + vecCross.dx*m_nLen[1]*0.5;
			ptEDGE[0][3].y = m_pt[0].y + m_vec.dy*m_nLen[0]*0.5 + vecCross.dy*m_nLen[1]*0.5;
			ptEDGE[0][3].z = m_pt[0].z + m_vec.dz*m_nLen[0]*0.5 + vecCross.dz*m_nLen[1]*0.5;

			ptEDGE[1][0].x = m_pt[1].x + m_vec.dx*m_nLen[2]*0.5 - vecCross.dx*m_nLen[3]*0.5;
			ptEDGE[1][0].y = m_pt[1].y + m_vec.dy*m_nLen[2]*0.5 - vecCross.dy*m_nLen[3]*0.5;
			ptEDGE[1][0].z = m_pt[1].z + m_vec.dz*m_nLen[2]*0.5 - vecCross.dz*m_nLen[3]*0.5;
			ptEDGE[1][1].x = m_pt[1].x - m_vec.dx*m_nLen[2]*0.5 - vecCross.dx*m_nLen[3]*0.5;
			ptEDGE[1][1].y = m_pt[1].y - m_vec.dy*m_nLen[2]*0.5 - vecCross.dy*m_nLen[3]*0.5;
			ptEDGE[1][1].z = m_pt[1].z - m_vec.dz*m_nLen[2]*0.5 - vecCross.dz*m_nLen[3]*0.5;
			ptEDGE[1][2].x = m_pt[1].x - m_vec.dx*m_nLen[2]*0.5 + vecCross.dx*m_nLen[3]*0.5;
			ptEDGE[1][2].y = m_pt[1].y - m_vec.dy*m_nLen[2]*0.5 + vecCross.dy*m_nLen[3]*0.5;
			ptEDGE[1][2].z = m_pt[1].z - m_vec.dz*m_nLen[2]*0.5 + vecCross.dz*m_nLen[3]*0.5;
			ptEDGE[1][3].x = m_pt[1].x + m_vec.dx*m_nLen[2]*0.5 + vecCross.dx*m_nLen[3]*0.5;
			ptEDGE[1][3].y = m_pt[1].y + m_vec.dy*m_nLen[2]*0.5 + vecCross.dy*m_nLen[3]*0.5;
			ptEDGE[1][3].z = m_pt[1].z + m_vec.dz*m_nLen[2]*0.5 + vecCross.dz*m_nLen[3]*0.5;

			IndexTable[0].nCount = 5;
			IndexTable[0].aryRow[0]=0;IndexTable[0].aryCol[0]=0;
			IndexTable[0].aryRow[1]=0;IndexTable[0].aryCol[1]=1;
			IndexTable[0].aryRow[2]=0;IndexTable[0].aryCol[2]=2;
			IndexTable[0].aryRow[3]=0;IndexTable[0].aryCol[3]=3;
			IndexTable[0].aryRow[4]=0;IndexTable[0].aryCol[4]=0;

			IndexTable[1].nCount = 5;
			IndexTable[1].aryRow[0]=0;IndexTable[1].aryCol[0]=1;
			IndexTable[1].aryRow[1]=1;IndexTable[1].aryCol[1]=1;
			IndexTable[1].aryRow[2]=1;IndexTable[1].aryCol[2]=2;
			IndexTable[1].aryRow[3]=0;IndexTable[1].aryCol[3]=2;
			IndexTable[1].aryRow[4]=0;IndexTable[1].aryCol[4]=1;

			IndexTable[2].nCount = 5;
			IndexTable[2].aryRow[0]=1;IndexTable[2].aryCol[0]=0;
			IndexTable[2].aryRow[1]=1;IndexTable[2].aryCol[1]=1;
			IndexTable[2].aryRow[2]=1;IndexTable[2].aryCol[2]=2;
			IndexTable[2].aryRow[3]=1;IndexTable[2].aryCol[3]=3;
			IndexTable[2].aryRow[4]=1;IndexTable[2].aryCol[4]=0;

			IndexTable[3].nCount = 5;
			IndexTable[3].aryRow[0]=0;IndexTable[3].aryCol[0]=0;
			IndexTable[3].aryRow[1]=1;IndexTable[3].aryCol[1]=0;
			IndexTable[3].aryRow[2]=1;IndexTable[3].aryCol[2]=3;
			IndexTable[3].aryRow[3]=0;IndexTable[3].aryCol[3]=3;
			IndexTable[3].aryRow[4]=0;IndexTable[3].aryCol[4]=0;

			IndexTable[4].nCount = 5;
			IndexTable[4].aryRow[0]=0;IndexTable[4].aryCol[0]=2;
			IndexTable[4].aryRow[1]=1;IndexTable[4].aryCol[1]=2;
			IndexTable[4].aryRow[2]=1;IndexTable[4].aryCol[2]=3;
			IndexTable[4].aryRow[3]=0;IndexTable[4].aryCol[3]=3;
			IndexTable[4].aryRow[4]=0;IndexTable[4].aryCol[4]=2;

			for(int i=0;i < 5;++i)
			{
				for(int j=0;j < IndexTable[i].nCount;++j)
				{
					ptEDGE[2][j] = ptEDGE[IndexTable[i].aryRow[j]][IndexTable[i].aryCol[j]];
					ptEDGE[2][j] = pViewFmt->MODEL2VIEW(ptEDGE[2][j]);
				}

				CHSRFace* pFace=CHSRFace::CreateFace(IndexTable[i].nCount,ptEDGE[2]);
				if(pFace)
				{
					pFace->id  = nID++;
					
					pFace->m_enmLayer = GetHSRLayer();
					pFace->SetColor(*m_pstrColor);
					
					pFace->type= BOX;
					pFace->ele = HSR_RECTANGLE_SHAPE;
					pFace->body = this;	/// 2012.01.13 added by humkyung

					pHSRApp->AppendFace(NULL,pFace);
				}
			}
		}
		else if(BOX_TRIANGLE == m_eType)
		{
			return NULL;// not yet support!!

			VECTOR_T vec={0.};
			vec.dx = (m_pt[1].x + m_vecDist.dx) - m_pt[0].x;
			vec.dy = (m_pt[1].y + m_vecDist.dy) - m_pt[0].y;
			vec.dz = (m_pt[1].z + m_vecDist.dz) - m_pt[0].z;
			double nLen=sqrt(vec.dx*vec.dx + vec.dy*vec.dy + vec.dz*vec.dz);
			CMath::NormalizeVector(vec);
			VECTOR_T vecCross = CMath::CrossProduct(vec,m_vec);
			CMath::NormalizeVector(vecCross);

			POINT_T ptCenter={0.};
			ptCenter.x = (m_pt[0].x + m_pt[1].x)*0.5;
			ptCenter.y = (m_pt[0].y + m_pt[1].y)*0.5;
			ptCenter.z = (m_pt[0].z + m_pt[1].z)*0.5;

			ptEDGE[0][0].x = m_pt[0].x - m_vec.dx*m_nLen[0]*0.5 + vecCross.dx*m_nLen[1]*0.5;
			ptEDGE[0][0].y = m_pt[0].y - m_vec.dy*m_nLen[0]*0.5 + vecCross.dy*m_nLen[1]*0.5;
			ptEDGE[0][0].z = m_pt[0].z - m_vec.dz*m_nLen[0]*0.5 + vecCross.dz*m_nLen[1]*0.5;
			ptEDGE[0][1].x = m_pt[0].x - m_vec.dx*m_nLen[0]*0.5 - vecCross.dx*m_nLen[1]*0.5;
			ptEDGE[0][1].y = m_pt[0].y - m_vec.dy*m_nLen[0]*0.5 - vecCross.dy*m_nLen[1]*0.5;
			ptEDGE[0][1].z = m_pt[0].z - m_vec.dz*m_nLen[0]*0.5 - vecCross.dz*m_nLen[1]*0.5;
			ptEDGE[0][2].x = m_pt[1].x - m_vec.dx*m_nLen[0]*0.5;
			ptEDGE[0][2].y = m_pt[1].y - m_vec.dy*m_nLen[0]*0.5;
			ptEDGE[0][2].z = m_pt[1].z - m_vec.dz*m_nLen[0]*0.5;

			/*
			ptEDGE[0][2].x = m_pt[0].x + m_vec.dx*nWidth*0.5 + vecCross.dx*nHeight*0.5;
			ptEDGE[0][2].y = m_pt[0].y + m_vec.dy*nWidth*0.5 + vecCross.dy*nHeight*0.5;
			ptEDGE[0][2].z = m_pt[0].z + m_vec.dz*nWidth*0.5 + vecCross.dz*nHeight*0.5;
			ptEDGE[0][3].x = m_pt[0].x - m_vec.dx*nWidth*0.5 + vecCross.dx*nHeight*0.5;
			ptEDGE[0][3].y = m_pt[0].y - m_vec.dy*nWidth*0.5 + vecCross.dy*nHeight*0.5;
			ptEDGE[0][3].z = m_pt[0].z - m_vec.dz*nWidth*0.5 + vecCross.dz*nHeight*0.5;
			ptEDGE[0][2].x = (ptEDGE[0][2].x + ptEDGE[0][3].x)*0.5;
			ptEDGE[0][2].y = (ptEDGE[0][2].y + ptEDGE[0][3].y)*0.5;
			ptEDGE[0][2].z = (ptEDGE[0][2].z + ptEDGE[0][3].z)*0.5;
			*/

			ptEDGE[1][0].x = m_pt[0].x + m_vec.dx*m_nLen[0]*0.5 + vecCross.dx*m_nLen[1]*0.5;
			ptEDGE[1][0].y = m_pt[0].y + m_vec.dy*m_nLen[0]*0.5 + vecCross.dy*m_nLen[1]*0.5;
			ptEDGE[1][0].z = m_pt[0].z + m_vec.dz*m_nLen[0]*0.5 + vecCross.dz*m_nLen[1]*0.5;
			ptEDGE[1][1].x = m_pt[0].x + m_vec.dx*m_nLen[0]*0.5 - vecCross.dx*m_nLen[1]*0.5;
			ptEDGE[1][1].y = m_pt[0].y + m_vec.dy*m_nLen[0]*0.5 - vecCross.dy*m_nLen[1]*0.5;
			ptEDGE[0][1].z = m_pt[0].z + m_vec.dz*m_nLen[0]*0.5 - vecCross.dz*m_nLen[1]*0.5;
			ptEDGE[1][2].x = m_pt[1].x + m_vec.dx*m_nLen[0]*0.5;
			ptEDGE[1][2].y = m_pt[1].y + m_vec.dy*m_nLen[0]*0.5;
			ptEDGE[1][2].z = m_pt[1].z + m_vec.dz*m_nLen[0]*0.5;

			/*
			ptEDGE[1][2].x = m_pt[1].x + m_vec.dx*nWidth*0.5 + vecCross.dx*nHeight*0.5;
			ptEDGE[1][2].y = m_pt[1].y + m_vec.dy*nWidth*0.5 + vecCross.dy*nHeight*0.5;
			ptEDGE[1][2].z = m_pt[1].z + m_vec.dz*nWidth*0.5 + vecCross.dz*nHeight*0.5;
			ptEDGE[1][3].x = m_pt[1].x - m_vec.dx*nWidth*0.5 + vecCross.dx*nHeight*0.5;
			ptEDGE[1][3].y = m_pt[1].y - m_vec.dy*nWidth*0.5 + vecCross.dy*nHeight*0.5;
			ptEDGE[1][3].z = m_pt[1].z - m_vec.dz*nWidth*0.5 + vecCross.dz*nHeight*0.5;
			ptEDGE[1][2].x = (ptEDGE[1][2].x + ptEDGE[1][3].x)*0.5;
			ptEDGE[1][2].y = (ptEDGE[1][2].y + ptEDGE[1][3].y)*0.5;
			ptEDGE[1][2].z = (ptEDGE[1][2].z + ptEDGE[1][3].z)*0.5;
			*/

			IndexTable[0].nCount = 4;
			IndexTable[0].aryRow[0]=0;IndexTable[0].aryCol[0]=0;
			IndexTable[0].aryRow[1]=0;IndexTable[0].aryCol[1]=1;
			IndexTable[0].aryRow[2]=0;IndexTable[0].aryCol[2]=2;
			IndexTable[0].aryRow[3]=0;IndexTable[0].aryCol[3]=0;

			IndexTable[1].nCount = 5;
			IndexTable[1].aryRow[0]=0;IndexTable[1].aryCol[0]=1;
			IndexTable[1].aryRow[1]=1;IndexTable[1].aryCol[1]=1;
			IndexTable[1].aryRow[2]=1;IndexTable[1].aryCol[2]=2;
			IndexTable[1].aryRow[3]=0;IndexTable[1].aryCol[3]=2;
			IndexTable[1].aryRow[4]=0;IndexTable[1].aryCol[4]=1;

			IndexTable[2].nCount = 4;
			IndexTable[2].aryRow[0]=1;IndexTable[2].aryCol[0]=0;
			IndexTable[2].aryRow[1]=1;IndexTable[2].aryCol[1]=1;
			IndexTable[2].aryRow[2]=1;IndexTable[2].aryCol[2]=2;
			IndexTable[2].aryRow[3]=1;IndexTable[2].aryCol[3]=0;

			IndexTable[3].nCount = 5;
			IndexTable[3].aryRow[0]=0;IndexTable[3].aryCol[0]=0;
			IndexTable[3].aryRow[1]=1;IndexTable[3].aryCol[1]=0;
			IndexTable[3].aryRow[2]=1;IndexTable[3].aryCol[2]=2;
			IndexTable[3].aryRow[3]=0;IndexTable[3].aryCol[3]=2;
			IndexTable[3].aryRow[4]=0;IndexTable[3].aryCol[4]=0;

			IndexTable[4].nCount = 5;
			IndexTable[4].aryRow[0]=0;IndexTable[4].aryCol[0]=0;
			IndexTable[4].aryRow[1]=1;IndexTable[4].aryCol[1]=0;
			IndexTable[4].aryRow[2]=1;IndexTable[4].aryCol[2]=1;
			IndexTable[4].aryRow[3]=1;IndexTable[4].aryCol[3]=0;
			IndexTable[4].aryRow[4]=0;IndexTable[4].aryCol[4]=0;

			for(int i=0;i < 5;i++)
			{
				for(int j=0;j < IndexTable[i].nCount;j++)
				{
					//ptEDGE[2][j] = KGeometry::RotateAboutYXAxis(ptEDGE[2][j],-beta,-alpha);
					ptEDGE[2][j] = ptEDGE[IndexTable[i].aryRow[j]][IndexTable[i].aryCol[j]];
					ptEDGE[2][j] = pViewFmt->MODEL2VIEW(ptEDGE[2][j]);
				}

				CHSRFace* pFace=CHSRFace::CreateFace(IndexTable[i].nCount,ptEDGE[2]);
				if(pFace)
				{
					pFace->id  = nID++;
					pFace->m_enmLayer = GetHSRLayer();
					pFace->type= BOX;
					pFace->ele = HSR_RECTANGLE_SHAPE;
					pFace->body = this;	/// 2012.01.13 added by humkyung
					pFace->SetColor(*m_pstrColor);

					pHSRApp->AppendFace(NULL,pFace);
				}
			}
		}
	}

	return NULL;
}