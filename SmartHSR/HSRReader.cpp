// HSRReader.cpp: implementation of the CHSRReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <tchar.h>
#include <math\ITKMath.h>
#include <math\vect.h>
#include "HSRReader.h"

#include <stack>
#include <set>
using namespace std;

static list<CHSRElement*> m_lstExtElm;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CHSRReader::CHSRReader()
{
	m_pSxfFormatParse = NULL;
}

/**	\brief
*/
CHSRReader::~CHSRReader()
{
	Reset();
#ifdef _DEBUG
	OutputDebugString("Destructure of CHSRReader");
#endif
}

/******************************************************************************
	@author     humkyung
	@date       2012-03-05
	@class      CHSRReader
	@function   ParseDraFile
	@return     int
	@param      LPCTSTR pDraFilePath
	@brief
******************************************************************************/
int CHSRReader::ParseDraFile(LPCTSTR pDraFilePath)
{
	assert(pDraFilePath && "pDraFilePath is NULL");

	if(pDraFilePath)
	{
		CMarkup xml;
		if(true == xml.Load(pDraFilePath))
		{
			MCD_STR sLoadResult = xml.GetResult();

			if(true == xml.FindElem(MCD_T("DRA")))
			{
				xml.IntoElem();
				while(true == xml.FindElem(MCD_T("VIEW")))
				{
					CHSRViewFormat* pView = CHSRViewFormat::CreateInstance();
					if(pView)
					{
						pView->Parse(xml);
						m_lstVIEWFormat.push_back( pView );
					}
				}
				xml.OutOfElem();
			}

			return ERROR_SUCCESS;
		}

		return ERROR_FILE_NOT_FOUND;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-05
    @class      CHSRReader
    @function   IsInDrawing
    @return     bool
    @param      CHSRElement*    pElm
    @brief		check if element is in drawnig
******************************************************************************/
bool CHSRReader::IsInDrawing(CHSRElement* pElm)
{
	for(list<CHSRViewFormat*>::iterator itr = m_lstVIEWFormat.begin();itr != m_lstVIEWFormat.end();++itr)
	{
		CIsVolume vol = (*itr)->GetModelVolume();
		if(true ==  pElm->IsInVolume(vol)) return true;
	}

	return false;
}

static STRING_T GetFolderNameOf(const CHSRElement::MODEL_TYPE_T& modelType)
{
	STRING_T sFolderName;
	switch(modelType)
	{
		case CHSRElement::PIPE_MODEL_T:
			sFolderName = _T("Pipe");
			break;
		case CHSRElement::EQUIPMENT_MODEL_T:
			sFolderName = _T("Equipment");
			break;
		case CHSRElement::STRUCTURE_MODEL_T:
			sFolderName = _T("Structure");
			break;
		case CHSRElement::CABLE_T_MODEL_T:
			sFolderName = _T("CableT");
			break;
		case CHSRElement::MISC_MODEL_T:
			sFolderName = _T("Misc");
			break;
	}

	return sFolderName;
}

/**	
	@brief	The CHSRReader::Read function

	@author	humkyung

	@return	void	
*/
void CHSRReader::Read(const STRING_T& sSxfFolderPath)
{
	if(NULL == m_pSxfFormatParse) m_pSxfFormatParse = new CHSRParser(this);

	if(m_pSxfFormatParse)
	{
		/// read all sxf files
		set<STRING_T> oReadedSxfFilePath;
		for(list<CHSRViewFormat*>::iterator itr = m_lstVIEWFormat.begin();itr != m_lstVIEWFormat.end();++itr)
		{
			for(list<CHSRElement::MODEL_T>::iterator jtr = (*itr)->m_oModelList.begin();jtr != (*itr)->m_oModelList.end();++jtr)
			{
				const STRING_T sFolderName = GetFolderNameOf(jtr->modelType);

				STRING_T sSxfFilePath = sSxfFolderPath + _T("\\") + sFolderName + _T("\\") + jtr->rModelName;
				STRING_T::size_type at = sSxfFilePath.rfind(_T("."));
				if(STRING_T::npos != at)
				{
					sSxfFilePath = sSxfFilePath.substr(0 , at) + _T(".sxf");
				}

				set<STRING_T>::iterator where = oReadedSxfFilePath.find(sSxfFilePath);
				if(where == oReadedSxfFilePath.end())
				{
					m_pSxfFormatParse->Parse(sSxfFilePath.c_str() , *jtr);
					oReadedSxfFilePath.insert(sSxfFilePath);
				}
			}
		}
		/// up to here

		if(m_lstVIEWFormat.empty()) return;

		this->CreatePlaneFace();
		for(list<CHSRViewFormat*>::iterator itr = m_lstVIEWFormat.begin();itr != m_lstVIEWFormat.end();++itr)
		{
			try
			{
				for(list<CElbowElement*>::iterator jtr = m_lstELBOWFormat.begin();jtr != m_lstELBOWFormat.end();++jtr)
				{
					if((*itr)->DoesInclude((*jtr)->m_model) && (*jtr)->IsInVolume((*itr)->GetModelVolume()))
					{
						*(*itr)+= (*jtr);
					}
				}
			}
			catch(...)
			{
				OutputDebugString(_T("Error in processing ELBOW"));
			}

			try
			{
				for(list<CTeeElement*>::iterator jtr = m_lstTEEFormat.begin();jtr != m_lstTEEFormat.end();++jtr)
				{
					if((*itr)->DoesInclude((*jtr)->m_model) && (*jtr)->IsInVolume((*itr)->GetModelVolume()))
					{
						CHSRViewFormat* pView=*itr;
						CTeeElement*  pTee =*jtr;
						*(*itr)+= (*jtr);
					}
				}
			}
			catch(...)
			{
				OutputDebugString(_T("Error in processing TEE"));
			}

			try
			{
				for(list<CTorsElement*>::iterator jtr = m_lstTORSFormat.begin();jtr != m_lstTORSFormat.end();++jtr)
				{ 
					if((*itr)->DoesInclude((*jtr)->m_model) && (*jtr)->IsInVolume((*itr)->GetModelVolume()))
					{
						*(*itr)+= (*jtr);
					}
				}
			}
			catch(...)
			{
				OutputDebugString(_T("Error in processing TORS"));
			}

			try
			{
				for(list<CCTorElement*>::iterator jtr = m_lstCTORFormat.begin();jtr != m_lstCTORFormat.end();++jtr)
				{
					if((*itr)->DoesInclude((*jtr)->m_model) && (*jtr)->IsInVolume((*itr)->GetModelVolume()))
					{
						*(*itr)+= (*jtr);
					}
				}
			}
			catch(...)
			{
				OutputDebugString(_T("Error in processing CTOR"));
			}

			try
			{
				for(list<CRTorElement*>::iterator jtr =m_lstRTorElement.begin();jtr != m_lstRTorElement.end();++jtr)
				{
					if((*itr)->DoesInclude((*jtr)->m_model) && (*jtr)->IsInVolume((*itr)->GetModelVolume()))
					{
						*(*itr)+= (*jtr);
					}
				}
			}
			catch(...)
			{
				OutputDebugString(_T("Error in processing RTOR"));
			}

			try
			{
				for(list<CDomeElement*>::iterator jtr = m_lstDomeElement.begin();jtr != m_lstDomeElement.end();++jtr)
				{
					if((*itr)->DoesInclude((*jtr)->m_model) && (*jtr)->IsInVolume((*itr)->GetModelVolume()))
					{
						*(*itr)+= (*jtr);
					}
				}
			}
			catch(...)
			{
				OutputDebugString(_T("Error in processing DOME"));
			}

			int nIndex = 0;
			try
			{
				for(list<CHSRElement*>::iterator jtr = m_lstElement.begin();jtr != m_lstElement.end();++jtr)
				{
					CHSRElement* pElm = *jtr;
					if((*itr)->DoesInclude((*jtr)->m_model) && (*jtr)->IsInVolume((*itr)->GetModelVolume()))
					{
						*(*itr)+= (*jtr);
					}
					++nIndex;
				}
			}
			catch(...)
			{
				OutputDebugString(_T("Error in processing element"));
			}

			//////////////////////////////////////////////////////////////////////////
			/// external element
			try
			{
				for(list<CHSRElement*>::iterator jtr = m_lstExtElm.begin();jtr != m_lstExtElm.end();++jtr)
				{
					CHSRElement* pElm=*jtr;
					if((*itr)->DoesInclude((*jtr)->m_model) && (*jtr)->IsInVolume((*itr)->GetModelVolume()))
					{
						*(*itr)+= (*jtr);
					}
				}
			}
			catch(...)
			{
				OutputDebugString(_T("Error in processing external element"));
			}

			try
			{
				for(list<CHSRNormalFace*>::iterator jtr = m_lstNormalFace.begin();jtr != m_lstNormalFace.end();++jtr)
				{
					if((*itr)->DoesInclude((*jtr)->m_model) && (*jtr)->IsInVolume((*itr)->GetModelVolume()))
					{
						*(*itr)+= (*jtr);
					}
				}
			}
			catch(...)
			{
				OutputDebugString(_T("Error in processing Normal Face2"));
			}

			try
			{
				for(list<CPreHNormalFace*>::iterator jtr = m_lstHNormalFace.begin();jtr != m_lstHNormalFace.end();++jtr)
				{
					if((*itr)->DoesInclude((*jtr)->m_model) && (*jtr)->IsInVolume((*itr)->GetModelVolume()))
					{
						*(*itr)+= (*jtr);
					}
				}
			}
			catch(...)
			{
				OutputDebugString(_T("Error in processing Hidden Face2"));
			}
		}
	}
}

/**	
	@brief	The CHSRReader::IsAdjacentView function

	@author	humkyung

	@param	pView1	a parameter of type CHSRViewFormat*
	@param	pView2	a parameter of type CHSRViewFormat*
	@param	nTol	a parameter of type const double

	@return	bool
*/
bool CHSRReader::IsAdjacentView(CHSRViewFormat* pView1,CHSRViewFormat* pView2,const double nTol)
{
	assert(pView1 && "pView1 is NULL");
	assert(pView2 && "pView2 is NULL");
	bool bRet=false;

	if(pView1 && pView2)
	{
		if(!pView1->IsIn(pView2) && !pView2->IsIn(pView1))
		{
			POINT_T ptOrigin[2]={0};
			ptOrigin[0] = pView1->GetViewOrigin();
			ptOrigin[1] = pView2->GetViewOrigin();

			double dx=fabs(ptOrigin[0].x - ptOrigin[1].x);
			double dy=fabs(ptOrigin[0].y - ptOrigin[1].y);
			double nWidth =(pView1->GetViewWidth()*0.5)  + (pView2->GetViewWidth()*0.5)  + nTol;
			double nHeight=(pView1->GetViewHeight()*0.5) + (pView2->GetViewHeight()*0.5) + nTol;
			if((dx < nWidth) && (dy < nHeight))
			{
				bRet = true;
			}
		}
	}

	return bRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHSRReader::SetCoIncidentView(CHSRViewFormat* pView1,CHSRViewFormat* pView2)
{
	assert(pView1 && "pView1 is NULL");
	assert(pView2 && "pView2 is NULL");

	if(pView1 && pView2)
	{
		POINT_T ptOrigin[2]={0};
		ptOrigin[0] = pView1->GetViewOrigin();
		ptOrigin[1] = pView2->GetViewOrigin();

		double dx=fabs(ptOrigin[0].x - ptOrigin[1].x);
		double dy=fabs(ptOrigin[0].y - ptOrigin[1].y);
		double nWidth =(pView1->GetViewWidth())*0.5 + (pView2->GetViewWidth())*0.5;
		double nHeight=(pView1->GetViewHeight())*0.5+ (pView2->GetViewHeight())*0.5;
		if(dx > nWidth)
		{
			double nGap=dx - nWidth;
			CVect vecDir;

			vecDir.dx = ptOrigin[1].x - ptOrigin[0].x;vecDir.dz = 0;
			vecDir.Normalize();
			POINT_T ptOrigin=(POINT_T)(pView1->GetViewOrigin());
			ptOrigin.x += vecDir.dx*nGap*0.5;
			pView1->SetViewOrigin(ptOrigin);

			vecDir.dx = -vecDir.dx;
			ptOrigin=(POINT_T)(pView2->GetViewOrigin());
			ptOrigin.x += vecDir.dx*nGap*0.5;
			pView2->SetViewOrigin(ptOrigin);
		}

		if(dy > nHeight)
		{
			double nGap=dy - nHeight;
			CVect vecDir;

			vecDir.dy = ptOrigin[1].y - ptOrigin[0].y;vecDir.dz = 0;
			vecDir.Normalize();
			POINT_T ptOrigin=(POINT_T)(pView1->GetViewOrigin());
			ptOrigin.y += vecDir.dy*nGap*0.6;
			pView1->SetViewOrigin(ptOrigin);

			vecDir.dy = -vecDir.dy;
			ptOrigin=(POINT_T)(pView2->GetViewOrigin());
			ptOrigin.y += vecDir.dy*nGap*0.6;
			pView2->SetViewOrigin(ptOrigin);
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-05
    @class      CHSRReader
    @function   Reset
    @return     void
    @brief
******************************************************************************/
void CHSRReader::Reset()
{
	if(m_pSxfFormatParse)
	{
		delete m_pSxfFormatParse;
		m_pSxfFormatParse = NULL;
	}

	for(list<CHSRViewFormat*>::iterator itr=m_lstVIEWFormat.begin();itr != m_lstVIEWFormat.end();++itr)
	{
		delete (*itr);
	}
	m_lstVIEWFormat.clear();

	for(list<CElbowElement*>::iterator itr = m_lstELBOWFormat.begin();itr != m_lstELBOWFormat.end();++itr)
	{
		delete (*itr);
	}
	m_lstELBOWFormat.clear();

	for(list<CTeeElement*>::iterator itr = m_lstTEEFormat.begin();itr != m_lstTEEFormat.end();++itr)
	{
		delete (*itr);
	}
	m_lstTEEFormat.clear();

	for(list<CTorsElement*>::iterator itr = m_lstTORSFormat.begin();itr != m_lstTORSFormat.end();++itr)
	{
		delete (*itr);
	}
	m_lstTORSFormat.clear();

	for(list<CDomeElement*>::iterator itr = m_lstDomeElement.begin();itr != m_lstDomeElement.end();++itr)
	{
		delete (*itr);
	}
	m_lstDomeElement.clear();

	for(list<CHSRElement*>::iterator itr=m_lstElement.begin();itr!= m_lstElement.end();++itr)
	{
		delete (*itr);
	}
	m_lstElement.clear();

	for(list<CLineElement*>::iterator itr=m_lstLineElement.begin();itr != m_lstLineElement.end();++itr)
	{
		delete (*itr);
	}
	m_lstLineElement.clear();

	for(list<CHSRNormalFace*>::iterator itr=m_lstNormalFace.begin();itr != m_lstNormalFace.end();++itr)
	{
		delete (*itr);
	}
	m_lstNormalFace.clear();

	for(list<CHLineElement*>::iterator itr = m_lstHLINEFormat.begin();itr != m_lstHLINEFormat.end();++itr)
	{
		delete (*itr);
	}
	m_lstHLINEFormat.clear();

	for(list<CPreHNormalFace*>::iterator itr = m_lstHNormalFace.begin();itr != m_lstHNormalFace.end();++itr)
	{
		delete (*itr);
	}
	m_lstHNormalFace.clear();

	//////////////////////////////////////////////////////////////////////////
	/// external element
	m_lstExtElm.clear();
}

bool CHSRReader::AddExtElm(CHSRElement *pElm)
{
	assert(pElm && "pElm is NULL");

	if(pElm) m_lstExtElm.push_back(pElm);

	return true;
}

/**	
	@brief	The CHSRReader::CreatePlaneFace function

	@author	humkyung

	@date	????.??.??

	@param	

	@return	void	
*/
void CHSRReader::CreatePlaneFace()
{
	//////////////////////////////////////////////////////////////////////////
	/// calc total view's model volume.
	VOLUME_T total_volume={0,};
	for(list<CHSRViewFormat*>::iterator itr=m_lstVIEWFormat.begin();itr != m_lstVIEWFormat.end();++itr)
	{
		CIsVolume volume=(*itr)->GetModelVolume();
		if(m_lstVIEWFormat.begin() == itr)
		{
			total_volume.x[LOW]  = volume.minx();
			total_volume.y[LOW]  = volume.miny();
			total_volume.z[LOW]  = volume.minz();
			total_volume.x[HIGH] = volume.maxx();
			total_volume.y[HIGH] = volume.maxy();
			total_volume.z[HIGH] = volume.maxz();
		}
		else
		{
			if(volume.minx() < total_volume.x[0]) total_volume.x[0] = volume.minx();
			if(volume.maxx() > total_volume.x[1]) total_volume.x[1] = volume.maxx();
			if(volume.miny() < total_volume.y[0]) total_volume.y[0] = volume.miny();
			if(volume.maxy() > total_volume.y[1]) total_volume.y[1] = volume.maxy();
			if(volume.minz() < total_volume.z[0]) total_volume.z[0] = volume.minz();
			if(volume.maxz() > total_volume.z[1]) total_volume.z[1] = volume.maxz();
		}
	}

	CIsVolume oVolume(	total_volume.x[0] , total_volume.y[0] , total_volume.z[0] , 
						total_volume.x[1] , total_volume.y[1] , total_volume.z[1]);
	//////////////////////////////////////////////////////////////////////////
	/// create normal face
	try
	{
		stack<CHSRNormalFace*> _mem_pool;

		CHSRNormalFace *pFace=NULL,*pTemp=NULL;
		for(list<CLineElement*>::iterator itrLINE=m_lstLineElement.begin();itrLINE != m_lstLineElement.end();itrLINE++)
		{
			const long nGroup=(*itrLINE)->m_nGroup;

			if(pFace && (pFace->GetGroupNo() == nGroup))
			{
				(*pFace) += (*itrLINE);
			}
			else
			{	// create new normal face
				if(pFace && this->IsInDrawing(pFace))///pFace->IsInVolume( oVolume ))
				{
					m_lstElement.push_back(pFace);
					pFace = NULL;
				}else if(pFace) _mem_pool.push(pFace);

				//////////////////////////////////////////////////////////////////////////
				/// allocate memory for CHSRNormalFace.
				if(!_mem_pool.empty())
				{	/// check memory pool.
					pFace = _mem_pool.top();_mem_pool.pop();
					pFace->Init(nGroup);
					(*pFace) += (*itrLINE);
				}
				else
				{
					if(pFace = new CHSRNormalFace(nGroup))
					{
						(*pFace) += (*itrLINE);
					}
				}
			}
		}
		if(pFace && (pFace->GetLineCount() > 2)) m_lstElement.push_back(pFace);
	}
	catch(...)
	{
		OutputDebugString(_T("Error in processing Normal Face"));
	}

	//////////////////////////////////////////////////////////////////////////
	/// create hidden face
	try
	{
		stack<CPreHNormalFace*> _mem_pool;

		CPreHNormalFace* pHNormalFace=NULL;
		for(list<CHLineElement*>::iterator itrHLINE=m_lstHLINEFormat.begin();itrHLINE != m_lstHLINEFormat.end();itrHLINE++)
		{
			long nGroup=(*itrHLINE)->m_nGroup;

			if(pHNormalFace && (pHNormalFace->GetGroupNo() == nGroup))
			{
				(*pHNormalFace) += (*itrHLINE);
			}
			else
			{
				if(pHNormalFace && pHNormalFace->IsInVolume(oVolume))
				{
					m_lstHNormalFace.push_back(pHNormalFace);
					pHNormalFace = NULL;
				}else if(pHNormalFace) _mem_pool.push(pHNormalFace);

				//////////////////////////////////////////////////////////////////////////
				/// allocate memory for CHPreNormalFace.
				if(!_mem_pool.empty())
				{
					pHNormalFace = _mem_pool.top();_mem_pool.pop();
					pHNormalFace->Init(nGroup);
					(*pHNormalFace) += (*itrHLINE);
				}
				else
				{
					if(pHNormalFace = new CPreHNormalFace(nGroup))
					{
						(*pHNormalFace) += (*itrHLINE);
					}
				}
			}
		}
		if(pHNormalFace) m_lstHNormalFace.push_back(pHNormalFace);
	}
	catch(...)
	{
		OutputDebugString(_T("Error in processing Hidden Face1"));
	}
}
