// SmartDrawAnnoColorButton.cpp : implementation file
//

#include "stdafx.h"
#include <assert.h>
#include <Tokenizer.h>
#include "SmartDrawRsc.h"
#include "SmartDrawColorButton.h"

#include <fstream>
using namespace std;

// CSmartDrawColorButton

IMPLEMENT_DYNAMIC(CSmartDrawColorButton, CMFCColorButton)

CSmartDrawColorButton::CSmartDrawColorButton()
{
	m_pColorList = new vector<RGBQuad>();
}

CSmartDrawColorButton::~CSmartDrawColorButton()
{
	try
	{
		SAFE_DELETE(m_pColorList);
	}
	catch(...)
	{
	}
}

BEGIN_MESSAGE_MAP(CSmartDrawColorButton, CMFCColorButton)
END_MESSAGE_MAP()

// CSmartDrawColorButton message handlers

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.18
*/
int CSmartDrawColorButton::GetColorIndex(void)
{
	int nIndex = 0;
	COLORREF clr = GetColor();
	for(vector<RGBQuad>::iterator itr = m_pColorList->begin();itr != m_pColorList->end();++itr,++nIndex)
	{
		if(clr == RGB(itr->r , itr->g , itr->b))
		{
			return nIndex;
		}
	}

	return -1;
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.25
*/
int CSmartDrawColorButton::SetColorIndex(const int& nIndex)
{
	if((NULL != m_pColorList) && (m_pColorList->size() > UINT(nIndex)))
	{
		SetColor(RGB((*m_pColorList)[nIndex].r , (*m_pColorList)[nIndex].g , (*m_pColorList)[nIndex].b));
	}
	
	return ERROR_SUCCESS;
}

/**
	@brief	
	@author	HumKyung.BAEK
	@date	2010.06.18
*/
void CSmartDrawColorButton::PreSubclassWindow()
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
int CSmartDrawColorButton::FillColors(const CString& rIniFilePath , const CString& sOutputFormat)
{
	m_rIniFilePath = rIniFilePath;

	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	CString rPaletteFilePath;
	if(_T("AUTOCAD") == sOutputFormat)
	{
		rPaletteFilePath = GetExecPath() + _T("\\Setting\\AUTOCAD.PAL");
	}
	else if(_T("MSTN") == sOutputFormat)
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
			rPaletteFilePath = GetExecPath() + _T("\\Setting\\MSTN.PAL");
		}
	}

	IFSTREAM_T ifile(rPaletteFilePath.operator LPCTSTR());
	if(ifile.is_open())
	{
		m_pColorList->clear();

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
				m_pColorList->push_back(rgbquad);
			}
		}

		this->FillColors(m_pColorList);
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-04-15
    @function   FillColors
    @return     int
    @param      vector<CSmartDrawColorButton::RGBQuad>* pColorList
    @brief
******************************************************************************/
int CSmartDrawColorButton::FillColors(vector<CSmartDrawColorButton::RGBQuad>* pColorList)
{
	assert(pColorList && "pColorList is NULL");

	if(pColorList)
	{
		const UINT NUM_COLOURS = pColorList->size();
		const UINT nSize = sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*NUM_COLOURS;

		int nIndex = 0;
		LOGPALETTE *pLogPalette = (LOGPALETTE*)calloc(nSize , sizeof(BYTE));
		pLogPalette->palVersion = 0x300;
		pLogPalette->palNumEntries = NUM_COLOURS;
		for (vector<RGBQuad>::iterator itr = pColorList->begin(); itr != pColorList->end(); ++itr,++nIndex)
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

		if(m_pColorList->empty()) m_pColorList->insert(m_pColorList->begin() , pColorList->begin() , pColorList->end());

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}