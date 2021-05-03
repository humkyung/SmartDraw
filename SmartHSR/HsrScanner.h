// PreFormatScanner.h: interface for the CHSRScanner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HSRSCANNER_H__71C75DB4_6B33_4904_8454_BF3C84C058D1__INCLUDED_)
#define AFX_HSRSCANNER_H__71C75DB4_6B33_4904_8454_BF3C84C058D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __DLL_EXPORT__
#define __DLL_EXPORT__
#endif

#include <IsTools.h>

#include <string>
#include <fstream>
#include <vector>

using namespace std;

#define SCANNER_BUF_SIZ		20480
#define	MAX_FACTOR_COUNT	320
#define FACTOR_BUF_SIZ		64

class __DLL_EXPORT__ CHSRScanner  
{
public:
	CHSRScanner(IFSTREAM_T* file);
	virtual ~CHSRScanner();

	int Reset(IFSTREAM_T* ifile);	/// 2012.03.06 added by humkyung
public:
	bool ReadLine();
	TCHAR* Val(const TCHAR* pVar);
private:
	bool Scan(const STRING_T& line);
public:
	bool GetLine(TCHAR* pszBuf , const size_t& count);
	int m_nFactor;
	///TCHAR m_aryFactor[MAX_FACTOR_COUNT][FACTOR_BUF_SIZ];
	vector<STRING_T>* m_aryFactor;

	int m_nLineNo;
	STRING_T* m_pstrLine;
private:
	IFSTREAM_T* m_pfile;
};

#endif // !defined(AFX_PREFORMATSCANNER_H__71C75DB4_6B33_4904_8454_BF3C84C058D1__INCLUDED_)
