#include "StdAfx.h"
#include <assert.h>
#include <float.h>
#include <IsString.h>
#include <aDraw_View.h>
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoEnv.h"
#include "CableTrayInternalTerritory.h"
#include "CableRoute.h"

#include <map>
#include <algorithm>

using namespace std;
using namespace CableTray;

vector<CCableTrayRoute*> g_oCableRouteList;
/// output data list
vector<CCableOutputData> g_oCableOutputList;
vector<CCableOutputData> g_oCableHatchOutputList;
vector<CCableOutputData> g_oCableUpDownOutputList;
long g_GroupNumber = 1L;

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CableTray
    @function   CreateCableRoute
    @return     int
    @param      vector<CCableTrayRoute*>&       oCableRouteList
    @param      vector<CaDraw_TagCableTray*>&   oCableInputList
    @brief
******************************************************************************/
int CableTray::CreateCableRoute(vector<CaDraw_TagCableTray*>& oCableInputList)
{
	/// clear cable tray route
	for(vector<CCableTrayRoute*>::iterator itr = g_oCableRouteList.begin();itr != g_oCableRouteList.end();++itr)
	{
		SAFE_DELETE(*itr);
	}
	g_oCableRouteList.clear();
	/// up to here

	int nRouteNo = 0;
	for(nRouteNo = 1;;++nRouteNo)
	{
		CCableTrayRoute* pCableRoute = new CCableTrayRoute(nRouteNo);
		g_oCableRouteList.push_back(pCableRoute);

		bool bLoop = false;
		do
		{
			bLoop = false;
			for (vector<CaDraw_TagCableTray*>::iterator itr = oCableInputList.begin(); itr != oCableInputList.end(); ++itr)
			{
				if(0 != (*itr)->m_iCableRouteNo) continue;
				if(true == pCableRoute->IsPossibleToBeMember(*itr))
				{
					pCableRoute->m_sViewNo  = (*itr)->GetViewName();
					(*itr)->m_iCableRouteNo = pCableRoute->m_iRouteNo;
					pCableRoute->AddData(*itr);
					
					bLoop = true;
				}
			}
		}while(bLoop);

		bool bExist = true;
		for (vector<CaDraw_TagCableTray*>::iterator itr = oCableInputList.begin(); itr != oCableInputList.end(); ++itr)
		{
			if(0 == (*itr)->m_iCableRouteNo)
			{
				bExist = false;
				break;
			}
		}
		if(bExist) break;
	}
	
	//Cable Route의 CableTrayInput List를 연결된 순서데로 Sort
	for (vector<CCableTrayRoute*>::iterator itr = g_oCableRouteList.begin(); itr != g_oCableRouteList.end(); ++itr)
	{
		(*itr)->CableRoute_Sort();
	}
	
	//Sort된 Cable Route내의 CablTrayInput List를 Elevation별로 Grouping 한다
	for(vector<CCableTrayRoute*>::iterator itr = g_oCableRouteList.begin(); itr != g_oCableRouteList.end(); ++itr)
	{
		(*itr)->CreateCableGroup();
	}

	return ERROR_SUCCESS;
}

bool C_Len_Sort(const CCableMidData& r1, const CCableMidData& r2)
{
	bool res = r1.len < r2.len;
	return res;
}
bool C_Cnt_Sort(const CCableMidData& r1, const CCableMidData& r2)
{
	bool res = r1.cnt < r2.cnt;
	return res;
}

/**
	@brief	하나의 Cable에 대해서 Cable Item을 위치시킬 Position을 구한다.

	@author	KyungMin.Choi

	@date	????.??.??
*/
CCableOutputData FindTagItemPositionForCableTray(vector<CaDraw_TagCableTray*>& oCableTrayList , CaDraw_View* pView , bool bTag)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	CaDraw_TagCableTray* Ci_iter = oCableTrayList.front();
	const STRING_T sDepartment = Ci_iter->m_sDepartmentType;

	CCableOutputData res;
	
	CIsPoint2d m_cen;
	double m_len = 0.0;
	int PR_cnt = 0, m_cnt = 0;
	
	res.tag = false;
	res.line_2 = false;  // NO XY LINE2
	int nTrayList_Cnt = 0;
	if(oCableTrayList.size() != 0) nTrayList_Cnt = oCableTrayList.size() - 1;

	double nBoxHeight = env.m_cableT.m_dRowHeight * (env.m_cableT.m_iRowCount + nTrayList_Cnt);
	int nCellHeight = int(ceil(nBoxHeight * CCableTrayInternalTerritory::m_pCellMapExp->scale())); 
	TCHAR sBot_el[100], sTagName2[100], sTraySizeTmp[100];
	
	//! Cable Tray Size(WxH) List를 하나의 Vector에 담아둠(Tagging 할때 표기)
	res.m_oTraySizeList.clear();
	for (vector<CaDraw_TagCableTray*>::iterator itr = oCableTrayList.begin(); itr != oCableTrayList.end(); ++itr)
	{
		//sprintf(sTraySizeTmp, "W%dxH%d\\%d", int(itr->m_nWidth), int(itr->m_nHeight),int(itr->m_nCableRouteNo)); //string으로 변환
		sprintf(sTraySizeTmp, _T("W%dxH%d") , int((*itr)->m_nWidth), int((*itr)->m_nHeight)); //string으로 변환
		res.m_oTraySizeList.push_back(sTraySizeTmp);
		res.m_oIntPtList.push_back((*itr)->m_IntPt);
	}

	//! Cable Tag Item의 모든 Text중에서 가장 긴 것의 길이를 BoxWidth로 한다.
	res.TAG_NAME(Ci_iter->name() , _T("") , 1);
	sprintf(sTagName2, _T("W%dxH%d") , int(Ci_iter->m_nWidth), int(Ci_iter->m_nHeight)); //string으로 변환
	res.tag_name2 = sTagName2;
	//prh파일에서 받아온 z값중에 작은 것을 사용
	int nBOT_EL = int(Ci_iter->GetElevation());
	sprintf(sBot_el, _T(" %d") , nBOT_EL); //string으로 변환

	//Prefix와 BOT EL을 하나의 스트링으로.. (Text 길이를 구하기 위해)
	STRING_T sTot_BotEL;
	if(_T("ELEC") == sDepartment)
	{
		 sTot_BotEL = env.m_cableT.m_sElecCableT_EL_Prefix + sBot_el;
	}
	else
	{
		sTot_BotEL = env.m_cableT.m_sInstCableT_EL_Prefix + sBot_el;
	}

	//Cable Tag 중에서 길이가 가장 긴 Tag를 이용하여 Box의 Point를 지정해줌
	double nBoxWidth[3] = {0.0,0.0,0.0};
	nBoxWidth[0] = env.m_cableT.m_oLabelTextStyle.GetTextLength(res.tag_name1);
	nBoxWidth[1] = env.m_cableT.m_oLabelTextStyle.GetTextLength(res.tag_name2);
	nBoxWidth[2] = env.m_cableT.m_oLabelTextStyle.GetTextLength(sTot_BotEL);

	if (env.m_cableT.m_iRowCount <= 2) nBoxWidth[2] = 0.0;

	int nBoxMax = 0;
	for (int nCnt = 0; nCnt <= 2; nCnt++)
	{
		if(nBoxWidth[nBoxMax] < nBoxWidth[nCnt]) 
			nBoxMax = nCnt;
	}
	
	double nBoxWidth_Result = nBoxWidth[nBoxMax] + (nBoxWidth[nBoxMax] * 0.2);

	int nCellWidth = int(ceil(nBoxWidth_Result * CCableTrayInternalTerritory::m_pCellMapExp->scale())); //Box의 width를 Cell 단위로 변환

	m_cen = Ci_iter->m_ptCenter; /// Cable Tray의 Center           
	m_len = Ci_iter->length();  /// Cable Tray의 Length	
	//W_cnt = 0;
	m_cnt = int(ceil(m_len * CCableTrayInternalTerritory::m_pCellMapExp->scale())); //Cable Tray의 Length를 Cell 단위로 변환
	if (0 == m_cnt) m_cnt = 2;
	PR_cnt = 0;   

	//! 찾을 영역을 구한다.
	CCableMidData oMidData;
	oMidData.insert_point = m_cen;
	if (oMidData.insert_point.x() < pView->origin().x())
		oMidData.insert_point.SetX(pView->origin().x());
	else if (oMidData.insert_point.x() > (pView->origin().x() + pView->GetViewWidth()))
		oMidData.insert_point.SetX(pView->origin().x() + pView->GetViewWidth());

	if (oMidData.insert_point.y() < pView->origin().y())
		oMidData.insert_point.SetY(pView->origin().y());
	else if (oMidData.insert_point.y() > (pView->origin().y() + pView->GetViewHeight()))
		oMidData.insert_point.SetY((pView->origin().y() + pView->GetViewHeight()));
	
	/// Insert Point 가 View의 범위를 벗어 날경우.. View의 가장자리로 이동.
	for (vector<CIsPoint2d>::iterator itr = res.m_oIntPtList.begin(); itr != res.m_oIntPtList.end(); ++itr)
	{
		if (itr->x() < pView->origin().x())
			itr->SetX(pView->origin().x());
		else if (itr->x() > (pView->origin().x() + pView->GetViewWidth()))
			itr->SetX(pView->origin().x() + pView->GetViewWidth());

		if (itr->y() < pView->origin().y())
			itr->SetY(pView->origin().y());
		else if (itr->y() > (pView->origin().y() + pView->GetViewHeight()))
			itr->SetY(pView->origin().y() + pView->GetViewHeight());
	}

	CCellMap::CellIndex ins_index = CCableTrayInternalTerritory::m_pCellMapExp->PointToIndex(m_cen); //!  그래픽 좌표를 CELL 좌표로 변환시킨다.
	//m_Cnt
	int st_x = int(ins_index.x - nBoxWidth_Result * 5);//floor(double(View.m_nWidth) / 2.0);			
	int ed_x = int(ins_index.x + nBoxWidth_Result * 5 - nCellWidth);//floor(double(View.m_nWidth) / 2.0) - nCellWidth;
	int st_y = int(ins_index.y - nBoxHeight * 10);//floor(double(View.m_nHeight) / 2.0);
	int ed_y = int(ins_index.y + nBoxHeight * 10 - nCellHeight);//floor(double(View.m_nHeight) / 2.0) - nCellHeight;
	
	st_x = (st_x < 0) ? 0 : st_x;
	ed_x = (ed_x >= (int(CCableTrayInternalTerritory::m_pCellMapExp->xlen()) - nCellWidth)) ? ((CCableTrayInternalTerritory::m_pCellMapExp->xlen() - nCellWidth) - 1) : ed_x;
	st_y = (st_y < 0) ? 0 : st_y;
	ed_y = (ed_y >= (int(CCableTrayInternalTerritory::m_pCellMapExp->ylen()) - nCellHeight))? ((CCableTrayInternalTerritory::m_pCellMapExp->ylen() - nCellHeight)- 1) : ed_y;
	//!
	
	vector<CCableMidData> oMidDataList;
	if(bTag)
	{
		int K = 0;
		if (!(st_x < 0) && !(ed_x >= int(CCableTrayInternalTerritory::m_pCellMapExp->xlen())))
		{
			if ((ed_y - st_y) > 0) 
			{
				for (int J = st_y; J <= ed_y; ++J)
				{
					for (int I = st_x; I <= ed_x; ++I)
					{
						K = 0;
						//! Tag Item의 영역
						for (int II = I; II <= (I + nCellWidth -1); ++II)
						{
							for (int JJ = J ; JJ <= (J + nCellHeight - 1); ++JJ)
							{
								if (CCableTrayInternalTerritory::m_pCellMapExp->Value(II , JJ) != 0) K++;
							}
						}

						CCellMap::CellIndex cIndex;
						cIndex.x = I;
						cIndex.y = J;
						CIsPoint2d tag_po = CCableTrayInternalTerritory::m_pCellMapExp->IndexToPoint(cIndex);
						oMidData.cnt = K;
						oMidData.PRI(tag_po, nBoxWidth_Result , nBoxHeight);
						oMidDataList.push_back(oMidData);				
						PR_cnt++;
					}
				}
			}
		}
	}
	else
	{
		CCellMap::CellIndex cIndex;
		cIndex.x = st_x;
		cIndex.y = st_y;
		CIsPoint2d tag_po = CCableTrayInternalTerritory::m_pCellMapExp->IndexToPoint(cIndex);
		oMidData.cnt = 1;
		oMidData.PRI(tag_po, nBoxWidth_Result , nBoxHeight);
		oMidDataList.push_back(oMidData);
		PR_cnt++;
	}

	//! 무수히 많은 중간 값들 중에서 알맞은 값을 구한다.
	if (PR_cnt > 0)
	{
		stable_sort(oMidDataList.begin(), oMidDataList.end(), C_Len_Sort);  // LEN 으로 SORT
		stable_sort(oMidDataList.begin(), oMidDataList.end(), C_Cnt_Sort);  // TAG CNT 으로 SORT
		oMidData = oMidDataList.front();

		res.xdir = oMidData.x_dir;

		res.insert_point = oMidData.insert_point;
		res.m_tag_point  = oMidData.tag_point;
		res.m_nTagWidth  = nBoxWidth_Result;
		res.m_nTagHeight = nBoxHeight;
		res.m_nWidth     = Ci_iter->m_nWidth;
		res.m_nHeight    = Ci_iter->m_nHeight;
		res.bot_el       = sTot_BotEL;
		res.tray_start   = Ci_iter->GetStartPoint();
		res.tray_end     = Ci_iter->GetEndPoint();
		res.width_dir    = Ci_iter->widthDir();
		res.height_dir   = Ci_iter->heightDir();
		res.m_sType	 = Ci_iter->m_sType;
		res.m_sSubType   = Ci_iter->m_sSubType;
		res.m_sDepartmentType = Ci_iter->m_sDepartmentType;

		if (_T("ELBOW") == res.m_sType)
		{
			res.m_sSubType = Ci_iter->m_sSubType;
			res.m_nItemAng = Ci_iter->m_nItemAng;
			res.tray_mid   = Ci_iter->GetMidPoint();
		}
		else if (_T("TEE") == res.m_sType)
		{
			res.m_sSubType = Ci_iter->m_sSubType;
			res.m_nWidth2  = Ci_iter->m_nWidth2;
			res.tray_mid   = Ci_iter->GetMidPoint();
		}
		else if (_T("REDUCE") == res.m_sType)
		{
			res.m_sSubType = Ci_iter->m_sSubType;
			res.m_nWidth2  = Ci_iter->m_nWidth2;
			
			res.m_oReducePntList.clear();
			res.m_oReducePntList.insert(res.m_oReducePntList.end() , Ci_iter->m_pReducePntList->begin() , Ci_iter->m_pReducePntList->end());
		}

		res.OUTPUT_DATA(oMidData.tag_point, env.m_cableT.m_oLabelTextStyle.m_nHeight , env.m_cableT.m_oLabelTextStyle.m_nHeight , nBoxWidth_Result, env.m_cableT.m_dRowHeight);
		
		/// occupy cell
		if (bTag)
		{
			CIsPoint2d minPnt(res.m_tag_point);
			CIsPoint2d maxPnt(res.m_tag_point);
			maxPnt.SetX(maxPnt.x() + nBoxWidth_Result);
			maxPnt.SetY(maxPnt.y() + nBoxHeight);
					
			CCellMap::CellConversionAxis cAxis;
			CCableTrayInternalTerritory::m_pCellMapExp->UpdateRectOf(minPnt , maxPnt , 13);
		}

		oMidDataList.clear();
		res.tag = true;
	}

	return res;
}

/*
	@brief	모든 Cable에 대해서 빈 공간을 찾아 Cable Item을 위치시킨다.

	@author	humkyung

	@date	????.??.??
*/
int CableTray::Annotate(/*double Tol*/)
{
	return ERROR_SUCCESS;

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	bool ch_id = false;
	bool chkElev = false;

	CCableOutputData oOutputData;
	for (vector<CCableTrayRoute*>::iterator itr = g_oCableRouteList.begin(); itr != g_oCableRouteList.end(); ++itr)
	{
		CaDraw_View* pView = (*itr)->GetView();
		ch_id = false;

		CaDraw_View::DIRECTIONT_T vdir = pView->GetViewDir();
		if((CaDraw_View::ISO1_VIEW_T == vdir) || (CaDraw_View::ISO2_VIEW_T == vdir) || (CaDraw_View::ISO3_VIEW_T == vdir))
		{
			// ISO View에서는 BOT. Elev. 만 표기함
			(*itr)->SetCableISOData(pView, g_oCableOutputList);
		}
		else
		{
			(*itr)->SetCableHatchData(pView , g_oCableHatchOutputList); /// Hatch 할 Cable Tray Data

			/// Section View에서는 Cabl Tray Tag를 표기 하지 않음.2010.10.15
			if((CaDraw_View::FRONT_VIEW_T == vdir) || (CaDraw_View::BACK_VIEW_T == vdir) ||
				(CaDraw_View::LEFT_VIEW_T == vdir) || (CaDraw_View::RIGHT_VIEW_T == vdir))
			{
				continue;
			}

			// cable tray route에서 tagging할 cable tray를 구한다.
			vector<CaDraw_TagCableTray*> oTaggingCableTrayList;
			(*itr)->GetTaggingCableTray(oTaggingCableTrayList);

			for(vector<CaDraw_TagCableTray*>::iterator jtr = oTaggingCableTrayList.begin();jtr != oTaggingCableTrayList.end();++jtr)
			{
				/// Department 별로 On/Off 기능 추가
				if(env.m_cableT.m_sElecCableT_Tag_OnOff != _T("ON") && (*jtr)->m_sDepartmentType == _T("ELEC")) continue;
				if(env.m_cableT.m_sInstCableT_Tag_OnOff != _T("ON") && (*jtr)->m_sDepartmentType == _T("INST")) continue;

				//! grouping할 cable tray를 구한다.
				vector<CaDraw_TagCableTray*> oCableTrayGroupingList;
				for (vector<CCableTrayRoute*>::iterator ktr = g_oCableRouteList.begin(); ktr != g_oCableRouteList.end(); ++ktr)
				{
					if((*jtr)->m_iCableRouteNo == (*ktr)->m_iRouteNo) continue;

					(*ktr)->GetCableTrayToBeGroupingOn(oCableTrayGroupingList , *jtr , (*jtr)->GetElevation());
				}

				(*jtr)->m_IntPt = (*jtr)->m_ptCenter;
				oCableTrayGroupingList.insert(oCableTrayGroupingList.begin() , *jtr);

				oOutputData = FindTagItemPositionForCableTray(oCableTrayGroupingList , pView , true);
				if (oOutputData.tag) 
				{
					ch_id = true;
					oOutputData.m_pView = pView;
					oOutputData.height_dir = (*jtr)->heightDir();
					g_oCableOutputList.push_back(oOutputData);
				}
			}

			//Fitting 류 Tag 처리
			if(_T("ON") == env.m_cableT.m_sFittingTag_OnOff)
			{
				(*itr)->SetCableFittingData(pView, g_oCableOutputList);
			}

			////UpDown Mark
			if(_T("ON") == env.m_cableT.m_sUpDownMark_OnOff)
			{
				(*itr)->SetCableUpDownMarkData(pView , g_oCableUpDownOutputList);
			}
		}
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-23
    @class      CableTray
    @function   Write
    @return     int
    @param      OFSTREAM_T& ofile
    @brief		write cable tray tag
******************************************************************************/
int CableTray::Write(OFSTREAM_T& ofile)
{
	int out_count = 1001;
	/// Cable Tray Tag Output List
	for(vector<CCableOutputData>::iterator itr = g_oCableOutputList.begin(); itr != g_oCableOutputList.end(); ++itr)
	{
		if((CaDraw_View::ISO1_VIEW_T == itr->m_pView->GetViewDir()) || (CaDraw_View::ISO2_VIEW_T == itr->m_pView->GetViewDir()) || 
			(CaDraw_View::ISO3_VIEW_T == itr->m_pView->GetViewDir()))
		{
			itr->WriteBOT_ForISO(ofile);
			out_count++;
		}
		else
		{
			itr->WriteTrayTag(ofile);
			out_count++;
		}
	}
	
	/// write cable tray hatch
	for (vector<CCableOutputData>::iterator itr = g_oCableHatchOutputList.begin(); itr != g_oCableHatchOutputList.end(); ++itr)
	{
		itr->WriteHatch(ofile);
	}
	
	/// write cable tray up/down mark
	for (vector<CCableOutputData>::iterator itr = g_oCableUpDownOutputList.begin(); itr != g_oCableUpDownOutputList.end(); ++itr)
	{
		itr->WriteUpDownMark(ofile);
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CCableMidData
    @function   PRI
    @return     void
    @param      CIsPoint2d  Origin
    @param      double      nBoxWidth
    @param      double      nBoxHeight
    @brief
******************************************************************************/
void CCableMidData::PRI(CIsPoint2d Origin, double nBoxWidth, double nBoxHeight)
{
	CIsPoint2d Mid_point;

	Mid_point.SetX(Origin.x() + nBoxWidth * 0.5);
	Mid_point.SetY(Origin.y() + nBoxHeight* 0.5);
	tag_point = Origin;
	len = insert_point.DistanceTo(Mid_point);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CCableOutputData

CCableOutputData::CCableOutputData(const CCableOutputData& rhs)
{
	(*this) = rhs;
}

CCableOutputData& CCableOutputData::operator=(const CCableOutputData& rhs)
{
	if(this != &rhs)
	{
		tag = rhs.tag;
		insert_point = rhs.insert_point;	//! graphic data origin point
		m_tag_point = rhs.m_tag_point;	//! tag data origin point(low-left)
		m_nWidth = rhs.m_nWidth;
		m_nHeight = rhs.m_nHeight;

		tag_name1 = rhs.tag_name1;
		tag_name2 = rhs.tag_name2;
		name1_point = rhs.name1_point;
		name2_point = rhs.name2_point;
		xdir = rhs.xdir;
		line_2 = rhs.line_2;
		Angle = rhs.Angle;
		///line1_point = rhs.line1_point;
		line2_point = rhs.line2_point;

		//added by K.M Choi
		bot_el = rhs.bot_el;
		tray_start = rhs.tray_start;
		tray_end = rhs.tray_end;
		len_dir = rhs.len_dir;
		width_dir = rhs.width_dir;
		height_dir = rhs.height_dir;
		m_sType = rhs.m_sType;
		m_sSubType = rhs.m_sSubType;
		m_sDepartmentType = rhs.m_sDepartmentType;
		tray_mid = rhs.tray_mid;

		m_oReducePntList.clear();
		m_oReducePntList.insert(m_oReducePntList.begin() , rhs.m_oReducePntList.begin() , rhs.m_oReducePntList.end());

		m_nItemAng = rhs.m_nItemAng;
		m_nWidth2 = rhs.m_nWidth2;
		
		m_oTraySizeList.clear();
		m_oTraySizeList.insert(m_oTraySizeList.begin() , rhs.m_oTraySizeList.begin() , rhs.m_oTraySizeList.end());

		m_oIntPtList.clear();
		m_oIntPtList.insert(m_oIntPtList.begin(), rhs.m_oIntPtList.begin(), rhs.m_oIntPtList.end());

		m_pView = rhs.m_pView;
	}

	return (*this);
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-24
    @class      CCableOutputData
    @function   GetClipPolygonOfView
    @return     int
    @brief
******************************************************************************/
int CCableOutputData::GetClipPolygonOfView(pointVector_& clipPolygon)
{
	clipPolygon.clear();

	clipPolygon.push_back(CIsPoint2d(m_pView->origin().x() , m_pView->origin().y()));
	clipPolygon.push_back(CIsPoint2d(m_pView->origin().x() + m_pView->GetViewWidth() , m_pView->origin().y()));
	clipPolygon.push_back(CIsPoint2d(m_pView->origin().x() + m_pView->GetViewWidth() , m_pView->origin().y() + m_pView->GetViewHeight()));
	clipPolygon.push_back(CIsPoint2d(m_pView->origin().x() , m_pView->origin().y() + m_pView->GetViewHeight()));
	clipPolygon.push_back(CIsPoint2d(m_pView->origin().x() , m_pView->origin().y()));

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CCableOutputData
    @function   TAG_NAME
    @return     void
    @param      STRING_T    Name1
    @param      STRING_T    Name2
    @param      int         Type
    @brief
******************************************************************************/
void CCableOutputData::TAG_NAME(STRING_T Name1, STRING_T Name2, int Type)
{
	switch (Type) 
	{
	case 1 : 
		tag_name1 = Name1;
		tag_name2 = _T("");
		break;
	case 2 : 
		tag_name1 = Name1 + _T(" (") + Name2 + _T(")");
		tag_name2 = _T("");
		break;
	case 3 : 
		tag_name1 = Name1;
		tag_name2 = Name2;
		break;
	default : break;
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @function   GetFittingTagName
    @return     void
    @param      const       CaDraw_TagCableTray*
    @param      objInput
    @brief
******************************************************************************/
void CCableOutputData::GetFittingTagName(const CaDraw_TagCableTray* objInput)
{
	assert(objInput && "objInput is NULL");

	if(objInput)
	{
		STRING_T sTagName;

		if(objInput->m_sSubType == _T("HORIZONTAL")) sTagName = _T("H");
		else if(objInput->m_sSubType == _T("VERTICAL")) sTagName = _T("V");

		if(objInput->m_sType == _T("ELBOW"))
		{
			if(objInput->m_nItemAng < 40.0) sTagName += _T("E3");
			else if(objInput->m_nItemAng < 50.0) sTagName += _T("E4");
			else sTagName += _T("E");
		}
		else if(objInput->m_sType == _T("TEE")) sTagName = _T("T");
		else if(objInput->m_sType == _T("REDUCER")) sTagName = _T("R");
		else if(objInput->m_sType == _T("CROSS")) sTagName = _T("CR");

		tag_name1 = sTagName;
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CCableOutputData
    @function   OUTPUT_DATA
    @return     void
    @param      CIsPoint2d  cen
    @param      double      H_len1
    @param      double      H_len2
    @param      double      len
    @param      double      Gap
    @brief
******************************************************************************/
void CCableOutputData::OUTPUT_DATA(CIsPoint2d cen, double H_len1, double H_len2, double len, double Gap)
{
	name1_point.SetX(cen.x());
	name1_point.SetY(cen.y() + Gap);
	name2_point.SetX(cen.x());
	name2_point.SetY(cen.y() - (Gap + H_len2));
	Angle = 0.0;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-23
    @class      CCableOutputData
    @function   WriteTrayTag
    @return     void
    @param      OFSTREAM_T& ofile
    @brief
******************************************************************************/
void CCableOutputData::WriteTrayTag(OFSTREAM_T& ofile)
{
	int nCnt, nMaxCnt;
	CIsPoint2d Cabl_ClosePnt;
	double Cabl_CloseTmp[4];
	int Cabl_Row[2];
	int nCabl_Min;

	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	int nTrayList_Cnt = 0;
	if(m_oTraySizeList.size() != 0) nTrayList_Cnt = m_oTraySizeList.size() - 1;
	
	if (env.m_cableT.m_iRowCount < 2)	return; //Cable Tray Tag는 2 Rows 부터 태그..

	double dLen_Tmp[3] = {0.0,};
	dLen_Tmp[0] = env.m_cableT.m_oLabelTextStyle.GetTextLength(tag_name1);
	dLen_Tmp[1] = env.m_cableT.m_oLabelTextStyle.GetTextLength(tag_name2);
	dLen_Tmp[2] = env.m_cableT.m_oLabelTextStyle.GetTextLength(bot_el);
	if (env.m_cableT.m_iRowCount <= 2) dLen_Tmp[2] = 0.0; //2 Row일 경우에는 BOT_EL은 제외..

	nMaxCnt = 0;

	//Tag의 길이가 가장 긴것을 선택하여 Box의 Width로 사용 
	for (int nCnt = 0; nCnt <= 2; nCnt++)
	{
		if(dLen_Tmp[nMaxCnt] < dLen_Tmp[nCnt])
		{
			nMaxCnt = nCnt;
		}
	}

	double dTag_Len = dLen_Tmp[nMaxCnt] + (dLen_Tmp[nMaxCnt] * 0.2); //Font Size가 제대로 나오면 0.2는 삭제

	CIsPoint2d Cabl_Insert(insert_point);

	CIsPoint2d Cabl_PLinePnt[4]; //poly Line 좌표
	//Left Low
	Cabl_PLinePnt[0].SetX( m_tag_point.x() );
	Cabl_PLinePnt[0].SetY( m_tag_point.y() + (env.m_cableT.m_dRowHeight * (env.m_cableT.m_iRowCount + nTrayList_Cnt)) );
	//Right Top
	Cabl_PLinePnt[1].SetX( m_tag_point.x() + dTag_Len );
	Cabl_PLinePnt[1].SetY( m_tag_point.y() + (env.m_cableT.m_dRowHeight * (env.m_cableT.m_iRowCount + nTrayList_Cnt)) );
	//Right Bottom
	Cabl_PLinePnt[2].SetX( m_tag_point.x() + dTag_Len );
	Cabl_PLinePnt[2].SetY( m_tag_point.y() );
	//Left Bottom
	Cabl_PLinePnt[3].SetX( m_tag_point.x() );
	Cabl_PLinePnt[3].SetY( m_tag_point.y() );

	ofile << _T("<GROUP>|") << (g_GroupNumber++) << _T(",CABLET,Pat 01") << std::endl;
	ofile << _T("<PLINE>|") << Cabl_PLinePnt[0].x() << _T(",") << Cabl_PLinePnt[0].y() << _T(",");
	ofile << Cabl_PLinePnt[1].x() << _T(",") << Cabl_PLinePnt[1].y() << _T(",");
	ofile << Cabl_PLinePnt[2].x() << _T(",") << Cabl_PLinePnt[2].y() << _T(",");
	ofile << Cabl_PLinePnt[3].x() << _T(",") << Cabl_PLinePnt[3].y() << _T(",");
	ofile << Cabl_PLinePnt[0].x() << _T(",") << Cabl_PLinePnt[0].y() << _T("|");
	ofile << env.m_cableT.sLeader_Color << _T(",1,0,1,1") << std::endl;


	Cabl_Row[0] = 1;
	Cabl_Row[1] = 0;

	if(3 == env.m_cableT.m_iRowCount)
	{
		Cabl_Row[0] = env.m_cableT.m_iRowCount + nTrayList_Cnt -1;
		Cabl_Row[1] = 1;
		ofile << _T("<TEXT>|") << m_tag_point.x() + (dTag_Len * 0.05) << _T(",") << m_tag_point.y() + ((env.m_cableT.m_dRowHeight - env.m_cableT.m_oLabelTextStyle.m_nHeight) * 0.5);
		ofile << _T(",") << 0.0 << _T(",") << env.m_cableT.m_oLabelTextStyle.m_nHeight;
		ofile << _T(",") << (env.m_cableT.m_oLabelTextStyle.m_nHeight * env.m_cableT.m_oLabelTextStyle.m_nWidthFactor) << _T(",") << Angle << _T(",") << bot_el << _T(",") << _T("Left Bottom,") << 0.0 << _T("|");
		ofile << env.m_cableT.sText_Color << _T(",") << 1 << _T(",") << 3 << _T(",") << 0 << std::endl;
	}

	ofile << _T("<TEXT>|") << m_tag_point.x() + (dTag_Len * 0.05) << _T(",");
	ofile << m_tag_point.y() + ((env.m_cableT.m_dRowHeight - env.m_cableT.m_oLabelTextStyle.m_nHeight) * 0.5) + (env.m_cableT.m_dRowHeight * Cabl_Row[0]) << _T(",");
	ofile << _T("0,") << env.m_cableT.m_oLabelTextStyle.m_nHeight << _T(",");
	ofile << (env.m_cableT.m_oLabelTextStyle.m_nHeight*env.m_cableT.m_oLabelTextStyle.m_nWidthFactor) << _T(",") << Angle << _T(",") << tag_name1 << _T(",Left Bottom,0|") << env.m_cableT.sText_Color << _T(",1,3,0") << std::endl; 
	
	for(vector<STRING_T>::iterator itr = m_oTraySizeList.begin(); itr != m_oTraySizeList.end(); ++itr)
	{
		ofile << _T("<TEXT>|") << m_tag_point.x() + (dTag_Len * 0.05) << _T(",") << m_tag_point.y() + ((env.m_cableT.m_dRowHeight - env.m_cableT.m_oLabelTextStyle.m_nHeight) * 0.5) + (env.m_cableT.m_dRowHeight * Cabl_Row[1]) << _T(",");
		ofile << _T("0,") << env.m_cableT.m_oLabelTextStyle.m_nHeight << _T(",");
		ofile << (env.m_cableT.m_oLabelTextStyle.m_nHeight*env.m_cableT.m_oLabelTextStyle.m_nWidthFactor) << _T(",") << Angle << _T(",") << itr->c_str() << _T(",Left Bottom,0|");
		ofile << env.m_cableT.sText_Color << _T(",1,3,0") << std::endl;

		Cabl_Row[1] += 1;
	}

	//most close point from insert point

	Cabl_CloseTmp[0] = Cabl_Insert.DistanceTo(Cabl_PLinePnt[0]);
	Cabl_CloseTmp[1] = Cabl_Insert.DistanceTo(Cabl_PLinePnt[1]);
	Cabl_CloseTmp[2] = Cabl_Insert.DistanceTo(Cabl_PLinePnt[2]);
	Cabl_CloseTmp[3] = Cabl_Insert.DistanceTo(Cabl_PLinePnt[3]);
	nCabl_Min = 0;

	for (nCnt = 0; nCnt <= 3; nCnt++)
	{
		if(Cabl_CloseTmp[nCabl_Min] > Cabl_CloseTmp[nCnt])
		{
			nCabl_Min = nCnt;
		}
	}

	Cabl_ClosePnt = Cabl_PLinePnt[nCabl_Min];
	
	CIsPoint2d minDistance_Pnt, maxDistance_Pnt;
	minDistance_Pnt = Cabl_Insert;
	maxDistance_Pnt = Cabl_Insert;


	for(vector<CIsPoint2d>::iterator itr = m_oIntPtList.begin(); itr != m_oIntPtList.end(); ++itr)
	{
		if(Cabl_ClosePnt.DistanceTo(*itr) < Cabl_ClosePnt.DistanceTo(minDistance_Pnt))
			minDistance_Pnt = *itr;

		if(Cabl_ClosePnt.DistanceTo(*itr) > Cabl_ClosePnt.DistanceTo(maxDistance_Pnt))
			maxDistance_Pnt = *itr;

		ofile << _T("<DONUT>|") << 0.25 << _T(",") << 0.5 << _T(",") << itr->x() << _T(",") << itr->y() << _T("|");
		ofile << env.m_cableT.sLeader_Color << _T(",1") << std::endl;
	}
	
	STRING_T sArrowType = env.m_cableT.m_sArrowType;

	ofile << _T("<LEADER>|") << minDistance_Pnt.x() << _T(",") << minDistance_Pnt.y() << _T(",");
	ofile << Cabl_ClosePnt.x() << _T(",") << Cabl_ClosePnt.y() << _T(",");
	ofile << Cabl_ClosePnt.x() << _T(",") << Cabl_ClosePnt.y() << _T("|");
	ofile << env.m_cableT.sLeader_Color << _T(",1,") << sArrowType << _T(",1,") << (1.f/3.f) << _T(",1") << std::endl;

	for (nCnt = 1; nCnt < env.m_cableT.m_iRowCount + nTrayList_Cnt; nCnt++)
	{
		ofile << _T("<LINE>|") << m_tag_point.x() << _T(",") << m_tag_point.y() + (env.m_cableT.m_dRowHeight * (env.m_cableT.m_iRowCount + nTrayList_Cnt)) - env.m_cableT.m_dRowHeight * nCnt << _T(",0,");
		ofile << m_tag_point.x() + dTag_Len << _T(",") << m_tag_point.y() + (env.m_cableT.m_dRowHeight * (env.m_cableT.m_iRowCount + nTrayList_Cnt)) - env.m_cableT.m_dRowHeight * nCnt << _T(",0|");
		ofile << env.m_cableT.sLeader_Color << _T(",1,0,1,1") << std::endl;
	}
	
	ofile << _T("<LINE>|") << minDistance_Pnt.x() << _T(",") << minDistance_Pnt.y() << _T(",0,");
	ofile << maxDistance_Pnt.x() << _T(",") << maxDistance_Pnt.y() << _T(",0|");
	ofile << env.m_cableT.sLeader_Color << _T(",1,0,1,1") << std::endl;

	ofile << _T("</GROUP>") << std::endl;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-23
    @class      CCableOutputData
    @function   WriteHatch
    @return     int
    @param      OFSTREAM_T& ofile
    @brief
******************************************************************************/
int CCableOutputData::WriteHatch(OFSTREAM_T& ofile)
{
	if(_T("STRAIGHT") == m_sType )
		this->WriteStraightHatch(ofile);
	else if (_T("ELBOW") == m_sType)
		this->WriteElbowHatch(ofile);
	else if (_T("TEE") == m_sType)
		this->WriteTeeHatch(ofile);
	else if (_T("REDUCE") == m_sType)  
		this->WriteReduceHatch(ofile);

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-23
    @class      CCableOutputData
    @function   WriteElbowHatch
    @return     void
    @param      OFSTREAM_T& int_file
    @brief
******************************************************************************/
void CCableOutputData::WriteElbowHatch(OFSTREAM_T& int_file)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	const double vScale = 1.0 / m_pView->GetScale();

	// Insert Cable Tray Hatch
	double tray_width = m_nWidth * vScale;
	double tray_height= m_nHeight * vScale;
	//GetWidthAndHeight(tray_width , tray_height);
		
	CIsVect3d vec_stPnt, vec_edPnt, vec_cenPnt, BasePnt;
	pointVector_ input , clipPolygon , output;
	CIsVect3d vec_D_stPnt, vec_D_edPnt, vec_D_cenPnt;
	
	BasePnt = tray_mid;

	CIsVect3d vec_st(tray_start - tray_mid);
	vec_st.Normalize();
	
	CIsVect3d vec_ed(tray_end - tray_mid);
	vec_ed.Normalize();
	
	CIsVect3d vec_tmp = vec_st*vec_ed;
	vec_tmp.Normalize();
	
	vector<CIsPoint3d> HatchPoints;
	STRING_T sViewDir = m_pView->GetViewDirString();
	IsString::ToUpper(sViewDir);
	if(_T("HORIZONTAL") == m_sSubType)
	{
		if(this->IsVisible(vec_tmp))
		{
			HatchPoints.resize(4);
			HatchPoints[0] = tray_end;
			HatchPoints[1] = tray_start;
			HatchPoints[2] = tray_start - (height_dir * tray_height);
			HatchPoints[3] = tray_end - (height_dir * tray_height);
		}
		else
		{
			double nRadius[2] = {0.f,};	
			HatchPoints.resize(10);
			
			nRadius[0] = tray_mid.DistanceTo(tray_start) + tray_width * 0.5;
			nRadius[1] = tray_mid.DistanceTo(tray_start) - tray_width * 0.5;

			HatchPoints[0] = tray_mid + (vec_st * nRadius[0]);
			HatchPoints[9] = tray_mid + (vec_st * nRadius[1]);

			HatchPoints[4] = tray_mid + (vec_ed * nRadius[0]);
			HatchPoints[5] = tray_mid + (vec_ed * nRadius[1]);

			HatchPoints[2] = GetMiddlePoint(HatchPoints[0] , HatchPoints[4] , tray_mid , nRadius[0]);
			HatchPoints[1] = GetMiddlePoint(HatchPoints[0] , HatchPoints[2] , tray_mid , nRadius[0]);
			HatchPoints[3] = GetMiddlePoint(HatchPoints[2] , HatchPoints[4] , tray_mid , nRadius[0]);
			
			HatchPoints[7] = GetMiddlePoint(HatchPoints[9] , HatchPoints[5] , tray_mid , nRadius[1]);
			HatchPoints[8] = GetMiddlePoint(HatchPoints[9] , HatchPoints[7] , tray_mid , nRadius[1]);
			HatchPoints[6] = GetMiddlePoint(HatchPoints[7] , HatchPoints[5] , tray_mid , nRadius[1]);
		}
	}
	else //Vertical
	{
		if(this->IsVisible(vec_tmp))
		{
			HatchPoints.resize(4);
			HatchPoints[0] = tray_start - width_dir * tray_width * 0.5;
			HatchPoints[1] = tray_end - width_dir * tray_width * 0.5;
			HatchPoints[2] = tray_end + width_dir * tray_width * 0.5; 
			HatchPoints[3] = tray_start + width_dir * tray_width * 0.5;
			
			if(sViewDir == _T("TOP"))
			{
				if(vec_st.m_dz > 0)
				{
					int_file << _T("<LINE>|") << HatchPoints[0].x() << _T(",") << HatchPoints[0].y() << _T(",") << 0.0;
					int_file << _T(",") << HatchPoints[2].x() << _T(",") << HatchPoints[2].y() << _T(",") << 0.0 ;
					int_file << _T("|0,0,0,1,1") << std::endl;

					int_file << _T("<LINE>|") << HatchPoints[1].x() << _T(",") << HatchPoints[1].y() << _T(",") << 0.0;
					int_file << _T(",") << HatchPoints[3].m_x << _T(",") << HatchPoints[3].m_y << _T(",") << 0.0 ;
					int_file << _T("|0,0,0,1,1") << std::endl;
				}
				else if(vec_st.m_dz < 0)
				{
					CIsPoint3d basePnt;
					basePnt = HatchPoints[0] + (HatchPoints[2] - HatchPoints[0]) * 0.5;

					int_file << _T("<DONUT>|");
					int_file << 0 << _T(",");
					int_file << 0.5 << _T(",");
					int_file << basePnt.x();
					int_file << _T(",") << basePnt.y();
					int_file << _T("|0,1") << std::endl;
				}
			}
			else if(sViewDir == _T("BOTTOM"))
			{
				if(vec_st.dz() < 0)
				{
					int_file << _T("<LINE>|") << HatchPoints[0].x() << _T(",") << HatchPoints[0].y() << _T(",") << 0.0;
					int_file << _T(",") << HatchPoints[2].x() << _T(",") << HatchPoints[2].y() << _T(",") << 0.0 ;
					int_file << _T("|0,0,0,1,1") << std::endl;

					int_file << _T("<LINE>|") << HatchPoints[1].x() << _T(",") << HatchPoints[1].y() << _T(",") << 0.0;
					int_file << _T(",") << HatchPoints[3].x() << _T(",") << HatchPoints[3].y() << _T(",") << 0.0 ;
					int_file << _T("|0,0,0,1,1") << std::endl;
				}
				else if(vec_st.dz() > 0)
				{
					CIsPoint3d basePnt;
					basePnt = HatchPoints[0] + (HatchPoints[2] - HatchPoints[0]) * 0.5;

					int_file << _T("<DONUT>|");
					int_file << 0 << _T(",");
					int_file << 0.5 << _T(",");
					int_file << basePnt.x();
					int_file << _T(",") << basePnt.y();
					int_file << _T("|0,1") << std::endl;
				}
			}
		}
		else
		{
			double nRadius[2] = {0.f,};
			HatchPoints.resize(10);
			
			nRadius[0] = tray_mid.DistanceTo(tray_start);
			CIsPoint3d tmpPnt = tray_start - (height_dir * tray_height);
			nRadius[1] = tray_mid.DistanceTo(tmpPnt);

			HatchPoints[0] = tray_mid + (vec_st * nRadius[0]);
			HatchPoints[9] = tray_mid + (vec_st * nRadius[1]);

			HatchPoints[4] = tray_mid + (vec_ed * nRadius[0]);
			HatchPoints[5] = tray_mid + (vec_ed * nRadius[1]);

			HatchPoints[2] = this->GetMiddlePoint(HatchPoints[0] , HatchPoints[4] , tray_mid , nRadius[0]);
			HatchPoints[1] = this->GetMiddlePoint(HatchPoints[0] , HatchPoints[2] , tray_mid , nRadius[0]);
			HatchPoints[3] = this->GetMiddlePoint(HatchPoints[2] , HatchPoints[4] , tray_mid , nRadius[0]);

			HatchPoints[7] = this->GetMiddlePoint(HatchPoints[9] , HatchPoints[5] , tray_mid , nRadius[1]);
			HatchPoints[8] = this->GetMiddlePoint(HatchPoints[9] , HatchPoints[7] , tray_mid , nRadius[1]);
			HatchPoints[6] = this->GetMiddlePoint(HatchPoints[7] , HatchPoints[5] , tray_mid , nRadius[1]);
		}
	}
	
	if(!HatchPoints.empty())
	{
		//! clip shape by view
		for(int i = 0;i < int(HatchPoints.size());++i)
		{
			input.push_back(CIsPoint2d(HatchPoints[i].x() , HatchPoints[i].y()));
		}

		clipPolygon.push_back(CIsPoint2d(m_pView->origin().x() , m_pView->origin().y()));
		clipPolygon.push_back(CIsPoint2d(m_pView->origin().x() + m_pView->GetViewWidth() , m_pView->origin().y()));
		clipPolygon.push_back(CIsPoint2d(m_pView->origin().x() + m_pView->GetViewWidth() , m_pView->origin().y() + m_pView->GetViewHeight()));
		clipPolygon.push_back(CIsPoint2d(m_pView->origin().x() , m_pView->origin().y() + m_pView->GetViewHeight()));
		clipPolygon.push_back(CIsPoint2d(m_pView->origin().x() , m_pView->origin().y()));
		SutherlandHodgmanPolygonClip(input , clipPolygon , output);

		if(!output.empty())
		{
			STRING_T sCellName;
			STRING_T sCellFilePath = _T("NULL");
			STRING_T sHatchColor;

			UINT uiOutLineWeight = 0;
			bool bOutLineOnOff = true;

			if(_T("INST") == m_sDepartmentType)
			{
				sCellName = env.m_cableT.m_sInstCableT_CellName;
				sHatchColor = env.m_cableT.m_sInstCableT_HatchColor;
				if(sCellName != "NULL") sCellFilePath = env.m_cableT.m_sCellFilePath;

				uiOutLineWeight = env.m_cableT.m_uiInstCableT_OutLine_Weight;
				if(env.m_cableT.m_sInstCableT_OutLine_OnOff == _T("ON")) bOutLineOnOff = true;
				else bOutLineOnOff = false;
			}
			else
			{
				sCellName = env.m_cableT.m_sElecCableT_CellName;
				sHatchColor = env.m_cableT.m_sElecCableT_HatchColor;
				if(sCellName != "NULL") sCellFilePath = env.m_cableT.m_sCellFilePath;

				uiOutLineWeight = env.m_cableT.m_uiElecCableT_OutLine_Weight;
				if(env.m_cableT.m_sElecCableT_OutLine_OnOff == _T("ON")) bOutLineOnOff = true;
				else bOutLineOnOff = false;
			}

			int_file << _T("<HATCHPLINE>|");
			for(vector<CIsPoint2d>::iterator itr = output.begin();itr != output.end();++itr)
			{	
				int_file << itr->x() << _T(",") << itr->y() << _T(",");
			}
			int_file << output.front().x() << _T(",") << output.front().y();
			int_file << _T("|") << sHatchColor << _T(",1,") << sCellFilePath << _T(",") << sCellName << _T(",45,1,Yes") << std::endl;

			//Out Line Drawing

			if(bOutLineOnOff)
			{
				int nLineStyle;
				if(sViewDir == _T("TOP") || sViewDir == _T("BOTTOM")) nLineStyle = 0;
				else nLineStyle = 0;

				int_file << _T("<PLINE>|");
				int_file << output[0].x() << _T(",") << output[0].y();
				for(int i = 1;i < output.size()*0.5 ;++i)
				{
					int_file << _T(",") << output[i].x() << _T(",") << output[i].y();
				}
				int_file << _T("|") << sHatchColor << _T(",1,") << nLineStyle << _T(",1,") << uiOutLineWeight << std::endl;

				int_file << _T("<PLINE>|");
				int_file << output[int(output.size()*0.5)].x() << _T(",") << output[int(output.size()*0.5)].y();
				for(int i = int(output.size()*0.5) + 1;i < int(output.size());++i)
				{
					int_file << _T(",") << output[i].x() << _T(",") << output[i].y();
				}
				int_file << _T("|") << sHatchColor << _T(",1,") << nLineStyle << _T(",1,") << uiOutLineWeight << std::endl;
			}
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-23
    @class      CCableOutputData
    @function   IsVisible
    @return     bool
    @param      const   CIsVect3d&
    @param      rhs
    @brief
******************************************************************************/
bool CCableOutputData::IsVisible(const CIsVect3d& rhs)
{
	CIsVect3d dir(0.0 , 0.0 , 1.0);

	const double nDot = fabs(dir.DotProduct(rhs));
	return (!((1.f - nDot) < DBL_EPSILON));
}

/**
	@brief	destructor

	@author	

	@date	2010.10.28
*/
CCableTrayRoute::~CCableTrayRoute()
{
	try
	{
		for(list<CCableTrayGroup*>::iterator itr = m_oCableGroupList.begin();itr != m_oCableGroupList.end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_oCableGroupList.clear();
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CCableTrayRoute
    @function   GetView
    @return     CaDraw_View*
    @brief
******************************************************************************/
CaDraw_View* CCableTrayRoute::GetView() const
{
	if(!m_oCableTrayList.empty())
	{
		return (m_oCableTrayList.front())->GetView();
	}

	return NULL;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-21
    @class      CCableTrayRoute
    @function   IsPossibleToBeMember
    @return     bool
    @param      CaDraw_TagCableTray&    cable
    @brief		주어진 cable이 기존의 cable route의 새로운 멤버가 될수 있는지 검사한다
******************************************************************************/
bool CCableTrayRoute::IsPossibleToBeMember(CaDraw_TagCableTray* pCable)
{
	assert(pCable && "pCable is NULL");

	if(pCable)
	{
		if(m_oCableTrayList.empty())
		{
			return true;
		}
		else
		{
			for (list<CaDraw_TagCableTray*>::iterator itr = m_oCableTrayList.begin(); itr != m_oCableTrayList.end(); ++itr)
			{
				//! 연결이 되지는 여부를 검사한다.
				if((*itr)->CanBeConnected(pCable)) return true;
			}
		}
	}

	return false;
}

/**
	@brief	Cable Tray Route에서 Tag할 Cable Tray들을 구한다.

	@author	humkyung

	@date	2010.09.16

	@return
*/
bool CCableTrayRoute::GetTaggingCableTray(vector<CaDraw_TagCableTray*>& oCableTrayList)
{
	if(!m_oCableGroupList.empty())
	{
		CaDraw_TagCableTray* res = NULL;	
		for(list<CCableTrayGroup*>::iterator itr = m_oCableGroupList.begin();itr != m_oCableGroupList.end();++itr)
		{
			//! 이미 태깅한 CableTrayGroup이면 건너 뛴다.
			if(true == (*itr)->m_bTag) continue;
			
			//! CableTrayGroup에서 Tagging할 CableTray를 구한다.
			res = (*itr)->GetTaggingCableTray();
			if(NULL != res)
			{
				(*itr)->m_bTag = true;
				oCableTrayList.push_back(res);
			}
		}

		return true;
	}

	return false;
}

/**
	@brief	Cable Tray Route에서 grouping이 가능한 Cable Tray들을 구한다.

	@author	HumKyung.BAEK

	@date	2010.09.17

	@return
*/
bool CCableTrayRoute::GetCableTrayToBeGroupingOn(vector<CaDraw_TagCableTray*>& CableTrayList , CaDraw_TagCableTray* pCableT , const double& dElev)
{
	for(list<CCableTrayGroup*>::iterator itr = m_oCableGroupList.begin();itr != m_oCableGroupList.end();++itr)
	{
		(*itr)->GetCableTrayToBeGroupingOn(CableTrayList , pCableT , dElev);
	}

	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CCableTrayRoute
    @function   AddData
    @return     int
    @param      CaDraw_TagCableTray*    pCableTray
    @brief
******************************************************************************/
int CCableTrayRoute::AddData(CaDraw_TagCableTray* pCableTray)
{
	assert(pCableTray && "pCableTray is NULL");
	
	if(pCableTray)
	{
		m_oCableTrayList.push_back(pCableTray);
		return ERROR_SUCCESS;
	}
	
	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CCableTrayRoute
    @function   SetCableHatchData
    @return     int
    @param      View_Par&                   View
    @param      vector<CCableOutputData>&   CableHatchOutputList
    @brief
******************************************************************************/
int CCableTrayRoute::SetCableHatchData(CaDraw_View* pView , vector<CCableOutputData>& CableHatchOutputList)
{
	CCableOutputData oOutputData;
	for(list<CaDraw_TagCableTray*>::iterator itr = m_oCableTrayList.begin(); itr != m_oCableTrayList.end();++itr)
	{
		/// Cable_input data를 Cable_Output Data로 넣는다.(대입)
		oOutputData.tray_start = (*itr)->GetStartPoint();
		oOutputData.tray_end   = (*itr)->GetEndPoint();
		oOutputData.len_dir    = (*itr)->lengthDir();
		oOutputData.width_dir  = (*itr)->widthDir();
		oOutputData.height_dir = (*itr)->heightDir();
		oOutputData.m_sType	 = (*itr)->m_sType;
		oOutputData.m_sSubType = (*itr)->m_sSubType;
		oOutputData.m_pView = pView;
		oOutputData.m_nWidth   = (*itr)->m_nWidth;
		oOutputData.m_nHeight  = (*itr)->m_nHeight;
		oOutputData.m_sDepartmentType = (*itr)->m_sDepartmentType;
			
		if (_T("ELBOW") == oOutputData.m_sType) 
		{
			oOutputData.m_sSubType = (*itr)->m_sSubType;
			oOutputData.m_nItemAng = (*itr)->m_nItemAng;
			oOutputData.tray_mid   = (*itr)->GetMidPoint();
		}
		else if (_T("TEE") == oOutputData.m_sType)
		{
			oOutputData.m_sSubType = (*itr)->m_sSubType;
			oOutputData.m_nWidth2  = (*itr)->m_nWidth2;
			oOutputData.tray_mid   = (*itr)->GetMidPoint();
		}
		else if (_T("REDUCE") == oOutputData.m_sType)
		{
			oOutputData.m_sSubType       = (*itr)->m_sSubType;
			oOutputData.m_nWidth2        = (*itr)->m_nWidth2;
			oOutputData.m_oReducePntList.insert(oOutputData.m_oReducePntList.begin() , (*itr)->m_pReducePntList->begin() , (*itr)->m_pReducePntList->end());
		}

		//! Hatch Data만 별도로 생성
		CableHatchOutputList.push_back(oOutputData);
	}

	return ERROR_SUCCESS;
}

//101209 KM Fitting류 Item Tagging
int CCableTrayRoute::SetCableFittingData(CaDraw_View* pView , vector<CCableOutputData>& CableOutputList/*, vector<Cell_List>& C_list, Cabl_Option CA_opt*/)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	const double vScale = CCableTrayInternalTerritory::m_pCellMapExp->scale();

	for(list<CaDraw_TagCableTray*>::iterator itr = m_oCableTrayList.begin(); itr != m_oCableTrayList.end();++itr)
	{
		////////////////////////////////////////////////////////
		CaDraw_TagCableTray* Ci_iter = (*itr);
		const STRING_T sDepartment = Ci_iter->m_sDepartmentType;

		if(_T("STRAIGHT") == (*itr)->m_sType) continue;

		CCableOutputData res;
		CCableMidData oMidData;
		vector<CCableMidData> oMidDataList;

		CIsPoint2d tag_po, m_cen;
		double m_len = 0.0;
		int PR_cnt, m_cnt;

		res.tag = false;
		res.line_2 = false;  // NO XY LINE2

		double nBoxHeight = env.m_cableT.m_dRowHeight * env.m_cableT.m_iRowCount;
		int nCellHeight = int(ceil(nBoxHeight * vScale)); 
		int nBOT_EL;
		TCHAR sBot_el[100], sTagName2[100], sTraySizeTmp[100];
		STRING_T sTot_BotEL;

		//! Cable Tray Size(WxH) List를 하나의 Vector에 담아둠(Tagging 할때 표기)
		res.m_oTraySizeList.clear();

		sprintf(sTraySizeTmp, _T("W%dxH%d") , int(Ci_iter->m_nWidth), int(Ci_iter->m_nHeight)); //string으로 변환
		res.m_oTraySizeList.push_back(sTraySizeTmp);
		res.m_oIntPtList.push_back(Ci_iter->m_ptCenter);

		//! Cable Tag Item의 모든 Text중에서 가장 긴 것의 길이를 BoxWidth로 한다.
		res.GetFittingTagName(Ci_iter); //tag_name1 
		sprintf(sTagName2, _T("W%dxH%d") , int(Ci_iter->m_nWidth), int(Ci_iter->m_nHeight)); //string으로 변환
		res.tag_name2 = sTagName2;
		//prh파일에서 받아온 z값중에 작은 것을 사용
		nBOT_EL = int(Ci_iter->GetElevation());
		sprintf(sBot_el, _T(" %d") , nBOT_EL); //string으로 변환

		//Prefix와 BOT EL을 하나의 스트링으로.. (Text 길이를 구하기 위해)
		if(_T("ELEC") == sDepartment)
		{
			sTot_BotEL = env.m_cableT.m_sElecCableT_EL_Prefix + sBot_el;
		}
		else
		{
			sTot_BotEL = env.m_cableT.m_sInstCableT_EL_Prefix + sBot_el;
		}

		double nBoxWidth[3];
		double nBoxWidth_Result = 0.;

		/// Cable Tag 중에서 길이가 가장 긴 Tag를 이용하여 Box의 Point를 지정해줌
		nBoxWidth[0] = env.m_cableT.m_oLabelTextStyle.GetTextLength(res.tag_name1);
		nBoxWidth[1] = env.m_cableT.m_oLabelTextStyle.GetTextLength(res.tag_name2);
		nBoxWidth[2] = env.m_cableT.m_oLabelTextStyle.GetTextLength(sTot_BotEL);
		if (env.m_cableT.m_iRowCount <= 2) nBoxWidth[2] = 0L;

		int nBoxMax = 0;
		for (int nCnt = 0; nCnt <= 2; nCnt++)
		{
			if(nBoxWidth[nBoxMax] < nBoxWidth[nCnt]) 
				nBoxMax = nCnt;
		}

		nBoxWidth_Result = nBoxWidth[nBoxMax] + (nBoxWidth[nBoxMax] * 0.2);

		int nCellWidth = int(ceil(nBoxWidth_Result * vScale)); //Box의 width를 Cell 단위로 변환

		m_cen = Ci_iter->m_ptCenter;	//Cable Tray의 Center           
		m_len = Ci_iter->length();		//Cable Tray의 Length	
		///W_cnt = 0;
		m_cnt = int(ceil(m_len * vScale));	//Cable Tray의 Length를 Cell 단위로 변환
		if (m_cnt == 0) m_cnt = 2;
		PR_cnt = 0;   

		/// 찾을 영역을 구한다.			
		oMidData.insert_point = m_cen;
		if (oMidData.insert_point.x() < pView->origin().x())
			oMidData.insert_point.SetX(pView->origin().x());
		else if (oMidData.insert_point.x() > (pView->origin().x() + pView->GetViewWidth()))
			oMidData.insert_point.SetX(pView->origin().x() + pView->GetViewWidth());

		if (oMidData.insert_point.y() < pView->origin().y())
			oMidData.insert_point.SetY(pView->origin().y());
		else if (oMidData.insert_point.y() > (pView->origin().y() + pView->GetViewHeight()))
			oMidData.insert_point.SetY((pView->origin().y() + pView->GetViewHeight()));

		//Insert Point 가 View의 범위를 벗어 날경우.. View의 가장자리로 이동.
		for (vector<CIsPoint2d>::iterator itr = res.m_oIntPtList.begin(); itr != res.m_oIntPtList.end(); ++itr)
		{
			if (itr->x() < pView->origin().x())
				itr->SetX(pView->origin().x());
			else if (itr->x() > (pView->origin().x() + pView->GetViewWidth()))
				itr->SetX((pView->origin().x() + pView->GetViewWidth()));

			if (itr->y() < pView->origin().y())
				itr->SetY(pView->origin().y());
			else if (itr->y() > (pView->origin().y() + pView->GetViewHeight()))
				itr->SetY((pView->origin().y() + pView->GetViewHeight()));
		}

		CCellMap::CellIndex cIndex = CCableTrayInternalTerritory::m_pCellMapExp->PointToIndex(m_cen); //!  그래픽 좌표를 CELL 좌표로 변환시킨다.
		//m_Cnt
		int st_x = int(cIndex.x - nBoxWidth_Result * 5);//floor(double(View.m_nWidth) / 2.0);			
		int ed_x = int(cIndex.x + nBoxWidth_Result * 5 - nCellWidth);//floor(double(View.m_nWidth) / 2.0) - nCellWidth;
		int st_y = int(cIndex.y - nBoxHeight * 10);//floor(double(View.m_nHeight) / 2.0);
		int ed_y = int(cIndex.y + nBoxHeight * 10 - nCellHeight);//floor(double(View.m_nHeight) / 2.0) - nCellHeight;

		st_x = (st_x < 0) ? 0 : st_x;
		ed_x = (ed_x >= (int(CCableTrayInternalTerritory::m_pCellMapExp->xlen()) - nCellWidth)) ? ((CCableTrayInternalTerritory::m_pCellMapExp->xlen() - nCellWidth) - 1) : ed_x;
		st_y = (st_y < 0) ? 0 : st_y;
		ed_y = (ed_y >= (int(CCableTrayInternalTerritory::m_pCellMapExp->ylen()) - nCellHeight))? ((CCableTrayInternalTerritory::m_pCellMapExp->ylen() - nCellHeight) - 1) : ed_y;
		//!

		int K = 0;
		if (!(st_x < 0) && !(ed_x >= int(CCableTrayInternalTerritory::m_pCellMapExp->xlen())))
		{
			if ((ed_y - st_y) > 0) 
			{
				for (int J = st_y; J <= ed_y; ++J)
				{
					for (int I = st_x; I <= ed_x; ++I)
					{
						K = 0;
						/// Tag Item의 영역
						for (int II = I; II <= (I + nCellWidth -1); ++II)
						{
							for (int JJ = J ; JJ <= (J + nCellHeight - 1); ++JJ)
							{
								if (CCableTrayInternalTerritory::m_pCellMapExp->Value(II , JJ) != 0) K++;
							}
						}

						tag_po = CCableTrayInternalTerritory::m_pCellMapExp->IndexToPoint(CCellMap::CellIndex(I , J));
						oMidData.cnt = K;
						oMidData.PRI(tag_po, nBoxWidth_Result , nBoxHeight);
						oMidDataList.push_back(oMidData);				
						PR_cnt++;
					}
				}
			}
		}

		//! 무수히 많은 중간 값들 중에서 알맞은 값을 구한다.
		if (PR_cnt > 0)
		{
			stable_sort(oMidDataList.begin(), oMidDataList.end(), C_Len_Sort);  // LEN 으로 SORT
			stable_sort(oMidDataList.begin(), oMidDataList.end(), C_Cnt_Sort);  // TAG CNT 으로 SORT
			oMidData = oMidDataList.front();

			res.xdir = oMidData.x_dir;

			res.insert_point = oMidData.insert_point;
			res.m_tag_point  = oMidData.tag_point;
			res.m_nTagWidth  = nBoxWidth_Result;
			res.m_nTagHeight = nBoxHeight;
			res.m_nWidth     = Ci_iter->m_nWidth;
			res.m_nHeight    = Ci_iter->m_nHeight;
			res.m_pView      = pView;
			res.bot_el       = sTot_BotEL;
			res.tray_start   = Ci_iter->GetStartPoint();
			res.tray_end     = Ci_iter->GetEndPoint();
			res.width_dir    = Ci_iter->widthDir();
			res.height_dir   = Ci_iter->heightDir();
			res.m_sType	 = Ci_iter->m_sType;
			res.m_sSubType   = Ci_iter->m_sSubType;
			res.m_sDepartmentType = Ci_iter->m_sDepartmentType;

			if (res.m_sType == _T("ELBOW")) 
			{
				res.m_sSubType = Ci_iter->m_sSubType;
				res.m_nItemAng = Ci_iter->m_nItemAng;
				res.tray_mid   = Ci_iter->GetMidPoint();
			}
			else if (res.m_sType == _T("TEE")) 
			{
				res.m_sSubType = Ci_iter->m_sSubType;
				res.m_nWidth2 = Ci_iter->m_nWidth2;
				res.tray_mid   = Ci_iter->GetMidPoint();
			}
			else if (res.m_sType == _T("REDUCE"))
			{
				res.m_sSubType     = Ci_iter->m_sSubType;
				res.m_nWidth2 = Ci_iter->m_nWidth2;
				
				res.m_oReducePntList.clear();
				res.m_oReducePntList.insert(res.m_oReducePntList.end() , Ci_iter->m_pReducePntList->begin() , Ci_iter->m_pReducePntList->end());
			}

			res.line2_point = oMidData.ed_point;
			res.OUTPUT_DATA(oMidData.tag_point, env.m_cableT.m_oLabelTextStyle.m_nHeight , env.m_cableT.m_oLabelTextStyle.m_nHeight , 
				nBoxWidth_Result, env.m_cableT.m_dRowHeight);

			/// occupy cell
			CIsPoint2d minPnt(res.m_tag_point);
			CIsPoint2d maxPnt(res.m_tag_point);
			maxPnt.SetX( maxPnt.x() + nBoxWidth_Result);
			maxPnt.SetY( maxPnt.y() + nBoxHeight);
			CCableTrayInternalTerritory::m_pCellMapExp->UpdateRectOf(minPnt , maxPnt , 13);

			oMidDataList.clear();
			res.tag = true;

			CableOutputList.push_back(res);
		}
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-23
    @class      CCableTrayRoute
    @function   SetCableUpDownMarkData
    @return     int
    @param      CaDraw_View*                pView
    @param      vector<CCableOutputData>&   CableUpDownOutputList
    @brief
******************************************************************************/
int CCableTrayRoute::SetCableUpDownMarkData(CaDraw_View* pView , vector<CCableOutputData>& CableUpDownOutputList)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	CaDraw_TagCableTray* previous = NULL;
	CCableOutputData res;
	bool bstPnt = true;
	bool bprestPnt = true;

	double nCurElev = m_oCableTrayList.front()->GetElevation();

	for(list<CaDraw_TagCableTray*>::iterator itr = m_oCableTrayList.begin(); itr != m_oCableTrayList.end();++itr)
	{
		if((*itr)->m_sType != _T("STRAIGHT") || ((*itr)->lengthDir().dz() != 0.)) continue;
		if((NULL != previous) && (nCurElev != (*itr)->GetElevation()))
		{
			CIsPoint2d stPnt, edPnt, tagPnt, pre_stPnt, pre_edPnt, pre_tagPnt;

			stPnt.SetX( (*itr)->GetStartPoint().x() );
			stPnt.SetY( (*itr)->GetStartPoint().y() );
			edPnt.SetX( (*itr)->GetEndPoint().x() );
			edPnt.SetY( (*itr)->GetEndPoint().y() );
			
			pre_stPnt.SetX( previous->GetStartPoint().x() );
			pre_stPnt.SetY( previous->GetStartPoint().y() );
			pre_edPnt.SetX( previous->GetEndPoint().x() );
			pre_edPnt.SetY( previous->GetEndPoint().y() );
			
			bstPnt = true;
			bprestPnt = true;

			if(stPnt.DistanceTo(pre_stPnt) <= edPnt.DistanceTo(pre_stPnt))	
				tagPnt = stPnt;
			else	
			{
				tagPnt = edPnt;
				bstPnt = false;
			}
			
			if(pre_stPnt.DistanceTo(stPnt) <= pre_edPnt.DistanceTo(stPnt))	
				pre_tagPnt = pre_stPnt;
			else	
			{
				pre_tagPnt = pre_edPnt;
				bprestPnt = false;
			}
			
			CIsPoint3d tmpStPnt, tmpEdPnt;
			if(previous->GetElevation() <= (*itr)->GetElevation())
			{
				tagPnt = pre_tagPnt;
				if(!bprestPnt)	
				{
					tmpStPnt.Set(pre_edPnt.x() , pre_edPnt.y() , 0.0);
					tmpEdPnt.Set(pre_stPnt.x() , pre_stPnt.y() , 0.0);
				}
				else  
				{
					tmpStPnt.Set(pre_stPnt.x() , pre_stPnt.y() , 0.0);
					tmpEdPnt.Set(pre_edPnt.x() , pre_edPnt.y() , 0.0);
				}
				{
					OSTRINGSTREAM_T oss;
					oss << previous->GetElevation();
					res.bot_el = oss.str();
				}
			}
			else 
			{
				if(!bstPnt)  
				{
					tmpStPnt.Set(edPnt.x() , edPnt.y() , 0.0);
					tmpEdPnt.Set(stPnt.x() , stPnt.y() , 0.0);
				}
				else  
				{
					tmpStPnt.Set(stPnt.x() , stPnt.y() , 0.0);
					tmpEdPnt.Set(edPnt.x() , edPnt.y() , 0.0);
				}
				{
					OSTRINGSTREAM_T oss;
					oss << (*itr)->GetElevation();
					res.bot_el = oss.str();
				}
			}

			CIsVect3d lengthDir = tmpEdPnt - tmpStPnt;
			const double length = lengthDir.Length();
			if(length > 0)
			{
				res.len_dir = lengthDir;
				res.len_dir.Normalize();

				const double nTmp = res.len_dir.dy() / sqrt(pow(res.len_dir.dx() , 2.0) + pow(res.len_dir.dy() , 2.0)) ;
				const double nRadian = asin(nTmp);
				double nTmpAng = RAD2DEG(nRadian);
				
				if(nTmpAng > 90.0 && nTmpAng <= 180.0) nTmpAng -= 180.0;
				else if(nTmpAng <= -90.0 && nTmpAng >= -180.0) nTmpAng += 180.0;

				res.m_nItemAng = nTmpAng;
			}
			else
			{
				res.m_nItemAng = 0;
			}

			STRING_T sTagName;
			TCHAR sTmp[100];
			
			sprintf(sTmp, _T("UP%d") , int(fabs((*itr)->GetElevation() - previous->GetElevation()))); //string으로 변환 

			sTagName = sTmp;

			res.insert_point = (*itr)->m_ptCenter;
			res.tag_name1    = sTagName;
			res.m_tag_point  = tagPnt;
			res.m_nTagWidth  = env.m_cableT.m_oLabelTextStyle.GetTextLength(res.tag_name1);
			res.m_nTagHeight = env.m_cableT.m_oLabelTextStyle.m_nHeight;
			res.m_pView      = pView;
			res.tray_start   = (*itr)->GetStartPoint();
			res.tray_end     = (*itr)->GetEndPoint();
			res.width_dir    = (*itr)->widthDir();
			res.height_dir   = (*itr)->heightDir();
			res.m_sType	     = (*itr)->m_sType;
			res.m_sSubType   = (*itr)->m_sSubType;
			res.m_sDepartmentType = (*itr)->m_sDepartmentType;
			
			if(int(previous->GetElevation()) != 0)
				CableUpDownOutputList.push_back(res);

			nCurElev = (*itr)->GetElevation();
		}

		previous = (*itr);
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CCableTrayRoute
    @function   SetCableISOData
    @return     int
    @param      CaDraw_View*                pView
    @param      vector<CCableOutputData>&   CableOutputList
    @brief
******************************************************************************/
int CCableTrayRoute::SetCableISOData(CaDraw_View* pView , vector<CCableOutputData>& CableOutputList)
{
	CCableOutputData oOutputData;

	/*oOutputData.tray_start = m_CableTrayList.begin()->vert_start;
	oOutputData.tray_end   = m_CableTrayList.begin()->vert_end;
	oOutputData.width_dir  = m_CableTrayList.begin()->width_dir;
	oOutputData.height_dir = m_CableTrayList.begin()->height_dir;
	oOutputData.m_sType	 = m_CableTrayList.begin()->m_sType;
	oOutputData.m_sSubType = m_CableTrayList.begin()->m_sSubType;
	*/
	oOutputData.m_pView     = pView;
	/*oOutputData.len_dir    = m_CableTrayList.begin()->len_dir;
	oOutputData.height_dir = m_CableTrayList.begin()->height_dir;
	oOutputData.m_nWidth   = m_CableTrayList.begin()->m_nWidth;
	oOutputData.m_nHeight  = m_CableTrayList.begin()->m_nHeight;

	if (oOutputData.m_sType == "ELBOW") 
	{
		oOutputData.m_sSubType = m_CableTrayList.begin()->m_sSubType;
		oOutputData.m_nItemAng = m_CableTrayList.begin()->m_nItemAng;
		oOutputData.tray_mid   = m_CableTrayList.begin()->vert_mid;
	}
	else if (oOutputData.m_sType == "TEE") 
	{
		oOutputData.m_sSubType = m_CableTrayList.begin()->m_sSubType;
		oOutputData.m_nWidth2  = m_CableTrayList.begin()->m_nWidth2;
		oOutputData.tray_mid   = m_CableTrayList.begin()->vert_mid;
	}
	else if (oOutputData.m_sType == "REDUCE")
	{
		oOutputData.m_sSubType     = m_CableTrayList.begin()->m_sSubType;
		oOutputData.m_nWidth2      = m_CableTrayList.begin()->m_nWidth2;
		oOutputData.m_oReducePntList= m_CableTrayList.begin()->m_oReducePntList;
	}

	CableOutputList.push_back(oOutputData);*/

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CCableTrayRoute
    @function   CreateCableGroup
    @return     int
    @brief		cable route에서 같은 elevation을 가지는 cable tray들을 묶어 하나의 group으로 만든다.
******************************************************************************/
int CCableTrayRoute::CreateCableGroup()
{
	//ofstream test_file("c://CableRoute.txt");
	if(m_oCableTrayList.empty()) return ERROR_SUCCESS;

	CCableTrayGroup* pCableTrayGroup = new CCableTrayGroup;
	double nChkElev = (*(m_oCableTrayList.begin()))->GetElevation();

	for(list<CaDraw_TagCableTray*>::iterator itr = m_oCableTrayList.begin(); itr != m_oCableTrayList.end(); ++itr)
	{
		if((nChkElev == (*itr)->GetElevation()) && (0.0 == (*itr)->lengthDir().dz()))
		{
			pCableTrayGroup->Add(*itr);
		}
		else
		{
			m_oCableGroupList.push_back(pCableTrayGroup);

			nChkElev = (*itr)->GetElevation();
			pCableTrayGroup = new CCableTrayGroup;
			pCableTrayGroup->Add(*itr);
		}
		//test_file << "Type = " << (*itr).m_sType << std::endl;
		//test_file << "Elev = " << (*itr).m_nElevation << std::endl;
		//test_file << "st_x = " << (*itr).vert_start.m_x;
		//test_file << "st_y = " << (*itr).vert_start.m_y;
		//test_file << "st_z = " << (*itr).vert_start.m_z << std::endl;
		//test_file << "ed_x = " << (*itr).vert_end.m_x;
		//test_file << "ed_y = " << (*itr).vert_end.m_y;
		//test_file << "ed_z = " << (*itr).vert_end.m_z << std::endl;
		//test_file << "LenDirZ = " << (*itr).len_dir.m_z << std::endl;
	}
	if(!(pCableTrayGroup->m_oCableTrayRefList).empty())
		m_oCableGroupList.push_back(pCableTrayGroup);
	else
	{
		delete pCableTrayGroup;
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CCableTrayRoute
    @function   CableRoute_Sort
    @return     int
    @brief		connection point에 따라 cable route내에 있는 cable tray를 소팅한다.
******************************************************************************/
int CCableTrayRoute::CableRoute_Sort()
{
	list<CaDraw_TagCableTray*> oCableTrayList;
	CIsPoint3d frontPnt, backPnt;
	int nCnt = 0;

	/// Front Point Sort
	bool bLoop = false;
	do
	{
		for(list<CaDraw_TagCableTray*>::iterator itr = m_oCableTrayList.begin(); itr != m_oCableTrayList.end(); ++itr)
		{	
			bLoop = false;
			nCnt = 0;
			for(vector<CIsPoint3d>::iterator jtr = (*itr)->m_pConnPointList->begin();jtr != (*itr)->m_pConnPointList->end();++jtr)
			{
				if(oCableTrayList.empty()) 
				{
					oCableTrayList.push_back(*itr);
					frontPnt = *jtr;
					backPnt  = *(++jtr);
					m_oCableTrayList.erase(itr);
					bLoop = true;
					break;
				}

				if(frontPnt == (*jtr)) 
				{
					if(nCnt == 0)	frontPnt = *(++jtr);
					else		frontPnt = *((*itr)->m_pConnPointList->begin());

					oCableTrayList.push_front(*itr);
					m_oCableTrayList.erase(itr);
					bLoop = true;
					break;
				}
				nCnt++;
			}
			if(bLoop) break;
		}
		if(0 == m_oCableTrayList.size()) bLoop = false;
	}while(bLoop);

	//Back Point Sort
	bLoop = false;
	do
	{
		for(list<CaDraw_TagCableTray*>::iterator itr = m_oCableTrayList.begin(); itr != m_oCableTrayList.end(); ++itr)
		{	
			bLoop = false;
			nCnt = 0;
			for(vector<CIsPoint3d>::iterator jtr = (*itr)->m_pConnPointList->begin();jtr != (*itr)->m_pConnPointList->end();++jtr)
			{
				if(oCableTrayList.empty()) 
				{
					oCableTrayList.push_back(*itr);
					frontPnt = *jtr;
					backPnt  = *(++jtr);
					m_oCableTrayList.erase(itr);
					bLoop = true;
					break;
				}

				if(backPnt == (*jtr)) 
				{
					if(nCnt == 0)	backPnt = *(++jtr);
					else		backPnt = *((*itr)->m_pConnPointList->begin());
					
					oCableTrayList.push_back(*itr);
					m_oCableTrayList.erase(itr);
					bLoop = true;

					break;
				}
				nCnt++;
			}
			if(bLoop) break;
		}
		if(0 == m_oCableTrayList.size()) bLoop = false;
	}while(bLoop);

	m_oCableTrayList.clear();
	m_oCableTrayList.insert(m_oCableTrayList.begin() , oCableTrayList.begin() , oCableTrayList.end());

	return ERROR_SUCCESS;
}

void CCableOutputData::WriteBOT_ForISO(OFSTREAM_T& int_file)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	int_file << _T("<LINE>|");
	int_file << tray_start.x() << _T(",");
	int_file << tray_start.y() << _T(",");
	int_file << 0 << _T(",");
	int_file << tray_end.x() << _T(",");
	int_file << tray_end.y() << _T(",");
	int_file << 11.935 << _T("|");
	int_file << env.m_cableT.sLeader_Color << _T(",0,0,1,1") << std::endl;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-23
    @class      CCableOutputData
    @function   WriteUpDownMark
    @return     void
    @param      OFSTREAM_T& int_file
    @brief
******************************************************************************/
void CCableOutputData::WriteUpDownMark(OFSTREAM_T& ofile)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	const STRING_T sJustify = _T("Center Center");

	const double dTextHeight = env.m_cableT.m_oLabelTextStyle.m_nHeight;
	m_nTagWidth  = env.m_cableT.m_oLabelTextStyle.GetTextLength(tag_name1);

	CIsPoint2d textPnt;
	textPnt.SetX(m_tag_point.x() + (len_dir.dx() * ((m_nTagWidth*0.5 + dTextHeight*1.5))));
	textPnt.SetY(m_tag_point.y() + (len_dir.dy() * ((m_nTagWidth*0.5 + dTextHeight*1.5))));

	ofile << _T("<TEXT>|");
	ofile << textPnt.x() << _T(",") << textPnt.y() << _T(",0,");
	ofile << env.m_cableT.m_oLabelTextStyle.m_nHeight << _T(",");
	ofile << env.m_cableT.m_oLabelTextStyle.m_nHeight * env.m_cableT.m_oLabelTextStyle.m_nWidthFactor << _T(",");
	ofile << m_nItemAng << _T(",") << tag_name1 << _T(",") << sJustify << _T(",0|");
	ofile << 0 << _T(",1,3,2") << std::endl;
	
	/// test code for text length
	/*
	{
		CIsVect2d pendicular(len_dir.dx() , len_dir.dy());
		pendicular = pendicular.Perpendicular();
		
		ofile << _T("<PLINE>|");
		ofile << IsString::TrimedTrailingZero(textPnt.x() - len_dir.dx()*(m_nTagWidth*0.5) - pendicular.dx()*dTextHeight*0.5) << _T(",");
		ofile << IsString::TrimedTrailingZero(textPnt.y() - len_dir.dy()*(m_nTagWidth*0.5) - pendicular.dy()*dTextHeight*0.5) << _T(",");
		ofile << IsString::TrimedTrailingZero(textPnt.x() - len_dir.dx()*(m_nTagWidth*0.5) + pendicular.dx()*dTextHeight*0.5) << _T(",");
		ofile << IsString::TrimedTrailingZero(textPnt.y() - len_dir.dy()*(m_nTagWidth*0.5) + pendicular.dy()*dTextHeight*0.5) << _T(",");
		ofile << IsString::TrimedTrailingZero(textPnt.x() + len_dir.dx()*(m_nTagWidth*0.5) + pendicular.dx()*dTextHeight*0.5) << _T(",");
		ofile << IsString::TrimedTrailingZero(textPnt.y() + len_dir.dy()*(m_nTagWidth*0.5) + pendicular.dy()*dTextHeight*0.5) << _T(",");
		ofile << IsString::TrimedTrailingZero(textPnt.x() + len_dir.dx()*(m_nTagWidth*0.5) - pendicular.dx()*dTextHeight*0.5) << _T(",");
		ofile << IsString::TrimedTrailingZero(textPnt.y() + len_dir.dy()*(m_nTagWidth*0.5) - pendicular.dy()*dTextHeight*0.5) << _T(",");
		ofile << IsString::TrimedTrailingZero(textPnt.x() - len_dir.dx()*(m_nTagWidth*0.5) - pendicular.dx()*dTextHeight*0.5) << _T(",");
		ofile << IsString::TrimedTrailingZero(textPnt.y() - len_dir.dy()*(m_nTagWidth*0.5) - pendicular.dy()*dTextHeight*0.5) << _T("|");
		/// color,layer,ltype,lscale,lweight
		ofile << _T("3,0,0,0,0") << std::endl;
	}
	*/

	CIsPoint2d LDPnt;
	LDPnt.SetX(m_tag_point.x() + (len_dir.dx() * 1));
	LDPnt.SetY(m_tag_point.y() + (len_dir.dy() * 1));

	ofile << _T("<LEADER>|");
	ofile << m_tag_point.x() << _T(",") << m_tag_point.y() << _T(",");
	ofile << LDPnt.x() << _T(",") << LDPnt.y() << _T(",");
	ofile << LDPnt.x() << _T(",") << LDPnt.y() << _T("|");
	ofile << 0 << _T(",1,CLOSE,") << env.m_cableT.m_oLabelTextStyle.m_nHeight << _T(",") << env.m_cableT.m_oLabelTextStyle.m_nHeight << _T(",1") << std::endl;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-23
    @function   WriteStraightHatch
    @return     void
    @param      OFSTREAM_T& int_file
    @brief
******************************************************************************/
void CCableOutputData::WriteStraightHatch(OFSTREAM_T& int_file)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	const double vScale = 1.0 / m_pView->GetScale();
	// Insert Cable Tray Hatch
	double tray_width = m_nWidth * vScale;
	double tray_height= m_nHeight * vScale;

	pointVector_ HatchPoints;
	if(this->IsVisible(len_dir))
	{
		if(this->IsVisible(width_dir))
		{
			HatchPoints.push_back(CIsPoint2d(
				tray_start.x() + width_dir.dx() * tray_width * 0.5 , 
				tray_start.y() + width_dir.dy() * tray_width * 0.5));
			HatchPoints.push_back(CIsPoint2d(
				tray_start.x() - width_dir.dx() * tray_width * 0.5 ,
				tray_start.y() - width_dir.dy() * tray_width * 0.5));
			HatchPoints.push_back(CIsPoint2d(
				tray_end.x() - width_dir.dx() * tray_width * 0.5 ,
				tray_end.y() - width_dir.dy() * tray_width * 0.5));
			HatchPoints.push_back(CIsPoint2d(
				tray_end.x() + width_dir.dx() * tray_width * 0.5 ,
				tray_end.y() + width_dir.dy() * tray_width * 0.5));
		}
		else
		{
			HatchPoints.push_back(CIsPoint2d(
				tray_start.x() + height_dir.dx() * tray_height ,
				tray_start.y() + height_dir.dy() * tray_height));
			HatchPoints.push_back(CIsPoint2d(
				tray_end.x() + height_dir.dx() * tray_height ,
				tray_end.y() + height_dir.dy() * tray_height));
			HatchPoints.push_back(CIsPoint2d(tray_end.x() , tray_end.y()));
			HatchPoints.push_back(CIsPoint2d(tray_start.x() , tray_start.y()));
		}
	}
	else
	{
		HatchPoints.push_back(CIsPoint2d(
			tray_start.x() + width_dir.dx() * tray_width * 0.5 ,
			tray_start.y() + width_dir.dy() * tray_width * 0.5));
		HatchPoints.push_back(CIsPoint2d(
			tray_start.x() - width_dir.dx() * tray_width * 0.5 ,
			tray_start.y() - width_dir.dy() * tray_width * 0.5));
		HatchPoints.push_back(CIsPoint2d(
			HatchPoints[1].x() + height_dir.dx() * tray_height ,
			HatchPoints[1].y() + height_dir.dy() * tray_height));
		HatchPoints.push_back(CIsPoint2d(
			HatchPoints[0].x() + height_dir.dx() * tray_height ,
			HatchPoints[0].y() + height_dir.dy() * tray_height));
	}
	
	pointVector_ clipPolygon , output;
	this->GetClipPolygonOfView(clipPolygon);
	SutherlandHodgmanPolygonClip(HatchPoints , clipPolygon , output);

	if(!output.empty())
	{
		STRING_T sCellName = _T("");
		STRING_T sHatchColor = _T("");
		STRING_T sCellFilePath = _T("NULL");

		UINT uiOutLineWeight = 0;
		bool bOutLineOnOff = true;

		if(_T("INST") == m_sDepartmentType)
		{
			sCellName = env.m_cableT.m_sInstCableT_CellName;
			sHatchColor = env.m_cableT.m_sInstCableT_HatchColor;
			if(sCellName != _T("NULL")) sCellFilePath = env.m_cableT.m_sCellFilePath;

			uiOutLineWeight = env.m_cableT.m_uiInstCableT_OutLine_Weight;
			if(env.m_cableT.m_sInstCableT_OutLine_OnOff == _T("ON")) bOutLineOnOff = true;
			else bOutLineOnOff = false;
		}
		else
		{
			sCellName = env.m_cableT.m_sElecCableT_CellName;
			sHatchColor = env.m_cableT.m_sElecCableT_HatchColor;
			if(sCellName != _T("NULL")) sCellFilePath = env.m_cableT.m_sCellFilePath;
			
			uiOutLineWeight = env.m_cableT.m_uiElecCableT_OutLine_Weight;
			if(env.m_cableT.m_sElecCableT_OutLine_OnOff == _T("ON")) bOutLineOnOff = true;
			else bOutLineOnOff = false;
		}

		int_file << _T("<HATCHPLINE>|");
		for(vector<CIsPoint2d>::iterator itr = output.begin();itr != output.end();++itr)
		{
			int_file << itr->x() << _T(",") << itr->y() << _T(","); 
		}
		int_file << (output.front().x()) << _T(",") << (output.front().y()) << _T("|"); 

		int_file << sHatchColor << _T(",1,") << sCellFilePath << _T(",") << sCellName << _T(",45,1,Yes") << std::endl;
		
		//Out Line Drawing
		if(bOutLineOnOff)
		{
			int nLineStyle;

			if(m_pView->GetViewDirString() == _T("TOP") || m_pView->GetViewDirString() == _T("BOTTOM")) nLineStyle = 0;
			else nLineStyle = 0;
			
			int_file << _T("<PLINE>|");
			for(vector<CIsPoint2d>::iterator itr = output.begin();itr != output.end();++itr)
			{
				int_file << itr->x() << _T(",") << itr->y() << _T(",") ;
			}
			int_file << output.front().x() << _T(",") << output.front().y() << _T("|"); 
			int_file << sHatchColor << _T(",1,") << nLineStyle << _T(",1,") << uiOutLineWeight << std::endl;
		}
	}
	else
	{
		int d = 1;
	}
}

/**
	@brief	get middle point 

	@author	HumKyung.BAEK

	@date	2010.09.07
*/
CIsPoint3d CCableOutputData::GetMiddlePoint(const CIsPoint3d& pt1 , const CIsPoint3d& pt2 , const CIsPoint3d& ptCenter , const double& nRadius)
{
	CIsPoint3d res;

	CIsPoint3d mid((pt1 + pt2) * 0.5);
	
	CIsVect3d vec(mid - ptCenter);
	vec.Normalize();

	res = ptCenter + (vec * nRadius);
	
	return res;
}

/**
	@brief	

	@author	KyungMin.Choi, HumKyung.Baek

	@date	

	@return
*/
void CCableOutputData::WriteTeeHatch(OFSTREAM_T& int_file)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	const double vScale = 1.0 / m_pView->GetScale();

	// Insert Cable Tray Hatch
	double tray_width = m_nWidth * vScale;
	double tray_height= m_nHeight * vScale;
	
	double tray_width2 = m_nWidth2 * vScale;

	//TEE Base Hatch
	vector<CIsPoint3d> HatchPoints;
	if(this->IsVisible(len_dir))
	{
		if(this->IsVisible(width_dir))
		{
			HatchPoints.resize(12);

			HatchPoints[0] = tray_start - width_dir * tray_width * 0.5;
			HatchPoints[4] = tray_mid - (len_dir * tray_width2 * 0.5);
			HatchPoints[5] = tray_mid + (len_dir * tray_width2 * 0.5);
			HatchPoints[9] = tray_end - (width_dir * tray_width * 0.5);
			HatchPoints[10]= tray_end + (width_dir * tray_width * 0.5);
			HatchPoints[11] = tray_start + width_dir * tray_width * 0.5;

			CIsPoint2d ptEdge[2][2] , ptCenter;
			CIsPoint3d pt3d;

			pt3d = (tray_start + width_dir*1000);
			ptEdge[0][0].Set(pt3d.x() , pt3d.y());
			pt3d = tray_start - width_dir*1000;
			ptEdge[0][1].Set(pt3d.x() , pt3d.y());
			pt3d = tray_mid + len_dir*1000;
			ptEdge[1][0].Set(pt3d.x() , pt3d.y());
			pt3d = tray_mid - len_dir*1000;
			ptEdge[1][1].Set(pt3d.x() , pt3d.y());
			intersect(&(ptEdge[0][0]) , &(ptEdge[0][1]) , ptEdge[1] , &ptCenter);

			pt3d.Set(ptCenter.x() , ptCenter.y() , HatchPoints[0].z());
			double nRadius = pt3d.DistanceTo(HatchPoints[0]);
			HatchPoints[2] = GetMiddlePoint(HatchPoints[0] , HatchPoints[4] , pt3d , nRadius);
			HatchPoints[1] = GetMiddlePoint(HatchPoints[0] , HatchPoints[2] , pt3d , nRadius);
			HatchPoints[3] = GetMiddlePoint(HatchPoints[2] , HatchPoints[4] , pt3d , nRadius);

			pt3d = tray_end + width_dir*1000;
			ptEdge[0][0].Set(pt3d.x() , pt3d.y());
			pt3d = tray_end - width_dir*1000;
			ptEdge[0][1].Set(pt3d.x() , pt3d.y());
			pt3d = tray_mid + len_dir*1000;
			ptEdge[1][0].Set(pt3d.x() , pt3d.y());
			pt3d = tray_mid - len_dir*1000;
			ptEdge[1][1].Set(pt3d.x() , pt3d.y());
			intersect(&(ptEdge[0][0]) , &(ptEdge[0][1]) , ptEdge[1] , &ptCenter);
	
			pt3d.Set(ptCenter.x() , ptCenter.y() , HatchPoints[0].z());
			nRadius = pt3d.DistanceTo(HatchPoints[5]);
			HatchPoints[7] = GetMiddlePoint(HatchPoints[5] , HatchPoints[9] , pt3d , nRadius);
			HatchPoints[6] = GetMiddlePoint(HatchPoints[5] , HatchPoints[7] , pt3d , nRadius);
			HatchPoints[8] = GetMiddlePoint(HatchPoints[7] , HatchPoints[9] , pt3d , nRadius);
		}
		else
		{
			HatchPoints.resize(4);

			HatchPoints[0] = tray_start + height_dir * tray_height;
			HatchPoints[1] = tray_end   + height_dir * tray_height;
			HatchPoints[2] = tray_end;
			HatchPoints[3] = tray_start;
		}
	}
	else
	{
		HatchPoints.resize(4);

		HatchPoints[0] = tray_start + width_dir * tray_width * 0.5;
		HatchPoints[1] = tray_start - width_dir * tray_width * 0.5;
		HatchPoints[2] = HatchPoints[1] + height_dir * tray_height;
		HatchPoints[3] = HatchPoints[0] + height_dir * tray_height;
	}

	//! clip shape by view
	pointVector_ input , clipPolygon , output;
	this->GetClipPolygonOfView(clipPolygon);
	for(vector<CIsPoint3d>::iterator itr = HatchPoints.begin();itr != HatchPoints.end();++itr)
	{
		input.push_back(CIsPoint2d(itr->x() , itr->y()));
	}
	SutherlandHodgmanPolygonClip(input , clipPolygon , output);

	if(!output.empty())
	{
		STRING_T sCellName;
		STRING_T sHatchColor;
		STRING_T sCellFilePath = _T("NULL");

		UINT uiOutLineWeight = 0;
		bool bOutLineOnOff = true;

		if(_T("INST") == m_sDepartmentType)
		{
			sCellName = env.m_cableT.m_sInstCableT_CellName;
			sHatchColor = env.m_cableT.m_sElecCableT_HatchColor;
			if(sCellName != _T("NULL")) sCellFilePath = env.m_cableT.m_sCellFilePath;

			uiOutLineWeight = env.m_cableT.m_uiInstCableT_OutLine_Weight;
			if(env.m_cableT.m_sInstCableT_OutLine_OnOff == _T("ON")) bOutLineOnOff = true;
			else bOutLineOnOff = false;
		}
		else
		{
			sCellName = env.m_cableT.m_sElecCableT_CellName;
			sHatchColor = env.m_cableT.m_sElecCableT_HatchColor;
			if(sCellName != _T("NULL")) sCellFilePath = env.m_cableT.m_sCellFilePath;

			uiOutLineWeight = env.m_cableT.m_uiElecCableT_OutLine_Weight;
			if(env.m_cableT.m_sElecCableT_OutLine_OnOff == _T("ON")) bOutLineOnOff = true;
			else bOutLineOnOff = false;
		}

		int_file << _T("<HATCHPLINE>|");
		for(vector<CIsPoint2d>::iterator itr = output.begin();itr != output.end();++itr)
		{	
			int_file << itr->x() << _T(",") << itr->y() << _T(",");
		}

		int_file << output.front().x() << _T(",") << output.front().y();
		int_file << _T("|") << sHatchColor << _T(",1,") << sCellFilePath << _T(",") << sCellName << _T(",45,1,Yes") << std::endl;

		//Out Line Drawing
		if(bOutLineOnOff)
		{
			STRING_T sViewDir = m_pView->GetViewDirString();
			IsString::ToUpper(sViewDir);

			int nLineStyle = 0;
			if(sViewDir == _T("TOP") || sViewDir == _T("BOTTOM")) nLineStyle = 0;
			else nLineStyle = 0;

			int_file << _T("<PLINE>|");
			for(vector<CIsPoint2d>::iterator itr = output.begin();itr != output.end();++itr)
			{
				int_file << itr->x() << _T(",") << itr->y() << _T(",");
			}

			int_file << output.front().x() << _T(",") << output.front().y();
			int_file << _T("|") << sHatchColor << _T(",1,") << nLineStyle << _T(",1,") << uiOutLineWeight << std::endl;
		}
	}

	//End TEE Base Hatch
}

/**
	@brief	

	@author	KyungMin.Choi, HumKyung.Baek

	@date	

	@return
*/
void CCableOutputData::WriteReduceHatch(OFSTREAM_T& int_file)
{	
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	CIsPoint2d firstPnt;
	bool isFirst = true;

	//! clipping reducer outline
	pointVector_ input , clipPolygon , output;
	for(vector<CIsPoint3d>::iterator itr = m_oReducePntList.begin();itr != m_oReducePntList.end();++itr)
	{
		input.push_back(CIsPoint2d(itr->m_x , itr->m_y));
	}

	this->GetClipPolygonOfView(clipPolygon);
	SutherlandHodgmanPolygonClip(input , clipPolygon , output);
	
	if(!output.empty())
	{
		STRING_T sCellName;
		STRING_T sHatchColor;
		STRING_T sCellFilePath = _T("NULL");

		UINT uiOutLineWeight = 0;
		bool bOutLineOnOff = true;

		if(_T("INST") == m_sDepartmentType)
		{
			sCellName = env.m_cableT.m_sInstCableT_CellName;
			sHatchColor = env.m_cableT.m_sInstCableT_HatchColor;
			if(sCellName != _T("NULL")) sCellFilePath = env.m_cableT.m_sCellFilePath;

			uiOutLineWeight = env.m_cableT.m_uiInstCableT_OutLine_Weight;
			if(env.m_cableT.m_sInstCableT_OutLine_OnOff == _T("ON")) bOutLineOnOff = true;
			else bOutLineOnOff = false;
		}
		else
		{
			sCellName = env.m_cableT.m_sElecCableT_CellName;
			sHatchColor = env.m_cableT.m_sElecCableT_HatchColor;
			if(sCellName != _T("NULL")) sCellFilePath = env.m_cableT.m_sCellFilePath;

			uiOutLineWeight = env.m_cableT.m_uiElecCableT_OutLine_Weight;
			if(env.m_cableT.m_sElecCableT_OutLine_OnOff == _T("ON")) bOutLineOnOff = true;
			else bOutLineOnOff = false;
		}

		int_file << _T("<HATCHPLINE>|");
		for(vector<CIsPoint2d>::iterator itr = output.begin();itr != output.end();++itr)
		{	
			int_file << itr->x() << _T(",") << itr->y() << _T(",");
		}

		int_file << output.front().x() << _T(",") << output.front().y();
		int_file << _T("|") << sHatchColor << _T(",1,") << sCellFilePath << _T(",") << sCellName << _T(",45,1,Yes") << std::endl;


		//Out Line Drawing
		if(bOutLineOnOff)
		{
			int nLineStyle;

			STRING_T sViewDir = m_pView->GetViewDirString();
			IsString::ToUpper(sViewDir);
			if(sViewDir == _T("TOP") || sViewDir == _T("BOTTOM")) nLineStyle = 0;
			else nLineStyle = 0;

			isFirst = true;

			if(!output.empty())
			{
				int_file << _T("<PLINE>|");
				for(vector<CIsPoint2d>::iterator itr = output.begin();itr != output.end();++itr)
				{
					int_file << itr->x() << _T(",") << itr->y() << _T(",");
				}

				int_file << output.front().x() << _T(",") << output.front().y();
				int_file << _T("|") << sHatchColor << _T(",1,") << nLineStyle << _T(",1,") << uiOutLineWeight << std::endl;
			}
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CCableTrayGroup
    @function   Add
    @return     bool
    @param      CaDraw_TagCableTray*    pCableTrayInput
    @brief
******************************************************************************/
bool CCableTrayGroup::Add(CaDraw_TagCableTray* pCableTrayInput)
{
	//assert(pCableTrayInput && "pCableTrayInput is NULL");

	if(pCableTrayInput)
	{
		m_bHasSameElevation = (0.f == pCableTrayInput->lengthDir().dz()) ? true : false;
		m_oCableTrayRefList.push_back(pCableTrayInput);

		return true;
	}

	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class
    @function   IsEquivalentPoint
    @return     bool
    @param      const CIsPoint2d& lhs
    @param      const CIsPoint2d& rhs
    @param      const double& nTol
    @brief
******************************************************************************/
bool IsEquivalentPoint(const CIsPoint2d& lhs , const CIsPoint2d& rhs , const double& nTol)
{
	return (lhs.DistanceTo(rhs) <= nTol);
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-22
    @class      CCableTrayGroup
    @function   GetCableTrayToBeGroupingOn
    @return     bool
    @param      vector<CaDraw_TagCableTray*>&   CableTrayList
    @param      CaDraw_TagCableTray*            pCableT
    @param      const                           double&
    @param      dElev
    @brief
******************************************************************************/
bool CCableTrayGroup::GetCableTrayToBeGroupingOn(vector<CaDraw_TagCableTray*>& CableTrayList , CaDraw_TagCableTray* pCableT , const double& dElev)
{
	assert(pCableT && "pCableT is NULL");

	if(pCableT && (false == m_bTag))
	{
		CIsPoint2d clipEdge[2] , ptMid;
		ptMid.SetX((pCableT->GetStartPoint().x() + pCableT->GetEndPoint().x()) * 0.5);
		ptMid.SetY((pCableT->GetStartPoint().y() + pCableT->GetEndPoint().y()) * 0.5);

		CIsVect2d vec[2];
		vec[0].Set(pCableT->GetStartPoint().x() - ptMid.x() , pCableT->GetStartPoint().y() - ptMid.y());
		vec[0].Normalize();
		vec[0] = vec[0].Perpendicular(); /// 90 rotate

		vec[1].Set(pCableT->GetEndPoint().x() - ptMid.x() , pCableT->GetEndPoint().y() - ptMid.y());
		vec[1].Normalize();
		vec[1] = vec[1].Perpendicular();

		clipEdge[0].SetX(pCableT->GetStartPoint().x());
		clipEdge[0].SetY(pCableT->GetStartPoint().y());
		clipEdge[1].SetX(pCableT->GetEndPoint().x());
		clipEdge[1].SetY(pCableT->GetEndPoint().y());
		const double distance = clipEdge[0].DistanceTo(clipEdge[1]);
		clipEdge[0] = ptMid + vec[0]*distance*0.5;
		clipEdge[1] = ptMid + vec[1]*distance*0.5; 

		for(list<CaDraw_TagCableTray*>::iterator itr = m_oCableTrayRefList.begin();itr != m_oCableTrayRefList.end();++itr)
		{
			if((*itr)->IsVertical3d() || (dElev != (*itr)->GetElevation())) continue;
			if(pCableT->m_sDepartmentType != (*itr)->m_sDepartmentType) continue;
			if((*itr)->m_sType != _T("STRAIGHT")) continue;

			CIsPoint2d ptStart , ptEnd , intPt;
			ptStart.SetX((*itr)->GetStartPoint().x());
			ptStart.SetY((*itr)->GetStartPoint().y());
			ptEnd.SetX((*itr)->GetEndPoint().x());
			ptEnd.SetY((*itr)->GetEndPoint().y());
			intPt.SetX(pCableT->GetStartPoint().x());
			intPt.SetY(pCableT->GetEndPoint().y());
			const int res = intersect(&ptStart , &ptEnd , clipEdge , &intPt);
			if((ERROR_SUCCESS == res) && IsEquivalentPoint(intPt , ptMid , 500))
			{
				m_bTag = true;
				(*itr)->m_IntPt = intPt;
				CableTrayList.push_back(*itr);
				
				return true;
			}
		}
	}

	return false;
}