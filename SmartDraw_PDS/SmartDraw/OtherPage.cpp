// OtherPage.cpp : implementation file
//

#include "stdafx.h"
#include "SmartDraw.h"
#include "OtherPage.h"

using namespace PropertyPage;
// COtherPage dialog

IMPLEMENT_DYNAMIC(COtherPage, CDialog)

COtherPage::COtherPage(CWnd* pParent /*=NULL*/)
	: CDialog(COtherPage::IDD, pParent)
{

}

COtherPage::~COtherPage()
{
}

void COtherPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COtherPage, CDialog)
END_MESSAGE_MAP()


// COtherPage message handlers
