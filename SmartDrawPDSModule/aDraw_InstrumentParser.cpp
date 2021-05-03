#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include <aDraw_Instrument.h>
#include "aDraw_InstrumentParser.h"


CaDraw_InstrumentParser::CaDraw_InstrumentParser(const CaDraw_Entity::MODEL_TYPE_T& eModelType , const STRING_T& rModelPath , const double& uor_per_subunit)
{
	m_eModelType = eModelType;
	m_rModelName = rModelPath;
	m_uor_per_subunit = uor_per_subunit;
	m_occur = -1;
}

CaDraw_InstrumentParser::~CaDraw_InstrumentParser(void)
{
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
int CaDraw_InstrumentParser::Parse(CaDraw_View* pDraView , CDgnCellHeader* pDgnCell, CADODB& adoDB , const int& nPartitionNo , const CIsPoint3d& ptModelOffset)
{
	assert(pDgnCell && pDraView && "pDgnCell or pDraView is NULL");

	if(pDgnCell && pDraView)
	{
		DPoint3d _origin = pDgnCell->origin();
		CIsPoint3d origin = (CIsPoint3d(_origin.x , _origin.y , _origin.z) / m_uor_per_subunit) + ptModelOffset;
		
		CIsPoint3d ptMin(pDgnCell->volume().minx() / m_uor_per_subunit , pDgnCell->volume().miny() / m_uor_per_subunit , pDgnCell->volume().minz() / m_uor_per_subunit);
		ptMin += ptModelOffset;
		CIsPoint3d ptMax(pDgnCell->volume().maxx() / m_uor_per_subunit , pDgnCell->volume().maxy() / m_uor_per_subunit , pDgnCell->volume().maxz() / m_uor_per_subunit);
		ptMax += ptModelOffset;
		CIsVolume volume;
		volume.Add(ptMin); volume.Add(ptMax);

	    CString rSql;
		rSql.Format(_T("SELECT instrument_comp_no FROM PDTABLE_67_%d WHERE system_unique_no = '%d'") , nPartitionNo , m_occur);
		adoDB.OpenQuery(rSql.operator LPCTSTR());

		STRING_T instrument_comp_no;
		LONG lRecordCount = 0L;
		adoDB.GetRecordCount(&lRecordCount);
		if(1 == lRecordCount)
		{
			adoDB.GetFieldValue(0 , _T("instrument_comp_no") , &instrument_comp_no);
		}

		if(instrument_comp_no.empty()) instrument_comp_no = _T("NIL");

		CaDraw_Instrument* pInst = (CaDraw_Instrument*)CaDraw_Instrument::CreateInstance();
		if(pInst)
		{
			pInst->SetModelType(m_eModelType);
			pInst->SetModelName(m_rModelName);
			pInst->SetVolume(volume);
			pInst->SetOrigin(origin);
			pInst->SetName(instrument_comp_no);
			pDraView->Add(pInst);
		}
	}

	return ERROR_SUCCESS;
}