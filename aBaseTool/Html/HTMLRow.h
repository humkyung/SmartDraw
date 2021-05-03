#ifndef __HTMLROW_H__
#define __HTMLROW_H__

#include "HTMLCol.h"

#include <list>
using namespace std;

class CHTMLRow{
public:
	CHTMLRow();
	virtual ~CHTMLRow();
public:
	void Add(CHTMLCol* pHTMLCol);
	int  Size();
	CHTMLCol* Col(const int nIndex);
	double& Width(){return m_nWidth;}
	double& Height(){return m_nHeight;}
	char*   Color(){return m_szColor;}
	void Resize();
	
	void Write(FILE* fp,double& x,double& y,bool bBorder);
private:
	double m_nWidth;
	double m_nHeight;
	double m_nTextHeight;
	char   m_szColor[8];

	list<CHTMLCol*> m_lstHTMLCol;
};

#endif
