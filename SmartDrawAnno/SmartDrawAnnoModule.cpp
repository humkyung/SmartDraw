#include "StdAfx.h"
#include <assert.h>
#include <tchar.h>
#include <IsTools.h>
#include <IsString.h>
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoModule.h"

#include <map>

double CSmartDrawAnnoModule::UNIT = 1.f;

map<STRING_T , CVFTFile*> CSmartDrawAnnoModule::m_VFTFontMap;
boost::object_pool<CaDraw_TagItem>* CSmartDrawAnnoModule::m_pCaDraw_TagItemPool=NULL;
boost::object_pool<CaDraw_TagPos>*  CSmartDrawAnnoModule::m_pCaDraw_TagPosPool=NULL;
CCellMap* CSmartDrawAnnoModule::m_pCellMap = NULL;

CSmartDrawAnnoModule::CSmartDrawAnnoModule(void)
{
	if(NULL == CSmartDrawAnnoModule::m_pCaDraw_TagItemPool) CSmartDrawAnnoModule::m_pCaDraw_TagItemPool = new boost::object_pool<CaDraw_TagItem>;
	if(NULL == CSmartDrawAnnoModule::m_pCaDraw_TagPosPool)  CSmartDrawAnnoModule::m_pCaDraw_TagPosPool  = new boost::object_pool<CaDraw_TagPos>;
}

CSmartDrawAnnoModule::~CSmartDrawAnnoModule(void)
{
	try
	{
		for(map<STRING_T,CVFTFile*>::iterator itr = m_VFTFontMap.begin();itr != m_VFTFontMap.end();++itr)
		{
			delete (itr->second);
		}

		if(CSmartDrawAnnoModule::m_pCellMap) SAFE_DELETE(CSmartDrawAnnoModule::m_pCellMap);
	}
	catch(...)
	{
	}
}

CSmartDrawAnnoModule& CSmartDrawAnnoModule::GetInstance(void)
{
	static CSmartDrawAnnoModule __INSTANCE__;

	return __INSTANCE__;
}

/**	\brief	The CDraft::new_ditem_t function


	\return	DITEM_T*	
*/
CaDraw_TagItem* CSmartDrawAnnoModule::new_titem_t()
{
	CaDraw_TagItem* pRet=NULL;

	if(NULL != CSmartDrawAnnoModule::m_pCaDraw_TagItemPool)
	{
		pRet = CSmartDrawAnnoModule::m_pCaDraw_TagItemPool->construct();
	}

	return pRet;
}

/**	\brief	The CDraft::new_tag_pos function


	\return	CaDraw_TagPos*	
*/
CaDraw_TagPos* CSmartDrawAnnoModule::new_tag_pos()
{
	CaDraw_TagPos* pRet=NULL;

	if(NULL != CSmartDrawAnnoModule::m_pCaDraw_TagPosPool)
	{
		pRet = CSmartDrawAnnoModule::m_pCaDraw_TagPosPool->construct();
	}

	return pRet;
}

int CSmartDrawAnnoModule::Clear(void)
{
	//////////////////////////////////////////////////////////////////////////
	/// clear object pool
	if(NULL != CSmartDrawAnnoModule::m_pCaDraw_TagItemPool) delete CSmartDrawAnnoModule::m_pCaDraw_TagItemPool;
	CSmartDrawAnnoModule::m_pCaDraw_TagItemPool = NULL;
	if(NULL != CSmartDrawAnnoModule::m_pCaDraw_TagPosPool)  delete CSmartDrawAnnoModule::m_pCaDraw_TagPosPool;
	CSmartDrawAnnoModule::m_pCaDraw_TagPosPool = NULL;
	//////////////////////////////////////////////////////////////////////////

	return ERROR_SUCCESS;
}

/**	
	@brief	The CDraft::UNIT2STR function

	@author	humkyung

	@date	????.??.??

	@param	unit	a parameter of type const double &

	@return	const char*	
*/
const TCHAR* CSmartDrawAnnoModule::UNIT2STR(const double &unit,const UINT_TYPE_E sub_unit )
{
	static TCHAR szBuf[MAX_PATH]={0,};
	
	ZeroMemory(szBuf,sizeof(TCHAR)*MAX_PATH);
	if(CaDraw_TagEnv::INCH_FEET == CaDraw_TagEnv::INPUT_UNIT)
	{
		if(CaDraw_TagEnv::INCH_FEET == CaDraw_TagEnv::OUTPUT_UNIT)
		{
			return CSmartDrawAnnoModule::INCH2STR(unit);
		}else	return szBuf;
	}
	else
	{
		if(CaDraw_TagEnv::INCH_FEET == CaDraw_TagEnv::OUTPUT_UNIT)
		{
			return CSmartDrawAnnoModule::METRIC2INCH(unit);
		}
		else
		{
			const int iunit=(int)(SAFE_ROUND(unit,1));
			SPRINTF_T(szBuf,_T("%d"),iunit);
			
			CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
			if(CaDraw_TagEnv::XY_COORD == env.m_eAnnotateCoord)
			{
				if(M == sub_unit)
				{
					const int len=STRLEN_T(szBuf);
					if(iunit > 0)
					{
						if(len > 3)
						{
							memmove(szBuf + len - 2,szBuf + len - 3,sizeof(TCHAR)*3);
							szBuf[len - 3] = '.';
						}
						else
						{
							memmove(szBuf + 2,szBuf,sizeof(TCHAR)*len);
							szBuf[0] = '0';szBuf[1] = '.';
						}
					}
					else
					{
						if(len > 4)
						{
							memmove(szBuf + len - 2,szBuf + len - 3,sizeof(TCHAR)*3);
							szBuf[len - 3] = '.';
						}
						else
						{
							memmove(szBuf + 3,szBuf + 1,sizeof(TCHAR)*(len - 1));
							szBuf[1] = '0';szBuf[2] = '.';
						}
					}
				}
			}

			return szBuf;
		}
	}
	
	return szBuf;
}

/**	
	@brief	The M_FRACTION function
	@param	n1	a parameter of type const int
	@param	n2	a parameter of type const int
	@return	string	
*/
const TCHAR* CSmartDrawAnnoModule::M_FRACTION(int n1,int n2)
{
	static TCHAR _szBuf[MAX_PATH]={0,};
	while ((n1 % 2)==0 && (n2 % 2)==0 )
	{ 
		n1/=2; n2/=2;
	}
	
	SPRINTF_T(_szBuf, _T("%d/%d\"") ,n1,n2);
	
	return _szBuf;
} 

/**	
	@brief	The CDraft::INCH2STR function
	@param	unit	a parameter of type const double &
	@return	const char*	
*/
const TCHAR* CSmartDrawAnnoModule::INCH2STR(const double &unit)
{
	const static double coef_mm_to_inch = 1;
	static TCHAR res[MAX_PATH]={0,};
	
	TCHAR inch[MAX_PATH]={0,};
	TCHAR FRACTION_REDUCE[MAX_PATH]={0,};
	
	ZeroMemory(res,sizeof(TCHAR)*MAX_PATH);
	double mm = fabs(unit);
	int coef_feet_inch = 12;
	int denominator = 32;
	double inch_value_and_fraction = mm * coef_mm_to_inch;
	int inch_value_integer = int(floor(inch_value_and_fraction));
	int FEET_VALUE_tmp = floor(inch_value_and_fraction / coef_feet_inch);
	int INCH_VALUE_tmp = SAFE_ROUND(inch_value_integer - (coef_feet_inch * FEET_VALUE_tmp), 1);
	double decimal_point = (inch_value_and_fraction - floor(inch_value_and_fraction));
	int numerator = SAFE_ROUND(decimal_point * denominator, 1);
	int INCH_VALUE_tmp_1 = (numerator == 32.0) ? (INCH_VALUE_tmp + 1) : INCH_VALUE_tmp;
	int INCH_VALUE = (INCH_VALUE_tmp_1 == 12) ? 0 : INCH_VALUE_tmp_1;
	int FEET_VALUE = (INCH_VALUE_tmp_1 == 12) ? (FEET_VALUE_tmp + 1) : FEET_VALUE_tmp;
	if ((numerator == 0) || (numerator == 32)) 
		FRACTION_REDUCE[0] = '\0';
	else	STRCPY_T(FRACTION_REDUCE,M_FRACTION(numerator,denominator));
	if ('\0' != FRACTION_REDUCE[0])
	{
		if ((FEET_VALUE == 0) && (INCH_VALUE == 0))
			STRCPY_T(inch,FRACTION_REDUCE);
		if ((FEET_VALUE >= 1) && (INCH_VALUE == 0)) 
			SPRINTF_T(inch, _T("%d\'%s") ,FEET_VALUE,FRACTION_REDUCE);
		if ((FEET_VALUE == 0) && (INCH_VALUE >= 1)) 
			SPRINTF_T(inch,_T("%d.%s"),INCH_VALUE,FRACTION_REDUCE);
		if ((FEET_VALUE >= 1) && (INCH_VALUE >= 1)) 
			SPRINTF_T(inch,_T("%d\'-%d.%s"),FEET_VALUE,INCH_VALUE,FRACTION_REDUCE);
	}
	else
	{
		if ((FEET_VALUE == 0) && (INCH_VALUE == 0)) 
			SPRINTF_T(inch, _T("%d\"") ,0);
		if ((FEET_VALUE >= 1) && (INCH_VALUE == 0))
			SPRINTF_T(inch, _T("%d\'") , FEET_VALUE);
		if ((FEET_VALUE == 0) && (INCH_VALUE >= 1)) 
			SPRINTF_T(inch, _T("%d\"") ,INCH_VALUE);
		if ((FEET_VALUE == 0) && (INCH_VALUE <= -1)) 
			SPRINTF_T(inch, _T("-%d\"") ,INCH_VALUE);
		if ((FEET_VALUE >= 1) && (INCH_VALUE >= 1)) 
			SPRINTF_T(inch,_T("%d\'-%d\""),FEET_VALUE,INCH_VALUE);
	}
	
	if (unit < 0)
		SPRINTF_T(res, _T("- %s") ,inch);
	else	STRCPY_T(res,inch);
	
	return res;
}

/**	
	@brief	The CDraft::METRIC2INCH function
	@param	mm	a parameter of type double
	@return	CString	
*/
const TCHAR* CSmartDrawAnnoModule::METRIC2INCH(double in_mm)
{
	const static double coef_mm_to_inch = 0.03937007874015748;
	static TCHAR res[MAX_PATH]={0,};

	double mm;
	TCHAR inch[MAX_PATH]={0,};
	TCHAR FRACTION_REDUCE[MAX_PATH]={0,};
	
	ZeroMemory(res,sizeof(TCHAR)*MAX_PATH);
	mm = fabs(in_mm);
	int coef_feet_inch = 12;
	int denominator = 32;
	double inch_value_and_fraction = mm * coef_mm_to_inch;
	int inch_value_integer = floor(inch_value_and_fraction);
	int FEET_VALUE_tmp = floor(inch_value_and_fraction / coef_feet_inch);
	int INCH_VALUE_tmp = SAFE_ROUND(inch_value_integer - (coef_feet_inch * FEET_VALUE_tmp), 1);
	double decimal_point = (inch_value_and_fraction - floor(inch_value_and_fraction));
	int numerator = SAFE_ROUND(decimal_point * denominator, 1);
	int INCH_VALUE_tmp_1 = (numerator == 32.0) ? (INCH_VALUE_tmp + 1) : INCH_VALUE_tmp;
	int INCH_VALUE = (INCH_VALUE_tmp_1 == 12) ? 0 : INCH_VALUE_tmp_1;
	int FEET_VALUE = (INCH_VALUE_tmp_1 == 12) ? (FEET_VALUE_tmp + 1) : FEET_VALUE_tmp;
	if ((numerator == 0) || (numerator == 32)) 
		FRACTION_REDUCE[0] = '\0';
	else	STRCPY_T(FRACTION_REDUCE,M_FRACTION(numerator,denominator));
	if ('\0' != FRACTION_REDUCE[0])
	{
		if ((FEET_VALUE == 0) && (INCH_VALUE == 0))
			STRCPY_T(inch,FRACTION_REDUCE);
		if ((FEET_VALUE >= 1) && (INCH_VALUE == 0)) 
			SPRINTF_T(inch, _T("%d\'%s") ,FEET_VALUE,FRACTION_REDUCE);
		if ((FEET_VALUE == 0) && (INCH_VALUE >= 1)) 
			SPRINTF_T(inch, _T("%d.%s") ,INCH_VALUE,FRACTION_REDUCE);
		if ((FEET_VALUE >= 1) && (INCH_VALUE >= 1)) 
			SPRINTF_T(inch, _T("%d\'-%d.%s") ,FEET_VALUE,INCH_VALUE,FRACTION_REDUCE);
	}
	else
	{
		if ((FEET_VALUE == 0) && (INCH_VALUE == 0)) 
			SPRINTF_T(inch, _T("%d\"") ,0);
		if ((FEET_VALUE >= 1) && (INCH_VALUE == 0))
			SPRINTF_T(inch, _T("%d\'") , FEET_VALUE);
		if ((FEET_VALUE == 0) && (INCH_VALUE >= 1)) 
			SPRINTF_T(inch, _T("%d\"") ,INCH_VALUE);
		if ((FEET_VALUE == 0) && (INCH_VALUE <= -1)) 
			SPRINTF_T(inch, _T("-%d\"") ,INCH_VALUE);
		if ((FEET_VALUE >= 1) && (INCH_VALUE >= 1)) 
			SPRINTF_T(inch, _T("%d\'-%d\"") ,FEET_VALUE,INCH_VALUE);
	}
	
	if (in_mm < 0)
		SPRINTF_T(res, _T("- %s") ,inch);
	else	STRCPY_T(res,inch);
	
	return res;
}

/// drawing items
#include <aDraw_Pipe.h>
#include <aDraw_Equipment.h>
#include <aDraw_Structure.h>
#include <aDraw_CableTray.h>
#include <aDraw_Platform.h>
#include <aDraw_Valve.h>
/// up to here

/// tag entities
#include "SmartDrawAnnoPipe.h"
#include "SmartDrawAnnoEqui.h"
#include "SmartDrawAnnoStru.h"
#include "SmartDrawAnnoElbow.h"
#include "SmartDrawAnnoNozzle.h"
#include "SmartDrawAnnoReducer.h"
#include "SmartDrawAnnoInstrument.h"
#include "SmartDrawAnnoSupport.h"
#include "SmartDrawAnnoCableTray.h"
#include "SmartDrawAnnoPlatform.h"
#include "SmartDrawAnnoValve.h"
#include "SmartDrawAnnoSpecialty.h"
/// up to here

/**	
	@brief	create tag item consistent to entity
	@author	humkyung
	@date	2010.06.02
	@param	CaDraw_Entity*
	@return	CaDraw_TagEntity*	
*/
CaDraw_TagEntity* CSmartDrawAnnoModule::CreateTagEntityOf(CaDraw_Entity* pEnt)
{
	assert(pEnt && "pEnt is NULL");
	CaDraw_TagEntity* pTagEnt = NULL;

	if(pEnt)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		if(pEnt->IsKindOf(CaDraw_Pipe::TypeString()) && (env.m_generation.IsEnable(CaDraw_TagPipe::TypeString(),pEnt)))
		{
			pTagEnt = CaDraw_TagPipe::CreateInstance(pEnt);
		}
		else if(pEnt->IsKindOf(CaDraw_Equipment::TypeString()) && (env.m_generation.IsEnable(CaDraw_TagEqui::TypeString(),pEnt)))
		{
			pTagEnt = CaDraw_TagEqui::CreateInstance(pEnt);
		}
		else if(pEnt->IsKindOf(CaDraw_Structure::TypeString()) && (env.m_generation.IsEnable(CaDraw_TagStru::TypeString(),pEnt)))
		{
			pTagEnt = CaDraw_TagStru::CreateInstance(pEnt);
		}
		else if(pEnt->IsKindOf(CaDraw_Elbow::TypeString()))
		{
			pTagEnt = CaDraw_TagElbow::CreateInstance(pEnt);
		}
		else if(pEnt->IsKindOf(CaDraw_Nozzle::TypeString()) && (env.m_generation.IsEnable(CaDraw_TagNozzle::TypeString(),pEnt)))
		{
			pTagEnt = CaDraw_TagNozzle::CreateInstance(pEnt);
		}
		else if(pEnt->IsKindOf(CaDraw_Reducer::TypeString()) && (env.m_generation.IsEnable(CaDraw_TagReducer::TypeString(),pEnt)))
		{
			pTagEnt = CaDraw_TagReducer::CreateInstance(pEnt);
		}
		else if(pEnt->IsKindOf(CaDraw_Instrument::TypeString()) && (env.m_generation.IsEnable(CaDraw_TagInstrument::TypeString(),pEnt)))
		{
			pTagEnt = CaDraw_TagInstrument::CreateInstance(pEnt);
		}
		else if(pEnt->IsKindOf(CaDraw_Support::TypeString()) && (env.m_generation.IsEnable(CaDraw_Support::TypeString(),pEnt))) 
		{
			pTagEnt = CaDraw_TagSupport::CreateInstance(pEnt);
		}
		/// 2012.02.21 added by humkyung
		else if(pEnt->IsKindOf(CaDraw_CableTray::TypeString()))
		{
			pTagEnt = CaDraw_TagCableTray::CreateInstance(pEnt);
			
			CaDraw_TagCableTray* pCableTray = static_cast<CaDraw_TagCableTray*>(pTagEnt);
			pCableTray->Parse();
		}
		/// up to here
		else if(pEnt->IsKindOf(CaDraw_Platform::TypeString()))
		{
			pTagEnt = CSmartDrawAnnoPlatform::CreateInstance(pEnt);
			
			CSmartDrawAnnoPlatform* pAnnoPlatform = static_cast<CSmartDrawAnnoPlatform*>(pTagEnt);
			pAnnoPlatform->Parse();
		}
		/// 2012.03.24 added by humkyung
		else if(pEnt->IsKindOf(CaDraw_Valve::TypeString()) && (env.m_generation.IsEnable(CSmartDrawAnnoValve::TypeString(),pEnt)))
		{
			pTagEnt = CSmartDrawAnnoValve::CreateInstance(pEnt);
		}
		/// up to here
		/// 2015.09.01 added by humkyung
		else if(pEnt->IsKindOf(CaDraw_Specialty::TypeString()) && (env.m_generation.IsEnable(CSmartDrawAnnoSpecialty::TypeString(),pEnt)))
		{
			pTagEnt = CSmartDrawAnnoSpecialty::CreateInstance(pEnt);
		}
		/// up to here
	}

	return pTagEnt;
}

/******************************************************************************
    @brief
    @author     humkyung
    @date       2012-03-02
    @class      CSmartDrawAnnoModule
    @function   CreateDrawingOf
    @return     CDrawing*
    @param      const           STRING_T&
    @param      sDrawingType
******************************************************************************/
CDrawing* CSmartDrawAnnoModule::CreateDrawingOf(const STRING_T& sDrawingType)
{
	CDrawing* pDrawing = NULL;
	if(sDrawingType == _T("OpeningHole"))
	{
		pDrawing = new CDrawing;
	}
	else if(_T("CableTray") == sDrawingType)
	{
		pDrawing = new CDrawing;
	}
	else
	{
		pDrawing = new CDrawing;
	}

	return pDrawing;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-25
    @function   CSVString
    @return     STRING_T
    @param      const   STRING_T&
    @param      str
    @brief
******************************************************************************/
STRING_T CSmartDrawAnnoModule::CSVString(const STRING_T& str)
{
	STRING_T aLine(str);
	STRING_T::size_type at = aLine.find(_T("\""));
	if(STRING_T::npos != at)
	{
		IsString::ReplaceOf(aLine , _T("\"") , _T("\"\""));
		aLine = _T("\"") + aLine + _T("\"");
	}

	return aLine;
}