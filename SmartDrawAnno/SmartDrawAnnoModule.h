#pragma once

#include <boost/pool/object_pool.hpp>
#include <VFTFile.h>
#include "CellMap.h"
#include "SmartDrawAnnoItem.h"
#include "SmartDrawAnnoTextStyle.h"

#include "Drawing.h"

#include <map>
using namespace std;

class CSmartDrawAnnoModule
{
	CSmartDrawAnnoModule(void);
public:
	static CSmartDrawAnnoModule& GetInstance(void);
	~CSmartDrawAnnoModule(void);

	static STRING_T CSVString(const STRING_T& str);	/// 2012.03.25 added by humkyung

	static const TCHAR* METRIC2INCH(double in_mm);
	static const TCHAR* INCH2STR(const double &unit);
	static const TCHAR* UNIT2STR(const double &unit,const UINT_TYPE_E sub_unit = MM );
	CaDraw_TagItem* new_titem_t();
	CaDraw_TagPos* new_tag_pos();
private:
	static const TCHAR* M_FRACTION(int n1,int n2);

	static boost::object_pool<CaDraw_TagItem>* m_pCaDraw_TagItemPool;
	static boost::object_pool<CaDraw_TagPos>*  m_pCaDraw_TagPosPool;
public:
	int Clear(void);

	static CDrawing* CreateDrawingOf(const STRING_T& sDrawingType);	/// 2012.03.02 added by humkyung

	static double UNIT;
	static CaDraw_TagEntity* CreateTagEntityOf(CaDraw_Entity* pEnt);
	static CCellMap *m_pCellMap;
	static map<STRING_T , CVFTFile*> m_VFTFontMap;
};
