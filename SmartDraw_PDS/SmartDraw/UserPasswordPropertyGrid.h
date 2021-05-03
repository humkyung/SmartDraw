#pragma once

// CUserPasswordPropertyGrid command target

class CUserPasswordPropertyGrid : public CMFCPropertyGridProperty
{
public:
	CUserPasswordPropertyGrid(const CString& strName, const _variant_t& varValue,
                LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
                LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
                LPCTSTR lpszValidChars = NULL);
	virtual ~CUserPasswordPropertyGrid();

	void OnDrawValue (CDC* pDC, CRect rect);
	CWnd* CreateInPlaceEdit (CRect rectEdit, BOOL& bDefaultFormat);
};


