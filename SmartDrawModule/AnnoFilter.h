#pragma once

#ifdef ADRAW_MODULE_EXPORTS
	#define ON_MODULE_EXT_DLL	__declspec(dllexport)
	#define	ON_MODULE_EXT_DECL	extern "C" __declspec(dllexport)
	#define ON_MODULE_EXTERN_DECL	extern
#else
	#define ON_MODULE_EXT_DLL	__declspec(dllimport)
	#define	ON_MODULE_EXT_DECL	extern "C" __declspec(dllexport)
	#define ON_MODULE_EXTERN_DECL	extern __declspec(dllimport)
#endif

#include <list>

class ON_MODULE_EXT_DLL CAnnoFilter
{
public:
	CAnnoFilter(const int&,const STRING_T&,const STRING_T&,const STRING_T&);
	CAnnoFilter(const CAnnoFilter&);
	~CAnnoFilter(void);
	CAnnoFilter& operator=(const CAnnoFilter&);

	int GetSeq() const;
	int SetSeq(const int&);
	LPCTSTR GetProperty() const;
	int SetProperty(const STRING_T&);
	LPCTSTR GetOperator() const;
	int SetOperator(const STRING_T&);
	LPCTSTR GetValue() const;
	int SetValue(const STRING_T&);

	bool Evaluate(const STRING_T&) const;
	bool Evaluate(const double&) const;
	bool Evaluate(const bool&) const;

	CAnnoFilter* Clone() const;
	STRING_T ToString() const;
	LPCTSTR ToSql(LPCTSTR psSetting,LPCTSTR psItem) const;
private:
	int m_iSeq;
	STRING_T *m_psProperty,*m_psOperator,*m_psValue;
};

typedef list<CAnnoFilter*> AnnoFilters;
