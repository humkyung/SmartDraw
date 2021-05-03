#pragma once

#include <IsTools.h>
#include <IsPoint2d.h>
#include <IsMatrix3d.h>
#include <util\Markup\Markup.h>
#include "aDraw_Entity.h"
#include "ViewBoundary.h"

#include <list>
#include <map>
using namespace std;

typedef enum
{
	BOTTOM_LEFT	= 0x00,
	BOTTOM_RIGHT= 0x01,
	RIGHT_DOWN	= 0x02,
	RIGHT_UP	= 0x03,
	TOP_RIGHT	= 0x04,
	TOP_LEFT	= 0x05,
	LEFT_UP		= 0x06,
	LEFT_DOWN	= 0x07
}VIEW_COORD;

class CaDraw_Equipment;
class ON_MODULE_EXT_DLL CaDraw_View : public CaDraw_Entity
{
public:
	typedef struct
	{
		MODEL_TYPE_T modelType;
		STRING_T rModelName;
	}MODEL_T;

	typedef enum
	{
		COORD_ALL					= 1,
		COORD_TOPLEFT_BOTTOMRIGHT	= 2,
		COORD_BOTTOMLEFT_TOPRIGHT	= 3
	}COORD_OPTION_E;

	typedef enum
	{
		TOP_VIEW_T		= 0x00,
		BOTTOM_VIEW_T	= 0x01,
		FRONT_VIEW_T	= 0x02,
		BACK_VIEW_T		= 0x03,
		LEFT_VIEW_T		= 0x04,
		RIGHT_VIEW_T	= 0x05,
		ISO1_VIEW_T		= 0x06,
		ISO2_VIEW_T		= 0x07,
		ISO3_VIEW_T		= 0x08
	}DIRECTIONT_T;
	
	typedef enum
	{
		R0_T	= 0,
		R90_T	= 90,
		R180_T	= 180,
		R270_T	= 270
	}ROTATE_T;

	typedef enum
	{
		NONE_SIDE_T	= 0xFF,
		UP_SIDE_T	= 0x00,
		DOWN_SIDE_T = 0x01,
		EAST_SIDE_T	= 0x02,
		WEST_SIDE_T	= 0x03,
		NORTH_SIDE_T= 0x04,
		SOUTH_SIDE_T= 0x05
	};

	typedef enum
	{
		NO_MODEL_FILE			= 0x01,
		DUPLICATED_MODEL_FILE	= 0x02
	}STATUS_E;

	CaDraw_View();
	virtual ~CaDraw_View(void);

	bool IsEnable(AnnoFilters*) const;

	int IsAdjacent(const CaDraw_View* rhs,const double dToler) const;	/// 2016.03.07 added by humkyung
	int status() const;												/// 2014.06.18 added by humkyung
	CaDraw_Equipment* FindEquipmentByName( const STRING_T& name );	/// 2012.03.25 added by humkyung

	bool IsPlanView(void);											/// 2012.02.27 added by humkyung
	bool IsSectionView(void);										/// 2012.02.27 added by humkyung

	LPCTSTR name() const;											/// 2012.02.21 added by humkyung

	CIsPoint3d center() const;										/// 2012.02.15 added by humkyung
	int Report(soci::session&,const STRING_T&) const;				/// 2012.02.15 added by humkyung
	bool DoesIncludeVolume(const CIsVolume& volume);				/// 2012.02.15 added by humkyung

	int SetViewInfo(const TCHAR* p);								/// 2012.02.11 added by humkyung
	const TCHAR* GetViewInfo(void) const;

	static DIRECTIONT_T GetViewDirFrom(const STRING_T& sViewDir);	/// 2012.02.11 added by humkyung
	static ROTATE_T GetViewAngleFrom(const STRING_T& sViewAngle);	/// 2012.02.11 added by humkyung

	const TCHAR* GetViewMatchLine( const int& nSide) const;
	const TCHAR* GetMatchLineData(const int& nSide) const;			/// 2012.02.11 added by humkyung
	int SetMatchLineData(const int& nSide , const TCHAR* pData);	/// 2012.02.11 added by humkyung

	CViewBoundary* CreateViewBoundary(const int& iExtTagSide , const CaDraw_View::COORD_OPTION_E& coords);
	bool IsInsideViewOf(CaDraw_View* pView);
	bool IsSameShapeView(CaDraw_View *pView);

	CIsPoint3d origin() const;
	double GetViewWidth() const;
	double GetViewHeight() const;

	int SetViewDir(const DIRECTIONT_T& dir );	/// 2012.02.11 added by humkyung
	STRING_T GetViewDirString() const;			/// 2012.02.11 added by humkyung
	DIRECTIONT_T GetViewDir() const;

	ROTATE_T GetViewAngle() const;
	int SetViewAngle(const ROTATE_T& rot);		/// 2012.02.11 added by humkyung
	STRING_T GetViewAngleString() const;		/// 2012.02.11 added by humkyung

	void SetScale(const double& nScale);
	double GetScale() const;
	void SetName(const STRING_T& rName);
	void Copy(CaDraw_View* unnamed);

	int Save(CMarkup& xml);		/// 2012.02.12 added by humkyung
	int Parse(CMarkup& xml);
	ADRAW_DECLARE_FUNC()
private:
	void GetCoordText(TCHAR *pCoordText, int nDir, double nVal);
	const double GetCoordValue(const int& where);

	long m_nIndex;

	DIRECTIONT_T m_direction;
	double       m_scale;			/// view scale
	ROTATE_T     m_rotate;
	double	     m_nViewWidth , m_nViewHeight;
	TCHAR	     m_szViewMatch[6][VIEW_MATCH_BUF_SIZ + 1];	/// match line text
	STRING_T*    m_pViewInfo;
	list<MODEL_T>* m_pModelList;

	map<STRING_T , list<CaDraw_Entity*>*>* m_pEntMap;
public:
	int SetModelList(list<MODEL_T>* pModelList) ;	/// 2012.02.11 added by humkyung
	list<MODEL_T>* GetModelList() const;			/// 2012.02.11 added by humkyung

	int GetEntList(list<CaDraw_Entity*>* pEntList) const;
	bool IsEntityIn(CaDraw_Entity* pEnt);
	int Add(CaDraw_Entity* pEnt);
	
	CIsPoint3d ModelCoord2ViewCoord(const CIsPoint3d& pt);
	double ModelCoord2ViewCoord(const double& value);
	CIsVolume ModelCoord2ViewCoord(const CIsVolume& volume);
	
	CIsPoint3d ViewCoord2ModelCoord(const CIsPoint3d& pt);	/// 2012.03.19 added by humkyung

	CIsMatrix3d GetRMatrix(void);
	double GetViewTopElev(void);
};
