#pragma once

#include <aDraw_Pipe.h>
#include "CellMap.h"
#include "SmartDrawAnnoentity.h"
#include "ExtDivision.h"

class ON_TAG_EXT_DLL CaDraw_TagPipe : public CaDraw_TagEntity
{
public:
	typedef enum tagArea
	{
		INSIDE_OF_PIPE	= 0x01,
		OVER_PIPE		= 0x02
	}TAG_AREA;

	class ON_TAG_EXT_DLL HATCHING_ENV
	{
		HATCHING_ENV(void);
		HATCHING_ENV(const HATCHING_ENV&);
	public:
		typedef enum Type
		{
			NONE			= 0x00,
			DEPEND_ON_SIZE	= 0x01,
			ALL				= 0x02
		};

		static HATCHING_ENV& GetInstance();
		int Parse(const STRING_T& sIniFilePath);
		
		~HATCHING_ENV(void);
	public:
		Type m_eHatchingType;
		int m_iHatchingTypeA , m_iHatchingTypeB;
		CompSize m_oSize;
		CompProp m_oHatchingProp;
	};

	CaDraw_Pipe* pipe() const
	{
		return static_cast<CaDraw_Pipe*>(m_pDrawEntity);
	}

	bool IsVertical() const;
	bool IsHorizontal() const;
	STRING_T GetLineLabel() const;
	void GetTagNo(STRING_T* pTagNo,const CIsPoint3d& pt,CaDraw_View* pView);

	CaDraw_TagPipe(CaDraw_Entity* unnamed);
	~CaDraw_TagPipe(void);

	SMARTDRAW_TAG_DECLARE_FUNC()
public:
	CIsPoint3d m_ptTag;							/// view coordinates except z value(z value is still model value)
	int FillSectionMark();						/// 2014.04.13 added by humkyung
	int WriteSectionMark(OFSTREAM_T& ofile,const double& dDrawingScale);	/// 2014.04.13 added by humkyung
	bool TryToPlaceTagAlongPipe(CCellMap::SearchRes& res , STRING_T& sLabelNo , STRING_T& sElev , const TAG_AREA& eTagArea , const int& nMinOccupied);
	int ClipByViewBoundary(CIsPoint3d pt[2]);
	int CalculateTagItemRect(STRING_T& sLabelNo , STRING_T& sElev , double& dItemWidth , double& dItemHeight);	/// 2012.03.19 added by humkyung
private:
	bool ExamineToDisplaySectionMark() const;		/// 2014.04.13 added by humkyung
	STRING_T GetElevString( const CIsPoint3d& pt );	/// 2012.03.19 added by humkyung
};