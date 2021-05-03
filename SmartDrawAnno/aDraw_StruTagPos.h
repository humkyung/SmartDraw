// DraftStruTagPos.h: interface for the CDraftStruTagPos class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAFTSTRUTAGPOS_H__D2E9D192_0A3D_4B29_8926_66494648C82C__INCLUDED_)
#define AFX_DRAFTSTRUTAGPOS_H__D2E9D192_0A3D_4B29_8926_66494648C82C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ExtDivision.h"
#include "SmartDrawAnnoPos.H"

namespace tag_pos
{
class CaDraw_StruTagPos : public CaDraw_TagPos  
{
public:
	virtual void Write(OFSTREAM_T& file,CExtDivision* pDivision,const double& dDrawingScale);
	CaDraw_StruTagPos();
	virtual ~CaDraw_StruTagPos();
};
};

#endif // !defined(AFX_DRAFTSTRUTAGPOS_H__D2E9D192_0A3D_4B29_8926_66494648C82C__INCLUDED_)
