#include "StdAfx.h"
#include <assert.h>
#include <IsTools.h>
#include <aDraw_Elbow.h>
#include <aDraw_Reducer.h>
#include <aDraw_Valve.h>
#include <aDraw_Specialty.h>
#include "aDraw_GeneralPipingParser.h"
#include "aDraw_InchToMetricTable.h"

CaDraw_GeneralPipingParser::CaDraw_GeneralPipingParser(const CaDraw_Entity::MODEL_TYPE_T& eModelType , const STRING_T& rModelPath , const double& uor_per_subunit)
{
	m_eModelType = eModelType;
	m_rModelName = rModelPath;
	m_uor_per_subunit = uor_per_subunit;
	m_occur[0] = -1;
	m_occur[1] = -1;
	m_occur[2] = -1;
}

CaDraw_GeneralPipingParser::~CaDraw_GeneralPipingParser(void)
{
}

/**	
	@brief	
	@author	HumKyung.BAEK
	@date	2010.05.24
	@param	
	@return
*/
int CaDraw_GeneralPipingParser::Parse(CaDraw_View* pDraView , CDgnCellHeader* pDgnCell, CADODB& adoDB , const int& nPartitionNo , const CIsPoint3d& ptModelOffset)
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

	    CString sSql;
		sSql.Format(_T("SELECT piping_comp_no,model_code,COMMODITY_NAME,REMARKS FROM PDTABLE_34_%d WHERE system_unique_no = '%d'") , nPartitionNo , m_occur[0]);
		//sSql.Format(_T("SELECT piping_comp_no,model_code FROM PDTABLE_34_%d WHERE system_unique_no = '%d'") , nPartitionNo , m_occur[0]);
		adoDB.OpenQuery(sSql.operator LPCTSTR());

		LONG lRecordCount = 0L;
		adoDB.GetRecordCount(&lRecordCount);
		if(1 == lRecordCount)
		{
			STRING_T piping_comp_no , model_code,commodity_name,remarks;
			adoDB.GetFieldValue(0 , _T("piping_comp_no") , &piping_comp_no);
			adoDB.GetFieldValue(0 , _T("model_code") , &model_code);
			adoDB.GetFieldValue(0 , _T("commodity_name") , &commodity_name);
			adoDB.GetFieldValue(0 , _T("remarks") , &remarks);

			if((0 == STRICMP_T(_T("E90") , model_code.substr(0 , 3).c_str())) ||
			   (0 == STRICMP_T(_T("E45") , model_code.substr(0 , 3).c_str())))
			{
				const STRING_T sName1 = GetLineNumberLabel(adoDB , nPartitionNo , m_occur[1]);
				const STRING_T sName2 = GetLineNumberLabel(adoDB , nPartitionNo , m_occur[2]);

				CaDraw_Elbow* pElbow = (CaDraw_Elbow*)CaDraw_Elbow::CreateInstance();
				if(pElbow)
				{
					pElbow->SetModelType(m_eModelType);
					pElbow->SetModelName(m_rModelName);
					pElbow->SetVolume(volume);
					pElbow->SetOrigin(origin);
					pElbow->SetName(piping_comp_no);
					pElbow->SetConnName1(sName1);
					pElbow->SetConnName2(sName2);
					pDraView->Add(pElbow);
				}
			}
			else if(0 == STRICMP_T(_T("RED") , model_code.substr(0 , 3).c_str()))
			{
				CaDraw_Reducer* pReducer = (CaDraw_Reducer*)CaDraw_Reducer::CreateInstance();
				if(pReducer)
				{
					pReducer->SetModelType(m_eModelType);
					pReducer->SetModelName(m_rModelName);
					pReducer->SetVolume(volume);
					pReducer->SetOrigin(origin);
					pReducer->SetName(piping_comp_no);
					double* pSize = GetReducerSize(adoDB , nPartitionNo , m_occur[1]);
					pReducer->SetSize1(pSize[0]);
					pReducer->SetSize1InMetric(pSize[1]);	/// 2016.02.12 added by humkyung
					pSize = GetReducerSize(adoDB , nPartitionNo , m_occur[2]);
					pReducer->SetSize2(pSize[0]);
					pReducer->SetSize2InMetric(pSize[1]);	/// 2016.02.12 added by humkyung
					pDraView->Add(pReducer);
				}
			}
			else if((0 == STRICMP_T(_T("SPE") , model_code.c_str())) || (model_code.empty()))
			{
				CaDraw_Specialty* pSpecialty = (CaDraw_Specialty*)CaDraw_Specialty::CreateInstance();
				if(pSpecialty)
				{
					pSpecialty->SetModelType(m_eModelType);
					pSpecialty->SetModelName(m_rModelName);
					pSpecialty->SetVolume(volume);
					pSpecialty->SetOrigin(origin);
					pSpecialty->SetName(piping_comp_no);
					pDraView->Add(pSpecialty);
				}
			}
			else if(0 == commodity_name.find(_T("6Q1C")))
			{
				CaDraw_Valve* pValve = (CaDraw_Valve*)CaDraw_Valve::CreateInstance();
				if(pValve)
				{
					pValve->SetModelType(m_eModelType);
					pValve->SetModelName(m_rModelName);
					pValve->SetVolume(volume);
					pValve->SetOrigin(origin);
					pValve->SetName(piping_comp_no);
					pDraView->Add(pValve);
				}
			}
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
double* CaDraw_GeneralPipingParser::GetReducerSize(CADODB& adoDB , const int& nPartitionNo , const long& occur) const
{
	static double dNominalPipingDia[2] = {-1.f,-1.f};

	CString rSql;
	rSql.Format(_T("SELECT nominal_piping_dia FROM PDTABLE_12_%d WHERE system_unique_no = '%d'") , nPartitionNo , occur);
	adoDB.OpenQuery(rSql.operator LPCTSTR());

	LONG lRecordCount = 0L;
	adoDB.GetRecordCount(&lRecordCount);
	if(1 == lRecordCount)
	{
		STRING_T sNominalPipingDia;
		adoDB.GetFieldValue(0 , _T("nominal_piping_dia") , &sNominalPipingDia);
		dNominalPipingDia[0] = ATOF_T(sNominalPipingDia.c_str());
		//////////////////////////////////////////////////////////////////////////////////////////
		if(dNominalPipingDia[0] > 5000.f)
		{
			dNominalPipingDia[0] = (dNominalPipingDia[0] - 5000.f) / 32.f;
		}
		else
		{
			dNominalPipingDia[0] /= 25.4;
		}

		/// get metric size from inch value by using inch-metric table
		CaDraw_InchToMetricTable& InchToMetricTable = CaDraw_InchToMetricTable::GetInstance();
		CaDraw_InchToMetricTable::RangeValue* pRangeValue = InchToMetricTable.Find(dNominalPipingDia[0]);
		dNominalPipingDia[1] = (NULL != pRangeValue) ? pRangeValue->value : dNominalPipingDia[0] * 25.4;
	}

	return dNominalPipingDia;
}