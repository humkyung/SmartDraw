#pragma once

#include <aDraw_DraFile.h>
#include "SmartDrawAnnoItem.h"

#include <list>
using namespace std;

//#ifdef ADRAW_TAG_EXPORTS
//	#define ON_TAG_EXT_DLL		__declspec(dllexport)
//	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
//	#define ON_TAG_EXTERN_DECL	extern
//#else
//	#define ON_TAG_EXT_DLL		__declspec(dllimport)
//	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
//	#define ON_TAG_EXTERN_DECL	extern __declspec(dllimport)
//#endif

class ON_TAG_EXT_DLL CSmartDrawAnno
{
public:
	CSmartDrawAnno(void);
	~CSmartDrawAnno(void);
public:
	int Annotate(const TCHAR* pOutputFilePath , const TCHAR* pHsrTextFilePath , const TCHAR* pIniFilePath , CaDraw_DraFile* pDraFile,const double& dDrawingScale);
private:
	int ParseHiddenTextFile(const STRING_T& rHiddenFilePath);
};
