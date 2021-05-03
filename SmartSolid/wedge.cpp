#include "wedge.h"

/**	\brief
*/
CWEdge::CWEdge(PVERTEX pStart,PVERTEX pEnd,CFace3D* pParent)
{
    	m_Attr.visible = 1;
    	m_pvtxStart = pStart;
	m_pvtxEnd   = pEnd;
    	m_pParent   = pParent;

	m_pPrev = NULL;
	m_pNext = NULL;
}

/**	\brief
*/
CWEdge::~CWEdge()
{
}

/**	\brief	The CWEdge::operator+= function\n
	append edge(s)

	\param	pWEdge	a parameter of type CWEdge*

	\return	void	
*/
void CWEdge::operator+=(CWEdge* pWEdge)
{
    	if(pWEdge)
	{
	    	CWEdge* ptr=NULL;
		for(ptr = pWEdge;ptr && ptr->Next();ptr = ptr->Next());
		assert(ptr);
		ptr->m_pNext = m_pNext;
		if(m_pNext) m_pNext->m_pPrev = ptr;

		m_pNext = pWEdge;
		pWEdge->m_pPrev = this;
	}
	else
	{
	    	m_pNext = NULL;
	}
}

/**	\brief	The CWEdge::GetParent function


	\return	CFace3D*	
*/
CFace3D* CWEdge::GetParent()
{
	return m_pParent;
}

/**	\brief	The CWEdge::SetParent function

	\param	pParent	a parameter of type CFace3D*

	\return	void	
*/
void CWEdge::SetParent(CFace3D* pParent)
{
	m_pParent = pParent;
}

/**	\brief	The CWEdge::Length function\n
	return length of edge


	\return	double	
*/
double CWEdge::Length()
{
    	double len=0.;

	return len;
}

/**	\brief	The CWEdge::operator() function\n
	set start vertex and end vertex

	\param	pStart	a parameter of type PVERTEX
	\param	pEnd	a parameter of type PVERTEX

	\return	void	
*/
void CWEdge::operator()(PVERTEX pStart,PVERTEX pEnd)
{
    	assert(pStart && "pStart is invalid");
	assert(pEnd   && "pEnd is invalid");

	if(pStart && pEnd)
	{
	    	m_pvtxStart = pStart;
		m_pvtxEnd   = pEnd;
	}
}

/**	\brief	The CWEdge::Prev function\n
	return previous vertex


	\return	CWEdge*	
*/
CWEdge* CWEdge::Prev()
{
	return m_pPrev;
}

/**	\brief	The CWEdge::Next function\n
	return next vertex


	\return	CWEdge*	
*/
CWEdge* CWEdge::Next()
{
    	return m_pNext;
}
