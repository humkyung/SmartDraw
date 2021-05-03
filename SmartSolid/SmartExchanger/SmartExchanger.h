#pragma once

#ifndef SMARTEXCHANGER_EXPORTS
	#ifdef _UNICODE
		#if _MSC_VER == 1600
			#ifdef _DEBUG
				#pragma comment(lib, "smartexchanger_vc100_unicode_debug")
				#pragma message("Automatically linking with smartexchanger_vc100_unicode_debug.dll (Debug Unicode)")
			#else
				#pragma comment(lib, "smartexchanger_vc100_unicode")
				#pragma message("Automatically linking with smartexchanger_vc100_unicode.dll (Release Unicode)")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "smartexchanger_vc90_unicode_debug")
				#pragma message("Automatically linking with smartexchanger_vc90_unicode_debug.dll (Debug Unicode)")
			#else
				#pragma comment(lib, "smartexchanger_vc90_unicode")
				#pragma message("Automatically linking with smartexchanger_vc90_unicode.dll (Release Unicode)")
			#endif
		#endif
	#else
		#if _MSC_VER == 1600
			#ifdef _DEBUG
				#pragma comment(lib, "smartexchanger_vc100_debug")
				#pragma message("Automatically linking with smartexchanger_vc100_debug.dll (Debug)")
			#else
				#pragma comment(lib, "smartexchanger_vc100")
				#pragma message("Automatically linking with smartexchanger_vc100.dll (Release)")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "smartexchanger_vc90_debug")
				#pragma message("Automatically linking with smartexchanger_vc90_debug.dll (Debug)")
			#else
				#pragma comment(lib, "smartexchanger_vc90")
				#pragma message("Automatically linking with smartexchanger_vc90.dll (Release)")
			#endif
		#endif
	#endif // _UNICODE

	extern "C" __declspec(dllimport) void __stdcall Dgn2Sxf(const TCHAR* pExportFilePath,const int& iModelType , const TCHAR* pImportFilePath , const TCHAR* pIniFilePath , const double& , const double& , const double&);
#endif
