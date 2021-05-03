#pragma once

#include "SmartDrawRscImp.h"
// CCellLibraryFileBrowseEdit

class ON_EXT_SMARTDRAWRSC CCellLibraryFileBrowseEdit : public CMFCEditBrowseCtrl
{
	DECLARE_DYNAMIC(CCellLibraryFileBrowseEdit)

public:
	CCellLibraryFileBrowseEdit();
	virtual ~CCellLibraryFileBrowseEdit();

	virtual void OnBrowse();

	CString m_sIniFilePath , m_rFilePath;
protected:
	DECLARE_MESSAGE_MAP()
};


