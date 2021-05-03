#pragma once

typedef	CString (* GetMACaddressFunc)();
typedef	int (__stdcall *AutoUpFunc)(const TCHAR* , const BOOL, const BOOL);
typedef int (__stdcall *CheckForUpdateFunc)(const TCHAR* , const BOOL);
typedef	int (__stdcall *AutoUpExFunc)(const TCHAR* , const TCHAR* , const TCHAR* , const TCHAR* , const TCHAR*);
typedef void (__stdcall *ShowRegisterFormFunc)();

//extern "C" __declspec(dllimport) CString GetMACaddress();
extern "C" __declspec(dllimport) int __stdcall CheckForUpdate(const TCHAR* rIniFilePath , const BOOL , const BOOL);
extern "C" __declspec(dllimport) int __stdcall AutoUp(const TCHAR* pszIniFilePath , const BOOL);
extern "C" __declspec(dllimport) void __stdcall ShowRegisterForm();

const int RETURN_ERROR				= -1;
const int RETURN_OK					= 0;
const int RETURN_NEWVERSION			= 1;
const int RETURN_NONEWVERSION		= 2;
const int RETURN_SECURITYERROR		= 3;
const int RETURN_BAD_ENVIRONMENT	= 4;
const int RETURN_LICENSE_EXPIRED	= 5;
const int RETURN_NEEDTOUPDATE		= 6;
const int RETURN_IGNOREUPDATE		= 7;

#ifdef _DLL
	#ifdef _UNICODE
		#ifdef _DEBUG
			#if _MSC_VER == 1600
				#define AutoUpDLL	_T("AutoUp_vc100_UD.dll")
			#else
				#define AutoUpDLL	_T("AutoUp_vc90_UD.dll")
			#endif
		#else
			#if _MSC_VER == 1600
				#define AutoUpDLL	_T("Auto_vc100_UpU.dll")
			#else
				#define AutoUpDLL	_T("Auto_vc90_UpU.dll")
			#endif
		#endif
	#else
		#ifdef _DEBUG
			#if _MSC_VER == 1600
				#define AutoUpDLL	_T("AutoUp_vc100_debug.dll")
			#else
				#define AutoUpDLL	_T("AutoUp_vc90_debug.dll")
			#endif
		#else
			#if _MSC_VER == 1600
				#define AutoUpDLL	_T("AutoUp_vc100.dll")
			#else
				#define AutoUpDLL	_T("AutoUp_vc90.dll")
			#endif
		#endif
	#endif
#else
	#ifdef _UNICODE
		#ifdef _DEBUG
			#define AutoUpDLL	_T("AutoUpSUD.dll")
		#else
			#define AutoUpDLL	_T("AutoUpSU.dll")
		#endif
	#else
		#ifdef _DEBUG
			#define AutoUpDLL	_T("AutoUpSD.dll")
		#else
			#define AutoUpDLL	_T("AutoUpS.dll")
		#endif
	#endif
#endif

///#pragma comment(lib,IsUtil2008DLL DLL_CRTSTATIC DLL_UNICODE DLL_DEBUG)
///#pragma comment(lib,AutoUpDLL DLL_CRTSTATIC DLL_UNICODE DLL_DEBUG)
