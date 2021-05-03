#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include <aDraw_Support.h>
#include "aDraw_SupportParser.h"


CaDraw_SupportParser::CaDraw_SupportParser(const CaDraw_Entity::MODEL_TYPE_T& eModelType , const STRING_T& rModelPath , const double& uor_per_subunit)
{
	m_eModelType = eModelType;
	m_rModelName = rModelPath;
	m_uor_per_subunit = uor_per_subunit;
	m_occur[0] = -1;
	m_occur[1] = -1;
}

CaDraw_SupportParser::~CaDraw_SupportParser(void)
{
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
int CaDraw_SupportParser::Parse(CaDraw_View* pDraView , CDgnCellHeader* pDgnCell, CADODB& adoDB , const int& nPartitionNo , const CIsPoint3d& ptModelOffset)
{
	assert(pDgnCell && pDraView && "pDgnCell or pDraView is NULL");

	if(pDgnCell && pDraView)
	{
		DPoint3d _origin = pDgnCell->origin();
		CIsPoint3d origin = (CIsPoint3d(_origin.x , _origin.y , _origin.z) / m_uor_per_subunit) + ptModelOffset;
		if(false == pDraView->GetVolume().Contains(origin))
		{
			return ERROR_BAD_ENVIRONMENT;
		}
		
		CIsPoint3d ptMin(pDgnCell->volume().minx() / m_uor_per_subunit , pDgnCell->volume().miny() / m_uor_per_subunit , pDgnCell->volume().minz() / m_uor_per_subunit);
		ptMin += ptModelOffset;
		CIsPoint3d ptMax(pDgnCell->volume().maxx() / m_uor_per_subunit , pDgnCell->volume().maxy() / m_uor_per_subunit , pDgnCell->volume().maxz() / m_uor_per_subunit);
		ptMax += ptModelOffset;

		CIsVolume volume;
		volume.Add(ptMin); volume.Add(ptMax);

		STRING_T rLineNumberLabel = GetLineNumberLabel(adoDB , nPartitionNo , m_occur[1]);
		if(rLineNumberLabel.empty()) rLineNumberLabel = _T("NO Line Number");

	    CString rSql;
		rSql.Format(_T("SELECT pipe_support_no,details_for_shop,details_for_field,commodity_code FROM PDTABLE_80_%d WHERE system_unique_no = '%d'") , nPartitionNo , m_occur[0]);
		adoDB.OpenQuery(rSql.operator LPCTSTR());

		STRING_T rName , rShopName , rFieldName , sCommodityCode;
		LONG lRecordCount = 0L;
		adoDB.GetRecordCount(&lRecordCount);
		if(1 == lRecordCount)
		{
			adoDB.GetFieldValue(0 , _T("pipe_support_no") , &rName);
			adoDB.GetFieldValue(0 , _T("details_for_shop") , &rShopName);
			adoDB.GetFieldValue(0 , _T("details_for_field") , &rFieldName);
			adoDB.GetFieldValue(0 , _T("commodity_code") , &sCommodityCode);
		}

		if(rShopName.empty()) rShopName = _T("NIL");
        if(rFieldName.empty()) rFieldName = _T("NIL");
		if(sCommodityCode.empty()) sCommodityCode = _T("NIL");

		if(!rName.empty())
		{
			CaDraw_Support* pSupport = (CaDraw_Support*)CaDraw_Support::CreateInstance();
			if(pSupport)
			{
				pSupport->SetModelType(m_eModelType);
				pSupport->SetModelName(m_rModelName);
				pSupport->SetVolume(volume);
				pSupport->SetOrigin(origin);
				pSupport->SetName(rName);
				pSupport->SetLineNo(rLineNumberLabel);
				pSupport->SetShopName(rShopName);
				pSupport->SetFieldName(rFieldName);
				pSupport->SetCommodityCode(sCommodityCode);
				pDraView->Add(pSupport);
			}
		}
	}

	return ERROR_SUCCESS;
}
