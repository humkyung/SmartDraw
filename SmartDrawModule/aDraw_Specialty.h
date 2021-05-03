#pragma once
#include "adraw_entity.h"

class ON_MODULE_EXT_DLL CaDraw_Specialty :	public CaDraw_Entity
{
public:
	CaDraw_Specialty(void);
	~CaDraw_Specialty(void);

	bool IsEnable(AnnoFilters* ) const;

	int Report(soci::session&,const STRING_T&) const;	/// 2016.02.25 added by humkyung

	ADRAW_DECLARE_FUNC()
};
