#ifndef __HTMLTABLE_H__
#define __HTMLTABLE_H__

#include "HTMLRow.h"

class CHTMLTable{
public:
	CHTMLTable();
	virtual ~CHTMLTable();
public:
	void SetX(const double x);
	void SetY(const double y);
	void SetWidth(const double nWidth)  {m_nWidth = nWidth;}
	void SetHeight(const double nHeight){m_nHeight= nHeight;}
	char* Color(){return m_szColor;}
	bool& Border(){return m_bBorder;}
	void Add(CHTMLRow* pHTMLRow);
	void Add(CHTMLCol* pHTMLCol);	

	void Resize();
	void Write(FILE* fp);
private:
	list<CHTMLRow*> m_lstHTMLRow;

	double m_nTextHeight;
	double m_nMargin;

	long m_nRow,m_nCol;
	double m_nX,m_nY;
	double m_nWidth,m_nHeight;
	char   m_szColor[8];
	bool   m_bBorder;
};

#endif
