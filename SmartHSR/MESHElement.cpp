// MESHElement.cpp: implementation of the CMeshElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MESHElement.h"
#include "SmartHSR.h"
#include "HSRNormalFace.h"

IMPLEMENTS_HSR_ELEMENT(CMeshElement , CHSRElement , _T("__MESH_HSR_ELEMENT__"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CMeshElement::CMeshElement()
{
	m_nRow = 0;
	m_nCol = 0;
}

/**	\brief
*/
CMeshElement::~CMeshElement()
{

}

/**	
	@brief	The CMeshElement::ParseLine function

	@author	humkyung

	@param	pScanner	a parameter of type CHSRScanner*

	@return	bool	
*/
bool CMeshElement::ParseLine(CHSRScanner* pScanner)
{
	assert(pScanner && "pScanner is NULL");
	bool bRet=false;

	if(pScanner)
	{
		m_nRow=ATOI_T((*pScanner->m_aryFactor)[1].c_str());
		m_nCol=ATOI_T((*pScanner->m_aryFactor)[2].c_str());

		assert((m_nRow >= 2) && (m_nCol >= 2));
		if((m_nRow >= 2) && (m_nRow < MAX_MESH_ELEMENT) && (m_nCol >= 2) && (m_nCol < MAX_MESH_ELEMENT)){
			static TCHAR szBuf[3][32]={0,};
			int nCount=0,nIndex=0;
			for(int i = 0;i < m_nRow;i++)
			{
				for(int j = 0;j < m_nCol;j++)
				{
					int nLen=STRLEN_T((*pScanner->m_aryFactor)[i*m_nCol + j + 3].c_str());
					nCount = nIndex = 0;
					for(int k=0;k < nLen;k++){
						if('^' == (*pScanner->m_aryFactor)[i*m_nCol + j + 3][k])
						{
							szBuf[nCount][nIndex] = '\0';
							nCount++;
							nIndex = 0;
						}else	szBuf[nCount][nIndex++] = (*pScanner->m_aryFactor)[i*m_nCol + j + 3][k];
					}
					szBuf[nCount][nIndex] = '\0';

					m_pt[i][j].x = ATOF_T(szBuf[0]);
					m_pt[i][j].y = ATOF_T(szBuf[1]);
					m_pt[i][j].z = ATOF_T(szBuf[2]);
				}
			}
			bRet = true;
		}
	}

	return bRet;
}

/**	\brief	The CMeshElement::IsInVolume function

	\param	volume	a parameter of type const VOLUME_T

	\return	bool	
*/
bool CMeshElement::IsInVolume(const CIsVolume& volume)
{
	m_vol.x[0] = m_vol.x[1] = m_pt[0][0].x;
	m_vol.y[0] = m_vol.y[1] = m_pt[0][0].y;
	m_vol.z[0] = m_vol.z[1] = m_pt[0][0].z;
	for(int i=0;i < m_nRow;i++)
	{
		for(int j=0;j < m_nCol;j++)
		{
			if(m_vol.x[0] > m_pt[i][j].x) m_vol.x[0] = m_pt[i][j].x;
			if(m_vol.x[1] < m_pt[i][j].x) m_vol.x[1] = m_pt[i][j].x;
			if(m_vol.y[0] > m_pt[i][j].y) m_vol.y[0] = m_pt[i][j].y;
			if(m_vol.y[1] < m_pt[i][j].y) m_vol.y[1] = m_pt[i][j].y;
			if(m_vol.z[0] > m_pt[i][j].z) m_vol.z[0] = m_pt[i][j].z;
			if(m_vol.z[1] < m_pt[i][j].z) m_vol.z[1] = m_pt[i][j].z;
		}
	}

	if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
		(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
		(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;

	return true;
}

/**	\brief	The CMeshElement::CreateFace function

	\param	pVIEWFormat	a parameter of type CPreViewFormat*
	\param	nID	a parameter of type long&

	\return	PFACE	
*/
CHSRFace* CMeshElement::CreateFace(CHSRViewFormat* pVIEWFormat,long& nID)
{
	assert(pVIEWFormat && "pVIEWFormat is NULL");
	CHSRFace* pRet=NULL;

	CHSRApp* pHSRApp=CHSRApp::GetInstance();
	if(pVIEWFormat && pHSRApp)
	{
		static POINT_T ptEDGE[5];
		
		for(int i=0;i < m_nRow - 1;i++)
		{
			for(int j=0;j < m_nCol - 1;j++)
			{
				ptEDGE[0] = m_pt[i][j];
				ptEDGE[1] = m_pt[i][j + 1];
				ptEDGE[2] = m_pt[i + 1][j + 1];
				ptEDGE[3] = m_pt[i + 1][j];

				int nCount=4;
				for(int k=0;k < nCount;k++)
				{
					for(int l=k + 1;l < nCount;l++)
					{
						double dx=ptEDGE[k].x - ptEDGE[l].x;
						double dy=ptEDGE[k].y - ptEDGE[l].y;
						double dz=ptEDGE[k].z - ptEDGE[l].z;

						if((fabs(dx) < 0.001) && (fabs(dy) < 0.001) && (fabs(dz) < 0.001))
						{
							memmove(&ptEDGE[l],&ptEDGE[l + 1],sizeof(POINT_T)*(4 - l - 1));
							l--;
							nCount--;
						}
					}
				}

				if(nCount >= 3)
				{
					for(int k = 0;k < nCount;k++) ptEDGE[k] = pVIEWFormat->MODEL2VIEW(ptEDGE[k]);
					if(pRet=CHSRFace::CreateFace(nCount,ptEDGE))
					{
						pRet->id  = nID++;
						pRet->ele = NORMAL;
						pRet->m_enmLayer = LAYER_OTHER;
						pRet->SetColor(*m_pstrColor);
						pHSRApp->AppendFace(NULL,pRet);
					}
				}
			}
		}
	}

	return NULL;
}