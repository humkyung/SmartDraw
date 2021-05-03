#include <assert.h>
#include <string.h>
#include "HTMLCol.h"

CHTMLCol::CHTMLCol(){
	strcpy(m_szTextColor,"white");
	m_nTextHeight = 1.;
	
    	m_nWidth = m_nHeight = 1.;
	m_nMargin= 0.;
	strcpy(m_szColor,"white");
	m_szContents[0] = '\0';
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
CHTMLCol::~CHTMLCol(){}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHTMLCol::Add(const char* psz){
    	assert(psz && "psz is NULL");

	if(psz){
		strcat(m_szContents,psz);
		//m_nWidth = strlen(m_szContents)*0.9*m_nTextHeight; // update width
	}
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
double& CHTMLCol::Width(){
    	return m_nWidth;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHTMLCol::SetMargin(const double nMargin){
	m_nMargin = nMargin;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
void CHTMLCol::Write(FILE* fp,double& x,double& y,bool bBorder){
    	assert(fp && "fp is NULL");

	if(fp){
		double nLen=strlen(m_szContents)*m_nTextHeight*0.9;
		fprintf(fp,"TEXT,standard,%s,%.5lf,"
			"HOR,L,%.5lf,%.5lf,%s\r\n",
			m_szTextColor,m_nTextHeight,x + m_nWidth*0.5 + m_nMargin*0.5 - nLen*0.5,y + (m_nHeight - m_nTextHeight)*0.5,m_szContents);
		
		if(bBorder){
			fprintf(fp,"LINE,Style,%s,Weight,%.5lf,%.5lf,%.5lf,%.5lf\r\n",
				m_szColor,x,y,x,y + m_nHeight);
		}
		x += m_nWidth;
	}
}
