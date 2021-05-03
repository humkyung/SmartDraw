#include "StdAfx.h"
#include <tchar.h>
#include <assert.h>
#include <Tokenizer.h>

#include "SmartDrawAnnoEnv.h"
#include "Drawing.h"
#include "SmartDrawAnnoView.h"

#include "NozzleChart.h"
#include "RevisionChart.h"
#include "KeyPlan.h"
#include "NorthMark.h"
#include "KeyPlanHatching.h"

CDrawing::CDrawing(void)
{
	m_pHsrLineList = new list<CIsLine2d>;
	m_pAnnoTerritoryList = new list<CAnnoTerritory*>;
}

CDrawing::~CDrawing(void)
{
	try
	{
		SAFE_DELETE( m_pHsrLineList );
		SAFE_DELETE( m_pAnnoTerritoryList );
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-02
    @class      CDrawing
    @function   Annotate
    @return     int
    @brief
******************************************************************************/
int CDrawing::Annotate(const STRING_T& sOutputFilePath , const STRING_T& sHsrFilePath , CaDraw_DraFile* pDraFile,const double& dDrawingScale)
{
	assert(pDraFile && "pDraFile is NULL");

	if(pDraFile)
	{
		CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

		this->ParseHsrTextFile(sHsrFilePath);

		pDraFile->SetExtTagSide( env.m_area.m_iExtTagSide );	/// set Ext_Tag_Side - 2012.03.18 added by humkyung
		pDraFile->SetViewCoords( env.m_coord.m_eOption );		/// set view coordinate option - 2012.03.25 added by humkyung

		pDraFile->CreateLogicView();
		list<CaDraw_LogicView*> oLogicViewList;
		pDraFile->GetLogicViewList(&oLogicViewList);
		for(list<CaDraw_LogicView*>::iterator itr = oLogicViewList.begin();itr != oLogicViewList.end();++itr)
		{
			CAnnoTerritory* pAnnoTerritory = new CAnnoTerritory(*itr);
			if(NULL != pAnnoTerritory)
			{
				pAnnoTerritory->Prepare();
				///pAnnoTerritory->Annotate(*m_pHsrLineList);
				m_pAnnoTerritoryList->push_back(pAnnoTerritory);
			}
		}

		this->Write(sOutputFilePath , pDraFile->GetFileNameWithoutExt() , dDrawingScale);

		/// clear
		for(list<CAnnoTerritory*>::iterator itr = m_pAnnoTerritoryList->begin();itr != m_pAnnoTerritoryList->end();++itr)
		{
			SAFE_DELETE(*itr);
		}
		m_pAnnoTerritoryList->clear();
		/// up to here

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-02
    @class      CDrawing
    @function   Write
    @return     int
    @param      const STRING_T&
    @param      sOutputFilePath
    @brief
******************************************************************************/
int CDrawing::Write(const STRING_T& sOutputFilePath , const STRING_T& sDraFileName,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	OFSTREAM_T ofile(sOutputFilePath.c_str());
	if(ofile.is_open())
	{
		ofile.precision( 5 );
		ofile.setf(ios_base:: fixed, ios_base:: floatfield);

		env.m_intDimStyle.Write( ofile,dDrawingScale );
		env.m_extDimStyle.Write( ofile,dDrawingScale );
		
		for(list<CAnnoTerritory*>::iterator itr = m_pAnnoTerritoryList->begin();itr != m_pAnnoTerritoryList->end();++itr)
		{
			(*itr)->Write(ofile,dDrawingScale);
		}

		this->WriteBorderData(ofile , dDrawingScale);
		this->WriteNozzleChart(ofile,dDrawingScale);
		this->WriteRevisionChart(ofile , sDraFileName , dDrawingScale);
		
		ofile.close();

		return ERROR_SUCCESS;
	}

	return ERROR_FILE_NOT_FOUND;
}

/**
	@brief	write nozzle chart
	@author	humkyung
	@date	
	@return	int
*/
int CDrawing::WriteNozzleChart(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	if(false == env.m_generation.IsEnable(_T("Nozzle Chart"),NULL)) return ERROR_SUCCESS;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// write nozzle chart
	CNozzleChart oNozzleChart;
	for(list<CAnnoTerritory*>::const_iterator itr = m_pAnnoTerritoryList->begin();itr != m_pAnnoTerritoryList->end();++itr)
	{
		list<CSmartDrawAnnoView*>* pAnnoViewList = (*itr)->GetAnnoViewList();
		for(list<CSmartDrawAnnoView*>::iterator jtr = pAnnoViewList->begin();jtr != pAnnoViewList->end();++jtr)
		{
			list<CaDraw_TagEntity*> oTagEntList;
			(*jtr)->GetTagEntList(&oTagEntList);
			for(list<CaDraw_TagEntity*>::const_iterator ktr = oTagEntList.begin();ktr != oTagEntList.end();++ktr)
			{
				if((*ktr)->IsKindOf(CaDraw_TagEqui::TypeString()))
				{
					CaDraw_TagEqui* pTagEqui = static_cast<CaDraw_TagEqui*>(*ktr);
					oNozzleChart.m_pNozzleChartEntry->push_back(pTagEqui);
				}
			}
		}
	}
	oNozzleChart.LoadIni(env.m_sIniFilePath.c_str());
	oNozzleChart.Write(ofile , dDrawingScale);
	/// up to here

	return ERROR_SUCCESS;
}

/**
	@brief	write revision chart
	@author	humkyung
	@date	
	@return	int
*/
int CDrawing::WriteRevisionChart(OFSTREAM_T& ofile , const STRING_T& sDraFileName , const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	if(false == env.m_generation.IsEnable(_T("Revision Chart"),NULL)) return ERROR_SUCCESS;

	CRevisionChart oRevisionChart;
	oRevisionChart.Load(env.m_sIniFilePath.c_str() , sDraFileName.c_str());
	oRevisionChart.Write(ofile,dDrawingScale);

	return ERROR_SUCCESS;
}

/**
	@brief	write border data(north mark , key plan)
	@author	humkyung
	@date	2014.06.09
*/
int CDrawing::WriteBorderData(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	const STRING_T sIniFilePath(env.m_sIniFilePath);

	TCHAR szTmp[MAX_PATH + 1]={'\0',} , szBuf[MAX_PATH + 1]={'\0',};
	STRING_T sBorderCfgFilePath(sIniFilePath);
	if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		STRING_T sBorderFileName(szBuf);

		int at = sBorderCfgFilePath.rfind(_T("\\"));
		sBorderCfgFilePath = sBorderCfgFilePath.substr(0 , at);
		at = sBorderCfgFilePath.rfind(_T("\\"));
		sBorderCfgFilePath = sBorderCfgFilePath.substr(0 , at);
		sBorderCfgFilePath += _T("\\Border\\") + sBorderFileName + _T(".cfg");

		double dBorderScale = 1.0;
		if(GetPrivateProfileString(_T("GENERAL") , _T("Border_Scale") , _T("1") , szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			dBorderScale = 1.0/ATOF_T(szBuf);
		}

		CKeyPlan keyplan;
		keyplan.Write(ofile , dBorderScale , dDrawingScale , sBorderCfgFilePath);
		WriteKeyPlanHatching(ofile , dDrawingScale);

		/// get the largest TOP direciton view
		double dViewVolume = 0.0;
		CaDraw_View::ROTATE_T eRotate = CaDraw_View::R0_T;
		for(list<CAnnoTerritory*>::iterator itr = m_pAnnoTerritoryList->begin();itr != m_pAnnoTerritoryList->end();++itr)
		{
			list<CSmartDrawAnnoView*>* pAnnoViewList = (*itr)->GetAnnoViewList();
			for(list<CSmartDrawAnnoView*>::iterator jtr = pAnnoViewList->begin();jtr != pAnnoViewList->end();++jtr)
			{
				if(CaDraw_View::TOP_VIEW_T == (*jtr)->drawView()->GetViewDir())
				{
					const double dx = (*jtr)->drawView()->GetVolume().maxx() - (*jtr)->drawView()->GetVolume().minx();
					const double dy = (*jtr)->drawView()->GetVolume().maxy() - (*jtr)->drawView()->GetVolume().miny();
					const double dz = (*jtr)->drawView()->GetVolume().maxz() - (*jtr)->drawView()->GetVolume().minz();
					if(dViewVolume < dx*dy*dz)
					{
						eRotate = (*jtr)->drawView()->GetViewAngle();
						dViewVolume = dx*dy*dz;
					}
				}
			}
		}
		/// up to here
		CNorthMark northmark((double)eRotate);
		northmark.Write(ofile , dBorderScale , dDrawingScale , sBorderCfgFilePath);
	}
	
	return true;
}

/**
	@brief	write keyplan hatching
	@author	humkyung
	@date	2014.06.06
	@return	int
*/
int CDrawing::WriteKeyPlanHatching(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	if(false == env.m_generation.IsEnable(_T("KeyPlan Hatching"),NULL)) return ERROR_SUCCESS;

	CKeyPlanHatching oKeyPlanHatching;
	oKeyPlanHatching.LoadIni(env.m_sIniFilePath.c_str());
	for(list<CAnnoTerritory*>::iterator itr = m_pAnnoTerritoryList->begin();itr != m_pAnnoTerritoryList->end();++itr)
	{
		list<CSmartDrawAnnoView*>* pViewList = (*itr)->GetAnnoViewList();
		for(list<CSmartDrawAnnoView*>::iterator jtr = pViewList->begin();jtr != pViewList->end();++jtr)
		{
			oKeyPlanHatching.WriteHatching(ofile,*jtr,dDrawingScale);
		}
	}
	oKeyPlanHatching.WriteThisDrawing(ofile , dDrawingScale);

	return ERROR_SUCCESS;
}

/**	
	@brief	parse hidden text file(*.hsr)
	@author	humkyung
	@date	2010.06.08
	@param	
	@return
*/
int CDrawing::ParseHsrTextFile(const STRING_T& rHsrFilePath)
{
	IFSTREAM_T ifile(rHsrFilePath.c_str());
	if(ifile.is_open())
	{
		STRING_T aLine;
		vector<STRING_T> oResult;
		while(!ifile.eof())
		{
			getline(ifile , aLine);
			if(ifile.eof()) break;
			if(aLine.empty()) continue;

			CTokenizer<CIsFromString>::Tokenize(oResult , aLine , CIsFromString(_T("|")));
			if(!oResult.empty())
			{
				if((_T("<LINE>") == oResult[0]) && (oResult.size() > 1))
				{
					STRING_T sValue = oResult[1];
					CTokenizer<CIsComma>::Tokenize(oResult , sValue , CIsComma());
					if(6 == oResult.size())
					{
						CIsPoint2d start , end;
						start.x() = ATOF_T(oResult[0].c_str());
						start.y() = ATOF_T(oResult[1].c_str());
						double z  = ATOF_T(oResult[2].c_str());
						end.x() = ATOF_T(oResult[3].c_str());
						end.y() = ATOF_T(oResult[4].c_str());
						z       = ATOF_T(oResult[5].c_str());

						m_pHsrLineList->push_back(CIsLine2d(start , end));
					}
				}
				else if((_T("<PLINE>") == oResult[0]) && (oResult.size() > 1 ))
				{
					STRING_T sValue = oResult[1];
					CTokenizer<CIsComma>::Tokenize(oResult , sValue , CIsComma());
					if(!oResult.empty() && (0 == (oResult.size() % 2)))
					{
						CIsPoint2d start , end;
						for(vector<STRING_T>::iterator itr = oResult.begin();itr != oResult.end();++itr)
						{
							if(itr == oResult.begin())
							{
								start.x() = ATOF_T(itr->c_str());
								start.y() = ATOF_T((itr+1)->c_str());
							}
							else
							{
								end.x() = ATOF_T(itr->c_str());
								end.y() = ATOF_T((itr+1)->c_str());
								m_pHsrLineList->push_back(CIsLine2d(start , end));

								start = end;
							}
							++itr;
							if(itr == oResult.end()) break;
						}
					}
				}
				else if((_T("<CIRCLE>") == oResult[0]) && (oResult.size() > 1 ))
				{
					STRING_T sValue = oResult[1];
					CTokenizer<CIsComma>::Tokenize(oResult , sValue , CIsComma());
					if(4 == oResult.size())
					{
						CIsPoint2d origin;
						origin.x() = ATOF_T(oResult[0].c_str());
						origin.y() = ATOF_T(oResult[1].c_str());
						const double dRadius = ATOF_T(oResult[3].c_str());

						CIsPoint2d start(dRadius , 0.0) , end;
						const double cs = cos(DEG2RAD(30.)) , sn = sin(DEG2RAD(30.));
						for(int i = 1;i < 12;++i)
						{
							end.x() = start.x()*cs - start.y()*sn;
							end.y() = start.x()*sn + start.y()*cs;
							m_pHsrLineList->push_back(CIsLine2d(
								CIsPoint2d(origin.x() + start.x() , origin.y() + start.y()) ,
								CIsPoint2d(origin.x() + end.x() , origin.y() + end.y())));

							start = end;
						}
						m_pHsrLineList->push_back(CIsLine2d(
							CIsPoint2d(origin.x() + start.x() , origin.y() + start.y()) , 
							CIsPoint2d(origin.x() + dRadius , origin.y())));
					}
				}
				else if((_T("<ARC>") == oResult[0]) && (oResult.size() > 1 ))
				{
					STRING_T sValue = oResult[1];
					CTokenizer<CIsComma>::Tokenize(oResult , sValue , CIsComma());
					if(6 == oResult.size())
					{
						CIsPoint2d origin;
						origin.x() = ATOF_T(oResult[0].c_str());
						origin.y() = ATOF_T(oResult[1].c_str());
						const double z = ATOF_T(oResult[2].c_str());
						const double dRadius = ATOF_T(oResult[3].c_str());
						const double dStartAng = DEG2RAD(ATOF_T(oResult[4].c_str()));
						const double dEndAng   = DEG2RAD(ATOF_T(oResult[5].c_str()));

						CIsPoint2d start(dRadius , 0.0) , end;
						double cs = cos(dStartAng) , sn = sin(dStartAng);
						start.x() = dRadius*cs - 0.0*sn;
						start.y() = dRadius*sn + 0.0*cs;
						double dSweepAng = fabs(dEndAng - dStartAng);
						///dSweepAng = (dSweepAng < 0) ? 2*PI + dSweepAng : dSweepAng;
						cs = cos(dSweepAng / 6.0) , sn = sin(dSweepAng / 6.0);
						for(int i = 1;i < 6;++i)
						{
							end.x() = start.x()*cs - start.y()*sn;
							end.y() = start.x()*sn + start.y()*cs;
							m_pHsrLineList->push_back(CIsLine2d(
								CIsPoint2d(origin.x() + start.x() , origin.y() + start.y()) ,
								CIsPoint2d(origin.x() + end.x() , origin.y() + end.y())));

							start = end;
						}
						m_pHsrLineList->push_back(CIsLine2d(
							CIsPoint2d(origin.x() + start.x() , origin.y() + start.y()) , 
							CIsPoint2d(origin.x() + dRadius*cos(dEndAng) , origin.y() + dRadius*sin(dEndAng))));
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}
