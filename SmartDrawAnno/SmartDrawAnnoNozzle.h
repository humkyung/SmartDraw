#pragma once

#include <aDraw_Nozzle.h>
#include "SmartDrawAnnoEntity.h"

//namespace tag_entity
//{
class CaDraw_TagNozzle : public CaDraw_TagEntity
{
public:
	CaDraw_Nozzle* nozzle() const
	{
		return static_cast<CaDraw_Nozzle*>(m_pDrawEntity);
	}

	int Annotate();
	int Write(OFSTREAM_T& ofile , const double& dDrawingScale);

	CaDraw_TagNozzle(CaDraw_Entity* unnamed);
	~CaDraw_TagNozzle(void);

	SMARTDRAW_TAG_DECLARE_FUNC()
};
//};