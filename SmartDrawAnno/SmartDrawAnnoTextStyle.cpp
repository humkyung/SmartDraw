#include "StdAfx.h"
#include "SmartDrawAnnoTextStyle.h"
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoEnv.h"

#include <Tokenizer.h>

CSmartDrawAnnoTextStyle::CSmartDrawAnnoTextStyle(void) : m_rFontName(_T("3"))
{
	m_nHeight = 1.0;
	m_nWidthFactor = 1.0;
	m_nOblique = 0.0;
	m_nWeight = 0;
}

CSmartDrawAnnoTextStyle::CSmartDrawAnnoTextStyle(const STRING_T& sFontName , const double& dHeight , const double& dWidthFactor) : m_nOblique(0.0) , m_nWeight(0)
{
	m_rFontName = sFontName;
	m_nHeight = dHeight;
	m_nWidthFactor = dWidthFactor;
}

CSmartDrawAnnoTextStyle::~CSmartDrawAnnoTextStyle(void)
{
}

/******************************************************************************
    @author     humkyung
    @date       2014-06-28
    @function   SetName
    @return     void
    @brief		assign name with given name
******************************************************************************/
void CSmartDrawAnnoTextStyle::SetName(const STRING_T& sName)
{
	m_rName = sName;
}

/******************************************************************************
    @author     humkyung
    @date       2014-06-29
    @function   GetName
    @return     STRING_T
    @brief		return text style name
******************************************************************************/
STRING_T CSmartDrawAnnoTextStyle::GetName() const
{
	return m_rName;
}

/******************************************************************************
    @author     humkyung
    @date       2014-06-28
    @function   GetFontName
    @return     STRING_T
    @brief
******************************************************************************/
STRING_T CSmartDrawAnnoTextStyle::GetFontName() const
{
	return m_rFontName;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-25
    @function   width
    @return     double
    @brief
******************************************************************************/
double CSmartDrawAnnoTextStyle::width() const
{
	return m_nHeight*m_nWidthFactor;
}

/******************************************************************************
    @author     humkyung
    @date       2014-06-28
    @function   height
    @return     double
    @brief
******************************************************************************/
double CSmartDrawAnnoTextStyle::height() const
{
	return m_nHeight;
}

/******************************************************************************
    @author     humkyung
    @date       2014-06-28
    @function   height
    @return     double&
    @brief
******************************************************************************/
double& CSmartDrawAnnoTextStyle::height()
{
	return m_nHeight;
}

/******************************************************************************
    @author     humkyung
    @date       2014-06-28
    @function   weight
    @return     int
    @brief
******************************************************************************/
int CSmartDrawAnnoTextStyle::weight() const
{
	return m_nWeight;
}

/******************************************************************************
    @author     humkyung
    @date       2014-06-28
    @function   weight
    @return     int&
    @brief
******************************************************************************/
int& CSmartDrawAnnoTextStyle::weight()
{
	return m_nWeight;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-25
    @function   GetWidthFactor
    @return     double
    @brief
******************************************************************************/
double CSmartDrawAnnoTextStyle::GetWidthFactor() const
{
	return m_nWidthFactor;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-25
    @function   GetOblque
    @return     double
    @brief
******************************************************************************/
double CSmartDrawAnnoTextStyle::GetOblique() const
{
	return m_nOblique;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-09
    @class
    @function   FileExist
    @return     bool
    @param      STRING_T    sFilePath
    @brief
******************************************************************************/
static bool FileExist(STRING_T sFilePath)
{
	if(!sFilePath.empty())
	{
		HANDLE hFile;

		hFile= CreateFile(sFilePath.c_str(),
						  0,						/// Open for reading
						  FILE_SHARE_READ,			/// Share for reading
						  NULL,						/// No security
						  OPEN_EXISTING,			/// Existing file only
						  FILE_ATTRIBUTE_NORMAL,	/// Normal file
						  NULL);					/// No template file

		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return true;
		}
	}
	return false;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.29
	@param
	@return	
*/
double CSmartDrawAnnoTextStyle::GetTextLength(const STRING_T& rTextString)
{
	double res = 0.f;

	if(!m_rFontName.empty() && !rTextString.empty())
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		CVFTFile *pVFTFile = NULL;
		map<STRING_T , CVFTFile*>::iterator where = CSmartDrawAnnoModule::m_VFTFontMap.find(m_rFontName);
		if(where == CSmartDrawAnnoModule::m_VFTFontMap.end())
		{
			const STRING_T& rFontPath = GetExecPath() + _T("\\Setting\\Fonts\\") + 
				STRING_T((MSTN == env.m_generation.m_eOutputFormat) ? _T("MSTN\\") : _T("AutoCAD\\")) + m_rFontName + _T(".vft");
			if(::FileExist(rFontPath))
			{
				pVFTFile = new CVFTFile;
				pVFTFile->Read(rFontPath.c_str());
				CSmartDrawAnnoModule::m_VFTFontMap.insert(make_pair(m_rFontName , pVFTFile));
			}
		}else	pVFTFile = where->second;

		if(pVFTFile)
		{
			//if(MSTN == env.m_generation.m_eOutputFormat)
			{
				const int count = rTextString.length();
				///STRING_T temp;
				///temp.assign(rTextString[count - 1]);
				const double aLength = pVFTFile->TextLength(&(rTextString[count - 1]) , 1.0 , 1.0);
				res = (count - 1) + aLength;
				res *= m_nHeight;
				res *= m_nWidthFactor;
			}
			/*else
			{
				res = pVFTFile->TextLength(rTextString.c_str() , 1.0 , 1.0);
				res *= m_nHeight;
				res *= m_nWidthFactor;
			}*/
		}
		else
		{
			res = rTextString.length() * m_nHeight * m_nWidthFactor;;
		}
	}
	else
	{
		res = rTextString.length() * m_nHeight * m_nWidthFactor;
	}

	return res;
}

/**	
	@brief	read text style from ini file
	@author	humkyung
	@date	2010.06.12
	@param
	@return	
*/
int CSmartDrawAnnoTextStyle::Parse(const STRING_T& sTextStyleName , const STRING_T& rIniFilePath)
{
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};
	
	STRING_T sOutputFormat;
	if(GetPrivateProfileString(_T("GENERATION_OPTIONS") , _T("Output Format") , NULL , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		sOutputFormat.assign(szBuf);
	}else	return ERROR_BAD_ENVIRONMENT;

	m_rName = sTextStyleName;
	if(GetPrivateProfileString(STRING_T(sOutputFormat + _T("_TEXT_STYLE")).c_str() , m_rName.c_str() , NULL , szBuf , MAX_PATH , rIniFilePath.c_str()))
	{
		vector<STRING_T> oResult;
		CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
		if(5 == oResult.size())
		{
			m_rFontName = oResult[0];
			m_nHeight = ATOF_T(oResult[1].c_str());
			m_nWidthFactor = ATOF_T(oResult[2].c_str());
			m_nOblique= ATOF_T(oResult[3].c_str());
			m_nWeight = ATOI_T(oResult[4].c_str());

			return ERROR_SUCCESS;
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}

/**
	@brief	write text style name or font name which does depend on output format
	@author	humkyung
	@date	2014.06.28
*/
OSTREAM_T& operator <<(OSTREAM_T& c , const CSmartDrawAnnoTextStyle& t)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();	
	if(MSTN == env.m_generation.m_eOutputFormat)
	{
		c << t.m_rFontName << _T(",") << t.m_nWeight;
	}
	else
	{
		c << t.m_rName << _T(",") << t.m_nWeight;
	}
	return c; 
};