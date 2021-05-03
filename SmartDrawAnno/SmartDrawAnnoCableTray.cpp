#include "StdAfx.h"
#include <assert.h>
#include <tchar.h>
#include <aDraw_CableTray.h>
#include <aDraw_View.h>
#include "SmartDrawAnnoView.h"
#include "SmartDrawAnnoEnv.h"
#include "ExtDivision.h"
#include "SmartDrawAnnoCableTray.h"

/******************************************************************************
    @author     humkyung
    @date       2012-02-23
    @class      CaDraw_TagCableTray
    @function   ENV::Parse
    @return     int
    @param      const           STRING_T&
    @param      sIniFilePath
    @brief
******************************************************************************/
int CaDraw_TagCableTray::ENV::Parse(const STRING_T& sIniFilePath)
{
	static const STRING_T sApp(_T("ANNOTATION_OPTIONS"));
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	m_iRowCount = ::GetPrivateProfileInt(sApp.c_str() , _T("CableT_Row_Count") , 3 , sIniFilePath.c_str());
	m_dRowHeight= 3.5;
	if(::GetPrivateProfileString(sApp.c_str() , _T("CableT_Row_Height") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_dRowHeight = ATOF_T(szBuf);
	}

	m_sCellFilePath = _T("NULL");
	if(::GetPrivateProfileString(sApp.c_str() , _T("CableT_Cell_Library") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sCellFilePath = szBuf;
	}
	sText_Color = _T("0");
	if(::GetPrivateProfileString(sApp.c_str() , _T("CableT_TextColor") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		sText_Color = szBuf;
	}

	m_sFittingTag_OnOff = _T("ON");
	if(GetPrivateProfileString(sApp.c_str() , _T("CableT_Fitting_Tag_OnOff") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sFittingTag_OnOff = szBuf;
	}

	m_sUpDownMark_OnOff = _T("ON");
	if(::GetPrivateProfileString(sApp.c_str() , _T("CableT_UpDown_Mark_OnOff") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sUpDownMark_OnOff = szBuf;
	}
		
	/// for Electric Cable Tray
	m_sElecCableT_Label = _T("ELEC CABLE TRAY");
	if(::GetPrivateProfileString(sApp.c_str() , _T("Elec_Cable_Tray_Label") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sElecCableT_Label = szBuf;
	}
	if(::GetPrivateProfileString(sApp.c_str() , _T("Elec_Cable_Tray_EL_Prefix") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sElecCableT_EL_Prefix = szBuf;
	}
	if(::GetPrivateProfileString(sApp.c_str() , _T("Elec_Cable_Tray_Tag_OnOff") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sElecCableT_Tag_OnOff = szBuf;
	}
	m_sElecCableT_OutLine_OnOff = _T("OFF");
	if(::GetPrivateProfileString(sApp.c_str() , _T("Elec_CableT_Outline_OnOff") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sElecCableT_OutLine_OnOff = szBuf;
	}
	m_uiElecCableT_OutLine_Weight = ::GetPrivateProfileInt(sApp.c_str() , _T("Elec_CableT_Outline_Weight") , 0 , sIniFilePath.c_str());

	if(GetPrivateProfileString(sApp.c_str() , _T("Elec_CableT_FileName_Rule") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sElecCableT_FileName_Rule = szBuf;
	}

	m_sElecCableT_CellName = _T("NULL");
	if(::GetPrivateProfileString(sApp.c_str() , _T("Elec_CableT_CellName") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sElecCableT_CellName = szBuf;
	}
	m_sElecCableT_HatchColor = _T("4");
	if(::GetPrivateProfileString(sApp.c_str() , _T("Elec_CableT_HatchColor") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sElecCableT_HatchColor = szBuf;
	}

	/// for Inst. Cable Tray
	m_sInstCableT_Label = _T("INST CABLE TRAY");
	if(::GetPrivateProfileString(sApp.c_str() , _T("Inst_Cable_Tray_Label") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sInstCableT_Label = szBuf;
	}
	if(::GetPrivateProfileString(sApp.c_str() , _T("Inst_Cable_Tray_EL_Prefix") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sInstCableT_EL_Prefix = szBuf;
	}
	if(::GetPrivateProfileString(sApp.c_str() , _T("Inst_Cable_Tray_Tag_OnOff") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sInstCableT_Tag_OnOff = szBuf;
	}
	m_sInstCableT_OutLine_OnOff = _T("OFF");
	if(::GetPrivateProfileString(sApp.c_str() , _T("Inst_CableT_Outline_OnOff") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sInstCableT_OutLine_OnOff = szBuf;
	}
	m_uiInstCableT_OutLine_Weight = ::GetPrivateProfileInt(sApp.c_str() , _T("Inst_CableT_Outline_Weight") , 0 , sIniFilePath.c_str());

	if(GetPrivateProfileString(sApp.c_str() , _T("Inst_CableT_FileName_Rule") , NULL , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sInstCableT_FileName_Rule = szBuf;
	}

	m_sInstCableT_CellName = _T("NULL");
	if(::GetPrivateProfileString(sApp.c_str() , _T("Inst_CableT_CellName") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sInstCableT_CellName = szBuf;
	}
	m_sInstCableT_HatchColor = _T("4");
	if(::GetPrivateProfileString(sApp.c_str() , _T("Inst_CableT_HatchColor") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sInstCableT_HatchColor = szBuf;
	}

	if(::GetPrivateProfileString(sApp.c_str() , _T("Cable_Tray_TextStyle") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLabelTextStyle.Parse(szBuf , sIniFilePath);	
	}
	
	if(::GetPrivateProfileString(sApp.c_str() , _T("Cable_Tray_ArrowType") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sArrowType = szBuf;
	}
	
	return ERROR_SUCCESS;
}

CaDraw_TagCableTray::CSmartDrawAnnoCableTrayTagItem::CSmartDrawAnnoCableTrayTagItem(const double& nElevation) : m_nElevation(nElevation)
{
	nLevel = DIM_LEVEL_0;
}

CaDraw_TagCableTray::CSmartDrawAnnoCableTrayTagItem::~CSmartDrawAnnoCableTrayTagItem()
{

}

/******************************************************************************
    @author     humkyung
    @date       2012-02-27
    @function   OnRegisterToDivision
    @return     void
    @param      list<CExtDivision*>&    oDivisionList
    @brief
******************************************************************************/
void CaDraw_TagCableTray::CSmartDrawAnnoCableTrayTagItem::OnRegisterToDivision(list<CExtDivision*>& oDivisionList)
{
	bool bStore=false;
	for(list<CExtDivision*>::iterator itr = oDivisionList.begin();itr != oDivisionList.end();++itr)
	{
		const double cross = this->vecDir.dx()*(*itr)->m_vecTagDir.dy() - (this)->vecDir.dy()*(*itr)->m_vecTagDir.dx();
		CExtDivision* pDivision = (*itr);
		if(fabs(cross) < 0.001)
		{ /// test is same dir?
			CIsPoint3d ptOrigin = (this)->m_ptOrigin;
			if((*itr)->IsInDivision(ptOrigin , (this)->vecDir , 1))
			{
				(*itr)->Register(this);
				
				bStore = true;
			}
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-27
    @class      CaDraw_TagCableTray
    @function   CSmartDrawAnnoCableTrayTagItem::OnCreateTagPos
    @return     TAGPOS*
    @param      vector<CaDraw_TagPos*>& oTagPosList
    @param      CExtDivision            *pDivision
    @brief
******************************************************************************/
int CaDraw_TagCableTray::CSmartDrawAnnoCableTrayTagItem::OnCreateTagPos(vector<CaDraw_TagPos*>& oTagPosList , CExtDivision *pDivision)
{
	assert(pDivision && "pDivision is NULL");

	if(NULL != pDivision)
	{
		CaDraw_TagCableTray::CSmartDrawAnnoCableTrayTagPos* pRet = new CaDraw_TagCableTray::CSmartDrawAnnoCableTrayTagPos(m_nElevation);
		if(NULL != pRet)
		{
			CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
			if(pDivision->IsVertical())
			{
				pRet->m_ptStart.SetX(pDivision->GetBaseLine().start().x());
				pRet->m_ptStart.SetY(m_ptOrigin.y());
				pRet->rect.width	= 0;
				pRet->nOverlap		= 0;
				pRet->type		= SINGLE_TAG;
				pRet->pTITEM		= this;
			
				pRet->rect.height = env.m_stru.m_dRadius*2;			
				pRet->m_ptEnd = pRet->m_ptStart;

				//! 2010.10.20 --
				pRet->pTITEM->enmSubType = CABLE_T;
				pRet->type = SINGLE_TAG;
				pRet->nLevelOn = DIM_LEVEL_0_ON;
				pRet->rect.height= env.m_anno.m_nLineTextHeight;

				pRet->m_ptEnd.SetX(pRet->m_ptStart.x());
				pRet->m_ptEnd.SetY(pRet->m_ptStart.y() + pRet->rect.height*0.5);
			}
			else
			{
				pRet->m_ptStart.SetX(m_ptOrigin.x());
				pRet->m_ptStart.SetY(pDivision->GetBaseLine().start().y());
				pRet->rect.width = 0;
				pRet->nOverlap   = 0;
				pRet->type       = SINGLE_TAG;
				pRet->pTITEM	 = this;

				pRet->rect.height= env.m_stru.m_dRadius*2;
				pRet->m_ptEnd    = pRet->m_ptStart;

				//! 2010.10.20 --
				pRet->pTITEM->enmSubType = CABLE_T;
				pRet->nLevelOn = DIM_LEVEL_0_ON;
				pRet->type = SINGLE_TAG;
				pRet->rect.height= env.m_anno.m_nLineTextHeight;

				pRet->m_ptEnd.SetX(pRet->m_ptStart.x() - pRet->rect.height*0.5);
				pRet->m_ptEnd.SetY(pRet->m_ptStart.y());
			}

			oTagPosList.push_back(pRet);
		}
	}

	return ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////
CaDraw_TagCableTray::CSmartDrawAnnoCableTrayTagPos::CSmartDrawAnnoCableTrayTagPos(const double& dElevation) : m_nElevation(dElevation)
{
	bFixed = false;
	ZeroMemory(&rect,sizeof(RECT_T));
	nOverlap = 0;
	nPrevSpace = nNextSpace = 0;
	pTITEM = NULL;
	next = NULL;
}

CaDraw_TagCableTray::CSmartDrawAnnoCableTrayTagPos::~CSmartDrawAnnoCableTrayTagPos()
{

}

/**	
	@brief	write platform tag information to .ext file.

	@author	humkyung

	@param	file	a parameter of type tstream&
	@param	pDivision	a parameter of type CExtDivision*
	@param	pEnv	a parameter of type CDraftEnv*

	@return	void	
*/
void CaDraw_TagCableTray::CSmartDrawAnnoCableTrayTagPos::Write(OFSTREAM_T& ofile , CExtDivision* pDivision)
{
	assert(pDivision && "pDivision is NULL");
	static TCHAR szBuf[512]={0,};
	
	if(pDivision)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		if(pDivision->view()->IsPlanView())
		{
		}
		else if(pDivision->view()->IsSectionView())
		{
			const double nStartDist = (env.m_outside.m_nDistA + env.m_outside.m_nDistB + env.m_outside.m_nDistC + env.m_anno.m_nLineTextHeight);
			STRING_T rAlign = (pDivision->m_vecTagDir.dx() < 0.0) ? _T("Left Bottom") : _T("Right Bottom");
			
			TCHAR szName[2][64]={'\0',};
			TCHAR pName1[MAX_PATH]={0,};
			SPRINTF_T(pName1 , _T("BOT. EL.%d") , (int)m_nElevation);
			STRCPY_T(szName[0],pName1);
			double nTxtLen1 = env.m_cableT.m_oLabelTextStyle.GetTextLength(szName[0]);

			ofile << _T("<TEXT>|");
			ofile << m_ptStart.x() + pDivision->m_vecTagDir.dx() * nStartDist + pDivision->m_vecTagDir.dx() * nTxtLen1 << _T(",");
			ofile << m_ptStart.y() + pDivision->m_vecTagDir.dy() * nStartDist + 0.25 << _T(",0,");
			ofile << env.m_cableT.m_oLabelTextStyle.height() << _T(",") << env.m_cableT.m_oLabelTextStyle.height()*env.m_cableT.m_oLabelTextStyle.GetWidthFactor() << _T(",0,");
			ofile << _T("BOT. EL.") << (int)m_nElevation << _T(",") << rAlign << _T(",0|");
			ofile << env.m_cableT.m_oLabelProp.level << _T(",") << env.m_cableT.m_oLabelProp.color << _T(",");
			ofile << env.m_cableT.m_oLabelTextStyle << std::endl;

			ofile << _T("<LINE>|");
			ofile << m_ptStart.x() + pDivision->m_vecTagDir.dx() * nStartDist << _T(",");
			ofile << m_ptStart.y() + pDivision->m_vecTagDir.dy() * nStartDist << _T(",0,");
			ofile << m_ptStart.x() + pDivision->m_vecTagDir.dx() * nStartDist + pDivision->m_vecTagDir.dx() * nTxtLen1 << _T(",");
			ofile << m_ptStart.y() + pDivision->m_vecTagDir.dy() * nStartDist + pDivision->m_vecTagDir.dy() * nTxtLen1 << _T(",0|");
			ofile << env.m_cableT.m_oLeaderProp << std::endl;
		}
	}
}

/**	
	@brief	register dimenion object for cable tray

	@author	HumKyung.Baek

	@param	pDimLevel	a parameter of type CDimLevel *
	@param	level	a parameter of type const int

	@return	void	
*/
void CaDraw_TagCableTray::CSmartDrawAnnoCableTrayTagPos::OnRegisterToDimension(CDimLevel *pDimLevel, const DIM_LEVEL_E level)
{
	//! do nothing
	/*
	assert(pDimLevel && "pDimLevel is NULL");

	if(pDimLevel)
	{
		if((DIM_LEVEL_0 == level) || (DIM_LEVEL_1 == level))
			(*pDimLevel) += this;
		else	throw exception();
	}
	*/
}

///using namespace tag_entity;
SMARTDRAW_TAG_IMPLEMENT_FUNC(CaDraw_TagCableTray , CaDraw_TagEntity , _T("CaDraw_TagCableTray"))

CaDraw_TagCableTray::CaDraw_TagCableTray(CaDraw_Entity* unnamed) : CaDraw_TagEntity(unnamed)
{
	m_iCableRouteNo = 0;
	m_dElevation = 0.0;
	m_dLength = 0.0;
	s_type = 4;

	m_pConnPointList = new vector<CIsPoint3d>;	/// connection point list
	m_pReducePntList = new vector<CIsPoint3d>;	/// point list for reducer
}

CaDraw_TagCableTray::~CaDraw_TagCableTray(void)
{
	try
	{
		SAFE_DELETE(m_pConnPointList);
		SAFE_DELETE(m_pReducePntList);
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_TagCableTray
    @function   Parse
    @return     int
    @brief		parse cable tray
******************************************************************************/
int CaDraw_TagCableTray::Parse()
{
	CaDraw_CableTray* pDrawCableTray = static_cast<CaDraw_CableTray*>(m_pDrawEntity);
	CaDraw_View* pDrawView = pDrawCableTray->GetView();
	m_sViewName = pDrawView->name();

	m_sType = pDrawCableTray->GetTypeString();
	m_dElevation = pDrawCableTray->startPoint().z();
	m_sName = pDrawCableTray->name();

	const CaDraw_CableTray::ELEMENT_E eType = pDrawCableTray->GetType();
	
	m_sDepartmentType = pDrawCableTray->GetCategoryString();

	m_ptStart = pDrawView->ModelCoord2ViewCoord(pDrawCableTray->startPoint());
	m_ptEnd   = pDrawView->ModelCoord2ViewCoord(pDrawCableTray->endPoint());
	
	m_nWidth = pDrawCableTray->width1();
	m_nHeight= pDrawCableTray->height();

	m_lengthDir = pDrawCableTray->GetLengthDir();
	m_widthDir  = pDrawCableTray->GetWidthDir();
	m_heightDir = pDrawCableTray->GetHeightDir();
	
	if(CaDraw_CableTray::STRAIGHT_T == eType)
	{
		///double   m_nWidth, m_nWidth2, m_nHeight, m_nItemAng;
	}
	else if(CaDraw_CableTray::ELBOW_T == eType)
	{
		m_sSubType = pDrawCableTray->GetSubTypeString();
		m_nItemAng = pDrawCableTray->GetAngle();
		m_ptMid = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pDrawCableTray->Get3rdPoint());
	}
	else if(CaDraw_CableTray::TEE_T == eType)
	{
		m_sSubType = pDrawCableTray->GetSubTypeString();
		m_ptMid = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pDrawCableTray->Get3rdPoint());
		m_nWidth2 = pDrawCableTray->width2();
	}
	else if(CaDraw_CableTray::REDUCE_T == eType)
	{
		m_sSubType = pDrawCableTray->GetSubTypeString();
		m_ptMid = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pDrawCableTray->Get3rdPoint());
		m_nWidth2 = pDrawCableTray->width2();

		vector<CIsPoint3d> oEdgeList;
		pDrawCableTray->GetEdgeList(oEdgeList);
		for(vector<CIsPoint3d>::iterator itr = oEdgeList.begin();itr != oEdgeList.end();++itr)
		{
			CIsPoint3d pt = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(*itr);
			m_pReducePntList->push_back(pt);
		}
	}

	m_ptCenter.SetX((m_ptStart.x() + m_ptEnd.x())*0.5);
	m_ptCenter.SetY((m_ptStart.y() + m_ptEnd.y())*0.5);
	m_dLength = m_ptStart.DistanceTo(m_ptEnd);

	m_pConnPointList->clear();

	m_pConnPointList->push_back(m_ptStart);
	m_pConnPointList->push_back(m_ptEnd);

	// 1:x_seg, 2:y_seg, 3:z_seg, 4:non_ortho
	s_type = 4;
	if ((0.1 > fabs(m_ptStart.y() - m_ptEnd.y())) && (0.1 < fabs(m_ptStart.x() - m_ptEnd.y()))) s_type = 1;
	if ((0.1 > fabs(m_ptStart.x() - m_ptEnd.x())) && (0.1 < fabs(m_ptStart.y() - m_ptEnd.y()))) s_type = 2;
	if ((0.1 > fabs(m_ptStart.x() - m_ptEnd.x())) && (0.1 > fabs(m_ptStart.y() - m_ptEnd.y()))) s_type = 3;	

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_TagCableTray
    @function   CanBeConnected
    @return     bool
    @param      CaDraw_TagCableTray*    rhs
    @brief		두개의 Cable이 서로 연결되는지 검사한다.
******************************************************************************/
bool CaDraw_TagCableTray::CanBeConnected(CaDraw_TagCableTray* rhs)
{
	assert(rhs && "rhs is NULL");

	if(rhs)
	{
		for(vector<CIsPoint3d>::iterator itr = m_pConnPointList->begin();itr != m_pConnPointList->end();++itr)
		{
			for(vector<CIsPoint3d>::iterator jtr = rhs->m_pConnPointList->begin();jtr != rhs->m_pConnPointList->end();++jtr)
			{
				if((*itr) == (*jtr)) return true;
			}
		}
	}

	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   operator=
    @return     CaDraw_TagCableTray&
    @param      const   CaDraw_TagCableTray&
    @param      rhs
    @brief		대입연산자
******************************************************************************/
CaDraw_TagCableTray& CaDraw_TagCableTray::operator=(const CaDraw_TagCableTray& rhs)
{
	if(this != &rhs)
	{
		///seq_no = rhs.seq_no;
		m_sType = rhs.m_sType;
		m_sViewName = rhs.m_sViewName;
		m_sName     = rhs.m_sName;
		
		m_nWidth  = rhs.m_nWidth;
		m_nWidth2 = rhs.m_nWidth2;
		m_nHeight = rhs.m_nHeight;
		m_sSubType = rhs.m_sSubType;
		m_nItemAng = rhs.m_nItemAng;
		m_dElevation = rhs.m_dElevation;

		m_ptStart = rhs.m_ptStart;
		m_ptEnd   = rhs.m_ptEnd;
		m_ptMid   = rhs.m_ptMid;
		m_lengthDir = rhs.m_lengthDir;
		m_widthDir  = rhs.m_widthDir;
		m_heightDir = rhs.m_heightDir;
		
		m_pReducePntList->clear();
		m_pReducePntList->insert(m_pReducePntList->begin() , rhs.m_pReducePntList->begin() , rhs.m_pReducePntList->end());

		m_ptCenter = rhs.m_ptCenter;
		m_dLength  = rhs.m_dLength;
		s_type = rhs.s_type;

		//! connection point list
		m_pConnPointList->clear();
		m_pConnPointList->insert(m_pConnPointList->begin() , rhs.m_pConnPointList->begin() , rhs.m_pConnPointList->end());
		m_iCableRouteNo = rhs.m_iCableRouteNo;

		m_sDepartmentType = rhs.m_sDepartmentType;
	}

	return (*this);
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   name
    @return     STRING_T
    @brief
******************************************************************************/
STRING_T CaDraw_TagCableTray::name() const
{
	return m_sName;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   lengthDir
    @return     CIsVect3d
    @brief
******************************************************************************/
CIsVect3d CaDraw_TagCableTray::lengthDir() const
{
	return m_lengthDir;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   heightDir
    @return     CIsVect3d
    @brief
******************************************************************************/
CIsVect3d CaDraw_TagCableTray::heightDir() const
{
	return m_heightDir;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   widthDir
    @return     CIsVect3d
    @brief
******************************************************************************/
CIsVect3d CaDraw_TagCableTray::widthDir() const
{
	return m_widthDir;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   GetElevation
    @return     double
    @brief
******************************************************************************/
double CaDraw_TagCableTray::GetElevation() const
{
	return m_dElevation;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   IsHorizontal
    @return     bool
    @brief
******************************************************************************/
bool CaDraw_TagCableTray::IsHorizontal() const
{
	const bool b = ((floor(m_ptStart.x()) != floor(m_ptEnd.x())) && (floor(m_ptStart.y()) == floor(m_ptEnd.y())));
	return b;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   IsVertical
    @return     bool
    @brief
******************************************************************************/
bool CaDraw_TagCableTray::IsVertical() const
{
	const bool b = (floor(m_ptStart.x()) == floor(m_ptEnd.x()) && floor(m_ptStart.y()) != floor(m_ptEnd.y()));
	return b;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   IsVertical3d
    @return     bool
    @brief		3d로 vertical인지를 검사한다.
******************************************************************************/
bool CaDraw_TagCableTray::IsVertical3d() const
{
	const bool b = ((floor(m_ptStart.x()) == floor(m_ptEnd.x())) && 
		(floor(m_ptStart.y()) == floor(m_ptEnd.y())) && 
		(floor(m_ptStart.z()) != floor(m_ptEnd.z())));
	
	return b;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   GetViewName
    @return     STRING_T
    @brief
******************************************************************************/
STRING_T CaDraw_TagCableTray::GetViewName() const
{
	return m_sViewName;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   length
    @return     double
    @brief
******************************************************************************/
double CaDraw_TagCableTray::length() const
{
	return m_dLength;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   GetStartPoint
    @return     CIsPoint3d
    @brief
******************************************************************************/
CIsPoint3d CaDraw_TagCableTray::GetStartPoint() const
{
	return m_ptStart;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   GetEndPoint
    @return     CIsPoint3d
    @brief
******************************************************************************/
CIsPoint3d CaDraw_TagCableTray::GetEndPoint() const
{
	return m_ptEnd;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CaDraw_TagCableTray
    @function   GetMidPoint
    @return     CIsPoint3d
    @brief
******************************************************************************/
CIsPoint3d CaDraw_TagCableTray::GetMidPoint() const
{
	return m_ptMid;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_TagCableTray
    @function   CreateTagObjects
    @return     CaDraw_TagItem*
    @param      list<CaDraw_TagItem*>&  TagItemList
    @param      CSmartDrawAnnoView   *pAnnoView
    @brief
******************************************************************************/
CaDraw_TagItem* CaDraw_TagCableTray::CreateTagObjects(list<CaDraw_TagItem*>& oTagItemList , CSmartDrawAnnoView *pAnnoView)
{
	assert(pAnnoView && "pAnnoView is NULL");

	if(pAnnoView)
	{
		oTagItemList.clear();

		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		CaDraw_View* pView = pAnnoView->drawView();

		const CIsPoint3d ptViewOrigin= pView->center();
		const double dViewWidth   = pView->GetViewWidth();
		const double dViewHeight  = pView->GetViewHeight();
		const AREA_T oTagRange   = env.m_area;

		CIsPoint3d pt[2][2] , ptTag[2];
		CIsVect3d vec[2];
		pt[0][0] = m_ptStart;
		pt[0][1] = m_ptEnd;
		pt[1][0] = m_ptStart;
		pt[1][1] = m_ptEnd;

		CIsVect3d vecViewDirVector(0.0 , 0.0 , 1.0);
		const double nDotValue = vecViewDirVector.DotProduct(m_widthDir);

		const double vscale = 1.f / pView->GetScale();
		if((fabs(m_lengthDir.dx()) < 0.001) && (fabs(m_lengthDir.dy()) < 0.001) && (fabs(m_lengthDir.dz()) > 0.001))
		{
			bool bFlags[2][2] = {false,};

			pt[0][0].Set(pt[0][0].x() - (m_widthDir.dx() * m_nWidth * vscale * 0.5) , pt[0][0].y() - (m_widthDir.dy() * m_nWidth * vscale * 0.5) , 0.0);
			pt[0][1] = pt[0][0];
			pt[0][1].Set(pt[0][1].x() + (m_heightDir.dx() * m_nHeight * vscale) , pt[0][1].y() + (m_heightDir.dy() * m_nHeight * vscale) , pt[0][0].z());
			if(fabs(m_heightDir.dx()) > fabs(m_heightDir.dy()))
				this->CreateXDirTagObject(oTagItemList , bFlags[0] ,  pt[0] , pAnnoView);
			else	this->CreateYDirTagObject(oTagItemList , bFlags[0] ,  pt[0] , pAnnoView);

			pt[1][0].Set(pt[1][0].x() + (m_widthDir.dx() * m_nWidth * vscale * 0.5) , pt[1][0].y() + (m_widthDir.dy() * m_nWidth * vscale * 0.5) , 0.0);
			pt[1][1] = pt[1][0];
			pt[1][1].Set(pt[1][1].x() + (m_heightDir.dx() * m_nHeight * vscale) , pt[1][1].y() + (m_heightDir.dy() * m_nHeight * vscale) , 0.0);
			if(fabs(m_heightDir.dx()) > fabs(m_heightDir.dy()))
				this->CreateXDirTagObject(oTagItemList , bFlags[1] ,  pt[1] , pAnnoView);
			else	this->CreateYDirTagObject(oTagItemList , bFlags[1] ,  pt[1] , pAnnoView);
		}
		else if((fabs(m_lengthDir.dz()) < 0.001) && (fabs(m_lengthDir.dx()) < 0.001))
		{
			bool bFlags[2][2] = {false,};

			if(fabs(nDotValue) < CaDraw_TagEnv::TOLERANCE)
			{
				/// Y축 방향으로 놓임.
				pt[0][0].SetX(pt[0][0].x() - (m_nWidth * vscale * 0.5));
				pt[0][1].SetX(pt[0][1].x() - (m_nWidth * vscale * 0.5));
				this->CreateYDirTagObject(oTagItemList , bFlags[0] ,  pt[0] , pAnnoView);

				pt[1][0] = m_ptStart; pt[1][1] = m_ptEnd;
				pt[1][0].SetX(pt[1][0].x() + (m_nWidth * vscale * 0.5));
				pt[1][1].SetX(pt[1][1].x() + (m_nWidth * vscale * 0.5));
				this->CreateYDirTagObject(oTagItemList , bFlags[1] ,  pt[1] , pAnnoView);
				/// HORIZONTAL X-DIR
			}
			else
			{
				this->CreateYDirTagObject(oTagItemList , bFlags[0] ,  pt[0] , pAnnoView , true);
				bFlags[1][0] = bFlags[0][0];
				bFlags[1][1] = bFlags[0][1];
			}
		}	// x
		else if((fabs(m_lengthDir.dz()) < 0.001) && (fabs(m_lengthDir.dy()) < 0.001))
		{
			bool bFlags[2][2] = {false,};

			if(fabs(nDotValue) < CaDraw_TagEnv::TOLERANCE)
			{
				/// X축 방향으로 놓임.
				pt[0][0].SetY(pt[0][0].y() - (m_nWidth * vscale * 0.5));
				pt[0][1].SetY(pt[0][1].y() - (m_nWidth * vscale * 0.5));
				this->CreateXDirTagObject(oTagItemList , bFlags[0] ,  pt[0] , pAnnoView);

				pt[1][0] = m_ptStart; pt[1][1] = m_ptEnd;
				pt[1][0].SetY(pt[1][0].y() + (m_nWidth * vscale * 0.5));
				pt[1][1].SetY(pt[1][1].y() + (m_nWidth * vscale * 0.5));
				this->CreateXDirTagObject(oTagItemList , bFlags[1] ,  pt[1] , pAnnoView);
			}
			else
			{
				this->CreateXDirTagObject(oTagItemList , bFlags[0] , pt[0] , pAnnoView , true);
				bFlags[1][0] = bFlags[0][0];
				bFlags[1][1] = bFlags[0][1];
			}
		}
	}

	return NULL;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CaDraw_TagCableTray
    @function   OnCreateTagObject
    @return     CaDraw_TagItem*
    @param      CSmartDrawAnnoView   *pAnnoView
    @param      const CIsPoint3d& ptOrigin
	@param      const CIsVect3d& vec
    @param      const DITEM_E& enmType
    @brief
******************************************************************************/
CaDraw_TagItem* CaDraw_TagCableTray::OnCreateTagObject(CSmartDrawAnnoView *pAnnoView , const CIsPoint3d& ptOrigin,const CIsVect3d& vec,const DITEM_E& enmType)
{
	return NULL;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-27
    @class      CaDraw_TagCableTray
    @function   GetTagNo
    @return     void
    @param      STRING_T*   pTagNo
    @param      const       CIsPoint3d
    @param      pt          CaDraw_View*
    @param      pView
    @brief
******************************************************************************/
void CaDraw_TagCableTray::GetTagNo(STRING_T* pTagNo , const CIsPoint3d pt , CaDraw_View* pView)
{
	assert(pTagNo && "pTagNo is NULL");
	assert(pView && "pView is NULL");
	
	if(pTagNo && pView) (*pTagNo) = m_sName;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-27
    @class      CaDraw_TagCableTray
    @function   CreateTagObject
    @return     CaDraw_TagItem*
    @param      CaDraw_View *pView
    @param      const CIsPoint3d  ptOrigin
	@param      const CIsVect3d   vec
    @param      DITEM_E enmType
    @brief
******************************************************************************/
CaDraw_TagItem* CaDraw_TagCableTray::CreateTagObject(CaDraw_View *pView, const CIsPoint3d ptOrigin, const CIsVect3d vec, DITEM_E enmType)
{
	assert(pView && "pView is NULL");
	
	CSmartDrawAnnoCableTrayTagItem* pItem=NULL;
	if(pView)
	{
		if(pItem = new CSmartDrawAnnoCableTrayTagItem(m_dElevation))
		{
			pItem->m_ptOrigin= ptOrigin;
			pItem->vecDir  = vec;
			pItem->enmType = enmType;
			pItem->m_pEnt  = this;
			
			CIsPoint3d ptViewCenter = pView->center();
			double nViewWidth = pView->GetViewWidth();
			double nViewHeight= pView->GetViewHeight();
			if(pItem->m_ptOrigin.x() > ptViewCenter.x() + nViewWidth*0.5)
				pItem->m_ptOrigin.x() = ptViewCenter.x() + nViewWidth*0.5;
			if(pItem->m_ptOrigin.x() < ptViewCenter.x() - nViewWidth*0.5)
				pItem->m_ptOrigin.x() = ptViewCenter.x() - nViewWidth*0.5;
			if(pItem->m_ptOrigin.y() > ptViewCenter.y() + nViewHeight*0.5)
				pItem->m_ptOrigin.y() = ptViewCenter.y() + nViewHeight*0.5;
			if(pItem->m_ptOrigin.y() < ptViewCenter.y() - nViewHeight*0.5)
				pItem->m_ptOrigin.y() = ptViewCenter.y() - nViewHeight*0.5;
		}
	}
	
	return pItem;
}

/**
	@brief	create x-dir tag object for cable tray item
	@author	humkyung
	@date	????.??.??
	@return
**/
bool CaDraw_TagCableTray::CreateXDirTagObject(list<CaDraw_TagItem*>& oTagItemList , bool bFlags[2] , const CIsPoint3d pt[], CSmartDrawAnnoView *pAnnoView , const bool& bCreateTagItem)
{
	assert(pAnnoView && "pAnnoView is NULL");

	if(pAnnoView)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		CaDraw_View* pView = pAnnoView->drawView();

		const CIsPoint3d ptViewOrigin= pView->center();
		const double dViewWidth   = pView->GetViewWidth();
		const double dViewHeight  = pView->GetViewHeight();
		const AREA_T oTagRange   = env.m_area;
		
		bFlags[0]=false , bFlags[1] = false;
		CIsPoint3d ptTag[2];
		CIsVect3d vec[2];

		if(	(pt[0].x() >= (ptViewOrigin.x() + dViewWidth*0.5 - dViewWidth*oTagRange.Range[RIGHT_SIDE])) ||
			(pt[0].x() <= (ptViewOrigin.x() - dViewWidth*0.5 + dViewWidth*oTagRange.Range[LEFT_SIDE])))
		{
			vec[0] = pt[0] - pt[1];
			vec[0].Normalize();
			
			ptTag[0] = pAnnoView->GetTagPoint(pt[0],pt[1],X_DIR);
			bFlags[0]= true;
		}
		if(	(pt[1].x() >= (ptViewOrigin.x() + dViewWidth*0.5 - dViewWidth*oTagRange.Range[RIGHT_SIDE])) ||
			(pt[1].x() <= (ptViewOrigin.x() - dViewWidth*0.5 + dViewWidth*oTagRange.Range[LEFT_SIDE])))
		{
			vec[1] = pt[1] - pt[0];
			vec[1].Normalize();
			
			ptTag[1] = pAnnoView->GetTagPoint(pt[1],pt[0],X_DIR);
			bFlags[1]= true;
		}
		
		if(bFlags[0] && bFlags[1])
		{
			if((ptTag[0].x() - ptViewOrigin.x())*(ptTag[1].x() - ptViewOrigin.x()) > 0)
			{
				if(fabs(ptTag[0].x() - ptViewOrigin.x()) > fabs(ptTag[1].x() - ptViewOrigin.x()))
				{
					CaDraw_TagItem* pDITEM = this->CreateTagObject(pView , ptTag[0] , vec[0] , DRAFT_DIM);
					if(pDITEM)
					{
						pDITEM->nLevel = DIM_LEVEL_0;
						pDITEM->SetTypeString(*m_pTypeString);
						pDITEM->enmSubType = HORIZONTAL;
						GetTagNo(pDITEM->name[0],ptTag[0],pView);
						pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
						pDITEM->m_pEnt = this;
						
						oTagItemList.push_back(pDITEM);
					}

					//!
					if(bCreateTagItem)
					{
						pDITEM = this->CreateTagObject(pView , ptTag[0] , vec[0] , DRAFT_CABLE);
						if(pDITEM)
						{
							pDITEM->nLevel = DIM_LEVEL_0;
							pDITEM->SetTypeString(*m_pTypeString);
							pDITEM->enmSubType = HORIZONTAL;
							GetTagNo(pDITEM->name[0],ptTag[0],pView);
							pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
							pDITEM->m_pEnt = this;
							
							oTagItemList.push_back(pDITEM);
						}
					}
				}
				else
				{
					CaDraw_TagItem* pDITEM = this->CreateTagObject(pView , ptTag[1] , vec[1] , DRAFT_DIM);
					if(pDITEM)
					{
						pDITEM->nLevel = DIM_LEVEL_0;
						pDITEM->SetTypeString(*m_pTypeString);
						pDITEM->enmSubType = HORIZONTAL;
						GetTagNo(pDITEM->name[0],ptTag[1],pView);
						pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
						///m_bTagged = true;
						pDITEM->m_pEnt = this;
						
						oTagItemList.push_back(pDITEM);
					}

					if(bCreateTagItem)
					{
						pDITEM = this->CreateTagObject(pView , ptTag[1] , vec[1] , DRAFT_CABLE);
						if(pDITEM)
						{
							pDITEM->nLevel = DIM_LEVEL_0;
							pDITEM->SetTypeString(*m_pTypeString);
							pDITEM->enmSubType = HORIZONTAL;
							GetTagNo(pDITEM->name[0],ptTag[0],pView);
							pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
							pDITEM->m_pEnt = this;
							
							oTagItemList.push_back(pDITEM);
						}
					}
				}
			}
			else
			{
				CaDraw_TagItem* pDITEM = this->CreateTagObject(pView , ptTag[0] , vec[0] , DRAFT_DIM);
				if(pDITEM)
				{
					pDITEM->nLevel = DIM_LEVEL_0;
					pDITEM->SetTypeString(*m_pTypeString);
					pDITEM->enmSubType = HORIZONTAL;
					GetTagNo(pDITEM->name[0],ptTag[0],pView);
					pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
					///m_bTagged = true;
					pDITEM->m_pEnt = this;
					
					oTagItemList.push_back(pDITEM);
				}

				pDITEM = this->CreateTagObject(pView , ptTag[1] , vec[1] , DRAFT_DIM);
				if(pDITEM)
				{
					pDITEM->nLevel = DIM_LEVEL_0;
					pDITEM->SetTypeString(*m_pTypeString);
					pDITEM->enmSubType = HORIZONTAL;
					GetTagNo(pDITEM->name[0],ptTag[1],pView);
					pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
					///m_bTagged = true;
					pDITEM->m_pEnt = this;
					
					oTagItemList.push_back(pDITEM);
				}

				if(bCreateTagItem)
				{
					pDITEM = this->CreateTagObject(pView , ptTag[0] , vec[0] , DRAFT_CABLE);
					if(pDITEM)
					{
						pDITEM->nLevel = DIM_LEVEL_0;
						pDITEM->SetTypeString(*m_pTypeString);
						pDITEM->enmSubType = HORIZONTAL;
						GetTagNo(pDITEM->name[0],ptTag[0],pView);
						pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
						pDITEM->m_pEnt = this;
						
						oTagItemList.push_back(pDITEM);
					}
				}
			}
		}
		else if(bFlags[0])
		{
			CaDraw_TagItem* pDITEM = this->CreateTagObject(pView , ptTag[0] , vec[0] , DRAFT_DIM);
			if(pDITEM)
			{
				pDITEM->nLevel = DIM_LEVEL_0;
				pDITEM->SetTypeString(*m_pTypeString);
				pDITEM->enmSubType = HORIZONTAL;
				GetTagNo(pDITEM->name[0],ptTag[0],pView);
				pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
				//m_bTagged = true;
				pDITEM->m_pEnt = this;
				
				oTagItemList.push_back(pDITEM);
			}

			if(bCreateTagItem)
			{
				pDITEM = this->CreateTagObject(pView , ptTag[0] , vec[0] , DRAFT_CABLE);
				if(pDITEM)
				{
					pDITEM->nLevel = DIM_LEVEL_0;
					pDITEM->SetTypeString(*m_pTypeString);
					pDITEM->enmSubType = HORIZONTAL;
					GetTagNo(pDITEM->name[0],ptTag[0],pView);
					pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
					pDITEM->m_pEnt = this;
					
					oTagItemList.push_back(pDITEM);
				}
			}
		}
		else if(bFlags[1])
		{
			CaDraw_TagItem* pDITEM = this->CreateTagObject(pView , ptTag[1] , vec[1] , DRAFT_DIM);
			if(pDITEM)
			{
				pDITEM->nLevel = DIM_LEVEL_0;
				pDITEM->SetTypeString(*m_pTypeString);
				pDITEM->enmSubType = HORIZONTAL;
				GetTagNo(pDITEM->name[0],ptTag[1],pView);
				pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
				///m_bTagged = true;
				pDITEM->m_pEnt = this;
				
				oTagItemList.push_back(pDITEM);
			}

			if(bCreateTagItem)
			{
				pDITEM = this->CreateTagObject(pView , ptTag[1] , vec[1] , DRAFT_CABLE);
				if(pDITEM)
				{
					pDITEM->nLevel = DIM_LEVEL_0;
					pDITEM->SetTypeString(*m_pTypeString);
					pDITEM->enmSubType = HORIZONTAL;
					GetTagNo(pDITEM->name[0],ptTag[0],pView);
					pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
					pDITEM->m_pEnt = this;
					
					oTagItemList.push_back(pDITEM);
				}
			}
		}

		return true;
	}

	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-27
    @class      CaDraw_TagCableTray
    @function   CreateYDirTagObject
    @return     bool
    @param      bool    bFlags[2]
    @param      const   CIsPoint3d
    @param      pt[2]   CaDraw_View
    @param      *pView  const
    @param      bool&   bCreateTagItem
    @brief
******************************************************************************/
bool CaDraw_TagCableTray::CreateYDirTagObject(list<CaDraw_TagItem*>& oTagItemList , bool bFlags[2] , const CIsPoint3d pt[2] , CSmartDrawAnnoView *pAnnoView , const bool& bCreateTagItem)
{
	assert(pAnnoView && "pAnnoView is NULL");

	if(pAnnoView)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
		CaDraw_View* pView = pAnnoView->drawView();

		const CIsPoint3d ptViewOrigin= pView->center();
		const double nViewWidth      = pView->GetViewWidth();
		const double nViewHeight     = pView->GetViewHeight();
		const AREA_T TagRange       = env.m_area;
		
		bFlags[0]=false , bFlags[1] = false;
		CIsPoint3d ptTag[2];
		CIsVect3d vec[2];

		const double nTopSide    = ptViewOrigin.y() + nViewHeight*0.5 - nViewHeight*TagRange.Range[TOP_SIDE];
		const double nBottomSide = ptViewOrigin.y() - nViewHeight*0.5 + nViewHeight*TagRange.Range[BOTTOM_SIDE];
		const double nLeftSide   = ptViewOrigin.x() + nViewWidth*0.5  - nViewWidth*TagRange.Range[LEFT_SIDE];
		const double nRightSide  = ptViewOrigin.x() - nViewWidth*0.5  + nViewWidth*TagRange.Range[RIGHT_SIDE];
		if(	(pt[0].y() >= (nTopSide)) ||
			(pt[0].y() <= (nBottomSide)))
		{
			vec[0] = pt[0] - pt[1];
			vec[0].Normalize();
			
			ptTag[0] = pAnnoView->GetTagPoint(pt[0],pt[1],Y_DIR);
			bFlags[0]= true;
		}
		if(	(pt[1].y() >= (nTopSide)) ||
			(pt[1].y() <= (nBottomSide)))
		{
			vec[1] = pt[1] - pt[0];
			vec[1].Normalize();
			
			ptTag[1] = pAnnoView->GetTagPoint(pt[1],pt[0],Y_DIR);
			bFlags[1]= true;
		}
		
		if(bFlags[0] && bFlags[1])
		{
			if((ptTag[0].y() - ptViewOrigin.y())*(ptTag[1].y() - ptViewOrigin.y()) > 0)
			{
				//! 같은 영역에 두개가 모두 포함된다.
				if(fabs(ptTag[0].y() - ptViewOrigin.y()) > fabs(ptTag[1].y() - ptViewOrigin.y()))
				{
					CaDraw_TagItem* pDITEM = this->CreateTagObject(pView,ptTag[0],vec[0],DRAFT_DIM);
					if(pDITEM)
					{
						pDITEM->nLevel = DIM_LEVEL_0;
						pDITEM->SetTypeString(*m_pTypeString);
						pDITEM->enmSubType = HORIZONTAL;
						this->GetTagNo(pDITEM->name[0],ptTag[0],pView);
						pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
						pDITEM->m_pEnt = this;
						
						oTagItemList.push_back(pDITEM);
					}

					if(bCreateTagItem)
					{
						pDITEM = this->CreateTagObject(pView,ptTag[0],vec[0],DRAFT_CABLE);
						if(pDITEM)
						{
							pDITEM->nLevel = DIM_LEVEL_0;
							pDITEM->SetTypeString(*m_pTypeString);
							pDITEM->enmSubType = HORIZONTAL;
							this->GetTagNo(pDITEM->name[0],ptTag[0],pView);
							pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
							pDITEM->m_pEnt = this;
							
							oTagItemList.push_back(pDITEM);
						}
					}
				}
				else
				{
					CaDraw_TagItem* pDITEM = this->CreateTagObject(pView,ptTag[1],vec[1],DRAFT_DIM);
					if(pDITEM)
					{
						pDITEM->nLevel = DIM_LEVEL_0;
						pDITEM->SetTypeString(*m_pTypeString);
						pDITEM->enmSubType = HORIZONTAL;
						this->GetTagNo(pDITEM->name[0],ptTag[1],pView);
						pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
						pDITEM->m_pEnt = this;
						
						oTagItemList.push_back(pDITEM);
					}

					if(bCreateTagItem)
					{
						pDITEM = this->CreateTagObject(pView , ptTag[1],vec[1],DRAFT_CABLE);
						if(pDITEM)
						{
							pDITEM->nLevel = DIM_LEVEL_0;
							pDITEM->SetTypeString(*m_pTypeString);
							pDITEM->enmSubType = HORIZONTAL;
							this->GetTagNo(pDITEM->name[0],ptTag[0],pView);
							pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
							pDITEM->m_pEnt = this;
							
							oTagItemList.push_back(pDITEM);
						}
					}
				}
			}
			else
			{
				CaDraw_TagItem* pDITEM = this->CreateTagObject(pView , ptTag[0],vec[0],DRAFT_DIM);
				if(pDITEM)
				{
					pDITEM->nLevel = DIM_LEVEL_0;
					pDITEM->SetTypeString(*m_pTypeString);
					pDITEM->enmSubType = HORIZONTAL;
					this->GetTagNo(pDITEM->name[0],ptTag[0],pView);
					pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
					pDITEM->m_pEnt = this;
					
					oTagItemList.push_back(pDITEM);
				}
				pDITEM = this->CreateTagObject(pView , ptTag[1],vec[1],DRAFT_DIM);
				if(pDITEM)
				{
					pDITEM->nLevel = DIM_LEVEL_0;
					pDITEM->SetTypeString(*m_pTypeString);
					pDITEM->enmSubType = HORIZONTAL;
					this->GetTagNo(pDITEM->name[0],ptTag[1],pView);
					pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
					pDITEM->m_pEnt = this;
					
					oTagItemList.push_back(pDITEM);
				}

				if(bCreateTagItem)
				{
					pDITEM = this->CreateTagObject(pView , ptTag[0],vec[0],DRAFT_CABLE);
					if(pDITEM)
					{
						pDITEM->nLevel = DIM_LEVEL_0;
						pDITEM->SetTypeString(*m_pTypeString);
						pDITEM->enmSubType = HORIZONTAL;
						this->GetTagNo(pDITEM->name[0],ptTag[0],pView);
						pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
						pDITEM->m_pEnt = this;
						
						oTagItemList.push_back(pDITEM);
					}
				}
			}
		}
		else if(bFlags[0])
		{
			CaDraw_TagItem* pDITEM = this->CreateTagObject(pView,ptTag[0],vec[0],DRAFT_DIM);
			if(pDITEM)
			{
				pDITEM->nLevel = DIM_LEVEL_0;
				pDITEM->SetTypeString(*m_pTypeString);
				pDITEM->enmSubType = HORIZONTAL;
				this->GetTagNo(pDITEM->name[0],ptTag[0],pView);
				pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
				pDITEM->m_pEnt = this;
				
				oTagItemList.push_back(pDITEM);
			}

			if(bCreateTagItem)
			{
				pDITEM = this->CreateTagObject(pView,ptTag[0],vec[0],DRAFT_CABLE);
				if(pDITEM)
				{
					pDITEM->nLevel = DIM_LEVEL_0;
					pDITEM->SetTypeString(*m_pTypeString);
					pDITEM->enmSubType = HORIZONTAL;
					this->GetTagNo(pDITEM->name[0],ptTag[0],pView);
					pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
					pDITEM->m_pEnt = this;
					
					oTagItemList.push_back(pDITEM);
				}
			}
		}
		else if(bFlags[1])
		{
			CaDraw_TagItem* pDITEM = this->CreateTagObject(pView,ptTag[1],vec[1],DRAFT_DIM);
			if(pDITEM)
			{
				pDITEM->nLevel = DIM_LEVEL_0;
				pDITEM->SetTypeString(*m_pTypeString);
				pDITEM->enmSubType = HORIZONTAL;
				this->GetTagNo(pDITEM->name[0],ptTag[1],pView);
				pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
				pDITEM->m_pEnt = this;
				
				oTagItemList.push_back(pDITEM);
			}

			if(bCreateTagItem)
			{
				pDITEM = this->CreateTagObject(pView,ptTag[1],vec[1],DRAFT_CABLE);
				if(pDITEM)
				{
					pDITEM->nLevel = DIM_LEVEL_0;
					pDITEM->SetTypeString(*m_pTypeString);
					pDITEM->enmSubType = HORIZONTAL;
					this->GetTagNo(pDITEM->name[0],ptTag[0],pView);
					pDITEM->name[1]->assign(pDITEM->name[0]->c_str());
					pDITEM->m_pEnt = this;
					
					oTagItemList.push_back(pDITEM);
				}
			}
		}

		return true;
	}

	return false;
}