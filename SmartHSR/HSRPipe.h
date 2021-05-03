// HSRPipe.h: interface for the CHSRPipe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSRPIPE_H__E4A98213_3A66_405C_BAF5_95046A93F3F1__INCLUDED_)
#define AFX_HSRPIPE_H__E4A98213_3A66_405C_BAF5_95046A93F3F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiddenDef.h"
#include "Element.h"
#include "HSRFace.h"
#include "HSRScanner.h"
#include "EnumString.h"

class CHSRViewFormat;
class CHSRPipe : public CHSRElement 
{
public:
	typedef enum ENM_SINGLE_LINE_OPTION
	{
		EQUAL_OD   = 1,
		FIXED_SIZE = 2
	};

	CHSRPipe();
	virtual ~CHSRPipe();
public:
	bool ParseLine(CHSRScanner* pHSRScanner);
	bool IsInVolume(const CIsVolume& volume);
	CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID);
	virtual int Copy(CHSRElement* pCopied);

	DECLARE_HSR_ELEMENT(CHSRPipe)
protected:
	void Volume();
public:
	const double GetShapeRadius(CHSRViewFormat* pVIEWFormat,const double& nRadius);
	static void ReadEnv(LPCTSTR pIniFilePath);

	int m_nSegments;

	int m_nPIPEs;
	double  m_nRadius[3];
	POINT_T m_pt[6];
private:
	void ClipAxisByElevation(POINT_T& ptStart , POINT_T& ptEnd , bool& clipStart , bool& clipEnd , double& clipStartZ , double& clipEndZ , VECTOR_T vecAxis , const double& zhigh , const double& zlow);

	static CHSRPipe::ENM_SINGLE_LINE_OPTION m_enmSingleLineOption;
	static double m_nFixedSize;
};

#endif // !defined(AFX_HSRPIPE_H__E4A98213_3A66_405C_BAF5_95046A93F3F1__INCLUDED_)
