#pragma once

#include <util/ado/ADODB.h>

class CaDraw_NozzleAttribute
{
public:
	CaDraw_NozzleAttribute(void);
	~CaDraw_NozzleAttribute(void);

	int GetAttribute(CADODB& adoDB , const int& nPartitionNo , const int& nozzle_occur , const int& equi_occur);
public:
	double m_nominal_piping_dia;
	STRING_T  m_rating , m_nozzle_code;
	/// PDTABLE_23_X
	STRING_T m_nozzle_type;
	STRING_T m_orientation , m_projection;
private:
	STRING_T GetRatingString(const STRING_T& rRating, const STRING_T& rPreparation);
	STRING_T GetNozzleCode(const STRING_T& rTableSuffix);
};
