#ifndef __SAT_H__
#define __SAT_H__

#include <list>
using namespace std;

typedef struct tagSATLine{
	long  nIndex;
	char* pLine;
}SATLINE,* PSATLINE;

typedef struct tagSATSurface{
	PSATLINE pSurface;
	list<PSATLINE>* plstSAT;
}SATSURFACE,* PSATSURFACE;

#endif