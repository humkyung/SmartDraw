// NozzleChart.cpp: implementation of the CNozzleChart class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>
#include <math.h>
#include <assert.h>
#include <Tokenizer.h>
#include "SmartDrawAnnoModule.h"
#include "SmartDrawAnnoNozzle.h"
#include "NozzleChart.h"

#include <algorithm>
#include <fstream>
using namespace std;

int CNozzleChart::m_nSizeUnit = INCH; /// default
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/**	
	@brief
*/
CNozzleChart::CNozzleChart() : m_iRowCount(0) , m_bSkipEqpHasNoNozzle(false)
{
	m_sJustify = _T("UpperRight");

	m_bLoadBorderIni = false;
	m_bGridNo  = false;
	m_nRows    = 0;
		
	m_dX = m_dY = 0.;
	STRCPY_T(m_szColor,_T("black"));

	m_dWidth   = 0.;
	m_varLabel.nColumn = 8;
	m_varLabel.nSequence[0] = 1;
	m_varLabel.nSequence[1] = 2;
	m_varLabel.nSequence[2] = 3;
	m_varLabel.nSequence[3] = 4;
	m_varLabel.nSequence[4] = 5;
	m_varLabel.nSequence[5] = 6;
	m_varLabel.nSequence[6] = 7;
	m_varLabel.nSequence[7] = 8;
	m_varLabel.dWidth[0]	= 10;
	m_varLabel.dWidth[1]	= 10;
	m_varLabel.dWidth[2]	= 10;
	m_varLabel.dWidth[3]	= 10;
	m_varLabel.dWidth[4]	= 10;
	m_varLabel.dWidth[5]	= 10;
	m_varLabel.dWidth[6]	= 10;
	m_varLabel.dWidth[7]	= 10;
	STRCPY_T(m_varLabel.szLabel[0],_T("1"));
	STRCPY_T(m_varLabel.szLabel[1],_T("2"));
	STRCPY_T(m_varLabel.szLabel[2],_T("3"));
	STRCPY_T(m_varLabel.szLabel[3],_T("4"));
	STRCPY_T(m_varLabel.szLabel[4],_T("5"));
	STRCPY_T(m_varLabel.szLabel[5],_T("6"));
	STRCPY_T(m_varLabel.szLabel[6],_T("7"));
	STRCPY_T(m_varLabel.szLabel[7],_T("8"));
	
	ZeroMemory(m_varLabel.szEqpLabel , sizeof(TCHAR) * 16);
	m_varLabel.dEqpWidth = 0.f;

	ZeroMemory(m_szUnknownItem , sizeof(TCHAR) * 16);
	m_nType = TYPE_1;

	/// create a collection to contain equipment pointer
	m_pNozzleChartEntry = new vector<CaDraw_TagEqui*>();
}

CNozzleChart::~CNozzleChart()
{
	try
	{
		SAFE_DELETE(m_pNozzleChartEntry);
	}
	catch(...)
	{
	}
}

/**	
	@brief	load nozzle chart setting
	@author	humkyung
	@param	pszIniFilePath	a parameter of type const char*
	@return	void	
*/
void CNozzleChart::LoadIni(LPCTSTR pszIniFilePath)
{
	assert(pszIniFilePath && "pszIniFilePath is NULL");

	static const STRING_T sApp(_T("Nozzle Chart"));
	if(pszIniFilePath)
	{	
		STRING_T sBorderCfgFilePath(pszIniFilePath);
		if(GetPrivateProfileString(_T("GENERAL") , _T("Border File") , _T("") , m_szBuf , MAX_PATH , pszIniFilePath))
		{
			STRING_T sBorderFileName(m_szBuf);

			int at = sBorderCfgFilePath.rfind(_T("\\"));
			sBorderCfgFilePath = sBorderCfgFilePath.substr(0 , at);
			at = sBorderCfgFilePath.rfind(_T("\\"));
			sBorderCfgFilePath = sBorderCfgFilePath.substr(0 , at);
			sBorderCfgFilePath += _T("\\Border\\") + sBorderFileName + _T(".cfg");
		}
		
		if(GetPrivateProfileString(sApp.c_str() , _T("Eqp_RowHeight") , _T("15") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_dEqpRowHeight = ATOF_T(m_szBuf);
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Eqp_TextStyle") , _T("") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_oEqpTextStyle.Parse(m_szBuf , pszIniFilePath);
		}

		if(GetPrivateProfileString(sApp.c_str() , _T("EqpLabel_Level") , _T("") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_oEqpLabelProp.level = m_szBuf;
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("EqpLabel_Color") , _T("") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_oEqpLabelProp.color = m_szBuf;
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("EqpLabel_Style") , _T("") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_oEqpLabelProp.style = m_szBuf;
		}

		if(GetPrivateProfileString(sApp.c_str() , _T("Nozzle_RowHeight") , _T("10") ,m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_dNozzRowHeight = ATOI_T(m_szBuf);
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Nozzle_TextStyle") , _T("") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_oNozzleTextStyle.Parse(m_szBuf , pszIniFilePath);
		}

		if(GetPrivateProfileString(sApp.c_str() , _T("NozzleLabel_Level") , _T("") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_oNozzLabelProp.level = m_szBuf;
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("NozzleLabel_Color") , _T("") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_oNozzLabelProp.color = m_szBuf;
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("NozzleLabel_Style") , _T("") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_oNozzLabelProp.style = m_szBuf;
		}

		vector<STRING_T> oResult;
		if(GetPrivateProfileString(sApp.c_str() , _T("Width") , _T("") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , m_szBuf , CIsComma());
			for(int i = 0;i < int(oResult.size());++i)
			{
				m_varLabel.dWidth[i] = ATOF_T(oResult[i].c_str());
				m_dWidth += m_varLabel.dWidth[i];
			}
		}
		
		if(GetPrivateProfileString(sApp.c_str() , _T("Origin") , _T("") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , m_szBuf , CIsComma());
			if(3 == oResult.size())
			{
				m_sJustify = oResult[0];
				m_dX = ATOF_T(oResult[1].c_str());
				m_dY = ATOF_T(oResult[2].c_str());
			}
		}
		
		if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Level") , _T("") ,m_szBuf,MAX_PATH,sBorderCfgFilePath.c_str()))
		{
			m_oLeaderProp.level = m_szBuf;
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Color") , _T("") ,m_szBuf,MAX_PATH,sBorderCfgFilePath.c_str()))
		{
			m_oLeaderProp.color = m_szBuf;
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Style") , _T("") ,m_szBuf,MAX_PATH,sBorderCfgFilePath.c_str()))
		{
			m_oLeaderProp.style = m_szBuf;
		}
		if(GetPrivateProfileString(sApp.c_str() , _T("Leader_Weight") , _T("") ,m_szBuf,MAX_PATH,sBorderCfgFilePath.c_str()))
		{
			m_oLeaderProp.weight = m_szBuf;
		}
		
		if(GetPrivateProfileString(sApp.c_str() , _T("EqupGridNo") , _T("") , m_szBuf,MAX_PATH,pszIniFilePath))
		{
			m_bGridNo = (0 == STRICMP_T(m_szBuf , _T("ON")));
		}
		
		if(GetPrivateProfileString(sApp.c_str() , _T("Label") , _T("") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , m_szBuf , CIsComma());
			for(int i = 0;i < int(oResult.size());++i)
			{
				STRCPY_T(m_varLabel.szLabel[i] , oResult[i].c_str());
			}
			m_varLabel.nColumn = oResult.size();
		}
		
		if(GetPrivateProfileString(sApp.c_str() , _T("Column") , _T("") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			CTokenizer<CIsComma>::Tokenize(oResult , m_szBuf , CIsComma());
			for(int i = 0;i < int(oResult.size());++i)
			{
				m_varLabel.nSequence[i] = ATOI_T(oResult[i].c_str());
			}		
		}
		
		if(GetPrivateProfileString(sApp.c_str() , _T("SizeUnit") , _T("Inch Feet") ,m_szBuf,MAX_PATH,sBorderCfgFilePath.c_str()))
		{
			if(0 == STRCMP_T(m_szBuf,_T("Inch Feet")))
				CNozzleChart::m_nSizeUnit = INCH;
			else	CNozzleChart::m_nSizeUnit = METRIC;
		}
		
		//////////////////////////////////////////////////////////////////////////
		/// 2005.11.15
		ZeroMemory(m_szUnknownItem , sizeof(TCHAR) * 16);
		if(GetPrivateProfileString(sApp.c_str() , _T("Unknown_Item") , _T("-") , m_szBuf , MAX_PATH , pszIniFilePath))
		{
			STRNCPY_T(m_szUnknownItem , m_szBuf , sizeof(TCHAR)*16);
		}

		//////////////////////////////////////////////////////////////////////////
		/// 2005.11.30
		if(GetPrivateProfileString(sApp.c_str() , _T("ChartType") , _T("") , m_szBuf , MAX_PATH , pszIniFilePath))
		{
			if(0 == STRCMP_T(m_szBuf,_T("Type-1")))
			{
				m_nType = TYPE_1;
			}
			else
			{
				m_nType = TYPE_2;
			}
		}
		if(TYPE_2 == m_nType)
		{
			if(GetPrivateProfileString(sApp.c_str() , _T("EqpLabel") , _T("") , m_szBuf , MAX_PATH , pszIniFilePath))
			{
				STRNCPY_T(m_varLabel.szEqpLabel,m_szBuf,sizeof(TCHAR)*16);
			}
			if(GetPrivateProfileString(sApp.c_str() , _T("EqpWidth") , _T("") , m_szBuf , MAX_PATH , pszIniFilePath))
			{
				m_varLabel.dEqpWidth = ATOF_T(m_szBuf);
				m_dWidth += m_varLabel.dEqpWidth;
			}
		}
		m_iRowCount = GetPrivateProfileInt(sApp.c_str() , _T("RowCount") , 0 , sBorderCfgFilePath.c_str());

		/// 2014.12.31 added by humkyung
		if(GetPrivateProfileString(sApp.c_str() , _T("Skip Eqp Has No Nozzle") , _T("False") , m_szBuf , MAX_PATH , sBorderCfgFilePath.c_str()))
		{
			m_bSkipEqpHasNoNozzle = (STRING_T(_T("True")) == STRING_T(m_szBuf));
		}
		/// up to here
	}
}

/**	
	@brief	The CNozzleChart::LoadBorderIni function
	@author	humkyung
	@param	pFilePath	a parameter of type const char*
	@return	void	
*/
void CNozzleChart::LoadBorderIni(LPCTSTR pFilePath)
{
	assert(pFilePath && "pFilePath is NULL");

	if(pFilePath)
	{			
		vector<STRING_T> oResult;
		if(GetPrivateProfileString(_T("Grid No Data"),_T("X-Axis"),_T(""),m_szBuf,MAX_PATH,pFilePath))
		{
			int nStrLen=STRLEN_T(m_szBuf);
			for(int i=0;i < nStrLen;i++) m_szBuf[i] = toupper(m_szBuf[i]);
			CTokenizer<CIsComma>::Tokenize(oResult , m_szBuf , CIsComma());
			
			m_varGridNo.nTypeX = (0 == STRCMP_T(oResult[0].c_str() , _T("NUMERIC") )) ? 0 : 1;
			m_varGridNo.nStepX = ATOF_T(oResult[1].c_str());
		}
		
		if(GetPrivateProfileString(_T("Grid No Data") , _T("Y-Axis") , _T("") ,m_szBuf,MAX_PATH,pFilePath))
		{
			int nStrLen=STRLEN_T(m_szBuf);
			for(int i=0;i < nStrLen;i++) m_szBuf[i] = toupper(m_szBuf[i]);
			CTokenizer<CIsComma>::Tokenize(oResult , m_szBuf , CIsComma());
			
			m_varGridNo.nTypeY = (0 == STRCMP_T(oResult[0].c_str() , _T("NUMERIC"))) ? 0 : 1;
			m_varGridNo.nStepY = ATOF_T(oResult[1].c_str());
		}
		
		if(GetPrivateProfileString(_T("Grid No Data"),_T("Start_Loc"),_T(""),m_szBuf,MAX_PATH,pFilePath))
		{
			int nStrLen=STRLEN_T(m_szBuf);
			for(int i=0;i < nStrLen;i++) m_szBuf[i] = toupper(m_szBuf[i]);
			CTokenizer<CIsComma>::Tokenize(oResult , m_szBuf , CIsComma());
			
			m_varGridNo.nX = ATOF_T(oResult[1].c_str());
			m_varGridNo.nY = ATOF_T(oResult[2].c_str());
			
			if(0 == STRICMP_T(oResult[0].c_str() , _T("UPPERRIGHT")))
			{
				m_varGridNo.nStepX = -m_varGridNo.nStepX;
			}
			else if(0 == STRICMP_T(oResult[0].c_str() , _T("LOWERLEFT")))
			{
				m_varGridNo.nStepY = -m_varGridNo.nStepY;
			}
			else if(0 == STRICMP_T(oResult[0].c_str() , _T("LOWERRIGHT")))
			{
				m_varGridNo.nStepX = -m_varGridNo.nStepX;
				m_varGridNo.nStepY = -m_varGridNo.nStepY;
			}
		}

		m_bLoadBorderIni = true;
	}
}

/**	
	@brief	The CNozzleChart::Load function
	@param	pszFilePath	a parameter of type const char*
	@remarks	;2004.06.24 - equipment format is changed to support 'SPECIAL' equipment.
	@return	void	
*/
void CNozzleChart::Load(const char* pszFilePath)
{
	assert(pszFilePath && "pszFilePath is NULL");

	if(pszFilePath){}
}

static bool __SortByEquiName(CaDraw_TagEqui* lhs , CaDraw_TagEqui* rhs)
{
	return (STRCMP_T(lhs->equi()->name() , rhs->equi()->name()) < 0);
}

/**	
	@brief	The CNozzleChart::Write function
	@param	pszFilePath	a parameter of type const char*
	@return	void	
*/
void CNozzleChart::Write(OFSTREAM_T& ofile,const double& dDrawingScale)
{
	if(ofile.is_open())
	{
		//////////////////////////////////////////////////////////////////////////
		/// calculate char height and rows.
		m_nRows   = m_pNozzleChartEntry->size();
		m_nHeight = m_pNozzleChartEntry->size()*m_dEqpRowHeight;
		for(vector<CaDraw_TagEqui*>::const_iterator itr = m_pNozzleChartEntry->begin();itr != m_pNozzleChartEntry->end();++itr)
		{
			list<CaDraw_TagNozzle*> oTagNozzleList;
			(*itr)->GetTagNozzleList( oTagNozzleList );
			m_nHeight += oTagNozzleList.size()*m_dNozzRowHeight;
			m_nRows   += oTagNozzleList.size();
		}

		if(0 == STRICMP_T(m_sJustify.c_str() , _T("UPPERRIGHT")))
		{
			m_dX -= m_dWidth;
		}
		else if(0 == STRICMP_T(m_sJustify.c_str() , _T("LOWERLEFT")))
		{
			m_dY += (m_nHeight + m_dNozzRowHeight);
		}
		else if(0 == STRICMP_T(m_sJustify.c_str() , _T("LOWERRIGHT")))
		{
			m_dX -= m_dWidth;
			m_dY += (m_nHeight + m_dNozzRowHeight);
		}
		
		ofile << _T("# Nozzle Chart : ") << __LINE__ << _T(" at ") << __FILE__ << std::endl;
		/// up to here
		stable_sort(m_pNozzleChartEntry->begin() , m_pNozzleChartEntry->end() , __SortByEquiName);
		if(0 == m_iRowCount)
		{
			ofile << _T("<LINE>|");
			ofile << m_dX*dDrawingScale << _T(",") << m_dY*dDrawingScale << _T(",0,");
			ofile << (m_dX + GetWidth())*dDrawingScale << _T(",") << m_dY*dDrawingScale << _T(",0|");
			ofile << m_oLeaderProp.level << _T(",") << m_oLeaderProp.color << _T(",");
			ofile << m_oLeaderProp.style << _T(",0,") << m_oLeaderProp.weight << std::endl;

			this->WriteLabel(ofile , m_dX , dDrawingScale);
			double x = m_dX*dDrawingScale;
			double y = (m_dY - m_dNozzRowHeight)*dDrawingScale;
			for(vector<CaDraw_TagEqui*>::const_iterator itr = m_pNozzleChartEntry->begin();itr != m_pNozzleChartEntry->end();++itr)
			{
				this->WriteEqui(ofile , x , y , *itr , dDrawingScale);
			}

			//////////////////////////////////////////////////////////////////////////
			/// draw left total vertical line
			ofile << _T("<LINE>|");
			ofile << m_dX*dDrawingScale << _T(",") << m_dY*dDrawingScale << _T(",0,") << m_dX*dDrawingScale << _T(",") << y << _T(",0|");
			ofile << m_oLeaderProp.level << _T(",") << m_oLeaderProp.color << _T(",");
			ofile << m_oLeaderProp.style << _T(",0,") << m_oLeaderProp.weight << std::endl;

			ofile << _T("<LINE>|");
			ofile << (m_dX + GetWidth())*dDrawingScale << _T(",") << m_dY*dDrawingScale << _T(",0,") << (m_dX + GetWidth())*dDrawingScale << _T(",") << y << _T(",0|");
			ofile << m_oLeaderProp.level << _T(",") << m_oLeaderProp.color << _T(",");
			ofile << m_oLeaderProp.style << _T(",0,") << m_oLeaderProp.weight << std::endl;

			ofile << _T("<LINE>|");
			ofile << m_dX*dDrawingScale << _T(",") << y << _T(",0,") << (m_dX + GetWidth())*dDrawingScale << _T(",") << y << _T(",0|");
			ofile << m_oLeaderProp.level << _T(",") << m_oLeaderProp.color << _T(",");
			ofile << m_oLeaderProp.style << _T(",0,") << m_oLeaderProp.weight << std::endl;
		}
		else if(m_iRowCount > 0)
		{
			size_t nItemCount = 0 , nTables = 0;
			for(vector<CaDraw_TagEqui*>::const_iterator itr = m_pNozzleChartEntry->begin();itr != m_pNozzleChartEntry->end();++itr)
			{
				++nItemCount;
				list<CaDraw_TagNozzle*> oTagNozzleList;
				(*itr)->GetTagNozzleList( oTagNozzleList );
				nItemCount += oTagNozzleList.size();
				if(int(nItemCount) > int(m_iRowCount))
				{
					++nTables;
					nItemCount = 0;
				}
			}
			if( nItemCount > 0 ) ++nTables; /// nItemCount의 갯수가 0보다 크면 또 하나의 테이블이 필요하므로....

			double x = m_dX - (nTables - 1)*m_varLabel.GetTotalWidth();
			double y = m_dY;
			size_t loc = 0;
			for(int i = 0;i < int(nTables);++i)
			{
				WriteTable( ofile , x , y , loc , dDrawingScale);
				x += m_varLabel.GetTotalWidth();
				y  = m_dY;
			}
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-26
    @function   WriteEqui
    @return     void
    @param      OFSTREAM_T&     file
    @param      double &x
    @param      double &y
    @param      CaDraw_TagEqui* pTagEqui
    @brief
******************************************************************************/
void CNozzleChart::WriteEqui(OFSTREAM_T& ofile , double &x , double &y, CaDraw_TagEqui* pTagEqui,const double& dDrawingScale)
{
	assert(pTagEqui && "pTagEqui is NULL");

	if(pTagEqui)
	{
		TCHAR szBuf[MAX_PATH] = {'\0',};

		list<CaDraw_TagNozzle*> oTagNozzleList;
		pTagEqui->GetTagNozzleList( oTagNozzleList );
		if((true == m_bSkipEqpHasNoNozzle) && (0 == oTagNozzleList.size())) return;

		//////////////////////////////////////////////////////////////////////////
		/// draw upper horizontal line
		ofile << _T("<LINE>|");
		ofile << x << _T(",") << y << _T(",0,") << (x + this->GetWidth()*dDrawingScale) << _T(",") << y << _T(",0|");
		ofile << m_oLeaderProp << std::endl;
		
		if(TYPE_1 == this->GetType())
		{
			const double dWidth = this->GetWidth()*dDrawingScale;
			const double dRowHeight = this->GetEquiRowHeight()*dDrawingScale;

			if(this->m_bLoadBorderIni && this->m_bGridNo)
			{
				ofile << _T("<TEXT>|");
				ofile << (x + dWidth*0.5) << _T(",") << (y - dRowHeight*0.5) << _T(",0,");
				ofile << m_oEqpTextStyle.height()*dDrawingScale << _T(",") << m_oEqpTextStyle.width()*dDrawingScale << _T(",0,");
				SPRINTF_T(szBuf , _T("%s GRID NO.(%s-%s)") , pTagEqui->equi()->name() , pTagEqui->m_szGridNo[0] , pTagEqui->m_szGridNo[1]);
				ofile << szBuf << _T(",Cener Center,0|");
				ofile << m_oEqpLabelProp.level << _T(",") << m_oEqpLabelProp.color << _T(",");
				ofile << m_oEqpTextStyle << std::endl;
			}
			else
			{
				ofile << _T("<TEXT>|");
				ofile << (x + dWidth*0.5) << _T(",") << (y - dRowHeight*0.5) << _T(",0,");
				ofile << m_oEqpTextStyle.height()*dDrawingScale << _T(",") << m_oEqpTextStyle.width()*dDrawingScale << _T(",0,");
				ofile << pTagEqui->equi()->name() << _T(",Center Center,0|");
				ofile << m_oEqpLabelProp.level << _T(",") << m_oEqpLabelProp.color << _T(",");
				ofile << m_oEqpTextStyle << std::endl;
			}
			y -= dRowHeight;

			double nTop = y;
			for(list<CaDraw_TagNozzle*>::const_iterator itr = oTagNozzleList.begin();itr != oTagNozzleList.end();++itr)
			{
				//////////////////////////////////////////////////////////////////////////
				ofile << _T("<LINE>|");
				ofile << x << _T(",") << y << _T(",0,") << (x + this->GetWidth()*dDrawingScale) << _T(",") << y << _T(",0|");
				ofile << m_oLeaderProp << std::endl;
				
				this->WriteNozzle(ofile , x , y , pTagEqui->equi()->origin().x() , pTagEqui->equi()->origin().y() , pTagEqui->equi()->origin().z() , m_nType , *itr , dDrawingScale);
			}
			double nBottom = y;

			double nTotalWidth = 0.f;
			//////////////////////////////////////////////////////////////////////////
			ofile << _T("<LINE>|");
			ofile << (x + nTotalWidth) << _T(",") << nTop << _T(",0,") << (x + nTotalWidth) << _T(",") << nBottom << _T(",0|");
			ofile << m_oLeaderProp << std::endl;

			for(int i = 0;i < 8;++i)
			{
				const int n = this->m_varLabel.nSequence[i] - 1;
				const double dLabelWidth = this->m_varLabel.dWidth[n];

				nTotalWidth += dLabelWidth*dDrawingScale;
				if(0. != dLabelWidth)
				{
					ofile << _T("<LINE>|");
					ofile << (x + nTotalWidth) << _T(",") << nTop << _T(",0,") << (x + nTotalWidth) << _T(",") << nBottom << _T(",0|");
					ofile << m_oLeaderProp << std::endl;
				}
			}
		}
		else
		{
			const double dWidth = this->m_varLabel.dEqpWidth*dDrawingScale;
			const double dHeight= oTagNozzleList.size()*this->GetNozzRowHeight()*dDrawingScale;
			if(this->m_bLoadBorderIni && this->m_bGridNo)
			{
				ofile << _T("<TEXT>|");
				ofile << (x + dWidth*0.5) << _T(",") << (y - dHeight*0.5) << _T(",0,");
				ofile << m_oEqpTextStyle.height()*dDrawingScale << _T(",") << m_oEqpTextStyle.width()*dDrawingScale << _T(",0,");
				SPRINTF_T(szBuf , _T("%s GRID NO.(%s-%s)") , pTagEqui->equi()->name() , pTagEqui->m_szGridNo[0] , pTagEqui->m_szGridNo[1]);
				ofile << szBuf << _T(",Center Center,0|");
				ofile << m_oEqpLabelProp.level << _T(",") << m_oEqpLabelProp.color << _T(",");
				ofile << m_oEqpTextStyle << std::endl;
			}
			else
			{
				ofile << _T("<TEXT>|");
				ofile << (x + dWidth*0.5) << _T(",") << y - dHeight*0.5 << _T(",0,");
				ofile << m_oEqpTextStyle.height()*dDrawingScale << _T(",") << m_oEqpTextStyle.width()*dDrawingScale << _T(",0,");
				ofile << pTagEqui->equi()->name() << _T(",Center Center,0|");
				ofile << m_oEqpLabelProp.level << _T(",") << m_oEqpLabelProp.color << _T(",");
				ofile << m_oEqpTextStyle << std::endl;
			}
			//////////////////////////////////////////////////////////////////////////
			ofile << _T("<LINE>|");
			ofile << (x + dWidth) << _T(",") << y << _T(",0,")  << (x + dWidth) << _T(",") << (y - dHeight) << _T(",0|");
			ofile << m_oLeaderProp << std::endl;

			double tempx = x + this->m_varLabel.dEqpWidth*dDrawingScale;
			double nTop = y;
			for(list<CaDraw_TagNozzle*>::const_iterator itr = oTagNozzleList.begin();itr != oTagNozzleList.end();++itr)
			{
				//////////////////////////////////////////////////////////////////////////
				ofile << _T("<LINE>|");
				ofile << (x + this->m_varLabel.dEqpWidth*dDrawingScale) << _T(",") << y << _T(",0,") << (x + this->GetWidth()*dDrawingScale) << _T(",") << y << _T(",0|");
				ofile << m_oLeaderProp << std::endl;

				this->WriteNozzle(ofile , tempx , y , pTagEqui->equi()->origin().x() , pTagEqui->equi()->origin().y() , pTagEqui->equi()->origin().z() , m_nType , *itr , dDrawingScale);
			}
			double nBottom = y;

			double nTotalWidth = 0.f;
			for(int i = 0;i < 8;i++)
			{
				const int n=this->m_varLabel.nSequence[i] - 1;
				const double dLabelWidth = this->m_varLabel.dWidth[n];

				nTotalWidth += dLabelWidth*dDrawingScale;
				if(0. != dLabelWidth)
				{
					ofile << _T("<LINE>|");
					ofile << (tempx + nTotalWidth) << _T(",") << nTop << _T(",0,") << (tempx + nTotalWidth) << _T(",") << nBottom << _T(",0|");
					ofile << m_oLeaderProp << std::endl;
				}
			}
		}
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-28
    @class      CNozzleChart
    @function   GetSize
    @return     void
    @param      LPCTSTR             pSize
    @param      CaDraw_TagNozzle*   pTagNozzle
    @brief
******************************************************************************/
void CNozzleChart::GetSize(LPTSTR pSize , CaDraw_TagNozzle* pTagNozzle)
{
	assert(pSize && "pSize is NULL");
	const static CNozzleChart::METRIC_INCH_T table[]=
	{
		{0,7,_T("1/8\"")},	{7,9,_T("1/4\"")},	{9,12,_T("3/8\"")},	{12,17,_T("1/2\"")},
		{17,32,_T("3/4\"")},{22,29,_T("1\"")},	{29,36,_T("1.1/4\"")},{36,45,_T("1.1/2\"")},
		{45,57,_T("2\"")},	{57,72,_T("2.1/2\"")},{72,85,_T("3\"")},{85,95,_T("3.1/2\"")},
		{95,112,_T("4\"")},	{112,142,_T("5\"")},{142,175,_T("6\"")},{175,225,_T("8\"")},
		{225,275,_T("10\"")},{275,325,_T("12\"")},{325,375,_T("14\"")},{375,425,_T("16\"")},
		{425,475,_T("18\"")},{475,525,_T("20\"")},{525,575,_T("22\"")},{575,625,_T("24\"")},
		{625,675,_T("26\"")},{675,775,_T("28\"")}
	};

	if(pSize)
	{
		CSmartDrawAnnoModule& aModule = CSmartDrawAnnoModule::GetInstance();

		const double dSize = pTagNozzle->nozzle()->nominal_pipe_dia();
		if(dSize > 0.0)
		{
			if(INCH == CNozzleChart::m_nSizeUnit)
			{
				const int nSiz = sizeof(table)/sizeof(CNozzleChart::METRIC_INCH_T);
				for(int i=0;i < nSiz;++i)
				{
					if((table[i].nStart - dSize)*(table[i].nEnd - dSize) <= 0.)
					{
						STRCPY_T(pSize , table[i].szInch);
						STRING_T sCSVStr = aModule.CSVString(pSize);
						STRCPY_T(pSize , sCSVStr.c_str());

						return;
					}
				}
				int nInch = 28 + int(SAFE_ROUND((dSize - 775)/50,1)*2);
				SPRINTF_T(pSize , _T("\"%d\"\"") , nInch);
			}
			else
			{
				SPRINTF_T(pSize,_T("%.0lf"),SAFE_ROUND(dSize,1));
			}
		}else	OutputDebugString( _T("Nozzle Size is less than zero.") );
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-27
    @class      CNozzleChart
    @function   WriteNozzle
    @return     void
    @param      OFSTREAM_T&         file
    @param      double              &x
    @param      double              &y
    @param      const               double
    @param      eqpx                const
    @param      double              eqpy
    @param      const               double
    @param      eqpz                const
    @param      int                 nType
    @param      CaDraw_TagNozzle*   pTagNozzle
    @brief
******************************************************************************/
void CNozzleChart::WriteNozzle(OFSTREAM_T& ofile , double &x , double &y, const double eqpx , const double eqpy , const double eqpz , const int nType , CaDraw_TagNozzle* pTagNozzle , const double& dDrawingScale)
{
	assert(pTagNozzle && "pTagNozzle is NULL");

	if(pTagNozzle)
	{
		static TCHAR szProjection[32]={'\0',};
		const STRING_T strOffset = this->GetUnknownItemString();
		static TCHAR szDir[32]={0,};
		static TCHAR szSize[32]={0,};
		TCHAR szBuf[MAX_PATH] = {'\0' ,};

		//////////////////////////////////////////////////////////////////////////
		/// initialize buffer
		memset(szProjection,'\0',sizeof(TCHAR)*32);
		memset(szDir,'\0',sizeof(TCHAR)*32);
		//////////////////////////////////////////////////////////////////////////
		
		ZeroMemory(szSize,sizeof(TCHAR)*32);
		this->GetSize(szSize , pTagNozzle);

		double dWidth = 0.f;
		for(int i = 0;i < 8;++i)
		{
			const int n = this->m_varLabel.nSequence[i] - 1;
			const double dLabelWidth = this->m_varLabel.dWidth[n]*dDrawingScale;

			if(0 == n)
			{
				/// NO.
				if(0. != dLabelWidth)
				{
					ofile << _T("<TEXT>|");
					ofile << (x + dWidth + dLabelWidth*0.5) << _T(",") << (y - m_dNozzRowHeight*0.5*dDrawingScale) << _T(",0,");
					ofile << m_oNozzleTextStyle.height()*dDrawingScale << _T(",") << m_oNozzleTextStyle.width()*dDrawingScale << _T(",0,");
					ofile << pTagNozzle->nozzle()->name() << _T(",Center Center,0|");
					ofile << m_oNozzLabelProp.level << _T(",") << m_oNozzLabelProp.color << _T(",");
					ofile << m_oNozzleTextStyle << std::endl;
				}
			}
			else if(1 == n)
			{	
				/// SIZE.
				if(0. != dLabelWidth)
				{
					ofile << _T("<TEXT>|");
					ofile << (x + dWidth + dLabelWidth*0.5) << _T(",") << (y - m_dNozzRowHeight*0.5*dDrawingScale) << _T(",0,");
					ofile << m_oNozzleTextStyle.height()*dDrawingScale << _T(",") << m_oNozzleTextStyle.width()*dDrawingScale << _T(",0,");
					ofile << szSize << _T(",Center Center,0|");
					ofile << m_oNozzLabelProp.level << _T(",") << m_oNozzLabelProp.color << _T(",");
					ofile << m_oNozzleTextStyle << std::endl;
				}
			}
			else if(2 == n)
			{
				/// RATING.
				if(0. != dLabelWidth)
				{
					ofile << _T("<TEXT>|");
					ofile << (x + dWidth + dLabelWidth*0.5) << _T(",") << (y - m_dNozzRowHeight*0.5*dDrawingScale) << _T(",0,");
					ofile << m_oNozzleTextStyle.height()*dDrawingScale << _T(",") << m_oNozzleTextStyle.width()*dDrawingScale << _T(",0,");
					ofile << pTagNozzle->nozzle()->rating() << _T(",Center Center,0|");
					ofile << m_oNozzLabelProp.level << _T(",") << m_oNozzLabelProp.color << _T(",");
					ofile << m_oNozzleTextStyle << std::endl;
				}
			}
			else if(3 == n)
			{
				/// ORIENTATION.
				if(0. != dLabelWidth)
				{
					ofile << _T("<TEXT>|");
					ofile << (x + dWidth + dLabelWidth*0.5) << _T(",") << (y - m_dNozzRowHeight*0.5*dDrawingScale) << _T(",0,");
					ofile << m_oNozzleTextStyle.height()*dDrawingScale << _T(",") << m_oNozzleTextStyle.width()*dDrawingScale << _T(",0,");
					ofile << pTagNozzle->nozzle()->orientation() << _T(",Center Center,0|");
					ofile << m_oNozzLabelProp.level << _T(",") << m_oNozzLabelProp.color << _T(",");
					ofile << m_oNozzleTextStyle << std::endl;
				}
			}
			else if(4 == n)
			{
				/// CODE.
				if(0. != dLabelWidth)
				{
					ofile << _T("<TEXT>|");
					ofile << (x + dWidth + dLabelWidth*0.5) << _T(",") << (y - m_dNozzRowHeight*0.5*dDrawingScale) << _T(",0,");
					ofile << m_oNozzleTextStyle.height()*dDrawingScale << _T(",") << m_oNozzleTextStyle.width()*dDrawingScale << _T(",0,");
					ofile << pTagNozzle->nozzle()->nozzle_code() << _T(",Center Center,0|");
					ofile << m_oNozzLabelProp.level << _T(",") << m_oNozzLabelProp.color << _T(",");
					ofile << m_oNozzleTextStyle << std::endl;
				}
			}
			else if(5 == n)
			{
				/// ELEV.
				if(0. != dLabelWidth)
				{
					ofile << _T("<TEXT>|");
					ofile << (x + dWidth + dLabelWidth*0.5) << _T(",") << (y - m_dNozzRowHeight*0.5*dDrawingScale) << _T(",0,");
					ofile << m_oNozzleTextStyle.height()*dDrawingScale << _T(",") << m_oNozzleTextStyle.width()*dDrawingScale << _T(",0,");
					SPRINTF_T(szBuf , _T("%.0lf") , pTagNozzle->nozzle()->origin().z());
					ofile << szBuf << _T(",Center Center,0|");
					ofile << m_oNozzLabelProp.level << _T(",") << m_oNozzLabelProp.color << _T(",");
					ofile << m_oNozzleTextStyle << std::endl;
				}
			}
			else if(6 == n)
			{
				/// OFFSET.
				if(0. != dLabelWidth)
				{
					ofile << _T("<TEXT>|");
					ofile << (x + dWidth + dLabelWidth*0.5) << _T(",") << (y - m_dNozzRowHeight*0.5*dDrawingScale) << _T(",0,");
					ofile << m_oNozzleTextStyle.height()*dDrawingScale << _T(",") << m_oNozzleTextStyle.width()*dDrawingScale << _T(",0,");
					ofile << strOffset.c_str() << _T(",Center Center,0|");
					ofile << m_oNozzLabelProp.level << _T(",") << m_oNozzLabelProp.color << _T(",");
					ofile << m_oNozzleTextStyle << std::endl;
				}
			}
			else if(7 == n)
			{
				/// PROJECTION.
				if(0. != dLabelWidth)
				{
					ofile << _T("<TEXT>|");
					ofile << (x + dWidth + dLabelWidth*0.5) << _T(",") << (y - m_dNozzRowHeight*0.5*dDrawingScale) << _T(",0,");
					ofile << m_oNozzleTextStyle.height()*dDrawingScale << _T(",") << m_oNozzleTextStyle.width()*dDrawingScale << _T(",0,");
					ofile << pTagNozzle->nozzle()->projection() << _T(",Center Center,0|");
					ofile << m_oNozzLabelProp.level << _T(",") << m_oNozzLabelProp.color << _T(",");
					ofile << m_oNozzleTextStyle << std::endl;
				}
			}
			
			dWidth += dLabelWidth;
		}
		y -= m_dNozzRowHeight*dDrawingScale;
	}
}

/**
 * GetUnknownItemString:
 *
 * @return const char* 
 */
const TCHAR* CNozzleChart::GetUnknownItemString()
{
	return m_szUnknownItem;
}

/**
	@brief	WriteLabel:
	@param file 
	@return void 
 */
void CNozzleChart::WriteLabel(OFSTREAM_T& ofile , double x , const double& dDrawingScale)
{
	m_dWidth = 0.f;
	
	if(TYPE_1 == m_nType)
	{
		for(int i = 0;i < m_varLabel.nColumn;++i)
		{
			const int nIndex = i;///m_varLabel.nSequence[i] - 1;
			if((nIndex >= 0) && (0. != m_varLabel.dWidth[nIndex]))
			{
				ofile << _T("<TEXT>|");
				ofile << (x + m_varLabel.dWidth[nIndex]*0.5)*dDrawingScale << _T(",") << (m_dY - m_dNozzRowHeight*0.5)*dDrawingScale << _T(",0,");
				ofile << m_oNozzleTextStyle.height()*dDrawingScale << _T(",") << m_oNozzleTextStyle.width()*dDrawingScale << _T(",0,");
				ofile << m_varLabel.szLabel[nIndex] << _T(",Center Center,0|");
				ofile << m_oNozzLabelProp.level << _T(",") << m_oNozzLabelProp.color << _T(",");
				ofile << m_oNozzleTextStyle << std::endl;

				x += m_varLabel.dWidth[nIndex];
				
				ofile << _T("<LINE>|");
				ofile << x*dDrawingScale << _T(",") << m_dY*dDrawingScale << _T(",0,") << x*dDrawingScale << _T(",") << (m_dY - m_dNozzRowHeight)*dDrawingScale << _T(",0|");
				ofile << m_oLeaderProp << std::endl;
				
				m_dWidth += m_varLabel.dWidth[nIndex];
			}
		}
	}
	else if(TYPE_2 == m_nType)
	{
		ofile << _T("<TEXT>|");
		ofile << (x + m_varLabel.dEqpWidth*0.5)*dDrawingScale << _T(",") << (m_dY - m_dNozzRowHeight*0.5)*dDrawingScale << _T(",0,");
		ofile << m_oNozzleTextStyle.height()*dDrawingScale << _T(",") << m_oNozzleTextStyle.width()*dDrawingScale << _T(",0,");
		ofile << m_varLabel.szEqpLabel << _T(",Center Center,0|");
		ofile << m_oNozzLabelProp.level << _T(",") << m_oNozzLabelProp.color << _T(",");
		ofile << m_oNozzleTextStyle << std::endl;
		
		x += m_varLabel.dEqpWidth;
		m_dWidth += m_varLabel.dEqpWidth;
		
		ofile << _T("<LINE>|");
		ofile << x*dDrawingScale << _T(",") << m_dY*dDrawingScale << _T(",0,") << x*dDrawingScale << _T(",") << (m_dY - m_dNozzRowHeight)*dDrawingScale << _T(",0|");
		ofile << m_oLeaderProp << std::endl;
		
		for(int i = 0;i < m_varLabel.nColumn;++i)
		{
			const int nIndex = i;///m_varLabel.nSequence[i] - 1;
			if((nIndex >= 0) && (0. != m_varLabel.dWidth[nIndex]))
			{
				ofile << _T("<TEXT>|");
				ofile << (x + m_varLabel.dWidth[nIndex]*0.5)*dDrawingScale << _T(",") << (m_dY - m_dNozzRowHeight*0.5)*dDrawingScale << _T(",0,");
				ofile << m_oNozzleTextStyle.height()*dDrawingScale << _T(",") << m_oNozzleTextStyle.width()*dDrawingScale << _T(",0,");
				ofile << m_varLabel.szLabel[nIndex] << _T(",Center Center,0|");
				ofile << m_oNozzLabelProp.level << _T(",") << m_oNozzLabelProp.color << _T(",");
				ofile << m_oNozzleTextStyle << std::endl;
								
				x += m_varLabel.dWidth[nIndex];

				ofile << _T("<LINE>|");
				ofile << x*dDrawingScale << _T(",") << m_dY*dDrawingScale << _T(",0,") << x*dDrawingScale << _T(",") << (m_dY - m_dNozzRowHeight)*dDrawingScale << _T(",0|");
				ofile << m_oLeaderProp << std::endl;
				
				m_dWidth += m_varLabel.dWidth[nIndex];
			}
		}
	}
}

/**
 * GetWidth:
 *
 * @return const double 
 */
const double CNozzleChart::GetWidth()
{
	return m_dWidth;
}

/**
 * GetRowHeight:
 *
 * @return const double 
 */
const double CNozzleChart::GetRowHeight()
{
	return m_dNozzRowHeight;
}

/**
 * GetEquiRowHeight:
 *
 * @return const double 
 */
const double CNozzleChart::GetEquiRowHeight()
{
	return m_dEqpRowHeight;
}	

/**
 * GetNozzRowHeight:
 *
 * @return const double 
 */
const double CNozzleChart::GetNozzRowHeight()
{
	return m_dNozzRowHeight;
}

/**
 * GetType:
 *
 * @return const int 
 */
const int CNozzleChart::GetType()
{
	return m_nType;
}

/**
	@brief	한 테이블에 쓰여질 ROW COUNT를 반환한다.

	@author	humkyung

	@return
*/
int CNozzleChart::GetRowCount() const
{
	return m_iRowCount;
}

/**
	@brief	startPos에서 Row Count 만큼 테이블로 그린다.

	@author	humkyung

	@param	x = 테이블의 좌상단의 x 위치
	@param	y = 테이블의 좌상단의 y 위치
	@param	startPos = Entry에서의 시작 Loc.

	@return
*/
int CNozzleChart::WriteTable(OFSTREAM_T& ofile , double &x, double &y, size_t &startPos,const double& dDrawingScale)
{
	ofile << _T("<LINE>|");
	ofile << x*dDrawingScale << _T(",") << y*dDrawingScale << _T(",0,") << (x + GetWidth())*dDrawingScale << _T(",") << y*dDrawingScale << _T(",0|");
	ofile << m_oLeaderProp << std::endl;
	
	this->WriteLabel(ofile , x , dDrawingScale);

	size_t nItemCount = 0;
	y -= m_dNozzRowHeight*dDrawingScale;
	for(int loc = startPos;(loc < int(m_pNozzleChartEntry->size()));++loc)
	{
		++startPos;
		CaDraw_TagEqui* pTagEqui = m_pNozzleChartEntry->at(loc);
		if(pTagEqui)
		{
			++nItemCount;
			list<CaDraw_TagNozzle*> oTagNozzleList;
			pTagEqui->GetTagNozzleList( oTagNozzleList );
			nItemCount += oTagNozzleList.size();
			if('\0' != pTagEqui->equi()->name()) this->WriteEqui(ofile , x , y , pTagEqui,dDrawingScale);
			if(int(nItemCount) > int(m_iRowCount)) break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ofile << _T("<LINE>|");
	ofile << x*dDrawingScale << _T(",") << m_dY*dDrawingScale << _T(",0,") << x*dDrawingScale << _T(",") << y*dDrawingScale << _T(",0|");
	ofile << m_oLeaderProp << std::endl;
		
	ofile << _T("<LINE>|");
	ofile << (x + GetWidth())*dDrawingScale << _T(",") << m_dY*dDrawingScale << _T(",0,") << (x + GetWidth())*dDrawingScale << _T(",") << y*dDrawingScale << _T(",0|");
	ofile << m_oLeaderProp << std::endl;
	
	ofile << _T("<LINE>|");
	ofile << x*dDrawingScale << _T(",") << y*dDrawingScale << _T(",0,") << (x + GetWidth())*dDrawingScale << _T(",") << y*dDrawingScale << _T(",0|");
	ofile << m_oLeaderProp << std::endl;

	return ERROR_SUCCESS;
}
