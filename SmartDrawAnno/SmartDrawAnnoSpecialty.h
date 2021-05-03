#pragma once

#include <aDraw_Specialty.h>
#include "SmartDrawAnnoEntity.h"

class CSmartDrawAnnoSpecialty : public CaDraw_TagEntity
{
public:
	class ON_TAG_EXT_DLL ENV
	{
	public:
		int Parse(const STRING_T& sIniFilePath);

		ENV(void);
		~ENV(void);
	public:
		int    Tag;     // 0:No Tag 1:Tag	
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

	CaDraw_Specialty* entity() const
	{
		return static_cast<CaDraw_Specialty*>(m_pDrawEntity);
	}

	int Write(OFSTREAM_T& ofile,const double& dDrawingScale);
	int Annotate();

	CSmartDrawAnnoSpecialty(CaDraw_Entity* unnamed);
	~CSmartDrawAnnoSpecialty(void);

	SMARTDRAW_TAG_DECLARE_FUNC()
};
