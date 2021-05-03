#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartDrawAnnoEqui.h"
#include "SmartDrawAnnoTextStyle.h"

#include <list>
#include <vector>
using namespace std;

#define	METRIC	0
#define	INCH	1

#define TYPE_1	1
#define TYPE_2	2

typedef struct
{
	double nX,nY;
	double nStepX,nStepY;
	int nTypeX,nTypeY;
}GRID_NO_DATA_T;

#define	MAX_COLUMN_COUNT	16
class ON_TAG_EXT_DLL CNozzleChart
{
	typedef struct
	{
		int    nColumn;
		double dWidth[MAX_COLUMN_COUNT];
		TCHAR  szLabel[MAX_COLUMN_COUNT][16];
		int    nSequence[MAX_COLUMN_COUNT];
		
//////////////////////////////////////////////////////////////////////////
/// 2005.11.30
		TCHAR  szEqpLabel[16];
		double dEqpWidth;

		double GetTotalWidth() const
		{
			double res = 0.f;
			for(int i = 0;i < nColumn;++i)
			{
				res += dWidth[i];
			}

			return res;
		}
	}CHART_LABEL_T;

	typedef struct
	{
		double nStart,nEnd;
		TCHAR  szInch[8];
	}METRIC_INCH_T;
public:
	CNozzleChart();
	virtual ~CNozzleChart();
public:
	void LoadIni(LPCTSTR pszIniFilePath);
	void LoadBorderIni(LPCTSTR pFilePath);
	void Load(const char* pszFilePath);
	void Write(OFSTREAM_T& ofile,const double& dDrawingScale);
public:
	int GetRowCount() const;
	const int GetType();
	const double GetNozzTextHeight();
	const char* GetNozzTextColor();
	const double GetNozzRowHeight();
	const double GetEquiTextHeight();
	const char* GetEquiTextColor();
	const double GetEquiRowHeight();
	const double GetRowHeight();
	const double GetWidth();
	const TCHAR* GetUnknownItemString();
	static int m_nSizeUnit;
	
	vector<CaDraw_TagEqui*>* m_pNozzleChartEntry;
	bool m_bLoadBorderIni;
	bool m_bGridNo;
	int  m_nRows;
private:
	void GetSize(LPTSTR pSize , CaDraw_TagNozzle* pTagNozzle);
	void WriteEqui(OFSTREAM_T& file , double &x , double &y, CaDraw_TagEqui* pTagEqui,const double& dDrawingScale);
	void WriteNozzle(OFSTREAM_T& file , double &x , double &y, const double eqpx , const double eqpy , const double eqpz , const int nType , CaDraw_TagNozzle* pTagNozzle , const double& dDrawingScale);
	int WriteTable( OFSTREAM_T& file , double& x , double& y , size_t& startPos , const double& dDrawingScale);
	void WriteLabel(OFSTREAM_T& file , double x , const double& dDrawingScale);
	int m_nType;
	
	STRING_T m_sJustify;
	double m_dX,m_dY;	/// chart location
	double m_dWidth;	/// chart width
	double m_nHeight;
	TCHAR  m_szColor[8];
	int    m_iRowCount;	/// 한 테이블에 쓰여질 ROW COUNT
	bool   m_bSkipEqpHasNoNozzle;	/// 2014.12.31 added by humkyung

	double m_dEqpRowHeight;
	CSmartDrawAnnoTextStyle m_oEqpTextStyle;
	CompProp m_oEqpLabelProp;
	
	double m_dNozzRowHeight;
	CSmartDrawAnnoTextStyle m_oNozzleTextStyle;
	CompProp m_oNozzLabelProp;

	CompProp m_oLeaderProp;

	TCHAR m_szUnknownItem[16];

	CHART_LABEL_T m_varLabel;
	GRID_NO_DATA_T m_varGridNo;
		
	TCHAR m_szBuf[MAX_PATH + 1];
	void Init(void);
};
