// TinyFacetContainer.cpp: implementation of the CTinyFacetContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "TinyFacetContainer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTinyFacetContainer::CTinyFacetContainer()
{
	m_pFacetEntry = NULL;
}

CTinyFacetContainer::~CTinyFacetContainer()
{
	if(NULL != m_pFacetEntry)
	{
		for(vector<CFacet*>::iterator itr=m_pFacetEntry->begin();itr != m_pFacetEntry->end();++itr)
		{
			delete (*itr);
		}
		delete m_pFacetEntry;
	}
}

const long CTinyFacetContainer::Size()
{
	if(NULL != m_pFacetEntry) return m_pFacetEntry->size();

	return 0L;
}

CFacet* CTinyFacetContainer::GetAt(const long &nIndex)
{
	if(NULL != m_pFacetEntry)
	{
		if(nIndex < int(m_pFacetEntry->size())) return (*(m_pFacetEntry))[nIndex];
	}

	return NULL;
}

bool CTinyFacetContainer::Add(CFacet *pFacet)
{
	assert(pFacet && "pFacet is NULL");

	if(pFacet)
	{
		if(NULL == m_pFacetEntry) m_pFacetEntry = new vector<CFacet*>;
		if(NULL != m_pFacetEntry)
		{
			m_pFacetEntry->push_back(pFacet);
			return true;
		}
	}

	return false;
}
