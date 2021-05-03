// FACEMESHElement.cpp: implementation of the CFaceMeshElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmartHSR.h"
#include "FaceMeshElement.h"

IMPLEMENTS_HSR_ELEMENT(CFaceMeshElement , CHSRElement , _T("__FACE_MESH_HSR_ELEMENT__"));
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CFaceMeshElement::CFaceMeshElement() : m_nFaces(0) , m_nPoints(0) , m_pLoopIndex(NULL) , m_ppt(NULL)
{
}

CFaceMeshElement::~CFaceMeshElement()
{
	try
	{
		if(NULL != m_pLoopIndex)
		{
			delete []m_pLoopIndex;
			m_pLoopIndex = NULL;
		}

		if(NULL != m_ppt)
		{
			delete []m_ppt;
			m_ppt = NULL;
		}
	}
	catch(...)
	{
	}
}

/**	
	@brief	MESH_FACE 형상을 파싱한다.

	@author	HumKyung.BAEK

	@param	pScanner	a parameter of type CHSRScanner*

	@return	bool	true = 파싱 성공 , false = 파싱 실패
*/
bool CFaceMeshElement::ParseLine(CHSRScanner* pScanner)
{
	assert(pScanner && "pScanner is NULL");
	bool bRet = false;

	if(pScanner)
	{
		m_nFaces = ATOI_T((*pScanner->m_aryFactor)[1].c_str());
		m_pLoopIndex = new LoopIndex[m_nFaces];
		if(m_pLoopIndex)
		{
			int i = 0;
			for(i = 0;(i < m_nFaces) && (*pScanner->m_aryFactor)[i + 2][0];++i)
			{
				SSCANF_T((*pScanner->m_aryFactor)[i + 2].c_str() , _T("%d^%d^%d^%d"),
					&(m_pLoopIndex[i].nSeq[0]),&(m_pLoopIndex[i].nSeq[1]),&(m_pLoopIndex[i].nSeq[2]),&(m_pLoopIndex[i].nSeq[3]));
			}
			if(m_nFaces != i) return false;

			m_nPoints = pScanner->m_nFactor - (m_nFaces + 2);
			m_ppt = new POINT_T[m_nPoints];
			if(m_ppt)
			{
				for(i = 0;(i < m_nPoints) && (*pScanner->m_aryFactor)[i + (m_nFaces + 2)][0];++i)
				{
					SSCANF_T((*pScanner->m_aryFactor)[i + (m_nFaces + 2)].c_str(), _T("%lf^%lf^%lf") ,
						&(m_ppt[i].x),&(m_ppt[i].y),&(m_ppt[i].z));
				}

				bRet = true;
			}
		}
	}

	return bRet;
}

/**	@brief	뷰의 볼륨안에 들어가는지 검사한다.

	@author	HumKyung.BAEK

	@param	volume	뷰 볼륨

	@return	bool	true = 뷰의 볼륨에 속함. false = 뷰의 볼륨에 속하지 않음.
*/
bool CFaceMeshElement::IsInVolume(const CIsVolume& volume)
{
	if(m_ppt)
	{
		m_vol.x[0] = m_vol.x[1] = m_ppt[0].x;
		m_vol.y[0] = m_vol.y[1] = m_ppt[0].y;
		m_vol.z[0] = m_vol.z[1] = m_ppt[0].z;
		for(int i=0;i < m_nPoints;++i)
		{
			if(m_vol.x[0] > m_ppt[i].x) m_vol.x[0] = m_ppt[i].x;
			if(m_vol.x[1] < m_ppt[i].x) m_vol.x[1] = m_ppt[i].x;
			if(m_vol.y[0] > m_ppt[i].y) m_vol.y[0] = m_ppt[i].y;
			if(m_vol.y[1] < m_ppt[i].y) m_vol.y[1] = m_ppt[i].y;
			if(m_vol.z[0] > m_ppt[i].z) m_vol.z[0] = m_ppt[i].z;
			if(m_vol.z[1] < m_ppt[i].z) m_vol.z[1] = m_ppt[i].z;
		}

		if(	(m_vol.x[0] > volume.maxx()) || (m_vol.x[1] < volume.minx()) ||
			(m_vol.y[0] > volume.maxy()) || (m_vol.y[1] < volume.miny()) ||
			(m_vol.z[0] > volume.maxz()) || (m_vol.z[1] < volume.minz())) return false;

		return true;
	}

	return false;
}

/**	
	@brief	create faces for FACE_MESH element.

	@author	HumKyung.BAEK

	@param	pVIEWFormat	a parameter of type CPreViewFormat*
	@param	nID	a parameter of type long&

	@return	PFACE	
*/
CHSRFace* CFaceMeshElement::CreateFace(CHSRViewFormat* pVIEWFormat,long& nID)
{
	assert(pVIEWFormat && "pVIEWFormat is NULL");
	CHSRFace* pRet=NULL;

	CHSRApp* pHiddenApp=CHSRApp::GetInstance();
	if(pVIEWFormat && pHiddenApp && m_ppt && m_pLoopIndex)
	{
		static POINT_T ptEDGE[5];
		memset( ptEDGE , '\0' , sizeof(POINT_T) * 5);

		for(int i=0;i < m_nFaces;++i)
		{
			ptEDGE[0] = m_ppt[abs(m_pLoopIndex[i].nSeq[0]) - 1];
			ptEDGE[1] = m_ppt[abs(m_pLoopIndex[i].nSeq[1]) - 1];
			ptEDGE[2] = m_ppt[abs(m_pLoopIndex[i].nSeq[2]) - 1];
			ptEDGE[3] = m_ppt[abs(m_pLoopIndex[i].nSeq[3]) - 1];
			for(int j=0;j < 4;j++) ptEDGE[j] = pVIEWFormat->MODEL2VIEW(ptEDGE[j]);
			if(pRet = CHSRFace::CreateFace(4 , ptEDGE , false))	//! merge edge flag를 off시킨다.
			{
				//! vertex index값이 음수일 경우에 해당 edge는 보이지 않도록 처리한다.
				int index = 0;
				for(PHSR_VERTEX ptr = pRet->pHead;(index < 4) && ptr;ptr = ptr->next,++index)
				{
					if(m_pLoopIndex[i].nSeq[index] < 0) ptr->render = INVALID;
				}

				pRet->id  = nID++;
				pRet->ele = NORMAL;
				pRet->m_enmLayer = LAYER_OTHER;
				pRet->SetColor(*m_pstrColor);
				pHiddenApp->AppendFace(NULL,pRet);
			}
		}
	}

	return NULL;
}