// ExternalDim.h: interface for the CExternalTerritory class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ExtDivision.h"
#include "SmartDrawAnnoItem.h"

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

class CAnnoTerritory;
class ON_TAG_EXT_DLL CExternalTerritory  
{
public:
	CExternalTerritory(/*CAnnoTerritory* pTerritory*/);
	virtual ~CExternalTerritory();
public:
	int SetAnnoTerritory(CAnnoTerritory* Territory);
	void Write(OFSTREAM_T& file,const double& dDrawingScale);

	bool operator +=(CExtDivision* pDivision);

	void AddTagItem(CaDraw_TagItem* pTagItem)
	{
		m_plstTagItem->push_back(pTagItem);
	}

	list<CExtDivision*>* GetDivisionList(){return m_plstDivision;}
protected:
	void StoreStrToDivision(CaDraw_TagItem* pDITEM);
private:
	CAnnoTerritory* m_pTerritory;
	list<CaDraw_TagItem*>* m_plstTagItem;	//! external용으로 생성한 모든 tag item을 가지고 있다.

	list<CIsLine3d>* m_plstEqpCenterline;
	list<CExtDivision*>* m_plstDivision;
public:
	virtual int WriteViewInfo(OFSTREAM_T& ofile,const double& dDrawingScale);	/// 2012.03.25 added by humkyung
	int Annotate(void);
private:
	bool IsSecTagBoundary(CViewBoundaySegment* pVBS);
	int CreateDivisions(void);
};
