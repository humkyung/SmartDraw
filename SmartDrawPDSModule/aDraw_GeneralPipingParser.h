#pragma once

#include <DgnLineString.h>
#include <DgnCellHeader.h>

#include <util/ado/ADODB.h>
#include <aDraw_View.h>
#include <aDraw_Pipe.h>
#include "aDraw_PipeParser.h"

class CaDraw_GeneralPipingParser : public CaDraw_PipeParser
{
public:
	CaDraw_GeneralPipingParser(const CaDraw_Entity::MODEL_TYPE_T& eModelType , const STRING_T& rModelPath , const double& uor_per_subunit);
	~CaDraw_GeneralPipingParser(void);
	int Parse(CaDraw_View* pDraView , CDgnCellHeader* pDgnCell, CADODB& adoDB , const int& nPartitionNo , const CIsPoint3d& ptModelOffset);
private:
	double* GetReducerSize(CADODB& adoDB , const int& nPartitionNo , const long& occur) const;
public:
	long m_occur[3];
};
