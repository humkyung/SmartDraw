#pragma once

#include <gui\TreeListCtrl.h>
#include <util/ado/ADODB.h>
#include "../../SmartDrawModule/aDraw_DraFile.h"

#include <list>
using namespace std;

// CDraTreeListCtrl

class CDraTreeListCtrl : public CTreeListCtrl
{
	DECLARE_DYNAMIC(CDraTreeListCtrl)

public:
	CDraTreeListCtrl();
	virtual ~CDraTreeListCtrl();

	typedef struct
	{
		CString rColumnName;
		CString rAlignment;
		int     nColumnWidth;
	}ColumnData;
	map<int       , ColumnData> m_ColumnDataMap;
	map<CString , HTREEITEM> m_OptionTreeItemMap;
protected:
	DECLARE_MESSAGE_MAP()

	list<CaDraw_DraFile*> m_oDraFileList;	/// 2012.02.15 added by humkyung
public:
	CString m_rBeginLabelEditText;

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	int FindTreeItemAndCheck(HTREEITEM hItem, BOOL bCheck);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
protected:
	virtual int OnDeleteItem();
	HTREEITEM CopyTreeItem(HTREEITEM hDest , HTREEITEM hParent/* , const bool& bIncludeParent = false*/);
	bool CanDragItem(TVITEM & item);
	bool CanDropItem(HTREEITEM hDrag, HTREEITEM hDrop);
	void DragMoveItem(HTREEITEM hDrag, HTREEITEM hDrop, bool bCopy);
public:
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateDataCopyEdit(CCmdUI* pCmdUI);
	int FillDraView(CSmartDrawDoc*,const CString& rSettingFolderPath , const CString& rDraFolderPath);
	int UpdateOverlayIcons(CSmartDrawDoc*);	/// 2015.04.25 added by humkyung
private:
	HTREEITEM InsertDraItem(CaDraw_DraFile* pDraFile , const int& iImage , const int& iSelectedImage , const HTREEITEM& hParent , const HTREEITEM& at);	/// 2014.06.16 added by humkyung
	HTREEITEM InsertViewItem(CaDraw_View* pDraView , const HTREEITEM& hParent);
	int FillViewInfo(CaDraw_DraFile* pDraFile , const HTREEITEM& hParent);
	int SetBackgroundItemColor(HTREEITEM hItem);
	int GetCheckedSiblingItems(list<HTREEITEM>& oSelectedItems, HTREEITEM hItem);	/// 2017.07.07 added by humkyung
public:
	afx_msg void OnDraNew();	/// 2012.02.11 added by humkyung
	afx_msg void OnDraCopyedit();
	afx_msg void OnDraDelete();
	afx_msg void OnUpdateDraDelete(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDraEdit(CCmdUI *pCmdUI);
	afx_msg void OnDraEdit();
	afx_msg void OnUpdateDraNew(CCmdUI *pCmdUI);
	afx_msg void OnDraOpen();
	afx_msg void OnUpdateDraOpen(CCmdUI *pCmdUI);
	/// 2015.07.17 added by humkyung
	afx_msg void OnDraEditTitleBlockData();
	afx_msg void OnUpdateDraEditTitleBlockData(CCmdUI *pCmdUI);
	/// up to here
};


