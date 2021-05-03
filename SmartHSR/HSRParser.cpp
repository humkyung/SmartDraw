// PreFormatParse.cpp: implementation of the CHSRParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HSRParser.h"
#include "SmartHSR.h"
#include "HSRReader.h"

//////////////////////////////////////////////////////////////////////////
#include "HSRPipe.h"
#include "CylnElement.h"
#include "SphereElement.h"
#include "TeeElement.h"
#include "FLANElement.h"
#include "BoxElement.h"
#include "MeshElement.h"
#include "3d_FaceElement.h"
#include "FACEMESHElement.h"
#include "SSLCElement.h"
#include "ArcElement.h"
#include "SweepElbow.h"
#include "HSRRevolvedElement.h"
#include "MiterElement.h"
#include "HSRExtruElement.h"
#include "HSRRevolElement.h"
#include "LoopElement.h"
#include "PlaneSurfElement.h"
#include "EllipseElement.h"
#include "HLineElement.h"
#include "FaceElement.h"
#include <IsTools.h>
#include <sstream>
#include "HSRBoundingBox.h"
#include "MUELToElbowType.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CHSRParser::CHSRParser(CHSRReader* pHSRReader)
{
	m_pHSRReader = pHSRReader;
	m_pHSRScanner = NULL;
	m_pTemplateElement  = NULL;
}

/**	
	@brief	

	@author	BHK

	@date	?

	@param

	@return
*/
CHSRParser::~CHSRParser()
{
	try
	{
		if(NULL != m_pHSRScanner)	SAFE_DELETE(m_pHSRScanner);
		if(NULL != m_pTemplateElement)	SAFE_DELETE(m_pTemplateElement);
	}
	catch(...)
	{
	}

#ifdef _DEBUG
	OutputDebugString("Destructure of CHSRParser");
#endif
}

static CHSRBoundingBox _view_bbox;
static CHSRBoundingBox _line_bbox;
static long _prev_line_group_code=-1L;
static stack<CLineElement*> _line_stack;
static stack<CLineElement*> _line_pool;
static CHSRBoundingBox _hline_bbox;
static long _prev_hline_group_code=-1L;
static stack<CHLineElement*> _hline_stack;
static stack<CHLineElement*> _hline_pool;

/**	
	@brief	The CHSRParser::Parse function
	
	@author	HumKyung Baek

	@date	????.??.??

	@param	pFilePath	a parameter of type const char*

	@return	void	
*/
void CHSRParser::Parse(LPCTSTR pSxfFilePath , const CHSRElement::MODEL_T& modelT)
{
	assert(pSxfFilePath && "pSxfFilePath is NULL");

	if(pSxfFilePath)
	{
		IFSTREAM_T file(pSxfFilePath);
		if(file.is_open())
		{
			if(NULL == m_pHSRScanner)
			{
				m_pHSRScanner = new CHSRScanner(&file);
			}
			else
			{
				m_pHSRScanner->Reset(&file);
			}
			
			if(m_pHSRScanner)
			{
				_view_bbox.SetEmpty();
				_line_bbox.SetEmpty();
				_prev_line_group_code = -1L;
				_hline_bbox.SetEmpty();
				_prev_hline_group_code = -1L;
				
				while(!file.eof() && m_pHSRScanner->ReadLine())
				{
					if(0 == m_pHSRScanner->m_nFactor) continue;
					if('#' == (*(m_pHSRScanner->m_pstrLine))[0]) continue;	/// skip comment
					this->ParseLine(modelT);

					if(file.eof()) break;
				}
				file.close();
				
				//////////////////////////////////////////////////////////////////////////
				if(!_line_stack.empty())
				{
					for(;!_line_stack.empty();_line_stack.pop())
					{
						CLineElement* ptr=_line_stack.top();
						m_pHSRReader->m_lstLineElement.push_back(ptr);
					}
				}
				
				if(!_line_pool.empty())
				{
					for(;!_line_pool.empty();_line_pool.pop())
					{
						CLineElement* ptr=_line_pool.top();
						delete ptr;
					}
				}
				
				if(!_hline_stack.empty())
				{
					for(;!_hline_stack.empty();_hline_stack.pop())
					{
						CHLineElement* ptr=_hline_stack.top();
						m_pHSRReader->m_lstHLINEFormat.push_back(ptr);
					}
				}
				
				if(!_hline_pool.empty())
				{
					for(;!_hline_pool.empty();_hline_pool.pop())
					{
						CHLineElement* ptr=_hline_pool.top();
						delete ptr;
					}
				}
			}
		}
	}
}

static double __nUOR = 1.f;
typedef struct tag_HSR_ELM_CREATION
{
	TCHAR* pTypeString;
	CHSRElement* (*pfCreate)();
}HSR_ELM_CREATION;

HSR_ELM_CREATION HSR_ELM_CREATION_MAP[]=
{
	{_T("PIPE")			, CHSRPipe::CreateInstance},
	{_T("TEE")			, CTeeElement::CreateInstance},
	{_T("<MUPI>")		, CHSRPipe::CreateInstance},
	{_T("CYLN")			, CCylnElement::CreateInstance},
	{_T("<CYLN_EQP>")	, CCylnElement::CreateInstance},
	{_T("CONE")			, CConeElement::CreateInstance},
	{_T("<CONE_EQP>")	, CConeElement::CreateInstance},
	{_T("<FLAN>")		, CFLANElement::CreateInstance},
	{_T("<BOX>")		, CBoxElement::CreateInstance},
	{_T("<SSLC>")		, CSSLCElement::CreateInstance},
	{_T("<MESH>")		, CMeshElement::CreateInstance},
	{_T("<MESH_FACE>")	, CFaceMeshElement::CreateInstance},
	{_T("<3D_FACE>")	, C3d_FaceElement::CreateInstance},
	{_T("LOOP=")		, CLoopElement::CreateInstance},
	{_T("PLANESURF=")	, CPlaneSurfElement::CreateInstance},
	{_T("EXTR=")		, CHSRExtruElement::CreateInstance},
	{_T("REVO=")		, CHSRRevolElement::CreateInstance},
	{_T("<MITER>")		, CMiterElement::CreateInstance},
	{_T("<REVOLVED>")	, CHSRRevolvedElement::CreateInstance},
	{_T("SWEEP_ELBOW=")	, CSweepElbow::CreateInstance},
	{_T("SPHERE")		, CSphereElement::CreateInstance},
	{_T("FACE=")		, CFaceElement::CreateInstance},
	{_T("ARC")		, CArcElement::CreateInstance},
	{_T("ELLIPSE")	, CEllipseElement::CreateInstance}
};

/**	
	@brief	parsing a line from text file.
	@author	HumKyung Baek
	@date	????.??.??
	@param

	@remarks\n
	;2003.03.12 - process <MUPI> item. this is equal to <PIPE>
	;2003.03.12 - process <PIPE_BEND> item.
	;2003.03.28 - process <MESH> item.
	;2003.06.02 - process <SSLC> item.
	;2003.10.18 - process <3D_FACE> , <FACE_MESH> items.
	;2004.03.29 - store current layer
	;2008.07.17 - parsing miter element.

	@return	void	
*/
void CHSRParser::ParseLine(const CHSRElement::MODEL_T& modelT)
{
	CHSRElementFactory& factory =  CHSRElementFactory::GetInstance();

	if((*m_pHSRScanner->m_aryFactor)[0] == _T("UOR"))
	{
		__nUOR = ATOF_T((*m_pHSRScanner->m_aryFactor)[1].c_str());
	}
	else if((*m_pHSRScanner->m_aryFactor)[0] == _T("[/LAYER]"))
	{ 
		CHSRApp::m_strCurLayer.clear();
	}
	else if(('[' == (*m_pHSRScanner->m_aryFactor)[0][0]) && (']' == (*m_pHSRScanner->m_aryFactor)[0][(*m_pHSRScanner->m_aryFactor)[0].length() - 1]))
	{ 
		CHSRApp::m_strCurLayer = (*m_pHSRScanner->m_aryFactor)[0].substr(1 , (*m_pHSRScanner->m_aryFactor)[0].length() - 2);	/// store current layer
	}
	else if(0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str() ,_T("<VIEW>"),6))
	{
		CHSRViewFormat* pHSRVIEWFormat=NULL;

		if(pHSRVIEWFormat = new CHSRViewFormat(__nUOR))
		{
			pHSRVIEWFormat->ParseLine(m_pHSRScanner);
			const CIsVolume vol = pHSRVIEWFormat->GetModelVolume();
			CHSRBoundingBox bbox(vol.minx() , vol.miny() , vol.minz() , vol.maxx() , vol.maxy() , vol.maxz());
			_view_bbox += bbox;
			
			m_pHSRReader->m_lstVIEWFormat.push_back(pHSRVIEWFormat);
		}
		__nUOR = 1.f;
	}
	else if((0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str(),_T("ELBOW") , 5)) ||
		    (0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str(),_T("<MUEL>"),6)))
	{
		if(0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str(),_T("<MUEL>"),6))
		{
			CMUELToElbowType MUELToElbowType(m_pHSRScanner);
		}

		CElbowElement* pElm = static_cast<CElbowElement*>(CElbowElement::CreateInstance());
		if(NULL != pElm)
		{
			pElm->ParseLine(m_pHSRScanner);
			if(m_pHSRReader->IsInDrawing(pElm))
			{
				pElm->m_model = modelT;
				m_pHSRReader->m_lstELBOWFormat.push_back(pElm);
			}
			else
			{
				factory.ReturnElement(pElm);
			}
		}
	}
	/*else if(0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str() ,_T("TEE") , 3))
	{
		CTeeElement* pElm = static_cast<CTeeElement*>(CTeeElement::CreateInstance());
		if(NULL != pElm)
		{
			const bool pElm->ParseLine(m_pHSRScanner);
			if(m_pHSRReader->IsInDrawing(pElm))
			{
				pElm->m_model = modelT;
				m_pHSRReader->m_lstTEEFormat.push_back(pElm);
			}
			else
			{
				factory.ReturnElement(pElm);
			}
		}
	}*/
	else if((0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str(),_T("<PIPE_BEND>"),11)))
	{
		/// always registered
		if(NULL == m_pTemplateElement) m_pTemplateElement = new CTemplateElement(m_pHSRReader);
		if(m_pTemplateElement)
		{
			m_pTemplateElement->ParseLine(m_pHSRScanner);
		}
	}
	else if(0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str(),_T("<TORS>"),6))
	{
		CTorsElement* pElm = static_cast<CTorsElement*>(CTorsElement::CreateInstance());
		if(NULL != pElm)
		{
			pElm->ParseLine(m_pHSRScanner);
			if(m_pHSRReader->IsInDrawing(pElm))
			{
				pElm->m_model = modelT;
				m_pHSRReader->m_lstTORSFormat.push_back(pElm);
			}
			else
			{
				factory.ReturnElement(pElm);
			}
		}
	}
	else if((0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str(),_T("<CTOR>"),6)) ||
		    (0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str(),_T("TORUS"),5)) ||
		    (0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str(),_T("TORS_EQP") , 8)))
	{
		CCTorElement* pElm = static_cast<CCTorElement*>(CCTorElement::CreateInstance());
		if(NULL != pElm)
		{
			pElm->ParseLine(m_pHSRScanner);
			if(m_pHSRReader->IsInDrawing(pElm))
			{
				pElm->m_model = modelT;
				m_pHSRReader->m_lstCTORFormat.push_back(pElm);
			}
			else
			{
				factory.ReturnElement(pElm);
			}
		}
	}
	else if(0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str(),_T("<RTOR>"),6))
	{
		CRTorElement* pElm = static_cast<CRTorElement*>(CRTorElement::CreateInstance());
		if(NULL != pElm)
		{
			pElm->ParseLine(m_pHSRScanner);
			if(m_pHSRReader->IsInDrawing(pElm))
			{
				pElm->m_model = modelT;
				m_pHSRReader->m_lstRTorElement.push_back(pElm);
			}
			else
			{
				factory.ReturnElement(pElm);
			}
		}
	}
	else if(0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str(),_T("DOME"),4))
	{
		CDomeElement* pElm = static_cast<CDomeElement*>(CDomeElement::CreateInstance());
		if(NULL != pElm)
		{
			pElm->ParseLine(m_pHSRScanner);
			if(m_pHSRReader->IsInDrawing(pElm))
			{
				pElm->m_model = modelT;
				m_pHSRReader->m_lstDomeElement.push_back(pElm);
			}
			else
			{
				factory.ReturnElement(pElm);
			}
		}
	}
	else if((0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str() , _T("LINE") , 5)))
	{
		CLineElement* pElm = static_cast<CLineElement*>(CLineElement::CreateInstance());
		if(NULL != pElm)
		{
			if(pElm->ParseLine(m_pHSRScanner))
			{
				pElm->m_model = modelT;

				if(-1 != pElm->m_nGroup)
					m_pHSRReader->m_lstLineElement.push_back(pElm);
				else	m_pHSRReader->m_lstElement.push_back(pElm);
			}
		}
	}
	else if((m_pHSRScanner->m_aryFactor->size() > 1) && (0 == STRNCMP_T((*m_pHSRScanner->m_aryFactor)[1].c_str() , _T("HLINE") , 5)))
	{	
		CHLineElement* pElm = static_cast<CHLineElement*>(CHLineElement::CreateInstance());
		if(NULL != pElm)
		{
			pElm->ParseLine(m_pHSRScanner);
			if(m_pHSRReader->IsInDrawing(pElm))
			{
				pElm->m_model = modelT;
				m_pHSRReader->m_lstHLINEFormat.push_back(pElm);
			}
			else
			{
				factory.ReturnElement(pElm);
			}
		}
	}
	else
	{
		const int nSize = sizeof(HSR_ELM_CREATION_MAP) / sizeof(HSR_ELM_CREATION_MAP[0]);
		for(int i = 0;i < nSize;++i)
		{
			if(0 == STRCMP_T((*m_pHSRScanner->m_aryFactor)[0].c_str() , HSR_ELM_CREATION_MAP[i].pTypeString))
			{
				CHSRElement* pElm = (*(HSR_ELM_CREATION_MAP[i].pfCreate))();
				if(pElm)
				{
					try
					{
						const bool res = pElm->ParseLine(m_pHSRScanner);
						if(m_pHSRReader->IsInDrawing(pElm))
						{
							pElm->m_model = modelT;
							m_pHSRReader->m_lstElement.push_back(pElm);
						}
						else
						{
							factory.ReturnElement(pElm);
						}
					}
					catch(...)
					{
						OSTRINGSTREAM_T oss;
						oss << _T("Catch at(") << m_pHSRScanner->m_nLineNo << _T("): ") << *(m_pHSRScanner->m_pstrLine);
						::OutputDebugString(oss.str().c_str());
					}
					return;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		/// extension element.
		CHSRApp* pHSRApp=CHSRApp::GetInstance();
		CHSRElementFactory* pFactory=pHSRApp->GetFactory((*m_pHSRScanner->m_aryFactor)[0].c_str());
		if(NULL != pFactory)
		{
			CHSRElement* pElm=pFactory->ParseLine(m_pHSRScanner);
			if(NULL != pElm) m_pHSRReader->AddExtElm(pElm);
		}
	}
}

/**
	@brief	create a instance mapped to typestring
	@author	humkyung
	@date	2009.11.18
	@param	rTypeString
	@return	CHSRElement*
*/
CHSRElement* CHSRParser::CreateElement(const STRING_T &rTypeString)
{
	CHSRElement* pElm = NULL;

	const int nSize = sizeof(HSR_ELM_CREATION_MAP) / sizeof(HSR_ELM_CREATION_MAP[0]);
	for(int i = 0;i < nSize;++i)
	{
		if(rTypeString == HSR_ELM_CREATION_MAP[i].pTypeString)
		{
			return (*(HSR_ELM_CREATION_MAP[i].pfCreate))();
		}
	}
	
	return NULL;
}
