#pragma once

#include <DgnLineString.h>
#include <util/ado/ADODB.h>
#include <aDraw_View.h>
#include <aDraw_Pipe.h>

class CaDraw_PipeParser
{
public:
	CaDraw_PipeParser(void);
	~CaDraw_PipeParser(void);
	int Parse(CaDraw_View* pDraView , CDgnLineString* pLineString, CADODB& adoDB , const int& nPartitionNo , const CIsPoint3d& ptModelOffset);
protected:
	STRING_T GetLineNumberLabel(CADODB& adoDB , const int& nPartitionNo , const long& occur);
private:
	CaDraw_Pipe* ParseAttribute(CADODB& adoDB , const int& nPartitionNo , const long& occur);
public:
	CaDraw_Entity::MODEL_TYPE_T m_eModelType;
	STRING_T m_rModelName;
	double m_uor_per_subunit;
};
