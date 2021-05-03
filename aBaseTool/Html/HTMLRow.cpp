#include <assert.h>
#include "HTMLRow.h"

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CHTMLRow::CHTMLRow(){
	m_nHeight = 2.5 + 1;
	m_nWidth  = 1;
	m_nTextHeight = 2.5;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CHTMLRow::~CHTMLRow(){}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHTMLRow::Add(CHTMLCol* pHTMLCol){
	assert(pHTMLCol && "pHTMLCol is NULL");

	if(pHTMLCol){
		m_lstHTMLCol.push_back(pHTMLCol);
		pHTMLCol->Height() = m_nHeight;
		strcpy(pHTMLCol->Color(),m_szColor);
		m_nWidth += pHTMLCol->Width();
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
int CHTMLRow::Size(){
    	return m_lstHTMLCol.size();
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CHTMLCol* CHTMLRow::Col(const int nIndex){
	int i = 0;
    	for(list<CHTMLCol*>::iterator itr=m_lstHTMLCol.begin();(i < nIndex) && (itr != m_lstHTMLCol.end());++itr,i++);

	return (itr != m_lstHTMLCol.end()) ? (*itr) : NULL;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHTMLRow::Resize(){
	double nWidth=0.;
	for(list<CHTMLCol*>::iterator itr=m_lstHTMLCol.begin();(itr != m_lstHTMLCol.end());++itr)
		nWidth += (*itr)->Width();

	int nSize=m_lstHTMLCol.size();
	double nMargin = (m_nWidth - nWidth)/(double)nSize;
	
	for(itr=m_lstHTMLCol.begin();(itr != m_lstHTMLCol.end());++itr){
		(*itr)->Height() = m_nHeight;
		(*itr)->SetMargin(nMargin);
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHTMLRow::Write(FILE* fp,double& x,double& y,bool bBorder){
	assert(fp && "fp is NULL");

	if(fp){
		double nSaveX=x;
		
		y -= m_nHeight;
		if(bBorder){
			fprintf(fp,"LINE,Style,%s,weight,%.5lf,%.5lf,%.5lf,%.5lf\r\n",
				m_szColor,x,y,x + m_nWidth,y);
		}
		for(list<CHTMLCol*>::iterator itr=m_lstHTMLCol.begin();itr != m_lstHTMLCol.end();++itr){
		    	(*itr)->Write(fp,x,y,bBorder);
		}
		x = nSaveX;
	}
}

