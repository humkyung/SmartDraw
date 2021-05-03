#pragma once

#include <aDraw_Reducer.h>
#include "SmartDrawAnnoEntity.h"

//namespace tag_entity
//{
class ON_TAG_EXT_DLL CaDraw_TagReducer : public CaDraw_TagEntity
{
public:
	class ON_TAG_EXT_DLL ENV
	{
	public:
		int Parse(const STRING_T& sIniFilePath);

		STRING_T m_sNameStyle;
		STRING_T m_sECCLabeling;
		STRING_T m_sCONLabeling;
		int m_iSizeUnit;	/// 2016.02.11 added by humkyung
		STRING_T m_sArrowType;
		double m_dArrowSize;
		CompProp m_oLeaderProp;
		CSmartDrawAnnoTextStyle m_oTextStyle;
		double m_dLabelOffset;
		CompProp m_oLabelProp;
	};

	CaDraw_Reducer* reducer() const
	{
		return static_cast<CaDraw_Reducer*>(m_pDrawEntity);
	}

	int Write(OFSTREAM_T& ofile,const double& dDrawingScale);
	int Annotate();
	STRING_T name(const bool& csvf=false) const;

	CaDraw_TagReducer(CaDraw_Entity* unnamed);
	~CaDraw_TagReducer(void);

	SMARTDRAW_TAG_DECLARE_FUNC()
};
//};