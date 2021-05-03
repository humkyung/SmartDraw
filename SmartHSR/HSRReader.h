// HSRReader.h: interface for the CHSRReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSRREADER_H__2F24D12A_B455_4516_8FE7_32C9EE070A54__INCLUDED_)
#define AFX_HSRREADER_H__2F24D12A_B455_4516_8FE7_32C9EE070A54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
using namespace std;

#include "HiddenDef.h"
#include "HSRFace.h"
#include "HSRParser.h"

/// items
#include "HSRVIEWFormat.h"
#include "HSRPipe.h"
#include "CylnElement.h"
#include "ConeElement.h"
#include "DomeElement.h"
#include "ElbowElement.h"
#include "TeeElement.h"
#include "TorsElement.h"
#include "CTorElement.h"
#include "PreRTORFormat.h"
#include "SphereElement.h"
#include "Element.h"
#include "HSRNormalFace.h"
#include "PreHNormalFace.h"
/// up to here

class CHSRViewFormat;
class CTemplateElement;
class CHSRReader 
{
public:
	CHSRReader();
	virtual ~CHSRReader();
public:
	bool IsInDrawing(CHSRElement* pElm);	/// 2012.03.05 added by humkyung
	int ParseDraFile(LPCTSTR pDraFilePath);	/// 2012.03.05 added by humkyung

	bool AddExtElm(CHSRElement* pElm);
	void Read(const STRING_T& sSxfFolderPath);
	list<CHSRViewFormat*>* GetViewFormatList(){return &m_lstVIEWFormat;}
private:
	bool IsAdjacentView(CHSRViewFormat* pView1,CHSRViewFormat* pView2,const double nTol);
	void SetCoIncidentView(CHSRViewFormat* pView1,CHSRViewFormat* pView2);
	void Reset();
private:
	CHSRParser* m_pSxfFormatParse;

	list<CHSRViewFormat*> m_lstVIEWFormat;

	list<CDomeElement*> m_lstDomeElement;
	list<CElbowElement*> m_lstELBOWFormat;
	list<CTeeElement*> m_lstTEEFormat;
	list<CTorsElement*> m_lstTORSFormat;
	list<CCTorElement*> m_lstCTORFormat;
	list<CRTorElement*> m_lstRTorElement;
	list<CHSRElement*>	m_lstElement;
	list<CHSRNormalFace*>   m_lstNormalFace;
	list<CPreHNormalFace*>  m_lstHNormalFace;
	
	list<CLineElement*> m_lstLineElement;
	list<CHLineElement*> m_lstHLINEFormat;

	map<CHSRElement::MODEL_TYPE_T , STRING_T> m_oModelFolderMap;	/// 2012.03.05 added by humkyung

	friend class CHSRParser;
	friend class CHSRParser2xxx;
	friend class CTemplateElement;
protected:
	void CreatePlaneFace();
};

#endif // !defined(AFX_HSRREADER_H__2F24D12A_B455_4516_8FE7_32C9EE070A54__INCLUDED_)
