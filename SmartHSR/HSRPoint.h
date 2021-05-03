// HSRPoint.h: interface for the CHSRPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSRPOINT_H__CA8B269A_60CE_4F74_A6C9_F2F4F1B01DEF__INCLUDED_)
#define AFX_HSRPOINT_H__CA8B269A_60CE_4F74_A6C9_F2F4F1B01DEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <..\Primitive.h>

class CHSRPoint : public POINT_T
{
public:
	enum
	{
		PRIMARY			= 0,
		CONSTRUCTION	= 1
	};

	CHSRPoint();
	CHSRPoint(const CHSRPoint& rhs);
	CHSRPoint(const POINT_T& rhs);
	CHSRPoint& operator =(const CHSRPoint& rhs);
	virtual ~CHSRPoint();
public:
	unsigned char m_code;	/// 0 - , 1 - construction point
};

#endif // !defined(AFX_HSRPOINT_H__CA8B269A_60CE_4F74_A6C9_F2F4F1B01DEF__INCLUDED_)
