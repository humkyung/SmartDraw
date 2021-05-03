// Scanner.h: interface for the CScanner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCANNER_H__590F6AF2_3FE6_48EE_A1E1_0FDF3F8007E0__INCLUDED_)
#define AFX_SCANNER_H__590F6AF2_3FE6_48EE_A1E1_0FDF3F8007E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjInterface.h"
#include <string>
#include <fstream>

class DLL_EXPORT CScanner  
{
public:
	CScanner();
	CScanner(const char* pFilePath);
	virtual ~CScanner();
public:
	bool ReadLine();
	bool Read();
	bool Scan(std::string line);

	void SetDelimiter(const char* ptr);
public:
	char m_szDelimiter[3];
	char m_nDelimiterLen;

	int m_nFactor;
	char m_aryFactor[64][128];

	static char m_szLine[1024];
private:
	std::ifstream* m_file;
};

#endif // !defined(AFX_SCANNER_H__590F6AF2_3FE6_48EE_A1E1_0FDF3F8007E0__INCLUDED_)
