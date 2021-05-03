// CGridMethod.cpp: implementation of the CGridMethod class.
//
//////////////////////////////////////////////////////////////////////

#include "GridMethod.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**	\brief
*/
CGridMethod::CGridMethod()
{

}

CGridMethod::~CGridMethod()
{
}

/**	\brief	The CGridMethod::Initialize function


	\return	void	
*/
void CGridMethod::Initialize()
{
	for(list<CGrid*>::iterator itr=m_lstGrid.begin();itr != m_lstGrid.end();)
	{
		delete (*itr);
		m_lstGrid.erase(itr++);
	}
	m_lstGrid.clear();
}

/**	\brief	The CGridMethod::InsertGrid function\n
	add a grid to list of grid.

	\param	pGrid	a parameter of type CGrid*

	\return	void	
*/
void CGridMethod::InsertGrid(CGrid* pGrid)
{
	assert(pGrid && "pGrid is NULL");

	if(pGrid)
	{
		m_lstGrid.push_back(pGrid);
	}
}

/**	
	@brief	The CGridMethod::InsertPoint function

	@author	BHK

	@date	?

	@param	pt	a parameter of type const POINT_T&

	@return	void	
*/
void CGridMethod::InsertPoint(const POINT_T& pt)
{
	list<CGrid*> lstTmp;
	for(list<CGrid*>::iterator itr=m_lstGrid.begin();itr != m_lstGrid.end();)
	{
		if((*itr)->IsInnerPoint(pt))
		{
			if((*itr)->InsertPoint(lstTmp,pt))
			{
				delete (*itr);
				m_lstGrid.erase(itr++);
			}else	itr++;
		}else	itr++;
	}
	m_lstGrid.insert(m_lstGrid.end(),lstTmp.begin(),lstTmp.end());
}

/**	\brief	The CGridMethod::InsertEdge function

	\param	pEdge	a parameter of type snode<LINE_T>*

	\return	snode<LINE_T>*	
*/
snode<LINE_T>* CGridMethod::InsertEdge(snode<LINE_T>* pEdge)
{
	assert(pEdge && "pEdge is NULL");
	snode<LINE_T>* pRet=NULL;

	if(pEdge)
	{
		POINT_T pt={0};
		for(list<CGrid*>::iterator itr=m_lstGrid.begin();itr != m_lstGrid.end();++itr)
		{
			if((*itr)->DoIntersection(pt,pEdge->element))
			{
				CGrid* pGrid=*itr;
				InsertPoint(pt);
					
				snode<LINE_T>* _node=(snode<LINE_T>*)calloc(1,sizeof(snode<LINE_T>));
				if(_node)
				{
					_node->element.ptStart.x = pt.x;
					_node->element.ptStart.y = pt.y;
					_node->element.ptStart.z = 0;
					_node->element.ptEnd.x = pEdge->element.ptEnd.x;
					_node->element.ptEnd.y = pEdge->element.ptEnd.y;
					_node->element.ptEnd.z = 0;
					_node->next   = pEdge->next;
					pEdge->next   = _node;
					pEdge->element.ptEnd.x = pt.x;
					pEdge->element.ptEnd.y = pt.y;
					pEdge->element.ptEnd.z = 0;
				}
				pRet = pEdge;
				break;
			}
		}
		if(!pRet) pRet = pEdge->next;
	}

	return pRet;
}