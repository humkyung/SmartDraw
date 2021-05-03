#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#ifndef	DLL_EXPORT 
	#define DLL_EXPORT __declspec(dllexport) 
#endif

#include <SQLib.h>

class DLL_EXPORT CPlugin{
public:
	CPlugin(){}
	virtual ~CPlugin(){}
public:
	virtual const char* Query(int nQuery)=0;
	virtual PSQPRIMITIVE Load(const char* pFilePath)=0;
	virtual void SetAllocMemoryHandler(void* (*)(size_t,size_t))=0;
};

#endif
