#pragma once
#include "SmartDrawAnnoTextStyle.h"
#include "smartdrawannoentity.h"

class ON_TAG_EXT_DLL CSmartDrawAnnoPlatform : public CaDraw_TagEntity
{
public:
	class CHatchingData
	{
	public:
		STRING_T type;	
		
		CIsPoint2d pt[9];
		CIsPoint2d insert_point;
		CIsPoint2d min;
		CIsPoint2d max;    
		
		///ÇÔ¼ö
		int FillCellMap();	/// 2014.04.16 added by humkyung
		int Write(OFSTREAM_T& ofile) const;
	};

	class ENV
	{
	public:
		int Parse(const STRING_T& sIniFilePath);

		STRING_T m_sCellLibraryFilePath , m_sCellName;
		CSmartDrawAnnoTextStyle m_oTextStyle;
		CompProp m_oTextProp;
		CompProp m_oHatchingProp;
		int m_iHatchingSize;	/// 2014.04.12 added by humkyung
		CompProp m_oLeaderProp;
		STRING_T m_sArrowType;
		double m_dArrowSize;
	};

	CSmartDrawAnnoPlatform(CaDraw_Entity* unnamed);
	~CSmartDrawAnnoPlatform(void);

	int Write(OFSTREAM_T& ofile,const double&);
	int Annotate();
	int Parse();
	
	bool PassedThrough(const CIsPoint3d& start , const CIsPoint3d& end) const;	/// 2012.03.09 added by humkyung
	bool IsInnerPoint(const CIsPoint2d& pt) const;

	double elevation() const;
	STRING_T GetElevationString() const;

	SMARTDRAW_TAG_DECLARE_FUNC()
protected:
	list<CCellMap::SearchRes>* m_pAnnoResultList;
	list<CSmartDrawAnnoPlatform::CHatchingData>* m_pHatchingDataList;
private:
	double getPolygonArea();	/// 2012.02.26 added by humkyung
	CHatchingData GenerateHatchingData(const CIsPoint2d& prev , const CIsPoint2d& pt , const CIsPoint2d& next , const double& dMinDist);

	double m_dElevation;
	vector<CIsPoint3d>* m_pPaveList;
};
