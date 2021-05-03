#pragma once
#include "adraw_entity.h"

class ON_MODULE_EXT_DLL CaDraw_Valve : public CaDraw_Entity
{
public:
	CaDraw_Valve(void);
	~CaDraw_Valve(void);

	void Copy(CaDraw_Valve* unnamed);

	bool IsEnable(AnnoFilters* ) const;

	LPCTSTR remarks() const;
	int Report(soci::session&,const STRING_T&) const;	/// 2016.02.24 added by humkyung

	ADRAW_DECLARE_FUNC()
private:
	STRING_T *m_pRemarks;
};
