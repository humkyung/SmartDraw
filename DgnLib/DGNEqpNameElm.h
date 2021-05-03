// DGNEqpNameElm.h: interface for the CDGNEqpNameElm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DGNEQPNAMEELM_H__856CDD2F_75F1_4E83_ABFC_AC49BA812BDE__INCLUDED_)
#define AFX_DGNEQPNAMEELM_H__856CDD2F_75F1_4E83_ABFC_AC49BA812BDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DGNElement.h"
#include "DGNParser.h"

class CDGNEqpNameElm : public CDGNElement
{
public:
	long Parse(CDGNFile* hFile,CDGNParser* pParser);
	const char* GetName();
	CDGNEqpNameElm();
	virtual ~CDGNEqpNameElm();
private:
	char m_szName[20];
};

#endif // !defined(AFX_DGNEQPNAMEELM_H__856CDD2F_75F1_4E83_ABFC_AC49BA812BDE__INCLUDED_)
