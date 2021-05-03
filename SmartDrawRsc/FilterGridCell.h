// FilterGridCell.h: interface for the CFilterGridCell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILTERGRIDCELL_H__D561A19A_9F1B_45BE_9825_C4D6F28BBB23__INCLUDED_)
#define AFX_FILTERGRIDCELL_H__D561A19A_9F1B_45BE_9825_C4D6F28BBB23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <gui/GridCtrl/GridCell.h>
#include "SmartDrawRscImp.h"

class ON_EXT_SMARTDRAWRSC CFilterGridCell : public CGridCell  
{
	DECLARE_DYNCREATE(CFilterGridCell)
public:
	CFilterGridCell();
	virtual ~CFilterGridCell();
public:
	virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
};

#endif // !defined(AFX_FILTERGRIDCELL_H__D561A19A_9F1B_45BE_9825_C4D6F28BBB23__INCLUDED_)
