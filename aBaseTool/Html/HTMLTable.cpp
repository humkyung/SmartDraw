#include <assert.h>
#include "HTMLTable.h"

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CHTMLTable::CHTMLTable(){
	m_nTextHeight = 2.5;
	m_nMargin     = 0;

    	m_nRow   = 0;
	m_nCol   = 0;

	m_nX     = 0.;
	m_nY     = 0.;

	m_nWidth = m_nHeight = 0.;
	m_bBorder= true;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CHTMLTable::~CHTMLTable(){}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHTMLTable::SetX(const double x){
	m_nX = x;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHTMLTable::SetY(const double y){
	m_nY = y;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHTMLTable::Add(CHTMLRow* pHTMLRow){
    	assert(pHTMLRow && "pHTMLRow is NULL");
	
	if(pHTMLRow){
		strcpy(pHTMLRow->Color(),m_szColor);
	    	m_lstHTMLRow.push_back(pHTMLRow);
		m_nRow++;
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHTMLTable::Add(CHTMLCol* pHTMLCol){
	assert(pHTMLCol && "pHTMLCol is NULL");

	if(pHTMLCol){
		CHTMLRow* pHTMLRow=m_lstHTMLRow.back();
	    	//CHTMLRow* pHTMLRow=*(m_lstHTMLRow.end() - 1);
		if(pHTMLRow) pHTMLRow->Add(pHTMLCol);    
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHTMLTable::Resize(){
	for(list<CHTMLRow*>::iterator itr=m_lstHTMLRow.begin();itr != m_lstHTMLRow.end();++itr){
		if(m_nCol < (*itr)->Size()) m_nCol = (*itr)->Size();
	}

	double nMaxWidth[64]={1,};
	for(int i = 0;i < m_nCol;i++){
		for(list<CHTMLRow*>::iterator itr=m_lstHTMLRow.begin();itr != m_lstHTMLRow.end();++itr){
			if(m_nCol == (*itr)->Size()){
				CHTMLCol* pHTMLCol=(*itr)->Col(i);
				if(pHTMLCol){
			  		if(nMaxWidth[i] < pHTMLCol->Width()) nMaxWidth[i] = pHTMLCol->Width();
				}
			}
		}
	}

	for(i = 0;i < m_nCol;i++){
		for(list<CHTMLRow*>::iterator itr=m_lstHTMLRow.begin();itr != m_lstHTMLRow.end();++itr){
			if(m_nCol == (*itr)->Size()){
				CHTMLCol* pHTMLCol=(*itr)->Col(i);
				if(pHTMLCol) pHTMLCol->Width() = nMaxWidth[i];
			}
		}
		m_nWidth += nMaxWidth[i];
	}

	for(itr=m_lstHTMLRow.begin();itr != m_lstHTMLRow.end();++itr){
		(*itr)->Width() = m_nWidth;
		m_nHeight += (*itr)->Height();
		(*itr)->Resize();
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHTMLTable::Write(FILE* fp){
	assert(fp && "fp is NULL");

	if(fp){
		double nRowHeight=m_lstHTMLRow.front()->Height();
		if(m_bBorder){
			fprintf(fp,"LINE,Style,%s,weight,%.1lf,%.1lf,%.1lf,%.1lf\r\n",
				m_szColor,m_nX,m_nY,m_nX + m_nWidth,m_nY);
			fprintf(fp,"LINE,Style,%s,weight,%.1lf,%.1lf,%.1lf,%.1lf\r\n",
				m_szColor,m_nX + m_nWidth,m_nY,m_nX + m_nWidth,m_nY - m_nHeight);
			fprintf(fp,"LINE,Style,%s,weight,%.1lf,%.1lf,%.1lf,%.1lf\r\n",
				m_szColor,m_nX + m_nWidth,m_nY - m_nHeight,m_nX,m_nY - m_nHeight);
			fprintf(fp,"LINE,Style,%s,weight,%.1lf,%.1lf,%.1lf,%.1lf\r\n",
				m_szColor,m_nX,m_nY - m_nHeight,m_nX,m_nY);
		}
	    	for(list<CHTMLRow*>::iterator itr=m_lstHTMLRow.begin();itr != m_lstHTMLRow.end();++itr){
		    	(*itr)->Write(fp,m_nX,m_nY,m_bBorder);
		}
	}
}
