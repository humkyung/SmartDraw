#include "htmlib.h"

CHTMLLib::CHTMLLib(){
	m_pvecDoc = NULL;
}

CHTMLLib::~CHTMLLib(){
	if(NULL != m_pvecDoc){
	}
}

/**	\brief	The CHTMLLib::GetDoc function

	\param	nIndex	a parameter of type const unsigned int

	\return	CHTMLDoc*	
*/
CHTMLDoc* CHTMLLib::GetDoc(const unsigned int nIndex){
	if((NULL != m_pvecDoc) && (nIndex < m_pvecDoc->size())){
		return (*m_pvecDoc)[nIndex];
	}else return NULL;
}
