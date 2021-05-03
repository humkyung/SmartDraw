#include <string.h>
#include <assert.h>
#include "..\include\File.h"

char KFile::buf[1024]={0,};

/**	\brief
*/
KFile::KFile(){
	m_fp = NULL;
}

/**	\brief
*/
KFile::~KFile(){
	Close();
}

/**	\brief	The KFile::Read function

	\param	pText	a parameter of type char*
	\param	size	a parameter of type size_t

	\return	bool	
*/
bool KFile::Read(char* pText,size_t size){
	assert(m_fp  && "m_fp is NULL");
	assert(pText && "pText is NULL");
	bool bRet=false;

	if((m_fp && !feof(m_fp)) && pText){
		if(size == fread(pText,size,1,m_fp)){
			bRet = true;
		}
	}

	return bRet;
}

/**	\brief	The KFile::Close function


	\return	void	
*/
void KFile::Close(){
	if(m_fp){
		fclose(m_fp);
		m_fp=NULL;
	}
}

/**	\brief	The KFile::GetFileHandle function


	\return	const FILE*	
*/
const FILE* KFile::GetFileHandle(){
	return m_fp;
}

