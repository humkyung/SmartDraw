// HSRExtruElement.h: interface for the CHSRExtruElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSREXTRUELEMENT_H__D461207F_0CC0_49ED_BD79_DA48F459E33F__INCLUDED_)
#define AFX_HSREXTRUELEMENT_H__D461207F_0CC0_49ED_BD79_DA48F459E33F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"
#include "LoopElement.h"

class __DLL_EXPORT__ CHSRExtruElement : public CHSRElement  
{
	typedef enum
	{
		SOLID = 0x01,
		SURFACE = 0x02
	}Type;
public:
	CHSRExtruElement();
	virtual ~CHSRExtruElement();
public:
	///static CHSRElement* CreateInstance();
	CHSRFace* CreateFace(CHSRViewFormat* pViewFormat,long& nID);
	bool ParseLine(CHSRScanner* pHSRScanner);
	bool IsInVolume(const CIsVolume& volume);
	///static string TypeString();

	DECLARE_HSR_ELEMENT(CHSRExtruElement);
private:
	bool CheckConnection(int conn[], const LINE_T &lhs, const LINE_T &rhs , int desconn[]);
	void AlignLines();
	
	CHSRExtruElement::Type m_Type;	//! added by HumKyung.Baek - 2010.11.12
	CLoopElement* m_pLoopElement;

	void CalcVolume();

	double m_nHeight;
	VECTOR_T m_axis;
};

#endif // !defined(AFX_HSREXTRUELEMENT_H__D461207F_0CC0_49ED_BD79_DA48F459E33F__INCLUDED_)
