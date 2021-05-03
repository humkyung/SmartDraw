#pragma once

#include <aDraw_View.h>

#include "Enum.h"
#include "AnnoOption.h"
#include "OutsideOption.h"
#include "GenerationOption.h"
#include "PipeOption.h"
#include "NozzleOption.h"
#include "ReducerOption.h"
#include "InstrumentOption.h"
#include "SupportOption.h"
#include "SmartDrawAnnoCableTray.h"
#include "SmartDrawAnnoPlatform.h"
#include "SmartDrawAnnoReducer.h"
#include "SmartDrawAnnoValve.h"
#include "SmartDrawAnnoSpecialty.h"

#include <string>
using namespace tag_option;
using namespace std;

#ifdef ADRAW_TAG_EXPORTS
	#define ON_TAG_EXT_DLL		__declspec(dllexport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern
#else
	#define ON_TAG_EXT_DLL		__declspec(dllimport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern __declspec(dllimport)
#endif

class ON_TAG_EXT_DLL AREA_T
{
public:
	int Parse(const STRING_T& rIniFilePath);

	double LineNoRange[4] , Range[4];
	int m_iExtTagSide;
};

//--> structure environment
typedef struct ON_TAG_EXT_DLL tagStrEnv
{
	int Parse(const STRING_T& rIniFilePath);
	bool IsNorthSouth_EastWestNameDir();
	bool IsEastWest_NorthSouthNameDir();

	bool   bTaggingOverlap;
	int    nTagDir;
	double nTextHeight[2];		///< 텍스트가 2개일 경우 텍스트 각각의 높이.
	
	CSmartDrawAnnoTextStyle m_oTextStyle;
	CompProp m_oLabelProp , m_oLeaderProp;

	double nDist;
	double m_dRadius;
	STRING_T m_sColumnNameDir;	/// column name direction
	TCHAR  szDelimiter[2];		/// delimiter for direction
	TCHAR  szLine2Delimiter[2]; /// structure의 텍스트를 구분하는 구분자. default value = ' '
}STR_ENV_T;
//<--

//--> equipment environment
class ON_TAG_EXT_DLL EQP_ENV_T
{
public:
	EQP_ENV_T();
	int Parse(const STRING_T& sAppName , const STRING_T& rIniFilePath);
	
	TEXT_ALIGN_E GetTopSideAlignment() const;
	TEXT_ALIGN_E GetBottomSideAlignment() const;
	TEXT_ALIGN_E GetLeftSideAlignment() const;
	TEXT_ALIGN_E GetRightSideAlignment() const;

	EQP_TAG_E m_eTagStyle;
	bool bTag;			/// tag on/off - 2012.02.20 added by humkyung
	bool m_bWriteCoordinate;
	bool m_bWriteDesc;
	bool m_bCenterLine;
	CSmartDrawAnnoTextStyle m_oNameTextStyle;
	CSmartDrawAnnoTextStyle m_oCoordTextStyle;
	double m_dOffset;

	CompProp m_oNameProp , m_oCoordProp , m_oLeaderProp , m_oCenterlineProp;
private:
	TEXT_ALIGN_E m_enmTextAlign[4];	/// text alignment for external
};
//<--

typedef struct ON_TAG_EXT_DLL tagElevEnvT
{
	int Parse(const STRING_T& sIniFilePath);

	//bool m_bWriteElev;

	CSmartDrawAnnoTextStyle m_oTextStyle;
	CompProp m_oLabelProp;

	double m_dDistA;
	TCHAR  m_szJustify[16];
}ELEV_ENV_T;

typedef struct ON_TAG_EXT_DLL tagMatchEnvT
{
	int Parse(const STRING_T& rIniFilePath);

	CSmartDrawAnnoTextStyle m_oTextStyle;
	CompProp m_oLabelProp;
	double nMatchDistA;
	TCHAR  m_szJustify[16];
}MATCH_ENV_T;

class ON_TAG_EXT_DLL DIM_STYLE_ENV_T
{	
public:
	DIM_STYLE_ENV_T();
	~DIM_STYLE_ENV_T();

	int Write(OFSTREAM_T& ofile , double dDrawingScale);	/// 2012.03.18 added by humkyung
	int Parse(const STRING_T& sDimStyle , const STRING_T& rIniFilePath);
public:
	STRING_T m_sDimStyle;	/// setted by outside - 2012.03.18 added by humkyung

	STRING_T rLevel;
	STRING_T rArrowType;
	STRING_T rArrowSize;
	STRING_T rExtLineColor;
	STRING_T rDimLineDistance;
	STRING_T rDimLineColor;
	int m_iTextWeight;
	STRING_T rTextOffset;
	STRING_T rTextColor;
	STRING_T rTextStyle;
	int m_iTextPrecision;
	STRING_T m_sFontName;
	double	nTextHeight , m_dWidthFactor;
	double	nLineExt;
	double	nExtLineOffset;
	bool	bRotation;
	bool	m_bViewLimitOn;
private:
	bool    m_bAutoTextSize;	/// 2014.11.16 added by humkyung
};

class ON_TAG_EXT_DLL COORD_ENV_T
{
public:
	int Parse(const STRING_T& sIniFilePath);
	bool IsBoxOut() const;

	double m_dLabelOffset;
	CaDraw_View::COORD_OPTION_E m_eOption;
	
	CSmartDrawAnnoTextStyle m_oTextStyle;
	CompProp m_oLabelProp;
private:
	bool m_bCoordBoxOut;
};

typedef struct ON_TAG_EXT_DLL tagLeaderEnvT
{
	int Parse(const STRING_T& rIniFilePath);

	TCHAR szLineStyle[16];
	TCHAR szLineColor[16];
	TCHAR szLineWeight[16];
}LEADER_ENV_T;

class ON_TAG_EXT_DLL CaDraw_TagEnv
{
	CaDraw_TagEnv(void);
	CaDraw_TagEnv(const CaDraw_TagEnv&);
	CaDraw_TagEnv& operator=(const CaDraw_TagEnv&){return (*this);}
public:
	enum
	{
		INCH_FEET	= 0x04,
		METRIC		= 0x08,
	};

	typedef enum
	{
		EW_COORD = 1,
		XY_COORD = 2
	}ANNOTATE_COORD_T;

	static CaDraw_TagEnv& GetInstance(void);
	~CaDraw_TagEnv(void);

	int Parse(const STRING_T& rIniFilePath,const STRING_T&);
public:
	STRING_T m_sIniFilePath;

	double m_nWidthFactor;
	double m_nBaseLineOffset;
	double m_nUnitConvFactor;
	AREA_T m_area;
	CPipeOption m_pipe;
	STR_ENV_T m_stru;	/// setting for structure
	EQP_ENV_T m_equi , m_InsideEqp;
	ELEV_ENV_T m_elev;
	MATCH_ENV_T m_match;
	DIM_STYLE_ENV_T m_intDimStyle , m_extDimStyle;
	CAnnoOption m_anno;
	COutsideOption m_outside;
	CGenerationOption m_generation;
	CNozzleOption  m_nozzle;
	CInstrumentOption m_inst;
	CSupportOption m_support;
	COORD_ENV_T m_coord;
	LEADER_ENV_T m_leader;
	CaDraw_TagReducer::ENV m_reducer;
	CaDraw_TagCableTray::ENV m_cableT;
	CSmartDrawAnnoPlatform::ENV m_platform;
	CSmartDrawAnnoValve::ENV m_valve;
	CSmartDrawAnnoSpecialty::ENV m_specialty;

	ANNOTATE_COORD_T m_eAnnotateCoord;
	double m_nGap;
	double m_nDistText;
	bool   m_EnableOutsideDim;	///< 외부 치수를 기입 여부

	static int OUTPUT_UNIT;
	static int INPUT_UNIT;
	static double TOLERANCE;
};
