#include "StdAfx.h"
#include <Tokenizer.h>
#include <aDraw_View.h>
#include "SmartDrawAnnoEnv.h"
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoSpecialty.h"

CSmartDrawAnnoSpecialty::ENV::ENV(void)
{
	m_dEnclosureSizeA = m_dEnclosureSizeB = 0.0;
}

CSmartDrawAnnoSpecialty::ENV::~ENV(void)
{
}

/**
	@brief	parse specialty option
	@author	humkyung
	@date	2015.09.01
	@return
*/
int CSmartDrawAnnoSpecialty::ENV::Parse(const STRING_T& sIniFilePath)
{
	static const STRING_T sApp(_T("Specialty"));
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	if(GetPrivateProfileString(sApp.c_str() , _T("EnclosureType") ,NULL,szBuf,MAX_PATH , sIniFilePath.c_str()))
	{
		if(0 == STRICMP_T(szBuf , _T("Circle")))
		{
			m_iEnclosureType = CaDraw_EntTagShape::CIRCLE;
		}
		else if (0 == STRICMP_T(szBuf , _T("Diamond")))
		{ 
			m_iEnclosureType = CaDraw_EntTagShape::DIAMOND;
		}
		else if(0 == STRICMP_T(szBuf , _T("Rectangle")))
		{ 
			m_iEnclosureType = CaDraw_EntTagShape::RECTANGLE;
		}
		else if (0 == STRICMP_T(szBuf , _T("Ellipse")))
		{ 
			m_iEnclosureType = CaDraw_EntTagShape::ELLIPSE;
		}
		else if (0 == STRICMP_T(szBuf , _T("Hexagon")))
		{ 
			m_iEnclosureType = CaDraw_EntTagShape::HEXAGON;
		}
		else if (0 == STRICMP_T(szBuf , _T("Octagon")))
		{ 
			m_iEnclosureType = CaDraw_EntTagShape::OCTAGON;
		}
		else
		{
			m_iEnclosureType = CaDraw_EntTagShape::CIRCLE;
		}
	}else	m_iEnclosureType = CaDraw_EntTagShape::CIRCLE;

	if(GetPrivateProfileString(sApp.c_str() , _T("Enclosure_SizeA") , NULL , szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_dEnclosureSizeA = ATOF_T(szBuf);
	}
		
	if ((2 == m_iEnclosureType) || (4 == m_iEnclosureType) || (5 == m_iEnclosureType)) 
	{
		if(GetPrivateProfileString(sApp.c_str() , _T("Enclosure_SizeB") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
		{
			m_dEnclosureSizeB = ATOF_T(szBuf);
		}
	}else	m_dEnclosureSizeB = m_dEnclosureSizeA;

	if(GetPrivateProfileString(sApp.c_str() , _T("Enclosure_CenterLine") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_bEnclosureCenterLine = (0 == STRICMP_T(szBuf , _T("ON")));
	}else	m_bEnclosureCenterLine = false;

	if(GetPrivateProfileString(sApp.c_str() , _T("TextStyle") ,NULL,szBuf,MAX_PATH,sIniFilePath.c_str()))
	{
		m_oTextStyle.Parse(szBuf , sIniFilePath);
	}	

	m_sDelimiter = _T("-");
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Delimiter") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sDelimiter = szBuf;
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLabelProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLabelProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLabelProp.style = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Label_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLabelProp.weight = szBuf;
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Type") , _T("CLOSE") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sArrowType = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Arrow_Size") , _T("1.5") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_dArrowSize = ATOF_T(szBuf);
	}

	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.level = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.color = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.style = szBuf;
	}
	if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.weight = szBuf;
	}
	
	return ERROR_SUCCESS;
}

SMARTDRAW_TAG_IMPLEMENT_FUNC(CSmartDrawAnnoSpecialty , CaDraw_TagEntity , _T("Specialty"))

CSmartDrawAnnoSpecialty::CSmartDrawAnnoSpecialty(CaDraw_Entity* unnamed) : CaDraw_TagEntity(unnamed)
{
}

CSmartDrawAnnoSpecialty::~CSmartDrawAnnoSpecialty(void)
{
}

/**	
	@brief	annotate specialty tag in inside of view.
	@author	humkyung
	@date	2015.09.01
	@param	
	@return	
*/
int CSmartDrawAnnoSpecialty::Annotate()
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	CaDraw_View* pDrawView = entity()->GetView();
	CIsPoint3d ptOrigin;
	ptOrigin = pDrawView->ModelCoord2ViewCoord(entity()->origin());
	ptOrigin.z() = 0.f;

	list<CCellMap::SearchRes> SearchResList;

	const double dTextLength= env.m_specialty.m_oTextStyle.GetTextLength(entity()->name());
	const double dItemWidth = env.m_specialty.m_dEnclosureSizeB;
	const double dItemHeight= env.m_specialty.m_dEnclosureSizeA;
	const double dItemCellHeight   = dItemHeight * CSmartDrawAnnoModule::m_pCellMap->scale();		
	const double dItemCellLength   = dItemWidth  * CSmartDrawAnnoModule::m_pCellMap->scale();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// SEARCH
	const double nSearchCellLength = dItemCellLength * 8;	/// this value comes from experience.
	const double nSearchCellHeight = dItemCellLength * 8;	/// this value comes from experience.

	CCellMap::SIZE_T sizSearch , sizItem;
	sizSearch.width = nSearchCellLength;
	sizSearch.height= nSearchCellHeight;
	sizItem.width = dItemCellLength;
	sizItem.height= dItemCellHeight;

	CCellMap::CellConversionAxis cAxis;
	cAxis.xAxis.Set(1 , 0);
	cAxis.yAxis.Set(0 , 1);

	CCellMap::CellIndex tagOrigin(CSmartDrawAnnoModule::m_pCellMap->PointToIndex(CIsPoint2d(ptOrigin.x() , ptOrigin.y())));
	CCellMap::CellIndex cOrigin(tagOrigin);
	cOrigin.x -= nSearchCellLength * 0.5;
	cOrigin.y -= nSearchCellHeight * 0.5;

	CCellMap::SearchRes res;
	STRNCPY_T(res.szString , entity()->name() , 255);
	bool bFound = CSmartDrawAnnoModule::m_pCellMap->FindBestLoc(res , cOrigin , tagOrigin , 
		sizSearch , sizItem , cAxis , 0xFF);
	if(bFound)
	{
		res.ptTagOrigin = CIsPoint2d(ptOrigin.x() , ptOrigin.y());

		res.coords[0] = res.ptLoc;
		res.coords[1] = res.coords[0] + (res.cAxis.xAxis * dItemWidth);
		res.coords[2] = res.coords[1] + (res.cAxis.yAxis * dItemHeight);
		res.coords[3] = res.coords[2] - (res.cAxis.xAxis * dItemWidth);
		SearchResList.push_back(res);
	}

	if(!SearchResList.empty())
	{
		stable_sort(SearchResList.begin(), SearchResList.end(), CCellMap::fnResSort);  /// sorting

		CCellMap::SearchRes res = SearchResList.front();

		CSmartDrawAnnoModule::m_pCellMap->UpdateRectOf(res.cIndex , ceil(dItemCellLength) , ceil(dItemCellHeight) , res.cAxis , 155);

		if((res.radian > PI*0.5) && (res.radian < PI*1.5))
		{
			/// 글자가 뒤집혀 보이므로 보정해 준다.
			res.ptLoc += (res.cAxis.xAxis * dTextLength) + (res.cAxis.yAxis * env.m_specialty.m_oTextStyle.height());
			res.cAxis.xAxis = -res.cAxis.xAxis;
			res.cAxis.yAxis = -res.cAxis.yAxis;
			res.radian = res.cAxis.xAxis.angle();
		}

		m_bTagged = true;
		m_oEntTagShape.m_SearchRes = res;
		m_oEntTagShape.m_pTagStringList->push_back(res.szString);
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	write specialty tag
	@author	humkyung
	@date	2012.0.24
	@param	
	@return	
*/
int CSmartDrawAnnoSpecialty::Write(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	vector<STRING_T> oResult;
	if(!env.m_specialty.m_sDelimiter.empty())
	{
		CTokenizer<CIsFromString>::Tokenize(oResult , (*m_oEntTagShape.m_pTagStringList)[0] , CIsFromString(env.m_specialty.m_sDelimiter));
	}
	else
	{
		oResult.push_back((*m_oEntTagShape.m_pTagStringList)[0]);
	}
	
	ofile << _T("<GROUP>|0,VALV,Pat 04") << std::endl;

	m_oEntTagShape.Write(ofile , env.m_specialty.m_iEnclosureType , env.m_specialty.m_oLeaderProp , dDrawingScale);
	
	CIsPoint2d center((m_oEntTagShape.m_SearchRes.coords[0] + m_oEntTagShape.m_SearchRes.coords[2]) * 0.5);
	ofile << _T("<LEADER>|");
	ofile << (m_oEntTagShape.m_SearchRes.ptTagOrigin*dDrawingScale) << _T(",");
	if(CaDraw_EntTagShape::CIRCLE == env.m_specialty.m_iEnclosureType)
	{
		CIsVect2d dir(m_oEntTagShape.m_SearchRes.ptTagOrigin - center);
		dir.Normalize();
		CIsPoint2d tmp(center + dir*(env.m_specialty.m_dEnclosureSizeA*0.5));
		ofile << (tmp*dDrawingScale) << _T("|");
	}
	else
	{
		ofile << (center*dDrawingScale) << _T("|");
	}
	ofile << env.m_specialty.m_oLeaderProp.level << _T(",") << env.m_specialty.m_oLeaderProp.color << _T(",");
	ofile << env.m_specialty.m_sArrowType << _T(",") << env.m_specialty.m_dArrowSize*dDrawingScale << _T("," ) << env.m_specialty.m_dArrowSize/3.0*dDrawingScale << std::endl;

	const double nStep = env.m_specialty.m_dEnclosureSizeA / oResult.size();
	int nIndex = 0;
	for(vector<STRING_T>::iterator itr = oResult.begin();itr != oResult.end();++itr,++nIndex)
	{
		ofile << _T("<TEXT>|");
		ofile << center.x()*dDrawingScale << _T(",");
		ofile << (m_oEntTagShape.m_SearchRes.coords[0].y() + env.m_specialty.m_dEnclosureSizeA - nIndex*nStep - nStep*0.5)*dDrawingScale << _T(",0,");
		ofile << env.m_specialty.m_oTextStyle.height()*dDrawingScale << _T(",") << (env.m_specialty.m_oTextStyle.height()*env.m_specialty.m_oTextStyle.GetWidthFactor())*dDrawingScale << _T(",");
		ofile << RAD2DEG(m_oEntTagShape.m_SearchRes.radian) << _T(",");
		ofile << (*itr) << _T(",Center Center,0|");
		ofile << env.m_specialty.m_oLabelProp.level << _T(",") << env.m_specialty.m_oLabelProp.color << _T(",");
		ofile << env.m_specialty.m_oTextStyle << std::endl;
	}

	ofile << _T("</GROUP>") << std::endl;

	return ERROR_SUCCESS;
}

/**	
	@brief	create tag item of tag entity.
	@author	humkyung
	@date	2015.09.01
	@param	
	@return	
*/
CaDraw_TagItem* CSmartDrawAnnoSpecialty::CreateTagObjects(list<CaDraw_TagItem*>& TagItemList , CSmartDrawAnnoView *pAnnoView)
{
	TagItemList.clear();
	return NULL;
}

/**	
	@brief	
	@author	humkyung
	@date	2015.09.01
	@param	
	@return	CaDraw_TagItem*	
*/
CaDraw_TagItem* CSmartDrawAnnoSpecialty::OnCreateTagObject(CSmartDrawAnnoView *pAnnoView , const CIsPoint3d& ptOrigin,const CIsVect3d& vec,const DITEM_E& enmType)
{
	CaDraw_TagItem* pTagItem=NULL;
	
	return pTagItem;
}