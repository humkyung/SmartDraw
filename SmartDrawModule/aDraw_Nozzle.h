#pragma once
#include "adraw_entity.h"
//#include "aDraw_Equipment.h"

class ON_MODULE_EXT_DLL CaDraw_Nozzle : public CaDraw_Entity
{
public:
	CaDraw_Nozzle(void);
	~CaDraw_Nozzle(void);

	void Copy(CaDraw_Nozzle* unnamed);

	bool IsEnable(AnnoFilters* ) const;

	const TCHAR* projection() const;
	int SetProjection(const STRING_T& projection);
	const TCHAR* orientation() const;
	int SetOrientation(const STRING_T& orientation);
	const TCHAR* nozzle_type() const;
	int SetNozzleType(const STRING_T& nozzle_type);
	const TCHAR* nozzle_code() const;
	int SetNozzleCode(const STRING_T& nozzle_code);
	const TCHAR* rating() const;
	int SetRating(const STRING_T& rating);
	double nominal_pipe_dia() const;
	int SetNominalPipeDia(const double& rating);
	const TCHAR* equi_name() const;
	int SetEquiName(const STRING_T& rName);

	int Report(soci::session&,const STRING_T&) const;	/// 2016.02.24 added by humkyung

	ADRAW_DECLARE_FUNC()
private:
	STRING_T *m_pEquiName;
	double m_nNominalPipeDia;
	STRING_T *m_pRating , *m_pNozzleCode;
	STRING_T *m_pNozzleType;
	STRING_T *m_pOrientation , *m_pProjection;

	//CaDraw_Equipment* m_pEqui;
};
