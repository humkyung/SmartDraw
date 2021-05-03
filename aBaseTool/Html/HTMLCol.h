#ifndef __HTMLCOL_H__
#define __HTMLCOL_H__

#include <stdio.h>

class CHTMLCol{
public:
	CHTMLCol();
	virtual ~CHTMLCol();
public:
	void    Add(const char* psz);
	double& Width();
	double& Height(){return m_nHeight;}
	
	char*   TextColor(){return m_szTextColor;}
	double& TextHeight(){return m_nTextHeight;}
	char*   Color(){return m_szColor;}
	void    SetMargin(const double nMargin);
	void    Write(FILE* fp,double& x,double& y,bool bBorder);
private:
	char   m_szTextColor[8];
	double m_nTextHeight;

	double m_nHeight;
	double m_nWidth;
	double m_nMargin;
	char   m_szColor[8];

	char   m_szContents[256];
};

#endif
