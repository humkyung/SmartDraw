#include <DGNLib.h>
#include <DGNDoc.h>

/**	\brief
*/
CDGNDoc::CDGNDoc()
{
	m_pHead = NULL;
}

CDGNDoc::~CDGNDoc()
{
}

/**	\brief	The CDGNDoc::GetHead function


	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CDGNDoc::GetHead()
{
	return m_pHead;
}

/**	\brief	The CDGNDoc::CreateObject function

	\param	nSiz	a parameter of type size_t

	\return	void*	
*/
void* CDGNDoc::CreateObject(size_t nSiz)
{
	void* pRet=(void*)calloc(1,nSiz);

	return pRet;
}

/**	\brief	The CDGNDoc::Set function

	\param	pSet	a parameter of type PSQPRIMITIVE

	\return	void
*/
void CDGNDoc::Set(PSQPRIMITIVE pSet)
{
	assert(pSet && "pSet is NULL");

	if(pSet)
	{
		PSQPRIMITIVE ptrNext=NULL;
		for(PSQPRIMITIVE ptr=m_pHead;ptr;ptr=ptrNext)
		{
			ptrNext = ptr->next;
			
			if(ptr->hdr.attr.f) CDGNLib::DeleteElement(ptr);
			else free((void*)ptr);
		}
		m_pHead = pSet;
	}
}
