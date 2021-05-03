// LineWeightComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDrawRsc.h"
#include "LineWeightComboBox.h"
#include <Tokenizer.h>

#include <fstream>
#include <vector>
using namespace std;

static vector<STRING_T> g_LineWeightList;

// CLineWeightComboBox

IMPLEMENT_DYNAMIC(CLineWeightComboBox, CComboBox)

CLineWeightComboBox::CLineWeightComboBox()
{

}

CLineWeightComboBox::~CLineWeightComboBox()
{
}


BEGIN_MESSAGE_MAP(CLineWeightComboBox, CComboBox)
END_MESSAGE_MAP()



// CLineWeightComboBox message handlers
/******************************************************************************
    @author     humkyung
	@brief		clear line weight list
    @date       2014-06-24
    @function   ResetContents
    @return     int
******************************************************************************/
int CLineWeightComboBox::ResetContents()
{
	g_LineWeightList.clear();
	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
	@brief		fill line style contents
    @date       2014-06-24
    @function   LoadContents
    @return     int
******************************************************************************/
int CLineWeightComboBox::LoadContents(const CString& sLineWeightFilePath)
{
	if(g_LineWeightList.empty())
	{
		IFSTREAM_T ifile(sLineWeightFilePath.operator LPCTSTR());
		if(ifile.is_open())
		{
			vector<STRING_T> oResult;
			STRING_T aLine;
			while(!ifile.eof())
			{
				getline(ifile , aLine);
				if(ifile.eof()) break;
				if(aLine.empty()) continue;
				
				g_LineWeightList.push_back(aLine);
				AddString(aLine.c_str());
			}

			return ERROR_SUCCESS;
		}
	}
	else
	{
		CComboBox::ResetContent();
		for(vector<STRING_T>::iterator itr = g_LineWeightList.begin();itr != g_LineWeightList.end();++itr)
		{
			AddString(itr->c_str());
		}

		return ERROR_SUCCESS;
	}

	return ERROR_FILE_NOT_FOUND;
}

