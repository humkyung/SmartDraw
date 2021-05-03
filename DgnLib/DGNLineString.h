// DGNLineString.h: interface for the CDGNLineString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGNLINESTRING_H__1E4C0527_6131_4C11_9E54_6A2D6107B069__INCLUDED_)
#define AFX_DGNLINESTRING_H__1E4C0527_6131_4C11_9E54_6A2D6107B069__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <DGNElement.h>

class CDGNLineString : public CDGNElement  
{
public:
	static long Parse(PSQPRIMITIVE* ppPrimitiveList,CDGNFile* hDGN);
	CDGNLineString();
	virtual ~CDGNLineString();

};

#endif // !defined(AFX_DGNLINESTRING_H__1E4C0527_6131_4C11_9E54_6A2D6107B069__INCLUDED_)
