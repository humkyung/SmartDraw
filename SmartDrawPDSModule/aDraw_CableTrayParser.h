#pragma once

#include <IsPoint3d.h>
#include <IsVect3d.h>
#include <DgnCellHeader.h>

#include <aDraw_CableTray.h>

class CaDraw_CableTrayParser
{
public:
	CaDraw_CableTrayParser(const double& uor_per_subunit);
	~CaDraw_CableTrayParser(void);
	int Parse(CDgnCellHeader* pDgnCell);

	CaDraw_CableTray::ELEMENT_E m_eType;
	CaDraw_CableTray::SUBTYPE_E m_eSubType;

	DPoint3d m_pt[3];
	double m_width[2];
	double m_height;
	CIsVect3d  m_lengthDir , m_widthDir , m_heightDir;
	double m_nAngle;
	
	vector<DPoint3d> m_ptEdgeList;
private:
	static double TOLER;
	double m_uor_per_subunit;

	bool IsStraight(PDGNATTRDATA  pUserData);
	bool IsElbowType(PDGNATTRDATA pUserData);	/// 2012.02.16 added by humkyung
	bool IsTeeType(PDGNATTRDATA pUserData);		/// 2012.02.16 added by humkyung
	bool IsReduceType(PDGNATTRDATA pUserData);	/// 2012.02.16 added by humkyung
	
	void GetDimension(CDgnCellHeader* pDgnCell);
	void GetDimensionForElbow(CDgnCellHeader* pDgnCell);
	void GetDimensionForReduce(CDgnCellHeader* pDgnCell);
	void GetDimensionForTee(CDgnCellHeader* pDgnCell);

	STRING_T GetSubType() const;
};
