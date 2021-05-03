#include <assert.h>
#include "HTMLParse.h"

/**	\brief
*/
CHTMLParser::CHTMLParser(){
	m_pszBuffer       = NULL;
	m_ptr = m_ptrPrev = NULL;
}

/**	\brief
*/
CHTMLParser::~CHTMLParser(){
	for(list<CHTMLTable*>::iterator itr=m_lstHTMLTable.begin();itr != m_lstHTMLTable.end();){
		delete (*itr);
		m_lstHTMLTable.erase(itr++);
	}
}

/**	\brief	The CHTMLParser::GetNextToken function


	\return	char*	
*/
char* CHTMLParser::GetNextToken(){
    	static char szToken[256]={0,};

	for(;*m_ptr && (('\t' == *m_ptr) || (' ' == *m_ptr) || ('\r' == *m_ptr) || ('\n' == *m_ptr));m_ptr++);
	m_ptrPrev = m_ptr;
	
	if(('<' == *m_ptr) || ('>' == *m_ptr) || ('=' == *m_ptr)){
		szToken[0] = *m_ptr++;
		szToken[1] = '\0';
	}else{
		for(int i = 0;*m_ptr;){
			//szToken[i++] = tolower(*m_ptr++);
			szToken[i++] = (*m_ptr++);
			if((('<' == *m_ptr) || ('>' == *m_ptr) || ('=' == *m_ptr) || (' ' == *m_ptr))) break;
			
		}
		szToken[i] = '\0';
	}

	return (szToken[0]) ? szToken : NULL;
}

/**	\brief	The CHTMLParser::GetContentToken function


	\return	char*	
*/
char* CHTMLParser::GetContentToken(){
    	static char szToken[256]={0,};

	for(;*m_ptr && (('\t' == *m_ptr) || ('\r' == *m_ptr) || ('\n' == *m_ptr));m_ptr++);
	m_ptrPrev = m_ptr;
	
	if(('<' == *m_ptr) || ('>' == *m_ptr) || ('=' == *m_ptr)){
		szToken[0] = *m_ptr++;
		szToken[1] = '\0';
	}else{
		for(int i = 0;*m_ptr;){
			szToken[i++] = (*m_ptr++);
			if((('<' == *m_ptr) || ('>' == *m_ptr) || ('=' == *m_ptr))) break;
			
		}
		szToken[i] = '\0';
	}

	return (szToken[0]) ? szToken : NULL;
}

/**	\brief	The CHTMLParser::PushBackToken function


	\return	void	
*/
void CHTMLParser::PushBackToken(){
    	m_ptr = m_ptrPrev;
}

/**	\brief	The CHTMLParser::Parse function

	\param	pszBuffer	a parameter of type const char*

	\return	void	
*/
void CHTMLParser::Parse(const char* pszBuffer){
	assert(pszBuffer && "pszBuffer is NULL");

	if(pszBuffer){
	    	//strncpy(m_szBuffer,pszBuffer,102400);
		m_pszBuffer = (char*)pszBuffer;
		m_ptr = m_pszBuffer;
		Parse();
	}
}

/**	\brief	The CHTMLParser::Parse function


	\return	void	
*/
void CHTMLParser::Parse(){
	char* pszToken=NULL;
	while(pszToken = GetNextToken()){
		if(0 == strcmp(pszToken,"<")) 		ParseKeyword();
		else if(0 == strcmp(pszToken,">")) 	return;// error!!
	}
}

/**	\brief	The CHTMLParser::ParseKeyword function


	\return	void	
*/
void CHTMLParser::ParseKeyword(){
	char* pszToken=GetNextToken();
	if(pszToken){
		if(0 == strcmp(pszToken,"table")) 
		    	ParseTable();
		else if(0 == strcmp(pszToken,"tr"))
		    	ParseTR();
		else if(0 == strcmp(pszToken,"td"))
		    	ParseTD();
		else if(0 == strcmp(pszToken,"/tr"))
			GetNextToken();
		else if(0 == strcmp(pszToken,"/td"))
			GetNextToken();
		else if(0 == strcmp(pszToken,"/table"))
			GetNextToken();
	}
}

/**	\brief	The CHTMLParser::ParseTable function


	\return	void	
*/
void CHTMLParser::ParseTable(){
    	CHTMLTable* pHTMLTable=new CHTMLTable;
	if(pHTMLTable){
		char* pszToken=NULL;
		while(pszToken = GetNextToken()){
		    	if(0 == strcmp(pszToken,"x")){
			    	pszToken = GetNextToken();
				if(0 == strcmp(pszToken,"=")){
					pszToken = GetNextToken();
					pHTMLTable->SetX(atof(pszToken));
				}else	return; // error!!
			}else if(0 == strcmp(pszToken,"y")){
				pszToken = GetNextToken();
				if(0 == strcmp(pszToken,"=")){
					pszToken = GetNextToken();
					pHTMLTable->SetY(atof(pszToken));
				}else	return; // error!!
			}else if(0 == strcmp(pszToken,"height")){
				pszToken = GetNextToken();
				if(0 == strcmp(pszToken,"=")){
					pszToken = GetNextToken();
					atof(pszToken);	
				}else	return; // error!!
			}else if(0 == strcmp(pszToken,"color")){
				pszToken = GetNextToken();
				if(0 == strcmp(pszToken,"=")){
				    	strcpy(pHTMLTable->Color(),GetNextToken());
				}else return; // error!!
			}else if(0 == strcmp(pszToken,"border")){
				pszToken = GetNextToken();
				if(0 == strcmp(pszToken,"=")){
					pHTMLTable->Border() = (0 == strcmp(GetNextToken(),"on")) ? true : false;
				}else return; // error!!
			}else if(0 == strcmp(pszToken,">")){
			    	m_lstHTMLTable.push_back(pHTMLTable);
				break;
			}
		}
	}
}

/**	\brief	The CHTMLParser::ParseTR function


	\return	void	
*/
void CHTMLParser::ParseTR(){
	CHTMLRow* pHTMLRow=new CHTMLRow;

	if(pHTMLRow){
		char* pszToken=NULL;
		while(pszToken = GetNextToken()){
			if(0 == strcmp(pszToken,"height")){
				pszToken = GetNextToken();
				if(0 == strcmp(pszToken,"=")){
					pHTMLRow->Height() = atof(GetNextToken());
				}else	return; // error!!
			}else if(0 == strcmp(pszToken,"color")){
				pszToken = GetNextToken();
				if(0 == strcmp(pszToken,"=")){
				    	pszToken = GetNextToken();
				}else return; // error!!
			}else if(0 == strcmp(pszToken,">")){
				CHTMLTable* pHTMLTable=m_lstHTMLTable.back();
				if(pHTMLTable)	pHTMLTable->Add(pHTMLRow);
				break;
			}
		}
	}
}

/**	\brief	The CHTMLParser::ParseTD function


	\return	void	
*/
void CHTMLParser::ParseTD(){
    	CHTMLCol* pHTMLCol=new CHTMLCol;

	if(pHTMLCol){
		char* pszToken=NULL;
		while(pszToken = GetNextToken()){
			if(0 == strcmp(pszToken,"width")){
				pszToken = GetNextToken();
				if(0 == strcmp(pszToken,"=")){
					pHTMLCol->Width() = atof(GetNextToken());
				}else	return; // error!!
			}else if(0 == strcmp(pszToken,"height")){
				pszToken = GetNextToken();
				if(0 == strcmp(pszToken,"=")){
					pszToken = GetNextToken();
					atof(pszToken);	
				}else	return; // error!!
			}else if(0 == strcmp(pszToken,"color")){
				pszToken = GetNextToken();
				if(0 == strcmp(pszToken,"=")){
			    		pszToken = GetNextToken();
				}else return;  // error!!
			}else if(0 == strcmp(pszToken,"text_color")){
				pszToken = GetNextToken();
				if(0 == strcmp(pszToken,"=")){
					pszToken = GetNextToken();
					strcpy(pHTMLCol->TextColor(),pszToken);
				}else	return; // error!!
			}else if(0 == strcmp(pszToken,"text_height")){
				pszToken = GetNextToken();
				if(0 == strcmp(pszToken,"=")){
					pHTMLCol->TextHeight() = atof(GetNextToken());
				}else	return; // error!!
			}else if(0 == strcmp(pszToken,"<")){
			    	pszToken = GetNextToken();
				if(0 == strcmp(pszToken,"\\td")){
					pszToken = GetNextToken();
					if(0 == strcmp(pszToken,">")) break;
				}	    
			}else if(0 == strcmp(pszToken,">")){
			    	while(0 != strcmp(pszToken = GetContentToken(),"<")) pHTMLCol->Add(pszToken);

				CHTMLTable* pHTMLTable=m_lstHTMLTable.back();
				if(pHTMLTable)	pHTMLTable->Add(pHTMLCol);

				PushBackToken();
				break;
			}
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHTMLParser::Write(const char* pszFilePath,const char* ptrMode){
    	assert(pszFilePath && "pszFilePath is NULL");
	assert(ptrMode && "ptrMode is NULL");

	if(pszFilePath && ptrMode){
	    	FILE* fp=fopen(pszFilePath,ptrMode);
		if(fp){
		    	for(list<CHTMLTable*>::iterator itr=m_lstHTMLTable.begin();itr != m_lstHTMLTable.end();++itr){
				(*itr)->Resize();
			    	(*itr)->Write(fp);
			}
			fclose(fp);
		}
	}
}
