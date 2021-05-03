/********************************************************************
	created:	2002/08/12
	created:	1:1:2002   10:06
	filename: 	D:\Project\OpenSolid\DGNLib\KDGNImport.h
	file path:	D:\Project\OpenSolid\DGNLib
	file base:	KDGNImport
	file ext:	h
	author:		
	
	purpose:	import dgn file	
*********************************************************************/
#ifndef __DGN_IMPORT_H__
#define __DGN_IMPORT_H__

#include "DGNParser.h"

#include <list>
using namespace std;

#ifndef	DLL_EXPORT
#define	DLL_EXPORT __declspec(dllexport)
#endif

class DLL_EXPORT CDGNImporter
{
public:
	CDGNImporter();
	~CDGNImporter();
public:
	PSQPRIMITIVE OnImportFile(const char* pImportFile);
	PSQPRIMITIVE OnImportFile(const char* pImportFilePath,PSQVOLUME pVolume);
	void FreeImportedPrimitive();
private:
	CDGNFile* OpenFile(const char * pszFilePath);
	void CloseFile(CDGNFile* hDGN);
private:
	CDGNParser*  m_pDGNParser;

	list<PSQPRIMITIVE>* m_plstImportedPrimitive;
};

#endif