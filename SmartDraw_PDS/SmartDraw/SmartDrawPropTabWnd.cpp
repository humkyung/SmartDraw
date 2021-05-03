// ELoadPropTabWnd.cpp : implementation file
//

#include "stdafx.h"
#include <assert.h>
#include <sstream>
#include <Tokenizer.h>
#include "SmartDrawDoc.h"
#include "SmartDrawDocData.h"
#include "SmartDrawPropTabWnd.h"

IMPLEMENT_DYNAMIC(CMFCPropEx, CMFCPropertyGridProperty)
CMFCPropEx::CMFCPropEx(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData,
		LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate, LPCTSTR lpszValidChars) : CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData,
		lpszEditMask, lpszEditTemplate, lpszValidChars)
{
}

// CSmartDrawPropTabWnd

IMPLEMENT_DYNAMIC(CSmartDrawPropTabWnd, CMFCTabCtrl)

CSmartDrawPropTabWnd::CSmartDrawPropTabWnd() : m_pDraFile(NULL)
{

}

/**
        @brief  
        @author humkyung
*/
CSmartDrawPropTabWnd::~CSmartDrawPropTabWnd()
{
        try
        {
        }
        catch(...)
        {
        }
}


BEGIN_MESSAGE_MAP(CSmartDrawPropTabWnd, CMFCTabCtrl)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

/**
	@brief	set properties window's font
	@author	humkyung
	@date	2014.07.07
*/
void CSmartDrawPropTabWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	if(m_wndTitleBlockDataPropList.GetSafeHwnd())
	{
		m_wndTitleBlockDataPropList.SetFont(&m_fntPropList);
	}

	if(m_wndRevisionDataPropList.GetSafeHwnd())
	{
		m_wndRevisionDataPropList.SetFont(&m_fntPropList);
	}
}

// CSmartDrawPropTabWnd message handlers
/**
	@brief	Title Block Data,Revision Data에 대한 Tab을 생성한다.
	@author	humkyung
*/
int CSmartDrawPropTabWnd::InitPropList()
{
	UINT ID = 2;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if (TRUE == m_wndTitleBlockDataPropList.Create (WS_VISIBLE | WS_CHILD, rectDummy, this, ID++))
	{
		m_wndTitleBlockDataPropList.EnableHeaderCtrl (FALSE);
		m_wndTitleBlockDataPropList.EnableDescriptionArea ();
		m_wndTitleBlockDataPropList.SetVSDotNetLook ();
		m_wndTitleBlockDataPropList.MarkModifiedProperties ();

		this->AddTab(&m_wndTitleBlockDataPropList , _T("Title Block Data"));
	}
    
	if (TRUE == m_wndRevisionDataPropList.Create (WS_VISIBLE | WS_CHILD, rectDummy, this, ID++))
	{
		m_wndRevisionDataPropList.EnableHeaderCtrl (FALSE);
		m_wndRevisionDataPropList.EnableDescriptionArea ();
		m_wndRevisionDataPropList.SetVSDotNetLook ();
		m_wndRevisionDataPropList.MarkModifiedProperties ();

		this->AddTab(&m_wndRevisionDataPropList , _T("Revision Data"));
	}

	SetPropListFont();

	return ERROR_SUCCESS;
}

/**
	@brief	reset contents of properties
	@author	humkyung
	@date	2014.07.08
*/
int CSmartDrawPropTabWnd::Reset()
{
	{
		const int iPropCount = m_wndTitleBlockDataPropList.GetPropertyCount();
		for(int i = 0;i < iPropCount;++i)
		{
			m_wndTitleBlockDataPropList.GetProperty(i)->SetValue(_T(""));
		}
	}

	{
		const int iPropCount = m_wndRevisionDataPropList.GetPropertyCount();
		for(int i = 0;i < iPropCount;++i)
		{
			m_wndRevisionDataPropList.GetProperty(i)->SetValue(_T(""));
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	display title block data and revision data of dra file
	@author	humkyung
	@date	2014.07.08
*/
int CSmartDrawPropTabWnd::DisplayContentOf(CaDraw_DraFile* pDraFile)
{
	assert(pDraFile && "pDraFile is NULL");
	m_pDraFile = pDraFile;

	if(pDraFile)
	{
		static const CString sApp(_T("GENERAL"));
		TCHAR szBuf[MAX_PATH + 1]={'\0',};

		CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
		const CString sServerFolderPath = docData.GetServerFolderPath();
	
		CSmartDrawDoc* pDoc = GetMDIActiveDocument();
		const CString sPrjName = pDoc->GetProjectName();
		const CString sIniFileName = pDraFile->GetSettingFileName();
		const CString sIniFilePath = sServerFolderPath + _T("\\") + sPrjName + _T("\\Setting\\") + sIniFileName + _T(".ini");
		const CString sDraIniFilePath = CString(STRING_T(pDraFile->GetFilePath() + _T(".ini")).c_str());

		m_wndTitleBlockDataPropList.EnableWindow(TRUE);
		m_wndRevisionDataPropList.EnableWindow(TRUE);

		if(GetPrivateProfileString(sApp , _T("Border File") , _T("") , szBuf , MAX_PATH , sIniFilePath))
		{
			CString sKey;
			vector<STRING_T> oResult;

			int iPropCount = m_wndTitleBlockDataPropList.GetPropertyCount();
			const CString sBorderCfgFilePath = sServerFolderPath + _T("\\") + sPrjName + _T("\\Border\\") + szBuf + _T(".cfg");
			const UINT uiCount = GetPrivateProfileInt(_T("TITLE BLOCK") , _T("Count") , 0 , sBorderCfgFilePath);
			for(int i = 0;i < int(uiCount);++i)
			{
				sKey.Format(_T("Data%d") , i);	
				if(GetPrivateProfileString(_T("TITLE BLOCK") , sKey , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath))
				{
					CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
					if(!oResult.empty())
					{
						CMFCPropertyGridProperty* pSubItem = NULL;
						if(i < iPropCount)
						{
							pSubItem = m_wndTitleBlockDataPropList.GetProperty(i);
							pSubItem->SetName( oResult[0].c_str() );
							pSubItem->SetValue( _T("") );
						}
						else
						{
							pSubItem = new CMFCPropEx(oResult[0].c_str() , (_variant_t) _T("") , _T("Title block data"));
							m_wndTitleBlockDataPropList.AddProperty( pSubItem );
						}

						if(GetPrivateProfileString(_T("Title Block Data") , oResult[0].c_str() , _T("") , szBuf , MAX_PATH , sDraIniFilePath))
						{
							pSubItem->SetValue( szBuf );
						}
					}
				}
			}
			m_wndRevisionDataPropList.ExpandAll();

			iPropCount = m_wndTitleBlockDataPropList.GetPropertyCount();
			if(iPropCount > int(uiCount))
			{
				for(int i = iPropCount;i > int(uiCount);--i)
				{
					CMFCPropertyGridProperty* pSubItem = m_wndTitleBlockDataPropList.GetProperty(i - 1);
					m_wndTitleBlockDataPropList.DeleteProperty(pSubItem);
				}
			}

			DisplayRevisionData(sDraIniFilePath , sBorderCfgFilePath);
		}
		else
		{
			m_wndTitleBlockDataPropList.ExpandAll(FALSE);
			m_wndRevisionDataPropList.ExpandAll(FALSE);
		}
		
		return ERROR_SUCCESS;
	}
	else
	{
		m_wndTitleBlockDataPropList.ExpandAll(FALSE);
		m_wndTitleBlockDataPropList.EnableWindow(FALSE);
		m_wndRevisionDataPropList.ExpandAll(FALSE);
		m_wndRevisionDataPropList.EnableWindow(FALSE);
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	display revision data
	@autor	humkyung
	@date	2014.11.17
*/
int CSmartDrawPropTabWnd::DisplayRevisionData(const CString& sDraIniFilePath , const CString& sBorderCfgFilePath)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	vector<STRING_T> oColumnList , oLabelList;
	if(GetPrivateProfileString(_T("Revision Chart") , _T("Column") , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oColumnList , szBuf , CIsComma());
	}
	if(GetPrivateProfileString(_T("Revision Chart") , _T("Label") , _T("") , szBuf , MAX_PATH , sBorderCfgFilePath))
	{
		CTokenizer<CIsComma>::Tokenize(oLabelList , szBuf , CIsComma());
	}

	vector<STRING_T> oResult;
	int iPropCount = m_wndRevisionDataPropList.GetPropertyCount();
	const UINT uiCount = GetPrivateProfileInt(_T("Revision Data") , _T("Count") , 0 , sDraIniFilePath);
	CString sKey;
	for(int i = 0;i < int(uiCount);++i)
	{
		sKey.Format(_T("Data%d") , i);
		if(GetPrivateProfileString(_T("Revision Data") , sKey , _T("") , szBuf , MAX_PATH , sDraIniFilePath))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			if(!oResult.empty())
			{
				/// show rev. no
				CMFCPropertyGridProperty* pRevNoItem = NULL;
				if(i < iPropCount)
				{
					pRevNoItem = m_wndRevisionDataPropList.GetProperty(i);
					pRevNoItem->SetName( oResult[0].c_str() );
				}
				else
				{
					pRevNoItem = new CMFCPropertyGridProperty(oResult[0].c_str());
					m_wndRevisionDataPropList.AddProperty( pRevNoItem );
				}
				pRevNoItem->SetData((DWORD_PTR)i);	/// 2012.10.10 added by humkyung
				pRevNoItem->AllowEdit(FALSE);
				/// up to here

				CMFCPropertyGridProperty* pSubItem = NULL;
				if(i < iPropCount)
				{
					for(int j = 1;j < int(oResult.size());++j)
					{
						const STRING_T sColumnName = CRevisionChart::COLUMNS[j];
						STRING_T sLabelName;
						for(int k = 0;k < int(oColumnList.size());++k)
						{
							if(sColumnName == oColumnList[k])
							{
								sLabelName = oLabelList[k];
								break;
							}
						}

						pSubItem = pRevNoItem->GetSubItem(j - 1);
						pSubItem->SetName(sLabelName.c_str());
						pSubItem->SetValue( oResult[j].c_str() );
					}
				}
				else
				{
					for(int j = 1;j < int(oResult.size());++j)
					{
						const STRING_T sColumnName = CRevisionChart::COLUMNS[j];
						STRING_T sLabelName;
						for(int k = 0;k < int(oColumnList.size());++k)
						{
							if(sColumnName == oColumnList[k])
							{
								sLabelName = oLabelList[k];
								break;
							}
						}

						pSubItem = new CMFCPropEx(oResult[j].c_str() , (_variant_t) _T("") , _T("Revision data"));
						pSubItem->SetName(sLabelName.c_str());
						pSubItem->SetValue( oResult[j].c_str() );
						pRevNoItem->AddSubItem( pSubItem );
					}
				}
			}
		}
	}

	iPropCount = m_wndRevisionDataPropList.GetPropertyCount();
	if(iPropCount > int(uiCount))
	{
		for(int i = iPropCount;i > int(uiCount);--i)
		{
			CMFCPropertyGridProperty* pRevNoItem = m_wndRevisionDataPropList.GetProperty(i - 1);
			if(NULL != pRevNoItem)
			{
				for(int j = pRevNoItem->GetSubItemsCount() - 1;j >= 0;--j)
				{
					CMFCPropertyGridProperty* pSubItem = pRevNoItem->GetSubItem(j);
					pRevNoItem->RemoveSubItem( pSubItem );
				}
			}
			m_wndRevisionDataPropList.DeleteProperty(pRevNoItem);
		}
	}
	
	m_wndRevisionDataPropList.ExpandAll();

	return ERROR_SUCCESS;
}

/**
	@brief	called when a property is changed.
	@autor	humkyung
*/
LRESULT CSmartDrawPropTabWnd::OnPropertyChanged(WPARAM wp,  LPARAM lp)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)(lp);
	if(NULL != m_pDraFile)
	{
		const STRING_T sDraIniFilePath = m_pDraFile->GetFilePath() + _T(".ini");
		if(NULL == pProp->GetParent())
		{
			CString sValue(pProp->GetValue());
			WritePrivateProfileString(_T("Title Block Data") , pProp->GetName() , sValue , sDraIniFilePath.c_str());
		}
		else
		{
			CMFCPropertyGridProperty* pParent = pProp->GetParent();
			const int iRevNo = int(pParent->GetData());
			CString sKey , sValue;
			sValue = CString(pParent->GetName()) + _T(",");
			for(int i = 0;i < pParent->GetSubItemsCount();++i)
			{
				CMFCPropertyGridProperty* pSubItem = pParent->GetSubItem(i);
				sValue += CString(pSubItem->GetValue()) + _T(",");
			}
			sValue = sValue.Left(sValue.GetLength() - 1);

			sKey.Format(_T("Data%d") , iRevNo);
			WritePrivateProfileString(_T("Revision Data") , sKey , sValue , sDraIniFilePath.c_str());

			///#22(http://atools.co.kr:9002/redmine/issues/22) - 2016.03.28 added by humkyung
			const UINT uiCount = GetPrivateProfileInt(_T("Revision Data") , _T("Count") , 0 , sDraIniFilePath.c_str()); 
			if(uiCount > 0)
			{
				TCHAR szBuf[MAX_PATH + 1]={'\0',};
				vector<STRING_T> oResult;

				CString rString;
				rString.Format(_T("Data%d") , uiCount-1);
				GetPrivateProfileString(_T("Revision Data") , rString , _T("") , szBuf , MAX_PATH , sDraIniFilePath.c_str());
				CTokenizer<CIsComma>::Tokenize(oResult,szBuf,CIsComma());
				if(!oResult.empty())
				{
					WritePrivateProfileString(_T("Title Block Data") , _T("REV") , oResult[0].c_str() , sDraIniFilePath.c_str());
				}
			}
			///up to here
		}

		pProp->SetOriginalValue( pProp->GetValue() );
		((CMFCPropEx*)pProp)->SetModifiedFlag(FALSE);
	}

	return 0;
}

/**
        @brief  
        @author humkyung
*/
void CSmartDrawPropTabWnd::SetAlphabeticMode(BOOL bSet)
{
	const int nTabCount = GetTabsNum();
	for(int i = 0;i < nTabCount;++i)
	{
		CMFCPropertyGridCtrl* pPropList = static_cast<CMFCPropertyGridCtrl*>(GetTabWnd(i));
		if(pPropList) pPropList->SetAlphabeticMode(bSet);
	}
}

/**
        @brief  
        @author humkyung
*/
BOOL CSmartDrawPropTabWnd::IsAlphabeticMode()
{
	const int nTabCount = GetTabsNum();
	for(int i = 0;i < nTabCount;++i)
	{
		CMFCPropertyGridCtrl* pPropList = static_cast<CMFCPropertyGridCtrl*>(GetTabWnd(i));
		if(pPropList) return pPropList->IsAlphabeticMode();
	}

	return TRUE;
}

/**
        @brief	delete selected revision data in m_wndRevisionDataPropList  
        @author humkyung
		@date	2014.11.24
*/
void CSmartDrawPropTabWnd::DeleteProperty()
{
	/// delete selected rev. no - 2014.11.25 added by humkyung
	CMFCPropertyGridProperty* pProp = m_wndRevisionDataPropList.GetCurSel();
	if(NULL == pProp) return;
	pProp = (NULL != pProp->GetParent()) ? pProp->GetParent() : pProp;
	for(int i = pProp->GetSubItemsCount() - 1;i >= 0;--i)
	{
		CMFCPropertyGridProperty* pSubItem = pProp->GetSubItem(i);
		pProp->RemoveSubItem( pSubItem );
	}
	m_wndRevisionDataPropList.DeleteProperty(pProp);
	/// up to here

	const CString sDraIniFilePath((m_pDraFile->GetFilePath() + _T(".ini")).c_str());
	const CString sApp(_T("Revision Data"));
	CString sKey,sValue;
	int iPropCount = m_wndRevisionDataPropList.GetPropertyCount();
	sValue.Format(_T("%d") , iPropCount);
	WritePrivateProfileString(sApp , _T("Count") , sValue , sDraIniFilePath);
	for(int i = 0;i < iPropCount;++i)
	{
		CMFCPropertyGridProperty* pRevNo = m_wndRevisionDataPropList.GetProperty(i);
		sValue = CString(pRevNo->GetName()) + _T(",");
		for(int j = 0;j < pRevNo->GetSubItemsCount();++j)
		{
			CMFCPropertyGridProperty* pSubItem = pRevNo->GetSubItem(j);
			sValue += CString(pSubItem->GetValue()) + _T(",");
		}
		sValue = sValue.Left(sValue.GetLength() - 1);

		sKey.Format(_T("Data%d") , i);
		WritePrivateProfileString(sApp , sKey , sValue , sDraIniFilePath);
	}
}