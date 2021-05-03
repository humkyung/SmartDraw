#pragma once
#include "adraw_entity.h"

class ON_MODULE_EXT_DLL CaDraw_Reducer :
	public CaDraw_Entity
{
public:
	typedef enum tagReducerType
	{
		ECC = 0x00,
		CON = 0x01,
	}ReducerType;

	CaDraw_Reducer(void);
	~CaDraw_Reducer(void);

	void Copy(CaDraw_Reducer* unnamed);

	bool IsEnable(AnnoFilters* pAnnoFilters) const;

	ReducerType type() const;
	double size1() const;
	double GetSize1InMetric() const;		/// 2016.02.12 added by humkyung
	double size2() const;
	double GetSize2InMetric() const;		/// 2016.02.12 added by humkyung
	int SetReducerType(const ReducerType& type);
	int SetSize1(const double& size);
	int SetSize1InMetric(const double&);	/// 2016.02.12 added by humkyung
	int SetSize2(const double& size);
	int SetSize2InMetric(const double&);	/// 2016.02.12 added by humkyung
	ADRAW_DECLARE_FUNC()
private:
	ReducerType m_eType;
	double m_dSize[2];
	double m_dSizeInMetric[2];	/// size in metric - 2016.02.12 added by humkyung
};
