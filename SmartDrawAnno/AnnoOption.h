#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef ADRAW_TAG_EXPORTS
	#define ON_TAG_EXT_DLL		__declspec(dllexport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern
#else
	#define ON_TAG_EXT_DLL		__declspec(dllimport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern __declspec(dllimport)
#endif

#include "SmartDrawAnnoTextStyle.h"

namespace tag_option
{
class ON_TAG_EXT_DLL CAnnoOption  
{
public:
	CAnnoOption();
	virtual ~CAnnoOption();
public:
	void ReadLineNoOverSize(const TCHAR* pszFilePath);
	void Read4CADWorx(const TCHAR* pszFilePath);
	void Parse(const TCHAR* pszFilePath);
	
	double m_nTagSize;		/// tag over tag size
	double m_nLineTextHeight;
	double m_nElevTextHeight;
	CSmartDrawAnnoTextStyle /*m_oLineNoTextStyle , */m_oElevTextStyle;
};
};
