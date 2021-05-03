// CHSROption.h: interface for the COption class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COption_H__80EFE72E_F123_4792_9FE8_B58CCC17BAF9__INCLUDED_)
#define AFX_COption_H__80EFE72E_F123_4792_9FE8_B58CCC17BAF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <tchar.h>
#include <windows.h>
#include <Intent.h>

typedef enum
{
	SINGLE_LINE,
	DOUBLE_LINE,
	BOTH
}PIPELINEMODE;

class CComponentProp
{
public:
	CComponentProp()
	{
		m_sLevel = _T("0");
		m_sColor = _T("0");
		m_sStyle = _T("0");
		m_sScale = _T("0");	/// 2014.07.03 added by humkyung
		m_sWeight= _T("0");
		m_isOn = true;
	}
	~CComponentProp()
	{
	}

	CComponentProp& CComponentProp::operator =(const CComponentProp& rhs)
	{
		if(this != &rhs)
		{
			m_sLevel	= rhs.m_sLevel;
			m_sColor	= rhs.m_sColor;
			m_sStyle	= rhs.m_sStyle;
			m_sScale	= rhs.m_sScale;
			m_sWeight	= rhs.m_sWeight;
			m_isOn		= rhs.m_isOn;
		}

		return (*this);
	}

	int Parse(LPCTSTR pszLine);
	bool IsOn() const;
	STRING_T color() const;
	bool IsAutomaticColor() const;	/// 2015.03.22 added by humkyung
	friend OSTREAM_T& operator <<(OSTREAM_T& c , const CComponentProp& t);

	STRING_T m_sLevel;
	STRING_T m_sStyle;
	STRING_T m_sScale;	/// 2014.07.03 added by humkyung
	STRING_T m_sWeight;
private:
	STRING_T m_sColor;

	bool m_isOn;
	friend class CHSROption;
};

class CHSRParser;
class CHSRParser2xxx;
class CHSROption
{
	enum
	{
		Metric = 1,
		Inch_Feet = 2
	};

	typedef struct tagElbowOption
	{
		bool   Round;
		bool   CenterOn,CrossOn;
		double DistA,DistB;
		double Size;
		int    Angle;
		double MinSize;
		bool   TrimedMin;
		bool   TrimedMinNot;
	}ELBOWOPTION;
	
	typedef struct tagTeeOption
	{
		bool CenterOn;
		bool CrossOn;
		double DistA,DistB;
	}TEEOPTION;
public:
	CHSROption();
	virtual ~CHSROption();
public:
	bool Read(LPCTSTR pFilePath);

	const double WeldMinDotDist();
	double GetWeldElbowSize() const;
	double GetWeldElbowAngle() const;
	double GetWeldElbowDistA();
	double GetWeldElbowDistB();
	double GetWeldDotSize() const;
	bool IsWeldElbowCrosslineOn();
	bool IsWeldElbowCenterlineOn();
	double GetCriticalRadius();
	PIPELINEMODE GetPipeLineMode() const;

	PIPELINEMODE GetWeldTypeLineMode() const
	{
		return SINGLE_LINE;
	}

	PIPELINEMODE GetSWAndTHDTypeLineMode()
	{
		return SINGLE_LINE;
	}
public:
	bool IsViewLineDisplayOn() const;
	int Init();
	double GetWeldElbowMinSize() const;
	bool IsPipeCenterlineOn();
	double GetSWElbowMinSize();
	double GetSWElbowAngle() const;
	double GetSWElbowDistB();
	double GetSWElbowDistA();
	const double GetTeeDistA();
	bool IsTeeCenterlineOn();
	bool IsSWElbowCrosslineOn();
	bool IsSWElbowCenterlineOn();
	bool IsRoundTypeElbow();
	bool IsSquareTypeElbow();
	bool IsHiddenlineOn(const HSR_LAYER_E&) const;
	TCHAR m_szOutDWGFileName[MAX_PATH],m_szOutHideFileName[MAX_PATH];
	TCHAR m_szBorderFileName[MAX_PATH];

	/// added by humkyung
	CComponentProp m_oCompProp[24];
	CComponentProp m_oHiddenCompProp[24];
private:
	void ParseLine(LPCTSTR pszLine);
	LPCTSTR ConvertSize(LPCTSTR pSize);

	void ExtractViewBoundary(DList* pDList);
	int m_nUnit;
	bool   m_bUseSettingOn,m_bPipeCenterOn;
	double m_nCriticalPipeRadius;
	double m_nWeldMinDotDist;
	PIPELINEMODE m_nPipeLineMode;
	ELBOWOPTION  m_optWeldElbow,m_optSWandTHDElbow;
	TEEOPTION    m_optTee;

	friend CHSRParser;
	friend CHSRParser2xxx;
};

#endif // !defined(AFX_COption_H__80EFE72E_F123_4792_9FE8_B58CCC17BAF9__INCLUDED_)
