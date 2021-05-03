// CommandObj.cpp
//
///////////////////////////////////////////////////////////////
#include "../stdafx.h"
#include <assert.h>
#include <vector>
#include <FileVersion.h>
#include "../Tokenizer.h"
#include "../huffman/huffman.h"
#include "CommandObject.h"

using namespace std;
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


///////////////////////////
///   CCommandObject

CCommandObject::CCommandObject()
{
	memset(&m_Packet , '\0' , sizeof(Packet));
}

CCommandObject::~CCommandObject()
{

}

/**
	@brief	decode given data
	@author humkyung	
	@date 2009-09-16 오후 1:44:40	
	@param	
	@return		
*/
int CCommandObject::Decode(BYTE* p , const int& iDataSize)
{
	/// decode given data by using huffman code - 2014.04.03 added by humkyung
	unsigned char *pbufout;
	unsigned int pbufoutlen;
	huffman_decode_memory((const unsigned char*)p , iDataSize , &pbufout , &pbufoutlen);
	assert(pbufoutlen == sizeof(Packet));
	if(pbufoutlen == sizeof(Packet))
	{
		memcpy(&m_Packet , pbufout , sizeof(Packet));
	}
	else
	{
		memset(&m_Packet , '\0' , sizeof(Packet));
		m_Packet.Code = RES_CORRUPTED_PACKET;
	}
	free(pbufout);
	/// up to here

	return ERROR_SUCCESS;
}

/**
	@brief	
	@author humkyung	
	@date 2009-09-16 오후 1:44:43	
	@param	
	@return		
*/
int CCommandObject::Encode(BYTE* p)
{
	/// encode data by using huffman code - 2014.04.03 added by humkyung
	unsigned char *pbufout;
	unsigned int pbufoutlen;
	huffman_encode_memory((const BYTE*)(&m_Packet) , sizeof(Packet) , &pbufout , &pbufoutlen);
	memcpy(p , pbufout , pbufoutlen);
	free(pbufout);
	/// up to here

	/// decode given data by using huffman code - 2014.04.03 added by humkyung
	/*unsigned char *pbufdecode;
	unsigned int pbufdecodelen;
	huffman_decode_memory((const unsigned char*)p , pbufoutlen , &pbufdecode , &pbufdecodelen);
	free(pbufdecode);*/
	/// up to here

	return (pbufoutlen);
}

/******************************************************************************
    @brief		check if new version is available
	@author     humkyung
    @date       2013-11-10
    @class      CCommandObject
    @function   IsNewVersionAvailble
    @return     bool
    @param      char*   p
******************************************************************************/
bool CCommandObject::IsNewVersionAvailable(const CString& sVersion) const
{
	bool res = false;

	if(!sVersion.IsEmpty())
	{
		vector<STRING_T> oResult;
		CTokenizer<CIsFromString>::Tokenize(oResult , sVersion.operator LPCTSTR() , CIsFromString(_T(".")) );
		if(4 == oResult.size())
		{
			BYTE bMajor = ATOI_T(oResult[0].c_str());
			BYTE bMinor = ATOI_T(oResult[1].c_str());
			BYTE bMaintenance  = ATOI_T(oResult[2].c_str());
			BYTE bBuild = ATOI_T(oResult[3].c_str());

			if(bMajor > m_Packet.Major) return true;
			if((bMajor == m_Packet.Major) && (bMinor > m_Packet.Minor)) return true;
			if((bMajor == m_Packet.Major) && (bMinor == m_Packet.Minor) && (bMaintenance > m_Packet.Maintenance)) return true;
			if((bMajor == m_Packet.Major) && (bMinor == m_Packet.Minor) && (bMaintenance == m_Packet.Maintenance) && (bBuild > m_Packet.Build)) return true;
		}
	}

	return res;
}

/**
	@brief	initialize the given packet
	@author	humkyung
	@date	2014.05.14
	@return	int
*/
extern "C" SMARTLMSLIB_EXT_CLASS int __stdcall InitializePacket( Packet* pPacket)
{
	assert(pPacket && "pPacket is NULL");

	if(pPacket)
	{
		memset(pPacket , '\0' , sizeof(Packet));
		DWORD dwVolSerial;
		if(TRUE == GetVolumeInformation(_T("C:\\"),NULL,NULL,&dwVolSerial,NULL,NULL,NULL,NULL))
		{
			sprintf_s((char*)(pPacket->ActivateCode) , ACTIVATE_CODE_BUF_SIZ , ("%X") , dwVolSerial);

			TCHAR szModuleName[MAX_PATH+1];
			(void)GetModuleFileName(AfxGetInstanceHandle(), szModuleName , MAX_PATH);
			CFileVersion oVersion;
			if(TRUE == oVersion.Open(szModuleName))
			{
				strncpy_s((char*)(pPacket->AppName),32,oVersion.GetProductName().operator LPCTSTR(),oVersion.GetProductName().GetLength());

				vector<STRING_T> oResult;
				const CString res = oVersion.GetFileVersion();
				CTokenizer<CIsFromString>::Tokenize(oResult , res.operator LPCTSTR() , CIsFromString(_T(".")) );
				if(4 == oResult.size())
				{
					pPacket->Major = ATOI_T(oResult[0].c_str());
					pPacket->Minor = ATOI_T(oResult[1].c_str());
					pPacket->Maintenance = ATOI_T(oResult[2].c_str());
					pPacket->Build = ATOI_T(oResult[3].c_str());
				}
			}

			return ERROR_SUCCESS;
		}

		return ERROR_BAD_ENVIRONMENT;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	return the string against given code
	@author	humkyung
	@date	2014.05.13
	@return	string
*/
extern "C" SMARTLMSLIB_EXT_CLASS LPCTSTR __stdcall StringHelper( const BYTE& code)
{
	wchar_t localeName[LOCALE_NAME_MAX_LENGTH]={0};
	const LCID lcid = GetUserDefaultLCID();
	//GetUserDefaultLocaleName(localeName,lcid);	this function not exits in XP
	/// refer to(http://msdn.microsoft.com/nb-no/goglobal/bb964664.aspx)

	switch(code)
	{
		case RES_SUCCESS:
			return (1042 == lcid) ?  _T("성공") : _T("Success");
		break;
		case RES_WRONG_PASSWORD:
			return (1042 == lcid) ?  _T("잘못된 암호입니다.") : _T("Wrong password.");
			break;
		case RES_NO_USER:
			return (1042 == lcid) ?  _T("사용자를 찾을 수 없습니다.") : _T("Can't find user.");
			break;
		case RES_ALREADY_LOGGED_IN:
			return (1042 == lcid) ?  _T("이미 로그인되어 있습니다.") : _T("User is already log in.");
			break;
		case RES_FAIL_TO_OPEN_DATABSE:
			return (1042 == lcid) ?  _T("데이터베이스 접근 오류가 발생했습니다.") : _T("Error happened while access the database.");
			break;
		case RES_INVALID_ACTIVATE_CODE:
			return (1042 == lcid) ?  _T("무효한 활성화 코드입니다.") : _T("This is invalid activate code.");
			break;
		case RES_INVALID_REQUEST:
			return (1042 == lcid) ?  _T("알수없는 요청입니다.") : _T("Unkown request.");
			break;
		case RES_CORRUPTED_PACKET:
			return (1042 == lcid) ?  _T("손상된 패킷입니다.") : _T("Corrupted Packet.");
			break;
		case RES_FAIL_TO_RESET_ACTIVATE_CODE:
			return (1042 == lcid) ?  _T("활성화 코드를 초기화시키는데 실패했습니다.") : _T("Fail to reset activate code.");
			break;
		case RES_DISCONNECT:
			return (1042 == lcid) ?  _T("연결이 종료되었습니다.") : _T("Server is disconnected.");
			break;
		case RES_EXPIRED:
			return (1042 == lcid) ?  _T("라이센스가 만료되었습니다.") : _T("License is expired.");
			break;
		case 503:
			return (1042 == lcid) ?  _T("서비스를 사용할 수 없습니다.") : _T("The Service is unavailable.");
			break;
	}

	return _T("");
}