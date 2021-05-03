#pragma once

#include <tchar.h>
#include <IsTools.h>
#include "CellMap.h"

using namespace std;

#ifdef ADRAW_TAG_EXPORTS
	#define ON_TAG_EXT_DLL		__declspec(dllexport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern
#else
	#define ON_TAG_EXT_DLL		__declspec(dllimport)
	#define	ON_TAG_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_TAG_EXTERN_DECL	extern __declspec(dllimport)
#endif

class ON_TAG_EXT_DLL CompProp
{
public:
	CompProp();

	STRING_T level;
	STRING_T color;
	STRING_T style;
	STRING_T ltscale;
	STRING_T weight;

	friend OSTREAM_T& operator <<(OSTREAM_T& c , const CompProp& t);
};

class ON_TAG_EXT_DLL CompSize
{
public:
	CompSize(){}
	~CompSize(){}
	CompSize(const CompSize& rhs)
	{
		(*this) = rhs;
	}
	CompSize& operator=(const CompSize& rhs)
	{
		if(this != &rhs)
		{
			m_sSizeInInch = rhs.m_sSizeInInch;
			m_sSizeInMetric = rhs.m_sSizeInMetric;
		}

		return (*this);
	}

	int Parse(const STRING_T& str);
	bool EqualToInMetric(const double& value) const;
	bool GreaterThanInMetric(const double& value) const;
	bool LessThanInMetric(const double& value) const;
private:
	STRING_T m_sSizeInInch;
	STRING_T m_sSizeInMetric;
};


class ON_TAG_EXT_DLL CaDraw_EntTagShape
{
public:
	typedef enum
	{
		NONE	  = 0x00,
		CIRCLE    = 0x01,
		ELLIPSE   = 0x02,
		RECTANGLE = 0x03,
		DIAMOND   = 0x04,
		HEXAGON   = 0x05,
		OCTAGON   = 0x06,
	}EnclosureType;

	CaDraw_EntTagShape(void);
	CaDraw_EntTagShape(const CaDraw_EntTagShape& rhs)
	{
		(*this) = rhs;
	}
	~CaDraw_EntTagShape(void);

	CaDraw_EntTagShape& operator=(const CaDraw_EntTagShape& rhs)
	{
		if(this != &rhs)
		{
			m_pTagStringList->insert(m_pTagStringList->begin(), rhs.m_pTagStringList->begin() , rhs.m_pTagStringList->end());
			m_SearchRes  = rhs.m_SearchRes;
		}

		return (*this);
	}
public:
	int Write(OFSTREAM_T& ofile , const CaDraw_EntTagShape::EnclosureType& iEnclosureType , const CompProp& prop , const double& dDrawingScale);

	vector<STRING_T>* m_pTagStringList;
	CCellMap::SearchRes m_SearchRes;

	friend OSTREAM_T& operator <<(OSTREAM_T& c , const CIsPoint3d& t);
	friend OSTREAM_T& operator <<(OSTREAM_T& c , const CIsPoint2d& t);
};
