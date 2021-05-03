#pragma once
#include "adraw_entity.h"

class ON_MODULE_EXT_DLL CaDraw_Structure : public CaDraw_Entity
{
public:
	int Report(soci::session&,const STRING_T&) const;	/// 2012.02.15 added by humkyung

	bool Equal(const CaDraw_Entity* pEnt) const;	/// 2012.02.16 added by humkyung
	bool operator==(CaDraw_Structure* rhs) const;	/// 2012.02.16 added by humkyung

	bool IsEnable(AnnoFilters* ) const;

	const TCHAR* name2() const;
	const TCHAR* name1() const;
	int SetName2(const STRING_T& rName);
	int SetName1(const STRING_T& rName);

	CaDraw_Structure(void);
	virtual ~CaDraw_Structure(void);

	void Copy(CaDraw_Structure* unnamed);
	ADRAW_DECLARE_FUNC()
private:
	STRING_T* m_pName;		//! name
};
