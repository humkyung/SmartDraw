#ifndef __HTMLDOC_H__
#define __HTMLDOC_H__

#include <string>
using namespace std;

class CHTMLDoc{
public:
	CHTMLDoc(const char* pszPath="");
	virtual ~CHTMLDoc();
public:
	const char* Name();
private:
	string* m_pstrPath;
};

#endif
