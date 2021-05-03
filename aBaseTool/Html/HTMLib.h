#ifndef __HTMLLIB_H__
#define __HTMLLIB_H__

#include "HTMLDoc.h"

#include <vector>
using namespace std;

class CHTMLLib{
public:
	CHTMLLib();
	virtual ~CHTMLLib();
public:
	CHTMLDoc* GetDoc(const unsigned int nIndex);
private:
	vector<CHTMLDoc*>* m_pvecDoc;
};

#endif
