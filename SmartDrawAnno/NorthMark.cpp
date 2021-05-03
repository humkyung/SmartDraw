#include "StdAfx.h"
#include "NorthMark.h"

#include <Tokenizer.h>
#include <fstream>
using namespace std;

CNorthMark::CNorthMark(const double& dAngle) : m_dAngle(dAngle)
{
}

CNorthMark::~CNorthMark(void)
{
}

/**
	@brief	write north mark
	@author	humkyung
	@date	2014.06.09
*/
int CNorthMark::Write(OFSTREAM_T& ofile , const double& dBorderScale , const double& dDrawingScale , const STRING_T& sIniFilePath)
{
	vector<STRING_T> oResult;
	const STRING_T sApp(_T("North Mark"));

	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	STRING_T sCellLibFilePath , sCellName;
	if(GetPrivateProfileString(sApp.c_str() , _T("FileName") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		sCellLibFilePath = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("CellName") , _T("NONE") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		sCellName = szBuf;
	}
	CIsPoint2d ptPos;
	if(GetPrivateProfileString(sApp.c_str() , _T("Pos") , _T("0,0") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		if(2 == oResult.size())
		{
			ptPos.Set(ATOF_T(oResult[0].c_str())*dBorderScale , ATOF_T(oResult[1].c_str())*dBorderScale);
		}
	}
	double dScale = 1.0;
	if(GetPrivateProfileString(sApp.c_str() , _T("Scale") , _T("1") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		dScale = ATOF_T(szBuf);
	}

	if(!sCellLibFilePath.empty() && !sCellName.empty())
	{
		ofile << _T("<INSERT>|");
		ofile << sCellLibFilePath << _T(",") << sCellName << _T(",");
		ofile << ptPos.x()*dDrawingScale << _T(",") << ptPos.y()*dDrawingScale << _T(",ORIGIN,");
		ofile << dScale*dDrawingScale << _T(",") << dScale*dDrawingScale << _T(",") << m_dAngle << _T("|");
		ofile << _T("1,No") << std::endl;
	}

	return ERROR_SUCCESS;
}