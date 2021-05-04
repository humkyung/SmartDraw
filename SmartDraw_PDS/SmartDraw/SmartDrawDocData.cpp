#include "StdAfx.h"
#include "SmartDrawDocData.h"
#include "SmartDrawDoc.h"
#include "StringTable.h"

#include <Tokenizer.h>
#include <Util/FileTools.h>
#include <Util/FileVersion.h>

///#include "soapLicWebServiceSoapProxy.h"

#include <fstream>
#include <vector>
using namespace std;

/*
extern "C" __declspec(dllimport) int __stdcall Encode(LMS_ENCODE* param);
extern "C" __declspec(dllimport) int __stdcall Decode(LMS_ENCODE* param);
*/

CSmartDrawDocData::CSmartDrawDocData(void)
{
	const CString rExecFilePath = GetExecPath();

	const CString sStringFilePath = rExecFilePath + _T("\\Setting\\STRING.TBL");
	CStringTable& table = CStringTable::GetInstance();
	table.Parse(sStringFilePath.operator LPCTSTR());

	{
		const CString rPaletteFilePath = GetExecPath() + _T("\\Setting\\MSTN.PAL");
		IFSTREAM_T ifile(rPaletteFilePath.operator LPCTSTR());
		if(ifile.is_open())
		{
			vector<STRING_T> oResult;
			STRING_T aLine;
			while(!ifile.eof())
			{
				getline(ifile , aLine);
				if(ifile.eof()) break;

				CTokenizer<CIsComma>::Tokenize(oResult , aLine , CIsComma());
				if(3 == oResult.size())
				{
					RGBQuad rgbquad;

					rgbquad.r = ATOI_T(oResult[0].c_str());
					rgbquad.g = ATOI_T(oResult[1].c_str());
					rgbquad.b = ATOI_T(oResult[2].c_str());
					m_PaletteTable.push_back(rgbquad);
				}
			}
		}
	}

	//! read paper size data
	{
		IFSTREAM_T ifile(GetExecPath() + _T("\\Setting\\PaperSize.dat"));
		if(ifile.is_open())
		{
			vector<STRING_T> oResult;
			STRING_T aLine;
			while(!ifile.eof())
			{
				getline(ifile , aLine);
				if(ifile.eof()) break;
				if(aLine.empty() || ('#' == aLine[0])) continue;

				PaperSize paperSize;
				CTokenizer<CIsFromString>::Tokenize(oResult , aLine , CIsFromString(_T("=")));
				if(2 == oResult.size())
				{
					paperSize.name = oResult[0];
					STRING_T rSize(oResult[1]);
					CTokenizer<CIsFromString>::Tokenize(oResult , rSize , CIsFromString(_T(":")));
					if(2 == oResult.size())
					{
						paperSize.width = ATOF_T(oResult[0].c_str());
						paperSize.height= ATOF_T(oResult[1].c_str());
						m_PaperSizeList.push_back(paperSize);
					}
				}
			}
			ifile.close();
		}
	}

	m_sUser = _T("USER");
}

CSmartDrawDocData::~CSmartDrawDocData(void)
{
	try
	{
	}
	catch(...)
	{
	}
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.07
*/
CSmartDrawDocData& CSmartDrawDocData::GetInstance()
{
	static CSmartDrawDocData __INSTANCE__;

	return __INSTANCE__;
}

/**
	@brief	return color code according to color name
	@author	HumKyung.BAEK
	@date	2010.05.07
*/
COLORREF CSmartDrawDocData::GetColorCode(const STRING_T& rColorName) const
{
	STRING_T res(_T("0,0,0"));

	map<STRING_T,STRING_T>::const_iterator where = m_ColorMap.find(rColorName);
	if(where != m_ColorMap.end()) res = (where->second);

	vector<STRING_T> oResult;
	CTokenizer<CIsComma>::Tokenize(oResult , res , CIsComma());
	if(3 == oResult.size())
	{
		return RGB(ATOI_T(oResult[0].c_str()) , ATOI_T(oResult[1].c_str()) , ATOI_T(oResult[2].c_str()));
	}

	return RGB(0,0,0);
}

/**
	@brief	return application setting file path
	@author	humkyung
	@date	2010.05.07
*/
CString CSmartDrawDocData::GetAppSettingFilePath(void) const
{
	const CString sCommonAppDataPath = CFileTools::GetCommonAppDataPath();
	const CString sAppSettingFilePath = sCommonAppDataPath + _T("\\SmartDraw_PDS\\SmartDraw.ini");

	return sAppSettingFilePath;
}

/**
	@brief	

	@author	BHK

	@date	2010.05.07
*/
int CSmartDrawDocData::SetServerFolderPath(const CString& rServerFolderPath)
{
	m_rServerFolderPath = rServerFolderPath;
	
	return ERROR_SUCCESS;
}

/**
	@brief	return server folder path

	@author	BHK

	@date	2010.05.07
*/
CString CSmartDrawDocData::GetServerFolderPath() const
{
	return m_rServerFolderPath;
}

/**
	@brief	set user

	@author	humkyung

	@date	2012.12.27
*/
int CSmartDrawDocData::SetUser(const CString& sUser)
{
	m_sUser = sUser;
	return ERROR_SUCCESS;
}

/**
	@brief	return user

	@author	humkyung

	@date	2012.12.27
*/
CString CSmartDrawDocData::GetUser() const
{
	return m_sUser;
}

/**
	@brief	set password

	@author	humkyung

	@date	2012.12.27
*/
int CSmartDrawDocData::SetPassword(const CString& sPassword)
{
	m_sPassword = sPassword;
	return ERROR_SUCCESS;
}

/**
	@brief	return password

	@author	humkyung

	@date	2012.12.27
*/
CString CSmartDrawDocData::GetPassword() const
{
	return m_sPassword;
}

/**
	@brief	read table file
	@author	HumKyung.BAEK
	@date	2010.05.10
*/
int CSmartDrawDocData::ReadTableFile(map<int,CString>& table , const CString& rTableFilePath)
{
	IFSTREAM_T ifile(rTableFilePath.operator LPCTSTR());
	if(ifile.is_open())
	{
		table.clear();

		int nLineNo = 1;
		STRING_T aLine;
		while(!ifile.eof())
		{
			getline(ifile , aLine);
			if('#' != aLine[0])	/// # is start of comments, so skip this line
			{
				table.insert(make_pair(nLineNo,aLine.c_str()));
			}
			++nLineNo;

			if(ifile.eof()) break;
		}
		ifile.close();

		return ERROR_SUCCESS;
	}

	return ERROR_FILE_NOT_FOUND;
}

/**
	@brief	return microstation j file path
	@author	HumKyung.BAEK
	@date	2010.06.28
*/
CString CSmartDrawDocData::GetMSTNJFilePath(void)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	const CString rInstallIniFilePath = GetExecPath() + _T("\\SmartDraw_NSIS.ini");
	if(GetPrivateProfileString(_T("Field 4") , _T("State") , NULL , szBuf , MAX_PATH , rInstallIniFilePath))
	{
		return (szBuf + CString(_T("\\ustation.exe")));
	}

	return CString();
}

/**
	@brief  get file version
	@author humkyung
	@date   2013.11.13
*/
CString CSmartDrawDocData::GetFileVersion() const
{
	CString res;

	TCHAR szModuleName[MAX_PATH+1];
	(void)GetModuleFileName(AfxGetInstanceHandle(), szModuleName , MAX_PATH);
	CFileVersion oVersion;
	if(TRUE == oVersion.Open(szModuleName))
	{
		res = oVersion.GetFileVersion();
	}

	return res;
}

/**
	@brief  return size record list
	@author humkyung
	@date   2013.11.13
*/
list<CSmartDrawDocData::SizeRecord>* CSmartDrawDocData::GetSizeRecordList(const bool& bReload)
{
	if(true == bReload) m_SizeRecordList.clear();

	/// read size table
	if(m_SizeRecordList.empty())
	{
		CSmartDrawDoc* pDoc = GetMDIActiveDocument();
		const STRING_T sRefFilePath = pDoc->GetRefFilePath();

		soci::session oSession(*soci::factory_sqlite3() , pDoc->GetRefFilePath().c_str());
		{
			soci::rowset<soci::row> rs(oSession.prepare << _T("select Metric,InchStr from size"));
			for(soci::rowset<soci::row>::const_iterator itr = rs.begin();itr != rs.end();++itr)
			{
				if((soci::indicator::i_ok != itr->get_indicator(0)) || (soci::indicator::i_ok != itr->get_indicator(1))) continue;
				SizeRecord oSizeRecord;
				{
					OSTRINGSTREAM_T oss;
					oss << itr->get<double>(0);
					oSizeRecord.metric = oss.str();
					
					oSizeRecord.inch = itr->get<STRING_T>(1);
					m_SizeRecordList.push_back(oSizeRecord);
				}
			}
		}
	}

	return &m_SizeRecordList;
}

/**
	@brief	set token
	@author	humkyung
	@date	2016.04.09
*/
int CSmartDrawDocData::SetToken(const STRING_T& sToken)
{
	m_sToken = sToken;
	return ERROR_SUCCESS;
}

/**
	@brief	return token
	@author	humkyung
	@date	2016.04.09
*/
STRING_T CSmartDrawDocData::GetToken() const
{
	return m_sToken;
}