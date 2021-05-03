#pragma once

#include <aDraw_Entity.h>
#include "Enum.h"
#include "aDraw_EntTagShape.h"

#include <list>
using namespace std;

#ifdef ADRAW_TAG_EXPORTS
	#define ON_TAG_EXT_DLL		__declspec(dllexport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern
#else
	#define ON_TAG_EXT_DLL		__declspec(dllimport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern __declspec(dllimport)
#endif

class CSmartDrawAnnoView;
class CaDraw_TagItem;
#define SMARTDRAW_TAG_DECLARE_FUNC()\
public:\
	virtual bool IsKindOf(const TCHAR* typeString);\
	virtual CaDraw_TagEntity* Clone();\
	virtual CaDraw_TagItem* CreateTagObjects(list<CaDraw_TagItem*>& TagItemList , CSmartDrawAnnoView *pAnnoView);\
	virtual CaDraw_TagItem* OnCreateTagObject(CSmartDrawAnnoView *pAnnoView , const CIsPoint3d& ptOrigin,const CIsVect3d& vec,const DITEM_E& enmType);\
	static const TCHAR* TypeString();\
	STRING_T typeString() const;\
	static CaDraw_TagEntity* CreateInstance(CaDraw_Entity* unnamed);\

#define	SMARTDRAW_TAG_IMPLEMENT_FUNC(classname,baseclassname,typestr) \
bool classname::IsKindOf(const TCHAR* typeString)\
{\
	if(0 == STRCMP_T(classname::TypeString() , typeString)) return true;\
	return baseclassname::IsKindOf(typeString);\
}\
CaDraw_TagEntity* classname::Clone()\
{\
	classname* p = new classname(NULL);\
	p->Copy(this);\
       return p;\
}\
const TCHAR* classname::TypeString()\
{\
	return typestr;\
}\
STRING_T classname::typeString() const\
{\
	return typestr;\
}\
CaDraw_TagEntity* classname::CreateInstance(CaDraw_Entity* unnamed)\
{\
	return new classname(unnamed);\
}\

class ON_TAG_EXT_DLL CaDraw_TagEntity
{
public:
	CaDraw_View* GetView() const;	/// 2012.02.22 added by humkyung

	virtual int Write(OFSTREAM_T& ofile,const double& dDrawingScale){return ERROR_SUCCESS;}
	virtual int Annotate(){return ERROR_SUCCESS;}
	CIsPoint3d ModelCoord2ViewCoord(const CIsPoint3d& pt);
	virtual void Copy(CaDraw_TagEntity* unnamed);

	CaDraw_TagEntity(CaDraw_Entity* unnamed);
	virtual ~CaDraw_TagEntity(void);

	SMARTDRAW_TAG_DECLARE_FUNC()
public:
	bool m_bTagged;
	CaDraw_EntTagShape m_oEntTagShape;
protected:
	CaDraw_Entity* m_pDrawEntity;

	STRING_T* m_pTypeString;
};