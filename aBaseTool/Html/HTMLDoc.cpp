#include "HTMLDoc.h"

CHTMLDoc::CHTMLDoc(const char* pszPath){
	m_pstrPath = new string(pszPath);
}

CHTMLDoc::~CHTMLDoc(){
	if(NULL != m_pstrPath) delete m_pstrPath;
}

/**	\brief	The CHTMLDoc::Name function

	\param	m_pstrPath->c_str(	a parameter of type ){return

	\return	const char*	
*/
const char* CHTMLDoc::Name(){return m_pstrPath->c_str();}
