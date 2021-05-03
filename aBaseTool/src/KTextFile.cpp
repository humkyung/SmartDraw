#include <assert.h>
#include <string.h>
#include "KTextFile.h"

/**	\brief
*/
KTextFile::KTextFile(){
	m_nLength = 0;
}

/**	\brief
*/
KTextFile::~KTextFile(){
}

/**	\brief	The KTextFile::Open function

	\param	pFilePath	a parameter of type const char*
	\param	mode	a parameter of type OPENMODE_T

	\return	bool	
*/
bool KTextFile::Open(const char* pFilePath,OPENMODE_T mode){
	assert(pFilePath && "pFilePath is invalid");
	bool ret = false;

	if(m_fp) Close();

	if(pFilePath){
	    	if(READ == mode){
			if(NULL != (m_fp = fopen(pFilePath,"rt"))){
				ret = true;
			}
		}else if(WRITE == mode){
		    	if(NULL != (m_fp = fopen(pFilePath,"wt"))){
			    	ret = true;
			}
		}
		m_nLength = 0;
	}
	
	return ret;
}

/**	\brief	The KTextFile::ReadLine function\n
	read a line from file.

	\param	pLine	a parameter of type char*
	\param	size	a parameter of type int

	\return	number of chars readed.
*/
int KTextFile::ReadLine(char* pLine,int size){
	assert(m_fp &&  "m_fp is invalid-[KFile::ReadLine]");
	assert(pLine && "pLine is invalid");
	int nIndex=0;

	/*
	if((fp && !feof(fp)) && pLine){
		for(wint_t ch = fgetwc(fp);!feof(fp) && (ch != '\n');ch = fgetwc(fp)){
			//while(IsWhiteChar(ch)){
			//	ch = fgetwc(fp);
			//}

			if((ch != '\r') && (ch != '\n')) pLine[nIndex++] = ch;
		}
		pLine[nIndex] = '\0';
	}
	*/

	if(m_fp && !feof(m_fp) && pLine){
		if(NULL != fgets(pLine,size,m_fp)){
			nIndex = strlen(pLine);
			pLine[nIndex - 1] = '\0';
		}
	}

	return nIndex;
}

/**	\brief	The KTextFile::IsWhiteChar function

	\param	ch	a parameter of type wint_t&

	\return	bool	
*/
bool KTextFile::IsWhiteChar(wint_t& ch){
	const static char* szWhite=" \t";
	bool ret=false;

	if(strchr(szWhite,ch)){
		ret = true;
	}

	return ret;
}

/**	\brief	The KTextFile::IsEOF function


	\return	bool	
*/
bool KTextFile::IsEOF(){
	return (feof(m_fp)) ? true : false;
}
