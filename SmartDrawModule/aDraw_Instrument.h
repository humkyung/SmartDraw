#pragma once
#include "adraw_entity.h"

class ON_MODULE_EXT_DLL CaDraw_Instrument :
	public CaDraw_Entity
{
public:
	CaDraw_Instrument(void);
	~CaDraw_Instrument(void);

	bool IsEnable(AnnoFilters* pAnnoFilters) const;

	int Report(soci::session&,const STRING_T&) const;	/// 2016.02.25 added by humkyung

	ADRAW_DECLARE_FUNC()
};
