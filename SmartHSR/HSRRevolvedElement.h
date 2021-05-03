// HSRRevolvedElement.h: interface for the CRevolvedElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSRREVOLVEDELEMENT_H__C882B0C9_10E1_4809_8171_F7C8E83329EB__INCLUDED_)
#define AFX_HSRREVOLVEDELEMENT_H__C882B0C9_10E1_4809_8171_F7C8E83329EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Element.h"

#include <vector>
using namespace std;

class CHSRRevolvedElement : public CHSRElement  
{
public:
	///static const char* TypeString();
	virtual bool IsInVolume(const CIsVolume& volume);
	virtual CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);
	virtual bool ParseLine(CHSRScanner* pHSRScanner);
	CHSRRevolvedElement();
	virtual ~CHSRRevolvedElement();

	DECLARE_HSR_ELEMENT(CHSRRevolvedElement);
private:
	void Revolve(const VECTOR_T &vecAxis, vector<POINT_T>& pts , const double &nRotate, const int &nParts , const POINT_T& shift);
	vector<POINT_T> m_PointSet;
	VECTOR_T m_axis;
	double m_rotateAngle;

	POINT_T m_pts[1024];
};

#endif // !defined(AFX_HSRREVOLVEDELEMENT_H__C882B0C9_10E1_4809_8171_F7C8E83329EB__INCLUDED_)
