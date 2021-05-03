#pragma once
#include "adraw_entity.h"

class ON_MODULE_EXT_DLL CaDraw_Platform : public CaDraw_Entity
{
public:
	CaDraw_Platform(void);
	~CaDraw_Platform(void);

	int Dump(OFSTREAM_T& ofile);	/// 2012.02.15 added by humkyung

	void Copy(CaDraw_Platform* unnamed);

	bool IsEnable(AnnoFilters* ) const;

	int GetPointCount() const;	/// 2012.02.25 added by humkyung
	CIsPoint3d point(const int& at) const;
	int Add(const CIsPoint3d& pt);

	CIsVect3d dir() const;
	double height() const;
	int SetDir(const CIsVect3d& dir);
	int SetHeight(const double& height);

	ADRAW_DECLARE_FUNC()
private:
	vector<CIsPoint3d>* m_pPointList;
	CIsVect3d m_vecDir;
	double m_nHeight;
};
