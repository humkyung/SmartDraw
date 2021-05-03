// SmartDrawAnnoColorButton.cpp : implementation file
//

#include "stdafx.h"
#include <Tokenizer.h>
#include <util/FileTools.h>
#include "PluginColorButton.h"

#include <fstream>
using namespace std;

// CPluginColorButton

IMPLEMENT_DYNAMIC(CPluginColorButton, CMFCColorButton)

CPluginColorButton::CPluginColorButton()
{

}

CPluginColorButton::~CPluginColorButton()
{
}


BEGIN_MESSAGE_MAP(CPluginColorButton, CMFCColorButton)
END_MESSAGE_MAP()



// CPluginColorButton message handlers

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.18
*/
int CPluginColorButton::GetColorIndex(void)
{
	int nIndex = 0;
	COLORREF clr = GetColor();
	for(vector<RGBQuad>::iterator itr = m_oColorList.begin();itr != m_oColorList.end();++itr,++nIndex)
	{
		if(clr == RGB(itr->r , itr->g , itr->b))
		{
			return nIndex;
		}
	}

	return 0;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.25
*/
int CPluginColorButton::SetColorIndex(const int& nIndex)
{
	SetColor(RGB(m_oColorList[nIndex].r , m_oColorList[nIndex].g , m_oColorList[nIndex].b));
	
	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.18
*/
void CPluginColorButton::PreSubclassWindow()
{
	CMFCColorButton::PreSubclassWindow();
}

static BOOL FileExist(CString rFilePath)
{
	if(!rFilePath.IsEmpty())
	{
		HANDLE hFile;

		hFile= CreateFile(rFilePath,
						  0,						/// Open for reading
						  FILE_SHARE_READ,			/// Share for reading
						  NULL,						/// No security
						  OPEN_EXISTING,			/// Existing file only
						  FILE_ATTRIBUTE_NORMAL,	/// Normal file
						  NULL);					/// No template file

		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return TRUE;
		}
	}
	return FALSE;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	2010.06.25
*/
int CPluginColorButton::FillColors(const CString& rIniFilePath)
{
	m_rIniFilePath = rIniFilePath;

	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	
	CString sExecPath;
	CFileTools::GetExecutableDirectory(sExecPath);

	CString rOutputFormat;
	if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , rIniFilePath))
	{
		rOutputFormat = szBuf;
		rOutputFormat.MakeUpper();
	}else	return ERROR_BAD_ENVIRONMENT;

	CString rPaletteFilePath;
	if(_T("AUTOCAD") == rOutputFormat)
	{
		rPaletteFilePath = sExecPath + _T("Setting\\AUTOCAD.PAL");
	}
	else if(_T("MSTN") == rOutputFormat)
	{
		rPaletteFilePath = m_rIniFilePath;
		int nFound = rPaletteFilePath.ReverseFind('\\');
		if(-1 != nFound)
		{
			rPaletteFilePath = rPaletteFilePath.Left(nFound);
			nFound = rPaletteFilePath.ReverseFind('\\');
			if(-1 != nFound)
			{
				rPaletteFilePath = rPaletteFilePath.Left(nFound) + _T("\\Border");
			}
		}
		if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , NULL , szBuf , MAX_PATH , rIniFilePath))
		{
			CString rString(szBuf);
			nFound = rString.ReverseFind('.');
			if(-1 != nFound)
			{
				rString = rString.Left(nFound) + _T(".PAL");
				rPaletteFilePath += _T("\\") + rString;
			}
		}

		if(FALSE == FileExist(rPaletteFilePath))
		{
			rPaletteFilePath = sExecPath + _T("Setting\\MSTN.PAL");
		}
	}

	IFSTREAM_T ifile(rPaletteFilePath.operator LPCTSTR());
	if(ifile.is_open())
	{
		m_oColorList.clear();

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
				m_oColorList.push_back(rgbquad);
			}
		}

		const UINT NUM_COLOURS = m_oColorList.size();
		const UINT nSize = sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*NUM_COLOURS;

		int nIndex = 0;
		LOGPALETTE *pLogPalette = (LOGPALETTE*)calloc(nSize , sizeof(BYTE));
		pLogPalette->palVersion = 0x300;
		pLogPalette->palNumEntries = NUM_COLOURS;
		for (vector<RGBQuad>::iterator itr = m_oColorList.begin(); itr != m_oColorList.end(); ++itr,++nIndex)
		{
			//set the new color combination
			pLogPalette->palPalEntry[nIndex].peRed   = itr->r;
			pLogPalette->palPalEntry[nIndex].peGreen = itr->g;
			pLogPalette->palPalEntry[nIndex].peBlue  = itr->b;
			pLogPalette->palPalEntry[nIndex].peFlags = 0;
		}
		m_Palette.CreatePalette(pLogPalette);
		free(pLogPalette);

		SetPalette(&m_Palette);
	}

	return ERROR_SUCCESS;
}
