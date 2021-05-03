#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "KTokenizer.h"

/**	\brief
*/
KTokenizer::KTokenizer(){
	m_pszDelimiter = NULL;

    	m_pTokenString = NULL;
	m_nSearchIndex = 0;
}

/**	\brief
*/
KTokenizer::~KTokenizer(){
}

/**	\brief	The KTokenizer::SetDelimiter function

	\param	pszDelimiter	a parameter of type char*

	\return	void	
*/
void KTokenizer::SetDelimiter(const char* pszDelimiter){
	m_pszDelimiter = (char*)pszDelimiter;
}

/**	\brief	The KTokenizer::SetTokenString function

	\param	pszTokenString	a parameter of type char*

	\return	void	
*/
void KTokenizer::SetTokenString(char* pszTokenString){
    	assert(pszTokenString && "pszTokenString is invalid");

    	m_pTokenString = pszTokenString;
	m_nSearchIndex = 0;
}

/**	\brief	The KTokenizer::GetFirstToken function

	\param	pszToken	a parameter of type char*

	\return	bool	
*/
bool KTokenizer::GetFirstToken(char* pszToken){
    	assert(pszToken       && "pszToken is invalid");
	assert(m_pTokenString && "m_pTokenString is not setted.");
	bool ret=false;

	if(pszToken && m_pTokenString){
		m_nSearchIndex = 0;
		GetNextToken(pszToken);	
	}

	return ret;
}

/**	\brief	The KTokenizer::GetNextToken function

	\param	pszToken	a parameter of type char*

	\return	bool	
*/
bool KTokenizer::GetNextToken(char* pszToken){
    	assert(pszToken && "pszToken is invalid");
	bool ret=false;
	int i = 0 ;
	if(pszToken){
		if(pszToken && m_pTokenString[m_nSearchIndex]){
			while(strchr(m_pszDelimiter,m_pTokenString[m_nSearchIndex])) m_nSearchIndex++;
			for(i = 0;m_pTokenString[m_nSearchIndex + i];i++){
				if(strchr(m_pszDelimiter,m_pTokenString[m_nSearchIndex + i])){
					break;
				}else{
					pszToken[i] = m_pTokenString[m_nSearchIndex + i];
				}
			}			
			pszToken[i] = '\0';
			m_nSearchIndex += i;
			
			ret = true;
		}
	}

	return ret;
}
