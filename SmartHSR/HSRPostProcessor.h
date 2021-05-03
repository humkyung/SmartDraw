// KPostProcessor.h: interface for the KPostProcessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KPOSTPROCESSOR_H__CAC99CC3_CD32_4330_8A7F_E516D2F48307__INCLUDED_)
#define AFX_KPOSTPROCESSOR_H__CAC99CC3_CD32_4330_8A7F_E516D2F48307__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HSRFace.h"
class CHSRPostProcessor
{
public:
	void Run(CHSRFace* pFace);
	CHSRPostProcessor();
	virtual ~CHSRPostProcessor();

private:
	bool IsPointsOnLine(PPOINT_T pt1,PPOINT_T pt2,PPOINT_T pt3);
	void RunForSWElbow(CHSRFace* pFace);
	bool GetCrosslinePoints(PPOINT_T pt,CHSRFace* pFace);
	void AddCrossline(CHSRFace* pFace);
	void RunForElbow(CHSRFace* pFace);
};

#endif // !defined(AFX_KPOSTPROCESSOR_H__CAC99CC3_CD32_4330_8A7F_E516D2F48307__INCLUDED_)
