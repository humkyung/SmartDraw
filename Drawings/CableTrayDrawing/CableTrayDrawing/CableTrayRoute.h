#pragma once

#include "StdAfx.h"
#include <tchar.h>
#include <SmartDrawAnnoCableTray.h>

#include <list>
#include <vector>
#include <set>
using namespace std;

namespace CableTray
{
	typedef vector<CIsPoint2d> pointVector_;

	class CCableOutputData
	{
	public:
		CCableOutputData(){}
		CCableOutputData(const CCableOutputData& rhs);
		CCableOutputData& operator=(const CCableOutputData& rhs);

		bool    tag;
		CIsPoint2d insert_point;	//! graphic data origin point
		CIsPoint2d m_tag_point;	//! tag data origin point(low-left)
		double m_nWidth , m_nHeight, m_nWidth2;
		double m_nTagWidth, m_nTagHeight;

		STRING_T tag_name1;
		STRING_T tag_name2;
		CIsPoint2d name1_point;
		CIsPoint2d name2_point;
		bool    xdir;
		bool    line_2;
		double  Angle;
	//	CLine2d line1_point;
		CIsPoint2d line2_point;

	//	//added by K.M Choi
		STRING_T bot_el;
		CIsPoint3d tray_start , tray_end, tray_mid;
		CIsVect3d len_dir , width_dir , height_dir;
		string m_sType , m_sSubType, m_sDepartmentType;
		vector<CIsPoint3d> m_oReducePntList;
		double m_nItemAng, m_nTeeWidth, m_nReduceWidth;

		vector<STRING_T>   m_oTraySizeList;
		vector<CIsPoint2d> m_oIntPtList;

		CaDraw_View* m_pView;

	//	//함수
		void TAG_NAME(STRING_T Name1, STRING_T Name2, int Type);
		void OUTPUT_DATA(CIsPoint2d cen, double H_len1, double H_len2, double len, double Gap);
	//	void ANGLE_OUTPUT_DATA(CPoint2d cen, double H_len2, double Gap);

		void WriteTrayTag(OFSTREAM_T& ofile);
		void WriteBOT_ForISO(OFSTREAM_T& int_file);
		void GetFittingTagName(const CaDraw_TagCableTray* objInput);
		void WriteUpDownMark(OFSTREAM_T& int_file);
	private:
		CIsPoint3d GetMiddlePoint(const CIsPoint3d& pt1 , const CIsPoint3d& pt2 , const CIsPoint3d& ptCenter , const double& nRadius);

		void WriteStraightHatch(OFSTREAM_T& int_file);
		void WriteElbowHatch(OFSTREAM_T& int_file);
		void WriteTeeHatch(OFSTREAM_T& int_file);
		void WriteReduceHatch(OFSTREAM_T& int_file);
	public:
	//	int GetWidthAndHeight(double& nWidth, double& nHeight);
		int WriteHatch(OFSTREAM_T& ofile);
	private:
		int GetClipPolygonOfView(pointVector_& clipPolygon);	/// 2012.02.24 added by humkyung
		bool IsVisible(const CIsVect3d& rhs);
	};

	class CCableMidData
	{
	public:
		bool    tag;
		CIsPoint2d insert_point;
		STRING_T tag_name1;
		STRING_T  tag_name2;
		int     cnt;
		double  len;	
		bool    x_dir;
		CIsPoint2d tag_point;
		CIsPoint2d st_point;
		CIsPoint2d ed_point;
		
		//함수
		void PRI(CIsPoint2d Origin, double nBoxWidth , double nBoxHeight);
		/*void ANGLE_PRI(CPoint2d Origin, double X_len, double Y_len, double Xh_len, double Yh_len, int Type);
		void ORTHO(CPoint2d Origin, bool xdir, double H_len, double length, int Type);
		void INSERT(CPoint2d Origin, bool xdir, double H_len, double length, int Type);*/
	};

/*
Grouping된 Cable Tray Route를 Elevation 별로 다시 Grouping

2010.10.22
*/
class CCableTrayGroup
{
public:
	CCableTrayGroup() : m_bHasSameElevation(false) , m_bTag(false)
	{
	}

	list<CaDraw_TagCableTray*> m_oCableTrayRefList;
	bool m_bHasSameElevation , m_bTag;

	bool Add(CaDraw_TagCableTray* pCableTrayInput); 
	
	CaDraw_TagCableTray* GetTaggingCableTray()
	{
		CaDraw_TagCableTray* res = NULL;
		if((false == m_bTag) && (true == m_bHasSameElevation))
		{
			double nLen = -1;
			bool bChkRes = false;
			for(list<CaDraw_TagCableTray*>::iterator jtr = m_oCableTrayRefList.begin();jtr != m_oCableTrayRefList.end();++jtr)
			{
				if(_T("STRAIGHT") != (*jtr)->m_sType) continue;

				if((*jtr)->length() > nLen)
				{
					nLen = (*jtr)->length();
					res = *jtr;
				}
			}			
		}

		return res;
	}

	bool GetCableTrayToBeGroupingOn(vector<CaDraw_TagCableTray*>& CableTrayList , CaDraw_TagCableTray* pCableT , const double& nElev);
};

class CCableTrayRoute
{
public:
	CCableTrayRoute(const int& nRouteNo){ m_iRouteNo = nRouteNo; }
	~CCableTrayRoute();

	CaDraw_View* GetView() const;
	bool GetCableTrayToBeGroupingOn(vector<CaDraw_TagCableTray*>& CableTrayList , CaDraw_TagCableTray* pCableT , const double& dElev);
	bool GetTaggingCableTray(vector<CaDraw_TagCableTray*>& CableTrayList);
	bool IsPossibleToBeMember(CaDraw_TagCableTray* pCable);
	int AddData(CaDraw_TagCableTray* pCableTray);
	int SetCableHatchData(CaDraw_View* pView , vector<CCableOutputData>& CableHatchOutputList);
	int SetCableISOData(CaDraw_View* pView , vector<CCableOutputData>& CableOutputList);
	int CreateCableGroup();
	int CableRoute_Sort();
	int SetCableFittingData(CaDraw_View* pView , vector<CCableOutputData>& CableOutputList/*, vector<Cell_List>& C_list, Cabl_Option CA_opt*/); //101209 KM
	int SetCableUpDownMarkData(CaDraw_View* pView , vector<CCableOutputData>& CableUpDownOutputList/*, Cabl_Option CA_opt*/); //101209 KM

public:
	int m_iRouteNo;
	STRING_T m_sViewNo;
	list<CCableTrayGroup*> m_oCableGroupList;
private:
	list<CaDraw_TagCableTray*> m_oCableTrayList;
	set<double> m_TaggedElevationSet;
};

extern int CreateCableRoute(vector<CaDraw_TagCableTray*>& oCableInputList);
extern int Annotate(/*, vector<Cell_List>& C_list,Cabl_Option CA_opt, View_Par& View, double Tol*/);
extern int Write(OFSTREAM_T& ofile);
};