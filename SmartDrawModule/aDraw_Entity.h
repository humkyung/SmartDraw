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

#include <IsTools.h>
#include <IsVolume.h>

#include "AnnoFilter.h"

#include <soci.h>
#include <soci-sqlite3.h>

#include <algorithm>
#include <functional>
using namespace std;

#define ADRAW_DECLARE_FUNC()\
public:\
	virtual bool IsKindOf(const TCHAR* typeString) const;\
	virtual CaDraw_Entity* Clone();\
	static const TCHAR* TypeString();\
	virtual STRING_T typeString() const;\
	static CaDraw_Entity* CreateInstance();\

#define	ADRAW_IMPLEMENT_FUNC(classname,baseclassname,typestr) \
bool classname::IsKindOf(const TCHAR* typeString) const\
{\
	if(0 == STRCMP_T(classname::TypeString() , typeString)) return true;\
	return baseclassname::IsKindOf(typeString);\
}\
CaDraw_Entity* classname::Clone()\
{\
	classname* p = new classname;\
	p->Copy(this);\
       return p;\
}\
const TCHAR* classname::TypeString()\
{\
	return typestr;\
}\
STRING_T classname::typeString() const\
{\
	return classname::TypeString();\
}\
CaDraw_Entity* classname::CreateInstance()\
{\
	return new classname;\
}\

class CaDraw_View;
class ON_MODULE_EXT_DLL CaDraw_Entity
{
public:
	struct DataFinder : public std::unary_function<CaDraw_Entity* , bool>
	{
		DataFinder(CaDraw_Entity* pEnt) : m_pEnt(pEnt){}
		bool operator()(const CaDraw_Entity* pEnt) const 
		{
			return (m_pEnt->Equal(pEnt));
		}

		CaDraw_Entity* m_pEnt;
	};

	typedef enum
	{
		NONE_MODEL_T		= 0x00,
		PIPE_MODEL_T		= 0x01,
		EQUIPMENT_MODEL_T	= 0x02,
		STRUCTURE_MODEL_T	= 0x03,
		CABLE_T_MODEL_T		= 0x05,
		DRAWING_MODEL_T		= 0x06,
		KEY_PLAN_MODEL_T	= 0x07,
		BORDER_MODEL_T		= 0x08,
		SITE_MODEL_T		= 0x09,	/// 2015.06.02 added by humkyung
		ZONE_MODEL_T		= 0x0a,	/// 2015.06.02 added by humkyung
		MISC_MODEL_T		= 0xFF
	}MODEL_TYPE_T;

	CaDraw_Entity(void);
	virtual ~CaDraw_Entity(void);

	static STRING_T GetModelTypeString(const MODEL_TYPE_T& iModelType);	/// 2012.02.11 added by humkyung
	static CaDraw_Entity::MODEL_TYPE_T GetModelType(const STRING_T& rModelType);

	virtual bool IsEnable(AnnoFilters* ) const;					/// 2016.03.19 added by humkyung
	virtual int Report(soci::session&,const STRING_T&) const;	/// 2012.02.15 added by humkyung
	virtual bool Equal(const CaDraw_Entity* pEnt) const;		/// 2012.02.16 added by humkyung

	CIsPoint3d origin() const;
	const TCHAR* name() const;
	int SetName(const STRING_T& name);
	int SetOrigin(const CIsPoint3d& origin);
	CaDraw_View* GetView();
	void SetView(CaDraw_View* pView);
	void SetVolume(const CIsVolume& volume);
	CIsVolume GetVolume() const;
	void SetModelType(const MODEL_TYPE_T& eModelType);
	MODEL_TYPE_T GetModelType() const;
	int SetModelName(const STRING_T& rModelName);
	STRING_T GetModelName() const;
	
	virtual void Copy(CaDraw_Entity* unnamed);
	ADRAW_DECLARE_FUNC()
protected:
	MODEL_TYPE_T m_eModelType;
	STRING_T *m_pName , *m_pModelName;
	CIsVolume m_volume;
	CIsPoint3d m_ptOrigin;

	CaDraw_View* m_pView;
};
