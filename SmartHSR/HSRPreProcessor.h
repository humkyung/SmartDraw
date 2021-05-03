// KPreProcessor.h: interface for the KPreProcessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KPREPROCESSOR_H__004F7DCC_8BA0_4C54_B95D_5E8B227DCA33__INCLUDED_)
#define AFX_KPREPROCESSOR_H__004F7DCC_8BA0_4C54_B95D_5E8B227DCA33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HSRFace.h"
class CHSRPreProcessor  
{
public:
	CHSRFace* RunForSWElbow(CHSRFace* pFaceList,CHSRFace* pSide,CHSRFace* pTop,CHSRFace* pBottom);
	CHSRFace* Run(CHSRFace* pFaceList,int type,CHSRFace* pTop,CHSRFace* pSide,CHSRFace* pBottom);
	CHSRPreProcessor();
	virtual ~CHSRPreProcessor();

private:
	CHSRFace* RunForDome(CHSRFace* pFaceList,CHSRFace* pTop,CHSRFace* pSide,CHSRFace* pBottom);
	CHSRFace* RunForTee(CHSRFace* pFaceList,CHSRFace* pTop,CHSRFace* pSide,CHSRFace* pBottom);
	CHSRFace* RunForFlange(CHSRFace* pFaceList,CHSRFace* pTop,CHSRFace* pSide,CHSRFace* pBottom);
	CHSRFace* RunForPipe(CHSRFace* pFaceList,CHSRFace* pTop,CHSRFace* pSide,CHSRFace* pBottom);
	CHSRFace* RunForCylinder(CHSRFace* pFaceList,CHSRFace* pTop,CHSRFace* pSide,CHSRFace* pBottom);
	CHSRFace* RunForCone(CHSRFace* pFaceList,CHSRFace* pTop,CHSRFace* pSide,CHSRFace* pBottom);
	CHSRFace* AddElbowSymbol(CHSRFace* pFaceList,CHSRFace* pSide,CHSRFace* pFace);
	CHSRFace* RunForElbow(CHSRFace* pFace,CHSRFace* pTop,CHSRFace* pSide,CHSRFace* pBottom);
};

#endif // !defined(AFX_KPREPROCESSOR_H__004F7DCC_8BA0_4C54_B95D_5E8B227DCA33__INCLUDED_)
