// DGNScanner.h: interface for the CDGNScanner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGNSCANNER_H__3A7DB3DA_9367_4F6F_8579_E202DE40DD51__INCLUDED_)
#define AFX_DGNSCANNER_H__3A7DB3DA_9367_4F6F_8579_E202DE40DD51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "include\DGNLib.h"
#include "include\DGNFile.h"

class CDGNScanner  
{
public:
	CDGNScanner();
	virtual ~CDGNScanner();
public:
	bool ReadElement(CDGNFile* hDGN);
	void PushBackElement(CDGNFile* hDGN);
protected:
	long m_nFilePos;
};

#endif // !defined(AFX_DGNSCANNER_H__3A7DB3DA_9367_4F6F_8579_E202DE40DD51__INCLUDED_)
