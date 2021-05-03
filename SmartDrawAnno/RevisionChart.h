#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <unordered_map>
#include <util/Markup/Markup.h>
#include "aDraw_EntTagShape.h"
#include "SmartDrawAnnoTextStyle.h"

#define	REV_DATA_BUF_SIZ	128

typedef struct
{
	TCHAR szText[16][REV_DATA_BUF_SIZ + 1];
}REVISION_DATA_T;

typedef struct
{
	UINT	uiDisplayType;	/// 1 = only data , 2 = data + label , 3 = data + label + leader
	UINT	nMinCount;
	double	nX,nY,nWidth;
	int	nRows,nColumns;
	double	nRowHeight,nColWidth[16];
	TCHAR	szTextStyle[16];
	TCHAR	szTextColor[8];
	double	nTextHeight;
	TCHAR	szLabel[16][16];
	TCHAR	szColumn[16][16];
	REVISION_DATA_T varData[16];
	TCHAR	szColor[8];

	int col(const STRING_T&);
}REVISION_CHART_T;

class ON_TAG_EXT_DLL CRevisionChart  
{
public:
	CRevisionChart();
	virtual ~CRevisionChart();
public:
	bool Load(LPCTSTR pIniFilePath , LPCTSTR pSdrFileName);
	bool Write(OFSTREAM_T& ofile,const double& dDrawingScale);
public:
	static REVISION_CHART_T m_varRevision;
private:
	bool LoadSetting(const STRING_T& sIniFilePath , const STRING_T& sCfgFilePath);

	CSmartDrawAnnoTextStyle m_oTextStyle;
	CompProp m_oLabelProp , m_oLeaderProp;
	static TCHAR m_szLocation[MAX_PATH];
public:
	static STRING_T COLUMNS[8];
};
