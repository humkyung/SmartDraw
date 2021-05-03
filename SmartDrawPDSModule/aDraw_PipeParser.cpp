#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include "aDraw_PipeDiaTable.h"	/// 2014.08.27 added by humkyung
#include "aDraw_PipeParser.h"

CaDraw_PipeParser::CaDraw_PipeParser(void) : m_uor_per_subunit(80.f)
{
}

CaDraw_PipeParser::~CaDraw_PipeParser(void)
{
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
int CaDraw_PipeParser::Parse(CaDraw_View* pDraView , CDgnLineString* pLineString, CADODB& adoDB , const int& nPartitionNo , const CIsPoint3d& ptModelOffset)
{
	assert(pLineString && pDraView && "pLineString or pDraView is NULL");

	if(pLineString && pDraView)
	{
		CaDraw_Pipe* pSeedDrawPipe = NULL;
		PDGNATTRDATA pAttr = pLineString->attribute();
		for(PDGNATTRDATA ptr = pAttr;ptr;ptr = ptr->next)
		{
			if(!(ptr->hdr.u) && !(ptr->hdr.i) && (1 == ptr->UserID))
			{
				//! this is pipe!!!
				const long occur = ptr->data.occur;
				pSeedDrawPipe = ParseAttribute(adoDB , nPartitionNo , occur);
				break;
			}
		}

		if(pSeedDrawPipe)
		{
			bool bUseSeedDrawPipe = false;
			for(int i = 0;i < int(pLineString->GetVertexCount()) - 1;++i)
			{
				DPoint3d start = pLineString->GetVertexAt(i);
				DPoint3d end = pLineString->GetVertexAt(i + 1);

				CIsPoint3d ptStart(start.x / m_uor_per_subunit , start.y / m_uor_per_subunit , start.z / m_uor_per_subunit);
				ptStart += ptModelOffset;
				CIsPoint3d ptEnd(end.x / m_uor_per_subunit , end.y / m_uor_per_subunit , end.z / m_uor_per_subunit);
				ptEnd += ptModelOffset;

				const STRING_T name1 = pSeedDrawPipe->line_label();
				if(_T("1B-SCL62009-A1-IP30") == name1)
				{
					int d = 1;
				}
				const CIsVolume volume(ptStart.x() , ptStart.y() , ptStart.z() , ptEnd.x() , ptEnd.y() , ptEnd.z());
				if(pDraView->DoesIncludeVolume(volume))
				{
					if(false == bUseSeedDrawPipe)
					{
						pSeedDrawPipe->SetStartPoint(ptStart);
						pSeedDrawPipe->SetEndPoint(ptEnd);
						pSeedDrawPipe->SetVolume(volume);
						pDraView->Add(pSeedDrawPipe);
						bUseSeedDrawPipe = true;
					}
					else
					{
						CaDraw_Pipe* pNewDrawPipe = (CaDraw_Pipe*)pSeedDrawPipe->Clone();
						pNewDrawPipe->SetStartPoint(ptStart);
						pNewDrawPipe->SetEndPoint(ptEnd);
						pNewDrawPipe->SetVolume(volume);
						pDraView->Add(pNewDrawPipe);
					}
				}
			}

			if(false == bUseSeedDrawPipe) SAFE_DELETE(pSeedDrawPipe);
		}
	}

	return ERROR_SUCCESS;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
CaDraw_Pipe* CaDraw_PipeParser::ParseAttribute(CADODB& adoDB , const int& nPartitionNo , const long& occur)
{
	CaDraw_Pipe* pSeedDrawPipe = NULL;

	CString rSql;
	rSql.Format(_T("SELECT line_number_label,line_id,nominal_piping_dia,piping_mater_class,insulation_thick FROM PDTABLE_12_%d WHERE system_unique_no='%d'") , nPartitionNo , occur);
	adoDB.OpenQuery(rSql.operator LPCTSTR());
	LONG lRecordCount = 0L;
	adoDB.GetRecordCount(&lRecordCount);
	if(1 == lRecordCount)
	{
		STRING_T rValue;
		pSeedDrawPipe = (CaDraw_Pipe*)CaDraw_Pipe::CreateInstance();
		pSeedDrawPipe->SetModelType(m_eModelType);
		pSeedDrawPipe->SetModelName(m_rModelName);
		const int nFieldCount = adoDB.GetFieldCount();
		for(int i = 0;i < nFieldCount;++i)
		{
			STRING_T rFieldName(adoDB.GetFieldName(i));
			adoDB.GetFieldValue(0 , rFieldName , &rValue);
			pSeedDrawPipe->SetAttribute(rFieldName , rValue);
		}

		STRING_T rName1 , rName2 , rSpec , rInsul;
		long nDia = 5032;
		adoDB.GetFieldValue(0 , _T("line_number_label") , &rName1);
		pSeedDrawPipe->SetLineLabel(rName1);
		adoDB.GetFieldValue(0 , _T("line_id") , &rName2);
		pSeedDrawPipe->SetLineId(rName2);
		adoDB.GetFieldValue(0 , _T("nominal_piping_dia") , &rValue);
		nDia = ATOI_T(rValue.c_str());
		adoDB.GetFieldValue(0 , _T("piping_mater_class") , &rSpec);
		pSeedDrawPipe->SetSpec(rSpec);
		adoDB.GetFieldValue(0 , _T("insulation_thick") , &rInsul);

		//////////////////////////////////////////////////////////////////////////////////////////
		double nSize = 0.f;
		if(nDia > 5000.f)
		{
			nSize = (nDia - 5000) / 32.f;
		}
		else
		{
			nSize = nDia / 25.4;
		}
		pSeedDrawPipe->SetNominalPipeSize(nSize);
		pSeedDrawPipe->SetNominalPipeSizeInMetric(nSize*25.4);

		/// get stand for size of pipe - 2014.08.27 added by humkyung
		CaDraw_PipeDiaTable& oPipeDiaTable = CaDraw_PipeDiaTable::GetInstance();
		CaDraw_PipeDiaTable::RangeValue* pRangeVal = oPipeDiaTable.Find(nSize*25.4);
		if(NULL != pRangeVal)
		{
			nSize = pRangeVal->value/25.4;
		}
		/// up to here

		pSeedDrawPipe->SetSize(nSize);
		
		//////////////////////////////////////////////////////////////////////////////////////////
		double nInsul = ATOF_T(rInsul.c_str());
		if(nInsul > 0.f)
		{
			pSeedDrawPipe->SetInsThickInMetric(nInsul);
			nInsul /= 25.4;
			pSeedDrawPipe->SetInsThick(nInsul);
		}
	}
	else
	{
		//! TODO: 에러 처리할 것
	}

	return pSeedDrawPipe;
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
STRING_T CaDraw_PipeParser::GetLineNumberLabel(CADODB& adoDB , const int& nPartitionNo , const long& occur)
{
	CString rSql;
	rSql.Format(_T("SELECT line_number_label FROM PDTABLE_12_%d WHERE system_unique_no = '%d'") , nPartitionNo , occur);
	adoDB.OpenQuery(rSql.operator LPCTSTR());

	STRING_T strLineNumberLabel;
	LONG lRecordCount = 0L;
	adoDB.GetRecordCount(&lRecordCount);
	if(1 == lRecordCount)
	{
		STRING_T sValue;
		adoDB.GetFieldValue(0 , _T("line_number_label") , &sValue);
		IsString::TrimWhiteSpace(sValue);

		if(0 == sValue.find_first_of(_T("NUNDEFINED")))
		{
			strLineNumberLabel = _T("NUNDEFINED");
		}
		else if(0 == sValue.find_first_of(_T("UNDEFINED")))
		{
			strLineNumberLabel = _T("UNDEFINED");
		}
	}

	return strLineNumberLabel;
}