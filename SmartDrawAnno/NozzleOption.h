#pragma once

#include "aDraw_EntTagShape.h"
#include "SmartDrawAnnoTextStyle.h"

namespace tag_option
{
class ON_TAG_EXT_DLL CNozzleOption
{
public:
	CNozzleOption(void);
	~CNozzleOption(void);

	int Parse(const STRING_T& sIniFilePath);
public:	
	int    Tag;     // 0:No Tag 1:Tag	
	int    Level;
	bool   LeaderOnOff;
	double Text_height;
	double Gap;	
	CSmartDrawAnnoTextStyle m_oTextStyle;
	
	double m_dLabelOffset;
	STRING_T m_sArrowType;
	double m_dArrowSize;
	CompProp m_oLeaderProp , m_oLabelProp;
	
	STRING_T Leader_Color;
	STRING_T Txt_Header;
	STRING_T NozDelimiter;
};
};