#pragma once

#include <assert.h>
#include <IsTools.h>
#include <aDraw_EntTagShape.h>
#include <SmartDrawAnnoTextStyle.h>

#include <vector>
using namespace std;

class CLocation
{
public:
	CLocation(){ m_x = 0; m_y = 0;}
	CLocation(const STRING_T& rCorner , const double& x , const double& y)
	{ 
		m_rCorner = rCorner;
		m_x = x; m_y = y;
	}
	CLocation(const CLocation& rhs)
	{
		(*this) = rhs;
	}
	CLocation& operator=(const CLocation& rhs)
	{
		if( this != &rhs )
		{
			m_rCorner = rhs.m_rCorner;
			m_x       = rhs.m_x;
			m_y       = rhs.m_y;
		}

		return (*this);
	}

	virtual ~CLocation(){}

	int Set(const STRING_T& rCorner , const double& x , const double& y)
	{
		m_rCorner = rCorner;
		m_x = x;
		m_y = y;

		return ERROR_SUCCESS;
	}

	STRING_T corner() const
	{
		return m_rCorner;
	}

	double x() const
	{
		return m_x;
	}
	
	double y() const
	{
		return m_y;
	}
private:
	STRING_T m_rCorner;
	double m_x , m_y;
};

class CLabelWidthContainer
{
public:
	CLabelWidthContainer(){};
	CLabelWidthContainer(const CLabelWidthContainer& rhs)
	{
		(*this) = rhs;
	}
	CLabelWidthContainer& operator=(const CLabelWidthContainer& rhs)
	{
		if(this != &rhs)
		{
			m_oLabelWidthEntry.clear();
			m_oLabelWidthEntry.insert( m_oLabelWidthEntry.begin() , rhs.m_oLabelWidthEntry.begin() , rhs.m_oLabelWidthEntry.end());
		}

		return (*this);
	}
	virtual ~CLabelWidthContainer(){}

	int Parse(vector<STRING_T>& Tokens)
	{
		m_oLabelWidthEntry.clear();
		for(vector<STRING_T>::iterator itr = Tokens.begin();itr != Tokens.end();++itr)
		{
			m_oLabelWidthEntry.push_back(atof(itr->c_str()));
		}

		return ERROR_SUCCESS;
	}

	int GetWidthCount() const
	{
		return m_oLabelWidthEntry.size();
	}

	double GetTotalWidth() const
	{
		double res = 0.f;
		for(vector<double>::const_iterator itr = m_oLabelWidthEntry.begin();itr != m_oLabelWidthEntry.end();++itr)
		{
			res += (*itr);
		}

		return res;
	}

	double GetWidth( const size_t& at )
	{
		assert((at < m_oLabelWidthEntry.size()) && "range error");
		if(at < m_oLabelWidthEntry.size()) return m_oLabelWidthEntry[at];

		throw exception("range error");
	}
private:
	vector<double> m_oLabelWidthEntry;
};

class COpeningHoleDrawingOption
{
	COpeningHoleDrawingOption(void);
public:
	static COpeningHoleDrawingOption& GetInstance(void);
	~COpeningHoleDrawingOption(void);

	int Parse(const STRING_T& sIniFilePath);

	CLocation GetChartLocation();
	CLabelWidthContainer GetLabelWidthContainer(void) const;
	double GetClearanceOf(const double& dPipeSize) const;
	STRING_T GetGroupType() const;
	
	double GetChartRowHeight() const;
	CompProp GetChartLineProp() const;
	CompProp GetChartLabelProp() const;
	int GetLabelEntry(vector<STRING_T>& oLabelEntry);
	int GetChartTextColor(void) const;
public:
	CSmartDrawAnnoTextStyle m_oTextStyle , m_oChartTextStyle;
	
	CLocation m_oChartLocation;
	CLabelWidthContainer m_oLabelWidthContainer;
private:
	vector<STRING_T> m_oLabelEntry;

	double m_dChartRowHeight;
	CompProp m_oChartLineProp , m_oChartLabelProp;
	int m_iChartTextColor;
};
