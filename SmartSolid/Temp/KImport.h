#ifndef __KIMPORT_H__
#define __KIMPORT_H__

#include <SQLib.h>
#include "KTextFile.h"
#include "KTokenizer.h"
#include "SAT.h"

#include <vector>
using namespace std;

class KImport{
public:
	KImport();
	~KImport();
public:
	void OnImportFile(list<SATSURFACE>& lstSATSurface,const char* pFilePath);
private:
	void ReadPoint(list<PSATLINE>& lstPSAT,long nIndex);
	void ReadVertex(list<PSATLINE>& lstPSAT,long nIndex);
	void ReadEdge(list<PSATLINE>& lstPSAT,long nIndex);
	long ReadCoEdge(list<PSATLINE>& lstPSAT,long nIndex);
	long ReadFace(list<SATSURFACE>& lstPSAT,long nIndex);
	long ReadLoop(list<PSATLINE>& lstPSAT,long nIndex);
	long ReadShell(list<SATSURFACE>& lstPSAT,long nIndex);
	void ReadLump(list<SATSURFACE>& lstPSAT,long nIndex);
	void ReadBody(list<SATSURFACE>& lstPSAT);
private:
	vector<SATLINE> m_vtBody;
	vector<SATLINE> m_vtLump;
	vector<SATLINE> m_vtShell;
	vector<SATLINE> m_vtFace;
	vector<SATLINE> m_vtSurface;
	vector<SATLINE> m_vtLoop;
	vector<SATLINE> m_vtCoEdge;
	vector<SATLINE> m_vtEdge;
	vector<SATLINE> m_vtCurve;
	vector<SATLINE> m_vtVertex;
	vector<SATLINE> m_vtPoint;
};

#endif