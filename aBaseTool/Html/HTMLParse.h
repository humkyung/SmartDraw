#ifndef __HTMLPARSER_H__
#define __HTMLPARSER_H__

#include "HTMLTable.h"
#include <list>
using namespace std;

class CHTMLParser{
public:
	CHTMLParser();
	virtual ~CHTMLParser();
public:
	void Parse(const char* pszBuffer);
	void Write(const char* pszFilePath,const char* ptrMode="wb");
protected:
	void  Parse();
	void  ParseKeyword();
	void  ParseTable();
	void  ParseTR();
	void  ParseTD();
	char* GetNextToken();
	char* GetContentToken();
	void  PushBackToken();
private:
	list<CHTMLTable*> m_lstHTMLTable;

	char *m_ptrPrev,*m_ptr;
	char *m_pszBuffer;
	//char  m_szBuffer[102400];
};

#endif
