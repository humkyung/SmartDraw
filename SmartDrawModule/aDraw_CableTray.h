#pragma once
#include "adraw_entity.h"

class ON_MODULE_EXT_DLL CaDraw_CableTray : public CaDraw_Entity
{
public:
	typedef enum
	{
		NONE_T     =-1,
		STRAIGHT_T = 1,
		TEE_T      = 2,
		ELBOW_T    = 3,
		REDUCE_T   = 4,
		CROSS_T	   = 5,
		WHY_T      = 6
	}ELEMENT_E;

	typedef enum
	{
		NONE_ST	      =-1,
		STRAIGHT_ST   = 1,
		LEFT_ST       = 2,
		RIGHT_ST      = 3,
		HORIZONTAL_ST = 4,
		VERTICAL_ST   = 5
	}SUBTYPE_E;

	typedef enum
	{
		ELECTRIC	= 1,
		INSTRUMENT	= 2
	}CATEGORY_E;

	bool IsEnable(AnnoFilters* ) const;

	int Dump(OFSTREAM_T& ofile);				/// 2012.02.16 added by humkyung

	int GetEdgeList(vector<CIsPoint3d>& oEdgeList) const;	/// 2012.02.21 added by humkyung
	int SetEdgeList(const vector<CIsPoint3d>& oEdgeList);	/// 2012.02.21 added by humkyung

	CIsVect3d GetLengthDir() const;				/// 2012.02.21 added by humkyung
	CIsVect3d GetWidthDir() const;				/// 2012.02.21 added by humkyung
	CIsVect3d GetHeightDir() const;				/// 2012.02.21 added by humkyung
	int SetLengthDir(const CIsVect3d& dir);		/// 2012.02.21 added by humkyung
	int SetWidthDir(const CIsVect3d& dir);		/// 2012.02.21 added by humkyung
	int SetHeightDir(const CIsVect3d& dir);		/// 2012.02.21 added by humkyung

	double GetAngle() const;					/// 2012.02.21 added by humkyung
	int SetAngle(const double& angle);			/// 2012.02.21 added by humkyung
	STRING_T GetCategoryString() const;			/// 2012.02.21 added by humkyung
	CATEGORY_E GetCategory() const;				/// 2012.02.21 added by humkyung
	int SetCategory(const CATEGORY_E& category);/// 2012.02.21 added by humkyung
	STRING_T GetTypeString() const;				/// 2012.02.21 added by humkyung

	STRING_T GetSubTypeString() const;			/// 2012.02.16 added by humkyung
	ELEMENT_E GetType() const;					/// 2012.02.16 added by humkyung
	SUBTYPE_E GetSubType() const;				/// 2012.02.16 added by humkyung
	int SetType(const ELEMENT_E& eType);		/// 2012.02.16 added by humkyung
	int SetSubType(const SUBTYPE_E& eSubType);	/// 2012.02.16 added by humkyung

	CIsPoint3d Get3rdPoint() const;				/// 2012.02.16 added by humkyung
	CIsPoint3d endPoint() const;
	CIsPoint3d startPoint() const;
	double height() const;
	double width1() const;
	double width2() const;

	int Set3rdPoint(const CIsPoint3d& pt);	/// 2012.02.16 added by humkyung
	int SetEndPoint(const CIsPoint3d& pt);
	int SetStartPoint(const CIsPoint3d& pt);
	int SetHeight(const double& height);
	int SetWidth1(const double& width);
	int SetWidth2(const double& width);		/// 2012.02.16 added by humkyung

	CaDraw_CableTray(void);
	~CaDraw_CableTray(void);

	ADRAW_DECLARE_FUNC()
private:
	ELEMENT_E m_eType;
	SUBTYPE_E m_eSubType;
	CATEGORY_E m_eCategory;
	
	CIsPoint3d m_pt[3];
	CIsVect3d  m_widthDir , m_heightDir , m_lengthDir;
	double m_width[2];
	double m_height;
	
	double m_dAngle;					/// for elbow type
	vector<CIsPoint3d>* m_pEdgeList;	/// for reduce type
};
