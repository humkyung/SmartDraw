#pragma once

#include "aDraw_EntTagShape.h"
#include "SmartDrawAnnoTextStyle.h"

namespace tag_option
{
class ON_TAG_EXT_DLL CSupportOption
{
public:
	typedef enum
	{
		SUPPORT_NO		= 0x01,
		SHOP_DETAIL		= 0x02,
		FIELD_DETAIL	= 0x03,
		COMMODITY_CODE	= 0x04
	}eDisplayName;

	CSupportOption(void);
	~CSupportOption(void);

	int Parse(const STRING_T& sIniFilePath);
	eDisplayName GetDisplayName() const;
public:
	int    Tag;     // 0:No Tag 1:Tag	
	int    Level;
	int    Type;
	STRING_T m_sDisplayType;
	CaDraw_EntTagShape::EnclosureType m_iEnclosureType;
	CSmartDrawAnnoTextStyle m_oTextStyle;
	double m_dLabelOffset;
	STRING_T m_sArrowType;
	double m_dArrowSize;

	CompProp m_oLabelProp , m_oLeaderProp;
private:
	STRING_T m_sDisplayName;
};
};