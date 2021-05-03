// DGNEqpNameElm.cpp: implementation of the CDGNEqpNameElm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DGNEqpNameElm.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * CDGNEqpNameElm:
 *
 * @return  
 */
CDGNEqpNameElm::CDGNEqpNameElm() : CDGNElement()
{
	memset(m_szName,'\0',20);
}

CDGNEqpNameElm::~CDGNEqpNameElm()
{

}

/**
 * GetName:
 *
 * @return const char* 
 */
const char* CDGNEqpNameElm::GetName()
{
	return m_szName;
}

/**
 * Parse:
 *
 * @param hFile 
 * @param pParser 
 * @return long 
 */
long CDGNEqpNameElm::Parse(CDGNFile *hFile, CDGNParser *pParser)
{
	assert(hFile && "hFile is NULL");
	assert(pParser && "pParser is NULL");
	long nWords=0L;

	if(hFile && pParser){
	}

	return nWords;
}
