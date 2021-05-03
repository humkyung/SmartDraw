/********************************************************************
	created:	2002/08/06
	created:	6:8:2002   0:01
	filename: 	D:\Project\DGNLib\include\DGNLib.h
	file path:	D:\Project\DGNLib\include
	file base:	DGNLib
	file ext:	h
	author:		hum kyung baek
	
	purpose:	[2003.08.05] - add a function which convert point to int
*********************************************************************/

#ifndef __DGNLIB_H__
#define __DGNLIB_H__

#include <SQLib.h>
#include "DGNEnum.h"
#include <DGNRefFile.h>
#include "DGNAttrData.h"
#include "DGNFile.h"
#include "DGNDoc.h"
#include <DGNElement.h>
#include "..\DGNImport.h"
#include "..\DGNExport.h"

#include <queue>
#include <list>
using namespace std;

#ifndef	__DLL_EXPORT__
#define	__DLL_EXPORT__ __declspec(dllexport)
#endif

#ifndef DGNLIB_EXPORTS
#ifdef _UNICODE
	#ifdef _DEBUG
		#if _MSC_VER == 1600
			#pragma comment(lib, "DgnLib_vc100_unicode_debug")
			#pragma message("Automatically linking with DgnLib_vc100_unicode_debug.dll (Debug Unicode)")
		#else
			#pragma comment(lib, "DgnLib_vc90_unicode_debug")
			#pragma message("Automatically linking with DgnLib_vc90_unicode_debug.dll (Debug Unicode)")
		#endif
	#else
		#if _MSC_VER == 1600
			#pragma comment(lib, "DgnLib_vc100_unicode")
			#pragma message("Automatically linking with DgnLib_vc100_unicode.dll (Release Unicode)")
		#else
			#pragma comment(lib, "DgnLib_vc90_unicode")
			#pragma message("Automatically linking with DgnLib_vc90_unicode.dll (Release Unicode)")
		#endif
	#endif
#else
	#ifdef _DEBUG
		#if _MSC_VER == 1600
			#pragma comment(lib, "DgnLib_vc100_debug")
			#pragma message("Automatically linking with DgnLib_vc100_debug.dll (Debug)")
		#else
			#pragma comment(lib, "DgnLib_vc90_debug")
			#pragma message("Automatically linking with DgnLib_vc90_debug.dll (Debug)")
		#endif
	#else
		#if _MSC_VER == 1600
			#pragma comment(lib, "DgnLib_vc100")
			#pragma message("Automatically linking with DgnLib_vc100.dll (Release)")
		#else
			#pragma comment(lib, "DgnLib_vc90")
			#pragma message("Automatically linking with DgnLib_vc90.dll (Release)")
		#endif
	#endif
#endif // _UNICODE

#endif // USER_EXPORTS

class DLL_EXPORT CDGNLib
{
protected:
	CDGNLib();
public:
	virtual ~CDGNLib();
public:

	void GetColorValue(unsigned char& r,unsigned char& g,unsigned char&b,const int nDGNColor);
	const int GetDGNColorNumber(CDGNFile* hDGN,unsigned char r,unsigned char g,unsigned char b);
	const int GetSurfaceType(const int nDGNSurface);
	const int GetDGNSurfaceType(const int nDGNSurface);

	CDGNElement* GetElementPrototype(const int nType);

	void OnFreePrimitive();
	PSQPRIMITIVE NewPrimitive();

	static CDGNLib* GetInstance();
	CDGNDoc* GetDoc();
	bool Load(const char* pFilePath);

	static PSQPRIMITIVE CopyPrimitive(PSQPRIMITIVE rhs);
	static void* memory(size_t nSize);
	static void* (*AllocMemory)(size_t nSize);
	static void  (*FreeMemory)(void *memblock);
	static void  POINT2INT(unsigned char *paryTarget,POINT_T *ppt,CDGNFile* hDGN);
	static bool  IsPartOfComplexElement(CDGNFile* hDGN);
	static void  DeleteElement(PSQPRIMITIVE pPrimitive);
	static void  DeleteLinkageData(PSQPRIMITIVE ptrObj);
public:
	CDGNImporter m_clsImporter;
	CDGNExport m_DGNExport;
private:
	CDGNLib* _instance;

	CDGNDoc* m_pDoc;

	list<PSQPRIMITIVE>* m_plstPrimitive;
	list<CDGNElement*>* m_plstElement;
};

extern "C" __declspec(dllexport) double GetDGNTextLength(const char* text,double nHeight);

#endif
