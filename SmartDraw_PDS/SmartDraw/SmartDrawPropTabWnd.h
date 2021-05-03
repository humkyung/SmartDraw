#pragma once

#include <aDraw_DraFile.h>

#include <vector>
#include <fstream>
using namespace std;
// CSmartDrawPropTabWnd

class CMFCPropEx : public CMFCPropertyGridProperty
{
	DECLARE_DYNCREATE(CMFCPropEx)
public:
	CMFCPropEx(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL);

	void SetModifiedFlag( BOOL bFlag )
	{
		m_bIsModified = bFlag;
	}
};

class CSmartDrawPropTabWnd : public CMFCTabCtrl
{
	DECLARE_DYNAMIC(CSmartDrawPropTabWnd)
public:
	CSmartDrawPropTabWnd( );
	virtual ~CSmartDrawPropTabWnd();

protected:
	afx_msg LRESULT OnPropertyChanged(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
public:
	int InitPropList();
	int Reset();
	int DisplayContentOf(CaDraw_DraFile* pDraFile);
public:
	void SetAlphabeticMode(BOOL bSet = 1);
	BOOL IsAlphabeticMode();
	void SetPropListFont();
	void DeleteProperty();
private:
	int DisplayRevisionData(const CString& sDraIniFilePath , const CString& sBorderCfgFilePath);
private:
	CaDraw_DraFile* m_pDraFile;
	CFont m_fntPropList;
	CMFCPropertyGridCtrl m_wndTitleBlockDataPropList , m_wndRevisionDataPropList;
};
