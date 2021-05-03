#pragma once
#include <gui/GridCtrl/GridCtrl.h>

class CFileBrowseGridCell;
class CBrowseButton : public CMFCButton
{
	DECLARE_DYNCREATE(CBrowseButton)
public:
	CBrowseButton(){}
	~CBrowseButton(){}
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP();

	CFileBrowseGridCell* m_pGridCell;
};

class CFileBrowseGridCell : public CGridCell  
{
	DECLARE_DYNCREATE(CFileBrowseGridCell)
public:
	CFileBrowseGridCell();
	virtual ~CFileBrowseGridCell();
public:
	virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
private:
	CBrowseButton m_wndBrowseButton;
};