#ifndef __KFILE_H__
#define __KFILE_H__

#include <stdio.h>
#include "ObjInterface.h"

typedef enum
{
	READ,
	WRITE
}OPENMODE_T;

class DLL_EXPORT KFile
{
public:
	KFile();
	virtual ~KFile();
public:
	bool  Read(char* pText,size_t size);
	const FILE* GetFileHandle();
	void  Close();
protected:
	FILE* m_fp;

	static char buf[1024];
};

#endif
