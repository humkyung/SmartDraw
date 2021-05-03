#pragma once

#include <aDraw_Valve.h>
#include "SmartDrawAnnoEntity.h"

class CSmartDrawAnnoValve : public CaDraw_TagEntity
{
public:
	class ON_TAG_EXT_DLL ENV
	{
	public:
		typedef enum
		{
			PIPING_COMP_NO	= 0x01,
			REMARKS			= 0x02,
		}eDisplayName;

		ENV(void);
		~ENV(void);

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

	CaDraw_Valve* valve() const
	{
		return static_cast<CaDraw_Valve*>(m_pDrawEntity);
	}

	int Write(OFSTREAM_T& ofile,const double& dDrawingScale);
	int Annotate();

	CSmartDrawAnnoValve(CaDraw_Entity* unnamed);
	~CSmartDrawAnnoValve(void);

	SMARTDRAW_TAG_DECLARE_FUNC()
};