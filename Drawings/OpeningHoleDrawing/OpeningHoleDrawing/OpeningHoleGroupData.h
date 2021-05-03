#pragma once

#include <IsPolygon2d.h>
#include "OpeningHoleData.h"

class COpeningHoleGroupData
{
public:
	COpeningHoleGroupData(const int& iGroupNo);
	~COpeningHoleGroupData(void);

	int Annotate(CCellMap* pCellMap);
	void Write(OFSTREAM_T& ofile);
	int WriteChart(OFSTREAM_T& ofile , double& locx , double& locy);

	bool IsPossibleToBeMember(COpeningHoleData* pData);
	int Add(COpeningHoleData* pOpeningHoleData);
	int SortBySize();
	int groupNo() const;

	int GetOpeningHoleCount() const;
	COpeningHoleData* GetOpeningHoleDataAt(const size_t& at);

	static int m_iGroupCount;
private:
	int m_iGroupNo;					/// hole group no
	
	CIsVolume m_vol;				/// for "Box"
	CIsPolygon2d m_oGroupOutline;	/// for "Polyline"

	STRING_T m_sTag;
	CIsPoint2d m_origin , m_ptTagOrigin;
	CIsPoint2d m_ptLeader[3];

	vector<COpeningHoleData*> m_oOpeningHoleDataList;
};
