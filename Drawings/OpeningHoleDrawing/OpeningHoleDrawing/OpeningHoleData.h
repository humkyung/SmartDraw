#pragma once

#include <IsTools.h>
#include <IsPoint2d.h>

#include <SmartDrawAnnoPipe.h>

class COpeningHoleGroupData;
class COpeningHoleData
{
	COpeningHoleData(const COpeningHoleData& rhs){};
public:
	COpeningHoleData(const CIsPoint2d& origin , CaDraw_TagPipe* pTagPipe);
	~COpeningHoleData(void);

	int Annotate(CCellMap* pCellMap);
	void Write(OFSTREAM_T& ofile);
	int WriteChart(OFSTREAM_T& ofile , double x , double y);

	CaDraw_TagPipe* tagPipe() const;
	CIsPoint2d origin() const;
	double radius() const;
	COpeningHoleGroupData* parent() const;
	int SetParent(COpeningHoleGroupData* pParent);
public:
	CIsPoint3d m_ptModelOrigin;
private:
	CIsPoint2d m_origin , m_ptTagOrigin;
	CIsPoint2d m_ptLeader[3];
	CaDraw_TagPipe* m_pTagPipe;

	int m_iHoleSize;
	double m_dCircleRadius;
	STRING_T m_sTag;

	COpeningHoleGroupData* m_pParent;
};
