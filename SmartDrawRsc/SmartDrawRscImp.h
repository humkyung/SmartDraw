#pragma once

#ifdef	SMARTDRAWRSC_EXPORTS
	#define	ON_EXT_SMARTDRAWRSC	__declspec(dllexport)
#else
	#define	ON_EXT_SMARTDRAWRSC	__declspec(dllimport)

	#ifdef _UNICODE
		#ifdef _DEBUG
			#if _MSC_VER == 1700
				#pragma comment(lib, "SmartDrawRsc_vc110_unicode_debug")
				#pragma message("Automatically linking with SmartDrawRsc_vc110_unicode_debug.dll (Debug Unicode)")
			#elif _MSC_VER == 1600
				#pragma comment(lib, "SmartDrawRsc_vc100_unicode_debug")
				#pragma message("Automatically linking with SmartDrawRsc_vc100_unicode_debug.dll (Debug Unicode)")
			#else
				#pragma comment(lib, "SmartDrawRsc_vc90_unicode_debug")
				#pragma message("Automatically linking with SmartDrawRsc_vc90_unicode_debug.dll (Debug Unicode)")
			#endif
		#else
			#if _MSC_VER == 1700
				#pragma comment(lib, "SmartDrawRsc_vc110_unicode")
				#pragma message("Automatically linking with SmartDrawRsc_vc110_unicode.dll (Debug Unicode)")
			#elif _MSC_VER == 1600
				#pragma comment(lib, "SmartDrawRsc_vc100_unicode")
				#pragma message("Automatically linking with SmartDrawRsc_vc100_unicode.dll (Debug Unicode)")
			#else
				#pragma comment(lib, "SmartDrawRsc_vc90_unicode")
				#pragma message("Automatically linking with SmartDrawRsc_vc90_unicode.dll (Release Unicode)")
			#endif
		#endif
	#else
		#ifdef _DEBUG
			#if _MSC_VER == 1700
				#pragma comment(lib, "SmartDrawRsc_vc110_debug")
				#pragma message("Automatically linking with SmartDrawRsc_vc110_debug.dll (Debug)")
			#elif _MSC_VER == 1600
				#pragma comment(lib, "SmartDrawRsc_vc100_debug")
				#pragma message("Automatically linking with SmartDrawRsc_vc100_debug.dll (Debug)")
			#else
				#pragma comment(lib, "SmartDrawRsc_vc90_debug")
				#pragma message("Automatically linking with SmartDrawRsc_vc90_debug.dll (Debug)")
			#endif
		#else
			#if _MSC_VER == 1700
				#pragma comment(lib, "SmartDrawRsc_vc110")
				#pragma message("Automatically linking with SmartDrawRsc_vc110.dll (Release)")
			#elif _MSC_VER == 1600
				#pragma comment(lib, "SmartDrawRsc_vc100")
				#pragma message("Automatically linking with SmartDrawRsc_vc100.dll (Release)")
			#else
				#pragma comment(lib, "SmartDrawRsc_vc90")
				#pragma message("Automatically linking with SmartDrawRsc_vc90.dll (Release)")
			#endif
		#endif
	#endif // _UNICODE
#endif
