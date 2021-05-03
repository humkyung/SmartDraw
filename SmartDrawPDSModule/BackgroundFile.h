#pragma once

#ifdef ADRAW_PDSMODULE_EXPORTS
	#define ON_PDSMODULE_EXT_DLL		__declspec(dllexport)
	#define	ON_PDSMODULE_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_PDSMODULE_EXTERN_DECL	extern
#else
	#define ON_PDSMODULE_EXT_DLL		__declspec(dllimport)
	#define	ON_PDSMODULE_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_PDSMODULE_EXTERN_DECL	extern __declspec(dllimport)
#endif

#include <aDraw_DraFile.h>
#include <DgnElement.h>

class ON_PDSMODULE_EXT_DLL CBackgroundFile
{
public:
	CBackgroundFile(void);
	~CBackgroundFile(void);

	int Generate(LPCTSTR pOutputFilePath , LPCTSTR pDrawFilePath , const long& lHiddenLevel=62);
	int Generate(LPCTSTR pOutputFilePath , CaDraw_DraFile* pDraFile);
private:
	void WriteHsrFile(LPCTSTR pExportFilePath , list<CDgnElement*>& dgnElementList , const double& uor , const unsigned int& nHiddenLevel = 62);
};
