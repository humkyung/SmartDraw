#pragma once

#include "SmartDrawAnnoTextStyle.h"

namespace tag_option
{
class ON_TAG_EXT_DLL CReducerOption
{
public:
	CReducerOption(void);
	~CReducerOption(void);

	int Parse(const string& rIniFilePath);
public:
	int    Tag;     // 0:No Tag 1:Tag	
	int    Level;
	int    Type;
	double Gap;
	CSmartDrawAnnoTextStyle m_TextStyle;
	string Txt_Color;
	string Leader_Color;
	int    Arr_Type;

	string EccLabel;
	string ConLabel;
	int    NameStyle;  // 0: MAIN x RUN ECC(or CON)  1: ECC(or CON) MAIN x RUN
};
};