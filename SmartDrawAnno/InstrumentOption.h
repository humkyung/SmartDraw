#pragma once

#include "aDraw_EntTagShape.h"
#include "SmartDrawAnnoTextStyle.h"

namespace tag_option
{
class ON_TAG_EXT_DLL CInstrumentOption
{
public:
	int Parse(const STRING_T& sIniFilePath);

	CInstrumentOption(void);
	~CInstrumentOption(void);
public:
	int    Tag;     // 0:No Tag 1:Tag	
	int    Level;
	int    Name_Type; 
	STRING_T m_sDelimiter;
	bool   m_bEnclosureCenterLine;
	CSmartDrawAnnoTextStyle m_oTextStyle;
	CaDraw_EntTagShape::EnclosureType m_iEnclosureType;
	double m_dEnclosureSizeA , m_dEnclosureSizeB;
	CompProp m_oLabelProp;
	CompProp m_oLeaderProp;
	
	STRING_T m_sArrowType;
	double   m_dArrowSize;
};
};