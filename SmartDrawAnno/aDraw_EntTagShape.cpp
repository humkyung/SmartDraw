#include "StdAfx.h"
#include <fstream>
#include <Tokenizer.h>
#include <IsString.h>
#include "aDraw_EntTagShape.h"

using namespace std;

CompProp::CompProp()
{
	level = _T("1");
	color = _T("0");
	style = _T("0");
	ltscale = _T("0");
	weight= _T("0");
}

OSTREAM_T& operator <<(OSTREAM_T& c , const CompProp& t)
{ 
	c << t.level << _T(",") << t.color << _T(",") << t.style << _T(",") << t.ltscale << _T(",") << t.weight;
	return c; 
};

OSTREAM_T& operator <<(OSTREAM_T& c , const CIsPoint3d& t)
{
	c << t.x() << _T(",") << t.y() << _T(",") << t.z();
	return c;
}

OSTREAM_T& operator <<(OSTREAM_T& c , const CIsPoint2d& t)
{
	c << t.x() << _T(",") << t.y();
	return c;
}

/**
	@brief	parse given string
	@author	humkyung
	@date	2014.04.13
*/
int CompSize::Parse(const STRING_T& str)
{
	vector<STRING_T> oResult;
	CTokenizer<CIsFromString>::Tokenize(oResult , str , CIsFromString(_T("-")));
	if(2 == oResult.size())
	{
		m_sSizeInInch = oResult[0];
		IsString::TrimWhiteSpace(m_sSizeInInch);
		m_sSizeInMetric = oResult[1];
		IsString::TrimWhiteSpace(m_sSizeInMetric);

		return ERROR_SUCCESS;
	}

	return ERROR_INVALID_PARAMETER;
}

/**
	@brief	check size is equal to given value
	@author	humkyung
	@date	2014.04.13
*/
bool CompSize::EqualToInMetric(const double& value) const
{
	return (ATOF_T(m_sSizeInMetric.c_str()) == value);
}

/**
	@brief	check size is greater than given value
	@author	humkyung
	@date	2014.04.13
*/
bool CompSize::GreaterThanInMetric(const double& value) const
{
	return (ATOF_T(m_sSizeInMetric.c_str()) > value);
}

/**
	@brief	check size is less than given value
	@author	humkyung
	@date	2014.04.13
*/
bool CompSize::LessThanInMetric(const double& value) const
{
	return (ATOF_T(m_sSizeInMetric.c_str()) < value);
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-23
    @class      CaDraw_EntTagShape
    @function   CaDraw_EntTagShape
    @return
    @param      void
    @brief
******************************************************************************/
CaDraw_EntTagShape::CaDraw_EntTagShape(void)
{
	m_pTagStringList = new vector<STRING_T>;
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-23
    @class      CaDraw_EntTagShape
    @function   ~CaDraw_EntTagShape
    @return
    @param      void
    @brief
******************************************************************************/
CaDraw_EntTagShape::~CaDraw_EntTagShape(void)
{
	try
	{
		SAFE_DELETE( m_pTagStringList );
	}
	catch(...)
	{
	}
}

/******************************************************************************
    @author     humkyung
    @date       2012-03-24
    @function   Write
    @return     int
    @param      OFSTREAM_T&     ofile
    @param      const CaDraw_EntTagShape::EnclosureType& iEnclosureType
    @param      const CompProp& prop
    @brief
******************************************************************************/
int CaDraw_EntTagShape::Write(OFSTREAM_T& ofile , const CaDraw_EntTagShape::EnclosureType& iEnclosureType , const CompProp& prop , const double& dDrawingScale)
{
	CIsPoint2d center((this->m_SearchRes.coords[0] + this->m_SearchRes.coords[2]) * 0.5);
	const double dWidth = (this->m_SearchRes.coords[0].DistanceTo(this->m_SearchRes.coords[1]));
	const double dHeight= (this->m_SearchRes.coords[0].DistanceTo(this->m_SearchRes.coords[3]));
	if((CaDraw_EntTagShape::CIRCLE == iEnclosureType) || (CaDraw_EntTagShape::ELLIPSE == iEnclosureType))
	{
		ofile << _T("<ELLIPSE>|");
		ofile << center.x()*dDrawingScale << _T(",") << center.y()*dDrawingScale << _T(",");
		ofile << (dWidth*0.5)*dDrawingScale << _T(",") << (dHeight*0.5)*dDrawingScale << _T(",0|");
		ofile << prop << std::endl;
	}
	else if(CaDraw_EntTagShape::RECTANGLE == iEnclosureType)
	{
		CIsPoint2d at;
		ofile << _T("<PLINE>|");
		ofile << (this->m_SearchRes.coords[0]*dDrawingScale) << _T(",");
		ofile << (this->m_SearchRes.coords[1]*dDrawingScale) << _T(",");
		ofile << (this->m_SearchRes.coords[2]*dDrawingScale) << _T(",");
		ofile << (this->m_SearchRes.coords[3]*dDrawingScale) << _T(",");
		ofile << (this->m_SearchRes.coords[0]*dDrawingScale) << _T("|");
		ofile << prop << std::endl;
	}
	else if(CaDraw_EntTagShape::DIAMOND == iEnclosureType)
	{
		CIsPoint2d at;
		ofile << _T("<PLINE>|");
		at = (this->m_SearchRes.coords[0] + this->m_SearchRes.coords[1])*0.5;
		ofile << (at*dDrawingScale) << _T(",");
		at = (this->m_SearchRes.coords[1] + this->m_SearchRes.coords[2])*0.5;
		ofile << (at*dDrawingScale) << _T(",");
		at = (this->m_SearchRes.coords[2] + this->m_SearchRes.coords[3])*0.5;
		ofile << (at*dDrawingScale) << _T(",");
		at = (this->m_SearchRes.coords[3] + this->m_SearchRes.coords[0])*0.5;
		ofile << (at*dDrawingScale) << _T(",");
		at = (this->m_SearchRes.coords[0] + this->m_SearchRes.coords[1])*0.5;
		ofile << (at*dDrawingScale) << _T("|");
		ofile << prop << std::endl;
	}
	else if(CaDraw_EntTagShape::HEXAGON == iEnclosureType)
	{
		CIsPoint2d at;
		ofile << _T("<PLINE>|");
		at = (this->m_SearchRes.coords[0] + this->m_SearchRes.coords[1])*0.5;
		ofile << (at*dDrawingScale) << _T(",");
		at = this->m_SearchRes.coords[1] + (this->m_SearchRes.coords[2] - this->m_SearchRes.coords[1])*(1.0/4.0);
		ofile << (at*dDrawingScale) << _T(",");
		at = this->m_SearchRes.coords[1] + (this->m_SearchRes.coords[2] - this->m_SearchRes.coords[1])*(3.0/4.0);
		ofile << (at*dDrawingScale) << _T(",");
		at = (this->m_SearchRes.coords[2] + this->m_SearchRes.coords[3])*0.5;
		ofile << (at*dDrawingScale) << _T(",");
		at = this->m_SearchRes.coords[3] + (this->m_SearchRes.coords[0] - this->m_SearchRes.coords[3])*(1.0/4.0);
		ofile << (at*dDrawingScale) << _T(",");
		at = this->m_SearchRes.coords[3] + (this->m_SearchRes.coords[0] - this->m_SearchRes.coords[3])*(3.0/4.0);
		ofile << (at*dDrawingScale) << _T(",");
		at = (this->m_SearchRes.coords[0] + this->m_SearchRes.coords[1])*0.5;
		ofile << (at*dDrawingScale) << _T("|");
		ofile << prop << std::endl;
	}
	else if(CaDraw_EntTagShape::OCTAGON == iEnclosureType)
	{
		CIsPoint2d at;
		ofile << _T("<PLINE>|");
		at = this->m_SearchRes.coords[0] + (this->m_SearchRes.coords[1] - this->m_SearchRes.coords[0])*(1.0/3.0);
		ofile << (at*dDrawingScale) << _T(",");
		at = this->m_SearchRes.coords[0] + (this->m_SearchRes.coords[1] - this->m_SearchRes.coords[0])*(2.0/3.0);
		ofile << (at*dDrawingScale) << _T(",");
		at = this->m_SearchRes.coords[1] + (this->m_SearchRes.coords[2] - this->m_SearchRes.coords[1])*(1.0/4.0);
		ofile << (at*dDrawingScale) << _T(",");
		at = this->m_SearchRes.coords[1] + (this->m_SearchRes.coords[2] - this->m_SearchRes.coords[1])*(3.0/4.0);
		ofile << (at*dDrawingScale) << _T(",");
		at = this->m_SearchRes.coords[2] + (this->m_SearchRes.coords[3] - this->m_SearchRes.coords[2])*(1.0/3.0);
		ofile << (at*dDrawingScale) << _T(",");
		at = this->m_SearchRes.coords[2] + (this->m_SearchRes.coords[3] - this->m_SearchRes.coords[2])*(2.0/3.0);
		ofile << (at*dDrawingScale) << _T(",");
		at = this->m_SearchRes.coords[3] + (this->m_SearchRes.coords[0] - this->m_SearchRes.coords[3])*(1.0/4.0);
		ofile << (at*dDrawingScale) << _T(",");
		at = this->m_SearchRes.coords[3] + (this->m_SearchRes.coords[0] - this->m_SearchRes.coords[3])*(3.0/4.0);
		ofile << (at*dDrawingScale) << _T(",");
		at = this->m_SearchRes.coords[0] + (this->m_SearchRes.coords[1] - this->m_SearchRes.coords[0])*(1.0/3.0);
		ofile << (at*dDrawingScale) << _T("|");
		ofile << prop << std::endl;
	}

	return ERROR_SUCCESS;
}