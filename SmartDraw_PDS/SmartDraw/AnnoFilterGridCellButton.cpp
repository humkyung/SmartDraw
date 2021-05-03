#include "StdAfx.h"
#include <GridCtrl/GridCtrl.h>
#include <IsString.h>
#include <AnnoFilter.h>
#include "resource.h"
#include "AnnoFilterDlg.h"
#include "AnnoFilterGridCellButton.h"
#include "SmartDrawDoc.h"

#include "soci.h"
#include "soci-sqlite3.h"

#ifdef _DEBUG  
#define new DEBUG_NEW  
#undef THIS_FILE  
static char THIS_FILE[] = __FILE__;  
#endif  

IMPLEMENT_DYNCREATE(CAnnoFilterGridCellButton, CGridCell) 
CAnnoFilterGridCellButton::CAnnoFilterGridCellButton(void)
{
	m_pAnnoFilters = new AnnoFilters;
}

CAnnoFilterGridCellButton::~CAnnoFilterGridCellButton()
{
	try
	{
		for(AnnoFilters::iterator itr = m_pAnnoFilters->begin();itr != m_pAnnoFilters->end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		SAFE_DELETE(m_pAnnoFilters);
	}
	catch(...)
	{
	}
}

BOOL CAnnoFilterGridCellButton::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd /* = TRUE */)  
{   
	CGridCell::Draw(pDC,nRow,nCol,rect,bEraseBkgnd);

	m_rect = rect;  
    rect.DeflateRect(GetMargin(), 0);  
    pDC->DrawFrameControl(CRect(rect.right-17,rect.top,rect.right,rect.bottom), DFC_BUTTON, DFCS_BUTTONPUSH);  
	HICON hIcon = AfxGetApp()->LoadIcon(IDI_FILTER);
	::DrawIconEx(pDC->GetSafeHdc(),rect.right-16,rect.top + (int)(rect.Height() - 16)*0.5,hIcon,16,16,0,0,DI_NORMAL);
    
	return TRUE;  
}  

BOOL CAnnoFilterGridCellButton::Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar)
{
	if(TRUE == CRect(rect.right-rect.left-16,0,rect.Width(),rect.Height()).PtInRect(point))
	{
		CAnnoFilterDlg dlg(m_sSettingName,m_sFilterItem,m_pAnnoFilters);
		if(IDOK == dlg.DoModal())
		{
			for(AnnoFilters::iterator itr = m_pAnnoFilters->begin();itr != m_pAnnoFilters->end();++itr)
			{
				SAFE_DELETE(*itr);
			}
			m_pAnnoFilters->clear();

			STRING_T sValue;
			for(AnnoFilters::const_iterator itr = dlg.m_pAnnoFilters->begin();itr != dlg.m_pAnnoFilters->end();++itr)
			{
				m_pAnnoFilters->push_back((*itr)->Clone());
				sValue += (*itr)->ToString() + _T(";");
			}

			this->SetText(sValue.c_str());
		}
	}

    return TRUE;
}

/**
	@brief	set filter item
	@author	humkyung
	@date	2016.03.17
*/
int CAnnoFilterGridCellButton::SetFilterItem(const STRING_T& sSettingName,const STRING_T& sFilterItem)
{
	m_sSettingName = sSettingName;
	m_sFilterItem = sFilterItem;

	for(AnnoFilters::iterator itr = m_pAnnoFilters->begin();itr != m_pAnnoFilters->end();++itr)
	{
		SAFE_DELETE(*itr);
	}

	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	STRING_T sText;
	soci::session oSession(*soci::factory_sqlite3() , pDoc->GetRefFilePath().c_str());
	{
		OSTRINGSTREAM_T oss;
		oss << _T("select Seq,Prop,Oper,Value from AnnoFilters where Setting='") << sSettingName << _T("' and Item='") << sFilterItem << _T("' order by Seq");
		soci::rowset<soci::row> rs(oSession.prepare << oss.str());
		for(soci::rowset<soci::row>::const_iterator itr = rs.begin();itr != rs.end();++itr)
		{
			if((soci::indicator::i_ok != itr->get_indicator(0)) || (soci::indicator::i_ok != itr->get_indicator(1)) || (soci::indicator::i_ok != itr->get_indicator(2))) continue;
			CAnnoFilter* pAnnoFilter = new CAnnoFilter(itr->get<int>(0),itr->get<STRING_T>(1),itr->get<STRING_T>(2),
				(soci::indicator::i_ok == itr->get_indicator(3) ? itr->get<STRING_T>(3) : _T("")));
			sText += pAnnoFilter->ToString() + _T(";");
			m_pAnnoFilters->push_back(pAnnoFilter);
		}
	}
	this->SetText(sText.c_str());

	return ERROR_SUCCESS;
}

/**
	@brief	save data to database
	@author	humkyung
	@date	2016.03.21
*/
int CAnnoFilterGridCellButton::SaveData()
{
	CSmartDrawDoc* pDoc = GetMDIActiveDocument();
	soci::session oSession(*soci::factory_sqlite3() , pDoc->GetRefFilePath().c_str());
	try
	{
		soci::transaction txn(oSession);
		oSession << _T("delete from AnnoFilters where Setting='") << m_sSettingName << _T("' and Item='") << m_sFilterItem << _T("'");
		for(AnnoFilters::iterator itr = m_pAnnoFilters->begin();itr != m_pAnnoFilters->end();++itr)
		{
			STRING_T sValue((*itr)->GetValue());
			IsString::ReplaceOf(sValue,_T("'"),_T("''"));
			oSession << _T("insert into AnnoFilters(Seq,Setting,Item,Prop,Oper,Value) values(") << (*itr)->GetSeq() << _T(",'") << 
				m_sSettingName << _T("','") << m_sFilterItem << _T("','") << (*itr)->GetProperty() << _T("','") << (*itr)->GetOperator() << _T("','") <<
				sValue << _T("')");
		}
		txn.commit();
	}
	catch(...)
	{
	}

	return ERROR_SUCCESS;
}