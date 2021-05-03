#pragma once

#define	RES_SUCCESS						0
#define	RES_WRONG_PASSWORD				1
#define	RES_NO_USER						2
#define	RES_ALREADY_LOGGED_IN			3
#define	RES_FAIL_TO_OPEN_DATABSE		4
#define	RES_INVALID_ACTIVATE_CODE		5
#define	RES_INVALID_REQUEST				6
#define	RES_CORRUPTED_PACKET			7
#define	RES_FAIL_TO_RESET_ACTIVATE_CODE	8
#define	RES_DISCONNECT					9
#define	RES_NEW_VERSION_AVAILABLE		10
#define	RES_EXPIRED						11

#define	REQ_LOGIN					100
#define	REQ_LOGOUT					101
#define	REQ_RESET_ACTIVATE_CODE		102

#define	ID_BUF_SIZ					32
#define	ACTIVATE_CODE_BUF_SIZ		48
#define	URL_BUF_SIZ					256
#define	MSG_BUF_SIZ					1024

typedef struct tagPacket
{
	BYTE Code;
	BYTE ID[ID_BUF_SIZ];
	BYTE Password[ID_BUF_SIZ];
	BYTE AppName[ID_BUF_SIZ];
	BYTE Major,Minor,Maintenance,Build;
	BYTE ActivateCode[ACTIVATE_CODE_BUF_SIZ];
	BYTE URL[URL_BUF_SIZ];
	BYTE Message[MSG_BUF_SIZ];
}Packet;

#define	LMS_BUF_SIZE	4096

typedef struct
{
	int   size;
	TCHAR buf[LMS_BUF_SIZE];
}LMS_ENCODE;

#pragma once

#ifdef	SMARTLMSLIB_EXT
#define SMARTLMSLIB_EXT_CLASS	__declspec(dllexport)
#else
#define SMARTLMSLIB_EXT_CLASS	__declspec(dllimport)
#endif

#ifndef SMARTLMSLIB_EXT
	#ifdef _UNICODE
		#ifdef _DEBUG
			#if _MSC_VER == 1700
			#elif _MSC_VER == 1600
				#pragma comment(lib, "SmartLMSLib_vc100_unicode_debug")
				#pragma message("Automatically linking with SmartLMSLib_vc100_unicode_debug.dll (Debug Unicode)")
			#elif _MSC_VER == 1500
				#pragma comment(lib, "SmartLMSLib_vc90_unicode_debug")
				#pragma message("Automatically linking with SmartLMSLib_vc90_unicode_debug.dll (Debug Unicode)")
			#endif
		#else
			#if _MSC_VER == 1700
			#elif _MSC_VER == 1600
				#pragma comment(lib, "SmartLMSLib_vc100_unicode")
				#pragma message("Automatically linking with SmartLMSLib_vc100_unicode.dll (Release Unicode)")
			#elif _MSC_VER == 1500
				#pragma comment(lib, "SmartLMSLib_vc90_unicode")
				#pragma message("Automatically linking with SmartLMSLib_vc90_unicode.dll (Release Unicode)")
			#endif
		#endif
	#else
		#ifdef _DEBUG
			#if _MSC_VER == 1700
			#elif _MSC_VER == 1600
				#pragma comment(lib, "SmartLMSLib_vc100_debug")
				#pragma message("Automatically linking with SmartLMSLib_vc100_debug.dll (Debug)")
			#elif _MSC_VER == 1500
				#pragma comment(lib, "SmartLMSLib_vc90_debug")
				#pragma message("Automatically linking with SmartLMSLib_vc90_debug.dll (Debug)")
			#endif
		#else
			#if _MSC_VER == 1700
			#elif _MSC_VER == 1600
				#pragma comment(lib, "SmartLMSLib_vc100")
				#pragma message("Automatically linking with SmartLMSLib_vc100.dll (Release)")
			#elif _MSC_VER == 1500
				#pragma comment(lib, "SmartLMSLib_vc90")
				#pragma message("Automatically linking with SmartLMSLib_vc90.dll (Release)")
			#endif
		#endif
	#endif 

	extern "C" SMARTLMSLIB_EXT_CLASS LPCTSTR __stdcall StringHelper( const BYTE& code);
	extern "C" SMARTLMSLIB_EXT_CLASS int __stdcall InitializePacket( Packet* pPacket);
#endif

class AFX_EXT_CLASS CCommandObject
{
public:
	CCommandObject ();
	virtual ~CCommandObject();
public:
	Packet m_Packet;
public:
	virtual int Encode(BYTE* p);
	virtual int Decode(BYTE* p , const int& iDataSize);
protected:
	bool IsNewVersionAvailable(const CString& sVersion) const;
};
