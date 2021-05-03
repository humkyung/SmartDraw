// LineStyleComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDrawRsc.h"
#include "LineStyleComboBox.h"
#include <Tokenizer.h>

#include <fstream>
#include <vector>
using namespace std;

static vector<CLineStyleComboBox::LineStyle> g_LineStyleList;

// CLineStyleComboBox

IMPLEMENT_DYNAMIC(CLineStyleComboBox, CComboBox)

CLineStyleComboBox::CLineStyleComboBox()
{

}

CLineStyleComboBox::~CLineStyleComboBox()
{
}


BEGIN_MESSAGE_MAP(CLineStyleComboBox, CComboBox)
END_MESSAGE_MAP()

// CLineStyleComboBox message handlers
/******************************************************************************
    @author     humkyung
	@brief		clear line style list
    @date       2014-06-24
    @function   ResetContents
    @return     int
******************************************************************************/
int CLineStyleComboBox::ResetContents()
{
	g_LineStyleList.clear();
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
	@brief		fill line style contents
    @date       2014-06-23
    @function   LoadContents
    @return     int
******************************************************************************/
int CLineStyleComboBox::LoadContents(const CString& sLineStyleFilePath)
{
	if(g_LineStyleList.empty())
	{
		IFSTREAM_T ifile(sLineStyleFilePath.operator LPCTSTR());
		if(ifile.is_open())
		{
			vector<STRING_T> oResult;
			STRING_T aLine;
			while(!ifile.eof())
			{
				getline(ifile , aLine);
				if(ifile.eof()) break;
				if(aLine.empty() || (0 == aLine.find(_T(";;")))) continue;	/// skip comment
				if('*' == aLine[0])
				{
					CTokenizer<CIsComma>::Tokenize(oResult , aLine , CIsComma());
					if(2 == oResult.size())
					{
						LineStyle oLineStyle;
						oLineStyle.sType = oResult[0].substr(1);
						oLineStyle.sDesc = oResult[1];
						g_LineStyleList.push_back(oLineStyle);
					}
				}
			}
		}
	}
	
	if(!g_LineStyleList.empty())
	{
		for(vector<CLineStyleComboBox::LineStyle>::iterator itr = g_LineStyleList.begin();itr != g_LineStyleList.end();++itr)
		{
			AddString(itr->sType.c_str());
		}
		
		return ERROR_SUCCESS;
	}

	return ERROR_FILE_NOT_FOUND;
}