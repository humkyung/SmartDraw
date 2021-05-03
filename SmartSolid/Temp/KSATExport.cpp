#include <assert.h>
#include <SQLib.h>
#include "KTokenizer.h"
#include "SAT.h"
#include "KSATExport.h"

#include <list>
using namespace std;

KSATExport::KSATExport(){
}

KSATExport::~KSATExport(){
}

void KSATExport::OnExportFile(const char* pFilePath,list<SATSURFACE>& lstSATSurface){
	assert(pFilePath && "pFilePath is invalid");

	if(pFilePath){
		FILE* fp=NULL;

		if(fp = fopen(pFilePath,"wb")){
			list<SATSURFACE>::iterator itr;
			for(itr = lstSATSurface.begin();itr != lstSATSurface.end();++itr){
				PSATLINE pSurface = itr->pSurface;
				if(0 == strncmp(pSurface->pLine,"cone-surface",12)){
				}else if(0 == strncmp(pSurface->pLine,"sphere-surface",14)){
				}else if(0 == strncmp(pSurface->pLine,"torus-surface",13)){
				}else if(0 == strncmp(pSurface->pLine,"plane-surface",13)){
					list<PSATLINE>* pSATLine = itr->plstSAT;
					ExportPlaneSurface(fp,pSATLine);
				}
			}
			fclose(fp);
		}
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void KSATExport::ExportPlaneSurface(FILE* fp,list<PSATLINE>* pSATLine){
	assert(fp       && "fp is invalid");
	assert(pSATLine && "pSATLine is invalid");

	if(fp && pSATLine){
		KTokenizer tokenizer;
		char szToken[80]={0};

		fprintf(fp,"[PLANE]\r\n");
		list<PSATLINE>::iterator itr;
		for(itr = pSATLine->begin();itr != pSATLine->end();++itr){
			tokenizer.SetTokenString((*itr)->pLine);
 			tokenizer.SetDelimiter(' ');
		
			tokenizer.GetNextToken(szToken);
			if(0 == strncmp("point",szToken,5)){
				SQPOINT pt={0};

				tokenizer.GetNextToken(szToken);
				tokenizer.GetNextToken(szToken);
				pt.x = atof(szToken);

				tokenizer.GetNextToken(szToken);
				pt.y = atof(szToken);

				tokenizer.GetNextToken(szToken);
				pt.z = atof(szToken);

				fprintf(fp,"%lf %lf %lf ",pt.x,pt.y,pt.z);
			}else if(0 == strncmp("ellipse-curve",szToken,13)){
				SQPOINT pt={0};

				tokenizer.GetNextToken(szToken);
				tokenizer.GetNextToken(szToken);
				pt.x = atof(szToken);

				tokenizer.GetNextToken(szToken);
				pt.y = atof(szToken);

				tokenizer.GetNextToken(szToken);
				pt.z = atof(szToken);

				fprintf(fp," %lf %lf %lf\r\n",pt.x,pt.y,pt.z);
			}else if(0 == strncmp("straight-curve",szToken,14)){
				SQPOINT pt={0};

				tokenizer.GetNextToken(szToken);
				tokenizer.GetNextToken(szToken);
				pt.x = atof(szToken);

				tokenizer.GetNextToken(szToken);
				pt.y = atof(szToken);

				tokenizer.GetNextToken(szToken);
				pt.z = atof(szToken);
				fprintf(fp,"\r\n");
			}
		}
	}
}
