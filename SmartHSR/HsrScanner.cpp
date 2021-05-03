// PreFormatScanner.cpp: implementation of the CHSRScanner class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <IsTools.h>
#include <Tokenizer.h>
#include "HsrScanner.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CHSRScanner::CHSRScanner(IFSTREAM_T* file) : m_pfile(file) , m_nLineNo(1)
{
	m_pstrLine = new STRING_T;
	m_aryFactor= new vector<STRING_T>;
}

CHSRScanner::~CHSRScanner()
{
	try
	{
		if(NULL != m_pstrLine) delete m_pstrLine;
		m_pstrLine = NULL;
		
		if(NULL != m_aryFactor) delete m_aryFactor;
		m_aryFactor = NULL;
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-06
    @class      CHSRScanner
    @function   Reset
    @return     int
    @param      IFSTREAM_T* ifile
    @brief
******************************************************************************/
int CHSRScanner::Reset(IFSTREAM_T* ifile)
{
	assert(ifile && "ifile is NULL");

	if(ifile)
	{
		m_nLineNo = 1;
		(*m_pstrLine) = _T("");
		m_nFactor = 0;
		m_aryFactor->clear();
		m_pfile = ifile;

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**	
	@brief	The CHSRScanner::Read function

	@param	file	a parameter of type std::ifstream&
  
	@return	bool	
*/
#ifdef VER_03
bool CHSRScanner::ReadLine()
{
	bool bRet = true;

	m_nFactor = 0;
	while(m_pfile && m_pfile->good())
	{
		memset(m_aryFactor[m_nFactor],0,sizeof(char)*FACTOR_BUF_SIZ);
		m_pfile->getline(m_aryFactor[m_nFactor],FACTOR_BUF_SIZ);
		//getline(file,CHSRScanner::m_str);
		if((0 == strcmp(m_aryFactor[m_nFactor],"<END>")) || (0 == STRNCMP_T(m_aryFactor[m_nFactor],"FORMAT",6))) break;
		m_nFactor++;
	}
	return bRet;
}
#else
bool CHSRScanner::ReadLine()
{
	bool bRet = false;

	if(m_pfile && m_pfile->good())
	{
		///memset(m_szLine,'\0',SCANNER_BUF_SIZ);
		//! read a line
		(*m_pstrLine) = _T("");
		getline(*m_pfile , *m_pstrLine);
		///m_pfile->getline(m_szLine,SCANNER_BUF_SIZ);
		
		//! initialize variables.
		m_nFactor         = 0;
		m_aryFactor->clear();
		///for(int i = 0;i< MAX_FACTOR_COUNT;++i) memset(m_aryFactor[i] , '\0' , sizeof(char) * FACTOR_BUF_SIZ);

		///m_aryFactor[0][0] = '\0';
		Scan(*m_pstrLine);
		++m_nLineNo;
		
		bRet = true;
	}

	return bRet;
}
#endif

/**	
	@brief	The CHSRScanner::Val function

	@author	humkyung

	@param	pVar	a parameter of type const char*

	@return	char*	
*/
TCHAR* CHSRScanner::Val(const TCHAR* pVar)
{
	assert(pVar && "pVar is NULL");
	TCHAR* pRet=NULL;

	if(pVar)
	{
		for(int i=0;i < m_nFactor;i++)
		{
			if(0 == STRNCMP_T((*m_aryFactor)[i].c_str(),pVar,STRLEN_T(pVar)))
			{
				if(pRet = (TCHAR*)STRCHR_T((*m_aryFactor)[i].c_str(),'='))
				{
					pRet++;
					break;
				}else	pRet = NULL;
			}
		}
	}

	return pRet;
}

static bool IsEmpty(STRING_T sString)
{
	return sString.empty();
}

/**	
	@brief	파일에서 읽은 한 줄을 스캔한다.
	
	@author	HumKyung.BAEK

	@param	line	a parameter of type std::string

	@return	bool	true = 성공적으로 Scan , false = Scan 실패	
*/
bool CHSRScanner::Scan(const STRING_T& line)
{
	if(!line.empty())
	{
		/*
		unsigned int i=1, si=0, ei=0;
		
		ei = line.find(',');
		while ((ei != std::string::npos) && (m_nFactor < 64))
		{
			memset(m_aryFactor[m_nFactor],'\0',64);
			strncpy(m_aryFactor[m_nFactor++],line.substr(si,ei - si).data(),64);
			si = ei + 1;
			ei = line.find(',', si);
		}
		ei = line.size(); 
		*/
		///vector<string> oResult;
		CTokenizer<CIsComma>::Tokenize(*m_aryFactor , line.c_str() , CIsComma());
		m_aryFactor->erase(remove_if(m_aryFactor->begin() , m_aryFactor->end() , IsEmpty) , m_aryFactor->end());
		m_nFactor = m_aryFactor->size();
		///for(int i = 0;(i < MAX_FACTOR_COUNT) && (i < m_nFactor);++i)
		///{
		///	strcpy((*m_aryFactor)[i] , oResult[i].c_str());
		///}
		///if( m_nFactor != i) m_nFactor = 0;

		return ( m_nFactor == m_aryFactor->size() );
	}

	return false;
}

/**
	@brief	파일에서 한 라인을 읽는다.

	@author	BHK
*/
bool CHSRScanner::GetLine(TCHAR *pszBuf, const size_t &count)
{
	while(m_pfile && m_pfile->is_open() && !m_pfile->eof())
	{
		m_pfile->getline(pszBuf , count);
		if('\0' == pszBuf[0]) continue;
		
		return true;
	}

	return false;
}
