#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Enum.h"
#include <string>
#include <map>
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

typedef enum tagOutputFormat
{
	MSTN    = 0,
	AUTOCAD	= 1
}OUTPUT_FORMAT_E;

typedef enum tagExtTagItems
{
	TAG_NO       = 0,
	TAG_ALL      = 1,
	TAG_ONLY_PIPE= 2,
	TAG_PIPE_EQP = 3,
	TAG_PIPE_STR = 4,
	TAG_NO_PIPE  = 5
}EXT_TAG_ITEMS_E;

namespace tag_option
{
class ON_TAG_EXT_DLL CGenerationOption  
{
public:
	typedef enum
	{
		ON = 0x01,
		OFF = 0x02,
		OFF_IF_HAS_NO_NOZZLE = 0x03
	}eGenerationType;

	typedef enum
	{
		SEARCH_HORIZONTAL = 0x01,
		SEARCH_VERTICAL   = 0x02,
		SEARCH_ORGHOGONAL = 0x03,
		SEARCH_AROUND     = 0x04
	}eSearchType;

	typedef enum
	{
		SEARCH_NEAR   = 0x01,
		SEARCH_MIDDLE = 0x02,
		SEARCH_FAR    = 0x03
	}eSearchDist;

	typedef enum
	{
		LOCATION_ORDER1 = 0x01,
		LOCATION_ORDER2 = 0x02,
		LOCATION_ORDER3 = 0x03
	}eLocationOrder;

	class CGenerationItem
	{
	public:
		CGenerationItem();
		~CGenerationItem();

		bool IsEnable(CaDraw_Entity*) const;
		int Parse(vector<STRING_T>&,const STRING_T&,const STRING_T&);

		eGenerationType eGeneration;
		eSearchType eType;
		eSearchDist eDist;
	private:
		AnnoFilters* m_pAnnoFilters;
	};

	int GetDimLevelOf(const STRING_T& sItemType);	/// 2012.02.20 added by humkyung

	///@brief	return location order of given item - 2015.01.01 added by humkyung
	eLocationOrder GetLocationOrderOf(const STRING_T& sItemType) const;
	int GetItemListOfLLocationOrder(vector<STRING_T>& aItemList , const eLocationOrder&) const;

	///@brief	check item can be generated - 2014.03.30 added by humkyung
	bool IsEnable(const STRING_T& sItemType,CaDraw_Entity*) const;
	int GetSearchAreaOf(double& dLength , double& dHeight , const STRING_T& sItemType) const;
	eSearchType GetSearchTypeOf(const STRING_T& sItemType) const;
	void Parse(const STRING_T& rIniFilePath,const STRING_T&);
	CGenerationOption();
	virtual ~CGenerationOption();
public:
	OUTPUT_FORMAT_E m_eOutputFormat;
	GLOBAL_DIMENSION_E m_global_dim;
	EXT_TAG_ITEMS_E m_eTagItems;

	vector< STRING_T >* m_pLevelOnItemEntry[3];	/// 2012.02.20 added by humkyung
private:
	map<STRING_T , CGenerationItem*>* m_pGenerationItemMap;	/// 2014.03.30 added by humkyung
	vector< STRING_T >* m_pLocationOrderEntry[3];			/// 2015.01.01 added by humkyung
};
};