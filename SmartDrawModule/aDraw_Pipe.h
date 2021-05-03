#pragma once
#include "adraw_entity.h"
#include <IsPoint3d.h>

#include <string>
#include <map>
using namespace std;

class ON_MODULE_EXT_DLL CaDraw_Pipe : public CaDraw_Entity
{
	typedef enum tagNames
	{
		LINE_LABEL = 0x00,
		LINE_ID = 0x01
	}NAMES_E;
public:
	enum
	{
		UNTAGGED	= 1,
		READY_TAGGING	= 2,
		ALREADY_TAGGED	= 3
	};

	CaDraw_Pipe(void);
	virtual ~CaDraw_Pipe(void);
	
	bool IsEnable(AnnoFilters* ) const;

	double  insThickInMetric() const;
	int SetInsThickInMetric(const double& insul);

	double  insThick() const;
	int SetInsThick(const double& insul);
	double GetRadius() const;
	double GetRadiusInMetric() const;	/// 2012.04.15 added by humkyung
	double GetSize() const;
	double GetSizeInMetric() const;		/// 2012.04.15 added by humkyung
	void SetSize(const double& nSize);
	double GetNominalPipeSize() const;
	double GetNominalPipeSizeInMetric() const;
	void SetNominalPipeSize(const double& dNominalPipeSize);
	void SetNominalPipeSizeInMetric(const double&);

	STRING_T line_label() const;
	STRING_T line_id() const;
	STRING_T GetSpec() const;
	void SetLineLabel(const STRING_T& line_label);
	void SetLineId(const STRING_T& line_id);
	void SetSpec(const STRING_T& rSpec);

	CIsPoint3d GetStartPoint();
	CIsPoint3d GetEndPoint();
	void SetStartPoint(const CIsPoint3d& ptStart);
	void SetEndPoint(const CIsPoint3d& ptEnd);
	void Copy(CaDraw_Pipe* unnamed);
	int Report(soci::session&,const STRING_T&) const;
	ADRAW_DECLARE_FUNC()
private:
	CIsPoint3d m_ptStart , m_ptEnd;				/// model point
	STRING_T m_rNames[2] , m_rSpec;				/// pipe line label , line id , spec
	double m_dNominalPipeSize , m_nSize;		/// nominal pipe size and size in inch
	double m_dNominalPipeSizeInMetric;
	double m_dInsThick , m_dInsThickInMetric;
	map<STRING_T,STRING_T>* m_pAttributeMap;
public:
	CIsPoint3d m_pt[2];	//! view point
	short m_nTagFlag;	/// the draft pipe is tagged?

	int SetAttribute(const STRING_T& rAttr , const STRING_T& rValue);
	STRING_T GetAttribute(const STRING_T& rAttr) const;
};
