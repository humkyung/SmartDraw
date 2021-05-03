#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "SQLib.h"
#include "SQFormatFile.h"

//#define _SQDEBUG

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KFormatFile::KFormatFile(){
	m_fp = NULL;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KFormatFile::~KFormatFile(){
	if(m_fp) fclose(m_fp);
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KFormatFile::Close(){
	if(m_fp){
		fclose(m_fp);

		m_fp = NULL;
	}
}

//-----------------------------
//	parameter	:
//	description	:
//	remarks		:
//	returns		:
KWriteFormatFile::KWriteFormatFile(){
	m_nOffset = 0;
	pfWriteLinkageData  = NULL;
	pfGetLinkageDataSize=NULL;
}

KWriteFormatFile::KWriteFormatFile(const char* szFileName){
	if(szFileName){
		m_fp = fopen(szFileName,"wt");
		
		m_nOffset = 0;
	}
}

KWriteFormatFile::~KWriteFormatFile(){
}

bool KWriteFormatFile::Open(const char* szFileName){
	if(szFileName && m_fp){
		FILE* fp = NULL;

		if((fp = fopen(szFileName,"wb"))){
			fclose(m_fp);
			m_fp = fp;

			m_nOffset = 0;
			return true;
		}else	false;
	}else if(szFileName){
		if((m_fp = fopen(szFileName,"wb"))){
			m_nOffset = 0;

			return true;
		}else	false;
	}
	
	return false;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KWriteFormatFile::WritePrimitive(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive);

	if(/*m_fp && */pPrimitive){
		switch(pPrimitive->hdr.prefix.id){
			case SQ_GROUP:{
				PSQPRIMITIVE itr;

				pPrimitive->hdr.prefix.offset = m_nOffset;
				SetSizeOfPrimitive(pPrimitive);
				WritePrimitiveHeader(pPrimitive);
				WriteContent(pPrimitive);
				for(itr = pPrimitive->body.pPrimitive;itr;itr = itr->next)
					WritePrimitive(itr);
				}
			break;
			default:
				pPrimitive->hdr.prefix.offset = m_nOffset;
				SetSizeOfPrimitive(pPrimitive);
				WritePrimitiveHeader(pPrimitive);
				WriteContent(pPrimitive);
			break;
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KWriteFormatFile::SetSizeOfPrimitive(PSQPRIMITIVE pPrimitive){
	assert(pPrimitive);

	switch(pPrimitive->hdr.prefix.id){
		case SQ_POINT:
			pPrimitive->hdr.prefix.esize = sizeof(SQPOINT);
			if(pfGetLinkageDataSize)
				pPrimitive->hdr.prefix.tsize = pPrimitive->hdr.prefix.esize + sizeof(SQPREFIX) + pfGetLinkageDataSize(pPrimitive);
			else	pPrimitive->hdr.prefix.tsize = pPrimitive->hdr.prefix.esize + sizeof(SQPREFIX);
		case SQ_LINE:
			pPrimitive->hdr.prefix.esize = sizeof(SQLINE);
			if(pfGetLinkageDataSize)
				pPrimitive->hdr.prefix.tsize = pPrimitive->hdr.prefix.esize + sizeof(SQPREFIX) + pfGetLinkageDataSize(pPrimitive);
			else	pPrimitive->hdr.prefix.tsize = pPrimitive->hdr.prefix.esize + sizeof(SQPREFIX);
		break;
		case SQ_POINTS:
			if(pPrimitive->body.pvtPoints)
				pPrimitive->hdr.prefix.esize = (unsigned long)sizeof(SQPOINT)*pPrimitive->body.pvtPoints->size();
			if(pfGetLinkageDataSize)
				pPrimitive->hdr.prefix.tsize = pPrimitive->hdr.prefix.esize + sizeof(SQPREFIX) + pfGetLinkageDataSize(pPrimitive);
			else	pPrimitive->hdr.prefix.tsize = pPrimitive->hdr.prefix.esize + sizeof(SQPREFIX);
		break;
		case SQ_GROUP:{
			PSQPRIMITIVE itr;
			for(itr = pPrimitive->body.pPrimitive;itr;itr = itr->next){
				SetSizeOfPrimitive(itr);
				pPrimitive->hdr.prefix.esize += itr->hdr.prefix.tsize;
			}
			if(pfGetLinkageDataSize)
				pPrimitive->hdr.prefix.tsize = pPrimitive->hdr.prefix.esize + sizeof(SQPREFIX) + pfGetLinkageDataSize(pPrimitive);
			else	pPrimitive->hdr.prefix.tsize = pPrimitive->hdr.prefix.esize + sizeof(SQPREFIX);
			      }
		break;
	}
}

unsigned long KWriteFormatFile::CalcSizeofContent(PSQPRIMITIVE pPrimitive){
    	assert(pPrimitive);

	unsigned long size = 0;
	switch(pPrimitive->hdr.prefix.id){
		case SQ_LINE:
			size = (unsigned long)sizeof(SQLINE);
		break;
		case SQ_CIRCLE:
			size = (unsigned long)sizeof(SQCIRCLE);
		break;
		case SQ_POINTS:{
			if(pPrimitive->body.pvtPoints)
				size = (unsigned long)sizeof(SQPOINT)*pPrimitive->body.pvtPoints->size();
			       }
		break;
		case SQ_GROUP:
			if(pPrimitive->body.pPrimitive)
				size = pPrimitive->hdr.prefix.esize;
		break;
	}

	return size;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KWriteFormatFile::WritePrimitiveHeader(PSQPRIMITIVE pPrimitive){
    	assert(pPrimitive);
	long sizeofhdr = 0;
	int i,len;

	for(i = 0;i < pPrimitive->hdr.prefix.NumOfNames;i++)
		sizeofhdr += strlen(pPrimitive->hdr.names[i]);

	pPrimitive->hdr.prefix.tsize += sizeofhdr;
	m_nOffset += pPrimitive->hdr.prefix.tsize;

#ifdef _SQDEBUG
	fprintf(m_fp,"%d ",pPrimitive->hdr.prefix.id);
	fprintf(m_fp,"%d ",pPrimitive->hdr.prefix.layer);
	fprintf(m_fp,"%d ",pPrimitive->hdr.prefix.lcolor);
	fprintf(m_fp,"%d ",pPrimitive->hdr.prefix.ltype);
	fprintf(m_fp,"%d ",pPrimitive->hdr.prefix.lweight);
	fprintf(m_fp,"%d ",pPrimitive->hdr.prefix.group);
	fprintf(m_fp,"%d ",pPrimitive->hdr.prefix.seq);
	fprintf(m_fp,"%d ",pPrimitive->hdr.prefix.NumOfNames);
	fprintf(m_fp,"%d ",pPrimitive->hdr.prefix.offset);
	fprintf(m_fp,"%d ",pPrimitive->hdr.prefix.tsize);
	fprintf(m_fp,"%d ",pPrimitive->hdr.prefix.esize);

	for(i = 0;i < pPrimitive->hdr.prefix.NumOfNames;i++){
		len = strlen(pPrimitive->hdr.names[i]);

		fprintf(m_fp,"%d ",len);
		fprintf(m_fp,"%s ",pPrimitive->hdr.names[i]);
	}

#else
	fwrite((void*)&(pPrimitive->hdr.prefix),sizeof(SQPREFIX),1,m_fp);
	for(i = 0;i < pPrimitive->hdr.prefix.NumOfNames;i++){
		len = strlen(pPrimitive->hdr.Names[i]);

		fwrite((void*)&len,sizeof(int),1,m_fp);
		fprintf(m_fp,"%s",pPrimitive->hdr.Names[i]);
	}
#endif
}

void KWriteFormatFile::WriteContent(PSQPRIMITIVE pPrimitive){
    	assert(pPrimitive);

#ifdef	_SQDEBUG
	if(m_fp){
		switch(pPrimitive->hdr.prefix.id){
			case SQ_LINE:
				fprintf(m_fp,"%lf ",pPrimitive->body.line.ptStart.x);
				fprintf(m_fp,"%lf ",pPrimitive->body.line.ptStart.y);
				fprintf(m_fp,"%lf ",pPrimitive->body.line.ptStart.z);

				fprintf(m_fp,"%lf ",pPrimitive->body.line.ptEnd.x);
				fprintf(m_fp,"%lf ",pPrimitive->body.line.ptEnd.y);
				fprintf(m_fp,"%lf ",pPrimitive->body.line.ptEnd.z);

			break;
			case SQ_CIRCLE:
				fprintf(m_fp,"%lf ",pPrimitive->body.circle.ptOrigin.x);
				fprintf(m_fp,"%lf ",pPrimitive->body.circle.ptOrigin.y);
				fprintf(m_fp,"%lf ",pPrimitive->body.circle.ptOrigin.z);

				fprintf(m_fp,"%lf ",pPrimitive->body.circle.radius);
			break;
			case SQ_POINTS:{
				assert(pPrimitive->body.pvtPoints);

				vector<SQPOINT>::iterator itr;
				for(itr = pPrimitive->body.pvtPoints->begin();itr != pPrimitive->body.pvtPoints->end();++itr){
				    	fprintf(m_fp,"%lf ",itr->x);
					fprintf(m_fp,"%lf ",itr->y);
				    	fprintf(m_fp,"%lf ",itr->z);
				}
					}
			break;
		}
		
		fprintf(m_fp,"\r\n");
	}
#else
	if(m_fp){
		switch(pPrimitive->hdr.prefix.id){
			case SQ_LINE:
				fwrite((void*)&(pPrimitive->body.line),sizeof(SQLINE),1,m_fp);
			break;
			case SQ_CIRCLE:
				fwrite((void*)&(pPrimitive->body.circle),sizeof(SQCIRCLE),1,m_fp);
			break;
			case SQ_POINTS:{
				vector<SQPOINT>::iterator itr;
				for(itr = pPrimitive->body.pvtPoints->begin();itr != pPrimitive->body.pvtPoints->end();++itr)
				    	fwrite((void*)itr,sizeof(SQPOINT),1,m_fp);
			break;
		}
	}
#endif
}

//---------------------------
KLoadFormatFile::KLoadFormatFile(){
	m_nLength = 0;
}

KLoadFormatFile::KLoadFormatFile(const char* szFileName){
	m_nLength = 0;

	if(szFileName){
		m_fp = fopen(szFileName,"rb");
		
		fseek(m_fp,0,SEEK_END);
		m_nLength = ftell(m_fp);
		fseek(m_fp,0,SEEK_SET);
	}
}

KLoadFormatFile::~KLoadFormatFile(){
}

bool KLoadFormatFile::Open(const char* szFileName){
	if(szFileName && m_fp){
		FILE* fp = NULL;

		if((fp = fopen(szFileName,"rb"))){
			fclose(m_fp);
			m_fp = fp;

			fseek(m_fp,0,SEEK_END);
			m_nLength = ftell(m_fp);
			fseek(m_fp,0,SEEK_SET);

			return true;
		}else	false;
	}else if(szFileName){
		if(NULL != (m_fp = fopen(szFileName,"rb"))){
			fseek(m_fp,0,SEEK_END);
			m_nLength = ftell(m_fp);
			fseek(m_fp,0,SEEK_SET);

			return true;
		}else	false;
	}
	
	return false;
}

//	paramter	:
//	description	: load a primitive from file.
//	remarks		:
//	returns		:
PSQPRIMITIVE KLoadFormatFile::LoadPrimitive(){
	PSQPRIMITIVE primitive = NULL;

	if(m_fp && !feof(m_fp)){
		if(NULL != (primitive = (PSQPRIMITIVE)malloc(sizeof(SQPRIMITIVE)))){
			memset((void*)primitive,0,sizeof(SQPRIMITIVE));
			LoadHeader(primitive);
			
			switch(primitive->hdr.prefix.id){
				case SQ_GROUP:{
					PSQPRIMITIVE pRet;
					for(unsigned int i = 0;i < primitive->hdr.prefix.esize;){
						pRet = LoadPrimitive();
						if(pRet){
							primitive->body.pPrimitive = ListPush(primitive->body.pPrimitive,pRet);
							i += pRet->hdr.prefix.tsize;
						}
					}
					      }
				break;
				default:
					LoadContent(primitive);
				break;
			}
		}
	}

	return primitive;
}

void KLoadFormatFile::LoadHeader(PSQPRIMITIVE pPrimitive){
    	assert(pPrimitive);
	int i,len;
	
#ifdef	_SQDEBUG
	fscanf(m_fp,"%d",&(pPrimitive->hdr.prefix.id));
	fscanf(m_fp,"%d",&(pPrimitive->hdr.prefix.layer));
	fscanf(m_fp,"%d",&(pPrimitive->hdr.prefix.lcolor));
	fscanf(m_fp,"%d",&(pPrimitive->hdr.prefix.ltype));
	fscanf(m_fp,"%d",&(pPrimitive->hdr.prefix.lweight));
	fscanf(m_fp,"%d",&(pPrimitive->hdr.prefix.group));
	fscanf(m_fp,"%d",&(pPrimitive->hdr.prefix.seq));
	fscanf(m_fp,"%d",&(pPrimitive->hdr.prefix.NumOfNames));
	fscanf(m_fp,"%d",&(pPrimitive->hdr.prefix.offset));
	fscanf(m_fp,"%d",&(pPrimitive->hdr.prefix.tsize));
	fscanf(m_fp,"%d",&(pPrimitive->hdr.prefix.esize));

	if(pPrimitive->hdr.prefix.NumOfNames){
		pPrimitive->hdr.names = (PNAME*)malloc(sizeof(PNAME)*(pPrimitive->hdr.prefix.NumOfNames));
		for(i = 0;i < pPrimitive->hdr.prefix.NumOfNames;i++){
			fscanf(m_fp,"%d",&len);

			pPrimitive->hdr.names[i] = (char*)malloc(sizeof(char)*len + 1);
			fscanf(m_fp,"%s",pPrimitive->hdr.names[i]);
		}
	}
#else
	fread((void*)&(pPrimitive->hdr.prefix),sizeof(SQPREFIX),1,m_fp);
	if(pPrimitive->hdr.prefix.NumOfNames){
		pPrimitive->hdr.names = (char**)malloc(sizeof(char*)*(pPrimitive->hdr.prefix.NumOfNames));
		for(i = 0;i < pPrimitive->hdr.prefix.NumOfNames;i++){
			fread((void*)&len,sizeof(int),1,m_fp);
			
			pPrimitive->hdr.names[i] = (char*)malloc(sizeof(char)*len + 1);
			fread((void*)(pPrimitive->hdr.names[i]),sizeof(char)*len,1,m_fp);
			pPrimitive->hdr.names[i][len] = '\0';
		}
	}
#endif
}

void KLoadFormatFile::LoadContent(PSQPRIMITIVE pPrimitive){
    	assert(pPrimitive);

#ifdef	_SQDEBUG
	switch(pPrimitive->hdr.prefix.id){
		case SQ_LINE:
			fscanf(m_fp,"%lf",&(pPrimitive->body.line.ptStart.x));
			fscanf(m_fp,"%lf",&(pPrimitive->body.line.ptStart.y));
			fscanf(m_fp,"%lf",&(pPrimitive->body.line.ptStart.z));

			fscanf(m_fp,"%lf",&(pPrimitive->body.line.ptEnd.x));
			fscanf(m_fp,"%lf",&(pPrimitive->body.line.ptEnd.y));
			fscanf(m_fp,"%lf",&(pPrimitive->body.line.ptEnd.z));
		break;
		case SQ_CIRCLE:
			fscanf(m_fp,"%lf",&(pPrimitive->body.circle.ptOrigin.x));
			fscanf(m_fp,"%lf",&(pPrimitive->body.circle.ptOrigin.y));
			fscanf(m_fp,"%lf",&(pPrimitive->body.circle.ptOrigin.z));
			
			fscanf(m_fp,"%lf",&(pPrimitive->body.circle.radius));
		break;
		case SQ_POINTS:{
			if(NULL != (pPrimitive->body.pvtPoints = new vector<SQPOINT>)){
			    	int i,cnt = pPrimitive->hdr.prefix.esize/sizeof(SQPOINT);
				SQPOINT pt;
				
				for(i = 0;i < cnt;i++){
				    	fscanf(m_fp,"%lf",&(pt.x));
				    	fscanf(m_fp,"%lf",&(pt.y));
				    	fscanf(m_fp,"%lf",&(pt.z));
					
					pPrimitive->body.pvtPoints->push_back(pt);
				}
			}
			       }
		break;
	}
#else
	switch(pPrimitive->hdr.prefix.id){
		case SQ_LINE:
			fread((void*)&(pPrimitive->body.line),sizeof(SQLINE),1,m_fp);
		break;
		case SQ_CIRCLE:
			fread((void*)&(pPrimitive->body.circle),sizeof(SQCIRCLE),1,m_fp);
		break;
		case SQ_POINTS:{
			if(pPrimitive->body.pvtPoints = new vector<SQPOINT>){
			    	int i,cnt = pPrimitive->esize/sizeof(SQPOINT);
				SQPOINT pt;
				
				for(i = 0;i < cnt;i++){
				    	fread((void*)&pt,sizeof(SQPOINT),1,m_fp);
					
					pPrimitive->body.pvtPoints->push_back(pt);
				}
			}
			       }
		break;
	}
#endif
}
