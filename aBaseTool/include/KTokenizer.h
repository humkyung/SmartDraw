#ifndef __KTOKENIZER_H__
#define __KTOKENIZER_H__

#include "ObjInterface.h"

class DLL_EXPORT KTokenizer
{
public:
	KTokenizer();
	~KTokenizer();
public:
	void SetDelimiter(const char* pszDelimiter);
	void SetTokenString(char* pszTokenString);
	bool GetFirstToken(char* pszToken);
	bool GetNextToken(char* pszToken);
private:
	char* m_pszDelimiter;

	char* m_pTokenString;
	int   m_nSearchIndex;
};

#endif
