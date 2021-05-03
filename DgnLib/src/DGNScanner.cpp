#include "..\include\DGNScanner.h"

bool CDGNScanner::Read(CDGNFile* pFile){
	assert(pFile && "pFile is NULL");

	if(pFile && !feof(pFile->fp)){
		do{
			// check EOF
			if(feof(pFile->fp)) return false;
			// save file pointer for PushBackElement();
			pFile->m_nSavePos = ftell(pFile->fp);

			int nType=0, nWords=0, nLevel=0;
			if(4 != fread(pFile->m_aryElem,1,4,pFile->fp)) return false;
			
			// Is this an 0xFFFF endof file marker?
			if((0xff == pFile->m_aryElem[0]) && (0xff == pFile->m_aryElem[1])) return false;
			
			nWords = pFile->m_aryElem[2] + pFile->m_aryElem[3]*256;
			nType  = pFile->m_aryElem[1] & 0x7f;
			nLevel = pFile->m_aryElem[0] & 0x3f;
			
			// Read the rest of the element data into the working buffer.      
			if((unsigned)nWords != fread(pFile->m_aryElem + 4,2,nWords,hDGN->fp)) return false;
			
			pFile->m_nElemBytes = nWords*2 + 4;
			pFile->m_nElementID++;

			DGNHDR* pDGNHDR=(DGNHDR*)(pFile->m_aryElem);
		}while(pDGNHDR->deleted);
		return true;
	}

	return false;
}
