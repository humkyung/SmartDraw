// Element.h: interface for the CHSRElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENT_H__CF303C0F_044C_4A7E_A412_78AD7A2329A9__INCLUDED_)
#define AFX_ELEMENT_H__CF303C0F_044C_4A7E_A412_78AD7A2329A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <IsString.h>
#include <IsVolume.h>
#include "HSRFace.h"
#include "HSRScanner.h"

#include <string>
using namespace std;

#ifndef __DLL_EXPORT__
#define __DLL_EXPORT__
#endif

#define	DECLARE_HSR_ELEMENT(classname)\
public:\
	virtual bool IsKindOf(const STRING_T& typeString) const;\
	static CHSRElement* CreateInstance();\
	static int DeleteInstance(classname* p);\
	CHSRElement* Clone();\
	virtual STRING_T GetTypeString() const;\
	static const STRING_T TypeString();\

#define	IMPLEMENTS_HSR_ELEMENT(classname,baseclassname,typestr)\
CHSRElement* classname::CreateInstance()\
{\
	CHSRElementFactory& factory = CHSRElementFactory::GetInstance();\
	CHSRElement* pElm = factory.CreateElement(typestr);\
	if(NULL == pElm)\
	{\
		classname* p = new classname;\
		return p;\
	}\
	return pElm;\
}\
int classname::DeleteInstance(classname* p)\
{\
	assert(p && "p is NULL");\
	if(p)\
	{\
		delete p;\
		p = NULL;\
	}\
	return ERROR_SUCCESS;\
}\
CHSRElement* classname::Clone()\
{\
	classname* p = new classname;\
	if(p) p->Copy(this);\
	return p;\
}\
bool classname::IsKindOf(const STRING_T& typeString) const\
{\
	if(classname::TypeString() == typeString) return true;\
	return baseclassname::IsKindOf(typeString);\
}\
STRING_T classname::GetTypeString() const\
{\
	return classname::TypeString();\
}\
const STRING_T classname::TypeString()\
{\
	static const STRING_T __typeString__(typestr);\
	return __typeString__;\
}\

class CHSRViewFormat;
class __DLL_EXPORT__ CHSRElement  
{
public:
	typedef enum
	{
		PIPE_MODEL_T		= 0x01,
		EQUIPMENT_MODEL_T	= 0x02,
		STRUCTURE_MODEL_T	= 0x03,
		CABLE_T_MODEL_T		= 0x05,
		DRAWING_MODEL_T		= 0x06,
		KEY_PLAN_MODEL_T	= 0x07,
		BORDER_MODEL_T		= 0x08,
		MISC_MODEL_T		= 0xFF
	}MODEL_TYPE_T;

	typedef struct
	{
		MODEL_TYPE_T modelType;
		STRING_T rModelName;
	}MODEL_T;

	CHSRElement();
	virtual ~CHSRElement();
public:
	virtual bool ParseLine(CHSRScanner* pPreFormatScanner)=0;
	virtual bool IsInVolume(const CIsVolume& volume)=0;
	virtual CHSRFace* CreateFace(CHSRViewFormat* pVIEWFormat,long& nID)=0;
	virtual int Copy(CHSRElement* pCopied);
	virtual STRING_T GetTypeString() const;
	static STRING_T TypeString();
	virtual bool IsKindOf(const STRING_T& typeString) const
	{
		return (STRING_T(TypeString()) == typeString);
	}
	void SetLayerString(const STRING_T& strLayer)
	{
		(*m_pstrLayer) = strLayer;
	}
	STRING_T GetLayerString() const
	{
		return (*m_pstrLayer);
	}
	void SetColorString(const STRING_T& strColor)
	{
		(*m_pstrColor) = strColor;
	}
	STRING_T GetColorString() const
	{
		return (*m_pstrColor);
	}
	HSR_LAYER_E GetHSRLayer() const;
	VOLUME_T volume() const
	{
		return m_vol;
	}

	static bool IsSamePoint(const POINT_T& pt1,const POINT_T& pt2);
public:
	static const double m_tol;
	MODEL_T  m_model;
protected:
	STRING_T* m_pstrLayer;
	STRING_T* m_pstrColor;
	VOLUME_T m_vol;
};

#endif // !defined(AFX_ELEMENT_H__CF303C0F_044C_4A7E_A412_78AD7A2329A9__INCLUDED_)
