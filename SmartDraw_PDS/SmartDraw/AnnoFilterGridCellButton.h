#pragma once

#include <GridCtrl/GridCell.h>
#include <AnnoFilter.h>

class CAnnoFilterGridCellButton : public CGridCell
{
	DECLARE_DYNCREATE(CAnnoFilterGridCellButton)
public:
	CAnnoFilterGridCellButton(void);
	~CAnnoFilterGridCellButton();
public: 
	int SaveData();
	int SetFilterItem(const STRING_T&,const STRING_T&);

     BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE); 
	 BOOL Edit(  int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
protected:  
	CRect m_rect; 
private:
	STRING_T m_sSettingName,m_sFilterItem;
	AnnoFilters* m_pAnnoFilters;
};