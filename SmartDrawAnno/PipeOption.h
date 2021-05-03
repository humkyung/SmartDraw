#pragma once

#include "Enum.h"
#include "aDraw_EntTagShape.h"
#include "SmartDrawAnnoTextStyle.h"

#include <map>

namespace tag_option
{
class ON_TAG_EXT_DLL CPipeOption
{
	CPipeOption(const CPipeOption&){}
	CPipeOption& operator=(const CPipeOption&){return (*this);}
public:
	CPipeOption(void);
	~CPipeOption(void);

	bool GetTrimEndDash() const;
	int Parse(const STRING_T& sIniFilePath,const STRING_T&);
	PIPE_ELEV_T GetElevationTypeBySpec(const STRING_T&);	///2016.03.12 added by humkyung

	class ON_TAG_EXT_DLL EXT_T
	{
	public:
		EXT_T();

		int Parse(const STRING_T& sIniFilePath);

		TEXT_ALIGN_E GetTopSideAlignment() const;
		TEXT_ALIGN_E GetBottomSideAlignment() const;
		TEXT_ALIGN_E GetLeftSideAlignment() const;
		TEXT_ALIGN_E GetRightSideAlignment() const;

		STRING_T sTagDisplaySize;	/// 2013.06.16 added by humkyung
		/// 2014.06.14 added by humkyung
		double GetTagDisplaySize() const;
		STRING_T sDimDisplaySize;	
		double GetDimDisplaySize() const;
		/// up to here

		PIPE_TAG_T  eTag;
		PIPE_ELEV_T eElev;

		CSmartDrawAnnoTextStyle m_oLineNoTextStyle , m_oElevTextStyle;
		STRING_T m_sArrowType;	/// 2015.02.13 added by humkyung
		double   m_dArrowSize;	/// 2015.02.13 added by humkyung
		CompProp m_oLineNoProp , m_oElevProp , m_oLeaderProp;
		double m_dLabelOffset;

		bool m_bLeaderLine;

		LINE_NO_CFG_ENV_E aryLineNoCfg[16];
		LINE_NO_NAME2_OPT enmName2Opt; 
		
		TCHAR szPrefix[3][16];
		TCHAR szWPrefix[3][16];
		TCHAR szWP[16];
		
		int  nGroupLevel;
		int  nGroupArrow;

		LINE_NO_TAG_AREA  eTagArea;
		PIPE_SECOND_TAG_TYPE_T eSecondTagType;	/// 2007.11.9 Ãß°¡.
		TCHAR szTracingPrefix[32];				/// 2007.12.1
	private:
		TEXT_ALIGN_E m_enmTextAlign[4];	/// text alignment for external
	};

	class ON_TAG_EXT_DLL INT_T
	{
	public:
		int Parse(const STRING_T& sIniFilePath);

		STRING_T m_sDisplayStyle;		/// should be one of {"Only LineNo" , "LineNo+Elev" , "LineNo/Elev"}
		STRING_T m_sCoordDisplayStyle;	/// should be one of {"Elev" , "Elev+Elev"}
		CSmartDrawAnnoTextStyle m_oLineNoTextStyle , m_oElevTextStyle;
		double m_dOffset;

		STRING_T m_sArrowType;
		double   m_dArrowSize;
		CompProp m_oLeaderProp , m_oLineNoProp , m_oElevProp;
	};

	EXT_T m_ext;
	INT_T m_int;
private:
	std::map<STRING_T,STRING_T>* m_pElevationTypeBySpec;
	bool m_bTrimEndDash;	///2016.04.01 added by humkyung
};
};