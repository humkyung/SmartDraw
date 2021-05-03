#include "StdAfx.h"
#include <tchar.h>
#include <Tokenizer.h>
#include <aDraw_View.h>
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoEnv.h"
#include "aDraw_Platform.h"
#include "SmartDrawAnnoPlatform.h"

#include <fstream>

/**
	@brief	fill cell map with platform hatching data
	@author	humkyung
	@date	2014.04.16
*/
int CSmartDrawAnnoPlatform::CHatchingData::FillCellMap()
{
	CIsPoint2d min_(pt[0]) , max_(pt[0]);
	const int iEndIndex = (type == _T("9")) ? 9 : 6; 
	for(int i = 1;i < iEndIndex;++i)
	{
		min_.x() = min(min_.x() , pt[i].x());
		min_.y() = min(min_.y() , pt[i].y());
		max_.x() = max(max_.x() , pt[i].x());
		max_.y() = max(max_.y() , pt[i].y());
	}
	
	CSmartDrawAnnoModule::m_pCellMap->UpdateRectOf(min_ , max_ , CCellMap::TAG);

	return ERROR_SUCCESS;
}

/******************************************************************************
	@brief
    @author     humkyung
    @date       2012-02-25
    @function   CHatchingData::Write
    @return     int
    @param      OFSTREAM_T& ofile
******************************************************************************/
int CSmartDrawAnnoPlatform::CHatchingData::Write(OFSTREAM_T& ofile) const
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	/// Original
	ofile << _T("<HATCHPLINE>|") << pt[0].x() << _T(",") << pt[0].y() << _T(",");
	ofile << pt[1].x() << _T(",") << pt[1].y() << _T(",");
	ofile << pt[2].x() << _T(",") << pt[2].y() << _T(",");
	ofile << pt[3].x() << _T(",") << pt[3].y() << _T(",");
	ofile << pt[4].x() << _T(",") << pt[4].y() << _T(",");
	ofile << pt[5].x() << _T(",") << pt[5].y() << _T(",");
	if (type == _T("9")) 
	{
		ofile << pt[5].x() << _T(",") << pt[5].y() << _T(",");
		ofile << pt[6].x() << _T(",") << pt[6].y() << _T(",");
		ofile << pt[7].x() << _T(",") << pt[7].y() << _T(",");
		ofile << pt[8].x() << _T(",") << pt[8].y() << _T(",");
	}
	ofile << pt[0].x() << _T(",") << pt[0].y() << _T("|");
	ofile << env.m_platform.m_oHatchingProp.level << _T(",") << env.m_platform.m_oHatchingProp.color << _T(",");
	ofile << env.m_platform.m_sCellLibraryFilePath << _T(",") << env.m_platform.m_sCellName << _T(",");
	ofile << _T("45,1,No") << std::endl;

	if((_T("NULL") == env.m_platform.m_sCellLibraryFilePath) || (_T("NULL") == env.m_platform.m_sCellName))
	{
		/// Cross
		ofile << _T("<HATCHPLINE>|") << pt[0].x() << _T(",") << pt[0].y() << _T(",");
		ofile << pt[1].x() << _T(",") << pt[1].y() << _T(",");
		ofile << pt[2].x() << _T(",") << pt[2].y() << _T(",");
		ofile << pt[3].x() << _T(",") << pt[3].y() << _T(",");
		ofile << pt[4].x() << _T(",") << pt[4].y() << _T(",");
		ofile << pt[5].x() << _T(",") << pt[5].y() << _T(",");
		if (type == _T("9"))
		{
			ofile << pt[5].x() << _T(",") << pt[5].y() << _T(",");
			ofile << pt[6].x() << _T(",") << pt[6].y() << _T(",");
			ofile << pt[7].x() << _T(",") << pt[7].y() << _T(",");
			ofile << pt[8].x() << _T(",") << pt[8].y() << _T(",");
		}
		ofile << pt[0].x() << _T(",") << pt[0].y() << _T("|");
		ofile << env.m_platform.m_oHatchingProp.level << _T(",") << env.m_platform.m_oHatchingProp.color << _T(",");
		ofile << env.m_platform.m_sCellLibraryFilePath << _T(",") << env.m_platform.m_sCellName << _T(",");
		ofile << _T("45,1,No") << std::endl;
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-26
    @function   ENV::Parse
    @return     int
    @param      const           STRING_T&
    @param      sIniFilePath
    @brief
******************************************************************************/
int CSmartDrawAnnoPlatform::ENV::Parse(const STRING_T& sIniFilePath)
{
	static const STRING_T sApp(_T("ANNOTATION_OPTIONS"));
	TCHAR szBuf[MAX_PATH + 1] = {'\0',};

	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_TextStyle") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oTextStyle.Parse(szBuf , sIniFilePath);	
	}
	
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_TextLevel") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oTextProp.level = szBuf;
	}
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_TextColor") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oTextProp.color = szBuf;
	}
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_TextWeight") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oTextProp.weight = szBuf;
	}
	
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oHatchingProp.level = szBuf;
	}
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oHatchingProp.color = szBuf;
	}
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oHatchingProp.style = szBuf;
	}
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_Hatching_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oHatchingProp.weight = szBuf;
	}
	
	/// read platform hatching size - 2014.04.12 added by humkyung
	m_iHatchingSize = ::GetPrivateProfileInt(sApp.c_str() , _T("Platform_Hatching_Size") , 4 , sIniFilePath.c_str());
	/// up to here
	
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_Leader_Level") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.level = szBuf;
	}
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_Leader_Color") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.color = szBuf;
	}
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_Leader_Style") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.style = szBuf;
	}
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_Leader_Weight") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_oLeaderProp.weight = szBuf;
	}

	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_Arrow_Type") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sArrowType = szBuf;
	}
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_Arrow_Size") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_dArrowSize = ATOF_T(szBuf);
	}

	m_sCellLibraryFilePath = _T("NULL");
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_Hatching_CellLibrary") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		m_sCellLibraryFilePath = szBuf;
	}
	m_sCellName = _T("NULL");
	if(::GetPrivateProfileString(sApp.c_str() , _T("Platform_Hatching_CellName") , _T("") , szBuf , MAX_PATH , sIniFilePath.c_str()))
	{
		vector<STRING_T> oResult;
		CTokenizer<CIsFromString>::Tokenize(oResult , szBuf , CIsFromString(_T(" / ")));
		m_sCellName = oResult[0];
	}

	return ERROR_SUCCESS;
}

SMARTDRAW_TAG_IMPLEMENT_FUNC(CSmartDrawAnnoPlatform , CaDraw_TagEntity , _T("CSmartDrawAnnoPlatform"))

CSmartDrawAnnoPlatform::CSmartDrawAnnoPlatform(CaDraw_Entity* unnamed) : CaDraw_TagEntity(unnamed)
{
	m_dElevation = DBL_MAX;
	m_pPaveList = new vector<CIsPoint3d>;

	m_pAnnoResultList = new list<CCellMap::SearchRes>;
	m_pHatchingDataList = new list<CSmartDrawAnnoPlatform::CHatchingData>;
}

CSmartDrawAnnoPlatform::~CSmartDrawAnnoPlatform(void)
{
	try
	{
		SAFE_DELETE(m_pPaveList);
		SAFE_DELETE(m_pAnnoResultList);
		SAFE_DELETE(m_pHatchingDataList);
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-25
    @function   Annotate
    @return     int
    @brief
******************************************************************************/
int CSmartDrawAnnoPlatform::Annotate()
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();
	
	CaDraw_View* pView = this->GetView();
	CIsVolume vol = pView->GetVolume();
	vol = pView->ModelCoord2ViewCoord(vol);

	/// set m_bTagged
	m_bTagged = true;

	const int iSize = m_pPaveList->size();
	int iPrev = m_pPaveList->size() - 1 , iNext = 1 , pi = 1 , elvi = 1;
	CIsPoint3d first_pt, second_pt, last_pt;
	CIsPoint3d pr_pt, my_pt, nx_pt;
	if(iSize > 2)
	{
		for(pi = 0;pi < int(m_pPaveList->size());++pi,++iPrev,++iNext)
		{
			iPrev = iPrev % iSize;
			iNext = iNext % iSize;

			pr_pt = (*m_pPaveList)[iPrev];
			my_pt = (*m_pPaveList)[pi];
			nx_pt = (*m_pPaveList)[iNext];

			///o_elev = Pai_iter->elev_str;
			if(vol.Contains(my_pt))
			{
				/// get shortest distance between my_pt and others
				double dMinDist = DBL_MAX;
				for(int j = 0;j < int(m_pPaveList->size());++j)
				{
					if(j == pi) continue;
					const double d = CIsPoint2d((*m_pPaveList)[j].x() , (*m_pPaveList)[j].y()).DistanceTo(CIsPoint2d(my_pt.x() , my_pt.y()));
					dMinDist = min(dMinDist , d);
				}
				/// up to here

				{
					CSmartDrawAnnoPlatform::CHatchingData oHatchingData = this->GenerateHatchingData(
						CIsPoint2d(pr_pt.x() , pr_pt.y()) , CIsPoint2d(my_pt.x() , my_pt.y()) , CIsPoint2d(nx_pt.x() , nx_pt.y()) , dMinDist);
					if (oHatchingData.type != _T("NONE"))
					{
						oHatchingData.FillCellMap();	/// 2014.04.16 added by humkyung
						m_pHatchingDataList->push_back(oHatchingData);

						if(m_pAnnoResultList->empty())
						{
							const STRING_T sElevString = this->GetElevationString();

							const double nTextLength = env.m_platform.m_oTextStyle.GetTextLength(sElevString);
							const double nItemCellHeight   = (env.m_platform.m_oTextStyle.height()*1.2) * CSmartDrawAnnoModule::m_pCellMap->scale();
							const double nItemCellLength   = (nTextLength + env.m_platform.m_oTextStyle.height()*0.2) * CSmartDrawAnnoModule::m_pCellMap->scale();
							const double nSearchCellLength = nItemCellHeight * 20;	//! this value comes from experience.

							CCellMap::CellConversionAxis cAxis;
							cAxis.xAxis.Set(1 , 0);
							cAxis.yAxis.Set(0 , 1);

							CCellMap::CellIndex tagOrigin(CSmartDrawAnnoModule::m_pCellMap->PointToIndex(CIsPoint2d(my_pt.x() , my_pt.y())));
							CCellMap::CellIndex cOrigin(tagOrigin);
							cOrigin.x -= nSearchCellLength * 0.5;
							cOrigin.y -= nSearchCellLength * 0.5;

							CCellMap::SIZE_T sizSearch , sizItem;
							sizSearch.width = ceil(nSearchCellLength);
							sizSearch.height= ceil(nSearchCellLength);
							sizItem.width = ceil(nItemCellLength);
							sizItem.height= ceil(nItemCellHeight);
							
							CCellMap::SearchRes res;
							bool bFound = CSmartDrawAnnoModule::m_pCellMap->FindBestLoc(res , cOrigin , tagOrigin , 
								sizSearch , sizItem , cAxis , 0xFF);
							if(bFound)
							{
								res.coords[0] = CIsPoint2d(my_pt.x() , my_pt.y());
								res.coords[1] = res.ptLoc;
								res.coords[2] = res.ptLoc + CIsPoint2d(nTextLength , 0.0);
								if(res.coords[0].DistanceTo(res.coords[2]) < res.coords[0].DistanceTo(res.coords[1]))
								{
									swap(res.coords[1] , res.coords[2]);
								}

								CSmartDrawAnnoModule::m_pCellMap->UpdateRectOf(res.ptLoc , res.ptLoc + CIsPoint2d(nTextLength + env.m_platform.m_oTextStyle.height()*0.2 , env.m_platform.m_oTextStyle.height()*1.2) , CCellMap::TAG);
								m_pAnnoResultList->push_back(res);
							}
						}
					}
				}
			}
		}
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-25
    @function   Write
    @return     int
    @param      OFSTREAM_T& ofile
    @brief
******************************************************************************/
int CSmartDrawAnnoPlatform::Write(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	CaDraw_TagEnv& env = CaDraw_TagEnv::GetInstance();

	/// test code for platform outline
	/*ofile << _T("<PLINE>|");
	for(vector<CIsPoint3d>::iterator itr = m_pPaveList->begin();itr != m_pPaveList->end();++itr)
	{
		if(itr != m_pPaveList->begin()) ofile << _T(",");
		ofile << itr->x() << _T(",") << itr->y();
	}
	ofile << _T("|") << env.m_platform.m_oLeaderProp.level << _T(",") << env.m_platform.m_oLeaderProp.color << _T(",");
	ofile << env.m_platform.m_oLeaderProp.style << _T(",0,") << env.m_platform.m_oLeaderProp.weight << std::endl;
	*//// up to here

	for(list<CCellMap::SearchRes>::iterator itr = m_pAnnoResultList->begin();itr != m_pAnnoResultList->end();++itr)
	{
		ofile << _T("<GROUP>|0,PF,Pat 02") << std::endl;

		ofile << _T("<TEXT>|") << itr->ptLoc.x() << _T(",") << itr->ptLoc.y() + (env.m_platform.m_oTextStyle.height()*0.1) << _T(",0,");
		ofile << env.m_platform.m_oTextStyle.height() << _T(",") << (env.m_platform.m_oTextStyle.height() * env.m_platform.m_oTextStyle.GetWidthFactor()) << _T(",");
		ofile << _T("0,") << this->GetElevationString() << _T(",") << _T("Left Bottom,") << 0.0 << _T("|");
		ofile << env.m_platform.m_oTextProp.level << _T(",") << env.m_platform.m_oTextProp.color << _T(",");
		ofile << env.m_platform.m_oTextStyle << std::endl;

		ofile << _T("<LEADER>|") << itr->coords[0].x() << _T(",") << itr->coords[0].y() << _T(",");
		ofile << itr->coords[1].x() << _T(",") << itr->coords[1].y() << _T(",");
		ofile << itr->coords[2].x() << _T(",") << itr->coords[2].y() << _T("|");
		ofile << env.m_platform.m_oLeaderProp.level << _T(",") << env.m_platform.m_oLeaderProp.color << _T(",");
		ofile << env.m_platform.m_sArrowType << _T(",") << env.m_platform.m_dArrowSize << _T(",") << (env.m_platform.m_dArrowSize/3.0) << _T(",");
		ofile << env.m_platform.m_oLeaderProp.weight << std::endl;

		ofile << _T("</GROUP>") << std::endl;
	}

	if(env.m_generation.IsEnable(_T("Platform Hatch"),NULL))
	{
		for(list<CSmartDrawAnnoPlatform::CHatchingData>::const_iterator itr = m_pHatchingDataList->begin();itr != m_pHatchingDataList->end();++itr)
		{
			itr->Write(ofile);
		}
	}

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-25
    @function   GenerateHatchingData
    @return     int
    @param      const   CIsPoint3d&
    @param      pt
    @brief
******************************************************************************/
CSmartDrawAnnoPlatform::CHatchingData CSmartDrawAnnoPlatform::GenerateHatchingData(const CIsPoint2d& prev , const CIsPoint2d& pt , const CIsPoint2d& next , const double& dMinDist)
{
	CSmartDrawAnnoPlatform::CHatchingData res;

	CaDraw_View* pView = this->GetView();

	CIsVect2d ve1(prev - pt);
	CIsVect2d ve2(next - pt);

	double  x_size = 0.0, y_size = 0.0, p_size = double(CaDraw_TagEnv::GetInstance().m_platform.m_iHatchingSize);

	/// check if hatch point is over view limit
	if ((pt.x() - p_size) < pView->origin().x())
		x_size = (pt.x() - pView->origin().x());
	else if ((pt.x() + p_size) > (pView->origin().x() + pView->GetViewWidth()))
		x_size = (pView->origin().x() + pView->GetViewWidth() - pt.x());
	else
		x_size = p_size;

	if ((pt.y() - p_size) < pView->origin().y())
		y_size = (pt.y() - pView->origin().y());
	else if ((pt.y() + p_size) > (pView->origin().y() + pView->GetViewHeight()))
		y_size = (pView->origin().y() + pView->GetViewHeight() - pt.y());
	else
		y_size = p_size;
	/// up to here

	double r_size = min(min(min(x_size, y_size) , ve1.Length()) , ve2.Length());
	r_size = min(r_size , dMinDist);

	ve1.Normalize();
	ve1 = ve1 * r_size;
	ve2.Normalize();
	ve2 = ve2 * r_size;

	/// 연속된 3점이 counter clockwise인가?
	if (ccw(&prev , &pt , &next) >= 0/*this->IsInnerPoint(chk_pt)*/) 
	{
		res.type = _T("6");
		res.pt[0] = pt;
		res.pt[1] = res.pt[0] + (ve1);
		res.pt[2] = res.pt[1] - ((ve1) * 0.1) + ((ve2) * 0.3);
		res.pt[3] = res.pt[2] - ((ve1) * 0.3) + ((ve2) * 0.35);
		res.pt[4] = res.pt[3] - ((ve1) * 0.2) + ((ve2) * 0.1);
		res.pt[5] = res.pt[0] + (ve2);

		res.insert_point = pt + (ve1*0.4) + (ve2*0.4);
	}
	else 
	{
		/// Outer
		res.type = _T("9");
		res.pt[0] = pt;
		res.pt[1] = res.pt[0] + (ve1);
		res.pt[2] = res.pt[1] - ((ve1) * 0.17) - ((ve2) * 0.4);
		res.pt[3] = res.pt[2] - ((ve1) * 0.54) - ((ve2) * 0.16);
		res.pt[4] = res.pt[3] - ((ve1) * 0.47) + ((ve2) * 0.12);
		res.pt[5] = res.pt[4] - ((ve1) * 0.32) + ((ve2) * 0.46);
		res.pt[6] = res.pt[5] - ((ve1) * 0.1) + ((ve2) * 0.4);
		res.pt[7] = res.pt[6] + ((ve1) * 0.2) + ((ve2) * 0.38);
		res.pt[8] = res.pt[0] + (ve2);

		res.insert_point = pt - (ve1*0.15) - (ve2*0.15);
	}

	return res;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-09
    @function   PassedThrough
    @return     bool
    @param      const CIsPoint3d& start
	@param		const CIsPoint3d& end
    @brief
******************************************************************************/
bool CSmartDrawAnnoPlatform::PassedThrough(const CIsPoint3d& start , const CIsPoint3d& end) const
{
	bool res = false;
	static const double tol = 0.001;

	if (start.m_z <= end.m_z) 
	{
		res = (((m_dElevation + tol) >= start.m_z) && ((m_dElevation - tol) <= end.m_z)) ? true : false; 
	}
	else 
	{
		res = (((m_dElevation + tol) >= end.m_z) && ((m_dElevation - tol) <= start.m_z)) ? true : false; 
	}

	return res;
}

/**
	@brief	옵션으로 주어진 point가 platform 안에 있는지 검사한다.

	@author	HumKyung.BAEK

	@date	2010.08.23
*/
bool CSmartDrawAnnoPlatform::IsInnerPoint(const CIsPoint2d& pt) const
{
	double i_TOL = 0.0;

	int     i;
	int     n=0;        // point index; i1 = i-1 mod n 
	double  x=0;        // x intersection of e with ray 
	int     Rcross = 0; // number of right edge/ray crossings
	int     Lcross = 0; // number of left edge/ray crossings

	CIsPoint3d  P[100];

	// This is done for pedogical clarity.
	for (vector<CIsPoint3d>::iterator itr = m_pPaveList->begin(); itr != m_pPaveList->end(); ++itr)
	{
		if (n < 99)  
		{
			P[n].Set(itr->x() - pt.x() , itr->y() - pt.y() , 0.0);
			n++;
		}		
	}
	// 처음 Point로 마지막을 만듦
	P[n] = P[0];
	n++;

	int i1=0;
	for(i = 0;i < n;i++) 
	{
		i1 = ( i + n - 1 ) % n;

		if( ( P[i].m_y  > 0. ) != ( P[i1].m_y > 0. ) ) 
		{
			x = (P[i].m_x * (double)P[i1].m_y - P[i1].m_x * (double)P[i].m_y)/(double)(P[i1].m_y - P[i].m_y);
			if (x >= (0 + i_TOL)) Rcross++;
		}

		if ( ( P[i].m_y < 0 ) != ( P[i1].m_y < 0 ) ) 
		{ 
			x = (P[i].m_x * P[i1].m_y - P[i1].m_x * P[i].m_y)/(double)(P[i1].m_y - P[i].m_y);
			if (x <= (0 + i_TOL)) Lcross++;
		}
	}

	//* q on the edge if left and right cross are not the same parity.
	if((Rcross%2 ) != (Lcross%2)) return true;

	//* q inside iff an odd number of crossings.
	if((Rcross % 2) == 1) return true;
	else		      return false;

	return false;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-26
    @function   getPolygonArea
    @return     double
    @brief
******************************************************************************/
double CSmartDrawAnnoPlatform::getPolygonArea()
{
	double area = 0.0;
	
	int p = 0 , q = 0;
	const int iCount = m_pPaveList->size();
	for(p = iCount - 1,q = 0;q < iCount;p = q++)
	{
		area += (*m_pPaveList)[p].x()*(*m_pPaveList)[q].y() - (*m_pPaveList)[p].y()*(*m_pPaveList)[q].x();
	}

	return area;
}


/******************************************************************************
    @author     humkyung
    @date       2012-02-25
    @function   Parse
    @return     int
    @brief		parse platform item
******************************************************************************/
int CSmartDrawAnnoPlatform::Parse()
{
	CaDraw_Platform* pPlatform = static_cast<CaDraw_Platform*>(m_pDrawEntity);
	CaDraw_View* pDrawView = pPlatform->GetView();

	m_pPaveList->clear();
	int iPointCount = pPlatform->GetPointCount();
	for(int i = 0;i < iPointCount;++i)
	{
		CIsPoint3d pt3d = pPlatform->point(i);
		if(DBL_MAX == m_dElevation) m_dElevation = pt3d.z();

		CIsPoint3d pt = pDrawView->ModelCoord2ViewCoord(pt3d);
		vector<CIsPoint3d>::iterator where = find(m_pPaveList->begin() , m_pPaveList->end() , pt);
		if(where == m_pPaveList->end())
		{
			m_pPaveList->push_back(pt);
		}
	}

	/// set ccw
	if(m_pPaveList->size() > 2)
	{
		double area = this->getPolygonArea();
		if(area < 0.0)
		{
			::reverse(m_pPaveList->begin() , m_pPaveList->end());
		}
	}
	/// up to here

	///m_sViewName = pDrawView->name();

	//m_sType = pDrawCableTray->GetTypeString();
	//m_dElevation = pDrawCableTray->startPoint().z();
	//m_sName = pDrawCableTray->name();

	//const CaDraw_CableTray::ELEMENT_E eType = pDrawCableTray->GetType();
	//
	//m_sDepartmentType = pDrawCableTray->GetCategoryString();

	//m_ptStart = pDrawView->ModelCoord2ViewCoord(pDrawCableTray->startPoint());
	//m_ptEnd   = pDrawView->ModelCoord2ViewCoord(pDrawCableTray->endPoint());
	//
	//m_nWidth = pDrawCableTray->width1();
	//m_nHeight= pDrawCableTray->height();

	//m_lengthDir = pDrawCableTray->GetLengthDir();
	//m_widthDir  = pDrawCableTray->GetWidthDir();
	//m_heightDir = pDrawCableTray->GetHeightDir();
	//
	//if(CaDraw_CableTray::STRAIGHT_T == eType)
	//{
	//	///double   m_nWidth, m_nWidth2, m_nHeight, m_nItemAng;
	//}
	//else if(CaDraw_CableTray::ELBOW_T == eType)
	//{
	//	m_sSubType = pDrawCableTray->GetSubTypeString();
	//	m_nItemAng = pDrawCableTray->GetAngle();
	//	m_ptMid = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pDrawCableTray->Get3rdPoint());
	//}
	//else if(CaDraw_CableTray::TEE_T == eType)
	//{
	//	m_sSubType = pDrawCableTray->GetSubTypeString();
	//	m_ptMid = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pDrawCableTray->Get3rdPoint());
	//	m_nWidth2 = pDrawCableTray->width2();
	//}
	//else if(CaDraw_CableTray::REDUCE_T == eType)
	//{
	//	m_sSubType = pDrawCableTray->GetSubTypeString();
	//	m_ptMid = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(pDrawCableTray->Get3rdPoint());
	//	m_nWidth2 = pDrawCableTray->width2();

	//	vector<CIsPoint3d> oEdgeList;
	//	pDrawCableTray->GetEdgeList(oEdgeList);
	//	for(vector<CIsPoint3d>::iterator itr = oEdgeList.begin();itr != oEdgeList.end();++itr)
	//	{
	//		CIsPoint3d pt = m_pDrawEntity->GetView()->ModelCoord2ViewCoord(*itr);
	//		m_oReducePntList.push_back(pt);
	//	}
	//}

	//m_ptCenter.SetX((m_ptStart.x() + m_ptEnd.x())*0.5);
	//m_ptCenter.SetY((m_ptStart.y() + m_ptEnd.y())*0.5);
	//m_dLength = m_ptStart.DistanceTo(m_ptEnd);

	//m_oConnPointList.clear();

	//m_oConnPointList.push_back(m_ptStart);
	//m_oConnPointList.push_back(m_ptEnd);

	//// 1:x_seg, 2:y_seg, 3:z_seg, 4:non_ortho
	//s_type = 4;
	//if ((0.1 > fabs(m_ptStart.y() - m_ptEnd.y())) && (0.1 < fabs(m_ptStart.x() - m_ptEnd.y()))) s_type = 1;
	//if ((0.1 > fabs(m_ptStart.x() - m_ptEnd.x())) && (0.1 < fabs(m_ptStart.y() - m_ptEnd.y()))) s_type = 2;
	//if ((0.1 > fabs(m_ptStart.x() - m_ptEnd.x())) && (0.1 > fabs(m_ptStart.y() - m_ptEnd.y()))) s_type = 3;	

	return ERROR_SUCCESS;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-25
    @function   elevation
    @return     double
    @brief
******************************************************************************/
double CSmartDrawAnnoPlatform::elevation() const
{
	return m_dElevation;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-25
    @function   GetElevationString
    @return     STRING
    @brief
******************************************************************************/
STRING_T CSmartDrawAnnoPlatform::GetElevationString() const
{
	const int elev = int(SAFE_ROUND(m_dElevation , 0));
	
	OSTRINGSTREAM_T oss;
	oss << _T("EL. ") << elev;
	return oss.str();
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-25
    @function   CreateTagObjects
    @return     CaDraw_TagItem*
    @param      list<CaDraw_TagItem*>&  TagItemList
    @param      CSmartDrawAnnoView   *pAnnoView
    @brief
******************************************************************************/
CaDraw_TagItem* CSmartDrawAnnoPlatform::CreateTagObjects(list<CaDraw_TagItem*>& TagItemList , CSmartDrawAnnoView *pAnnoView)
{
	TagItemList.clear();
	return NULL;
}

/******************************************************************************
    @author     humkyung
    @date       2012-02-25
    @function   OnCreateTagObject
    @return     CaDraw_TagItem*
    @param      CSmartDrawAnnoView   *pAnnoView
    @param      const                   CIsPoint3d&
    @param      ptOrigin                const
    @param      CIsVect3d&              vec
    @param      const                   DITEM_E&
    @param      enmType
    @brief
******************************************************************************/
CaDraw_TagItem* CSmartDrawAnnoPlatform::OnCreateTagObject(CSmartDrawAnnoView *pAnnoView , const CIsPoint3d& ptOrigin,const CIsVect3d& vec,const DITEM_E& enmType)
{
	CaDraw_TagItem* pTagItem=NULL;

	return pTagItem;
}