#pragma once

#include <aDraw_Instrument.h>
#include "SmartDrawAnnoEntity.h"

//namespace tag_entity
//{
class CaDraw_TagInstrument : public CaDraw_TagEntity
{
public:
	CaDraw_Instrument* inst() const
	{
		return static_cast<CaDraw_Instrument*>(m_pDrawEntity);
	}

	int Write(OFSTREAM_T& ofile,const double& dDrawingScale);
	int Annotate();

	CaDraw_TagInstrument(CaDraw_Entity* unnamed);
	~CaDraw_TagInstrument(void);

	SMARTDRAW_TAG_DECLARE_FUNC()
};
//};