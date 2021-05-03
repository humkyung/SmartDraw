// PreVIEWFormat.h: interface for the CHSRViewFormat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREVIEWFORMAT_H__F6162A4A_9805_47E2_A346_78E26322A919__INCLUDED_)
#define AFX_PREVIEWFORMAT_H__F6162A4A_9805_47E2_A346_78E26322A919__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SQLib.h>
#include <IsTools.h>
#include <IsVolume.h>

#include "Markup/Markup.h"
#include "HSRScanner.h"

#include "HSRPipe.h"
#include "CylnElement.h"
#include "ConeElement.h"
#include "ElbowElement.h"
#include "TorsElement.h"
#include "CTorElement.h"
#include "RTorElement.h"
#include "TeeElement.h"
#include "DomeElement.h"
#include "SphereElement.h"
#include "Element.h"
#include "HSRNormalFace.h"
#include "PreHNormalFace.h"

/**
 * class CHSRViewFormat:
 *
 * 2005.12.30 : add m_UOR member for PDS.
 * @author 
 */

class CHSRViewFormat
{
public:
	typedef enum
	{
		TOP_VIEW_T		= 0x01,
		BOTTOM_VIEW_T	= 0x02,
		LEFT_VIEW_T		= 0x03,
		RIGHT_VIEW_T	= 0x04,
		FRONT_VIEW_T	= 0x05,
		BACK_VIEW_T		= 0x06,
		ISO1_VIEW_T		= 0x07,
		ISO2_VIEW_T		= 0x08,
		ISO3_VIEW_T		= 0x09
	}DIRECTIONT_T;

	typedef enum
	{
		R0_T	= 0,
		R90_T	= 90,
		R180_T	= 180,
		R270_T	= 270
	}ROTATE_T;

	typedef enum
	{
		RECTANGLE_VIEW	= 0,
		QUADRANT1_VIEW	= 1,
		QUADRANT2_VIEW	= 2,
		QUADRANT3_VIEW	= 3,
		QUADRANT4_VIEW	= 4
	}ViewType;

	CHSRViewFormat(const double& nUOR=1.f);
	virtual ~CHSRViewFormat();
public:
	bool DoesInclude(const CHSRElement::MODEL_T& modelT);	/// 2012.03.05 added by humkyung
	DIRECTIONT_T GetViewDirFrom(const STRING_T& sViewDir);	/// 2012.03.05 added by humkyung
	ROTATE_T GetViewAngleFrom(const STRING_T& sViewAngle);	/// 2012.03.05 added by humkyung
	CHSRElement::MODEL_TYPE_T GetModelType(const STRING_T& rModelType);	/// 2012.03.05 added by humkyung
	static CHSRViewFormat* CreateInstance();				/// 2012.03.05 added by humkyung
	int Parse(CMarkup& xml);								/// 2012.03.05 added by humkyung
	
	void ParseLine(CHSRScanner* pHSRScanner);
	void ParseLine3xxx(CHSRScanner* pHSRScanner);
	bool IsIn(CHSRViewFormat* pView);
	void SetViewOrigin(const POINT_T ptOrigin){m_ptCenter = ptOrigin;}
	const CIsVolume GetModelVolume(){return m_volume;}
	const POINT_T  GetViewOrigin(){return m_ptCenter;}
	const double   GetViewWidth(){return m_nWidth;}
	const double   GetViewHeight(){return m_nHeight;}
	POINT_T MODEL2VIEW(const POINT_T pt);
	VECTOR_T MODEL2VIEW(const VECTOR_T vec);
	double   MODEL2VIEW(const double nValue);
	CHSRFace* GetViewFace(){return m_pViewFace;}
	CHSRFace* CreateFace(long& nID);

	bool operator +=(CElbowElement* pPreELBOWFormat);
	bool operator +=(CTeeElement* pTeeElm);
	bool operator +=(CDomeElement* pDomeElm);
	bool operator +=(CTorsElement* pPreTORSFormat);
	bool operator +=(CCTorElement* pPreCTORFormat);
	bool operator +=(CRTorElement* pRTorElm);
	bool operator +=(CSphereElement* pPreSPHEREFormat);
	bool operator +=(CHSRElement* pElement);
	bool operator +=(CHSRNormalFace* pPreNormalFace);
	bool operator +=(CPreHNormalFace* pPreHNormalFace);
private:
	void CalculateViewInfo();
	STRING_T m_sName;	/// 2012.03.05 added by humkyung
public:
	VOLUME_T GetViewVolume();
	const DIRECTIONT_T GetViewDir() const;
	POINT_T  m_ptModelCenter;

	POINT_T  m_ptCenter;
	ROTATE_T m_rotate;
	double   m_nScale;

	double   m_oRMatrix[3][3];
	ViewType m_ViewType;

	list<CHSRElement::MODEL_T> m_oModelList;
private:
	POINT_T  m_ptViewLowLeft;

	CIsVolume m_volume;
	
	DIRECTIONT_T  m_direction;
	
	double m_nWidth;
	double m_nHeight;
	double m_nUOR;	/// for PDS

	CHSRFace* m_pViewFace;

	list<CElbowElement*> m_lstELBOWFormat;
	list<CTeeElement*> m_lstTEEFormat;
	list<CTorsElement*>   m_lstTORSFormat;
	list<CCTorElement*> m_lstCTORFormat;
	list<CRTorElement*> m_lstRTorElement;
	list<CDomeElement*> m_lstDomeElement;
	list<CHSRElement*>	m_lstElement;
	list<CHSRNormalFace*>   m_lstNormalFace;
	list<CPreHNormalFace*>  m_lstHNormalFace;
};

#endif // !defined(AFX_PREVIEWFORMAT_H__F6162A4A_9805_47E2_A346_78E26322A919__INCLUDED_)
