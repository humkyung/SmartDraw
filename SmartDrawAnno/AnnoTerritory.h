#pragma once

#include "Enum.h"
#include <IsLine3d.h>
#include <aDraw_DraFile.h>
#include "ExternalTerritory.h"
#include "InternalTerritory.h"

#ifdef ADRAW_TAG_EXPORTS
	#define ON_TAG_EXT_DLL		__declspec(dllexport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern
#else
	#define ON_TAG_EXT_DLL		__declspec(dllimport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern __declspec(dllimport)
#endif

class ON_TAG_EXT_DLL CAnnoTerritory
{
public:
	CAnnoTerritory(CaDraw_LogicView* pLogicView , CInternalTerritory* pIntTerritory = NULL , CExternalTerritory* pExtTerritory = NULL);
	virtual ~CAnnoTerritory(void);

	CExternalTerritory* GetExternalTerritory();
	CInternalTerritory* GetInternalTerritory();

	list<CSmartDrawAnnoView*>* GetAnnoViewList();
	virtual int Prepare();	/// 2012.03.11 added by humkyung
protected:
	CExternalTerritory* m_pExternalTerritory;
	CInternalTerritory* m_pInternalTerritory;
public:
	virtual int Annotate(list<CIsLine2d>& oHiddenLineList);
	virtual int Write(OFSTREAM_T& ofile , const double& dDrawingScale);

	RECT_T m_ExtentRect;
	CaDraw_LogicView* m_pLogicView;
private:
	list<CSmartDrawAnnoView*>* m_pAnnoViewList;

	friend class CExternalTerritory;
	friend class CInternalTerritory;
};
