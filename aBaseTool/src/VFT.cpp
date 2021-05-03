#include <assert.h>
#include "VFT.h"

char CVFT::m_szHead[4];
//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CVFT::CVFT(){
	strcpy(CVFT::m_szHead,"DVFT");

	m_nHeight = 0.;
	memset(m_ptrData,NULL,256*sizeof(VCHAR*));
	memset(m_ptrChar,NULL,256*sizeof(VCHAR_T*));
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CVFT::~CVFT(){
	for(int i = 0;i < 256;i++){
		if(m_ptrData[i]){
			free((void*)m_ptrData[i]->ptrLines->ptrPoints);
			free((void*)m_ptrData[i]->ptrLines);
			free((void*)m_ptrData[i]);
		}
	}

	for(i = 0;i < 256;i++){
		if(m_ptrChar[i]){
			VDATA_T* ptrNext=NULL;
			for(VDATA_T* ptr=m_ptrChar[i]->ptrData;ptr;ptr = ptrNext){
				free((void*)ptr->ptrFloat);
				ptrNext = ptr->next;
				free((void*)ptr);
			}
		}
	}
}

/**	\brief	The CVFT::Read function

	\param	pFilePath	a parameter of type const char*

	\return	bool	
*/
bool CVFT::Read(const char* pFilePath){
	assert(pFilePath && "pFilePath is NULL");
	bool bRet=false;

	if(pFilePath){
		FILE* fp=NULL;
		if(fp = fopen(pFilePath,"rb")){
			char szHead[13]={0,},szDesc[256]={0,};

			fread(szHead,sizeof(char),12,fp);
			char len=0;
			fread(&len,sizeof(char),1,fp);
			fread(szDesc,sizeof(char),len,fp);
			
			//////////////////////////////////////////////////////////////////////////
			/// get font name
			strcpy(m_szFontName,pFilePath);
			//////////////////////////////////////////////////////////////////////////

			m_nHeight = ReadFloat(fp);
			for(int i = 0;i < 256;i++) m_nAdvances[i] = ReadFloat(fp);
			
			while(!feof(fp)){
				ReadChar(fp);
			}
			fclose(fp);

			bRet = true;
		}
	}

	return bRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool CVFT::Load(const char* pFilePath){
	assert(pFilePath && "pFilePath is NULL");
	bool bRet=false;

	if(pFilePath){
		FILE* fp=NULL;
		if(fp = fopen(pFilePath,"rb")){
			char szHead[5]={0,};

			fread(szHead,sizeof(char),4,fp);
			if(0 == strcmp(szHead,CVFT::m_szHead)){
				m_nHeight = ReadFloat(fp);
				for(int i = 0;i < 256;i++) m_nAdvances[i] = ReadFloat(fp);
				
				while(!feof(fp)){
					LoadChar(fp);
				}
			}
			fclose(fp);

			bRet = true;
		}
	}

	return bRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
bool CVFT::Write(const char* pFilePath){
	assert(pFilePath && "pFilePath is NULL");
	bool bRet=false;

	if(pFilePath){
		FILE* fp=NULL;
		if(fp = fopen(pFilePath,"wb")){
			char *szDesc="gothice";

			fwrite("VFT Ver 1.0 ",sizeof(char),12,fp);
			char len=strlen(szDesc);
			fwrite(&len,sizeof(char),1,fp);
			fwrite(szDesc,sizeof(char),len,fp);
			
			WriteFloat(fp,m_nHeight);
			for(int i = 0;i < 256;i++) WriteFloat(fp,m_nAdvances[i]);
			for(i = 0;i < 256;i++){
				if(m_ptrData[i]){
					WriteShort(fp,m_ptrData[i]->nCharNum);
					for(int j = 0;j < m_ptrData[i]->nLines;j++){
						fprintf(fp,"%c",MOVETO);
						WriteFloat(fp,m_ptrData[i]->ptrLines[j].ptrPoints[0].x);
						WriteFloat(fp,m_ptrData[i]->ptrLines[j].ptrPoints[0].y);
						for(int k = 1;k < m_ptrData[i]->ptrLines[j].nPoints;k++){
							fprintf(fp,"%c",LINETO);
							WriteFloat(fp,m_ptrData[i]->ptrLines[j].ptrPoints[k].x);
							WriteFloat(fp,m_ptrData[i]->ptrLines[j].ptrPoints[k].y);
						}
					}
					fprintf(fp,"%c",fp);
				}
			}
			fclose(fp);

			bRet = true;
		}
	}

	return bRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CVFT::Dump(const char* pFilePath){
	assert(pFilePath && "pFilePath is NULL");

	if(pFilePath){
		FILE* fp=fopen(pFilePath,"wb");
		if(fp){
			for(int i = 0;i < 256;i++){
				if(m_ptrChar[i]) WriteChar(fp,m_ptrData[i]);
			}
			fclose(fp);
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
VCHAR_T* CVFT::GetChar(const char ch){
	return m_ptrChar[ch];
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CVFT::LoadChar(FILE* fp){
	assert(fp && "fp is NULL");

	if(fp){
		short nCharNum=ReadShort(fp);

		if((nCharNum > 0) && (nCharNum < 256)){
			if(NULL == m_ptrData[nCharNum]) m_ptrData[nCharNum] = (VCHAR*)calloc(1,sizeof(VCHAR));
			if(m_ptrData[nCharNum]){
				m_ptrData[nCharNum]->nCharNum = nCharNum;
				m_ptrData[nCharNum]->nLines   = ReadShort(fp);
				m_ptrData[nCharNum]->ptrLines = (VLINE*)calloc(m_ptrData[nCharNum]->nLines,sizeof(VLINE));
				if(m_ptrData[nCharNum]->ptrLines){
					for(int i = 0;i < m_ptrData[nCharNum]->nLines;i++){
						ReadLine(&(m_ptrData[nCharNum]->ptrLines[i]),fp);
					}
				}
			}
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CVFT::ReadChar(FILE* fp){
	assert(fp && "fp is NULL");

	if(fp){
		short nCharNum=ReadShort(fp);

		if((nCharNum >= 0) && (nCharNum < 256)){
			if(NULL == m_ptrChar[nCharNum]) m_ptrChar[nCharNum] = (VCHAR_T*)calloc(1,sizeof(VCHAR_T));
			if(m_ptrChar[nCharNum]){
				VDATA_T *pData=NULL,*pLast=NULL;
				while(pData = ReadData(fp)){
					if(NULL == pLast)
						m_ptrChar[nCharNum]->ptrData = pLast = pData;
					else{
						pLast->next = pData;
						pLast = pLast->next;
					}
				}
			}
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CVFT::WriteChar(FILE* fp,VCHAR* ptrChar){
	assert(fp      && "fp is NULL");
	assert(ptrChar && "ptrChar is NULL");

	if(fp && ptrChar){
		fprintf(fp,"%c\r\n",ptrChar->nCharNum);
		for(int i = 0;i < ptrChar->nLines;i++){
			WriteLine(fp,&(ptrChar->ptrLines[i]));
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CVFT::ReadLine(VLINE* ptrLine,FILE* fp){
	assert(ptrLine && "ptrLine is NULL");
	assert(fp      && "fp is NULL");

	if(ptrLine && fp){
		ptrLine->nPoints   = ReadShort(fp);
		ptrLine->ptrPoints = (POINT_T*)calloc(ptrLine->nPoints,sizeof(POINT_T));
		if(ptrLine->ptrPoints){
			for(int i = 0;i < ptrLine->nPoints;i++){
				ptrLine->ptrPoints[i].x = ReadFloat(fp);
				ptrLine->ptrPoints[i].y = ReadFloat(fp);
			}
		}
	}	
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
VDATA_T* CVFT::ReadData(FILE* fp){
	assert(fp && "fp is NULL");
	VDATA_T* pRet=NULL;

	if(fp){
		char code=0;
		fread(&code,sizeof(char),1,fp);

		if(code && (pRet = (VDATA_T*)calloc(1,sizeof(VDATA_T)))){
			pRet->chCode = code;
			if((MOVETO == code) || (LINETO == code)){
				pRet->ptrFloat = (float*)calloc(2,sizeof(float));
				if(pRet->ptrFloat){
					for(int i = 0;i < 2;i++)
						pRet->ptrFloat[i] = ReadFloat(fp);
				}
			}
		}
	}

	return pRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CVFT::WriteLine(FILE* fp,VLINE* ptrLine){
	assert(ptrLine && "ptrLine is NULL");
	assert(fp      && "fp is NULL");

	if(ptrLine && fp){
		for(int i = 0;i < ptrLine->nPoints;i++){
			fprintf(fp,"%lf,%lf\r\n",ptrLine->ptrPoints[i].x,ptrLine->ptrPoints[i].y);
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
short CVFT::ReadShort(FILE* fp){
	assert(fp && "fp is NULL");
	short nRet=0.;

	if(fp){
		char ch[2]={0,};
		
		fread(ch,sizeof(char),2,fp);
		
		char* ptr=(char*)&nRet;
		ptr[0] = ch[1];
		ptr[1] = ch[0];
	}

	return nRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CVFT::WriteShort(FILE* fp,short sh){
	assert(fp && "fp is NULL");

	if(fp){
		char* ptr=(char*)&sh;
		fwrite(ptr + 1,sizeof(char),1,fp);
		fwrite(ptr,sizeof(char),1,fp);
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
float CVFT::ReadFloat(FILE* fp){
	assert(fp && "fp is NULL");
	float nRet=0.;

	if(fp){
		char ch[4]={0,};
		
		fread(ch,sizeof(char),4,fp);
		
		char* ptr=(char*)&nRet;
		ptr[0] = ch[3];
		ptr[1] = ch[2];
		ptr[2] = ch[1];
		ptr[3] = ch[0];
	}

	return nRet;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CVFT::WriteFloat(FILE* fp,float fl){
	assert(fp && "fp is NULL");
	float nRet=0.;

	if(fp){
		char* ptr=(char*)&fl;
		fwrite(ptr + 3,sizeof(char),1,fp);
		fwrite(ptr + 2,sizeof(char),1,fp);
		fwrite(ptr + 1,sizeof(char),1,fp);
		fwrite(ptr,sizeof(char),1,fp);
	}
}