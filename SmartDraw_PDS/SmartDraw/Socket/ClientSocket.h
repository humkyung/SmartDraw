#if !defined(AFX_CLIENTSOCKET_H__63A1AC1E_52CD_11D4_90EB_CCC937AE7F04__INCLUDED_)
#define AFX_CLIENTSOCKET_H__63A1AC1E_52CD_11D4_90EB_CCC937AE7F04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSocket.h : header file
//

#include "CommandObject.h"

#define	SMARTLMS_MSG	(WM_USER + 100)

/////////////////////////////////////////////////////////////////////////////
// CClientSocket command target

class AFX_EXT_CLASS CClientSocket : public CSocket
{
// Attributes
public:
	CSocketFile*	m_pSocketFile;
	CArchive*	m_pArchiveLoad;
	CArchive*	m_pArchiveStore;

// Operations
public:
	CClientSocket();
	virtual ~CClientSocket();

// Overrides
public:
	bool IsConnected() const;
	int AttachWindow(HWND hWnd);
	BOOL Init(const CString& strServerAddress , const int& iPortNo);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSocket)
	public:
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual int Send(const Packet* pPacket , int nFlags=0);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	HWND m_hWnd;
	bool m_bConnected;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCKET_H__63A1AC1E_52CD_11D4_90EB_CCC937AE7F04__INCLUDED_)
