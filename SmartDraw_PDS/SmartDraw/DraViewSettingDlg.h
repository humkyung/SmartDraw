#pragma once

#include "..\..\SmartDrawModule\aDraw_View.h"

#include <vector>
#include <map>
using namespace std;

// CDraViewSettingDlg dialog

class CDraViewSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CDraViewSettingDlg)

public:
	CDraViewSettingDlg(const CString& rViewName , CWnd* pParent = NULL);   // standard constructor
	virtual ~CDraViewSettingDlg();

	int SaveViewInformation(const CString& rDraFilePath);
// Dialog Data
	enum { IDD = IDD_DRA_VIEW };
	CString m_rViewName;

	CImageList m_cImages;
	CMFCButton m_btnPickFirstPoint , m_btnPickSecondPoint;
	CMFCButton m_btnGetModelFromAllReference;
	CMFCButton m_btnPickViewOrigin;
	CMFCButton m_btnAddReference , m_btnRemoveReference;
	CMFCListCtrl m_wndReferenceListCtrl;
	CMFCTabCtrl m_wndTabs;
protected:
	CaDraw_View* m_pView;	/// 2012.02.11 added by humkyung
	map<int,CMFCListCtrl*>	m_ModelFolderListCtrlMap;

	CMFCListCtrl*		m_pDragList;	//Which CWnd we are dragging FROM
	CMFCListCtrl*		m_pDropList;	//Which CWnd we are dropping ON
	CImageList*		m_pDragImage;	//For creating and managing the drag-image
	BOOL			m_bDragging;	//T during a drag operation
	int			m_nDragIndex;	//Index of selected item in the List we are dragging FROM
	int			m_nDropIndex;	//Index at which to drop item in the List we are dropping ON

	int OnBeginDrag(LPNMLISTVIEW pNMLV , CMFCListCtrl& ListCtrl);
	void DropItemOnList(CMFCListCtrl* pDragList , CMFCListCtrl* pDropList);
	void GetSelectedDragItems(CMFCListCtrl& ListCtrl , vector<long>& DragItemIndexEntry);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnLvnBegindragList(UINT id , NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	int SetView(CaDraw_View* pView);	/// 2012.02.11 added by humkyung
	virtual BOOL OnInitDialog();
private:
	int FillReferenceFileList(void);
	int LoadViewInformation(const CString& rDraFilePath);
public:
	afx_msg void OnBnClickedButtonAddReference();
	afx_msg void OnBnClickedButtonRemoveReference();
	afx_msg void OnBnClickedButtonPickViewOrigin();
};
