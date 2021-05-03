#pragma once

#include <IsPoint3d.h>
#include "SmartDrawAnnoTextStyle.h"
#include "SmartDrawAnnoEntity.h"
#include "SmartDrawAnnoItem.h"

//namespace tag_entity
//{
class ON_TAG_EXT_DLL CaDraw_TagCableTray : public CaDraw_TagEntity
{
public:
	class ENV
	{
	public:
		int Parse(const STRING_T& sIniFilePath);

		CSmartDrawAnnoTextStyle m_oLabelTextStyle;
		CompProp m_oLabelProp , m_oLeaderProp;

		int m_iRowCount;		/// number of row
		double m_dRowHeight;	/// height of a row

		STRING_T m_sElecCableT_Label;
		STRING_T m_sElecCableT_EL_Prefix;
		STRING_T m_sElecCableT_Tag_OnOff;
		STRING_T m_sElecCableT_FileName_Rule;
		STRING_T m_sElecCableT_CellName , m_sElecCableT_HatchColor;
		STRING_T m_sElecCableT_OutLine_OnOff;
		UINT m_uiElecCableT_OutLine_Weight;

		STRING_T m_sInstCableT_Label;
		STRING_T m_sInstCableT_EL_Prefix;
		STRING_T m_sInstCableT_Tag_OnOff;
		STRING_T m_sInstCableT_FileName_Rule;
		STRING_T m_sInstCableT_CellName , m_sInstCableT_HatchColor;
		STRING_T m_sInstCableT_OutLine_OnOff;
		UINT m_uiInstCableT_OutLine_Weight;

		STRING_T m_sFittingTag_OnOff;
		STRING_T m_sUpDownMark_OnOff;
		STRING_T sText_Color , sLeader_Color;

		STRING_T m_sArrowType;
		STRING_T m_sCellFilePath;
	};

	class ON_TAG_EXT_DLL CSmartDrawAnnoCableTrayTagItem : public CaDraw_TagItem
	{
	public:
		CSmartDrawAnnoCableTrayTagItem(const double& nElevation);
		virtual ~CSmartDrawAnnoCableTrayTagItem();

		void OnRegisterToDivision(list<CExtDivision*>& oDivisionList);
		int OnCreateTagPos(vector<CaDraw_TagPos*>& TagPosList , CExtDivision *pDivision);
	private:
		double m_nElevation;
	};

	class ON_TAG_EXT_DLL CSmartDrawAnnoCableTrayTagPos : public CaDraw_TagPos
	{
	public:
		virtual void OnRegisterToDimension(CDimLevel* pDimLevel,const DIM_LEVEL_E level);
		void Write(OFSTREAM_T& file , CExtDivision* pDivision);

		CSmartDrawAnnoCableTrayTagPos(const double& nElevation);
		virtual ~CSmartDrawAnnoCableTrayTagPos();
	private:
		double m_nElevation;
	};

	CaDraw_TagCableTray(CaDraw_Entity* unnamed);
	~CaDraw_TagCableTray(void);

	bool CanBeConnected(CaDraw_TagCableTray* rhs);

	SMARTDRAW_TAG_DECLARE_FUNC()
public:
	STRING_T name() const;
	CIsPoint3d GetStartPoint() const;
	CIsPoint3d GetEndPoint() const;
	CIsPoint3d GetMidPoint() const;

	double length() const;	/// 2012.02.22 added by humkyung
	CIsVect3d heightDir() const;
	CIsVect3d lengthDir() const;
	CIsVect3d widthDir() const;
	double GetElevation() const;
	STRING_T GetViewName() const;
	CaDraw_TagCableTray& operator=(const CaDraw_TagCableTray& rhs);

	bool IsHorizontal() const;
	bool IsVertical() const;
	bool IsVertical3d() const;

	int Parse();
	int m_iCableRouteNo;

	STRING_T m_sType , m_sSubType , m_sDepartmentType;
	double   m_nWidth, m_nWidth2, m_nHeight, m_nItemAng;
	CIsPoint2d  m_ptCenter , m_IntPt;		/// Grouping 된 Cable의 위치 표기할때 사용

	vector<CIsPoint3d>* m_pConnPointList;	/// connection point list
	vector<CIsPoint3d>* m_pReducePntList;	/// point list for reducer
private:
	CaDraw_TagItem* CreateTagObject(CaDraw_View *pView, const CIsPoint3d ptOrigin, const CIsVect3d vec, DITEM_E enmType);
	void GetTagNo(STRING_T* pTagNo , const CIsPoint3d pt , CaDraw_View* pView);
	bool CreateXDirTagObject(list<CaDraw_TagItem*>& oTagItemList , bool bFlags[2] , const CIsPoint3d pt[2] , CSmartDrawAnnoView *pAnnoView , const bool& bCreateTagItem=false);
	bool CreateYDirTagObject(list<CaDraw_TagItem*>& oTagItemList , bool bFlags[2] , const CIsPoint3d pt[2] , CSmartDrawAnnoView *pAnnoView , const bool& bCreateTagItem=false);

	STRING_T m_sViewName , m_sName;
	double m_dElevation;
	CIsPoint3d m_ptStart , m_ptEnd, m_ptMid;
	CIsVect3d  m_lengthDir , m_widthDir , m_heightDir;

	double m_dLength;
	int    s_type;
};
//};