#pragma once
#include <aDraw_View.h>
#include <aDraw_Structure.h>
#include "SmartDrawAnnoEntity.h"

//namespace tag_entity
//{
class ON_TAG_EXT_DLL CaDraw_TagStru : public CaDraw_TagEntity
{
public:
	CaDraw_TagStru(CaDraw_Entity* unnamed);
	~CaDraw_TagStru(void);

	CaDraw_Structure* stru() const
	{
		return static_cast<CaDraw_Structure*>(m_pDrawEntity);
	}

	SMARTDRAW_TAG_DECLARE_FUNC()
private:
	CIsVect3d GetTagDir(CaDraw_View::DIRECTIONT_T type,const int& nIndex);
};
//};