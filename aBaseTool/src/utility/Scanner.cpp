// SITEScanner.cpp: implementation of the CSITEScanner class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <utility\Scanner.h>

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

char CScanner::m_szLine[1024]={0,};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	\brief
*/
CScanner::CScanner()
{
	m_file = new std::ifstream;

	m_szDelimiter[0] = '\0';
	m_nDelimiterLen  = 0;
}

/**	\brief
*/
CScanner::CScanner(const char* pFilePath)
{
	m_szDelimiter[0] = '\0';
	m_nDelimiterLen  = 0;

	m_file = new std::ifstream;
	m_file->open(pFilePath);
}

/**	\brief
*/
CScanner::~CScanner()
{
	try
	{
		if(m_file && m_file->is_open()) m_file->close();
		if(m_file) delete m_file;
	}
	catch(...)
	{
	}
}

/**	\brief	The CScanner::ReadLine function


	\return	bool	
*/
bool CScanner::ReadLine(){
	bool bRet=false;
	if(m_file->good() && !m_file->eof()){
		memset(m_szLine,0,1024);
		m_file->getline(m_szLine,1024);
		
		bRet = true;
	}

	return bRet;
}

/**	\brief	The CScanner::Read function


	\return	bool	
*/
bool CScanner::Read(){
	bool bRet=false;
	if(m_file->good() && !m_file->eof()){
		memset(m_szLine,0,1024);
		m_file->getline(m_szLine,1024);
		
		m_nFactor = 0;
		if(CScanner::m_szLine[0]){
			Scan(CScanner::m_szLine);
			bRet = true;
		}
	}

	return bRet;
}

/**	\brief	The CScanner::Scan function

	\param	line	a parameter of type std::string

	\return	bool	
*/
bool CScanner::Scan(std::string line){
	bool bRet=false;

	if(!line.empty()){
		unsigned int i=1, si=0, ei=0;
		
		m_nFactor = 0;
		ei = line.find(m_szDelimiter);
		while (ei != std::string::npos){
			memset(&m_aryFactor[m_nFactor],'\0',sizeof(char)*128);
			strcpy(m_aryFactor[m_nFactor++],line.substr(si,ei - si).data());
			si = ei + m_nDelimiterLen;
			ei = line.find(m_szDelimiter,si);
		}
		ei = line.size(); 
		strcpy(m_aryFactor[m_nFactor],line.substr(si,ei - si).data());
		bRet = true;
	}
	
	return bRet;
}

/**	\brief	The CScanner::SetDelimiter function

	\param	ptr	a parameter of type const char*

	\return	void	
*/
void CScanner::SetDelimiter(const char* ptr){
	memset(m_szDelimiter,'\0',3);
	strncpy(m_szDelimiter,ptr,2);
	m_nDelimiterLen = strlen(m_szDelimiter);
	//m_chDelimiter = ch;
}