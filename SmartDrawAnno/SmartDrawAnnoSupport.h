#pragma once

#include <aDraw_Support.h>
#include "SmartDrawAnnoEntity.h"

//namespace tag_entity
//{
class CaDraw_TagSupport : public CaDraw_TagEntity
{
public:
	CaDraw_Support* support() const
	{
		return static_cast<CaDraw_Support*>(m_pDrawEntity);
	}

	int Annotate();
	int Write(OFSTREAM_T& ofile,const double& dDrawingScale);

	CaDraw_TagSupport(CaDraw_Entity* unnamed);
	~CaDraw_TagSupport(void);

	SMARTDRAW_TAG_DECLARE_FUNC()
};
//};