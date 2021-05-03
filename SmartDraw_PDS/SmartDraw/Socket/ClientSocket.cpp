// ClientSocket.cpp : implementation file
//

#include "../stdafx.h"
#include <assert.h>
#include "ClientSocket.h"
#include "CommandObject.h"
#include <SplitPath.h>
#include "../Tokenizer.h"
#include "AutoUpInf.h"


#include <memory>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientSocket

CClientSocket::CClientSocket() : m_hWnd(NULL) , m_bConnected(false)
{
	m_pSocketFile = NULL;
	m_pArchiveLoad = NULL;
	m_pArchiveStore = NULL;
}

CClientSocket::~CClientSocket()
{
	try
	{
		if(NULL != m_pArchiveLoad)
		{
			delete m_pArchiveLoad;
			m_pArchiveLoad = NULL;
		}
		if(NULL != m_pArchiveStore)
		{
			delete m_pArchiveStore;
			m_pArchiveStore = NULL;
		}
		if(NULL != m_pSocketFile)
		{
			delete m_pSocketFile;
			m_pSocketFile = NULL;
		}
	}
	catch(...)
	{
	}
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSocket, CSocket)
	//{{AFX_MSG_MAP(CClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

typedef struct
{
	CClientSocket* pClientSocket;
	HWND hWnd;
}ThreadParam;

/**
	@brief  Get Module Path
	@author	humkyung
*/
CString GetExecPath(void)
{
	TCHAR szFileName[MAX_PATH + 1] = {'\0', };
	GetModuleFileName(NULL , szFileName , MAX_PATH);
	CString rFilePath = szFileName;
	int at = rFilePath.ReverseFind('\\');
	if(-1 != at) rFilePath.Left(at);
	CString rModulePath = rFilePath.Left(at);
	if(_T("\\") != rModulePath.Right(1)) rModulePath += _T("\\");

	return rModulePath;
}

/**
	@brief	download file
	@author	humkyung
	@date	2013.11.10
*/
BOOL AutoUpdate(const CString& sURL , const CString& sFtpUser , const CString& sFtpPassword , const CString& sInfo)
{
	CString sExecPath = GetExecPath();
	if(_T("\\") != sExecPath.Right(1)) sExecPath += _T("\\");

	int res = 0;
	HMODULE hModule = AfxLoadLibrary(sExecPath + AutoUpDLL);
	if(hModule)
	{
		AutoUpExFunc pfAutoUpEx = (AutoUpExFunc)GetProcAddress(hModule , (LPCSTR)MAKELONG(3,0));
		if(NULL != pfAutoUpEx)
		{
			/// Temp path
			TCHAR szTemp[_MAX_PATH + 1] = {0};
			::GetTempPath(_MAX_PATH, szTemp);
			CSplitPath path(sURL);
			const CString sDestFilePath = CString(szTemp) + path.GetFileName() + path.GetExtension();

			res = pfAutoUpEx(sURL , sFtpUser , sFtpPassword , sDestFilePath , sInfo);
		}

		AfxFreeLibrary(hModule);

		return TRUE;
	}
	
	return FALSE;  /// return TRUE  unless you set the focus to a control
}

/**
	@brief	receive data
	@author	humkyung
	@date	2013.06.27
*/
UINT ReceiveCallback(LPVOID pParam)
{
	ThreadParam* pThreadParam = (ThreadParam*)pParam;
	
	BYTE cbuf[LMS_BUF_SIZE];
	memset(cbuf , 0 , sizeof(BYTE)*LMS_BUF_SIZE);

	int iDataSize = 0;
	if((iDataSize = pThreadParam->pClientSocket->Receive(cbuf , LMS_BUF_SIZE)) > 0)
	{
		auto_ptr<CCommandObject> commandObj(new CCommandObject());
		commandObj->Decode(cbuf , iDataSize);

		/// 2013.11.10 added by humkyung
		if(RES_SUCCESS == commandObj->m_Packet.Code)
		{
			CString sURL(commandObj->m_Packet.URL) , sFtpUser(commandObj->m_Packet.ID) , sFtpPassword(commandObj->m_Packet.Password) , sInfo(commandObj->m_Packet.Message);
			if(!sURL.IsEmpty() && (_T("NULL") != sURL))
			{
				AutoUpdate(sURL, sFtpUser , sFtpPassword , sInfo);
			}
		}
		/// up to here
		if(::IsWindow(pThreadParam->hWnd)) SendMessage(pThreadParam->hWnd , SMARTLMS_MSG , (WPARAM)commandObj.get() , 0);

		delete pThreadParam;
	}

	return 0;
}

/**
	@brief	attach window to socket
	@author humkyung	
	@date 2013.06.27
	@param	
	@return		
**/
int CClientSocket::AttachWindow(HWND hWnd)
{
	m_hWnd = hWnd;

	return ERROR_SUCCESS;
}

/**
	@brief	return true if socket is connected to server
	@author humkyung	
	@date 2013.06.27
	@param	
	@return		
**/
bool CClientSocket::IsConnected() const
{
	return m_bConnected;
}

/**
	@brief	send data through socket
	@author	humkyung
	@date	2014.04.03
*/
int CClientSocket::Send(const Packet* pPacket , int nFlags)
{
	assert(pPacket && "pPacket is NULL");

	if(pPacket)
	{
		/// encode given data by using huffman code - 2014.04.03 added by humkyung
		static BYTE cbuf[LMS_BUF_SIZE];
		memset(cbuf , '\0' , sizeof(BYTE)*LMS_BUF_SIZE);

		auto_ptr<CCommandObject> commandObj(new CCommandObject());
		memcpy(&(commandObj->m_Packet) , pPacket , sizeof(Packet));
		const int iBufSize = commandObj->Encode(cbuf);
		/// up to here

		return CSocket::Send(cbuf , iBufSize , nFlags);
	}

	return ERROR_INVALID_PARAMETER;
}

/////////////////////////////////////////////////////////////////////////////
// CClientSocket member functions
/**
	@brief	
	@author humkyung	
	@date 2009-09-09 ¿ÀÈÄ 11:45:19	
	@param	
	@return		
**/
void CClientSocket::OnReceive(int nErrorCode) 
{
	do
	{
		ThreadParam* pThreadParam = new ThreadParam;
		if(pThreadParam)
		{
			pThreadParam->pClientSocket = this;
			pThreadParam->hWnd = m_hWnd;

			AfxBeginThread(ReceiveCallback , pThreadParam);
		}
	}while(!m_pArchiveLoad->IsBufferEmpty());
}

/**
	@brief	init socket
	@author	humkyung
	@date	2013.06.27
*/
BOOL CClientSocket::Init(const CString& strServerAddress , const int& iPortNo)
{
	Create();
	if (!Connect(strServerAddress, iPortNo))
	{
		ShutDown();
		Close();

		m_bConnected = false;

		return FALSE;
	}
	else 
	{
		m_pSocketFile = new CSocketFile (this);

		m_pArchiveLoad = new CArchive (m_pSocketFile, CArchive::load);
		m_pArchiveStore = new CArchive (m_pSocketFile, CArchive::store);
		
		m_bConnected = true;

		return TRUE;
	}
}

/**
	@brief	socket is closing
	@author	humkyung
	@date	2013.06.27
*/
void CClientSocket::OnClose(int nErrorCode) 
{
	ShutDown();
	Close();
	if (m_pSocketFile != NULL) 
	{
		delete m_pSocketFile;
		m_pSocketFile = NULL;
	}
	if (m_pArchiveLoad != NULL)
	{
		m_pArchiveLoad->Abort();
		delete m_pArchiveLoad;
		m_pArchiveLoad = NULL;
	}
	if (m_pArchiveStore != NULL)
	{
		m_pArchiveStore->Abort();
		delete m_pArchiveStore;
		m_pArchiveStore = NULL;
	}

	m_bConnected = false;

	AfxMessageBox(_T("Application will be closed becase license server is disconnected") , MB_OK);
	AfxGetApp()->PostThreadMessage(WM_QUIT,0,0);
}