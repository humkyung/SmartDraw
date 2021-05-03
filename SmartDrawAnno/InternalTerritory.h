#pragma once
#include <IsLine2d.h>
#include "SmartDrawAnnoPipe.h"
#include "SmartDrawAnnoItem.h"
#include "SmartDrawAnnoElbow.h"
#include "SmartDrawAnnoEqui.h"
#include "SmartDrawAnnoCableTray.h"	/// 2012.02.21 added by humkyung
#include "aDraw_EntTagShape.h"

#include <algorithm>
using namespace std;

class CAnnoTerritory;

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

class ON_TAG_EXT_DLL CLineId
{
public:
	CLineId();
	CLineId(const CLineId& rhs)
	{
		m_pSegmentList = NULL;
		(*this) = rhs;
	}
	CLineId::~CLineId();

	static bool fn_line_id_size_sort(const CLineId& lhs , const CLineId& rhs)
	{
		return (lhs.m_nMaxSize < rhs.m_nMaxSize);
	}

	static bool fn_sort_size(CaDraw_TagPipe* lhs , CaDraw_TagPipe* rhs)
	{
		return (lhs->pipe()->GetSize() < rhs->pipe()->GetSize());
	}

	void SortBySize()
	{
		::stable_sort(m_pSegmentList->begin() , m_pSegmentList->end() , fn_sort_size);
	}

	CLineId& operator=(const CLineId& rhs)
	{
		if(this != &rhs)
		{
			m_bTagged   = rhs.m_bTagged;
			m_line_id   = rhs.m_line_id;
			m_nMaxSize  = rhs.m_nMaxSize;
			m_oEntTagShape = rhs.m_oEntTagShape;
			
			if(NULL == m_pSegmentList) m_pSegmentList = new list<CaDraw_TagPipe*>;
			m_pSegmentList->clear();
			m_pSegmentList->insert(m_pSegmentList->begin() , rhs.m_pSegmentList->begin() , rhs.m_pSegmentList->end());
		}

		return (*this);
	}
	
	int Write(OFSTREAM_T& ofile,const double& dDrawingScale);	/// 2012.03.19 added by humkyung

	STRING_T m_line_id;
	double m_nMaxSize;
	list<CaDraw_TagPipe*>* m_pSegmentList;
	
	bool m_bTagged;
	CaDraw_EntTagShape m_oEntTagShape;

	bool HasTaggedMember() const;	/// 2012.03.25 added by humkyung

	bool TryToPlaceTagInsideOfPipe(const int& nMinOccupied);
	bool TryToPlaceTagAtCornerOfPipe(list<CaDraw_TagElbow*>& TagElbowList);
	bool TryToPlaceTagOverPipe(const int& nMinOccupied);
private:
	int CheckConnectionOfPipe(CaDraw_TagEntity** pStartEnt , CaDraw_TagEntity** pEndEnd , 
		CaDraw_TagPipe* pPipe , 
		list<CaDraw_TagElbow*>& oElbowList , list<CaDraw_TagPipe*>* pPipeList);
};

class ON_TAG_EXT_DLL CInternalTerritory
{
public:
	virtual void Write(OFSTREAM_T& ofile,const double& dDrawingScale);
	virtual int Annotate(list<CIsLine2d>& oHsrLineList);
	int SetAnnoTerritory(CAnnoTerritory* pTerritory);

	CInternalTerritory(/*CAnnoTerritory* pTerritory*/);
	virtual ~CInternalTerritory(void);
protected:
	CAnnoTerritory* m_pTerritory;

	CCellMap* m_pCellMap;
	list<CaDraw_TagEntity*>* m_pTagEntList;	///
	list<CLineId>* m_pLineIdList;			///

	virtual int PrepareCellMap(list<CIsLine2d>& oHiddenLineList);			/// 2012.03.02 added by humkyung
	int AnnotateLineId(list<CaDraw_TagElbow*>& oTagElbowList);
private:
	list<CaDraw_TagItem*>* m_plstTagItem;	/// external용으로 생성한 모든 tag item을 가지고 있다.

	int FillUpCellWithHiddenLine(list<CIsLine2d>& oHiddenLineList);
	int AnnotateEqui(list<CaDraw_TagEqui*>& TagEquiList);
	int AnnotateNozzle();
};
