#include <assert.h>
#include "KSATImport.h"

KImport::KImport(){
}

KImport::~KImport(){
}

void KImport::OnImportFile(list<SATSURFACE>& lstSATSurface,const char* pFilePath){
	assert(pFilePath && "pFilePath is invalid");

	if(pFilePath){
		KTextFile file;
		if(file.Open(pFilePath,READ)){
			static char szLine[1024]={0};

			SATLINE lnSAT;
			long nIndex=0;
			for(;file.ReadLine(szLine);){
				if(0 == strncmp(szLine,"body",4)){
					lnSAT.nIndex = nIndex++;
					if(lnSAT.pLine = (char*)malloc(sizeof(szLine) + 1)){
						strcpy(lnSAT.pLine,szLine);
						m_vtBody.push_back(lnSAT);
					}

					break;
				}
			}

			for(;file.ReadLine(szLine);){
				if(0 == strncmp(szLine,"body",4)){
					lnSAT.nIndex = nIndex++;
					if(lnSAT.pLine = (char*)malloc(sizeof(szLine) + 1)){
						strcpy(lnSAT.pLine,szLine);
						m_vtBody.push_back(lnSAT);
					}
				}else if(0 == strncmp(szLine,"lump",4)){
					lnSAT.nIndex = nIndex++;
					if(lnSAT.pLine = (char*)malloc(sizeof(szLine) + 1)){
						strcpy(lnSAT.pLine,szLine);
						m_vtLump.push_back(lnSAT);
					}
				}else if(0 == strncmp(szLine,"shell",5)){
					lnSAT.nIndex = nIndex++;
					if(lnSAT.pLine = (char*)malloc(sizeof(szLine) + 1)){
						strcpy(lnSAT.pLine,szLine);
						m_vtShell.push_back(lnSAT);
					}
				}else if(0 == strncmp(szLine,"face",4)){
					lnSAT.nIndex = nIndex++;
					if(lnSAT.pLine = (char*)malloc(sizeof(szLine) + 1)){
						strcpy(lnSAT.pLine,szLine);
						m_vtFace.push_back(lnSAT);
					}
				}else if(0 == strncmp(szLine,"loop",4)){
					lnSAT.nIndex = nIndex++;
					if(lnSAT.pLine = (char*)malloc(sizeof(szLine) + 1)){
						strcpy(lnSAT.pLine,szLine);
						m_vtLoop.push_back(lnSAT);
					}
				}else if(0 == strncmp(szLine,"coedge",6)){
					lnSAT.nIndex = nIndex++;
					if(lnSAT.pLine = (char*)malloc(sizeof(szLine) + 1)){
						strcpy(lnSAT.pLine,szLine);
						m_vtCoEdge.push_back(lnSAT);
					}
				}else if(0 == strncmp(szLine,"edge",4)){
					lnSAT.nIndex = nIndex++;
					if(lnSAT.pLine = (char*)malloc(sizeof(szLine) + 1)){
						strcpy(lnSAT.pLine,szLine);
						m_vtEdge.push_back(lnSAT);
					}
				}else if((0 == strncmp(szLine,"ellipse-curve",13)) || (0 == strncmp(szLine,"straight-curve",14))){
					lnSAT.nIndex = nIndex++;
					if(lnSAT.pLine = (char*)malloc(sizeof(szLine) + 1)){
						strcpy(lnSAT.pLine,szLine);
						m_vtCurve.push_back(lnSAT);
					}
				}else if((0 == strncmp(szLine,"cone-surface",12)) || (0 == strncmp(szLine,"plane-surface",13)) || 
					(0 == strncmp(szLine,"sphere-surface",14))|| (0 == strncmp(szLine,"torus-surface",13))){
					lnSAT.nIndex = nIndex++;
					if(lnSAT.pLine = (char*)malloc(sizeof(szLine) + 1)){
						strcpy(lnSAT.pLine,szLine);
						m_vtSurface.push_back(lnSAT);
					}
				}else if(0 == strncmp(szLine,"vertex",6)){
					lnSAT.nIndex = nIndex++;
					if(lnSAT.pLine = (char*)malloc(sizeof(szLine) + 1)){
						strcpy(lnSAT.pLine,szLine);
						m_vtVertex.push_back(lnSAT);
					}
				}else if(0 == strncmp(szLine,"point",5)){
					lnSAT.nIndex = nIndex++;
					if(lnSAT.pLine = (char*)malloc(sizeof(szLine) + 1)){
						strcpy(lnSAT.pLine,szLine);
						m_vtPoint.push_back(lnSAT);
					}
				}else	nIndex++;
			}
		}
		
		ReadBody(lstSATSurface);
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KImport::ReadBody(list<SATSURFACE>& lstSATSurface)
{
	KTokenizer tokenizer;
	char szToken[80]={0};

	vector<SATLINE>::iterator itr;
	for(itr = m_vtBody.begin();itr != m_vtBody.end();++itr){		
 		tokenizer.SetTokenString(itr->pLine);
 		tokenizer.SetDelimiter(' ');
		for(int i = 0;(i < 3) && tokenizer.GetNextToken(szToken);i++);
		long nLumpIndex = atoi(szToken + 1);
		ReadLump(lstSATSurface,nLumpIndex);
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KImport::ReadLump(list<SATSURFACE>& lstSATSurface,long nIndex)
{
	vector<SATLINE>::iterator itr;
	for(itr = m_vtLump.begin();(nIndex != itr->nIndex) && (itr != m_vtLump.end());++itr);
	if(nIndex == itr->nIndex){
		KTokenizer tokenizer;
		char szToken[80]={0};

 		tokenizer.SetTokenString(itr->pLine);
 		tokenizer.SetDelimiter(' ');
		for(int i = 0;(i < 4) && tokenizer.GetNextToken(szToken);i++);
		long nShellIndex = atoi(szToken + 1);
		long nNextShellIndex=nShellIndex;
		do{
			nNextShellIndex = ReadShell(lstSATSurface,nNextShellIndex);
		}while((nNextShellIndex != nShellIndex) && (nNextShellIndex != -1));
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		: index of next shell
long KImport::ReadShell(list<SATSURFACE>& lstSATSurface,long nIndex)
{
	long ret=-1;

	vector<SATLINE>::iterator itr;
	for(itr = m_vtShell.begin();(nIndex != itr->nIndex) && (itr != m_vtLump.end());++itr);
	if(nIndex == itr->nIndex){
		KTokenizer tokenizer;
		char szToken[80]={0};

 		tokenizer.SetTokenString(itr->pLine);
 		tokenizer.SetDelimiter(' ');
		for(int i = 0;(i < 5) && tokenizer.GetNextToken(szToken);i++);
		long nFaceIndex = atoi(szToken + 1);
		long nNextFaceIndex = nFaceIndex;
		do{
			nNextFaceIndex = ReadFace(lstSATSurface,nNextFaceIndex);
		}while((nNextFaceIndex != nFaceIndex) && (nNextFaceIndex != -1));
	}

	return ret;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
long KImport::ReadFace(list<SATSURFACE>& lstSATSurface,long nIndex)
{
	long ret=-1;

	vector<SATLINE>::iterator itr;
	for(itr = m_vtFace.begin();(nIndex != itr->nIndex) && (itr != m_vtLump.end());++itr);
	if(nIndex == itr->nIndex){
		KTokenizer tokenizer;
		char szToken[80]={0};
		long nLoopIndex = -1,nSurfaceIndex=-1;

 		tokenizer.SetTokenString(itr->pLine);
 		tokenizer.SetDelimiter(' ');
		for(int i = 0;tokenizer.GetNextToken(szToken);i++){
			if(2 == i){
				ret = atoi(szToken + 1);
			}else if(3 == i){
				nLoopIndex = atoi(szToken + 1);
			}else if(6 == i){
				nSurfaceIndex = atoi(szToken + 1);
			}

		}

		SATSURFACE SATSurface={0};
		if(SATSurface.plstSAT = new list<PSATLINE>){
			for(itr = m_vtSurface.begin();(nSurfaceIndex != itr->nIndex) && (itr != m_vtSurface.end());++itr);
			SATSurface.pSurface = itr;
			lstSATSurface.push_back(SATSurface);

			long nNextLoopIndex=nLoopIndex;
			do{
				nNextLoopIndex=ReadLoop(*(SATSurface.plstSAT),nNextLoopIndex);
			}while((nNextLoopIndex != nLoopIndex) && (nNextLoopIndex != -1));
		}
	}

	return ret;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
long KImport::ReadLoop(list<PSATLINE>& lstPSAT,long nIndex)
{
	long ret=-1;

	vector<SATLINE>::iterator itr;
	for(itr = m_vtLoop.begin();(nIndex != itr->nIndex) && (itr != m_vtLump.end());++itr);
	if(nIndex == itr->nIndex){
		KTokenizer tokenizer;
		char szToken[80]={0};

 		tokenizer.SetTokenString(itr->pLine);
 		tokenizer.SetDelimiter(' ');
		for(int i = 0;(i < 3) && tokenizer.GetNextToken(szToken);i++);
		ret = atoi(szToken + 1);
		
		tokenizer.GetNextToken(szToken);
		long nCoEdgeIndex = atoi(szToken + 1);
		long nNextCoEdgeIndex=nCoEdgeIndex;
		do{
			nNextCoEdgeIndex = ReadCoEdge(lstPSAT,nNextCoEdgeIndex);
		}while((nNextCoEdgeIndex != nCoEdgeIndex) && (nNextCoEdgeIndex != -1));
	}

	return ret;
}

long KImport::ReadCoEdge(list<PSATLINE>& lstPSAT,long nIndex)
{
	long ret=-1;

	vector<SATLINE>::iterator itr;
	for(itr = m_vtCoEdge.begin();(nIndex != itr->nIndex) && (itr != m_vtLump.end());++itr);
	if(nIndex == itr->nIndex){
		KTokenizer tokenizer;
		char szToken[80]={0};

 		tokenizer.SetTokenString(itr->pLine);
 		tokenizer.SetDelimiter(' ');
		for(int i = 0;(i < 4) && tokenizer.GetNextToken(szToken);i++);
		ret = atoi(szToken + 1);
		
		tokenizer.GetNextToken(szToken);
		tokenizer.GetNextToken(szToken);
		long nEdgeIndex = atoi(szToken + 1);
		ReadEdge(lstPSAT,nEdgeIndex);
	}

	return ret;
}


void KImport::ReadEdge(list<PSATLINE>& lstPSAT,long nIndex)
{
	vector<SATLINE>::iterator itr;
	for(itr = m_vtEdge.begin();(nIndex != itr->nIndex) && (itr != m_vtLump.end());++itr);
	if(nIndex == itr->nIndex){
		KTokenizer tokenizer;
		char szToken[80]={0};

 		tokenizer.SetTokenString(itr->pLine);
 		tokenizer.SetDelimiter(' ');
		for(int i = 0;(i < 3) && tokenizer.GetNextToken(szToken);i++);
		long nStart = atoi(szToken + 1);
		
		tokenizer.GetNextToken(szToken);
		long nEnd   = atoi(szToken + 1);

		ReadVertex(lstPSAT,nStart);
		ReadVertex(lstPSAT,nEnd);

		// extract curve information
		tokenizer.GetNextToken(szToken);
		tokenizer.GetNextToken(szToken);
		long nCurveIndex = atoi(szToken + 1);
		for(itr = m_vtCurve.begin();(itr->nIndex != nCurveIndex) && (itr != m_vtCurve.end());++itr);
		if(itr->nIndex == nCurveIndex) lstPSAT.push_back(itr);
		//
	}
}

void KImport::ReadVertex(list<PSATLINE>& lstPSAT,long nIndex)
{
	vector<SATLINE>::iterator itr;
	for(itr = m_vtVertex.begin();(nIndex != itr->nIndex) && (itr != m_vtLump.end());++itr);
	if(nIndex == itr->nIndex){
		KTokenizer tokenizer;
		char szToken[80]={0};

 		tokenizer.SetTokenString(itr->pLine);
 		tokenizer.SetDelimiter(' ');
		for(int i = 0;(i < 4) && tokenizer.GetNextToken(szToken);i++);
		long nPoint = atoi(szToken + 1);
		ReadPoint(lstPSAT,nPoint);
	}
}

void KImport::ReadPoint(list<PSATLINE>& lstPSAT,long nIndex)
{
	vector<SATLINE>::iterator itr;
	for(itr = m_vtPoint.begin();(nIndex != itr->nIndex) && (itr != m_vtLump.end());++itr);
	if(nIndex == itr->nIndex){
		KTokenizer tokenizer;
		char szToken[80]={0};
		SQPOINT pt={0};

		lstPSAT.push_back(itr);

 		tokenizer.SetTokenString(itr->pLine);
 		tokenizer.SetDelimiter(' ');
		for(int i = 0;(i < 3) && tokenizer.GetNextToken(szToken);i++);
		pt.x = atof(szToken);

		tokenizer.GetNextToken(szToken);
		pt.y = atof(szToken);

		tokenizer.GetNextToken(szToken);
		pt.z = atof(szToken);
	}
}
