// DraViewSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include <IsString.h>
#include "SmartDraw.h"
#include "DraViewSettingDlg.h"
#include "DraFileSettingDlg.h"
#include "SmartDrawDocData.h"
#include <Tokenizer.h>

#include <list>
#include <vector>
using namespace std;
// CDraViewSettingDlg dialog

IMPLEMENT_DYNAMIC(CDraViewSettingDlg, CDialog)

CDraViewSettingDlg::CDraViewSettingDlg(const CString& rViewName , CWnd* pParent /*=NULL*/)
	: CDialog(CDraViewSettingDlg::IDD, pParent) , m_rViewName(rViewName)
{
	m_pDragList = NULL;
	m_pDropList = NULL;
	m_bDragging = FALSE;
	m_nDragIndex = -1;
	m_nDropIndex = -1;
	m_pDragImage = NULL;

	m_pView = NULL;
}

CDraViewSettingDlg::~CDraViewSettingDlg()
{
}

void CDraViewSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX , IDC_BUTTON_PICK_FIRST_POINT , m_btnPickFirstPoint);
	DDX_Control(pDX , IDC_BUTTON_PICK_SECOND_POINT, m_btnPickSecondPoint);
	DDX_Control(pDX , IDC_BUTTON_GET_VOLUME_FROM_ALL_REFERENCE , m_btnGetModelFromAllReference);
	DDX_Control(pDX , IDC_BUTTON_PICK_VIEW_ORIGIN , m_btnPickViewOrigin);
	DDX_Control(pDX , IDC_LIST_REFERENCE , m_wndReferenceListCtrl);
	DDX_Control(pDX , IDC_BUTTON_ADD_REFERENCE , m_btnAddReference);
	DDX_Control(pDX , IDC_BUTTON_REMOVE_REFERENCE , m_btnRemoveReference);
}


BEGIN_MESSAGE_MAP(CDraViewSettingDlg, CDialog)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_RANGE(LVN_BEGINDRAG , IDC_LIST_REFERENCE , WM_USER + 6 , &CDraViewSettingDlg::OnLvnBegindragList)
	ON_BN_CLICKED(IDC_BUTTON_ADD_REFERENCE, &CDraViewSettingDlg::OnBnClickedButtonAddReference)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_REFERENCE, &CDraViewSettingDlg::OnBnClickedButtonRemoveReference)
	ON_BN_CLICKED(IDC_BUTTON_PICK_VIEW_ORIGIN, &CDraViewSettingDlg::OnBnClickedButtonPickViewOrigin)
END_MESSAGE_MAP()


// CDraViewSettingDlg message handlers

static bool __SortByType(const STRING_T& lhs , const STRING_T& rhs)
{
	static const STRING_T sTypes[]={_T("Pipe") , _T("Equipment") , _T("Structure") , _T("CableT") , _T("Misc")};

	const long at1 = find(sTypes , sTypes + SIZE_OF_ARRAY(sTypes) , lhs) - sTypes;
	const long at2 = find(sTypes , sTypes + SIZE_OF_ARRAY(sTypes) , rhs) - sTypes;

	return (at1 < at2);
}

BOOL CDraViewSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDI_DRA);
	HICON hOldIcon = SetIcon(hIcon , FALSE);
	if(hOldIcon) DestroyIcon(hOldIcon);

	{
		hIcon = (HICON) LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_MOUSE_PICK), IMAGE_ICON,0,0,0);
		
		hOldIcon = m_btnPickFirstPoint.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
		hOldIcon = m_btnPickSecondPoint.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);

		hOldIcon = m_btnPickViewOrigin.SetIcon(hIcon);
		if(hOldIcon) DestroyIcon(hOldIcon);
	}

	if(m_cImages.Create (16, 16, ILC_COLOR32|ILC_MASK, 15, 1))
	{
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_MSTNJ));
	}
	
	CRect rect;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_FILE_TAB);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_3D , rect , this, 1))
	{
		TRACE0("Failed to create Properies Grid \n");
		return FALSE;	// fail to create
	}
	m_wndTabs.SetLocation(CMFCTabCtrl::LOCATION_TOP);
	m_wndTabs.SetWindowPos(&(CWnd::wndTop) , 0 , 0 , 0 , 0 , SWP_NOSIZE | SWP_NOMOVE);
	m_wndTabs.ShowWindow(SW_SHOW);

	m_wndReferenceListCtrl.InsertColumn(0 , _T("File Name") , LVCFMT_LEFT , 130);
	m_wndReferenceListCtrl.InsertColumn(1 , _T("Type") , LVCFMT_LEFT , 80);
	m_wndReferenceListCtrl.SetImageList(&m_cImages,LVSIL_SMALL);

	CDraFileSettingDlg* pParent = (CDraFileSettingDlg*)(GetParent());

	rect.SetRectEmpty();

	/// get model folder list
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	const CString rProjectName = pParent->m_pDoc->GetProjectName();
	const CString rFindName = docData.GetServerFolderPath() + _T("\\") + rProjectName + _T("\\Model\\*.*");
	int nID = 0;

	vector<STRING_T> oFolderList;

	CFileFind finder;
	BOOL bWorking = finder.FindFile(rFindName);
	while(bWorking)
	{
		bWorking = finder.FindNextFile();
		if(finder.IsDirectory() && (_T("..") != finder.GetFileName()) && (_T(".") != finder.GetFileName()))
		{
			oFolderList.push_back( finder.GetFileName().operator LPCTSTR() );
		}
	}
	stable_sort(oFolderList.begin() , oFolderList.end() , __SortByType);

	for(vector<STRING_T>::iterator itr = oFolderList.begin();itr != oFolderList.end();++itr)
	{
		CMFCListCtrl* pList = new CMFCListCtrl;
		if(TRUE == pList->Create(WS_CHILD|WS_VISIBLE|LVS_REPORT|WS_BORDER , rect , &m_wndTabs , WM_USER + (nID++) + 2))
		{
			pList->SetImageList(&m_cImages,LVSIL_SMALL);
			pList->InsertColumn(0 , _T("File Name") , LVCFMT_LEFT , 200);
			m_wndTabs.AddTab(pList , itr->c_str());
		}
	}
	/// up to here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************
    @brief		fill reference list,view info and model file list
	@author     humkyung
    @date       2012-02-11
    @function   SetView
    @return     int
    @param      CaDraw_View*    pView
******************************************************************************/
int CDraViewSettingDlg::SetView(CaDraw_View* pView)
{
	m_pView = pView;

	FillReferenceFileList();

	CDraFileSettingDlg* pParent = (CDraFileSettingDlg*)(GetParent());
	LoadViewInformation(pParent->m_rDraFilePath);

	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	const CString rProjectName = pParent->m_pDoc->GetProjectName();
	const CString sPrjSettingFilePath =  docData.GetServerFolderPath() + _T("\\") + rProjectName + _T("\\Setting\\") + rProjectName + _T(".prj");

	TCHAR szBuf[MAX_PATH + 1]={'\0'};
	const int nTabNum = m_wndTabs.GetTabsNum();
	for(int i = 0;i < nTabNum;++i)
	{
		CString strLabel;
		m_wndTabs.GetTabLabel(i , strLabel);
		CMFCListCtrl* pListCtrl = (CMFCListCtrl*)m_wndTabs.GetTabWnd(i);
		pListCtrl->DeleteAllItems();

		if(GetPrivateProfileString(_T("Folder") , strLabel , _T("") , szBuf , MAX_PATH , sPrjSettingFilePath))
		{
			CString rFindName(szBuf);
			if(rFindName.IsEmpty()) continue;
			if(_T("\\") != rFindName.Right(1)) rFindName += _T("\\");
			rFindName += _T("\\*.*");

			CFileFind finder;
			BOOL bWorking = finder.FindFile(rFindName);
			int nIndex = 0;
			while(bWorking)
			{
				bWorking = finder.FindNextFile();
				if(finder.IsDirectory()) continue;

				bool bAlreadyReferenced = false;
				CString rFileName(finder.GetFileName());
				const int at = rFileName.ReverseFind('.');
				CString rFileExt;
				if(-1 != at) rFileExt = rFileName.Right(rFileName.GetLength() - at - 1).MakeUpper();
				if((_T("DGN") !=  rFileExt) && (_T("PRP") != rFileExt)) continue;
				const int nItemCount = m_wndReferenceListCtrl.GetItemCount();
				for(int i = 0;i < nItemCount;++i)
				{
					if((rFileName == m_wndReferenceListCtrl.GetItemText(i , 0)) && (strLabel == m_wndReferenceListCtrl.GetItemText(i , 1)))
					{
						bAlreadyReferenced = true;
						break;
					}
				}

				if(false == bAlreadyReferenced) pListCtrl->InsertItem(nIndex++ , finder.GetFileName(),0);
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
	@brief	

	@author	HumKyung.BAEK

	@date	?
*/
int CDraViewSettingDlg::FillReferenceFileList(void)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};
	CSmartDrawDocData& docData = CSmartDrawDocData::GetInstance();
	CDraFileSettingDlg* pParent = (CDraFileSettingDlg*)(GetParent()->GetParent());

	list<CaDraw_View::MODEL_T>* pModelList = m_pView->GetModelList();

	m_wndReferenceListCtrl.DeleteAllItems();

	vector<string> oResult;
	int nRefCount = 0;
	for(list<CaDraw_View::MODEL_T>::iterator itr = pModelList->begin();itr != pModelList->end();++itr)
	{
		const STRING_T sModelType = CaDraw_Entity::GetModelTypeString(itr->modelType);
		const int nItem = m_wndReferenceListCtrl.InsertItem(m_wndReferenceListCtrl.GetItemCount() , itr->rModelName.c_str());
		m_wndReferenceListCtrl.SetItemText(nItem , 1 , sModelType.c_str());
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	load view information and display it.

	@author	HumKyung.BAEK

	@date	2010.05.12

	@param	

	@return
*/
int CDraViewSettingDlg::LoadViewInformation(const CString& rDraFilePath)
{
	TCHAR szBuf[MAX_PATH + 1]={'\0',};

	if(NULL != m_pView)
	{
		vector<STRING_T> oResult;
		const CString rAppSettingFilePath = GetExecPath() + CString(_T("\\Setting\\")) + PRODUCT_NAME + CString(_T(".ini"));
		
		/// read view direction
		if(GetPrivateProfileString(_T("Standard") , _T("Direction List") , NULL , szBuf , MAX_PATH , rAppSettingFilePath))
		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_DIRECTION);
			pComboBox->ResetContent();

			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			for(vector<STRING_T>::iterator itr = oResult.begin();itr != oResult.end();++itr)
			{
				pComboBox->AddString(itr->c_str());
			}
		}

		const STRING_T sViewDirString = m_pView->GetViewDirString();
		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_DIRECTION);

			int at = pComboBox->FindStringExact(-1 , sViewDirString.c_str());
			if(-1 == at)
			{
				at = pComboBox->AddString(sViewDirString.c_str());
			}
			pComboBox->SetCurSel(at);
		}
		/// up to here

		const CIsVolume vol = m_pView->GetVolume();
		STRING_T sValue = IsString::TrimedTrailingZero( vol.minx() );
		CWnd* pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MINX);
		pWnd->SetWindowText(sValue.c_str());

		sValue = IsString::TrimedTrailingZero( vol.miny() );
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MINY);
		pWnd->SetWindowText(sValue.c_str());

		sValue = IsString::TrimedTrailingZero( vol.minz() );
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MINZ);
		pWnd->SetWindowText(sValue.c_str());

		sValue = IsString::TrimedTrailingZero( vol.maxx() );
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MAXX);
		pWnd->SetWindowText(sValue.c_str());

		sValue = IsString::TrimedTrailingZero( vol.maxy() );
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MAXY);
		pWnd->SetWindowText(sValue.c_str());

		sValue = IsString::TrimedTrailingZero( vol.maxz() );
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MAXZ);
		pWnd->SetWindowText(sValue.c_str());

		/// read scale
		if(GetPrivateProfileString(_T("Standard") , _T("Scale List") , NULL , szBuf , MAX_PATH , rAppSettingFilePath))
		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SCALE);
			pComboBox->ResetContent();

			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			for(vector<STRING_T>::iterator itr = oResult.begin();itr != oResult.end();++itr)
			{
				pComboBox->AddString(itr->c_str());
			}
		}
		const double dScale = m_pView->GetScale();
		sValue = IsString::TrimedTrailingZero( dScale );
		CString sTemp(sValue.c_str());

		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SCALE);
		int at = pComboBox->FindStringExact(0 , sTemp);
		if(-1 == at)
		{
			at = pComboBox->AddString(sTemp);
		}
		pComboBox->SetCurSel(at);
		/// up to here

		/// read view angle
		if(GetPrivateProfileString(_T("Standard") , _T("Angle List") , NULL , szBuf , MAX_PATH , rAppSettingFilePath))
		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_ANGLE);
			pComboBox->ResetContent();

			CTokenizer<CIsComma>::Tokenize(oResult , szBuf , CIsComma());
			for(vector<STRING_T>::iterator itr = oResult.begin();itr != oResult.end();++itr)
			{
				pComboBox->AddString(itr->c_str());
			}
		}
		const STRING_T sAngle = m_pView->GetViewAngleString();
		pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_ANGLE);
		at = pComboBox->FindStringExact(-1 , sAngle.c_str());
		if(-1 == at)
		{
			at = pComboBox->AddString(sAngle.c_str());
		}
		pComboBox->SetCurSel(at);
		/// up to here

		/// read view origin
		const CIsPoint3d origin = m_pView->origin();
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_VIEW_ORIGIN_X);
		sValue = IsString::TrimedTrailingZero( origin.x() );
		pWnd->SetWindowText(sValue.c_str());

		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_VIEW_ORIGIN_Y);
		sValue = IsString::TrimedTrailingZero( origin.y() );
		pWnd->SetWindowText(sValue.c_str());
		/// up to here

		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MATCH_LINE_DATA_UP);
		pWnd->SetWindowText(m_pView->GetMatchLineData(CaDraw_View::UP_SIDE_T));

		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MATCH_LINE_DATA_DOWN);
		pWnd->SetWindowText(m_pView->GetMatchLineData(CaDraw_View::DOWN_SIDE_T));

		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MATCH_LINE_DATA_EAST);
		pWnd->SetWindowText(m_pView->GetMatchLineData(CaDraw_View::EAST_SIDE_T));
		
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MATCH_LINE_DATA_WEST);
		pWnd->SetWindowText(m_pView->GetMatchLineData(CaDraw_View::WEST_SIDE_T));

		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MATCH_LINE_DATA_SOUTH);
		pWnd->SetWindowText(m_pView->GetMatchLineData(CaDraw_View::SOUTH_SIDE_T));
		
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MATCH_LINE_DATA_NORTH);
		pWnd->SetWindowText(m_pView->GetMatchLineData(CaDraw_View::NORTH_SIDE_T));
		
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_VIEW_INFO);
		pWnd->SetWindowText(m_pView->GetViewInfo());
	}

	return 0;
}

/**	
	@brief	save view information
	@author	HumKyung.BAEK
	@date	2010.05.12
	@param	
	@return
*/
int CDraViewSettingDlg::SaveViewInformation(const CString& rDraFilePath)
{
	if(NULL != m_pView)
	{
		CString rString;
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_DIRECTION);
		pComboBox->GetWindowText(rString);
	
		try
		{
			CaDraw_View::DIRECTIONT_T dir = CaDraw_View::GetViewDirFrom(rString.operator LPCTSTR());
			m_pView->SetViewDir(dir);
		}
		catch(const exception& ex)
		{
			ex;
		}

		/// set volume
		CWnd* pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MINX);
		pWnd->GetWindowText(rString);
		const double minx = ATOF_T(rString);
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MINY);
		pWnd->GetWindowText(rString);
		const double miny = ATOF_T(rString);
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MINZ);
		pWnd->GetWindowText(rString);
		const double minz = ATOF_T(rString);
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MAXX);
		pWnd->GetWindowText(rString);
		const double maxx = ATOF_T(rString);
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MAXY);
		pWnd->GetWindowText(rString);
		const double maxy = ATOF_T(rString);
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MAXZ);
		pWnd->GetWindowText(rString);
		const double maxz = ATOF_T(rString);

		m_pView->SetVolume(CIsVolume(minx , miny , minz , maxx , maxy , maxz));

		/// set scale
		pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SCALE);
		pComboBox->GetWindowText(rString);
		m_pView->SetScale(ATOF_T(rString));

		/// set angle
		pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_ANGLE);
		pComboBox->GetWindowText(rString);
		try
		{
			CaDraw_View::ROTATE_T rot = CaDraw_View::GetViewAngleFrom(rString.operator LPCTSTR());
			m_pView->SetViewAngle(rot);
		}
		catch(const exception& ex)
		{
			ex;
		}

		/// set view origin
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_VIEW_ORIGIN_X);
		pWnd->GetWindowText(rString);
		const double x = ATOF_T(rString);

		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_VIEW_ORIGIN_Y);
		pWnd->GetWindowText(rString);
		const double y = ATOF_T(rString);

		m_pView->SetOrigin( CIsPoint3d(x , y , 0.0) );

		/// set reference files
		list<CaDraw_View::MODEL_T> oReferenceList;
		const int nRefFileCount = m_wndReferenceListCtrl.GetItemCount();
		for(int i = 0;i < nRefFileCount;++i)
		{
			CaDraw_View::MODEL_T model;

			model.rModelName = m_wndReferenceListCtrl.GetItemText(i,0).operator LPCTSTR();
			const CString rRefFileType = m_wndReferenceListCtrl.GetItemText(i,1).operator LPCTSTR();
			model.modelType = CaDraw_Entity::GetModelType(rRefFileType.operator LPCTSTR());

			oReferenceList.push_back( model );
		}
		m_pView->SetModelList(&oReferenceList);

		/// set match line data
		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MATCH_LINE_DATA_UP);
		pWnd->GetWindowText(rString);
		m_pView->SetMatchLineData(CaDraw_View::UP_SIDE_T , rString.operator LPCTSTR());

		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MATCH_LINE_DATA_DOWN);
		pWnd->GetWindowText(rString);
		m_pView->SetMatchLineData(CaDraw_View::DOWN_SIDE_T , rString.operator LPCTSTR());

		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MATCH_LINE_DATA_EAST);
		pWnd->GetWindowText(rString);
		m_pView->SetMatchLineData(CaDraw_View::EAST_SIDE_T , rString.operator LPCTSTR());

		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MATCH_LINE_DATA_WEST);
		pWnd->GetWindowText(rString);
		m_pView->SetMatchLineData(CaDraw_View::WEST_SIDE_T , rString.operator LPCTSTR());

		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MATCH_LINE_DATA_SOUTH);
		pWnd->GetWindowText(rString);
		m_pView->SetMatchLineData(CaDraw_View::SOUTH_SIDE_T , rString.operator LPCTSTR());

		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_MATCH_LINE_DATA_NORTH);
		pWnd->GetWindowText(rString);
		m_pView->SetMatchLineData(CaDraw_View::NORTH_SIDE_T , rString.operator LPCTSTR());

		pWnd = (CWnd*)GetDlgItem(IDC_EDIT_VIEW_INFO);
		pWnd->GetWindowText(rString);
		m_pView->SetViewInfo(rString.operator LPCTSTR());
	}

	return ERROR_SUCCESS;
}

/**
	@brief	BEGIN DRAG

	@author HumKyung.BAEK

	@date	2010.05.12

	@return		
**/
int CDraViewSettingDlg::OnBeginDrag(LPNMLISTVIEW pNMLV , CMFCListCtrl& ListCtrl)
{
	//// Save the index of the item being dragged in m_nDragIndex
	////  This will be used later for retrieving the info dragged
	m_nDragIndex = pNMLV->iItem;

	//// Create a drag image
	POINT pt;
	int nOffset = -10; //offset in pixels for drag image (positive is up and to the left; neg is down and to the right)
	if((pNMLV->hdr.hwndFrom == ListCtrl.GetSafeHwnd()) && (ListCtrl.GetSelectedCount() > 0)) //! more than one item is selected
	{
		pt.x = nOffset;
		pt.y = nOffset;

		m_pDragImage = ListCtrl.CreateDragImage(m_nDragIndex, &pt);
		ASSERT(m_pDragImage); //make sure it was created
		//We will call delete later (in LButtonUp) to clean this up

		CBitmap bitmap;
		if(ListCtrl.GetSelectedCount() > 1) //more than 1 item in list is selected
			//bitmap.LoadBitmap(IDB_BITMAP_MULTI);
			bitmap.LoadBitmap(IDB_MULTI_BOXES);
		else	bitmap.LoadBitmap(IDB_BOX);
		m_pDragImage->Replace(0, &bitmap, &bitmap);

		//// Change the cursor to the drag image
		////	(still must perform DragMove() in OnMouseMove() to show it moving)
		m_pDragImage->BeginDrag(0, CPoint(nOffset, nOffset - 4));
		m_pDragImage->DragEnter(GetDesktopWindow(), pNMLV->ptAction);

		//// Set dragging flag and others
		m_bDragging = TRUE;	//we are in a drag and drop operation
		m_nDropIndex = -1;	//we don't have a drop index yet
		m_pDragList = &ListCtrl; //make note of which list we are dragging from
		m_pDropList = &ListCtrl; //at present the drag list is the drop list

		//// Capture all mouse messages
		SetCapture ();
	}

	return ERROR_SUCCESS;
}

/**
	@brief	ListCtrl에서 선택된 아이템을 구한다.

	@author HumKyung.BAEK

	@date	2010.05.12

	@return		
**/
void CDraViewSettingDlg::GetSelectedDragItems(CMFCListCtrl& ListCtrl , vector<long>& DragItemIndexEntry)
{
	POSITION pos = ListCtrl.GetFirstSelectedItemPosition();
	while(pos)
	{
		const int nItem  = ListCtrl.GetNextSelectedItem(pos);		
		DragItemIndexEntry.push_back( nItem );
	}
}

/**
	@brief	called when user drop item.

	@author HumKyung.BAEK	

	@date	2010.05.12

	@return		
**/
void CDraViewSettingDlg::DropItemOnList(CMFCListCtrl* pDragList , CMFCListCtrl* pDropList)
{
	vector<long> DragItemIndexEntry;
	GetSelectedDragItems(*pDragList , DragItemIndexEntry);

	if(pDropList->GetSafeHwnd() == m_wndReferenceListCtrl.GetSafeHwnd())
	{
		{
			// Turn off hilight for previous drop target
			pDropList->SetItemState(m_nDropIndex, 0, LVIS_DROPHILITED);
			pDropList->SetItemState(m_nDropIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			// Redraw previous item
			pDropList->RedrawItems (m_nDropIndex, m_nDropIndex);
			
			//! 드래그 한곳의 아이템을 삭제한다.
			for( vector< long >::reverse_iterator itr = DragItemIndexEntry.rbegin();itr != DragItemIndexEntry.rend();++itr)
			{
				pDragList->DeleteItem(*itr);
			}
		}
	}
}

/**
	@brief	Begin Drag

	@author HumKyung.BAEK	

	@date	2010.05.12

	@return		
**/

void CDraViewSettingDlg::OnLvnBegindragList(UINT id , NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if(NULL == pNMLV) return;

	map<int,CMFCListCtrl*>::iterator where = m_ModelFolderListCtrlMap.find(id);
	if(where != m_ModelFolderListCtrlMap.end())
	{
		OnBeginDrag(pNMLV , *(where->second));
	}

	*pResult = 0;
}

/**
	@brief	MOUSE MOVE

	@author HumKyung.BAEK	

	@date	2010.05.12

	@return		
**/
void CDraViewSettingDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		//// Move the drag image
		CPoint pt(point);	//get our current mouse coordinates
		ClientToScreen(&pt); //convert to screen coordinates
		m_pDragImage->DragMove(pt); //move the drag image to those coordinates
		// Unlock window updates (this allows the dragging image to be shown smoothly)
		m_pDragImage->DragShowNolock(false);

		//// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT(pDropWnd); //make sure we have a window

		//// If we drag outside current window we need to adjust the highlights displayed
		if (pDropWnd != m_pDropList)
		{
			if (m_nDropIndex != -1) //If we drag over the CListCtrl header, turn off the hover highlight
			{
				TRACE("m_nDropIndex is -1\n");
				CMFCListCtrl* pList = (CMFCListCtrl*)m_pDropList;
				//VERIFY (pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED));
				// redraw item
				//VERIFY (pList->RedrawItems (m_nDropIndex, m_nDropIndex));
				//pList->UpdateWindow ();
				m_nDropIndex = -1;
			}
			else //If we drag out of the CListCtrl altogether
			{
				TRACE("m_nDropIndex is not -1\n");
				//CListCtrl* pList = (CListCtrl*)m_pDropList;
				//int i = 0;
				//int nCount = pList->GetItemCount();
				//for(i = 0; i < nCount; i++)
				//{
				//	pList->SetItemState(i, 0, LVIS_DROPHILITED);
				//}
				//pList->RedrawItems(0, nCount);
				//pList->UpdateWindow();
			}
		}

		// Save current window pointer as the CListCtrl we are dropping onto
		m_pDropList = (CMFCListCtrl*)pDropWnd;
		if(m_pDropList != m_pDragList)
		{
			// Convert from screen coordinates to drop target client coordinates
			pDropWnd->ScreenToClient(&pt);

			//If we are hovering over a CListCtrl we need to adjust the highlights
			if(pDropWnd->IsKindOf(RUNTIME_CLASS(CMFCListCtrl)) && (pDropWnd->GetSafeHwnd() == m_wndReferenceListCtrl.GetSafeHwnd()))
			{
				TRACE("m_nDropIndex is not 100\n");
				//Note that we can drop here
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				CListCtrl* pList = (CListCtrl*)pDropWnd;
			}
		}
		else
		{
			//If we are not hovering over a CListCtrl, change the cursor
			// to note that we cannot drop here
			SetCursor(LoadCursor(NULL, IDC_NO));
		}
		// Lock window updates
		m_pDragImage->DragShowNolock(true);
	}
		
	CDialog::OnMouseMove(nFlags, point);
}

/**
	@brief	LBUTTON UP

	@author HumKyung.BAEK	

	@date	2010.05.12

	@return		
**/
void CDraViewSettingDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		// Release mouse capture, so that other controls can get control/messages
		ReleaseCapture ();
		
		// Note that we are NOT in a drag operation
		m_bDragging = FALSE;
		
		// End dragging image
		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();
		delete m_pDragImage; //must delete it because it was created at the beginning of the drag
		
		CPoint pt (point); //Get current mouse coordinates
		ClientToScreen (&pt); //Convert to screen coordinates
		// Get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT (pDropWnd); //make sure we have a window pointer
		// If window is CListCtrl, we perform the drop
		if ((m_pDragList != pDropWnd) && pDropWnd->IsKindOf (RUNTIME_CLASS (CMFCListCtrl)))
		{
			m_pDropList = (CMFCListCtrl*)pDropWnd; //Set pointer to the list we are dropping on
			DropItemOnList(m_pDragList, m_pDropList); //Call routine to perform the actual drop
		}
	}
	
	CDialog::OnLButtonUp(nFlags, point);
}

/**
	@brief	add selected model file as reference

	@author HumKyung.BAEK	

	@date	2010.05.12

	@return		
**/
void CDraViewSettingDlg::OnBnClickedButtonAddReference()
{
	const int nTab = m_wndTabs.GetActiveTab();
	if(-1 != nTab)
	{
		CString strLabel;
		m_wndTabs.GetTabLabel(nTab , strLabel);

		CMFCListCtrl* pListCtrl = (CMFCListCtrl*)m_wndTabs.GetTabWnd(nTab);

		list<int> SelectedItemEntry;
		POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
		while(pos)
		{
			int nItem = pListCtrl->GetNextSelectedItem(pos);
			SelectedItemEntry.push_back(nItem);
			const CString rItemText = pListCtrl->GetItemText(nItem , 0);

			nItem = m_wndReferenceListCtrl.InsertItem(m_wndReferenceListCtrl.GetItemCount() , rItemText);
			m_wndReferenceListCtrl.SetItemText(nItem , 1 , strLabel);
		}

		//! reference로 추가한 item을 제거한다.
		for(list<int>::reverse_iterator itr = SelectedItemEntry.rbegin();itr != SelectedItemEntry.rend();++itr)
		{
			pListCtrl->DeleteItem(*itr);
		}
	}
}

/**
	@brief	remove selected reference file from listctrl.

	@author HumKyung.BAEK	

	@date	2010.05.12

	@return		
**/
void CDraViewSettingDlg::OnBnClickedButtonRemoveReference()
{
	list<int> SelectedItemEntry;
	POSITION pos = m_wndReferenceListCtrl.GetFirstSelectedItemPosition();
	while(pos)
	{
		int nItem = m_wndReferenceListCtrl.GetNextSelectedItem(pos);
		SelectedItemEntry.push_back(nItem);
		const CString rItemName = m_wndReferenceListCtrl.GetItemText(nItem , 0);
		const CString rItemType = m_wndReferenceListCtrl.GetItemText(nItem , 1);

		for(int i = 0;i < m_wndTabs.GetTabsNum();++i)
		{
			CString strLabel;
			m_wndTabs.GetTabLabel(i,strLabel);
			if(rItemType == strLabel)
			{
				CMFCListCtrl* pListCtrl = (CMFCListCtrl*)m_wndTabs.GetTabWnd(i);
				pListCtrl->InsertItem(pListCtrl->GetItemCount() , rItemName);
				break;
			}
		}
	}

	//! reference로 추가한 item을 제거한다.
	for(list<int>::reverse_iterator itr = SelectedItemEntry.rbegin();itr != SelectedItemEntry.rend();++itr)
	{
		m_wndReferenceListCtrl.DeleteItem(*itr);
	}
}

void CDraViewSettingDlg::OnBnClickedButtonPickViewOrigin()
{
	// TODO: Add your control notification handler code here
}