#pragma once
#include "adraw_entity.h"
#include "aDraw_Nozzle.h"

#include <IsPoint3d.h>
#include <string>
#include <list>
using namespace std;

class ON_MODULE_EXT_DLL CaDraw_Equipment : public CaDraw_Entity
{
public:
	typedef struct CylinderShape
	{
		CIsPoint3d top,bottom;
		double radius;
	};

	CaDraw_Equipment(void);
	virtual ~CaDraw_Equipment(void);
	
	bool IsEnable(AnnoFilters* ) const;
	int ClearNozzleList();
	int AddNozzle(CaDraw_Nozzle*);

	void Copy(CaDraw_Equipment* unnamed);
	ADRAW_DECLARE_FUNC()
private:
	STRING_T* m_pName2;		/// name1 , name2
	STRING_T* m_pDesc;		/// description

	list<CylinderShape>* m_pShapeList;		/// 2014.04.17 added by humkyung
	list<CaDraw_Nozzle*>* m_pNozzleList;	/// 2016.03.21 added by humkyung
public:
	/// 2014.04.17 added by humkyung
	list<CylinderShape>* GetShapeList();
	int AppendShape(const CylinderShape& shape);
	/// up to here
	
	int Report(soci::session&,const STRING_T&) const;	/// 2012.02.15 added by humkyung

	const TCHAR* desc() const;
	const TCHAR* name2() const;
	int SetDesc(const STRING_T& rDesc);
	int SetName2(const STRING_T& rName);
};
