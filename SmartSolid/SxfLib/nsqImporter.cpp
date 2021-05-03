#include "KTokenizer.h"
#include "NSQImporter.h"

#include <fstream>
using namespace std;

/**	\brief
*/
CNSQImporter::CNSQImporter(){
}

/**	\brief
*/
CNSQImporter::~CNSQImporter()
{
	for(list<char*>::iterator itr=m_lstLine.begin();itr != m_lstLine.end();)
	{
		free((void*)*itr);
		m_lstLine.erase(itr++);
	}
}

/**	\brief	The CNSQImporter::OnImportFile function

	\param	pFilePath	a parameter of type const char*

	\return	PSQPRIMITIVE	
*/
PSQPRIMITIVE CNSQImporter::OnImportFile(const char* pFilePath)
{
	assert(pFilePath && "pFilePath is NULL");
	PSQPRIMITIVE pRet=NULL;

	if(pFilePath)
	{
		static char szLine[1024]={0,};

		/// read file
		ifstream file(pFilePath);
		while(!file.eof())
		{
			file.getline(szLine,1024);
			int len=strlen(szLine);
			if(len > 0)
			{
				char *pLine = (char*)calloc(1,len + 1);
				if(pLine)
				{
					strcpy(pLine,szLine);
					m_lstLine.push_back(pLine);
				}
			}
		}
		file.close();
		///

		if(!m_lstLine.empty())
		{
			long group=-1,ngroup=-1,seq=0;
			LINE_T line={0};
			PSQPRIMITIVE pGroup=NULL;
			KTokenizer tokenizer;
			tokenizer.SetDelimiter(",");
			for(list<char*>::iterator itr=m_lstLine.begin();itr != m_lstLine.end();++itr)
			{
				tokenizer.SetTokenString(*itr);
				
				tokenizer.GetNextToken(szLine); // sequence number
				tokenizer.GetNextToken(szLine); // identifier
				if(0 == strcmp(szLine,"LINE"))
				{
					tokenizer.GetNextToken(szLine);
					ngroup = atoi(szLine);
					
					tokenizer.GetNextToken(szLine);
					line.ptStart.x = atof(szLine);
					tokenizer.GetNextToken(szLine);
					line.ptStart.y = atof(szLine);
					tokenizer.GetNextToken(szLine);
					line.ptStart.z = atof(szLine);
					tokenizer.GetNextToken(szLine);
					line.ptEnd.x = atof(szLine);
					tokenizer.GetNextToken(szLine);
					line.ptEnd.y = atof(szLine);
					tokenizer.GetNextToken(szLine);
					line.ptEnd.z = atof(szLine);
				}
				if(group < 0)
				{
					pGroup = (PSQPRIMITIVE)calloc(1,sizeof(SQPRIMITIVE));
					if(pGroup)
					{
						pGroup->hdr.id     = ENM_GROUP;
						pGroup->hdr.attr.f = 1;
						pGroup->hdr.attr.d = ENM_3D;
						
						group = ngroup;
						
						PSQPRIMITIVE ptr=NULL;
						if(ptr = (PSQPRIMITIVE)calloc(1,sizeof(SQPRIMITIVE)))
						{
							ptr->hdr.id      = ENM_LINE;
							ptr->hdr.attr.f  = 1;
							ptr->hdr.attr.d  = ENM_3D;
							ptr->body.line.ptStart.x= line.ptStart.x;
							ptr->body.line.ptStart.y= line.ptStart.y;
							ptr->body.line.ptStart.z= line.ptStart.z;
							ptr->body.line.ptEnd.x  = line.ptEnd.x;
							ptr->body.line.ptEnd.y  = line.ptEnd.y;
							ptr->body.line.ptEnd.z  = line.ptEnd.z;
							pGroup->body.pPrimitive= ListPush(pGroup->body.pPrimitive,ptr);
						}
					}
				}
				else if(group != ngroup)
				{
					if(pGroup) pRet = ListPush(pRet,pGroup);
					
					pGroup = (PSQPRIMITIVE)calloc(1,sizeof(SQPRIMITIVE));
					if(pGroup)
					{
						pGroup->hdr.id     = ENM_GROUP;
						pGroup->hdr.attr.f = 1;
						pGroup->hdr.attr.d = ENM_3D;
						
						group = ngroup;
						
						PSQPRIMITIVE ptr=NULL;
						if(ptr = (PSQPRIMITIVE)calloc(1,sizeof(SQPRIMITIVE)))
						{
							ptr->hdr.id      = ENM_LINE;
							ptr->hdr.attr.f  = 1;
							ptr->hdr.attr.d  = ENM_3D;
							ptr->body.line.ptStart.x= line.ptStart.x;
							ptr->body.line.ptStart.y= line.ptStart.y;
							ptr->body.line.ptStart.z= line.ptStart.z;
							ptr->body.line.ptEnd.x  = line.ptEnd.x;
							ptr->body.line.ptEnd.y  = line.ptEnd.y;
							ptr->body.line.ptEnd.z  = line.ptEnd.z;
							pGroup->body.pPrimitive= ListPush(pGroup->body.pPrimitive,ptr);
						}
					}
				}
				else
				{
					PSQPRIMITIVE ptr=NULL;
					if(ptr = (PSQPRIMITIVE)calloc(1,sizeof(SQPRIMITIVE)))
					{
						ptr->hdr.id      = ENM_LINE;
						ptr->hdr.attr.f  = 1;
						ptr->hdr.attr.d  = ENM_3D;
						//ptr->hdr.seq     = (unsigned short)seq++;
						ptr->body.line.ptStart.x= line.ptStart.x;
						ptr->body.line.ptStart.y= line.ptStart.y;
						ptr->body.line.ptStart.z= line.ptStart.z;
						ptr->body.line.ptEnd.x  = line.ptEnd.x;
						ptr->body.line.ptEnd.y  = line.ptEnd.y;
						ptr->body.line.ptEnd.z  = line.ptEnd.z;
						if(pGroup) pGroup->body.pPrimitive= ListPush(pGroup->body.pPrimitive,ptr);
					}
				}
			}
			int i=0;
		}
	}

	return pRet;
}