#pragma once

#include "StdAfx.h"
#include <IsTools.h>

#include <map>
#include <string>
#include <vector>
#include <list>
using namespace std;

class CSmartDrawDocData
{
	CSmartDrawDocData(void);
	CSmartDrawDocData(const CSmartDrawDocData& rhs){}
public:
	typedef struct tagSizeRecord
	{
		STRING_T code;
		STRING_T metric , inch;
	}SizeRecord;

	typedef struct tagPaperSize
	{
		STRING_T name;
		double width , height;
	}PaperSize;

	static CSmartDrawDocData& GetInstance();
	~CSmartDrawDocData(void);

	int SetToken(const STRING_T&);
	STRING_T GetToken() const;

	/// 2013.11.13 added by humkyung
	CString GetFileVersion() const;
	/// up to here

	CString GetMSTNJFilePath(void);

	list<SizeRecord>* GetSizeRecordList(const bool& bReload=false);
	COLORREF GetColorCode(const STRING_T& rColorName) const;
	CString GetAppSettingFilePath(void) const;
	int SetServerFolderPath(const CString& rServerFolderPath);
	CString GetServerFolderPath() const;

	/// 2012.12.27 added by humkyung
	int SetUser(const CString& sUser);
	CString GetUser() const;
	int SetPassword(const CString& sPassword);
	CString GetPassword() const;
	/// up to here

	vector<RGBQuad>  m_PaletteTable;
	list<PaperSize>  m_PaperSizeList;
private:
	int ReadTableFile(map<int,CString>& table , const CString& rTableFilePath);

	CString m_rServerFolderPath , m_sUser , m_sPassword;
	map<STRING_T,STRING_T> m_RegExpMap,m_ColorMap;
	list<SizeRecord> m_SizeRecordList;
	STRING_T m_sToken;
};
