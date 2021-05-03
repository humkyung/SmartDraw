#pragma once

#include <aDraw_Elbow.h>
#include "SmartDrawAnnoentity.h"

class ON_TAG_EXT_DLL CaDraw_TagElbow : public CaDraw_TagEntity
{
public:
	CaDraw_Elbow* elbow() const
	{
		return static_cast<CaDraw_Elbow*>(m_pDrawEntity);
	}

	CaDraw_TagElbow(CaDraw_Entity* unnamed);
	~CaDraw_TagElbow(void);

	SMARTDRAW_TAG_DECLARE_FUNC()
};