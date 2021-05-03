// CHSROption.cpp: implementation of the CHSROption class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <assert.h>
#include <fstream>
#include "HSROption.h"

#include <Intent.h>
#include "SmartHSR.h"
#include "HSRView.h"
#include "Tokenizer.h"

/**
	@brief	parse hidden option property
	@author	humkyung
	@date	2014.06.05
*/
int CComponentProp::Parse(LPCTSTR pszLine)
{
	assert(pszLine && "pszLine is NULL");

	if(NULL != pszLine)
	{
		vector<STRING_T> oResult;
		CTokenizer<CIsComma>::Tokenize( oResult , pszLine , CIsComma() );
		if(oResult.size() >= 4)
		{
			m_sLevel = oResult[0];
			m_sColor = oResult[1];
			m_sStyle = oResult[2];
			m_sWeight= oResult[3];
			/// 2014.06.05 added by humkyung
			if(5 == oResult.size())
			{
				m_isOn = (0 == STRICMP_T(_T("True") , oResult[4].c_str()));
			}
			/// up to here
		}

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	return true when m_isOn is set
	@author	humkyung
	@date	2014.11.15
*/
bool CComponentProp::IsOn() const
{
	return m_isOn;
}

/**
	@brief	return true when m_sColor is -1
	@author	humkyung
	@date	2015.03.22
*/
bool CComponentProp::IsAutomaticColor() const
{
	return (_T("-1") == m_sColor);
}

/**
	@brief	return color
	@author	humkyung
	@date	2015.03.22
*/
STRING_T CComponentProp::color() const
{
	return (m_sColor);
}

OSTREAM_T& operator <<(OSTREAM_T& c , const CComponentProp& t)
{ 
	c << t.m_sLevel << _T(",") << t.m_sColor << _T(",") << t.m_sStyle << _T(",") << t.m_sScale << _T(",") << t.m_sWeight;
	return c; 
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	
	@brief
*/
CHSROption::CHSROption()
{
	m_nUnit = Metric;
	//m_nUnit = Inch_Feet; /// for CADWorx
	m_nWeldMinDotDist = 0.;
	m_bPipeCenterOn = false;
}

CHSROption::~CHSROption()
{

}

/**	\brief	The CHSROption::GetPipeLineMode function


	\return	PIPELINEMODE	
*/
PIPELINEMODE CHSROption::GetPipeLineMode() const
{
	return m_nPipeLineMode;
}

/**	\brief	The CHSROption::WeldDotDist function


	\return	double	
*/
const double CHSROption::WeldMinDotDist()
{
	return m_nWeldMinDotDist;
}

/**	\brief	The CHSROption::GetCriticalRadius function

	\remarks\n
	;2003-03-28 - 

	\return	double	
*/
double CHSROption::GetCriticalRadius()
{
	CHSRView* pHiddenView = GetHiddenView();
	if(pHiddenView)
	{
		const double nUOR = pHiddenView->GetUOR();
		return (m_nCriticalPipeRadius*nUOR)*pHiddenView->GetViewScale();
	}else	return 0.;
}



/**	
	@brief	read options from ini file.
	@author	humkyung
	@date	????.??.??
	@param	pFilePath	a parameter of type const char*
	@return	bool	
*/
bool CHSROption::Read(LPCTSTR pFilePath)
{
	assert(pFilePath && "pFilePath is NULL");
	bool bRet=false;

	if(pFilePath)
	{
		static const TCHAR sApp[MAX_PATH]=_T("HLR_OPTIONS");
		static TCHAR szLine[MAX_PATH + 1]={0,};

		try
		{
			Init();
			CHSRPipe::ReadEnv(pFilePath);

			IFSTREAM_T file(pFilePath);
			if(file.good())
			{
				if(GetPrivateProfileString( _T("UNIT") , _T("Input") , NULL , szLine , MAX_PATH , pFilePath))
				{
					if(0 == STRCMP_T(szLine , _T("Inch Feet")))
					{
						m_nUnit = Inch_Feet;
					}
					else
					{
						m_nUnit = Metric;
					}
				}

				m_optWeldElbow.Angle = m_optSWandTHDElbow.Angle = 30;
				while(!file.eof() && file.good())
				{
					memset(szLine,'\0',1024);
					file.getline(szLine,1024);
					
					ParseLine(szLine);
				}
			}
			file.close();

			m_nPipeLineMode = DOUBLE_LINE;
			if(GetPrivateProfileString(sApp , _T("DisplayStyle") , _T("") , szLine , MAX_PATH , pFilePath))
			{
				if(0 == STRICMP_T(szLine , _T("Depend on size")))   m_nPipeLineMode = BOTH;
				if(0 == STRICMP_T(szLine , _T("All symbol line")))	m_nPipeLineMode = SINGLE_LINE;
				if(0 == STRICMP_T(szLine , _T("All as it was")))	m_nPipeLineMode = DOUBLE_LINE;
			}

			m_nCriticalPipeRadius = 0.0;
			if(GetPrivateProfileString(sApp , _T("CriticalSize") , _T("") , szLine , MAX_PATH , pFilePath))
			{
				TCHAR szBuf[32]={0,};

				vector<STRING_T> oResult;
				CTokenizer<CIsFromString>::Tokenize(oResult , szLine , CIsFromString(_T("-")));
				if(2 == oResult.size())
				{
					m_nCriticalPipeRadius = ATOF_T(oResult[1].c_str())*0.5;
				}
			}

			/// read view critical distance
			CHSRApp::TOLERANCE = 1.f;
			if(GetPrivateProfileString(sApp , _T("View_Critical_Distance") , NULL , szLine , MAX_PATH , pFilePath))
			{
				CHSRApp::TOLERANCE = ATOF_T(szLine);
			}

			/// added by HumKyung.Baek - 2010.11.12
			vector<STRING_T> oResult;
			if(GetPrivateProfileString( sApp , _T("Pipe") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oCompProp[LAYER_PIPE_DOU].Parse(szLine);
				m_oCompProp[LAYER_ELBOW_DOU] = m_oCompProp[LAYER_PIPE_DOU];
				m_oCompProp[LAYER_TEE_DOU] = m_oCompProp[LAYER_PIPE_DOU];
			}

			/// read pipe,elbow,tee center line property - 2014.08.27 added by humkyung
			if(GetPrivateProfileString( sApp , _T("Pipe center line") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oCompProp[LAYER_PIPE_CEN].Parse(szLine);
				m_oCompProp[LAYER_ELBOW_CEN] = m_oCompProp[LAYER_PIPE_CEN];
				m_oCompProp[LAYER_TEE_CEN] = m_oCompProp[LAYER_PIPE_CEN];
			}
			/// up to here

			if(GetPrivateProfileString( sApp , _T("Pipe symbol line") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oCompProp[LAYER_PIPE_SIN].Parse(szLine);
				m_oCompProp[LAYER_ELBOW_SIN] = m_oCompProp[LAYER_PIPE_SIN];
				m_oCompProp[LAYER_ELBOW_WELD_MARK_LINE] = m_oCompProp[LAYER_PIPE_SIN];
				m_oCompProp[LAYER_TEE_SIN] = m_oCompProp[LAYER_PIPE_SIN];
			}
			if(GetPrivateProfileString( sApp , _T("Pipe hidden line") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oHiddenCompProp[LAYER_PIPE_DOU_HID].Parse(szLine);
				m_oHiddenCompProp[LAYER_ELBOW_DOU_HID] = m_oHiddenCompProp[LAYER_PIPE_DOU_HID];
				m_oHiddenCompProp[LAYER_TEE_DOU_HID] = m_oHiddenCompProp[LAYER_PIPE_DOU_HID];
			}
			if(GetPrivateProfileString( sApp , _T("Pipe symbol hidden line") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oHiddenCompProp[LAYER_PIPE_SIN_HID].Parse(szLine);
				m_oHiddenCompProp[LAYER_ELBOW_SIN_HID] = m_oHiddenCompProp[LAYER_PIPE_SIN_HID];
				m_oHiddenCompProp[LAYER_TEE_SIN_HID] = m_oHiddenCompProp[LAYER_PIPE_SIN_HID];
			}

			if(GetPrivateProfileString(sApp , _T("Pipe Misc.") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oCompProp[LAYER_PIPE_OTHER].Parse(szLine);
			}
			if(GetPrivateProfileString(sApp , _T("Pipe Misc. hidden line") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oHiddenCompProp[LAYER_PIPE_OTHER_HID].Parse(szLine);
			}

			if(GetPrivateProfileString(sApp , _T("Equipment") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oCompProp[LAYER_EQUI].Parse(szLine);
			}
			if(GetPrivateProfileString(sApp , _T("Equipment hidden line") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oHiddenCompProp[LAYER_EQUI_HID].Parse(szLine);
			}

			if(GetPrivateProfileString(sApp , _T("Structure") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oCompProp[LAYER_STRU].Parse(szLine);
			}
			if(GetPrivateProfileString(sApp , _T("Structure hidden line") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oHiddenCompProp[LAYER_STRU_HID].Parse(szLine);
			}
			
			if(GetPrivateProfileString(sApp , _T("Misc.") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oCompProp[LAYER_OTHER].Parse(szLine);
			}
			if(GetPrivateProfileString(sApp , _T("Misc. hidden line") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oHiddenCompProp[LAYER_OTHER_HID].Parse(szLine);
			}

			if(GetPrivateProfileString(sApp , _T("View line") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oCompProp[LAYER_VIEW].Parse(szLine);
			}

			if(GetPrivateProfileString(sApp , _T("Instrument") , _T("1,1,0,0,True") , szLine , MAX_PATH , pFilePath))
			{
				m_oCompProp[LAYER_INSTRUMENT].Parse(szLine);
			}
			if(GetPrivateProfileString(sApp , _T("Instrument hidden line") , _T("1,1,0,0,True") , szLine , MAX_PATH , pFilePath))
			{
				m_oHiddenCompProp[LAYER_INSTRUMENT_HID].Parse(szLine);
			}
			/// up to here

			/// read weld mark option - 2014.11.15 added by humkyung
			if(GetPrivateProfileString(sApp , _T("Weld Mark") , NULL , szLine , MAX_PATH , pFilePath))
			{
				m_oCompProp[LAYER_ELBOW_WELD_MARK_CIRCLE].Parse(szLine);
			}
			if(GetPrivateProfileString(sApp , _T("Weld Mark Size") , _T("1") , szLine , MAX_PATH , pFilePath))
			{
				m_optWeldElbow.Size = ATOF_T(szLine);
			}
			/// up to here

			/*if(GetPrivateProfileString(_T("Structure") , _T("Grid_Circle_Level") , _T("10") , szLine , MAX_PATH , pFilePath))
			{
				m_sGridCircleLevel.assign(szLine);
			}*/

			bRet = true;
		}
		catch(...)
		{
			OutputDebugString(_T("ini file error"));
		}
	}

	return bRet;
}

/**	
	@brief	The CHSROption::ConvertSize function

	@author	humkyung

	@param	pSize	a parameter of type const char*

	@return	const char*	
*/
LPCTSTR CHSROption::ConvertSize(LPCTSTR pSize)
{
	assert(pSize && "pSize is NULL");
	const static TCHAR szOrgSizeTable[28][16]=
	{
		_T("6A-1/8B") ,_T("8A-1/4B")  ,_T("10A-3/8B") ,_T("15A-1/2B") ,
		_T("20A-3/4B"),_T("25A-1B")   ,_T("32A-11/4B"),_T("40A-11/2B"),
		_T("50A-2B")  ,_T("65A-21/2B"),_T("80A-3B")   ,_T("90A-31/2B"),
		_T("100A-4B") ,_T("125A-5B")  ,_T("150A-6B")  ,_T("200A-8B")  ,
		_T("250A-10B"),_T("300A-12B") ,_T("350A-14B") ,_T("400A-16B") ,
		_T("450A-18B"),_T("500A-20B") ,_T("550A-22B") ,_T("600A-24B") ,
		_T("650A-26B"),_T("700A-28B") ,_T("750A-30B") ,_T("800A-32B")
	};
		
	const static TCHAR szDstSizeTable[28][16]=
	{
		_T("4A-1/8B") ,_T("10A-1/4B") ,_T("18A-3/8B") ,_T("22A-1/2B") ,
		_T("26A-3/4B"),_T("34A-1B")   ,_T("44A-11/4B"),_T("50A-11/2B"),
		_T("62A-2B")  ,_T("74A-21/2B"),_T("90A-3B")   ,_T("98A-31/2B"),
		_T("116A-4B") ,_T("142A-5B")  ,_T("170A-5B")  ,_T("220A-8B")  ,
		_T("274A-10B"),_T("324A-12B") ,_T("364A-14B") ,_T("408A-16B") ,
		_T("458A-18B"),_T("510A-20B") ,_T("560A-22B") ,_T("610A-24B") ,
		_T("662A-26B"),_T("712A-28B") ,_T("764A-30B") ,_T("814A-32B")
	};

	if(pSize)
	{
		for(int i = 0;i < 27;i++)
		{
			if(0 == STRCMP_T(szOrgSizeTable[i],pSize)) return szDstSizeTable[i];
		}
	}

	return pSize;
}

/**	
	@brief	read option file.
	@author humkyung
	@param	pszLine	a parameter of type const char*
	@return	void	
*/
void CHSROption::ParseLine(LPCTSTR pszLine)
{
	assert(pszLine && "pszLine is NULL");

	if(pszLine)
	{
		TCHAR szKey[128]={0,},szValue[128]={0,};
		
		if(0 == STRCMP_T(VER,_T("HSR Ver 0.2")))
		{
			if(STRCHR_T(pszLine,'='))
			{
				int i = 0;
				for(i = 0;pszLine[i] && ('=' != pszLine[i]);i++) szKey[i] = pszLine[i];
				STRNCPY_T(szValue,pszLine + i + 1,63);
				
				if(0 == STRCMP_T(szKey,_T("PIPE_CenterLine")))
				{
					m_bPipeCenterOn = (0 == STRCMP_T(szValue,_T("ON"))) ? true : false;
				}
				else if(0 == STRCMP_T(szKey,_T("ELBO_SW_Type")))
				{
					m_optSWandTHDElbow.Round = (0 == STRCMP_T(szValue,_T("Round"))) ? true : false;
				}
				else if(0 == STRCMP_T(szKey,_T("ELBO_SW_Center")))
				{
					m_optSWandTHDElbow.CenterOn = (0 == STRCMP_T(szValue,_T("ON"))) ? true : false;
				}
				else if(0 == STRCMP_T(szKey,_T("ELBO_SW_Cross")))
				{
					m_optSWandTHDElbow.CrossOn = (0 == STRCMP_T(szValue,_T("USE"))) ? true : false;
				}
				else if(0 == STRCMP_T(szKey,_T("ELBO_SW_DistA")))
				{
					m_optSWandTHDElbow.DistA = ATOF_T(szValue);
				}
				else if(0 == STRCMP_T(szKey,_T("ELBO_SW_DistB")))
				{
					m_optSWandTHDElbow.DistB = ATOF_T(szValue);
				}
				else if(0 == STRCMP_T(szKey,_T("ELBO_SW_Angle")))
				{
					m_optSWandTHDElbow.Angle = ATOF_T(szValue);
				}
				else if(0 == STRCMP_T(szKey,_T("ELBO_SW_MinSize")))
				{
					m_optSWandTHDElbow.MinSize = ATOF_T(szValue);
				}
				else if(0 == STRCMP_T(szKey,_T("Elbow_SW_Trimed_Min")))
				{
					m_optSWandTHDElbow.TrimedMin = (0 == STRCMP_T(szValue,_T("True"))) ? true : false;
				}
				else if(0 == STRCMP_T(szKey,_T("Elbow_SW_Trimed_MinNot")))
				{
					m_optSWandTHDElbow.TrimedMinNot = (0 == STRCMP_T(szValue,_T("True"))) ? true : false;
				}
				else if(0 == STRCMP_T(szKey,_T("ELBO_WELD_Center")))
				{
					m_optWeldElbow.CenterOn = (0 == STRCMP_T(szValue,_T("ON"))) ? true : false;
				}
				else if(0 == STRCMP_T(szKey,_T("ELBO_WELD_Cross")))
				{
					m_optWeldElbow.CrossOn = (0 == STRCMP_T(szValue,_T("ON"))) ? true : false;
				}
				else if(0 == STRCMP_T(szKey,_T("ELBO_WELD_DistA")))
				{
					m_optWeldElbow.DistA = ATOF_T(szValue);
				}
				else if(0 == STRCMP_T(szKey,_T("ELBO_WELD_DistB")))
				{
					m_optWeldElbow.DistB = ATOF_T(szValue);
				}
				else if(0 == STRCMP_T(szKey,_T("ELBO_WELD_Angle")))
				{
					m_optWeldElbow.Angle = ATOF_T(szValue);
				}
				else if(0 == STRCMP_T(szKey , _T("ELBO_WELD_MinSize")))
				{
					m_optWeldElbow.MinSize = ATOF_T(szValue);
				}
				else if(0 == STRCMP_T(szKey,_T("TEE_Center")))
				{
					m_optTee.CenterOn = (0 == STRCMP_T(szValue,_T("ON"))) ? true : false;
				}
				else if(0 == STRCMP_T(szKey,_T("TEE_Cross")))
				{
					m_optTee.CrossOn = (0 == STRCMP_T(szValue,_T("True"))) ? true : false;
				}
				else if(0 == STRCMP_T(szKey,_T("TEE_DistA")))
				{
					m_optTee.DistA = ATOF_T(szValue);
				}
				else if(0 == STRCMP_T(szKey,_T("TEE_DistB")))
				{
					m_optTee.DistB = ATOF_T(szValue);
				}
			}
		}
	}
}

/**	
	@brief	The CHSROption::IsWeldElbowCenterlineOn function
	@return	bool	
*/
bool CHSROption::IsWeldElbowCenterlineOn()
{
	return m_oCompProp[LAYER_ELBOW_CEN].m_isOn;
}

/**	\brief	The CHSROption::IsWeldElbowCrosslineOn function


	\return	bool	
*/
bool CHSROption::IsWeldElbowCrosslineOn()
{
	return m_optWeldElbow.CrossOn;
}

//	parameter	:
//	description	:
//	remarks		:
//	returns		:
double CHSROption::GetWeldElbowDistA()
{
	return m_optWeldElbow.DistA;
}

/**	\brief	The CHSROption::GetWeldElbowDistB function


	\return	double	
*/
double CHSROption::GetWeldElbowDistB()
{
	return m_optWeldElbow.DistB;
}

/**	
	@brief	The CHSROption::GetWeldDotSize function
	@author	humkyung
	@return	double	
*/
double CHSROption::GetWeldDotSize() const
{
	return m_optWeldElbow.Size;
}

/**	
	@brief	The CHSROption::GetWeldElbowAngle function
	@return	double	
*/
double CHSROption::GetWeldElbowAngle() const
{
	return m_optWeldElbow.Angle;
}

/**	
	@brief	The CHSROption::GetWeldElbowSize function
	@return	double
*/
double CHSROption::GetWeldElbowSize() const
{
	return m_optWeldElbow.Size;
}

/**	
	@brief	The CHSROption::IsHiddenlineOn function
	@author	humkyung
	@return	bool	
*/
bool CHSROption::IsHiddenlineOn(const HSR_LAYER_E& eLayer) const
{
	return m_oHiddenCompProp[eLayer].m_isOn;
}

/**	
	@brief	return grid circle level
	@author	humkyung
	@return	STRING_T
*/
//STRING_T CHSROption::GetGridCircleLevel() const
//{
//	return m_sGridCricleLevel;
//}

/**
	@brief	return true when view line is on otherwise return false
	@author	humkyung
	@date	2014.06.05
*/
bool CHSROption::IsViewLineDisplayOn() const
{
	return m_oCompProp[LAYER_VIEW].m_isOn;
}

/**	\brief	The CHSROption::IsSquareTypeElbow function


	\return	bool	
*/
bool CHSROption::IsSquareTypeElbow()
{
	return !m_optSWandTHDElbow.Round;
}

/**	\brief	The CHSROption::IsRoundTypeElbow function


	\return	bool	
*/
bool CHSROption::IsRoundTypeElbow()
{
	return m_optSWandTHDElbow.Round;
}

/**	
	@brief	The CHSROption::IsSWElbowCenterlineOn function
	@return	bool	
*/
bool CHSROption::IsSWElbowCenterlineOn()
{
	return m_oCompProp[LAYER_ELBOW_CEN].m_isOn;
}

/**	\brief	The CHSROption::IsSWElbowCrosslineOn function


	\return	bool	
*/
bool CHSROption::IsSWElbowCrosslineOn()
{
	return m_optSWandTHDElbow.CrossOn;
}

double CHSROption::GetSWElbowDistA()
{
	return m_optSWandTHDElbow.DistA;
}

double CHSROption::GetSWElbowDistB()
{
	return m_optSWandTHDElbow.DistB;
}

double CHSROption::GetSWElbowAngle() const
{
	return m_optSWandTHDElbow.Angle;
}

double CHSROption::GetSWElbowMinSize()
{
	return m_optSWandTHDElbow.MinSize;
}

/**	\brief	The CHSROption::GetTeeDistA function


	\return	double	
*/
const double CHSROption::GetTeeDistA()
{
	return m_optTee.DistA;
}

/**	
	@brief	The CHSROption::IsTeeCenterlineOn function
	@return	bool	
*/
bool CHSROption::IsTeeCenterlineOn()
{
	return m_oCompProp[LAYER_TEE_CEN].m_isOn;
}

/**	
	@brief	The CHSROption::IsPipeCenterlineOn function
	@return	bool	
*/
bool CHSROption::IsPipeCenterlineOn()
{
	return m_oCompProp[LAYER_PIPE_CEN].m_isOn;
}

//	parameter	:
//	description	: extract view boundaries from dlist.
//	remarks		:
//	returns		:
void CHSROption::ExtractViewBoundary(DList *pDList)
{
	assert(pDList && "pDList is NULL");

	if(pDList)
	{
		DList* pList = pDList;
		
		CHSRApp* pHSRApp=CHSRApp::GetInstance();
		if((df_list == pList->val.type) && pList->val.data.list)
		{
			for(pList = pDList;pList;pList = pList->next)
			{
				ExtractViewBoundary(pList->val.data.list);
			}
		}
		else if((df_point == pList->val.type) && pList->val.data.point)
		{
			for(DList* itr = pList;itr && (df_point == itr->val.type) && itr->val.data.point;)
			{
				pHSRApp->m_pViewBoundary->m_aryViewBoundary[pHSRApp->m_pViewBoundary->m_nViewBoundaryEdge].ptStart.x = itr->val.data.point->x;
				pHSRApp->m_pViewBoundary->m_aryViewBoundary[pHSRApp->m_pViewBoundary->m_nViewBoundaryEdge].ptStart.y = itr->val.data.point->y;
				pHSRApp->m_pViewBoundary->m_aryViewBoundary[pHSRApp->m_pViewBoundary->m_nViewBoundaryEdge].ptStart.z = itr->val.data.point->z;
				itr = itr->next;
					
				if(itr && (df_point == itr->val.type) && itr->val.data.point)
				{
					pHSRApp->m_pViewBoundary->m_aryViewBoundary[pHSRApp->m_pViewBoundary->m_nViewBoundaryEdge].ptEnd.x = itr->val.data.point->x;
					pHSRApp->m_pViewBoundary->m_aryViewBoundary[pHSRApp->m_pViewBoundary->m_nViewBoundaryEdge].ptEnd.y = itr->val.data.point->y;
					pHSRApp->m_pViewBoundary->m_aryViewBoundary[pHSRApp->m_pViewBoundary->m_nViewBoundaryEdge].ptEnd.z = itr->val.data.point->z;
						
					(pHSRApp->m_pViewBoundary->m_nViewBoundaryEdge)++;
				}
				itr = itr->next;
			}
		}
	}
}

/**
**/
double CHSROption::GetWeldElbowMinSize() const
{
	return m_optWeldElbow.MinSize;
}

/**
	@brief	initialize member variables.
	@author	humkyung
*/
int CHSROption::Init()
{
	memset(&m_optWeldElbow ,'\0' , sizeof(m_optWeldElbow));
	memset(&m_optSWandTHDElbow , '\0' , sizeof(m_optSWandTHDElbow));

	return ERROR_SUCCESS;
}
