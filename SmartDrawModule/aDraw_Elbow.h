#pragma once
#include "adraw_entity.h"

class ON_MODULE_EXT_DLL CaDraw_Elbow :
	public CaDraw_Entity
{
public:
	CaDraw_Elbow(void);
	~CaDraw_Elbow(void);

	void Copy(CaDraw_Elbow* unnamed);

	bool IsEnable(AnnoFilters* ) const;

	const TCHAR* conn_name1() const;
	const TCHAR* conn_name2() const;
	int SetConnName1(const STRING_T& name);
	int SetConnName2(const STRING_T& name);
	ADRAW_DECLARE_FUNC()
private:
	STRING_T* m_pConnName[2];
};
