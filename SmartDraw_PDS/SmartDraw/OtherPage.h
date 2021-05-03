#pragma once


// COtherPage dialog
namespace PropertyPage
{
class COtherPage : public CDialog
{
	DECLARE_DYNAMIC(COtherPage)

public:
	COtherPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~COtherPage();

// Dialog Data
	enum { IDD = IDD_OTHER_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
};