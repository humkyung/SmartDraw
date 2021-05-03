#include "StdAfx.h"
#include "KeyPlan.h"
#include <Tokenizer.h>
#include <IsPoint2d.h>

#include <fstream>
using namespace std;

CKeyPlan::CKeyPlan(void)
{
}

CKeyPlan::~CKeyPlan(void)
{
}

/**
	@brief	write keyplan data
	@author	humkyung
	@date	2014.06.09
*/
int CKeyPlan::Write(OFSTREAM_T& ofile , const double& dBorderScale , const double& dDrawingScale , const STRING_T& sIniFilePath)
{
	vector<STRING_T> oResult;
	const STRING_T sApp(_T("Key Plan"));

	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	STRING_T sCellLibFilePath , sCellName;
	if(GetPrivateProfileString(sApp.c_str() , _T("FileName") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		sCellLibFilePath = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("CellName") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		sCellName = szBuf;
	}
	CIsPoint2d ptPos;
	if(GetPrivateProfileString(sApp.c_str() , _T("Pos") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
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
		ofile << dScale*dDrawingScale << _T(",") << dScale*dDrawingScale << _T(",0|");
		ofile << _T("1,No") << std::endl;
	}

	return ERROR_SUCCESS;
}