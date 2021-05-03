// HSRRevolElement.h: interface for the CHSRRevolElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSRREVOLELEMENT_H__D461207F_0CC0_49ED_BD79_DA48F459E33F__INCLUDED_)
#define AFX_HSRREVOLELEMENT_H__D461207F_0CC0_49ED_BD79_DA48F459E33F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"
#include "LoopElement.h"

class __DLL_EXPORT__ CHSRRevolElement : public CHSRElement  
{
public:
	CHSRRevolElement();
	virtual ~CHSRRevolElement();
public:
	CHSRFace* CreateFace(CHSRViewFormat* pViewFormat,long& nID);
	bool ParseLine(CHSRScanner* pHSRScanner);
	bool IsInVolume(const CIsVolume& volume);

	DECLARE_HSR_ELEMENT(CHSRRevolElement);
private:
	bool CheckConnection(int conn[], const LINE_T &lhs, const LINE_T &rhs , int desconn[]);
	void Revolve(const VECTOR_T &vecAxis, const double &nRotate, const int &nParts , const POINT_T& shift);
	
	CLoopElement* m_pLoopElement;

	void CalcVolume();

	POINT_T m_pts[1024];
	double m_nAngle;	//! radian
	POINT_T m_ptStart , m_ptEnd;
};

#endif // !defined(AFX_HSRREVOLELEMENT_H__D461207F_0CC0_49ED_BD79_DA48F459E33F__INCLUDED_)
