#pragma once
#include "adraw_entity.h"

class ON_MODULE_EXT_DLL CaDraw_Support : public CaDraw_Entity
{
public:
	CaDraw_Support(void);
	~CaDraw_Support(void);

	void Copy(CaDraw_Support* unnamed);

	bool IsEnable(AnnoFilters* ) const;

	const TCHAR* line_no() const;
	const TCHAR* shop_name() const;
	const TCHAR* field_name() const;
	const TCHAR* commodity_code() const;
	int SetLineNo(const STRING_T& name);
	int SetShopName(const STRING_T& name);
	int SetFieldName(const STRING_T& name);
	int SetCommodityCode(const STRING_T& code);

	int Report(soci::session&,const STRING_T&) const;	/// 2016.02.25 added by humkyung

	ADRAW_DECLARE_FUNC()
private:
	STRING_T *m_pLineNo;
	STRING_T *m_pShopName , *m_pFieldName;
	STRING_T *m_pCommodityCode;
};
