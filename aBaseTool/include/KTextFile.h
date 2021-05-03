#ifndef __KTEXTFILE_H__
#define __KTEXTFILE_H__

#include <File.h>
#include "ObjInterface.h"

class DLL_EXPORT KTextFile : public KFile
{
public:
	KTextFile();
	~KTextFile();
public:
	bool Open(const char* pszFilePath,OPENMODE_T mode);
	int  ReadLine(char* pLine,int size);

	bool IsEOF();
private:
	bool  IsWhiteChar(wint_t& ch);
private:
	int m_nLength;
};

#endif
