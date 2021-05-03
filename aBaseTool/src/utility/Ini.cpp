#include <assert.h>
#include <utility\ini.h>
#include <fstream>
#include <windows.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CIni::CIni()
{
	m_szFilePath[0] = '\0';
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CIni::~CIni()
{
	for(list<SECTION*>::iterator itr=m_lstSection.begin();itr != m_lstSection.end();){
		delete ((*itr)->plst);
		m_lstSection.erase(itr++);
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool CIni::Read(const char* pszFilePath){
	assert(pszFilePath && "pszFilePath is NULL");
	bool bRet=false;

	if(pszFilePath){
		static char szLine[1024]={0,};
		
		ifstream file(pszFilePath);
		while(file.good()){
			memset(szLine,'\0',1024);
			file.getline(szLine,1024);
			ParseLine(szLine);
		}
		file.close();
	}
	return bRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CIni::ParseLine(const char* pszLine){
	assert(pszLine && "pszLine is NULL");

	if(pszLine){
		static char szSection[MAX_KEY_LEN]={0,};
		CIni::KEY_VALUE varKeyValue;
		char szKey[MAX_KEY_LEN]={0,},szValue[MAX_VAL_LEN]={0,};

		// SECTION.
		if(('[' == pszLine[0]) && (']' == pszLine[strlen(pszLine) - 1])){
			memset(szSection,'\0',MAX_KEY_LEN);
			
			strncpy(szSection,pszLine + 1,MAX_KEY_LEN);
			szSection[MAX_KEY_LEN - 1] = '\0';

			int nLen=strlen(szSection);
			if(']' == szSection[nLen - 1]){
				szSection[nLen - 1] = '\0';
				nLen--;
			}
			
			for(int i = 0;i < nLen;i++) szSection[i] = toupper(szSection[i]);
		}else if(strchr(pszLine,'=')){
			for(int i=0;pszLine[i] && ('=' != pszLine[i]);i++) varKeyValue.szKey[i] = pszLine[i];
			varKeyValue.szKey[i] = '\0';
			strncpy(varKeyValue.szValue,pszLine + i + 1,MAX_VAL_LEN);
			varKeyValue.szValue[MAX_VAL_LEN - 1] = '\0';
			
			int nLen=strlen(varKeyValue.szKey);
			for(i = 0;i < nLen;i++) varKeyValue.szKey[i] = toupper(varKeyValue.szKey[i]);
			nLen=strlen(varKeyValue.szValue);
			for(i = 0;i < nLen;i++) varKeyValue.szValue[i] = toupper(varKeyValue.szValue[i]);
			
			bool bFind=false;
			for(list<CIni::SECTION*>::iterator itr=m_lstSection.begin();itr != m_lstSection.end();++itr){
				if(0 == strcmp((*itr)->szSection,szSection)){
					if((*itr)->plst) (*itr)->plst->push_back(varKeyValue);
					bFind = true;
					break;
				}
			}
			if(!bFind){
				SECTION* pSection=(CIni::SECTION*)calloc(sizeof(CIni::SECTION),1);
				if(pSection){
					strcpy(pSection->szSection,szSection);
					pSection->plst = new list<CIni::KEY_VALUE>;
					pSection->plst->push_back(varKeyValue);
					m_lstSection.push_back(pSection);
				}
			}
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
char* CIni::Value(const char* pszSection,const char* pszKey){
	assert(pszSection && "pszSection is NULL");
	assert(pszKey     && "pszKey is NULL");

	if(pszSection && pszKey){
		char szSection[MAX_KEY_LEN]={0,},szKey[MAX_KEY_LEN]={0,};

		strncpy(szSection,pszSection,MAX_KEY_LEN);
		szSection[MAX_KEY_LEN - 1] = '\0';
		int nLen=strlen(szSection);
		for(int i = 0;i < nLen;i++) szSection[i] = toupper(szSection[i]);
		
		strncpy(szKey,pszKey,MAX_KEY_LEN);
		szKey[MAX_KEY_LEN - 1] = '\0';
		nLen=strlen(szKey);
		for(i = 0;i < nLen;i++) szKey[i] = toupper(szKey[i]);

		for(list<CIni::SECTION*>::iterator itr=m_lstSection.begin();itr != m_lstSection.end();++itr){
			if(0 == strcmp((*itr)->szSection,szSection)){
				for(list<CIni::KEY_VALUE>::iterator itrKeyValue=(*itr)->plst->begin();itrKeyValue != (*itr)->plst->end();++itrKeyValue){
					if(0 == strcmp(szKey,itrKeyValue->szKey)) return itrKeyValue->szValue;
				}
			}
		}
	}
	
	return NULL;
}
